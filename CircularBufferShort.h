/*
  ==============================================================================

    CircularBuffer.h
    Created: 8 Apr 2022 1:34:54pm
    Author:  Peter Liley
 
    A circular buffer object based on Pirkle's wire-anded implementation.
    See Pirkle ch.14 for details.

  ==============================================================================
*/


#ifndef CircularBuffer_h
#define CircularBuffer_h

#include <math.h>

class CircularBuffer {
public:
    
    /**
     Create a new CircularBuffer data structure
     with a duration between 0 - 44100 (in samples)
     */
    CircularBuffer(int len){
        // check that dur is within bounds of buffer memory
        jassert(len <= buflen && len > 0);
        std::fill_n(buffer, buflen, 0); // reset the buffer to 0
        
        // set the read and write head indexes based on
        // specified length
        readHeadIndex = 0;
        writeHeadIndex = len;
    }
    
    /**
     Create a new CircularBuffer data structure
     with a duration between 0 - 44100 (in samples)
     and specify a standard range of values for
     modulating the read head (in samples).
     
     NB: the range will be -modulationRange to modulationRange, being
     2 * modulationRange in total range.
     */
    CircularBuffer(unsigned int len, unsigned int modRange){
        // check that dur is within bounds of buffer memory
        jassert(len <= buflen && len > 0);
        // check that the modulation range is within the bounds of the buffer
        jassert(modRange < buflen / 2);
        // check that the modulaition won't overflow either end of the buffer
        jassert(len + modRange < buflen && len - modRange >= 0);
        
        std::fill_n(buffer, buflen, 0); // reset the buffer to 0
        
        // set the read and write head indexes based on
        // specified length
        readHeadIndex = 0;
        writeHeadIndex = len;
        // set the offset range
        this->modRange = modRange;
    }
    
    
    ~CircularBuffer(){};
    
    /**
     Increments the buffer's write-index and inserts
     the given value.
     */
    inline void pushSample(float sample){
        //writeHeadIndex &= mask;
        buffer[writeHeadIndex] = sample;
        writeHeadIndex++;
        if (writeHeadIndex >= buflen) writeHeadIndex -= buflen;
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
        //readHeadIndex &= mask; // wrap read head
        if (readHeadIndex >= buflen) readHeadIndex -= buflen;
        return lerp(samp1, samp2, offFloat);
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
    inline void setModRange(float offsetRangeIn){
        // CHECKUP
        modRange = (offsetRangeIn > getLatency() || offsetRangeIn < 0) ? 0 : offsetRangeIn;
    }
    
    /**
     Sets the length of the delay line (in samples)
     */
    inline void setReadHeadDelay(int delay) {
        readHeadIndex = wrap(writeHeadIndex - delay);
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
        //jassert(lfoOffset >=-1.0f && lfoOffset <= 1.0f);
        if(lfoOffset < -1) lfoOffset = -1;
        if(lfoOffset > 1) lfoOffset = 1;
        readHeadModulation = modRange * lfoOffset;
    }
    
private:
    unsigned const int buflen = 65536; // 2^16 (the smallest power of 2 above 44.1k)
    float buffer[65536];
    int mask = 65535; // for Pirkle's wire-and buffer wrapping (pg.395)
    int writeHeadIndex;
    int readHeadIndex;
    float readHeadModulation;
    int modRange = 0;
    int modCentreIndex = 0; // the 'centre' of the modulation (as an offset from the write head)
    
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


#endif /* CircularBuffer_h */

65536
