/*
  ==============================================================================

    filter.h
    Created: 7th Jun 2022
    Author:  Peter Liley
 
    Superclass for all filter types.

  ==============================================================================
*/

#ifndef filter_h
#define filter_h

class filter {
public:
    
    filter(int filterType, float frequency, float wet = 1, float dry = 0){
        this->filterType = filterType;
        this->freq = frequency;
        this->wet = wet;
        this->dry = dry;
    }
    
    virtual ~filter() {}
    
    /**
     * @brief Processes a sample through this filter.
     * 
     * @param samp The sample to be processed through the filter.
     * @return float The processed sample
     */
    virtual inline float processSample(float samp) = 0;
    virtual void setType(int newType) = 0;
    
    void setWet(float gain) {
        jassert(gain <= 1 && gain >= 0);
        wet = gain;
    }
    
    void setDry(float gain) {
        jassert(gain <= 1 && gain >= 0);
        dry = gain;
    }
    
    float getWet() { return wet; }
    float getDry() { return wet; }
    
    
protected:
    int filterType;
    float freq;
    float wet;
    float dry;
};


#endif /* filter_h */
