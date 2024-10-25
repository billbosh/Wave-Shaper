/*
Custom waveshaper class based on juce dsp processors
*/

#include <juce_dsp/juce_dsp.h>

// different options waveshaper mode
enum class BillboshWaveShaperType {
    HardClip,
    SoftClip,
    SinoidFold
};

template <typename SampleType>
class BillboshWaveShaper {
public:
    BillboshWaveShaper();

    // define functions
    void setDrive (SampleType driveDB);
    float getDrive();
    void setMix (SampleType mix);
    void setShaperType (BillboshWaveShaperType newBillboshWaveShaperType);
    void prepare (const juce::dsp::ProcessSpec& spec);
    void reset();
    void reset (SampleType initialValue);

    // process input and output samples supplied in the processing context
    template <typename ProcessContext>
    void process (const ProcessContext& context) noexcept {
        const auto& inputBlock = context.getInputBlock();
        auto& outputBlock      = context.getOutputBlock();
        const auto numChannels = outputBlock.getNumChannels();
        const auto numSamples  = outputBlock.getNumSamples();

        jassert (inputBlock.getNumChannels() <= yold.size());
        jassert (inputBlock.getNumChannels() == numChannels);
        jassert (inputBlock.getNumSamples()  == numSamples);

        if (context.isBypassed) {
             outputBlock.copyFrom (inputBlock);
             return;
        }

        for (size_t channel = 0; channel < numChannels; ++channel) {
            auto* inputSamples  = inputBlock .getChannelPointer (channel);
            auto* outputSamples = outputBlock.getChannelPointer (channel);

            for (size_t i = 0; i < numSamples; ++i)
                outputSamples[i] = processSample ((int) channel, inputSamples[i]);
        }

        #if JUCE_DSP_ENABLE_SNAP_TO_ZERO
         snapToZero();
        #endif
    }

    // process one sample at a time
    SampleType processSample (int channel, SampleType inputValue);

    // ensure state variables are rounded to 0 if the state variables are denormals. 
    void snapToZero() noexcept;

private:
    std::vector<SampleType> yold;
    double sampleRate = 44100.0;
    SampleType drive = 0.0, mix = 1.0;
    BillboshWaveShaperType shaperType = BillboshWaveShaperType::SinoidFold;  
};
