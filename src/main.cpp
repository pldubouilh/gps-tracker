#include <Arduino.h>
#include "Adafruit_FONA.h"
#include "Adafruit_MMA8451.h"
#include "Snooze.h"


#define SIM_PIN "SOME PIN !"
#define FONA_RX 2
#define FONA_TX 3
#define FONA_RST 4
#define FONA_OFF_CTL 22
#define FONA_ON_CTL 23

#define PIN_ACCEL_ISR 11
#define ISR_ACCEL PIN_ACCEL_ISR
#define ISR_TIME 35

#define EMERGENCY 418
#define EMERGENCY_GPS 421
#define URL_TO_FETCH "SUCH URL !"

#define DELAY_EMERGENCY 25
#define TIMER_HRS 0
#define TIMER_MINS 5
#define TIMER_SECS 0
#define ALARM() alarm.setRtcTimer(TIMER_HRS, TIMER_MINS, TIMER_SECS)

#define RESET_GPS() sprintf(lastGps, "\0")

// Fona
HardwareSerial *fonaSerial = &Serial1;
Adafruit_FONA fona = Adafruit_FONA(FONA_RST);

// Accel
Adafruit_MMA8451 mma = Adafruit_MMA8451();

// Alarm
SnoozeDigital sleepPinAccel;
SnoozeAlarm alarm;
SnoozeBlock config_teensy32(sleepPinAccel, alarm);

// Vars
char replybuffer[255];
char lastLoc[255];
char lastGps[120];
int report, stateServer, fonaState;
uint16_t lastBat = 0;
uint8_t  lastSig = 0;

/**
 *
 * Helpers
 *
*/
void blink() {
  digitalWrite(13, HIGH);
  delay(80);
  digitalWrite(13, LOW);
}

void blinkTwice() {
  blink();
  delay(80);
  blink();
}

/**
 *
 * Accel
 *
*/
void initAccel() {
  mma.begin();
  mma.resetInterrupt();
  sleepPinAccel.pinMode(PIN_ACCEL_ISR, INPUT_PULLUP, FALLING);
}

/**
 *
 * Fona
 *
*/
void unlockSim(){
  fona.unlockSIM(SIM_PIN);
  delay(10000);
}

void GPRSoff() {
  fona.enableGPRS(false);
  delay(500);
}

void GPRSon() {
  fona.enableGPRS(true);
  delay(500);
}

void GPRSLoc() {
  uint16_t returncode;
  fona.getGSMLoc(&returncode, lastLoc, 250);
}

void getbat() {
  // fona.getBattVoltage(&vbat)
  fona.getBattPercent(&lastBat);
  delay(500);
}

void getSignal() {
  lastSig = fona.getRSSI();
  delay(500);
}

void gpsOff () {
  fona.enableGPS(false);
  delay(500);
}

void gpsOn () {
  fona.enableGPS(true);
  delay(500);
}

void getGps() {
  fona.getGPS(0, lastGps, 120);
  delay(500);
}

void httpGet() {
    // read website URL
  uint16_t statuscode;
  int16_t length;
  char url[500];

  GPRSon();
  getSignal();
  getbat();
  GPRSLoc();

  sprintf(url, "%s/e?report=%d&bat=%ld&sig=%ld&pos=%s&gps=%s",
    URL_TO_FETCH, report, lastBat, lastSig, lastLoc, lastGps);

  if (!fona.HTTP_GET_start(url, &statuscode, (uint16_t *)&length)) {
    return;
  }
  while (length > 0) {
    while (fona.available()) {
      char c = fona.read();
      length--;
      if (! length) break;
    }
  }
  fona.HTTP_GET_end();
  stateServer = statuscode;
  delay(1000);
}

void fonaOff() {
  digitalWrite(FONA_OFF_CTL, HIGH);
  delay(500);
  digitalWrite(FONA_OFF_CTL, LOW);
  fonaState = 0;
}

void fonaOn() {
  // Leave if already on
  if (fonaState) return;

  digitalWrite(FONA_ON_CTL, HIGH);
  delay(500);
  digitalWrite(FONA_ON_CTL, LOW);
  delay(2000);

  fonaSerial->begin(4800);
  if (! fona.begin(*fonaSerial)) {
    while(1) blinkTwice();
  }
  fonaState = 1;

  // Print module IMEI number.
  char imei[16] = {0};
  uint8_t imeiLen = fona.getIMEI(imei);
  delay(1000);
  unlockSim();
}

/**
 *
 * emergency
 *
*/
void emergencyRoutine() {
  delay(DELAY_EMERGENCY*1000);
  blink();
  fonaOn();

  if (stateServer == EMERGENCY_GPS) {
    report = 3;
    gpsOn();
    getGps();
  } else {
    report = 2;
    gpsOff();
  }
  httpGet();
}

/**
 *
 * normal
 *
*/
void normalRoutine() {
  int who;
  fonaOff();
  who = Snooze.deepSleep(config_teensy32);

  // Emergency ping after bump
  if (who == ISR_ACCEL) {
    blink();
    report = 1;
  // Daily ping
  } else if (who == ISR_TIME) {
    blinkTwice();
    report = 0;
    ALARM();
  }

  fonaOn();
  httpGet();
}

/**
 *
 * Init
 *
*/
void setup() {
  pinMode(FONA_OFF_CTL, OUTPUT);
  pinMode(FONA_ON_CTL, OUTPUT);
  pinMode(13, OUTPUT);
  Serial.begin(115200);

  digitalWrite(FONA_OFF_CTL, LOW);
  digitalWrite(FONA_ON_CTL, LOW);

  stateServer = 200;
  fonaOff();
  initAccel();
  blinkTwice();
  ALARM();
}

void loop() {
  RESET_GPS();

  if (stateServer == EMERGENCY || stateServer == EMERGENCY_GPS) {
    emergencyRoutine();
  } else {
    normalRoutine();
  }
}
