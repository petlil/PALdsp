/*
  ==============================================================================

    HighShelfFilter.h
    Created: 10th Jun 2022
    Author:  Peter Liley
 
    Class for accessing various kinds of high shelf filter.
    Equations from: Audio EQ Cookbook
    https://webaudio.github.io/Audio-EQ-Cookbook/audio-eq-cookbook.html

  ==============================================================================
*/

#include "Biquad.h"

#ifndef HighShelfFilter_h
#define HighShelfFilter_h

class HighShelfFilter : public Biquad {
public:
    
    enum type {
        BIQUAD
    };
    
    HighShelfFilter(type filterType, float frequency, float Q, float dbGain, float wet = 1, float dry = 0) : Biquad (filterType, frequency, wet, dry) {

        // audio-eq-cookbook
        double w0 = 2 * (frequency/44100) * PI;
        double cosW0 = cos(w0);
        double sinW0 = sin(w0);
        double alpha = sinW0 / (Q * 2);
        double A = pow(10.0, dbGain/40);

        // calculate coefficients
        b0 = A * ((A + 1) + ((A - 1) * cosW0) + (2 * sqrt(A) * alpha));
        b1 = -2 * A * ((A - 1) + ((A + 1) * cosW0));
        b2 = A * ((A + 1) + ((A - 1) * cosW0) - (2 * sqrt(A) * alpha));
        a0 = (A + 1) - ((A - 1) * cosW0) + (2 * sqrt(A) * alpha);
        a1 = 2 * ((A - 1) - ((A + 1) * cosW0));
        a2 = (A + 1) - ((A - 1) * cosW0) - (2 * sqrt(A) * alpha);
    }
    
    inline float processSample(float samp){

        if(filterType == BIQUAD) {
            return Biquad::processSample(samp);
        }
        else return 0.0f;
    }

    void setType(int newType) {
        filterType = newType;
    }

private:
};


#endif /* HighShelfFilter_h */
