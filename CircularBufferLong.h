/*
  ==============================================================================

    CircularBuffer.h
    Created: 8 Apr 2022 1:34:54pm
    Author:  Peter Liley
 
    A circular buffer object based on Pirkle's wire-anded implementation.
    See Pirkle ch.14 for details.

  ==============================================================================
*/


#ifndef CircularBufferLong_h
#define CircularBufferLong_h

#include <math.h>
#include <vector>

class CircularBufferLong {
public:
    
    /**
     Create a new CircularBuffer data structure
     with a duration between 0 - 5 seconds
     */
    CircularBufferLong(float len){
        // check that dur is within bounds of buffer memory
        jassert(len * sampleRate <= buflen && len > 0);
        
        std::fill_n(buffer, buflen, 0); // reset the buffer to 0
        
        durationSecs = len;
        
        // set the read and write head indexes based on
        // specified length
        readHeadIndex = 0;
        writeHeadIndex = len * sampleRate;
    }
    
    /**
     Create a new CircularBuffer data structure
     with a duration between 0 - 5 seconds, and
     a given feedback amount (from 0 - 1).
     */
    CircularBufferLong(float len, float feedback){
        // check that dur is within bounds of buffer memory
        jassert(len * sampleRate <= buflen && len > 0);
        
        std::fill_n(buffer, buflen, 0); // reset the buffer to 0
        
        durationSecs = len;
        this->feedback = feedback;
        
        // set the read and write head indexes based on
        // specified length
        readHeadIndex = 0;
        writeHeadIndex = len * sampleRate;
    }
    
    /**
     Create a new CircularBuffer data structure
     with a duration between 0 - 5 seconds
     and specify a standard range of values for
     modulating the read head (in samples).
     
     NB: the range will be -modulationRange to modulationRange, being
     2x modulationRange in total range.
     */
    CircularBufferLong(float len, unsigned int modRange){
        // check that dur is within bounds of buffer memory
        jassert(len * sampleRate <= buflen && len > 0);
        // check that the modulation range is within the bounds of the buffer
        jassert(modRange < buflen / 2);

        std::fill_n(buffer, buflen, 0); // reset the buffer to 0
        
        durationSecs = len;
        
        // set the read and write head indexes based on
        // specified length
        readHeadIndex = 0;
        writeHeadIndex = len * sampleRate;
        // set the offset range
        this->modRange = modRange;
    }
    
    
    ~CircularBufferLong(){};
    
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
        //writeHeadIndex &= mask;
        buffer[writeHeadIndex] = sample + getFeedback();
        writeHeadIndex++;
        writeHeadIndex &= mask;
        //if (writeHeadIndex >= buflen) writeHeadIndex -= buflen;
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
        readHeadIndex &= mask; // wrap read head
        //if (readHeadIndex >= buflen) readHeadIndex -= buflen;
        return lerp(samp1, samp2, offFloat);
    }
    
    /**
     Returns a fed-back sample from the read head
     with any given effects applied.
     */
    float getFeedback() {
        float samp = buffer[readHeadIndex];
        // enact all feedback processing
        for(std::function<float(float)> function : feedbackFunctions) {
            samp = function(samp);
        }
        return samp * feedback;
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
    
    /**
     Sets the standard range of samples ahead of  / behind the
     read position in the buffer that the modulation offset may be.
     */
    inline void setModRange(float newModRange){
        // CHECKUP
        modRange = (newModRange > getLatency() || newModRange < 0) ? 0 : newModRange;
    }
    
    /**
     Sets the length of the delay line
     */
    inline void setReadHeadDelay(float dur) {
        jassert(dur * sampleRate < buflen);
        readHeadIndex = wrap(writeHeadIndex - (int) (dur * sampleRate));
        durationSecs = dur;
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
     Uses a given input between -1 and 1 to map the
     read head offset to somewhere in its set range.
     For use in automatic read head offsetting by an LFO.
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
     Set the current sample rate
     */
    inline void setSampleRate(int newRate) {
        sampleRate = newRate;
        
        // check that the new sample rate won't exceed the buffer
        if(durationSecs * sampleRate >= buflen) {
            durationSecs = 0;
        }
        
        // set the read and write head indexes based on
        // specified length
        readHeadIndex = 0;
        writeHeadIndex = durationSecs * sampleRate;
    }
    
private:
    unsigned const int buflen = 262144; // 2^18
    float buffer[262144];
    int mask = 262143; // for Pirkle's wire-and buffer wrapping (pg.395)
    int sampleRate = 44100;
    float durationSecs;
    int writeHeadIndex;
    int readHeadIndex;
    float feedback = 0;
    float readHeadModulation;
    int modRange = 0;
    int modCentreIndex = 0; // the 'centre' of the modulation (as an offset from the write head)
    
    // vector of (pointers to) functions that will be applied to feedback samples
    // (a fancy, and possibly unneccessary approach to feedback processing)
    std::vector<std::function<float(float)>> feedbackFunctions;
    
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
//        if (value >= buflen) return value - buflen;
//        else if (value < 0) return buflen + value;
//        else return value;
        
        return value &= mask;
    }
};


#endif /* CircularBufferLong_h */
