#include <SPI.h>
#include <LiquidCrystal.h>
/*********************************/
/*********** definition **********/
#define SLAVE_SELECT  53
#define MISO          50

#define CAR_LOCK      4
/* LCD Pins */
#define RS_PIN   12
#define EN_PIN   11
#define DS4      7
#define DS5      6
#define DS6      3
#define DS7      2

#define LCD_COLUMNS  16
#define LCD_ROWS     2

#define FRONT_LIGHT   9
#define HORN          10
/********************************/

/***** Global Variables *****/
float voltageSensor = 0.0;
float speedSensor = 0.0;
int distance = 0.0;
byte Buffer[20];
byte Buffer_len = 0;
unsigned char flag = false;
char horn;
char flasher;
char frontLight;
char carLock;
/****************************/
/* Define LCD */
LiquidCrystal lcd(RS_PIN, EN_PIN, DS4, DS5, DS6, DS7);
void setup() {
/****** Configure SPI *******/
SPI.begin();
pinMode(SLAVE_SELECT, INPUT);
SPCR|= _BV(SPE);  // SPI in slave mode
SPI.attachInterrupt(); // Enable Interrupt
/****************************/

/****** Configure UART ******/
Serial.begin(9600);
/****************************/

/* Initialize and Configure the LCD */
lcd.begin(LCD_COLUMNS, LCD_ROWS);
lcd.setCursor(0, 0);
/**************************************/
/* Car lock pin */
pinMode(CAR_LOCK, INPUT);
/* Front Light */
pinMode(FRONT_LIGHT, OUTPUT);
/* Horn */
pinMode(HORN, OUTPUT);
}

void loop() {
/* Receive Values from SPI */
SPI.beginTransaction(SPISettings(14000000, MSBFIRST, SPI_MODE0));
SPI.transfer((byte*)&distance, sizeof(distance));
SPI.transfer((byte*)&voltageSensor, sizeof(voltageSensor));
SPI.transfer((byte*)&speedSensor, sizeof(speedSensor));
SPI.endTransaction();
delay(100);
/***************************/

/* Display on character LCD */
lcd.setCursor(0, 0);
lcd.print("Dist:"+ String(distance)+" ");

lcd.setCursor(0, 1);
lcd.print("Speed:"+String(speedSensor)+"Km/h");
/****************************/

/***** Send and Receive from DWIN LCD *****/
DISPLAY_switchRead(); // receive from display horn, flasher, front light states
sendIntNumber(distance, 0x1200);  // Send distance value to lcd
sendFloatNumber(speedSensor, 0x1000); // send speed in Km/h to lcd
sendFloatNumber(voltageSensor, 0x1100); // send voltage of battery to lcd

/* Send state of car lock to lcd */
carLock = digitalRead(CAR_LOCK);
sendIntNumber(carLock, 0x1300);
/******************************************/
/* Control Horn, Flasher and front light */
digitalWrite(HORN, horn);
if(flasher == 1 && frontLight == 0){
digitalWrite(FRONT_LIGHT, flasher);  
}
else if(flasher == 0 && frontLight == 1){
digitalWrite(FRONT_LIGHT, frontLight);
}
}
/***************************************************************************************/
void DISPLAY_switchRead(void){
if(Serial.available()){
  Buffer[Buffer_len] = Serial.read();
Buffer_len++;
flag = true;
}
else{
if(flag){
  if(Buffer[0] == 0x5A){
    if(Buffer[4] == 0x06){
      horn = bitRead(Buffer[8], 0);
    }
  else if(Buffer[4] == 0x04){
       flasher = bitRead(Buffer[8], 0); 
  }
  else if(Buffer[4] == 0x02){
       frontLight = bitRead(Buffer[8], 0); 
  }            
  }
Buffer_len = 0;
flag = false;
}}  
}
/***************************************************************************************/
void sendIntNumber(int intValue, long int address){
  Serial.write(0x5A); // Header
  Serial.write(0xA5); // Header
  Serial.write(0x05); // Length: VP address + write command + length of float
  Serial.write(0x82); // write command
  Serial.write(highByte(address)); // Address High byte
  Serial.write(lowByte(address));  // Address Low byte
  Serial.write(highByte(intValue)); // data High byte
  Serial.write(lowByte(intValue));  // data low byte
}
/***************************************************************************************/
void sendFloatNumber(float floatValue, long int address){
  Serial.write(0x5A); // Header
  Serial.write(0xA5); // Header
  Serial.write(0x07); // Length: VP address + write command + length of float
  Serial.write(0x82); // write command        
  Serial.write(highByte(address));
  Serial.write(lowByte(address));
byte hex[4] = {0};
FloatToHex(floatValue, hex);

Serial.write(hex[3]);
Serial.write(hex[2]);
Serial.write(hex[1]);
Serial.write(hex[0]);
}
/***************************************************************************************/
void FloatToHex(float f, byte* hex){
  byte* f_byte = reinterpret_cast<byte*>(&f); // The value of f_byte is pointer to f
  memcpy(hex, f_byte, 4); // hex: destination, f_byte: source, 4: number of bytes to copy
  }