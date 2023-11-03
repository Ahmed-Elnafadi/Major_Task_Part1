#include <SPI.h>

/*********************************/
/*********** definition **********/
#define SLAVE_SELECT  53
#define SPEEDOMETER   A0
#define TRIG_PIN      10
#define ECHO_PIN      9
#define BATTERY_VOLT  A1
/*********************************/

/****** Define Global Variables ******/
float voltageSensor;
float speedSensor;
int distance;
long duration;
void setup() {
/****** Configure SPI *******/
pinMode(SLAVE_SELECT, OUTPUT);
SPI.begin();
/****************************/

/* Configure Pins Direction */
pinMode(SPEEDOMETER, INPUT);  // SPeedSensor
pinMode(TRIG_PIN, OUTPUT);    // Ultrasonic
pinMode(ECHO_PIN, INPUT);     // Ultrasonic
pinMode(BATTERY_VOLT, INPUT); // Voltage Sensor
}

void loop() {

/**** Read from speedSensor ****/
speedSensor = analogRead(SPEEDOMETER);
/* Map values to RPM then to Km/h */
speedSensor = map(speedSensor, 0, 1023, 0, 200); // RPM
speedSensor = speedSensor * 0.02261;
/**********************************/

/**** Read Voltage Sensor ****/
voltageSensor = analogRead(BATTERY_VOLT);
voltageSensor = map(voltageSensor, 0, 1023, 0, 74);
voltageSensor = voltageSensor * 5000 * 0.00001;
/*****************************/

/**** Read Distance ****/
/* Configure Ultrasonic */
digitalWrite(TRIG_PIN, LOW);
delayMicroseconds(2);
digitalWrite(TRIG_PIN, HIGH);
delayMicroseconds(10);
digitalWrite(TRIG_PIN, LOW);
/**************************/
/* Read Echo Pin */
duration = pulseIn(ECHO_PIN, HIGH);

/* Calculate Distance */
distance = duration*0.034/2;
/***********************/

/* Send all these data through SPI to Main Arduino */

SPI.beginTransaction(SPISettings(14000000, MSBFIRST, SPI_MODE0));
digitalWrite(SLAVE_SELECT, LOW);
SPI.transfer((byte*)&distance, sizeof(distance));
SPI.transfer((byte*)&voltageSensor, sizeof(voltageSensor));
SPI.transfer((byte*)&speedSensor, sizeof(speedSensor));
SPI.endTransaction();
digitalWrite(SLAVE_SELECT, HIGH);
delay(100);
/*****************************************************/
}
