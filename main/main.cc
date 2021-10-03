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
// History:       30 September 2021:  File created
//                3 October 2021:     Arduino Implementation
//-----------------------------------------------------


#include <stdio.h>
#include <math.h>
#include <Servo.h>

#define TempoCal 512
#define TempoPotMax 1023
#define PwmMax 255

#define speakerPIN 4
#define fretMotorPIN 5
#define strumMotorPIN 6
#define syncLEDPIN 7
#define playLEDPIN 8

int mode;
int octave;
int tempo;

int songLength = 54;
int beats[] = {2,1,2,1,2,1,1,1,2,1,2,1,1,1,2,1,1,1,6,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,1,1,2,1,1,1,5,1};

Servo servo_9;
Servo servo_10;


// called once on startup
void setup() {

    // set up inputs
    pinMode(A0, INPUT);  // mode switch
    pinMode(A1, INPUT);  // tempo potentiometer
    pinMode(A2, INPUT);  // octave potentiometer
    pinMode(A3, INPUT);  // start/stop button

    // set up outputs
    pinMode(speakerPIN, OUTPUT);
    pinMode(fretMotorPIN, OUTPUT);
    pinMode(strumMotorPIN, OUTPUT);
    pinMode(syncLEDPIN, OUTPUT);
    pinMode(playLEDPIN, OUTPUT);

    servo_9.attach(9);
    servo_10.attach(10);

    // set initial motor and servo conditions
    digitalWrite(motorPIN, 0);
    servo_9.write(5);
    servo_10.write(5);

}


// called repeatedly after setup
void loop() {

    // wait for start button to be pressed
    while (!digitalRead(A3)) {}

    readOctave();

    // read mode switch
    mode = 0;

    // fix condition
    if (mode == "conductor") {
        tempo = tempoSync();
        timingSync();
        playSong();
    } else if (mode == "test") {
        // read tempo potentiometer
        tempo = 5;
        playSong();
    }

}


void readOctave() {
    // read octave value from potentiometer
    octave = 4;
}


void tempoSync() {
    // tempo sync
}


void timingSync() {
    // timing sync
}


void playSong(int tempo) {

    // calculate note values based on octave
    int C = 16.3516 * pow(2, octave);
    int D = 18.35405 * pow(2, octave;
    int E = 20.60172 * pow(2, octave;
    int F = 21.82676 * pow(2, octave;
    int G = 24.49971 * pow(2, octave;
    int A = 27.5 * pow(2, octave);
    int B = 30.86771 * pow(2, octave);
    int high_C = 32.70320 * pow(2, octave);
    int rest = 0;

    int notes[] = {C, rest, C, rest, C, rest, D, rest, E, rest, E, rest, D, rest, E, rest, F, rest, G, rest, high_C, rest, high_C, rest, high_C, rest, G, rest, G, rest, G, rest, E, rest, E, rest, E, rest, C, rest, C, rest, C, rest, G, rest, F, rest, E, rest, D, rest, C, rest};

    // read value of stop button for while condition
    while (true) {
        // play song (example in conductor file)
    }
        
}
