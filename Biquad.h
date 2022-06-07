/*
  ==============================================================================

    filter.h
    Created: 7th Jun 2022
    Author:  Peter Liley
 
    Virtual superclass for all biquad types.

  ==============================================================================
*/

#include "filter.h"

#ifndef Biquad_h
#define Biquad_h
#define PI 3.14159265358979323846;
#include <math.h>

class Biquad : public filter {
public:
    
    Biquad(int lfoType, float frequency) : filter(lfoType, frequency){}

    Biquad(int lfoType, float frequency, float a0, float a1, float a2, float b1, float b2, float c0, float d0) : filter(lfoType, frequency){
      this->a0 = a0;
      this->a1 = a1;
      this->a2 = a2;
      this->b1 = b1;
      this->b2 = b2;
      this->c0 = c0;
      this->d0 = d0;
    }

    virtual ~Biquad() {}
    
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

        return (result * c0) + d0;
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
    float a0 = 0, a1 = 0, a2 = 0, b1 = 0, b2 = 0, d0 = 0, c0 = 0;
    // delay registers
    float a1Delay = 0;
    float a2Delay = 0;
    float b1Delay = 0;
    float b2Delay = 0;
};


#endif /* Biquad_h */
