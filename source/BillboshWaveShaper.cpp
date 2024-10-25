/*
Custom waveshaper class based on juce dsp processors
*/

#include "BillboshWaveShaper.h"
#include "juce_audio_basics/juce_audio_basics.h"

template <typename SampleType>
BillboshWaveShaper<SampleType>::BillboshWaveShaper() {
    setDrive (juce::Decibels::decibelsToGain(drive));
    setMix (mix);
}

template <typename SampleType>
void BillboshWaveShaper<SampleType>::setDrive (SampleType newDrive) {
    drive = juce::Decibels::decibelsToGain(newDrive);
}

template <typename SampleType>
void BillboshWaveShaper<SampleType>::setMix (SampleType newMix) {
    mix = newMix;
}

template <typename SampleType>
void BillboshWaveShaper<SampleType>::setShaperType (BillboshWaveShaperType newBillboshWaveShaperType) {
    shaperType = newBillboshWaveShaperType;
    reset();
}

template <typename SampleType>
void BillboshWaveShaper<SampleType>::prepare (const juce::dsp::ProcessSpec& spec) {
    jassert (spec.sampleRate > 0);
    jassert (spec.numChannels > 0);

    sampleRate = spec.sampleRate;
    
    setDrive (juce::Decibels::decibelsToGain(drive));
    setMix (mix);

    yold.resize (spec.numChannels);

    reset();
}

template <typename SampleType>
void BillboshWaveShaper<SampleType>::reset() {
    reset (0);
}

template <typename SampleType>
void BillboshWaveShaper<SampleType>::reset (SampleType initialValue) {
    for (auto& old : yold)
        old = initialValue;
}

template <typename SampleType>
SampleType BillboshWaveShaper<SampleType>::processSample (int channel, SampleType inputValue) {
    jassert (isPositiveAndBelow (channel, yold.size()));

    SampleType cleanSample = inputValue, distortedSample, result;

    switch (shaperType) {
        case BillboshWaveShaperType::HardClip:
            distortedSample = cleanSample * drive;
        
            if (distortedSample > 1.0f) {
                distortedSample = 1.0f;
            }
            else if (distortedSample < -1.0f) {
                distortedSample = -1.0f;
            }
            break;

        case BillboshWaveShaperType::SoftClip:
            distortedSample = std::tanh(cleanSample * drive);
            break;

        case BillboshWaveShaperType::SinoidFold:
            distortedSample = std::sin(cleanSample * drive);
            break;
    } 

    result = (1.0f - mix) * cleanSample + mix * distortedSample;
    return result;
}   

template <typename SampleType>
void BillboshWaveShaper<SampleType>::snapToZero() noexcept {
    for (auto& old : yold)
        juce::dsp::util::snapToZero (old);
}

template class BillboshWaveShaper<float>;
template class BillboshWaveShaper<double>;
