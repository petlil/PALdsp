/*
  ==============================================================================

    CircularBuffer.h
    Created: 7 Jun 2022 12:52:54pm
    Author:  Peter Liley
 
    A circular buffer object based on Pirkle's wire-anded implementation.
    See Pirkle ch.14 for details.

  ==============================================================================
*/


#ifndef CircularBufferShort_h
#define CircularBufferShort_h

#include <math.h>
#include <vector>

class CircularBufferShort {
public:
    
    CircularBufferShort(){}
    
    /**
     Make a new circular buffer with length up to 1 second.
     Values for feedback and modulation range are optional.
     */
    CircularBufferShort(float lenSeconds, float feedback = 0, unsigned int modRange = 0) {
        // check that dur is within bounds of buffer memory
        jassert(lenSeconds * sampleRate <= buflen && lenSeconds > 0);
        // check feedback is within range
        jassert(feedback <= 1 && feedback >= 0);
        std::fill_n(buffer, buflen, 0); // reset the buffer to 0
        durationSecs = lenSeconds;
        durationSamples = lenSeconds * sampleRate;
        // set the read and write head indexes based on specified length
        readHeadIndex = 0;
        writeHeadIndex = durationSamples;
        this->feedback = feedback;
        this->modRange = modRange;
    }
    
    
    CircularBufferShort(int lenSamples, float feedback = 0, unsigned int modRange = 0) {
        // check that dur is within bounds of buffer memory
        jassert(lenSamples <= buflen && lenSamples > 0);
        // check feedback is within range
        jassert(feedback <= 1 && feedback >= 0);
        std::fill_n(buffer, buflen, 0); // reset the buffer to 0
        durationSecs = lenSamples / sampleRate;
        durationSamples = lenSamples;
        // set the read and write head indexes based on specified length
        readHeadIndex = 0;
        writeHeadIndex = lenSamples;
        this->feedback = feedback;
        this->modRange = modRange;
    }
    
    ~CircularBufferShort(){};
    
    /**
     Sets all buffer values to 0
     */
    inline void clear() {
        std::fill_n(buffer, buflen, 0);
    }
    
    /**
     Increments the buffer's write-index and inserts
     the given value.
     */
    inline void pushSample(float sample){
        buffer[writeHeadIndex] = sample + getFeedbackSample();
        writeHeadIndex++;
        writeHeadIndex = wrap(writeHeadIndex);
    }

    /**
     Returns the next sample and increments the buffer's read-index.
     */
    inline float getSample(){
        int offSamp0 = floor(readHeadModulation); // get the samples either side
        int offSamp1 = offSamp0 + 1;          // of the fractional value
        float offFloat = (float) readHeadModulation - (float) offSamp0; // get the fractional value
        float samp1 = buffer[wrap(readHeadIndex + offSamp0)];
        float samp2 = buffer[wrap(readHeadIndex + offSamp1)];
        
        readHeadIndex++;
        readHeadIndex = wrap(readHeadIndex); // wrap read head
        return lerp(samp1, samp2, offFloat);
    }
    
    /**
     Get a custom index from the buffer.
     index 0 means no delay
     index 100 = 100 samples of delay, etc.
     */
    inline float tap(int index) {
        jassert(index < getLatency());
        return buffer[wrap(writeHeadIndex - index)];
    }
    
    /**
     Adds a function that will be applied to each sample that passes through the 'feedback' loop.
     */
    void addFeedbackProcessor(std::function<float(float)> function) {
        feedbackFunctions.push_back(function);
    }
    
    int getNumFeedbackProcessors() {
        return (int) feedbackFunctions.size();
    }
    
    /**
     Sets the standard range of samples ahead of  / behind the
     read position in the buffer that the modulation offset may be.
     */
    inline void setModRange(float newModRange){
        // CHECKUP
        modRange = (newModRange > getLatency() || newModRange < 0) ? 0 : newModRange;
    }
    
    /**
     Sets the length of the delay line in seconds
     */
    inline void setLengthSeconds(float dur) {
        jassert(dur * sampleRate < buflen);
        durationSecs = dur;
        durationSamples = dur * sampleRate;
        readHeadIndex = wrap(writeHeadIndex - durationSamples);
    }
    
    /**
     Sets the length of the delay line in samples.
     */
    inline void setLengthSamples(int length) {
        jassert(length < buflen);
        readHeadIndex = wrap(writeHeadIndex - length);
        durationSecs = length * sampleRate;
        durationSamples = length;
    }
    
    /**
     Sets the number of samples the read head is offset from its
     default delay length. Used to modulate the read head
     for chorusing effects, etc.
     Values can be negative.
     */
    inline void setReadHeadModulation(float offset){
        readHeadModulation = offset;
    }
    
    /**
     Uses a given input between to map the
     read head offset to somewhere in its set range.
     For use in automatic read head offsetting by an LFO.
     e.g. if modulation range is 20 samples, an offset of
     0.5 will offset the read head by +10 samples.
     Map range is set to -1 to 1.
     */
    inline void mapReadHeadMod(float lfoOffset){
        jassert(lfoOffset >=-1.0f && lfoOffset <= 1.0f);
        readHeadModulation = modRange * lfoOffset;
    }
    
    /**
     Set the amount of feedback in the buffer.
     (value from 0 - 1)
     */
    inline void setFeedback(float newValue) {
        jassert(newValue <= 1 && newValue >= 0);
        feedback = newValue;
    }
    
    /**
     Set the current sample rate.
     (Note, this will change the duration of the buffer)
     */
    inline void setSampleRate(int newRate) {
        sampleRate = newRate;
        durationSecs = durationSamples / sampleRate;
        
        // check that the new sample rate won't exceed the buffer
        jassert(durationSamples < newRate);
        
        // set the read and write head indexes based on
        // specified length
        readHeadIndex = 0;
        writeHeadIndex = durationSamples;
    }
    
    /**
     Returns the distance between the front and
     back indexes of the buffer
     */
    inline int getLatency() {
        if(writeHeadIndex > readHeadIndex) return writeHeadIndex - readHeadIndex;
        else return (writeHeadIndex + buflen) - readHeadIndex;
    }
    
    inline float getMaxLatency() {
        if(writeHeadIndex > readHeadIndex) return writeHeadIndex - readHeadIndex + modRange;
        else return (writeHeadIndex + buflen) - readHeadIndex + modRange;
    }
    
private:
    unsigned const int buflen = 65536; // 2^16
    float buffer[65536];
    int mask = 65535; // for Pirkle's wire-and buffer wrapping (pg.395)
    int sampleRate = 44100;
    float durationSecs;
    float durationSamples;
    int writeHeadIndex;
    int readHeadIndex;
    float feedback = 0;
    float readHeadModulation;
    int modRange = 0;
    
    // vector of (pointers to) functions that will be applied to feedback samples
    // (a fancy, and possibly unneccessary approach to feedback processing)
    std::vector<std::function<float(float)>> feedbackFunctions;
    
    /**
     Returns a fed-back sample from the read head
     with any given effects applied.
     */
    float getFeedbackSample() {
        float samp = buffer[readHeadIndex];
        // enact all feedback processing
        for(std::function<float(float)> function : feedbackFunctions) {
            samp = function(samp);
        }
        return samp * feedback;
    }
    
    /**
     Simple linear interpolation
     */
    float lerp(float a, float b, float t)
    {
        return a + t * (b - a);
    }
    
    /**
     Takes a sample index value and returns an appropriately
     wrapped value if necessary, or the original value if not
     */
    int wrap(int value) {
        return value &= mask;
    }
    
    
};

#endif /* CircularBufferShort_h */
