/*
  ==============================================================================

    BitCrush.h
    Created: 25 Mar 2022 5:09:38pm
    Author:  Peter Liley

  ==============================================================================
*/

#ifndef BitCrush_h
#define BitCrush_h
#include <string.h>

class BitCrush {
public:
    BitCrush() {
        // default rate of 1 (no desampling)
        rate = 1;
        bitDepth = 32;
    }
    
    ~BitCrush(){};
    
    /**
     Sets how many n samples are repeated in the
     desampling process.
     */
    inline void setDesamplingRate(int rateVal){
        rate = (rateVal > 0) ? rateVal : rate;
    }
    
    /**
     Returns the current desampling rate.
     */
    inline int getDesamplingRate() {
        return rate;
    }
    
    /**
     Sets how many quantisation levels will be applied to the signal
     */
    inline void setBitDepth(int bitDepthVal) {
        bitDepth = (bitDepthVal > 0 && bitDepthVal <= 32) ? bitDepthVal : bitDepth;
    }
    
    /**
     Gets the current bit depth
     */
    inline int getBitDepth() {
        return bitDepth;
    }
    
    /**
     Takes the buffer and sets every n samples to the same value.
     @param data Pointer to the beginning of the buffer.
     @param bufferLength The length of the current buffer.
     */
    inline void desample(float* data, int bufferLength){
        for(int samp = 0; samp < bufferLength; samp++){
            // use modulo operator to desample
            if (rate > 1)
            {
                if (samp%rate != 0) data[samp] = data[samp - samp%rate];
            }
        }
    }
    
    /**
     Takes the buffer and sets every n samples to the same value,
     accepting a custom n value (instead of using the class field 'rate')
     @param data Pointer to the beginning of the buffer.
     @param bufferLength The length of the current buffer.
     @param customRate The number of samples to repeat.
     */
    inline void desample(float* data, int bufferLength, int customRate){
        for(int samp = 0; samp < bufferLength; samp++){
            // use modulo operator to desample
            if (customRate > 1)
            {
                if (samp%customRate != 0) data[samp] = data[samp - samp%customRate];
            }
        }
    }

    /**
     Quantizes the audio to the closest signal value between -1 and 1
     according to the value of the bitDepth field.
     This function is pass-by-reference and directly edits the given sample.
     */
    inline void crush(float* sample) {
        
        // implementation taken from an online Bitcrusher Demo
        float totalQLevels = powf(2, bitDepth);
        *sample = (float) 1/totalQLevels * floor((*sample * totalQLevels) + 0.5);
    }
    
    /**
     Quantizes the audio to the closest signal value between -1 and 1
     according to the value of the bitDepth field.
     @return The quantized sample.
     */
    inline float crush(float sample) {
        
        // implementation taken from an online Bitcrusher Demo
        float totalQLevels = powf(2, bitDepth);
        float remainder = std::fmodf(sample, 1/totalQLevels);
        
        // quantize
        return sample - remainder;
    }
    

    
private:
    
    int rate;
    int bitDepth;
};


#endif /* BitCrush_h */
