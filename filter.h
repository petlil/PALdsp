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
    
    filter(int filterType, float frequency){
        this->filterType = filterType;
        this->freq = frequency;
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
    
protected:
    int filterType;
    float freq = 0;
};


#endif /* filter_h */
