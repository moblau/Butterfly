#include "SmoothDownsampler.h"
#include <algorithm>

SmoothDownsampler::SmoothDownsampler(int factor)
    : downsampleFactor(std::max(1, factor)), counter(0),
      lastOutput(0.0f), nextOutput(0.0f), interpStep(0.0f), currentInterp(0.0f)
{}

void SmoothDownsampler::setFactor(int factor)
{
    downsampleFactor = std::max(1, factor);
}

float SmoothDownsampler::processSample(float inputSample)
{
    if (downsampleFactor <= 1)
        return inputSample;
    
    if (counter == 0)
    {
        lastOutput = currentInterp;
        nextOutput = inputSample;
        interpStep = (nextOutput - lastOutput) / static_cast<float>(downsampleFactor);
        currentInterp = lastOutput;
    }

    float output = currentInterp;
    currentInterp += interpStep;
    counter = (counter + 1) % downsampleFactor;
    return output;
}
