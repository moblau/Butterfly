#pragma once

class SmoothDownsampler
{
public:
    SmoothDownsampler(int factor);

    void setFactor(int factor);
    float processSample(float inputSample);

private:
    int downsampleFactor;
    int counter;
    float lastOutput, nextOutput;
    float interpStep, currentInterp;
};
