#include <Adafruit_INA219.h>
#include <Arduino.h>
#include <U8g2lib.h>

// ------- Arduino I/O PIN MAP --------
//
// -L----A----- LCD ------------
// 12 -> 51
// 11 -> 53
// 10 -> 52
//  9 -> 50
//  8 -> 49
//  7 -> +5V DC     ( ! External ! )
//  6 -> +5V DC GND ( ! External ! )
//  5 -> +5V DC     ( ! External ! )
//
// ------------ Arduino ------------
// #define relayControlPin1  -> Digital pin to relay board "In1"
// #define voltPin           -> 18650 holder positive terminal thru 2.2k resistor
// GND                       -> 18650 holder negative terminal
// SDA(20)                   -> SDA on INA256 board
// SCL(21)                   -> SCL on INA256 board

U8G2_UC1701_MINI12864_F_4W_SW_SPI u8g2(U8G2_R0, /*clock=*/ 49, /*data=*/ 50, /*cs=*/ 51, /*dc=*/ 52, /*reset=*/ 53);
Adafruit_INA219 ina219;

#define RELAY_ON LOW
#define RELAY_OFF HIGH
#define relayControlPin1 12
#define voltPin A0
#define sampleAmount 20
#define cutoffVoltage 2.7


float voltage = 0.0;      // This variable exists soley for a single if statement in the GUI update func to print an error message or not. Probably really hacky.
float curLoopVolts = 0.0; // Stores the winning lottery numbers.
float curLoopRaw = 0.0;   // Stores the losing lottery numbers.

byte running = 0;

byte debugMode = 0;   // DO NOT set this to 1 unless you understand the code!
// this variable set to 1 can lead to the Arduino toggling
// the relay on and off as fast as it can when the cutoff voltage is reached.
// THIS CAN CAUSE PHYSICAL DAMAGE TO THE RELAY, DON'T USE THIS PLEASE, UNLESS YOU'VE READ AND UNDERSTAND THE CODE FLOW.

unsigned long looper = 0;       // 102 years?!
unsigned int loopCounter = 0;   // Counts it's eggs before they've hatched.


unsigned long previousTuneMillis = 0;
const long interval = 120000;  // 2 minutes



// INA256 Variables
float shuntvoltage = 0;
float busvoltage = 0;
float current_mA = 0;
float loadvoltage = 0;

// Timer Variables - Please see the getTime() function for owed credit.
unsigned long millisCalc_mAh;
unsigned long  startMillisec;
float hours;
float last_hours = 0.0;
int mins, secs, tMins;
float mAh_soFar = 0.0;


void setup(void) {
  if (debugMode == 0) {
    Serial.begin(19200);
  }
  pinMode(A0, INPUT);                        // Battery test lead from holder
  pinMode(3, OUTPUT);                        // Piezo buzzer
  tuneMario(1);
  pinMode(LED_BUILTIN, OUTPUT);              // Self-destruct button
  pinMode(relayControlPin1, OUTPUT);         // Digital pin to relay board "In1"
  digitalWrite(relayControlPin1, RELAY_OFF); // Digital pins start LOW, which means the relay board is ON by default.
  // I'm using the NO contacts, so this needs to be rectified as soon as the pin is initialized.

  u8g2.begin();                              // Initialize the LCD library
  u8g2.setFont(u8g2_font_5x7_tf);            //
  u8g2.setFlipMode(0);                       //

  ina219.begin();                            // Initialize the LCD library
  ina219.setCalibration_32V_1A();            // Higher precision calibration
  //ina219.setCalibration_16V_400mA();       // Highest precision calibration, low draw ability.
  getTime();                                 // Preload the time variables
  startMillisec = millis();                  //
  randomSeed(analogRead(3) + analogRead(0) / analogRead(2) );
}

float getVolts(int vPin) {                  // Custom function to read analog pin voltage and compensate for Arduino fluctuations.
  float sum = 0;
  float tempVolts;
  for (int i = 0; i < sampleAmount; i++) {  // Voltage reading value smoothing
    sum = sum + analogRead(vPin);           //
    delay(5);                               //
  }                                         //
  tempVolts = ((float)sum / (float)sampleAmount * 5.015) / 1024.0; // 5.015V is the calibrated reference voltage, we need to account for this.
  // This value will change depending on the individual Arduino board, the power source
  // and other factors, reading the +5V pin on the Arduino will give you the value you need.
  return tempVolts;
}

