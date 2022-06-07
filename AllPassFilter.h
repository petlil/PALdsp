/*
  ==============================================================================

    AllPassFilter.h
    Created: 26 May 2022 5:30:06pm
    Author:  Peter Liley

  ==============================================================================
*/

#define AllPassFilter_h
#include "CircularBuffer.h"
#include "CircularBufferVariable.h"

class AllPassFilter {
public:
    
    AllPassFilter() {
    }

    /**
    An allpass filter with a given delay length and feedback/forward gains
    */
    AllPassFilter(unsigned int length, float feedbackGain, float feedForwardGain){
        buffer.setReadHeadDelay(length);
        delay = length;
        this->feedbackGain = feedbackGain;
        this->feedForwardGain = feedForwardGain;
    }
    
    ~AllPassFilter(){};
    
    /**
     Process a sample through the filter.
     Returns the next sample.
     */
    inline float processSample(float sample) {
        float next = buffer.getSample();
        buffer.pushSample(sample + (next * feedbackGain));
        return next + (sample * feedForwardGain);
    }
    
    /**
     Set gain of feedback line
     */
    inline void setFeedbackGain(float gain) {
        jassert(gain >= 0 && gain <= 1);
        feedbackGain = gain;
    }
    
    /**
     Set gain of feedforward line
     */
    inline void setFeedForwardGain(float gain) {
        jassert(gain >= 0 && gain <= 1);
        feedForwardGain = gain;
    }
    
    /**
     Get gain value of feedback line.
     */
    inline float getFeedbackGain() {
        return feedbackGain;
    }
    
    /**
     Get gain value of feed forward line.
     */
    inline float getFeedForwardGain() {
        return feedForwardGain;
    }
    
private:
    CircularBuffer buffer { 1 }; // default length of 1
    unsigned int delay;
    float feedForwardGain;
    float feedbackGain;
};
