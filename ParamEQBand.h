/*
  ==============================================================================

    ParamEQBand.h
    Created: 10th Jun 2022
    Author:  Peter Liley
 
    Class for accessing various kinds of parametric band (aka peaking eq) filter
 
    Equations from: Audio EQ Cookbook
    https://webaudio.github.io/Audio-EQ-Cookbook/audio-eq-cookbook.html

  ==============================================================================
*/

#include "Biquad.h"

#ifndef ParamEQBand_h
#define ParamEQBand_h

class ParamEQBand : public Biquad {
public:
    
    enum type {
        BIQUAD
    };
    
    // constructor with default coefficients
    ParamEQBand(type filterType, float frequency, float Q, float dbGain, float wet = 1, float dry = 0) : Biquad (filterType, frequency, wet, dry) {

        double w0 = 2 * (frequency/44100) * PI;
        double cosW0 = cos(w0);
        double sinW0 = sin(w0);
        double alpha = sinW0 / (Q * 2);
        double A = pow(10.0, dbGain/40);

        // calculate coefficients
        b0 = 1 + (alpha * A)
        b1 = -2 * cosW0;
        b2 = 1 - (alpha * A);
        a0 = 1 + (alpha / A);
        a1 = -2 * cosW0;
        a2 = 1 - (alpha / A);
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


#endif /* ParamEQBand_h */
