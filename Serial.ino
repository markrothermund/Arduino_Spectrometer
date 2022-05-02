 /*
 * Serial tab 010/22/21
 * data is being output on miso, not being read by uteensy
 * note: add a '/' char before every transmitted comment
 */

void serialRead(void){
  char c = Serial.read();
  if (c == 'O'){
    digitalWrite(UVLED, HIGH);
    serialRead();
  }
  if (c == 'I'){          // set integration time by sending 'I' plus value
    Serial.println("");
    Serial.println("----------------");
  //  CLKPMCLK = 128;
    
    integrationTime = Serial.parseInt();
    integrationMClocks = integrationTime*1.16;  //microseconds * MHz = CCD clock cycles per integration period
    CNTSH = ceil((float)integrationTime*1.16);
 //   Serial.print("Integration clocks: ");
 //   Serial.print(CNTSH);

    if (integrationTime > minFrameTime){
      CNTPF = integrationMClocks;
      ES = 1;    
    }
    else {
      ES = ceil((float)minFrameClocks/integrationMClocks);  
      CNTPF = ES*CNTSH;
    }
      USPF = CNTPF /1.16; // microseconds per frame
    Serial.print("/Integration time (us): ");
    Serial.print(integrationTime);
    Serial.print(", Int periods / frame: ");
    Serial.print(ES);
//    Serial.print(", Clocks per frame: ");
//    Serial.print(CNTPF);
    Serial.print(", Microseconds per frame: ");
    Serial.println(USPF);

  setupClocks();  // is this correct?
  checkFlag = true;
  }  // end of set integration time
  
  else if(c == 'L'){
    loops = Serial.parseInt();
    Serial.print("/Number of loops: ");
    Serial.println(loops);
  }  // end of set loops
  
  else if(c =='&'){ // start of run
    counter = -32;
    loopCounter = 0;
    runFlag = true;
    Serial.println("/run flag set");
    //loops+=2;
  }  // end of start run
  
  else if (c == 'S'){
    runFlag = false;
    Serial.println("/run flag cleared");
  }  // end of stop run
  
  else if (c == '*'){  // user looking for spectrometer.
   //// instrument responds with '&' followed by 4 char serial no
   ///   check EEProm to see if ref voltages set
    char f  = EEPROM.read(0);
    if(f== 'Y' && setRef == false){
      lowRefV = EEPROM.read(6);
      lowRefV *= 255;
      lowRefV += EEPROM.read(7);
      highRefV = EEPROM.read(8);  
      highRefV *= 255;
      highRefV += EEPROM.read(9);
      
      if ( lowRefV < 5 ||lowRefV >1000){
        lowRefV = 420;
      }
      if ( highRefV < 5 ||highRefV >1000){
        highRefV = 1000;
      }
      
      analogWrite(lowRefPin,lowRefV);
      analogWrite(highRefPin,highRefV);
      setRef = true;
    }
 /*     Serial.print("/ low ref v: ");
      Serial.print(lowRefV);
      Serial.print(", high ref v: ");
      Serial.println(highRefV);  */


 //      f = '&';  // what is this?
    Serial.print('&');
    for(int b = 1; b<5; b++){  // Read 4 digit serial number from EEPROM and send to user
      char f = EEPROM.read(b);
      Serial.print(f);
    }
    Serial.println(""); 
    Serial.print('B'); 
      for(int b = 10; b<32; b++){  // Read 6 digit baseline values from EEPROM and send to user
      char f = EEPROM.read(b);
      Serial.print(f);
    }
    Serial.println("");  
    
  }
  else if (c=='N'){ // put 4 char serial number into EEPROM
    char d[4];
    for (int e = 1; e<5; e++){
      d[e] = Serial.read();
      EEPROM.write(e,d[e]);
    }
    
  }
   else if (c=='R'){ // put reference voltages into EEPROM
   int low = Serial.parseInt();
   int low0 = low/255;
   int low1 = low%255;
 
   int high = Serial.parseInt();
   int high0 = high/255;
   int high1 = high%255;
   EEPROM.write(0,'Y');
   EEPROM.write(6,low0);
   EEPROM.write(7,low1);
   EEPROM.write(8,high0);
   EEPROM.write(9,high1);
   analogWriteResolution(10);
   analogWriteFrequency(lowRefPin,146484.38);  // should not have to re-enter this?
   analogWriteFrequency(highRefPin,146484.38);
   analogWrite(lowRefPin,low);
   analogWrite(highRefPin,high);
}
else if (c=='B'){  // write baseline calibration values to EEPROM
  char c;
  for (int t = 10; t<28; t++){    // get baseline values
         c = Serial.read();
        EEPROM.write(t,c);     // ABC[0] in registers 10 to 15, ABC[1] in 16 to 21
    
    }
  }
else if (c=='@'){
  Serial.print('B');
for(int v = 10; v<28; v++){
  char f = EEPROM.read(v);
  Serial.print(f);
}
Serial.println("");
  
}
}
