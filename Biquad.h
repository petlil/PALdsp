/*
  ==============================================================================

    filter.h
    Created: 7th Jun 2022
    Author:  Peter Liley
 
    Virtual superclass for all biquad types.
    Equations from: Audio EQ Cookbook
    https://webaudio.github.io/Audio-EQ-Cookbook/audio-eq-cookbook.html

  ==============================================================================
*/

#include "filter.h"

#ifndef Biquad_h
#define Biquad_h
#include <math.h>

class Biquad : public filter {
public:
    
    Biquad(int lfoType, float frequency, float wet = 1, float dry = 0) : filter(lfoType, frequency, wet, dry){}

    virtual ~Biquad() {}
    
    inline float processSample(float samp){
        
        // calculate result
        double result = ((b0 / a0) * samp) +
                        ((b1 / a0) * a1Delay) +
                        ((b2 / a0) * a2Delay) -
                        ((a1 / a0) * b1Delay) -
                        ((a2 / a0) * b2Delay);

        // update delay register
        a2Delay = a1Delay;
        a1Delay = samp;
        b2Delay = b1Delay;
        b1Delay = result;

        return (float) ((result * wet) + (samp * dry));
    }

    virtual void setType(int newType) = 0;

    void reset() {
        a1Delay = 0;
        a2Delay = 0;
        b1Delay = 0;
        b2Delay = 0;
    }
    
protected:
    // coefficients
    double a0 = 0, a1 = 0, a2 = 0, b0 = 0, b1 = 0, b2 = 0;
    // delay registers
    double a1Delay = 0;
    double a2Delay = 0;
    double b1Delay = 0;
    double b2Delay = 0;
};


#endif /* Biquad_h */
