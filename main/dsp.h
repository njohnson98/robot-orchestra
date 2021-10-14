#ifndef DSP_
#define DSP_

namespace dsp {

    float findTempo(int mic);

}  // namespace dsp



// bool is_note_present(float* buf, int buf_len);
// float find_tempo(bool* digital_env, int buf_len, float* timestamps);  // digital_env and timestamps buf_len must be same
// int* find_note_lengths(bool* digital_env, int buf_len);
// float find_time_offset(int* note_lengths, int buf_len);  // add someway to get timestamps here


// // take ~20 samples, pass them into function
// bool is_note_present(float* buf, int buf_len) {
//     float sum = 0;
//     for (int i = 0; i < buf_len; ++i) {
//         sum += buf[i];
//     }

//     sum /= buf_len;
//     float threshold = .5;  // TODO(James): Experiment with threshold
//     if (sum > threshold) {
//         return true;
//     } else {
//         return false;
//     }
// }


// float find_tempo(bool* digital_env, int buf_len, float* timestamps) {
//     // find toggle indices
//     int toggle_indices[25];
//     int i = 0;
//     int toggle_index = 0;
//     while (i < buf_len - 3 && toggle_index < 25) {
//         bool cur_state = digital_env[i];
//         if (digital_env[i+1] != cur_state && digital_env[i+2] != cur_state && digital_env[i+3] == !cur_state) {
//             toggle_indices[toggle_index] = i;  // is a toggle
//             ++toggle_index;
//         }
//         ++i;
//     }

//     for (int j = 0; j < toggle_index - 2; ++j) {  // don't include last toggle (probably not full note)
//         float tempo;
//         int num_rests = 0;
//         if (digital_env[toggle_indices[j]+1] == false) {  // is a rest
//             tempo += timestamps[toggle_indices[j+1]] - timestamps[toggle_indices[j]];
//             num_rests += 1;
//         }

//         return tempo/num_rests;
//     }  // TODO(James): Also need to return the toggle_indices somehow



#endif  // DSP_