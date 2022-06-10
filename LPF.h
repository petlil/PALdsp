/*
  ==============================================================================

    LPF.h
    Created: 7th Jun 2022
    Author:  Peter Liley
 
    Class for accessing various kinds of lowpass filter

  ==============================================================================
*/

#include "Biquad.h"

#ifndef LPF_h
#define LPF_h

class LPF : public Biquad {
public:
    
    enum type {
        FIRSTORDER,
        BIQUAD
    };
    
    // constructor with default coefficients
    LPF(type filterType, float frequency, float Q) : Biquad (filterType, frequency, Q) {

        // audio-eq-cookbook
        double w0 = 2 * (frequency/44100) * PI;
        double cosW0 = cos(w0);
        double sinW0 = sin(w0);
        double alpha = sinW0 / (Q * 2);

        b0 = (1 - cosW0) / 2;
        b1 = 1 - cosW0;
        b2 = (1 - cosW0) / 2;
        a0 = 1 + alpha;
        a1 = -2 * cosW0;
        a2 = 1 - alpha;
        wet = 1.0; // fully wet
        dry = 0.0; // no dry signal
    }

    // constructor for custom coefficients
    LPF(type lfoType, float frequency, double Q, float a0, float a1, float a2, float b0, float b1, float b2, float wet, float dry)
         : Biquad (filterType, frequency, Q, a0, a1, a2, b0, b1, b2, wet, dry){}
    
    inline float processSample(float samp){
        
        if(filterType == FIRSTORDER) {
            float result = (0.5f * samp) + (firstOrderDelay * bandWidth);
            firstOrderDelay = samp;
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
    double firstOrderDelay = 0;
    double bandWidth = 0.5; // hard-set bandwidth for now
};


#endif /* LPF_h */
