#define NOTE_E6  1319
#define NOTE_G6  1568
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_D7  2349
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_G7  3136
#define NOTE_A7  3520
#define NOTE_B7  3951
#define NOTE_C8  4186
#define melodyPin 3
int song = 0;

int melodyMario[] = {
  NOTE_E7, NOTE_E7, 0, NOTE_E7,
  0, NOTE_C7, NOTE_E7, 0,
  NOTE_G7, 0, 0,  0,
  NOTE_G6, 0, 0, 0,

  NOTE_C7, 0, 0, NOTE_G6,
  0, 0, NOTE_E6, 0,
  0, NOTE_A6, 0, NOTE_B6,
  0, NOTE_AS6, NOTE_A6, 0,

  NOTE_G6, NOTE_E7, NOTE_G7,
  NOTE_A7, 0, NOTE_F7, NOTE_G7,
  0, NOTE_E7, 0, NOTE_C7,
  NOTE_D7, NOTE_B6, 0, 0,

  NOTE_C7, 0, 0, NOTE_G6,
  0, 0, NOTE_E6, 0,
  0, NOTE_A6, 0, NOTE_B6,
  0, NOTE_AS6, NOTE_A6, 0,

  NOTE_G6, NOTE_E7, NOTE_G7,//
  NOTE_A7, 0, NOTE_F7, NOTE_G7,
  0, NOTE_E7, 0, NOTE_C7,
  NOTE_D7, NOTE_B6, 0, 0
};

int tempoMario[] = {
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 15,

  9, 9, 9,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  9, 9, 9,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
};

int melodyMarioMini[] = {
  NOTE_E7, NOTE_E7, 0, NOTE_E7,
  0, NOTE_C7, NOTE_E7, 0,
  NOTE_G7, 0, 0,  0,
  NOTE_G6, 0, 0, 0,
};

int tempoMarioMini[] = {
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
};

void tuneMario(int s) {
  song = s;
  if (song == 1) {
    int size = sizeof(melodyMarioMini) / sizeof(int);

    for (int thisNote = 0; thisNote < size; thisNote++) {

      // to calculate the note duration, take one second
      // divided by the note type.
      //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
      int noteDuration = 1000 / tempoMario[thisNote];

      buzz(melodyPin, melodyMarioMini[thisNote], noteDuration);

      // to distinguish the notes, set a minimum time between them.
      // the note's duration + 30% seems to work well:
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);

      // stop the tone playing:
      buzz(melodyPin, 0, noteDuration);
    }

  };
  if (song == 2) {
    int size = sizeof(melodyMario) / sizeof(int);

    for (int thisNote = 0; thisNote < size; thisNote++) {

      // to calculate the note duration, take one second
      // divided by the note type.
      //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
      int noteDuration = 1000 / tempoMario[thisNote];

      buzz(melodyPin, melodyMario[thisNote], noteDuration);

      // to distinguish the notes, set a minimum time between them.
      // the note's duration + 30% seems to work well:
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);

      // stop the tone playing:
      buzz(melodyPin, 0, noteDuration);
    }

  };
}

void buzz(int targetPin, long frequency, long length) {
  long delayValue = 1000000 / frequency / 2; // calculate the delay value between transitions
  //// 1 second's worth of microseconds, divided by the frequency, then split in half since
  //// there are two phases to each cycle
  long numCycles = frequency * length / 1000; // calculate the number of cycles for proper timing
  //// multiply frequency, which is really cycles per second, by the number of seconds to
  //// get the total number of cycles to produce
  for (long i = 0; i < numCycles; i++) { // for the calculated length of time...
    digitalWrite(targetPin, HIGH); // write the buzzer pin high to push out the diaphram
    delayMicroseconds(delayValue); // wait for the calculated delay value
    digitalWrite(targetPin, LOW); // write the buzzer pin low to pull back the diaphram
    delayMicroseconds(delayValue); // wait again or the calculated delay value
  }
}

// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tuneR2D2() {
  beep(melodyPin, NOTE_A7, 100); //A
  beep(melodyPin, NOTE_G7, 100); //G
  beep(melodyPin, NOTE_E7, 100); //E
  beep(melodyPin, NOTE_C7, 100); //C
  beep(melodyPin, NOTE_D7, 100); //D
  beep(melodyPin, NOTE_B7, 100); //B
  beep(melodyPin, NOTE_F7, 100); //F
  beep(melodyPin, NOTE_C8, 100); //C
  beep(melodyPin, NOTE_A7, 100); //A
  beep(melodyPin, NOTE_G7, 100); //G
  beep(melodyPin, NOTE_E7, 100); //E
  beep(melodyPin, NOTE_C7, 100); //C
  beep(melodyPin, NOTE_D7, 100); //D
  beep(melodyPin, NOTE_B7, 100); //B
  beep(melodyPin, NOTE_F7, 100); //F
  beep(melodyPin, NOTE_C8, 100); //C
}

void beep (int speakerPin, float noteFrequency, long noteDuration)
{
  int x;
  // Convert the frequency to microseconds
  float microsecondsPerWave = 1000000 / noteFrequency;
  // Calculate how many milliseconds there are per HIGH/LOW cycles.
  float millisecondsPerCycle = 1000 / (microsecondsPerWave * 2);
  // Multiply noteDuration * number or cycles per millisecond
  float loopTime = noteDuration * millisecondsPerCycle;
  // Play the note for the calculated loopTime.
  for (x = 0; x < loopTime; x++)
  {
    digitalWrite(speakerPin, HIGH);
    delayMicroseconds(microsecondsPerWave);
    digitalWrite(speakerPin, LOW);
    delayMicroseconds(microsecondsPerWave);
  }
}

// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const int songLength = 18;
char notes[] = "cdfda ag cdfdg gf "; // a space represents a rest
int beats[] = {1, 1, 1, 1, 1, 1, 4, 4, 2, 1, 1, 1, 1, 1, 1, 4, 4, 2};
int tempo = 150;

void rickRoll() {
  int i, duration;
  for (i = 0; i < songLength; i++) // step through the song arrays
  {
    duration = beats[i] * tempo;  // length of note/rest in ms

    if (notes[i] == ' ')          // is this a rest?
    {
      delay(duration);            // then pause for a moment
    }
    else                          // otherwise, play the note
    {
      tone(melodyPin, frequency(notes[i]), duration);
      delay(duration); // wait for tone to finish
    }
    delay(tempo / 10);            // brief pause between notes
  }
}


