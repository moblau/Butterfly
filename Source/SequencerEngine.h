// SequencerEngine.h
#pragma once
#include <array>
#include <cmath>
#include <algorithm>

struct StepSequencerParams
{
    int   stepCount     = 8;        // 1..8
    float beatsPerStep  = 0.25f;    // e.g. 1/16
    std::array<float, 8> stepValues  { { 0.0f } }; // 0..1
    std::array<float, 9> stepOffsets { { 0.0f } }; // boundary offsets, [8] mirrors [0], range ~[-0.5, +0.5]
    float glideAmount   = 0.0f;     // 0..1 (portion of a step length to glide)
};

class SequencerEngine
{
public:
    void prepare (double sampleRateHz) { sr = sampleRateHz; }

    void setParams (const StepSequencerParams& p)
    {
        params = p;

        // Harden
        params.stepCount = std::clamp(params.stepCount, 1, 8);
        params.stepOffsets[8] = params.stepOffsets[0];
        for (float& o : params.stepOffsets) o = std::clamp(o, -0.5f, 0.5f);
    }

    // Call once per processBlock
    void beginBlock (double bpmIn, double ppqAtBlockStart)
    {
        bpm = (bpmIn > 0.0 ? bpmIn : 120.0);
        // steps/sample = (beats/sample) / (beats/step)
        const double beatsPerSample = (bpm / 60.0) / sr;
        stepsPerSample = beatsPerSample / (double)params.beatsPerStep;
        posSteps0      = ppqAtBlockStart / (double)params.beatsPerStep; // step units at sample 0
        // snapshot for this block
        P = params;
    }

    // Pure read: sample-accurate step value with linear glide into the current step
    float valueAtSample(int n, double currentSampleRate) const
    {
        const int N = P.stepCount;
        const float stepLenBeats = P.beatsPerStep;

        // Compute the absolute position in step units, using the current sample rate
        const double stepsPerSample = (stepLenBeats * bpm) / (60.0 * currentSampleRate); // steps per sample
        const double posSteps = posSteps0 + stepsPerSample * (double)n;
        const double posWrapped = wrapToRange(posSteps, 0.0, (double)N); // [0..N)

        // Find active step i using boundary model
        int i = 0;
        double Bi = 0.0, Bi1 = 0.0; // current boundary and next
        findStepAndBounds(posWrapped, N, Bi, Bi1, i);

        const int iprev = (i == 0 ? N - 1 : i - 1);

        const float vPrev = P.stepValues[iprev];
        const float vCurr = P.stepValues[i];

        // Time since step start in "steps"
        double elapsedSteps = posWrapped - Bi;
        if (elapsedSteps < 0.0) elapsedSteps += (double)N; // wrap guard

        // Convert to seconds
        const double secondsPerStep = (stepLenBeats * 60.0) / std::max(1.0, bpm);
        const double elapsedSec     = elapsedSteps * secondsPerStep;

        // Glide time = fraction of step duration
        const double glideSec = std::clamp((double)P.glideAmount, 0.0, 1.0) * secondsPerStep;

        if (glideSec <= 0.0) return vCurr;

        const float t = (float) std::clamp(elapsedSec / glideSec, 0.0, 1.0); // 0..1 in first part of step
        return vPrev + (vCurr - vPrev) * t; // linear glide from prev->curr
    }
    //    float valueAtSample (int n) const
//    {
//        const int   N = P.stepCount;
//        const float stepLenBeats = P.beatsPerStep;
//        const double posSteps = posSteps0 + stepsPerSample * (double)n;     // absolute position in step units
//        const double posWrapped = wrapToRange(posSteps, 0.0, (double)N);    // [0..N)
//
//        // Find active step i using boundary model B_i = i + offset[i]
//        int i = 0;
//        double Bi = 0.0, Bi1 = 0.0; // current boundary and next
//        findStepAndBounds(posWrapped, N, Bi, Bi1, i);
//
//        const int iprev = (i == 0 ? N - 1 : i - 1);
//
//        const float vPrev = P.stepValues[iprev];
//        const float vCurr = P.stepValues[i];
//
//        // Time since step start in "steps"
//        double elapsedSteps = posWrapped - Bi;
//        if (elapsedSteps < 0.0) elapsedSteps += (double)N; // wrap guard
//
//        // Convert to seconds
//        const double secondsPerStep = (stepLenBeats * 60.0) / std::max(1.0, bpm);
//        const double elapsedSec     = elapsedSteps * secondsPerStep;
//
//        // Glide time = fraction of step duration
//        const double glideSec = std::clamp((double)P.glideAmount, 0.0, 1.0) * secondsPerStep;
//
//        if (glideSec <= 0.0) return vCurr;
//
//        const float t = (float) std::clamp(elapsedSec / glideSec, 0.0, 1.0); // 0..1 in first part of step
//        return vPrev + (vCurr - vPrev) * t; // linear glide from prev->curr
////        return 0;
//    }

private:
    // ---- helpers ----
    static double wrapToRange (double x, double a, double b) // [a,b)
    {
        const double w = b - a;
        x = std::fmod(x - a, w);
        if (x < 0.0) x += w;
        return x + a;
    }

    // Given pos ∈ [0..N), find step i and boundaries [B_i, B_{i+1}) with wrap handling
    void findStepAndBounds (double posWrapped, int N, double& Bi, double& Bi1, int& i) const
    {
        for (int k = 0; k < N; ++k)
        {
            const double start = (double)k     + (double)P.stepOffsets[k];
            double       end   = (double)(k+1) + (double)P.stepOffsets[k+1];

            Bi  = start;
            Bi1 = end;

            // region without wrap
            if (end < (double)N)
            {
                if (posWrapped >= start && posWrapped < end) { i = k; return; }
            }
            else
            {
                // region wraps beyond N: treat as [start..N) ∪ [0..end-N)
                if (posWrapped >= start || posWrapped < (end - (double)N)) { i = k; return; }
            }
        }

        // Fallback (shouldn't happen)
        i  = (int)std::floor(posWrapped) % N;
        Bi = (double)i + (double)P.stepOffsets[i];
        Bi1= (double)(i+1) + (double)P.stepOffsets[i+1];
    }

    // ---- per-instance state ----
    double sr  = 44100.0;
    double bpm = 120.0;
    double posSteps0 = 0.0;       // steps at start of block
    double stepsPerSample = 0.0;  // steps advanced per sample

    StepSequencerParams params;   // latest (writer)
    StepSequencerParams P;        // block snapshot (reader)
};
