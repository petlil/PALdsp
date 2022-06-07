/*
  ==============================================================================

    LowpassFeedbackCombFilter.h
    Created: 5 Jun 2022 4:15:26pm
    Author:  Peter Liley

  ==============================================================================
*/


#ifndef LowpassFeedbackCombFilter_h
#define LowpassFeedbackCombFilter_h

#include "CircularBuffer.h"

class LowpassFeedbackCombFilter {
public:
    LowpassFeedbackCombFilter(int delay, float feedbackGain, float damping){
        buffer.setReadHeadDelay(delay);
        this->delay = delay;
        this->feedbackGain = feedbackGain;
        damp1 = damping;
        damp2 = 1 - damping;
    }
    
    ~LowpassFeedbackCombFilter(){};
    
    /**
     Process a sample through the filter.
     Returns the next sample.
     */
    inline float processSample(float input) {
        // code based on programme written by
        // Jezar at Dreampoint, June 2000
        // http://www.dreampoint.co.uk
        
        
        float output = buffer.getSample();
        // undenormalise may be necessary here
        
        filteredVal = (output * damp2) + (filteredVal * damp1);
        // undenormalise may be necessary here

        buffer.pushSample(input + filteredVal * feedbackGain);

        return output;
    }
    
    /* Set length of delay line. */
    inline void setDelay(int delay) {
        this-> delay = delay;
        buffer.setReadHeadDelay(1);
    }
    
    /* Set gain of feedback parameter. */
    inline void setFeedbackGain(float gain) {
        jassert(gain >= 0 && gain <= 1);
        feedbackGain = gain;
    }
    
    /**
     Get gain value of feedback line.
     */
    inline float getFeedbackGain() {
        return feedbackGain;
    }
    
    /**
     Set damping value for lowpass filter
     */
    inline void setDamping(float damping) {
        damp1 = damping;
        damp2 = 1 - damping;
    }
    
    /**
     Get value of damping value for lowpass filter
     */
    inline float getDamping() {
        return damp1;
    }
    
private:
    CircularBuffer buffer { 1 }; // default buffer with length 1 sample
    int delay;
    float feedbackGain;
    float filteredVal;
    float damp1;
    float damp2;
};


#endif /* FeedbackCombFilter_h */
