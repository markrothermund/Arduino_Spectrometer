 /*
 * WheeTrometerTeensy
 * TCD1304      - Teensy 4.0
 * ------------------------
 * pin 3 (ICG)  - pin 5 - flexPWM2.1
 * pin 4 (MCLK) - pin 6 - flexPWM2.2
 * pin 5 (SH)   - pin 4 - flexPWM2.0
 *
 * - pin 22 Low reference pwm 
 * - pin 8 High reference pwm 
 * 
 * ADC pins:
 * pin 10 - CS
 * pin 12 - MISO
 * Pin 13 - SCK
 * Pin 23 - UVLED
 * 
 *  Note: isr for ICG period moved to setup_PWM tab
 *  Note:  flexpwm2_3_isr was removed and replaced by readADC in 16bitADC tab
 *  Note: Changed M clock to 128 F clocks, gives 1.16 MHz M clock
 *    That made life a lot simpler since SH and ICG are on 128 clock divider
 */

#include <SPI.h> 
#include <EEPROM.h>
//#include <iostream>
//#include <string>
#define PIXELS 3648
#define lowRefPin 22
#define highRefPin 8
#define UVLED 23

///////// beginning of JSS additions  ///////////
bool collecting = false;  // flag is true when collecting data
volatile bool runFlag = false;  // set true when '&' recieved from user interface
volatile bool ICGFlag = false;  // set true in icg interrupt service routine (setupPWM tab)
bool checkFlag = false;
bool setRef = false;

//olatile int SHCounter = 0;  // counter for integration cycles
uint32_t data[PIXELS] = {0};
//SPISettings settings(30000000,MSBFIRST,SPI_MODE0);  // set SPI clock to run at 30 MHz, 33 is max
const int CSPin = 10;
int integrationTime = 2000;   // integration time in microsec (JSS)
float integrationMClocks = integrationTime*1.16;  //MClock is 1.16 MHz (Clocks/ us).  
             // integration time in us 

int minFrameClocks = 4*(32+PIXELS+4);   // minimum Frame time in us
float minFrameTime = minFrameClocks/1.16;
int CLKPMCLK = 128;   // this determines MClock period in setup_flexpwm.h
   // this is what gives 1.16 MHz clock.  150 gives 1 MHz
   /////////////// note: no prescaler on MClock ////////////

int loops = 1;              // number of times pixels averaged for one spectrum
int loopCounter;            // how many loops have we already collected?
int counter = 0;                          // number of pixels read
//bool a = false;               // boolean for testing the adc read isr
int lowRefV = 430;  // 1700 at 12 bit, 115 at 8
int highRefV = 1023; // at 12 bit resolution
int mClockShift = 30;
///////// end of JSS additions /////////

int minMCLKPF = ceil((float)(32+PIXELS+14)*4);  // minimum number of CCD clocks in a frame

///////// frame is the icg period ////////
// calculate counter values such that an integer number of both 128x divided fbus cycles and MCLK cycles happen during each frame


int CNTSH = ceil(integrationMClocks);
//int ES = ceil((float)minFrameClocks/integrationMClocks/128);  // changed JSS 9/11/21
int ES = ceil((float)minFrameClocks/CNTSH); 
int CNTPF = ES*CNTSH;   // JSS 9/11/21
//int CLKPF = CNTPF;
int USPF = 128*CNTPF / (F_BUS_ACTUAL / 1000000); // microseconds per frame

int CNT_ICG = (6.0 * (F_BUS_ACTUAL / 1000000))/128; 
int CNT_SH = 2; 
int shiftSH = 6;
int off = (0.5 * (F_BUS_ACTUAL / 1000000))/(128 / ES); // delay time between ICG low and SH high
int adc_off = 400; // counts before ADC read

void setup() {
  pinMode(UVLED,OUTPUT); //Sets UVLED pinmode to Output
  
  pinMode(CSPin,OUTPUT);
  digitalWrite(CSPin,HIGH);
  
  Serial.begin(115200);
  Serial.print("/Integration time (us): ");
  Serial.print(integrationTime);
  Serial.print(", Int periods / frame: ");
  Serial.print(ES);
  Serial.print(", Microseconds per frame: ");
   Serial.println(USPF);

  
  setupClocks();

  pinMode(lowRefPin,OUTPUT);
  pinMode(highRefPin,OUTPUT);
  analogWriteResolution(10);
  analogWriteFrequency(lowRefPin,146484.38);
  analogWriteFrequency(highRefPin,146484.38);
  analogWrite(lowRefPin,lowRefV);
  analogWrite(highRefPin,highRefV);
  

  SPI.begin();
  SPI.usingInterrupt(IRQ_FLEXPWM2_3);  // for external ADC

  // attach interrupts
  attachInterruptVector(IRQ_FLEXPWM2_1, ICG_isr);
  NVIC_ENABLE_IRQ(IRQ_FLEXPWM2_1);

  attachInterruptVector(IRQ_FLEXPWM2_3, readADC);   // new
  NVIC_ENABLE_IRQ(IRQ_FLEXPWM2_3);
  NVIC_SET_PRIORITY(IRQ_FLEXPWM2_3, 128);

}



void loop(){
    if (Serial.available()> 0){
//          if (!collecting && Serial.available()> 0){
   serialRead(); 
  }

 }
