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


// pinouts
#define SyncLED 7
#define PlayLED 6
#define TempoPot 14
#define OctaveSelectPot 15
#define ModeSelect 5
#define Fan 9
#define StartPB 3
#define ResetPB 4
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
    octave = 5;
    // read mode switch
    mode = digitalRead(ModeSelect);

    digitalWrite(SyncLED, HIGH);

    // execute code for current mode
    if (mode == 0) {  // play mode
        #ifdef DEBUG
            Serial.print("Starting tempo calc\n");
        #endif  // DEBUG
        
        float calc_tempo = dsp::findTempo(Mic);

        #ifdef DEBUG
            Serial.print("Calculated Tempo (ms): ");
            Serial.print(calc_tempo) ;
        #endif  // DEBUG 

        tempo = int(calc_tempo);     
        timingSync();
    } else {  // test mode
        // read tempo potentiometer and set tempo
        int tempoPot = analogRead(TempoPot);
        tempo = songTempo * float(tempoPot) / TempoCal;
    }

    playSong(tempo);
}


void tempoSync() {
    // tempo sync
  	digitalWrite(SyncLED, HIGH);
    tempo = 200;
}


void timingSync() {
    // timing sync
    digitalWrite(SyncLED, HIGH);
    delay(2000);
}


void playSong(int tempo) {
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
    int i = 0;
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
