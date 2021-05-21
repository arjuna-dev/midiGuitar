#include <Dictionary.h>
#include "MIDIUSB.h"

// Teensy 4.1 board pins
// #define numberOfFrets 16
// #define numberOfStrings 6
// #define numberOfCoordinates numberOfFrets *numberOfStrings

// int inputString1 = 0;
// int inputString2 = 1;
// int inputString3 = 2;
// int inputString4 = 3;
// int inputString5 = 4;
// int inputString6 = 5;
// int fretPins[numberOfFrets] = {6, 7, 8, 9, 10, 11, 12, 24, 25, 26, 27, 28, 29, 30, 31, 32};

// Arduino Leonardo board pins
#define numberOfFrets 14
#define numberOfStrings 6
#define numberOfCoordinates numberOfStrings * numberOfFrets

int inputString1 = A5;
int inputString2 = A4;
int inputString3 = A3;
int inputString4 = A2;
int inputString5 = A1;
int inputString6 = A0;
int fretPins[numberOfFrets]= {7, 6, 5, 4, 3, 2, 1, 0, 14, 15, 16, 17, 18, 19};


// Arduino Mega ATmega2560 board pins
// #define numberOfFrets 16
// #define numberOfStrings 6
// #define numberOfCoordinates numberOfStrings * numberOfFrets

// int inputString1 = 8;
// int inputString2 = 9;
// int inputString3 = 10;
// int inputString4 = 11;
// int inputString5 = 12;
// int inputString6 = 13;
// int fretPins[numberOfFrets]= {7, 6, 5, 4, 3, 2, 1, 0, 14, 15, 16, 17, 18, 19, 20, 21};

int stringValues[numberOfStrings] = {0, 0, 0, 0, 0, 0};

String noteCoordinates[numberOfCoordinates];
int nowPlayingCoords[6];
int tempNowPlayingCoords[6];

Dictionary &d = *(new Dictionary(numberOfCoordinates));

void setup()
{

  Serial.begin(115200);

  // Populate array with coordinates
  int coordinateCounter = 0;
  for (int i = 0; i < numberOfStrings; i++)
  {
    for (int j = 0; j < numberOfFrets; j++)
    {
      String x = String(j + 1);
      String y = String(i + 1);
      String xy = x + y;
      noteCoordinates[coordinateCounter] = xy;
      coordinateCounter++;
    }
  }

  // Populate dictionary with coordinates and MIDI values per guitar string
  int j = 0;
  for (int i = 0; i < numberOfFrets; i++)
  {
    d(noteCoordinates[j], String(41 + i));
    j++;
  }
  for (int i = 0; i < numberOfFrets; i++)
  {
    d(noteCoordinates[j], String(46 + i));
    j++;
  }
  for (int i = 0; i < numberOfFrets; i++)
  {
    d(noteCoordinates[j], String(51 + i));
    j++;
  }
  for (int i = 0; i < numberOfFrets; i++)
  {
    d(noteCoordinates[j], String(56 + i));
    j++;
  }
  for (int i = 0; i < numberOfFrets; i++)
  {
    d(noteCoordinates[j], String(60 + i));
    j++;
  }
  for (int i = 0; i < numberOfFrets; i++)
  {
    d(noteCoordinates[j], String(65 + i));
    j++;
  }

  //Make fret pins inputs
  for (int i = 0; i < numberOfFrets; i++)
  {
    pinMode(fretPins[i], INPUT);
  }

  //Turn on board LED (jsut to make sure it is running)
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);

  //Make guitar string pins inputs
  pinMode(inputString1, INPUT);
  pinMode(inputString2, INPUT);
  pinMode(inputString3, INPUT);
  pinMode(inputString4, INPUT);
  pinMode(inputString5, INPUT);
  pinMode(inputString6, INPUT);
}

// First parameter is the event type (0x0B = control change).
// Second parameter is the event type, combined with the channel.
// Third parameter is the control number number (0-119).
// Fourth parameter is the control value (0-127).

void controlChange(byte channel, byte control, byte value)
{
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}

//MIDI note on
void noteOn(byte channel, byte pitch, byte velocity)
{
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

//MIDI note off
void noteOff(byte channel, byte pitch, byte velocity)
{
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

//Function to update values of strings
void updateStrings(int fretNumber)
{
  // (Could be done in a for loop by putting both
  // inputString and valString in arrays)
  if (digitalRead(inputString1))
  {
    stringValues[0] = fretNumber + 1;
  }

  if (digitalRead(inputString2))
  {
    stringValues[1] = fretNumber + 1;
  }

  if (digitalRead(inputString3))
  {
    stringValues[2] = fretNumber + 1;
  }

  if (digitalRead(inputString4))
  {
    stringValues[3] = fretNumber + 1;
  }

  if (digitalRead(inputString5))
  {
    stringValues[4] = fretNumber + 1;
  }

  if (digitalRead(inputString6))
  {
    stringValues[5] = fretNumber + 1;
  }
}

void loop()
{

  //unsigned long start = micros();

  // Reset string values
  // (Do in for loop)
  stringValues[0] = 0;
  stringValues[1] = 0;
  stringValues[2] = 0;
  stringValues[3] = 0;
  stringValues[4] = 0;
  stringValues[5] = 0;

  // Pass current through each fret, one at a time
  // and check current for each string

  for (int i = 0; i < numberOfFrets; i++)
  {
    // if (i == 0)
    // {
    //   pinMode(fretPins[numberOfFrets - 1], INPUT);
    // }
    // else
    // {
    //   pinMode(fretPins[i - 1], INPUT);
    // }
    pinMode(fretPins[i], OUTPUT);
    digitalWrite(fretPins[i], HIGH);
    updateStrings(i);
    pinMode(fretPins[i], INPUT);
  }

  //    unsigned long end1 = micros();
  //    unsigned long delta1 = end1 - start;

  // Use coordintates as keys to retrieve MIDI values from dictionary and store as ints for MIDI use
  for (int i = 0; i < 6; i++)
  {
    String x = String(stringValues[i]);
    String y = String(i + 1);
    String xy = x + y;
    if (d(xy))
    {
      nowPlayingCoords[i] = d[xy].toInt();
    }
    else
    {
      nowPlayingCoords[i] = 0;
    }
  }

  //  unsigned long end2 = micros();
  //  unsigned long delta2 = end2 - start;

  // To make it polyphonic skip the loop and play all at the same time
  for (int i = 0; i < 6; i++)
  {

    // Was not a note, now is a note
    if (!tempNowPlayingCoords[i] && nowPlayingCoords[i])
    {
      noteOn(0, nowPlayingCoords[i], 120); // Channel, Note, Velocity
      MidiUSB.flush();

      Serial.print("Note Played: ");
      Serial.println(nowPlayingCoords[i]);
    }

    // Was a note, and new one is a different note or no note
    if (tempNowPlayingCoords[i] && tempNowPlayingCoords[i] != nowPlayingCoords[i])
    {
      noteOff(0, tempNowPlayingCoords[i], 120); // Channel, Note, Velocity
      MidiUSB.flush();

      Serial.print("Note Off: ");
      Serial.println(nowPlayingCoords[i]);
      // Play note if new one is a note
      if (nowPlayingCoords[i])
      {
        noteOn(0, nowPlayingCoords[i], 120); // Channel, Note, Velocity
        MidiUSB.flush();

        Serial.print("Note Played: ");
        Serial.println(nowPlayingCoords[i]);
      }
    }

    tempNowPlayingCoords[i] = nowPlayingCoords[i];
  }
  // delay(50);
}