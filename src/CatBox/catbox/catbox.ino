/*******************
 *     CatBox      *
 *******************/

#include <Timer.h>
#include <WiFi.h>

Timer timer;

// pins
int _pinMotor = 7;

// LEDs controls
int _pinLEDWifi = 4;
int _pinLEDPower = 8;
int _timerLEDWifi = 0;
int _timerLEDPower = 0;

// Serial return
String serialString = "";

// Periods
long period;
long periodMotorOn;
long oneDay;
long wifiDisconnect;

int nbSequenceByDay = 2; // 2 sequence motor by day
boolean statMotor = false; // Stop by default

// Wifi params
char ssid[] = "";
char pass[] = "";
int status = WL_IDLE_STATUS;
int _timerWifi = 0;

// Setup
void setup() {

  Serial.begin(19200);
  while(!Serial);
  
  // Check for the presence of the wifi shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
  }

  period = 12L * 60L * 60L * 1000L; // 12 h
  periodMotorOn = 20L * 1000L; // 20 s
  oneDay = 24L * 60L * 60L * 1000L; // 1 day
  wifiDisconnect = 15L * 1000L; // 15 s
  
  pinMode(_pinMotor, OUTPUT);
  pinMode(_pinLEDWifi, OUTPUT);
  pinMode(_pinLEDPower, OUTPUT);

  digitalWrite(_pinLEDPower, HIGH);
  stopMotor();
  
  timer.every(period, startMotor);
  timer.every(oneDay, initSequenceByDay);
  
  connectWifi();

  if (WiFi.status() != WL_CONNECTED && WiFi.status() != WL_NO_SHIELD) {
    _timerWifi = timer.every(wifiDisconnect, connectWifi);
  }
}

// Main programme
void loop() {
  timer.update();
  //ledController(); // TODO: Problème plantage timer
}

// Start Motor since period periodMotorOn
void startMotor() {
  
  Serial.println("Start Motor");
  if (nbSequenceByDay > 0 && !statMotor) {
    timer.after(periodMotorOn, stopMotor);
    statMotor = true;
    nbSequenceByDay--;
    digitalWrite(_pinMotor, HIGH);
  }
}

// Stop Motor
void stopMotor() {
  Serial.println("Stop Motor");
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

void ledController() {

  switch (WiFi.status()) {
    case WL_CONNECTION_LOST:
    case WL_CONNECT_FAILED:

      if (_timerLEDWifi == 0)
        _timerLEDWifi = timer.every(1000L, blinkLEDWifi);

      if (_timerWifi == 0)
        _timerWifi = timer.every(wifiDisconnect, connectWifi);
        
      break;
    case  WL_CONNECTED:
    
      if (_timerLEDWifi != 0)
        timer.stop(_timerLEDWifi);
      
      digitalWrite(_pinLEDWifi, LOW);
      _timerLEDWifi=0;
      
      break;
    default:
    
      if (_timerLEDWifi != 0)
        timer.stop(_timerLEDWifi);
        
      digitalWrite(_pinLEDWifi, HIGH);
      _timerLEDWifi=0;
    break;
  }
}

void blinkLEDWifi() {
  digitalWrite(_pinLEDWifi, !digitalRead(_pinLEDWifi));
}

void connectWifi() {

  Serial.println("Connecting...");
  if (WiFi.status() != WL_CONNECTED && WiFi.status() != WL_NO_SHIELD) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
    
    if (WiFi.status() == WL_CONNECTED) {
      
      Serial.print("IP Address: ");
      Serial.println(WiFi.localIP());
      
      if (_timerWifi != 0) {
        timer.stop(_timerWifi);
        _timerWifi = 0;
      }
      
    } else {
      
      Serial.println("Disconnected");
      if (_timerWifi == 0 && WiFi.status() != WL_NO_SHIELD) {
        _timerWifi = timer.every(wifiDisconnect, connectWifi);
      } else if (_timerWifi != 0 && WiFi.status() == WL_NO_SHIELD) {
        timer.stop(_timerWifi);
        _timerWifi = 0;
      }
    }
  } else {
    
    Serial.println("Disconnected");
    if (_timerWifi == 0 && WiFi.status() != WL_NO_SHIELD) {
      _timerWifi = timer.every(wifiDisconnect, connectWifi);
    } else if (_timerWifi != 0 && WiFi.status() == WL_NO_SHIELD) {
      timer.stop(_timerWifi);
      _timerWifi = 0;
    }
  }
}

