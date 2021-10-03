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

#define speakerPIN 5
#define fanPIN 6
#define syncLEDPIN 7
#define playLEDPIN 8

Servo strumServo;
Servo fretServo;

int mode;
int octave;
int tempo;

int songLength = 54;
int beats[] = {2,1,2,1,2,1,1,1,2,1,2,1,1,1,2,1,1,1,6,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,1,1,2,1,1,1,5,1};
songTempo = 250;


// called once on startup
void setup() {

    // set up inputs
    pinMode(A0, INPUT);  // mode switch
    pinMode(A1, INPUT);  // tempo potentiometer
    pinMode(A2, INPUT);  // octave potentiometer
    pinMode(A3, INPUT);  // start/stop button
    pinMode(A4, INPUT);  // microphone

    // set up outputs
    pinMode(speakerPIN, OUTPUT);
    pinMode(fanPIN, OUTPUT);
    pinMode(syncLEDPIN, OUTPUT);
    pinMode(playLEDPIN, OUTPUT);

    strumServo.attach(9);
    fretServo.attach(10);

    // set initial motor and servo conditions
    digitalWrite(motorPIN, 0);
    strumServo.write(5);
    fretServo.write(5);

    digitalWrite(syncLEDPIN, LOW);
    digitalWrite(playLEDPIN, LOW);

}


// called repeatedly after setup
void loop() {

    // wait for start button to be pressed
    while (!digitalRead(A3)) {}

    readOctave();

    // read mode switch
    mode = digitalRead(A0);

    digitalWrite(syncLEDPIN, HIGH);

    // execute code for current mode
    if (mode == 0) {
        tempoSync();
        timingSync();
    } else if (mode == "test") {
        // read tempo potentiometer and set tempo
        tempoPot = analogRead(A1);
        tempo = songTempo * float(tempoPot) / TempoCal;
    }

    playSong();

}


void readOctave() {
    // read octave value from potentiometer
    octave = 4;
}


void tempoSync() {
    // tempo sync
    tempo = 200;
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

    digitalWrite(syncLEDPIN, LOW);
    digitalWrite(playLEDPIN, HIGH);

    // play song until stop button is pressed
    int i = 0;
    while (!digitalRead(A3)) {
        // play song
        duration = beats[i] * tempo;
        tone(speakerPIN, notes[i], duration);
        delay(duration);

        // increment index
        i++;
        if (i >= songLength) {
            i = 0;
        }
    }

    // reset fan and LEDs
    noTone(fanPIN);
    digitalWrite(syncLEDPIN, LOW);
    digitalWrite(playLEDPIN, LOW);
        
}
