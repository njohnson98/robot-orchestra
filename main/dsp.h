#ifndef DSP_
#define DSP_

#define DEBUG
#define TOTAL_TOGGLES 34    // number of notes to record before calculating tempo
#define SONG_LENGTH 54

namespace dsp {

    int findTempo(int mic, bool notes_present[TOTAL_TOGGLES], unsigned long timestamps[TOTAL_TOGGLES]);
    int findTimeOffset(int tempo, bool notes_present[TOTAL_TOGGLES], unsigned long timestamps[TOTAL_TOGGLES], int beats[SONG_LENGTH]);

}  // namespace dsp

#endif  // DSP_