void readINA219() {
  float circuitOhms = 0.08;       // Circuit resistance for compensation
  float temp_mA = 0;              // mA smoothing temp storage
  float temp_V = 0;               // Load voltage smoothing temp storage
  float temp_shunt = 0;           // Shunt smoothing temp storage
  float vR;

  for (int i = 0; i < sampleAmount; i++) {                  // Shunt voltage value smoothing
    temp_shunt = temp_shunt + ina219.getShuntVoltage_mV();  //
    delay(5);                                               //
  }                                                         //
  shuntvoltage = temp_shunt / sampleAmount;                 //

  for (int i = 0; i < sampleAmount; i++) {                  // Bus voltage value smoothing
    temp_V = temp_V + ina219.getBusVoltage_V();             //
    delay(5);                                               //
  }                                                         //
  busvoltage = temp_V / sampleAmount;                       //

  for (int i = 0; i < sampleAmount; i++) {                  // Current value smoothing
    temp_mA = temp_mA + ina219.getCurrent_mA();             //
    delay(5);                                               //
  }                                                         //
  current_mA = temp_mA / sampleAmount;                      //

  vR = circuitOhms * current_mA / 1000;                     // Factoring in the voltage drop across the
  loadvoltage = busvoltage  + (shuntvoltage / 1000) + vR;   // INA256 0.1R shunt & load/wiring/contact resistance.

}

void loop(void) {
  digitalWrite(LED_BUILTIN, LOW);               // Visual loop heartbeat
  readINA219();                                 // Query the INA256 chip
  curLoopVolts = getVolts(voltPin);             // Read the "#define voltPin" pin voltage
  curLoopRaw = analogRead(voltPin);             // Read the "#define voltPin" pin analog value

  if (curLoopVolts <= cutoffVoltage) {          // Check if cell voltage is too low, if it is....
    voltage = 666;                              // ... set to 666 to trip an if statement in the GUI update
    if (running == 1) {
      running = 0;                              // ... flip the running variable to 0 (for the LCD mode display, it's rendered useless if we're into this if statement)
      relayOFF();                               // ... disconnect the battery from the load
      updateLCD();                              // and do a final LCD update once cutoffVoltage has been reached, so that the last values and test time are preserved.
      rickRoll();
      do {
        unsigned long currentTuneMillis = millis();
        if (currentTuneMillis - previousTuneMillis >= interval) {
          previousTuneMillis = currentTuneMillis;
          byte randSong = random(1, 4);
          if (randSong == 1) {
            tuneMario(1);
          };
          if (randSong == 2) {
            rickRoll();
          };
          if (randSong >= 3) {
            tuneImperialMarch();
          };
        }
        digitalWrite(LED_BUILTIN, LOW);         // 102 year loop to blink the builtin LED
        delay(750);                             // to signal the test is over.
        digitalWrite(LED_BUILTIN, HIGH);        //
        delay(750);                             // Yes I could have done this
        looper++;                               // tons of other ways
      } while (looper < 2147483647);            // .... but I didn't want to.
    }
  }
  if (curLoopVolts > 2.7) {                     // Check if cell voltage is NOT too low, if it is....
    if (running == 0) {                         // ... and we're just starting out,
      running = 1;                              // ... flip the running byte
      relayON();                                // and connect the battery to the load.
    }
  }
  loopCounter++;                                // Cookie clicker counter
  getTime();                                    // Update the time variables just before the LCD update
  updateLCD();                                  // Update the LCD
  digitalWrite(LED_BUILTIN, HIGH);              // Visual loop heartbeat
}

