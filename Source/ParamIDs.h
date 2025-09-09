// === Add to ParamIDs.h ===
#pragma once
#include <JuceHeader.h>

namespace PID
{

    // Global filter
    inline const juce::String filter_freq  { "filter_freq"  };
    inline const juce::String filter_res   { "filter_res"   };
    inline const juce::String filter_env   { "filter_env"   };
    inline const juce::String filter_drive { "filter_drive" };

    // Per-voice filter (index 0..3 => ids 1..4)
    inline const juce::String filter_freqN[4] { "filter_freq1","filter_freq2","filter_freq3","filter_freq4" };
    inline const juce::String filter_resN [4] { "filter_res1", "filter_res2", "filter_res3", "filter_res4"  };

    // Per-voice “modulate” flags for the filter controls shown in your code
    inline const juce::String FREQ_mod[4] { "Frequency1modulate","Frequency2modulate","Frequency3modulate","Frequency4modulate" };
    inline const juce::String RES_mod [4] { "Resonance1modulate","Resonance2modulate","Resonance3modulate","Resonance4modulate" };

    // Envelopes (2 envs)
    inline const juce::String attack[2]  { "attack0",  "attack1"  };
    inline const juce::String decay[2]   { "decay0",   "decay1"   };
    inline const juce::String sustain[2] { "sustain0", "sustain1" };
    inline const juce::String release[2] { "release0", "release1" };

    // Voices 1..4 (index 0..3)
    inline const juce::String PAN[4]        { "PAN1","PAN2","PAN3","PAN4" };
    inline const juce::String DETUNE[4]     { "DETUNE1","DETUNE2","DETUNE3","DETUNE4" };
    inline const juce::String DOWNSAMPLE[4] { "DOWNSAMPLE1","DOWNSAMPLE2","DOWNSAMPLE3","DOWNSAMPLE4" };
    inline const juce::String MOD_INDEX[4]  { "MOD_INDEX1","MOD_INDEX2","MOD_INDEX3","MOD_INDEX4" };
    inline const juce::String MOD_WAVE[4]   { "MOD_WAVEFORM1","MOD_WAVEFORM2","MOD_WAVEFORM3","MOD_WAVEFORM4" };
    inline const juce::String CARR_WAVE[4]  { "WAVEFORM1","WAVEFORM2","WAVEFORM3","WAVEFORM4" };

    inline const juce::String MOD_RATIO_NUM[4] { "MOD_RATIO_NUM1","MOD_RATIO_NUM2","MOD_RATIO_NUM3","MOD_RATIO_NUM4" };
    inline const juce::String MOD_RATIO_DEN[4] { "MOD_RATIO_DEN1","MOD_RATIO_DEN2","MOD_RATIO_DEN3","MOD_RATIO_DEN4" };

    inline const juce::String OCTAVE[4] { "Octave1","Octave2","Octave3","Octave4" };
    inline const juce::String VIBDEPTH[4] { "Vib Depth1","Vib Depth2","Vib Depth3","Vib Depth4" };
    inline const juce::String VIBFREQ[4] { "Vib Freq1","Vib Freq2","Vib Freq3","Vib Freq4" };

    // Per-voice “modulate” flags
    inline const juce::String PAN_mod[4]        { "Pan1modulate","Pan2modulate","Pan3modulate","Pan4modulate" };
    inline const juce::String DETUNE_mod[4]     { "Detune1modulate","Detune2modulate","Detune3modulate","Detune4modulate" };
    inline const juce::String DOWNSAMPLE_mod[4] { "Downsample1modulate","Downsample2modulate","Downsample3modulate","Downsample4modulate" };
    inline const juce::String MODAMOUNT_mod[4]  { "Mod Amount1modulate","Mod Amount2modulate","Mod Amount3modulate","Mod Amount4modulate" };
    inline const juce::String NUM_mod[4]        { "Num1modulate","Num2modulate","Num3modulate","Num4modulate" };
    inline const juce::String DEN_mod[4]        { "Den1modulate","Den2modulate","Den3modulate","Den4modulate" };

