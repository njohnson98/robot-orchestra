//-----------------------------------------------------
// Engineer: Nathan Johnson
// Overview
//    When this device is turned on it plays the song Row
//    Row Row Your Boat.
// Design Name:   Spongebot Guitarpants
// File Name:     main.cc
//
// Inputs: 
// Outputs: 
//
// History:       30 September 2021 File created
//-----------------------------------------------------


#include <stdio.h>
#include <math.h>

int mode;
int octave;
int manualTempo;


int main() {

    setup();
    startRobot();

}


void setup() {
    // do setup
}


void startRobot() {

    // fix condition
    if (mode == "conductor") {
        syncedTempo = tempoSync();
        timingSync();
        playSong(syncedTempo);
    } else if (mode == "test") {
        playSong(manualTempo);
    }

}


void tempoSync() {
    // tempo sync
}


void timingSync() {
    // timing sync
}


void playSong(int tempo) {

    int C = 16.3516 * pow(2, octave);
    int D = 18.35405 * pow(2, octave;
    int E = 20.60172 * pow(2, octave;
    int F = 21.82676 * pow(2, octave;
    int G = 24.49971 * pow(2, octave;
    int A = 27.5 * pow(2, octave);
    int B = 30.86771 * pow(2, octave);
    int high_C = 32.70320 * pow(2, octave);
    int rest = 0;

    int songLength = 54;  
    int notes[] = {C, rest, C, rest, C, rest, D, rest, E, rest, E, rest, D, rest, E, rest, F, rest, G, rest, high_C, rest, high_C, rest, high_C, rest, G, rest, G, rest, G, rest, E, rest, E, rest, E, rest, C, rest, C, rest, C, rest, G, rest, F, rest, E, rest, D, rest, C, rest};
    int beats[] = {2,1,2,1,2,1,1,1,2,1,2,1,1,1,2,1,1,1,6,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,1,1,2,1,1,1,5,1};

    // read value of stop button for while condition
    while (true) {
        // play song (example in conductor file)
    }
        
}
