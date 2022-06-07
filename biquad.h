/*
  ==============================================================================

    filter.h
    Created: 7th Jun 2022
    Author:  Peter Liley
 
    Virtual superclass for all biquad types.

  ==============================================================================
*/

#include "filter.h"
#include 

#ifndef Biquad_h
#define Biquad_h

class Biquad : public filter {
public:
    
    Biquad(int lfoType) : filter(lfoType){}
    
    ~Biquad(){};
    
    inline float processSample(float samp){
        // calculate result
        float result = (a0 * samp) +
                       (a1 * a1Delay) +
                       (a2 * a2Delay) -
                       (b1 * b1Delay) -
                       (b2 * b2Delay);

        // update delay register
        a2Delay = a1Delay;
        a1Delay = samp;
        b2Delay = b1Delay;
        b1Delay = result;

        return result;
    }

    virtual int setType() = 0;

    void reset() {
        a1Delay = 0;
        a2Delay = 0;
        b1Delay = 0;
        b2Delay = 0;
    }
    
private:
    // coefficients
    float a0, a1, a2, b1, b2, d0, c0;
    // delay registers
    float a1Delay = 0;
    float a2Delay = 0;
    float b1Delay = 0;
    float b2Delay = 0;
};


#endif /* Biquad_h */
