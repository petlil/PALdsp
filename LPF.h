/*
  ==============================================================================

    LPF.h
    Created: 7th Jun 2022
    Author:  Peter Liley
 
    Class for accessing various kinds of highpass filter

  ==============================================================================
*/

#include "Biquad.h"

#ifndef HPF_h
#define HPF_h
#define PI 3.14159265358979323846;
#include <math.h>

class HPF : public Biquad {
public:
    
    enum type {
        FIRSTORDER,
        BIQUAD
    };
    
    // constructor with default coefficients
    HPF(type filterType, float frequency) : Biquad (filterType, frequency) {
        float theta = 2 * freq * PI;
        float gamma = cos(theta) / (1 + sin(theta));
        a0 = (1 + gamma) / 2;
        a1 = -1 * ((1 + gamma) / 2);
        a2 = 0.0f;
        b1 = gamma * -1;
        b2 = 0.0f;
        c0 = 1.0f;
        d0 = 0.0f;
    }

    // constructor for custom coefficients
    HPF(type lfoType, float frequency, float a0, float a1, float a2, float b1, float b2, float c0, float d0)
         : Biquad (filterType, frequency, a0, a1, a2, b1, b2, c0, d0){}
    
    ~HPF(){};
    
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
    }

    int setType(type newType) {
        filterType = newType;
    }

private:
    float firstOrderDelayDry = 0;
    float firstOrderDelayWet = 0;
    float bandWidth = 0.5; // hard-set bandwidth for now
};


#endif /* HPF_h */