void updateLCD() {
  u8g2.clearBuffer();
  u8g2.setFlipMode(0);     // Otherwise the LCD will occasionally flip itself upside-down for some unknown reason...
  byte writeLine = 7;      // First row pixel number
  byte lineIncrement = 9;  // Number of pixels to jump down for every new line

  if (millis() > millisCalc_mAh + 1000) {                                 // If it's been more than 1 real second since the last mA consumed calculation...
    float this_hours = (millis() - startMillisec) / (1000.0 * 3600.0);    // Calculate mA used in the last second
    mAh_soFar = mAh_soFar + ((this_hours - last_hours) * current_mA);     //
    last_hours = this_hours;                                              //
    millisCalc_mAh = millis();                                            //
    Serial.print((int)hours); Serial.print(":");
    if (mins < 10) Serial.print("0"); Serial.print(mins); Serial.print(":");
    if (secs < 10) Serial.print("0");
    Serial.print(secs); Serial.print(",");
    Serial.print(curLoopVolts); Serial.print(","); Serial.println(mAh_soFar);
  }                                                                       //

  u8g2.setCursor(0, writeLine); u8g2.print("Cell 1: "); writeLine = writeLine + lineIncrement; // Cell 1 is because I plan on expanding this script to support a second INA256 board, 18650 cell and load.
  if (voltage == 666) {
    u8g2.print("No/Dead Cell!");  // Here's our mystery error message!
  }
  else {
    u8g2.print(curLoopVolts);  // or else we print the voltage as measured at "#define voltPin"
  }

  u8g2.setCursor(0, writeLine); u8g2.print("RAW:      "); u8g2.print(curLoopRaw); u8g2.print(" ("); u8g2.print(curLoopVolts); u8g2.print(" V)"); writeLine = writeLine + lineIncrement;  // Raw analog value, this is here because the prior line may have an error message.
  u8g2.setCursor(0, writeLine); u8g2.print("L-Volt:   "); u8g2.print(loadvoltage); u8g2.print(" V"); writeLine = writeLine + lineIncrement;                                                       // Voltage from INA256
  u8g2.setCursor(0, writeLine); u8g2.print("L-AMP:    "); u8g2.print(current_mA); u8g2.print(" mA"); writeLine = writeLine + lineIncrement;                                                       // Current draw from INA256
  u8g2.setCursor(0, writeLine); u8g2.print("Capacity: "); u8g2.print(mAh_soFar); u8g2.print(" mA"); writeLine = writeLine + lineIncrement;
  // Calculated mAh consumed
  u8g2.setCursor(0, writeLine);                                       //
  u8g2.print("Time: "); u8g2.print((int)hours); u8g2.print(":");      //
  if (mins < 10) u8g2.print("0"); u8g2.print(mins); u8g2.print(":");  // Time counter block
  if (secs < 10) u8g2.print("0");                                     //
  u8g2.print(secs);                                                   //
  
  u8g2.setCursor(0, 62); u8g2.print(running);         // Current running mode (debugging)
  u8g2.setCursor(103, 62); u8g2.print(loopCounter);   // Loop counter
  u8g2.sendBuffer();                                  // Send data to the LCD controller

  if (debugMode == 1) {
    voltage = 0;  // Obsolete re-zeroing of this variable from initial coding,
  }
  // this zero would have allowed for recovering from the "666"
  // voltage error without resetting the Arduino. Rendered useless
  // after the 102 year loop was coded in, but left here for debugging.
  // THIS CAN CAUSE PHYSICAL DAMAGE TO THE RELAY, DON'T USE THIS PLEASE.
}

void relayON() {
  digitalWrite(relayControlPin1, RELAY_ON);
  if (debugMode == 1) {
    Serial.println("Relay ON");
  }
}

void relayOFF() {
  digitalWrite(relayControlPin1, RELAY_OFF);
  if (debugMode == 1) {
    Serial.println("Relay OFF");
  }
}

void getTime() {  // This entire function was copy/paste'ed from http://www.vwlowen.co.uk/arduino/battery-tester/page3.htm
  unsigned long nTime;
  unsigned long milli =  millis() - startMillisec;
  nTime = milli / 1000;
  tMins = nTime / 60;
  nTime = nTime % (24 * 3600);
  hours = nTime / 3600;
  nTime = nTime % 3600;
  mins  = nTime / 60;
  secs  = nTime % 60;
}
