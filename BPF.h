/*
  ==============================================================================

    BPF.h
    Created: 10th Jun 2022
    Author:  Peter Liley
 
    Class for accessing various kinds of bandpass filter.
    - The Biquad implmentation has a constant 0dB peak gain.
 
    Equations from: Audio EQ Cookbook
    https://webaudio.github.io/Audio-EQ-Cookbook/audio-eq-cookbook.html

  ==============================================================================
*/

#include "Biquad.h"

#ifndef BPF_h
#define BPF_h

class BPF : public Biquad {
public:
    
    enum type {
        BIQUAD
    };
    
    BPF(type filterType, float frequency, float Q, float wet = 1, float dry = 0) : Biquad (filterType, frequency, wet, dry) {

        // audio-eq-cookbook
        double w0 = 2 * (frequency/44100) * PI;
        double cosW0 = cos(w0);
        double sinW0 = sin(w0);
        double alpha = sinW0 / (Q * 2);

        b0 = alpha;
        b1 = 0;
        b2 = -1 * alpha;
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


#endif /* BPF_h */
