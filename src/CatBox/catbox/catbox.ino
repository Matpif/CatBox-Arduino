/*******************
 *     CatBox      *
 *******************/

#include <Timer.h>

Timer timer;

// pins
int _pinMotor = 7;

// Serial return
String serialString = "";

// Periods
long period;
long periodMotorOn;
long oneDay;

int nbSequenceByDay = 2; // 2 sequence motor by day
boolean statMotor = false; // Stop by default

// Setup
void setup() {

  Serial.begin(19200);

  period = 12L * 60L * 60L * 1000L; // 12 h
  periodMotorOn = 20L * 1000L; // 20 s
  oneDay = 24L * 60L * 60L * 1000L; // 1 day
  
  pinMode(_pinMotor, OUTPUT);
  stopMotor();
  
  timer.every(period, startMotor);
  timer.every(oneDay, initSequenceByDay);
}

// Main programme
void loop() {
  timer.update();
}

// Start Motor since period periodMotorOn
void startMotor() {

  if (nbSequenceByDay > 0 && !statMotor) {
    statMotor = true;
    nbSequenceByDay--;
    digitalWrite(_pinMotor, HIGH);
    timer.after(periodMotorOn, stopMotor);
  }
}

// Stop Motor
void stopMotor() {
  statMotor = false;
  digitalWrite(_pinMotor, LOW);
}

// Init nb sequence
void initSequenceByDay(){
  nbSequenceByDay = 2;
}

// Beep signal
void beep() {
  // TODO: Beep event
}

// Serial event
void serialEvent(){
  
  if (Serial.available() > 0) {
    
    serialString = Serial.readString();

    if (serialString == "HELP") {
      Serial.println("*** HELP ***");
      Serial.println("   CBM1BC");
      Serial.println("   CBM0BC");
      Serial.println("   CBMFBC");
      Serial.println("*** HELP ***");
    } else if (serialString.length() == 6) {

      String begin = serialString.substring(0, 2);
      String end = serialString.substring(4, 6);
      String action = serialString.substring(2, 4);
      
      if (begin == "CB" && end == "BC") {
        if (action == "M1")
          startMotor();
        else if (action == "M0")
          stopMotor();
        else if (action == "MF") {
          nbSequenceByDay++;
          startMotor();
        }
      }
    }
  }
}

