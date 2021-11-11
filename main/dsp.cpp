#include "dsp.h"

#include "Arduino.h"

#define DOWNSAMPLE_RATE 40  // number of samples to average in digital low pass filter
#define NOTE_THRESHOLD .15   // threshold to define whether note prestent or not
#define MIC_OFFSET 0 // mic offset is 0 now after the filter  --> mic offset before: 1023.0*5.0/5.0/2.0
#define CONSEQ_NOTES 4

namespace dsp {

    int findTempo(int mic, bool notes_present[TOTAL_TOGGLES], unsigned long timestamps[TOTAL_TOGGLES]) {
        int numNotes = -1;  // start at negative 1 to get initial state (note present or not)
        // unsigned long timestamps[TOTAL_TOGGLES];
        // bool notes_present[TOTAL_TOGGLES];
        bool prevNotePresent = false;  // used to compare toggle, not used in first note find
        for (int i = 0; i < 100; ++i) {
            analogRead(mic);  // clear out to make sure we're getting good readings
        }

        int micCal = 0;
        unsigned long stopTime = millis() + 1000;  // 1 second to calibrate
        while (millis() < stopTime) {
            int micVal = analogRead(mic);
            micCal = (micVal > micCal) ? micVal : micCal;
        }
        micCal = 100;

        #ifdef DEBUG
            Serial.print("Mic Cal: ");
            Serial.println(micCal);
        #endif  // DEBUG

        int noteThreshold = int(micCal * NOTE_THRESHOLD);
        unsigned long tempTimestamp = 0;
        uint8_t numConseq = 0;  // used to make sure we don't get one false reading that indicates we have a note, must have configurable consequitive amount
        while (numNotes < TOTAL_TOGGLES) {
            float noteAvg = 0;
            for (int i = 0; i < DOWNSAMPLE_RATE; ++i) {
                int noteVal = analogRead(mic) - MIC_OFFSET;
                noteAvg += float(abs(noteVal)) / float(DOWNSAMPLE_RATE);
            }

            bool notePresent = noteAvg > noteThreshold;  // true if above threshold, false otherwise
            if (numNotes >= 0 && notePresent != prevNotePresent) {
                if (numConseq < CONSEQ_NOTES) {
                    ++numConseq;
                    tempTimestamp = millis();  // will be used for timestamp for next if following note is conseq
                } else {
                    notes_present[numNotes] = notePresent;
                    timestamps[numNotes] = tempTimestamp;
                    #ifdef DEBUG
                        Serial.print(notePresent);
                        Serial.print("\t\t");
                        Serial.println(timestamps[numNotes]);
                    #endif  // DEBUG
                    ++numNotes;
                    prevNotePresent = notePresent;
                    numConseq = 0;
                }
                
            } else if (numNotes < 0) {
                prevNotePresent = notePresent;
                ++numNotes;
                #ifdef DEBUG
                    Serial.println("Waiting for first toggle...");
                #endif  // DEBUG
            } else {
                numConseq = 0;
            }
        }

        int tempo = 0;
        int numRests = 0;
        for (int i = 0; i < TOTAL_TOGGLES-3; ++i) {
            if (!notes_present[i]) {
                if (timestamps[i+1] - timestamps[i] > 35) {
                    #ifdef DEBUG
                        Serial.println(timestamps[i+1] - timestamps[i]);
                    #endif // DEBUG
                    tempo += int(timestamps[i+1] - timestamps[i]);  // is a rest, add to calc tempo
                    ++numRests;
                }
            }
        }

        return int(float(tempo) / float(numRests));
    }


    int findTimeOffset(int tempo, bool notes_present[TOTAL_TOGGLES], unsigned long timestamps[TOTAL_TOGGLES], int beats[SONG_LENGTH]) {
        uint8_t note_length = 0;
        unsigned long note_timestamp = 0;
        for (int i = 0; i < TOTAL_TOGGLES - 1; i++) {
            note_length = round(float(timestamps[i+1]-timestamps[i])/float(tempo));
            #ifdef DEBUG
                Serial.print(note_length);
                Serial.print(" ");
            #endif  // DEBUG
            if (note_length == 5 || note_length == 6) {
                note_timestamp = timestamps[i];
                break;
            }

            if (i == TOTAL_TOGGLES - 2) {
                #ifdef DEBUG
                    Serial.println("Failed to find time offset, playing song from beginning");
                #endif  // DEBUG
                return 0;
            }
        }

        int match_index = 0;
        for (int i = 0; i < SONG_LENGTH; i++) {
            if (note_length == beats[i]) {
                match_index = i;
                Serial.print("Match Index: ");
                Serial.println(match_index);
                break;
            }
        }

        int dt = millis() - note_timestamp;
        while (dt > 0) {
            dt -= tempo * beats[match_index];  // TODO(James): Make tempo int
            ++match_index;
            if (match_index > SONG_LENGTH - 1) {
                match_index = 0;
            }
        }

        delay(abs(dt));
        return match_index;
    }

}  // namespace dsp