int frequency(char note) {
  int i;
  const int numNotes = 8;  // number of notes we're storing
  char names[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C' };
  int frequencies[] = {262, 294, 330, 349, 392, 440, 494, 523};
  for (i = 0; i < numNotes; i++)  // Step through the notes
  {
    if (names[i] == note)         // Is this the one?
    {
      return (frequencies[i]);    // Yes! Return the frequency
    }
  }
  return (0); // We looked through everything and didn't find it,
  // but we still need to return a value, so return 0.
}



// ////////////////////////////////////////////////////////////////////////////////////////


//http://www.liutaiomottola.com/formulae/freqtab.htm
const int e = 329;
const int f = 349;
const int fS = 369;
const int g = 392;
const int a = 440;
const int b = 494;
const int c = 523;
const int bPin = 8;

int Tempo = (60000 / 128) * 4;

void beep2(int note, int duration)
{
  tone(melodyPin, note, Tempo / duration);
  delay(Tempo / duration);
  noTone(bPin);
  delay(50);
}

void tuneFinalCountdown()
{
  firstLine();
  ScndLine();
  ThirdLine() ;
  ForthLine();
  FiftthLine();
}



void firstLine()
{
  beep2(g, 4);
  beep2(b, 16);
  beep2(a, 16);
  beep2(b, 4);
  beep2(e, 4);
  //-----------------
  delay(Tempo / 4);
  delay(Tempo / 8);
  beep2(c, 16);
  beep2(b, 16);
  beep2(c, 8);
  beep2(b, 8);
  beep2(a, 4);
  //-----------------
  delay(Tempo / 4);
  delay(Tempo / 8);
  beep2(c, 16);
  beep2(b, 16);
  beep2(c, 4);
  beep2(e, 4);
  //-----------------
  beep2(fS, 4);
  delay(Tempo / 4);
  beep2(a, 16);
  beep2(g, 16);
  beep2(a, 8);
  beep2(g, 8);
  beep2(fS, 8); //sharp
  beep2(a, 8);
  //-----------------

}

void ScndLine() {


  beep2(g, 4);
  delay(Tempo / 4);
  beep2(b, 16);
  beep2(a, 16);
  beep2(b, 4);
  beep2(e, 4);
  //-----------------
  delay(Tempo / 8);
  delay(Tempo / 4);
  beep2(c, 16);
  beep2(b, 16);
  beep2(c, 8);
  beep2(b, 8);
  beep2(a, 4);
  //-----------------
  delay(Tempo / 8);
  delay(Tempo / 4);
  beep2(b, 16);
  beep2(a, 16);
  beep2(b, 4);
  beep2(e, 4);
  //-----------------
  beep2(fS, 4); //sharp
  delay(Tempo / 4);
  beep2(a, 16);
  beep2(g, 16);
  beep2(a, 8);
  beep2(g, 8);
  beep2(fS, 8); //sharp
  beep2(a, 8);
}

void ThirdLine() {

  beep2(g, 3);
  beep2(fS, 16);  //sharp
  beep2(g, 16);
  beep2(a, 3);
  beep2(g, 16);
  beep2(a, 16);
  //-----------------
  beep2(b, 8);
  beep2(a, 8);
  beep2(g, 8);
  beep2(fS, 8); //sharp
  beep2(e, 4);
  beep2(b, 4);
  //-----------------
  beep2(c, 1);
  //-----------------
  //beep2(c, 1);
  //-----------------
  beep2(g, 4);
  delay(Tempo / 4);
  beep2(b, 16);
  beep2(a, 16);
  beep2(b, 4);
  beep2(e, 4);
  //-----------------
  delay(Tempo / 8);
  delay(Tempo / 4);
  beep2(c, 16);
  beep2(b, 16);
  beep2(c, 8);
  beep2(b, 8);
  beep2(a, 8);
  //-----------------
}

void ForthLine() {
  delay(Tempo / 8);
  delay(Tempo / 4);
  beep2(c, 16);
  beep2(b, 16);
  beep2(c, 4);
  beep2(e, 4);
  //-----------------
  beep2(fS, 4);
  delay(Tempo / 4);
  beep2(a, 16);
  beep2(g, 16);
  beep2(a, 8);
  beep2(g, 8);
  beep2(fS, 8); //sharp
  beep2(a, 8);
  //-----------------
  beep2(g, 4);
  delay(Tempo / 4);
  beep2(b, 16);
  beep2(a, 16);
  beep2(b, 4);
  beep2(e, 4);
  //-----------------
  delay(Tempo / 8);
  delay(Tempo / 4);
  beep2(c, 16);
  beep2(b, 16);
  beep2(c, 8);
  beep2(b, 8);
  beep2(a, 4);
  //-----------------
  delay(Tempo / 8);
  delay(Tempo / 4);
  beep2(c, 16);
  beep2(b, 16);
  beep2(c, 4);
  beep2(e, 4);
}

void FiftthLine() {
  beep2(fS, 4);
  delay(Tempo / 4);
  beep2(a, 16);
  beep2(g, 16);
  beep2(a, 8);
  beep2(g, 8);
  beep2(fS, 8); //sharp
  beep2(a, 8);
  //-----------------
  beep2(g, 3);
  beep2(fS, 16);  //sharp
  beep2(g, 16);
  beep2(a, 3);
  beep2(g, 16);
  beep2(a, 16);
  //-----------------
  beep2(b, 8);
  beep2(a, 8);
  beep2(g, 8);
  beep2(fS, 8); //sharp
  beep2(e, 4);
  beep2(b, 4);
  //-----------------
  beep2(c, 1);
  //-----------------
}
