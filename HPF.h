/*
  ==============================================================================

    HPF.h
    Created: 7th Jun 2022
    Author:  Peter Liley
 
    Class for accessing various kinds of highpass filter
 
    Equations from: Audio EQ Cookbook
    https://webaudio.github.io/Audio-EQ-Cookbook/audio-eq-cookbook.html

  ==============================================================================
*/

#include "Biquad.h"
#ifndef HPF_h
#define HPF_h

class HPF : public Biquad {
public:
    
    enum type {
        FIRSTORDER,
        BIQUAD
    };
    
    // constructor with default coefficients
    HPF(type filterType, float frequency, float Q, float wet = 1, float dry = 0) : Biquad (filterType, frequency, wet, dry) {
        
        // audio-eq-cookbook
        double w0 = 2 * (frequency/44100) * PI;
        double cosW0 = cos(w0);
        double sinW0 = sin(w0);
        double alpha = sinW0 / (Q * 2);
        
        b0 = (1 + cosW0) / 2;
        b1 = -1 * (1 + cosW0);
        b2 = (1 + cosW0) / 2;
        a0 = 1 + alpha;
        a1 = -2 * cosW0;
        a2 = 1 - alpha;
    }
    
    inline float processSample(float samp){
        
        if(filterType == FIRSTORDER) {
            float result = ((1 + bandWidth) / 2) * (samp - firstOrderDelayDry) + firstOrderDelayWet;
            firstOrderDelayDry = samp;
            firstOrderDelayWet = result;
            return result;
        }
        else if(filterType == BIQUAD) {
            return Biquad::processSample(samp);
        }
        else return 0.0f;
    }

    void setType(int newType) {
        filterType = newType;
    }

private:
    double firstOrderDelayDry = 0;
    double firstOrderDelayWet = 0;
    double bandWidth = 1;
};


#endif /* HPF_h */
