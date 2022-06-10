/*
  ==============================================================================

    AllPassFilter.h
    Created: 26 May 2022 5:30:06pm
    Author:  Peter Liley

  ==============================================================================
*/

#define AllPassFilter_h
#include "CircularBufferShort.h"
#include "LFO.h"

class AllPassFilter {
public:
    
    AllPassFilter() {
    }

    /**
    An allpass filter with a given delay length and feedback/forward gains
    */
    AllPassFilter(unsigned int length, float feedbackGain, float feedForwardGain){
        buffer.setLengthSamples(length);
        delay = length;
        this->feedbackGain = feedbackGain;
        this->feedForwardGain = feedForwardGain;
    }
    
    /**
    A modulated allpass filter with a given delay length and feedback/forward gains,
    plus given modulation rate (in Hz) and size (in samples).
    */
    AllPassFilter(unsigned int length, float feedbackGain, float feedForwardGain,
                  float lfoFreq, float lfoSizeSamples, int sampleRate = 44100, float phase = 0){
        buffer.setLengthSamples(length);
        delay = length;
        this->feedbackGain = feedbackGain;
        this->feedForwardGain = feedForwardGain;
        
        lfo.setfrequency(lfoFreq); // modulation rate
        lfo.setRange(0, 1);
        lfo.setPhase(phase);
        buffer.setSampleRate(sampleRate);
        buffer.setModRange(lfoSizeSamples); // modulation range
    }
    
    ~AllPassFilter(){};
    
    /**
     Process a sample through the filter.
     Returns the next sample.
     */
    inline float processSample(float sample) {
        lfo.next();
        buffer.mapReadHeadMod(lfo.getValue());
        float next = buffer.getSample();
        buffer.pushSample(sample + (next * feedbackGain));
        return next + (sample * feedForwardGain);
    }
    
    /**
     Taps the delay line at a given sample.
     */
    inline float tap(int index) {
        return buffer.tap(index);
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
    // standard allpass fields
    CircularBufferShort buffer { 1 }; // default length of 1
    unsigned int delay;
    float feedForwardGain;
    float feedbackGain;
    
    // modulated allpass fields
    LFO lfo;
};
