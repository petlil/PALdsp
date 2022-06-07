/*
  ==============================================================================

    LFO.h
    Created: 3 Apr 2022 9:36:09pm
    Author:  Peter Liley
 
    Creates an LFO that should be called every sample.

  ==============================================================================
*/

#ifndef LFO_h
#define LFO_h
#define PI      3.14159265358979323846
using namespace juce;

class LFO {
public:
    
    enum Oscillator {
        SINE,
        TRIANGLE,
        SQUARE,
        SAW,
        RANDOM
    };
    
    /**
     Creates a sine-wave LFO with a default frequency of 1Hz
     and a default sample rate of 44.1kHz.
     NB: You should definitely change the sample rate to match the host
     in the prepareToPlay function!*/
    LFO(){
        this->sampleRate = 44100;
        this->frequency = 1;
        this->samplesPerCycle = sampleRate * frequency;
        this->currProgress = 0;
        this->currOscillator = Oscillator::SINE;
    }
    
    /**
     Creates a sine-wave LFO with a default frequency of 1Hz.
     The amplitude range of this LFO is  0 - 1.
     */
    LFO(int sampleRate){
        LFO();
        this->sampleRate = sampleRate;
    }
    
    /**
     Creates a sine-wave LFO with a custom frequency (in Hz).
     The amplitude range of this LFO is  0 - 1.
     */
    LFO(int sampleRate, float frequency){
        LFO();
        this->sampleRate = sampleRate;
        this->frequency = frequency;
        this->samplesPerCycle = (int) sampleRate / frequency;
    }
    
    /**
     Creates a custom-wave LFO with a custom frequency (in Hz).
     The amplitude range of this LFO is  0 - 1.
     */
    LFO(int sampleRate, float frequency, LFO::Oscillator osc){
        this->sampleRate = sampleRate;
        this->frequency = frequency;
        this->samplesPerCycle = (int) (sampleRate / frequency);
        this->currProgress = 0;
        this->currOscillator = osc;
    }
    
    /**
     Creates a custom-wave LFO with a custom frequency (in Hz).
     The amplitude range of this LFO is  0 - 1.
     */
    LFO(int sampleRate, float frequency, LFO::Oscillator osc, float min, float max){
        this->sampleRate = sampleRate;
        this->frequency = frequency;
        this->samplesPerCycle = (int) (sampleRate / frequency);
        this->currProgress = 0;
        this->currOscillator = osc;
        this->min = min;
        this->max = max;
    }
    
    ~LFO(){};
    
    /**
     Sets the current oscillator type for the LFO
     */
    inline void setType(LFO::Oscillator s){
        currOscillator = s;
    }
    
    /**
     Sets the frequency of the LFO in Hz.
     */
    inline void setfrequency(float frequency){
        if(frequency <= 0) jassertfalse;
        this->frequency = frequency;
        // set samples per cycle according to sample rate
        this->samplesPerCycle = (int) (sampleRate / frequency);
    }
    
    /**
     Set the sample rate for the LFO.
     */
    inline void setSampleRate(int rate){
        sampleRate = rate;
        samplesPerCycle = (int) (sampleRate / frequency);
    }
    
    /**
     Sets the range of values that this LFO object returns
     */
    inline void setRange(float min, float max){
        this->min = min;
        this->max = max;
    }
    
    /**
     Sets the phase value of this LFO (From 0 - 1).
     */
    inline void setPhase(float phase) {
        jassert(phase <= 1 && phase >= 0);
        phaseVal = phase;
    }
    
    /**
     Get the phase of this LFO
     */
    inline float getPhase() {
        return phaseVal;
    }
    
    /**
     Get the value at the current point the LFO cycle.
     */
    inline float getValue(float phase = 0){
        if(phaseVal == 0) {
            // cheack the phase is within 0-1
            jassert(phase <= 1 && phase >= 0);
            phase = (phase > 1 || phase < 0) ? 0 : phase;
        }
        else phase = phaseVal;
        
        // get the current progress as a float value from 0 - 1
        float progress = (float) currProgress / (float) samplesPerCycle;
        // phase-offset
        if(phase != 0){
            progress += phase;
            progress = (progress > 1) ? progress - 1 : progress;
        }
        
        switch (currOscillator) {
            case Oscillator::SINE:
                return fastSin(progress);
                break;
            case Oscillator::TRIANGLE:
                return tri(progress);
                break;
            case Oscillator::SQUARE:
                return sqr(progress);
                break;
            case Oscillator::SAW:
                return saw(progress);
                break;
            case Oscillator::RANDOM:
                return rand(progress);
                break;
            default:
                return 0.0;
        }
    }

    /**
     Generates and returns the next sample value for the current LFO.
     */
    inline float next(){
        // SPEEDUP
        // increment the progress through the cycle in samples
        currProgress = (currProgress + 1) % samplesPerCycle;
        
        // and get the value (mapped if relevant)
        return jmap(getValue(), min, max);
    }
    
private:
    // FIELDS =================
    
    int sampleRate; // host sample frequency
    float frequency; // in Hz
    int samplesPerCycle; // number of samples in an LFO cycle
    int currProgress; // progress through the LFO cycle (in samples)
    unsigned int currOscillator; // the type of oscillator
    int phaseVal = 0;
    
    float min = 0;
    float max = 1;
    
    // FUNCTIONS =================

    
    //Parabolic sinewave approx (Martijn 2019 via Jim Murphy 2022)
    inline float fastSin(float progressFloat){
        if (progressFloat > 0.5)
            progressFloat -= 1.0f;
        return(progressFloat * (8 - (16 * abs(progressFloat))));
    }
    
    inline float tri(float progressFloat){
        return (progressFloat <= 0.5) ? progressFloat * 2 : (1 - progressFloat) * 2;
    }
    
    inline float sqr(float progressFloat){
        return (progressFloat <= 0.5) ? 1 : 0;
    }
    
    inline float saw(float progressFloat){
        return progressFloat;
    }
    
    inline float rand(float progressFloat){
        // TODO
        return 0.1f;
    }
};


#endif /* LFO_h */
