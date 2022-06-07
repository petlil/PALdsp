/*
  ==============================================================================

    CircularBuffer.h
    Created: 26 May 2022 1:32:50pm
    Author:  Peter Liley
 
    A circular buffer object based on Pirkle's wire-anded implementation.
    Buffer size is determined at initialisation.
    See Pirkle ch.14 for details.

  ==============================================================================
*/


#ifndef CircularBufferVariable_h
#define CircularBufferVariable_h

#include <math.h>

class CircularBufferVariable {
public:
    
    /* Default constructor */
    CircularBufferVariable(){
        
    }


    /** Creates a circular buffer with a length value in samples.
     *  Assumes sample rate 44.1k. Be sure to update this!
     */
    CircularBufferVariable(float* buffer, int buflen, int lengthSamples, float feedback, float modRange) {
        jassert(buflen == findLowestPowerOf2Above(lengthSamples));
        this->buffer = buffer;
        this->buflen = buflen;
        mask = buflen - 1;

        // set duration of buffer
        durationSamples = lengthSamples;
        // set read and write head positions
        readHeadIndex = 0;
        writeHeadIndex = lengthSamples;
        // set the offset range and feedback
        this->modRange = modRange;
        this->feedback = feedback;
    }
    
    
    ~CircularBufferVariable(){
    };
    
    
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
        buffer[writeHeadIndex] = sample + (buffer[readHeadIndex] * feedback);
        writeHeadIndex++;
        wrap(writeHeadIndex);
    }

    /**
     Returns the next sample and increments the buffer's read-index.
     */
    inline float getSample(){
        
        int offSamp0 = floor(readHeadModulation); // get the samples either side
        int offSamp1 = offSamp0 + 1;              // of the fractional value
        float offFloat = (float) readHeadModulation - (float) offSamp0; // get the fractional value
        float samp1 = buffer[wrap(readHeadIndex + offSamp0)];
        float samp2 = buffer[wrap(readHeadIndex + offSamp1)];
        
        readHeadIndex++;
        wrap(readHeadIndex);
        return lerp(samp1, samp2, offFloat);
    }
    
    /**
     Returns the distance between the front and
     back indexes of the buffer.
     */
    inline int getLatency() {
        if(writeHeadIndex > readHeadIndex) return writeHeadIndex - readHeadIndex;
        else return (writeHeadIndex + buflen) - readHeadIndex;
    }
    
    /**
     * Returns the maximum possible latency this buffer could
     * currently return, considering buffer write/read head indexes
     * and modulation.
     */
    inline float getMaxLatency() {
        if(writeHeadIndex > readHeadIndex) return writeHeadIndex - readHeadIndex + modRange;
        else return (writeHeadIndex + buflen) - readHeadIndex + modRange;
    }
    
    inline int getLength() {
        return durationSamples;
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
    inline void setLengthSamples(int lengthSamples) {
        jassert(lengthSamples < buflen);
        readHeadIndex = wrap(writeHeadIndex - lengthSamples);
        durationSamples = lengthSamples;
    }
    
    /**
     Sets the number of samples the read head is offset from its
     default delay length. Used to modulate the read head
     for chorusing effects, etc.
     Values can be negative.
     */
    inline void setReadHeadModulation(float offset){
        jassert(offset <= modRange && offset >= (-1 * modRange));
        readHeadModulation = offset;
    }
    
    /**
     Uses a given input between -1 and 1 to map the
     read head offset to somewhere in its set range.
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
        // check that the new sample rate won't exceed the buffer
        jassert(durationSamples < buflen);
        sampleRate = newRate;
        // set the read head based on the new sample rate
        readHeadIndex = wrap(writeHeadIndex - durationSamples);
    }
    
    /**
     Finds and returns the lowest power of 2 above a given integer.
     */
    static int findLowestPowerOf2Above(int val) {
        int powerOf2 = 2;
        while(powerOf2 < val) powerOf2 *= 2;
        return powerOf2;
    }
    
private:
    float *buffer;
    int buflen;
    int mask; // for Pirkle's wire-and buffer wrapping (pg.395)
    int sampleRate = 44100;
    int durationSamples;
    int writeHeadIndex;
    int readHeadIndex;
    float feedback = 0;
    float readHeadModulation;
    int modRange = 0;
    
    /**
     Simple linear interpolation
     */
    float lerp(float a, float b, float t)
    {
        return a + t * (b - a);
    }
    
    /**
     Takes an array index value and returns a value 'wrapped' around the
     start or end of the buffer if the index is >= buflen or < 0.
     Returns the original value if not applicable.
     */
    int wrap(int value) {
        return value &= mask;
    }
    
    
};


#endif /* CircularBufferVariable_h */
