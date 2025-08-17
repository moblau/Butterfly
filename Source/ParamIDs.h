#pragma once
#include <JuceHeader.h>

// ===========================
// Numeric groups and sizes
// ===========================
namespace PIDNum
{
    // Filter (global and per-voice)
    inline constexpr int FILTER_FREQ_BASE = 500;  // filter_freq1..4
    inline constexpr int FILTER_RES_BASE  = 600;  // filter_res1..4
    inline constexpr int FILTER_COUNT     = 4;

    // Wah (single)
    inline constexpr int WAH_BASE         = 700;  // wahFreq, wahFeedback, wahDelay, WahUseSync, WahSyncRate
    inline constexpr int WAH_COUNT        = 5;

    // Per-voice simple controls
    inline constexpr int PAN_BASE         = 800;  // PAN1..4
    inline constexpr int DETUNE_BASE      = 820;  // DETUNE1..4
    inline constexpr int MOD_INDEX_BASE   = 840;  // MOD_INDEX1..4
    inline constexpr int DOWNSAMPLE_BASE  = 860;  // DOWNSAMPLE1..4
    inline constexpr int VOICE_COUNT      = 4;

    // Waveforms per voice (choices)
    inline constexpr int MOD_WAVE_BASE    = 900;  // MOD_WAVEFORM1..4
    inline constexpr int CARRIER_WAVE_BASE= 920;  // WAVEFORM1..4

    // Sequencers (5 seq × 8 steps) — precomputed tables for zero concatenation at runtime
    inline constexpr int SEQ_STEP_BASE    = 1000; // seq{j}step{i}
    inline constexpr int SEQ_OFFS_BASE    = 1100; // seq{j}offset{i}
    inline constexpr int SEQ_COUNT        = 5;
    inline constexpr int STEPS_PER_SEQ    = 8;

    // Global filter
    inline constexpr int GLOBAL_FILTER_BASE = 2000; // res_filterfreq, filter_freq, filter_res, filter_env, filter_drive
    inline constexpr int GLOBAL_FILTER_COUNT = 5;

    // ADSRs (2 envelopes × 4 params each)
    inline constexpr int ENV_BASE         = 2100; // attack0, decay0, sustain0, release0, attack1, ...
    inline constexpr int ENV_ENVS         = 2;
    inline constexpr int ENV_PARAMS       = 4;

    // Example: “mod m -> car c” matrix (4×4)
    inline constexpr int MODMATRIX_BASE   = 3000; // mod_1_1 .. mod_4_4
    inline constexpr int MODMATRIX_DIM    = 4;
}

// ===========================
// String tables (constexpr)
// ===========================
namespace PIDStr
{
    // filter_freq1..4
    static constexpr const juce::String[] FILTER_FREQ[PIDNum::FILTER_COUNT] =
        { "filter_freq1", "filter_freq2", "filter_freq3", "filter_freq4" };

    static constexpr const juce::String[] FILTER_RES[PIDNum::FILTER_COUNT] =
        { "filter_res1",  "filter_res2",  "filter_res3",  "filter_res4"  };

    // per-voice
    static constexpr const juce::String[] PAN[PIDNum::VOICE_COUNT] =
        { "PAN1", "PAN2", "PAN3", "PAN4" };
    static constexpr const juce::String[] DETUNE[PIDNum::VOICE_COUNT] =
        { "DETUNE1", "DETUNE2", "DETUNE3", "DETUNE4" };
    static constexpr const juce::String[] MOD_INDEX[PIDNum::VOICE_COUNT] =
        { "MOD_INDEX1", "MOD_INDEX2", "MOD_INDEX3", "MOD_INDEX4" };
    static constexpr const juce::String[] DOWNSAMPLE[PIDNum::VOICE_COUNT] =
        { "DOWNSAMPLE1", "DOWNSAMPLE2", "DOWNSAMPLE3", "DOWNSAMPLE4" };

