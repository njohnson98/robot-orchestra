//-----------------------------------------------------
// Engineers: Nathan Johnson, James Springer
// Overview
//     When this device is turned on it plays the song Row
//     Row Row Your Boat.
// Design Name:   Spongebot Guitarpants
// File Name:     main.cc
//
// Inputs: 
//     Test Tempo - Determined by potentiometer
//     Octave - Determined by potentiometer
//     Mode - Selected with slider
//     Start - Push Button
//     Reset - Push Button
//     Audio Input - Conductor
// Outputs:
//     Sync Indicator - LED
//     Play Indicator - LED
//     Audio Output - Speaker
//     Servo Motors
//     Fan
//
// History:       30 September 2021:  File created
//                3 October 2021:     Arduino implementation
//                8 October 2021:     Fixed Arduino pin information
//-----------------------------------------------------

#include <math.h>
#include <Servo.h>
#include <stdio.h>

#include "dsp.h"

#define DEBUG

#define TempoCal 512
#define TempoPotMax 1023
#define PwmMax 255
#define TEMPO_MIN 30
#define TEMPO_MAX 500


// pinouts
#define SyncLED 7
#define PlayLED 6
#define TempoPot 14
#define OctaveSelectPot 15
#define ModeSelect 5
#define Fan 9
#define StartPB 4
#define ResetPB 3
#define Speaker 11
#define Mic 16


Servo strumServo;
Servo fretServo;

int mode;
int octave;
int tempo;
int duration;

int songLength = 54;
int beats[] = {2,1,2,1,2,1,1,1,2,1,2,1,1,1,2,1,1,1,6,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,1,1,2,1,1,1,5,1};
int songTempo = 250;

// set high and low servo values  
int strumLow = 50;
int strumHigh = 100;
int fretLow = 50;
int fretHigh = 100;


// called once on startup
void setup() {
    #ifdef DEBUG
        Serial.begin(9600);  // Set up serial port (only used for testing)
    #endif  // DEBUG

    // set up inputs	
	pinMode(TempoPot, INPUT);
	pinMode(OctaveSelectPot, INPUT);
	pinMode(ModeSelect, INPUT);
	pinMode(StartPB, INPUT);
	pinMode(ResetPB, INPUT);
    pinMode(Mic, INPUT);
  
    // set up outputs
    pinMode(SyncLED, OUTPUT);
	  pinMode(PlayLED, OUTPUT);
	  pinMode(Speaker, OUTPUT);
    pinMode(Fan, OUTPUT);
  
    strumServo.attach(9);
    fretServo.attach(10);

    // set initial motor and servo conditions
    strumServo.write(5);
    fretServo.write(5);
    digitalWrite(Fan, LOW);
    digitalWrite(Speaker, LOW);
    digitalWrite(SyncLED, LOW);
    digitalWrite(PlayLED, LOW);
}


// called repeatedly after setup
void loop() {
    // wait for start button to be pressed
    while (!digitalRead(StartPB)) {}

    // set the octave based on range of potentiometer value
    //octave = ( int(analogRead(OctaveSelectPot)) / (10000 / 8) ) + 1;
    octave = analogRead(OctaveSelectPot) < 512 ? 4 : 6;
    #ifdef DEBUG
        Serial.print("Octave: ");
        Serial.println(octave);
    #endif  // DEBUG

    int start_index = 0;
    // read mode switch
    mode = digitalRead(ModeSelect);

    digitalWrite(SyncLED, HIGH);

    // execute code for current mode
    if (mode == 0) {  // play mode
        #ifdef DEBUG
            Serial.print("Starting tempo calc\n");
        #endif  // DEBUG
        
        bool notes_present[TOTAL_TOGGLES];
        unsigned long timestamps[TOTAL_TOGGLES];
        tempo = dsp::findTempo(Mic, notes_present, timestamps);
        #ifdef DEBUG
            Serial.print("Calculated Tempo (ms): ");
            Serial.println(tempo) ;
        #endif  // DEBUG 
  
        start_index = dsp::findTimeOffset(tempo, notes_present, timestamps, beats);
        #ifdef DEBUG
            Serial.print("Calculated Start Index: ");
            Serial.println(start_index);
        #endif  // DEBUG
    } else {  // test mode
        // read tempo potentiometer and set tempo
        int tempoPot = analogRead(TempoPot);
        tempo = int(float(songTempo) * float(tempoPot) / float(TempoCal));
        tempo = (tempo < TEMPO_MIN) ? TEMPO_MIN : tempo;
        tempo = (tempo > TEMPO_MAX) ? TEMPO_MAX : tempo;
    }
    
    playSong(tempo, start_index);
}


// start index is index of first note to play in beats
void playSong(int tempo, int start_index) {
    #ifdef DEBUG
        Serial.print("TEMPO: ");
        Serial.print(tempo);
        Serial.print(", START INDEX: ");
        Serial.println(start_index);
    #endif  // DEBUG
    // calculate note values based on octave
    int C = 16.3516 * pow(2, octave);
    int D = 18.35405 * pow(2, octave);
    int E = 20.60172 * pow(2, octave);
    int F = 21.82676 * pow(2, octave);
    int G = 24.49971 * pow(2, octave);
    int A = 27.5 * pow(2, octave);
    int B = 30.86771 * pow(2, octave);
    int high_C = 32.70320 * pow(2, octave);
    int rest = 0;

    int notes[] = {C, rest, C, rest, C, rest, D, rest, E, rest, E, rest, D, rest, E, rest, F, rest, G, rest, high_C, rest, high_C, rest, high_C, rest, G, rest, G, rest, G, rest, E, rest, E, rest, E, rest, C, rest, C, rest, C, rest, G, rest, F, rest, E, rest, D, rest, C, rest};

    digitalWrite(SyncLED, LOW);
    digitalWrite(PlayLED, HIGH);

    // play song until stop button is pressed
    int i = (start_index < songLength && start_index >= 0) ? start_index : 0;  // make sure start_index is valid
    while (!digitalRead(ResetPB)) {
        // play current note
        duration = beats[i] * tempo;
        tone(Speaker, notes[i], duration);
        delay(duration);

        // move servos to opposite position if note is not a rest
        if (notes[i] != rest) {
            moveServos();
        }

        // increment index
        ++i;
        if (i >= songLength) {
            i = 0;
        }
    }

    // reset speaker and LEDs
    noTone(Speaker);
    digitalWrite(Speaker, LOW);
    digitalWrite(SyncLED, LOW);
    digitalWrite(PlayLED, LOW);   
}


void moveServos() {
    if (strumServo.read() == strumHigh) {
        strumServo.write(strumLow);
    } else {
        strumServo.write(strumHigh);
    }

    if (fretServo.read() == fretHigh) {
        fretServo.write(fretLow);
    } else {
        fretServo.write(fretHigh);
    }
}
