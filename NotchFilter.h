/*
  ==============================================================================

    NotchFilter.h
    Created: 10th Jun 2022
    Author:  Peter Liley
 
    Class for accessing various kinds of notch filter.
 
    Equations from: Audio EQ Cookbook
    https://webaudio.github.io/Audio-EQ-Cookbook/audio-eq-cookbook.html

  ==============================================================================
*/

#include "Biquad.h"
#ifndef NotchFilter_h
#define NotchFilter_h

class NotchFilter : public Biquad {
public:
    
    enum type {
        BIQUAD
    };
    
    // constructor with default coefficients
    NotchFilter(type filterType, float frequency, float Q, float wet = 1, float dry = 0) : Biquad (filterType, frequency, wet, dry) {

        double w0 = 2 * (frequency/44100) * PI;
        double cosW0 = cos(w0);
        double sinW0 = sin(w0);
        double alpha = sinW0 / (Q * 2);

        b0 = 1;
        b1 = -2 * cosW0;
        b2 = 1
        a0 = 1 + alpha;
        a1 = -2 * cosW0;
        a2 = 1 - alpha;
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


#endif /* NotchFilter_h */