    inline const juce::String OCTAVE_mod[4]        { "Octave1modulate","Octave2modulate","Octave3modulate","Octave4modulate" };
    inline const juce::String VIBDEPTH_mod[4]        { "Vib Depth1modulate","Vib Depth2modulate","Vib Depth3modulate","Vib Depth4modulate" };
    inline const juce::String VIBFREQ_mod[4]        { "Vib Freq1modulate","Vib Freq2modulate","Vib Freq3modulate","Vib Freq4modulate" };

    inline const juce::String RESOFF_mod[4]     { "Resonator Offset1modulate","Resonator Offset2modulate","Resonator Offset3modulate","Resonator Offset4modulate" };
    inline const juce::String RESFDB_mod[4]     { "Resonator Feedback1modulate","Resonator Feedback2modulate","Resonator Feedback3modulate","Resonator Feedback4modulate" };

    inline const juce::String OFFSTR_mod[4]     { "Offset Strength1modulate","Offset Strength2modulate","Offset Strength3modulate","Offset Strength4modulate" };

    inline const juce::String res_offset[4]     { "res_offset1","res_offset2","res_offset3","res_offset4" };

    inline const juce::String waveform[4]     { "WAVEFORM1","WAVEFORM2","WAVEFORM3","WAVEFORM4" };

    inline const juce::String mod_waveform[4]     { "MOD_WAVEFORM1","MOD_WAVEFORM2","MOD_WAVEFORM3","MOD_WAVEFORM4" };

    inline const juce::String res_feedback[4]     { "res_feedback1","res_feedback2","res_feedback3","res_feedback4" };
    inline const juce::String offset_strength[4]     { "res_offset_strength1","res_offset_strength2","res_offset_strength3","res_offset_strength4" };

    // Alias toggles per voice (your createParameters uses "1AliasToggle" .. "4AliasToggle")
    inline const juce::String aliasToggle[4] { "1AliasToggle","2AliasToggle","3AliasToggle","4AliasToggle" };

    // Sequencers (5) — CURRENT_STEP per seq
    inline const juce::String seqCurrentStep[5] {
        "seq1CURRENT_STEP","seq2CURRENT_STEP","seq3CURRENT_STEP","seq4CURRENT_STEP","seq5CURRENT_STEP"
    };
    
    inline const juce::String seqStepCount[5] {
    "seq1STEP_COUNT","seq2STEP_COUNT","seq3STEP_COUNT","seq4STEP_COUNT","seq5STEP_COUNT"
    };

    inline const juce::String seqRateIndex[5] {
    "seq1RATE","seq2RATE","seq3RATE","seq4RATE","seq5RATE"
    };

    // Sequencer steps (5×8) and offsets (5×8)
    inline const juce::String seqStep[5][8] = {
        { "seq1step0","seq1step1","seq1step2","seq1step3","seq1step4","seq1step5","seq1step6","seq1step7" },
        { "seq2step0","seq2step1","seq2step2","seq2step3","seq2step4","seq2step5","seq2step6","seq2step7" },
        { "seq3step0","seq3step1","seq3step2","seq3step3","seq3step4","seq3step5","seq3step6","seq3step7" },
        { "seq4step0","seq4step1","seq4step2","seq4step3","seq4step4","seq4step5","seq4step6","seq4step7" },
        { "seq5step0","seq5step1","seq5step2","seq5step3","seq5step4","seq5step5","seq5step6","seq5step7" }
    };

    inline const juce::String seqOffset[5][8] = {
        { "seq1offset0","seq1offset1","seq1offset2","seq1offset3","seq1offset4","seq1offset5","seq1offset6","seq1offset7" },
        { "seq2offset0","seq2offset1","seq2offset2","seq2offset3","seq2offset4","seq2offset5","seq2offset6","seq2offset7" },
        { "seq3offset0","seq3offset1","seq3offset2","seq3offset3","seq3offset4","seq3offset5","seq3offset6","seq3offset7" },
        { "seq4offset0","seq4offset1","seq4offset2","seq4offset3","seq4offset4","seq4offset5","seq4offset6","seq4offset7" },
        { "seq5offset0","seq5offset1","seq5offset2","seq5offset3","seq5offset4","seq5offset5","seq5offset6","seq5offset7" }
    };