    static constexpr const juce::String[] MOD_WAVEFORM[PIDNum::VOICE_COUNT] =
        { "MOD_WAVEFORM1", "MOD_WAVEFORM2", "MOD_WAVEFORM3", "MOD_WAVEFORM4" };
    static constexpr const char* CARRIER_WAVEFORM[PIDNum::VOICE_COUNT] =
        { "WAVEFORM1", "WAVEFORM2", "WAVEFORM3", "WAVEFORM4" };

    // Wah block (order fixed to match base+offset)
    static constexpr const juce::String[] WAH[PIDNum::WAH_COUNT] =
        { "wahFreq", "wahFeedback", "wahDelay", "WahUseSync", "WahSyncRate" };

    // Global filter (order fixed)
    static constexpr const juce::String[] GLOBAL_FILTER[PIDNum::GLOBAL_FILTER_COUNT] =
        { "res_filterfreq", "filter_freq", "filter_res", "filter_env", "filter_drive" };

    // ADSR names (attack, decay, sustain, release)
    static constexpr const juce::String[] ENV_NAMES[PIDNum::ENV_PARAMS] =
        { "attack", "decay", "sustain", "release" };

    // Sequencers: prebuild names
    // seq{1..5}step{0..7}
    static constexpr const juce::String[] SEQ_STEPS[PIDNum::SEQ_COUNT][PIDNum::STEPS_PER_SEQ] = {
        { "seq1step0","seq1step1","seq1step2","seq1step3","seq1step4","seq1step5","seq1step6","seq1step7" },
        { "seq2step0","seq2step1","seq2step2","seq2step3","seq2step4","seq2step5","seq2step6","seq2step7" },
        { "seq3step0","seq3step1","seq3step2","seq3step3","seq3step4","seq3step5","seq3step6","seq3step7" },
        { "seq4step0","seq4step1","seq4step2","seq4step3","seq4step4","seq4step5","seq4step6","seq4step7" },
        { "seq5step0","seq5step1","seq5step2","seq5step3","seq5step4","seq5step5","seq5step6","seq5step7" }
    };

    // seq{1..5}offset{0..7}
    static constexpr const juce::String[] SEQ_OFFS[PIDNum::SEQ_COUNT][PIDNum::STEPS_PER_SEQ] = {
        { "seq1offset0","seq1offset1","seq1offset2","seq1offset3","seq1offset4","seq1offset5","seq1offset6","seq1offset7" },
        { "seq2offset0","seq2offset1","seq2offset2","seq2offset3","seq2offset4","seq2offset5","seq2offset6","seq2offset7" },
        { "seq3offset0","seq3offset1","seq3offset2","seq3offset3","seq3offset4","seq3offset5","seq3offset6","seq3offset7" },
        { "seq4offset0","seq4offset1","seq4offset2","seq4offset3","seq4offset4","seq4offset5","seq4offset6","seq4offset7" },
        { "seq5offset0","seq5offset1","seq5offset2","seq5offset3","seq5offset4","seq5offset5","seq5offset6","seq5offset7" }
    };

    // Mod matrix: mod_m -> car_c (1..4)
    static constexpr const juce::String[] MODMATRIX[PIDNum::MODMATRIX_DIM][PIDNum::MODMATRIX_DIM] = {
        { "mod_1_1","mod_1_2","mod_1_3","mod_1_4" },
        { "mod_2_1","mod_2_2","mod_2_3","mod_2_4" },
        { "mod_3_1","mod_3_2","mod_3_3","mod_3_4" },
        { "mod_4_1","mod_4_2","mod_4_3","mod_4_4" }
    };
}

