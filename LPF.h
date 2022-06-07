
//
//  Gain.h
//  Gain Setter
//
//  Created by Jim Murphy on 9/05/20.
//

#include "filter.h"

#ifndef LPF_h
#define LPF_h

class LPF : public filter {
public:
    
    enum type {
        FIRSTORDER,
        BIQUAD
    }
    
    LPF(type lfoType){
        this->lfoType = lfoType;
    }
    
    ~LPF(){};
    
    inline void processSample(float samp){
        
        if(lfoType == type::FIRSTORDER) {
            // first order processing
        }
        else if(lfoType == type::BIQUAD) {
            // biquad processing
        }
    }

    int setType() {
        
    }
    
private:
    int lfoType;
};


#endif /* LPF_h */