    inline const juce::String MODMATRIX[4][4] = {
        { "mod_1_1", "mod_1_2", "mod_1_3", "mod_1_4" },
        { "mod_2_1", "mod_2_2", "mod_2_3", "mod_2_4" },
        { "mod_3_1", "mod_3_2", "mod_3_3", "mod_3_4" },
        { "mod_4_1", "mod_4_2", "mod_4_3", "mod_4_4" }
    };

}

//#pragma once
//#include <JuceHeader.h>
//
//// ===========================
//// Numeric groups and sizes
//// ===========================
//namespace PIDNum
//{
//    // Filter (global and per-voice)
//    inline constexpr int FILTER_FREQ_BASE = 500;  // filter_freq1..4
//    inline constexpr int FILTER_RES_BASE  = 600;  // filter_res1..4
//    inline constexpr int FILTER_COUNT     = 4;
//
//    // Wah (single)
//    inline constexpr int WAH_BASE         = 700;  // wahFreq, wahFeedback, wahDelay, WahUseSync, WahSyncRate
//    inline constexpr int WAH_COUNT        = 5;
//
//    // Per-voice simple controls
//    inline constexpr int PAN_BASE         = 800;  // PAN1..4
//    inline constexpr int DETUNE_BASE      = 820;  // DETUNE1..4
//    inline constexpr int MOD_INDEX_BASE   = 840;  // MOD_INDEX1..4
//    inline constexpr int DOWNSAMPLE_BASE  = 860;  // DOWNSAMPLE1..4
//    inline constexpr int VOICE_COUNT      = 4;
//
//    // Waveforms per voice (choices)
//    inline constexpr int MOD_WAVE_BASE    = 900;  // MOD_WAVEFORM1..4
//    inline constexpr int CARRIER_WAVE_BASE= 920;  // WAVEFORM1..4
//
//    // Sequencers (5 seq × 8 steps) — precomputed tables for zero concatenation at runtime
//    inline constexpr int SEQ_STEP_BASE    = 1000; // seq{j}step{i}
//    inline constexpr int SEQ_OFFS_BASE    = 1100; // seq{j}offset{i}
//    inline constexpr int SEQ_COUNT        = 5;
//    inline constexpr int STEPS_PER_SEQ    = 8;
//
//    // Global filter
//    inline constexpr int GLOBAL_FILTER_BASE = 2000; // res_filterfreq, filter_freq, filter_res, filter_env, filter_drive
//    inline constexpr int GLOBAL_FILTER_COUNT = 5;
//
//    // ADSRs (2 envelopes × 4 params each)
//    inline constexpr int ENV_BASE         = 2100; // attack0, decay0, sustain0, release0, attack1, ...
//    inline constexpr int ENV_ENVS         = 2;
//    inline constexpr int ENV_PARAMS       = 4;
//
//    // Example: “mod m -> car c” matrix (4×4)
//    inline constexpr int MODMATRIX_BASE   = 3000; // mod_1_1 .. mod_4_4
//    inline constexpr int MODMATRIX_DIM    = 4;
//}
//
//// ===========================
//// String tables (constexpr)
//// ===========================
//namespace PIDStr
//{
//    // filter_freq1..4
//    static constexpr const juce::String[] FILTER_FREQ[PIDNum::FILTER_COUNT] =
//        { "filter_freq1", "filter_freq2", "filter_freq3", "filter_freq4" };
//
//    static constexpr const juce::String[] FILTER_RES[PIDNum::FILTER_COUNT] =
//        { "filter_res1",  "filter_res2",  "filter_res3",  "filter_res4"  };
//
//    // per-voice
//    static constexpr const juce::String[] PAN[PIDNum::VOICE_COUNT] =
//        { "PAN1", "PAN2", "PAN3", "PAN4" };
//    static constexpr const juce::String[] DETUNE[PIDNum::VOICE_COUNT] =
//        { "DETUNE1", "DETUNE2", "DETUNE3", "DETUNE4" };
//    static constexpr const juce::String[] MOD_INDEX[PIDNum::VOICE_COUNT] =
//        { "MOD_INDEX1", "MOD_INDEX2", "MOD_INDEX3", "MOD_INDEX4" };
//    static constexpr const juce::String[] DOWNSAMPLE[PIDNum::VOICE_COUNT] =
//        { "DOWNSAMPLE1", "DOWNSAMPLE2", "DOWNSAMPLE3", "DOWNSAMPLE4" };
//
//    static constexpr const juce::String[] MOD_WAVEFORM[PIDNum::VOICE_COUNT] =
//        { "MOD_WAVEFORM1", "MOD_WAVEFORM2", "MOD_WAVEFORM3", "MOD_WAVEFORM4" };
//    static constexpr const char* CARRIER_WAVEFORM[PIDNum::VOICE_COUNT] =
//        { "WAVEFORM1", "WAVEFORM2", "WAVEFORM3", "WAVEFORM4" };
//
//    // Wah block (order fixed to match base+offset)
//    static constexpr const juce::String[] WAH[PIDNum::WAH_COUNT] =
//        { "wahFreq", "wahFeedback", "wahDelay", "WahUseSync", "WahSyncRate" };
//
//    // Global filter (order fixed)
//    static constexpr const juce::String[] GLOBAL_FILTER[PIDNum::GLOBAL_FILTER_COUNT] =
//        { "res_filterfreq", "filter_freq", "filter_res", "filter_env", "filter_drive" };
//
//    // ADSR names (attack, decay, sustain, release)
//    static constexpr const juce::String[] ENV_NAMES[PIDNum::ENV_PARAMS] =
//        { "attack", "decay", "sustain", "release" };
//
//    // Sequencers: prebuild names
//    // seq{1..5}step{0..7}
//    static constexpr const juce::String[] SEQ_STEPS[PIDNum::SEQ_COUNT][PIDNum::STEPS_PER_SEQ] = {
//        { "seq1step0","seq1step1","seq1step2","seq1step3","seq1step4","seq1step5","seq1step6","seq1step7" },
//        { "seq2step0","seq2step1","seq2step2","seq2step3","seq2step4","seq2step5","seq2step6","seq2step7" },
//        { "seq3step0","seq3step1","seq3step2","seq3step3","seq3step4","seq3step5","seq3step6","seq3step7" },
//        { "seq4step0","seq4step1","seq4step2","seq4step3","seq4step4","seq4step5","seq4step6","seq4step7" },
//        { "seq5step0","seq5step1","seq5step2","seq5step3","seq5step4","seq5step5","seq5step6","seq5step7" }
//    };
//
//    // seq{1..5}offset{0..7}
//    static constexpr const juce::String[] SEQ_OFFS[PIDNum::SEQ_COUNT][PIDNum::STEPS_PER_SEQ] = {
//        { "seq1offset0","seq1offset1","seq1offset2","seq1offset3","seq1offset4","seq1offset5","seq1offset6","seq1offset7" },
//        { "seq2offset0","seq2offset1","seq2offset2","seq2offset3","seq2offset4","seq2offset5","seq2offset6","seq2offset7" },
//        { "seq3offset0","seq3offset1","seq3offset2","seq3offset3","seq3offset4","seq3offset5","seq3offset6","seq3offset7" },
//        { "seq4offset0","seq4offset1","seq4offset2","seq4offset3","seq4offset4","seq4offset5","seq4offset6","seq4offset7" },
//        { "seq5offset0","seq5offset1","seq5offset2","seq5offset3","seq5offset4","seq5offset5","seq5offset6","seq5offset7" }
//    };
//
//    // Mod matrix: mod_m -> car_c (1..4)
//    static constexpr const juce::String[] MODMATRIX[PIDNum::MODMATRIX_DIM][PIDNum::MODMATRIX_DIM] = {
//        { "mod_1_1","mod_1_2","mod_1_3","mod_1_4" },
//        { "mod_2_1","mod_2_2","mod_2_3","mod_2_4" },
//        { "mod_3_1","mod_3_2","mod_3_3","mod_3_4" },
//        { "mod_4_1","mod_4_2","mod_4_3","mod_4_4" }
//    };
//}
//
//// ===========================
//// Numeric → string lookup
//// ===========================
//inline const juce::String[] paramIdToString (int numericId)
//{
//    using namespace PIDNum;
//    using namespace PIDStr;
//
//    // filter_freq
//    if (numericId >= FILTER_FREQ_BASE && numericId < FILTER_FREQ_BASE + FILTER_COUNT)
//        return FILTER_FREQ[numericId - FILTER_FREQ_BASE];
//
//    // filter_res
//    if (numericId >= FILTER_RES_BASE && numericId < FILTER_RES_BASE + FILTER_COUNT)
//        return FILTER_RES[numericId - FILTER_RES_BASE];
//
//    // per-voice PAN/DETUNE/MOD_INDEX/DOWNSAMPLE
//    if (numericId >= PAN_BASE && numericId < PAN_BASE + VOICE_COUNT)
//        return PAN[numericId - PAN_BASE];
//    if (numericId >= DETUNE_BASE && numericId < DETUNE_BASE + VOICE_COUNT)
//        return DETUNE[numericId - DETUNE_BASE];
//    if (numericId >= MOD_INDEX_BASE && numericId < MOD_INDEX_BASE + VOICE_COUNT)
//        return MOD_INDEX[numericId - MOD_INDEX_BASE];
//    if (numericId >= DOWNSAMPLE_BASE && numericId < DOWNSAMPLE_BASE + VOICE_COUNT)
//        return DOWNSAMPLE[numericId - DOWNSAMPLE_BASE];
//
//    if (numericId >= MOD_WAVE_BASE && numericId < MOD_WAVE_BASE + VOICE_COUNT)
//        return MOD_WAVEFORM[numericId - MOD_WAVE_BASE];
//    if (numericId >= CARRIER_WAVE_BASE && numericId < CARRIER_WAVE_BASE + VOICE_COUNT)
//        return CARRIER_WAVEFORM[numericId - CARRIER_WAVE_BASE];
//
//    // Wah
//    if (numericId >= WAH_BASE && numericId < WAH_BASE + WAH_COUNT)
//        return WAH[numericId - WAH_BASE];
//
//    // Global filter
//    if (numericId >= GLOBAL_FILTER_BASE && numericId < GLOBAL_FILTER_BASE + GLOBAL_FILTER_COUNT)
//        return GLOBAL_FILTER[numericId - GLOBAL_FILTER_BASE];
//
//    // ADSR: ENV_BASE + (env * 4 + p)
//    if (numericId >= ENV_BASE && numericId < ENV_BASE + ENV_ENVS * ENV_PARAMS)
//    {
//        const int idx = numericId - ENV_BASE;
//        const int p   = idx % ENV_PARAMS; // 0..3
//        const int e   = idx / ENV_PARAMS; // 0..(ENV_ENVS-1)
//        // Return "attack0"/"decay1" etc. We’ll build the number minimally here:
//        // (Only once at parameter creation time—never in the audio path.)
//        static thread_local juce::String tmp;
//        tmp = PIDStr::ENV_NAMES[p]; tmp += juce::String(e);
//        return tmp.toRawUTF8(); // note: only use immediately to construct ParameterID
//    }
//
//    // Sequencer steps/offsets: SEQ_STEP_BASE + (seq * 8 + step)
//    if (numericId >= SEQ_STEP_BASE && numericId < SEQ_STEP_BASE + SEQ_COUNT * STEPS_PER_SEQ)
//    {
//        const int idx = numericId - SEQ_STEP_BASE;
//        return PIDStr::SEQ_STEPS[idx / STEPS_PER_SEQ][idx % STEPS_PER_SEQ];
//    }
//    if (numericId >= SEQ_OFFS_BASE && numericId < SEQ_OFFS_BASE + SEQ_COUNT * STEPS_PER_SEQ)
//    {
//        const int idx = numericId - SEQ_OFFS_BASE;
//        return PIDStr::SEQ_OFFS[idx / STEPS_PER_SEQ][idx % STEPS_PER_SEQ];
//    }
//
//    // Mod matrix: MODMATRIX_BASE + (m*4 + c)
//    if (numericId >= MODMATRIX_BASE && numericId < MODMATRIX_BASE + MODMATRIX_DIM * MODMATRIX_DIM)
//    {
//        const int idx = numericId - MODMATRIX_BASE;
//        return PIDStr::MODMATRIX[idx / MODMATRIX_DIM][idx % MODMATRIX_DIM];
//    }
//
//    return nullptr; // unknown
//}
//
