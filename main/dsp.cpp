#include "dsp.h"

#include "Arduino.h"

#define TOTAL_TOGGLES 24    // number of notes to record before calculating tempo
#define DOWNSAMPLE_RATE 55  // number of samples to average in digital low pass filter
#define NOTE_THRESHOLD 15   // threshold to define whether note prestent or not
#define MIC_OFFSET 1023.0*3.3/5.0/2.0  // mic offset for 3.3V input to mic

namespace dsp {

    float findTempo(int mic) {
        int numNotes = -1;  // start at negative 1 to get initial state (note present or not)
        unsigned long timestamps[TOTAL_TOGGLES];
        bool notesPresent[TOTAL_TOGGLES];
        bool prevNotePresent = false;  // used to compare toggle, not used in first note find
        for (int i = 0; i < 100; ++i) {
            analogRead(mic);  // clear out to make sure we're getting good readings
        }

        unsigned long tempTimestamp = 0;
        bool isConseq = false;  // used to make sure we don't get one false reading that indicates we have a note, must get 2 consecqutive
        while (numNotes < TOTAL_TOGGLES) {
            float noteAvg = 0;
            for (int i = 0; i < DOWNSAMPLE_RATE; ++i) {
                int noteVal = analogRead(mic) - MIC_OFFSET;
                noteAvg += float(abs(noteVal)) / float(DOWNSAMPLE_RATE);
            }

            bool notePresent = noteAvg > NOTE_THRESHOLD;  // true if above threshold, false otherwise
            if (numNotes >= 0 && notePresent != prevNotePresent) {
                if (!isConseq) {
                    isConseq = true;
                    tempTimestamp = millis();  // will be used for timestamp for next if following note is conseq
                } else if (isConseq) {
                    notesPresent[numNotes] = notePresent;
                    timestamps[numNotes] = tempTimestamp;
                    #ifdef DEBUG
                        Serial.print(notePresent);
                        Serial.print("\t\t");
                        Serial.println(timestamps[numNotes]);
                    #endif  // DEBUG
                    ++numNotes;
                    prevNotePresent = notePresent;
                }
                
            } else if (numNotes < 0) {
                prevNotePresent = notePresent;
                ++numNotes;
                #ifdef DEBUG
                    Serial.println("Waiting for first toggle...");
                #endif  // DEBUG
            } else {
                isConseq = false;
            }
        }

        int tempo = 0;
        int numRests = 0;
        for (int i = 0; i < TOTAL_TOGGLES-3; ++i) {
            if (!notesPresent[i]) {
                if (timestamps[i+1] - timestamps[i] > 20) {
                    #ifdef DEBUG
                        Serial.println(timestamps[i+1] - timestamps[i]);
                    #endif // DEBUG
                    tempo += int(timestamps[i+1] - timestamps[i]);  // is a rest, add to calc tempo
                    ++numRests;
                }
            }
        }

        return float(tempo) / float(numRests);
    }

}  // namespace dsp
