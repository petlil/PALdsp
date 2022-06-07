/*
  ==============================================================================

    FeedbackCombFilter.h
    Created: 31 May 2022 9:59:05pm
    Author:  Peter Liley

  ==============================================================================
*/

#ifndef FeedbackCombFilter_h
#define FeedbackCombFilter_h

#include "CircularBufferShort.h"

class FeedbackCombFilter {
public:
    
    FeedbackCombFilter(int delay, float feedbackGain){
        buffer.setLengthSamples(delay);
        this->delay = delay;
        this->feedbackGain = feedbackGain;
    }
    
    ~FeedbackCombFilter(){};
    
    /**
     Process a sample through the filter.
     Returns the next sample.
     */
    inline float processSample(float input) {
        float nextSamp = input + (buffer.getSample() * feedbackGain);
        buffer.pushSample(nextSamp);
        return nextSamp;
    }
    
    /* Set length of delay line. */
    inline void setDelay(int delay) {
        this-> delay = delay;
        buffer.setLengthSamples(1);
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
    
private:
    CircularBufferShort buffer { 1 }; // default buffer with length 1 sample
    int delay;
    float feedbackGain;
};


#endif /* FeedbackCombFilter_h */
