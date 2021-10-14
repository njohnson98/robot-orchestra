#include "dsp.h"

#include "Arduino.h"

// number of notes to record before calculating tempo
#define TOTAL_TOGGLES 24
#define DOWNSAMPLE_RATE 55
#define NOTE_THRESHOLD 15

namespace dsp {

    float findTempo(int mic) {
        int numNotes = -1;  // start at negative 1 to get initial state
        unsigned long timestamps[TOTAL_TOGGLES];
        bool notesPresent[TOTAL_TOGGLES];
        bool prevNotePresent = false;  // used to compare toggle, not used in first note find
        for (int i = 0; i < 100; ++i) {
          analogRead(mic);  // clear out to make sure we're getting good readings
        }

        unsigned long tempTimestamp = 0;
        bool isConseq = false;
        while (numNotes < TOTAL_TOGGLES) {
            float noteAvg = 0;
            for (int i = 0; i < DOWNSAMPLE_RATE; ++i) {
                int noteVal = analogRead(mic) - 1023.0*3.3/5.0/2.0;
                noteAvg += float(abs(noteVal)) / float(DOWNSAMPLE_RATE);
            }

            bool notePresent = noteAvg > NOTE_THRESHOLD;  // true if above threshold, false otherwise
            if (numNotes >= 0 && notePresent != prevNotePresent) {
                // check if consecutive
                // if not, record timestamp
                if (!isConseq) {
                    isConseq = true;
                    tempTimestamp = millis();
                } else if (isConseq) {
                    notesPresent[numNotes] = notePresent;
                    timestamps[numNotes] = tempTimestamp;
                    Serial.print(notePresent);
                    Serial.print("\t\t");
                    Serial.println(timestamps[numNotes]);
                    ++numNotes;
                    prevNotePresent = notePresent;
                }
                
            } else if (numNotes < 0) {
                Serial.println("First Toggle Detected");
                prevNotePresent = notePresent;
                ++numNotes;
            } else {
                isConseq = false;
            }
        }

        unsigned long tempo = 0;
        int numRests = 0;
        for (int i = 0; i < TOTAL_TOGGLES-3; ++i) {
            if (!notesPresent[i]) {
                if (timestamps[i+1] - timestamps[i] > 20) {
                  Serial.println(timestamps[i+1] - timestamps[i]);
                  tempo += timestamps[i+1] - timestamps[i]; // is a rest, add to calc tempo
                  ++numRests;
                }
            }
        }

        return float(tempo) / float(numRests);
    }


}  // namespace dsp