// ===========================
// Numeric → string lookup
// ===========================
inline const juce::String[] paramIdToString (int numericId)
{
    using namespace PIDNum;
    using namespace PIDStr;

    // filter_freq
    if (numericId >= FILTER_FREQ_BASE && numericId < FILTER_FREQ_BASE + FILTER_COUNT)
        return FILTER_FREQ[numericId - FILTER_FREQ_BASE];

    // filter_res
    if (numericId >= FILTER_RES_BASE && numericId < FILTER_RES_BASE + FILTER_COUNT)
        return FILTER_RES[numericId - FILTER_RES_BASE];

    // per-voice PAN/DETUNE/MOD_INDEX/DOWNSAMPLE
    if (numericId >= PAN_BASE && numericId < PAN_BASE + VOICE_COUNT)
        return PAN[numericId - PAN_BASE];
    if (numericId >= DETUNE_BASE && numericId < DETUNE_BASE + VOICE_COUNT)
        return DETUNE[numericId - DETUNE_BASE];
    if (numericId >= MOD_INDEX_BASE && numericId < MOD_INDEX_BASE + VOICE_COUNT)
        return MOD_INDEX[numericId - MOD_INDEX_BASE];
    if (numericId >= DOWNSAMPLE_BASE && numericId < DOWNSAMPLE_BASE + VOICE_COUNT)
        return DOWNSAMPLE[numericId - DOWNSAMPLE_BASE];

    if (numericId >= MOD_WAVE_BASE && numericId < MOD_WAVE_BASE + VOICE_COUNT)
        return MOD_WAVEFORM[numericId - MOD_WAVE_BASE];
    if (numericId >= CARRIER_WAVE_BASE && numericId < CARRIER_WAVE_BASE + VOICE_COUNT)
        return CARRIER_WAVEFORM[numericId - CARRIER_WAVE_BASE];

    // Wah
    if (numericId >= WAH_BASE && numericId < WAH_BASE + WAH_COUNT)
        return WAH[numericId - WAH_BASE];

    // Global filter
    if (numericId >= GLOBAL_FILTER_BASE && numericId < GLOBAL_FILTER_BASE + GLOBAL_FILTER_COUNT)
        return GLOBAL_FILTER[numericId - GLOBAL_FILTER_BASE];

    // ADSR: ENV_BASE + (env * 4 + p)
    if (numericId >= ENV_BASE && numericId < ENV_BASE + ENV_ENVS * ENV_PARAMS)
    {
        const int idx = numericId - ENV_BASE;
        const int p   = idx % ENV_PARAMS; // 0..3
        const int e   = idx / ENV_PARAMS; // 0..(ENV_ENVS-1)
        // Return "attack0"/"decay1" etc. We’ll build the number minimally here:
        // (Only once at parameter creation time—never in the audio path.)
        static thread_local juce::String tmp;
        tmp = PIDStr::ENV_NAMES[p]; tmp += juce::String(e);
        return tmp.toRawUTF8(); // note: only use immediately to construct ParameterID
    }

    // Sequencer steps/offsets: SEQ_STEP_BASE + (seq * 8 + step)
    if (numericId >= SEQ_STEP_BASE && numericId < SEQ_STEP_BASE + SEQ_COUNT * STEPS_PER_SEQ)
    {
        const int idx = numericId - SEQ_STEP_BASE;
        return PIDStr::SEQ_STEPS[idx / STEPS_PER_SEQ][idx % STEPS_PER_SEQ];
    }
    if (numericId >= SEQ_OFFS_BASE && numericId < SEQ_OFFS_BASE + SEQ_COUNT * STEPS_PER_SEQ)
    {
        const int idx = numericId - SEQ_OFFS_BASE;
        return PIDStr::SEQ_OFFS[idx / STEPS_PER_SEQ][idx % STEPS_PER_SEQ];
    }

    // Mod matrix: MODMATRIX_BASE + (m*4 + c)
    if (numericId >= MODMATRIX_BASE && numericId < MODMATRIX_BASE + MODMATRIX_DIM * MODMATRIX_DIM)
    {
        const int idx = numericId - MODMATRIX_BASE;
        return PIDStr::MODMATRIX[idx / MODMATRIX_DIM][idx % MODMATRIX_DIM];
    }

    return nullptr; // unknown
}

