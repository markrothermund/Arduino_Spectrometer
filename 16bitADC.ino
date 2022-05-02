
/*********************************************
 * 16bitADC tab
 * readADC method is interrupt service routine 
 * based on flexpwm2_3_isr from ntyrell githup code
 * reads every 4 CCD clocks  
 * 
 * outputData method averages data and sends to host computer over usb
 * 
 */

void readADC(){
FLEXPWM2_SM3STS |= 12;  // clear all set bits
 // uncomment to test loop, look for toggling on scope at 8 CCD clock period
/*  if(a){ 
    digitalWrite(CSPin,HIGH);
  }else{
    digitalWrite(CSPin,LOW);
  }
  a=!a;  */
 // checkFlag = true;
 /*if(counter%10 == 0 && runFlag){  // this shows that counter is counting
  Serial.print("counter");
 } */
 uint16_t point = 0;
 SPI.beginTransaction(SPISettings(20000000,MSBFIRST,SPI_MODE0));
 digitalWrite(CSPin,LOW);
 point = SPI.transfer16(0);
 digitalWrite(CSPin,HIGH);
 SPI.endTransaction();

 if((ICGFlag) && (counter>-1) && (counter<PIXELS-1)){  // only collect real data
    if(loopCounter < 2) {   // do not collect during first loop
      data[counter] = 0;
      }
    else{       // sum data in subsequent loops
      data[counter] += point;
      }  
  }
      counter++;  // needs to be outside of loop above
 if(counter == PIXELS-1){   //if we have collected all of the data for this loop
  ICGFlag = false;         // end of read period for this loop
  if(loopCounter == loops+1){  // and if we have completed all the loops
   runFlag = false;        // run is finished
   Serial.println("/run finished");
   outputData(); 
  }
  // loop counter incremented in ICG_isr
    //loopCounter++;  // if we still have more loops to go, increment loop counter
  
 }
}

void outputData(){
 // byte a,b;
 int c = 0;
  while(c < PIXELS){
 // for(int c = 0; c < PIXELS; c++){
    data[c]/= loops;   // get average of loops
//  a = data[c]/255;  // >> 8;  
  //b = data[c]%255;
 
  Serial.print(c);
  Serial.print(", ");
  Serial.println(data[c]);
  delayMicroseconds(200);
  //Serial.print(a);
//  Serial.print(", ");
  //Serial.println(b);  
  c++;
  }
  Serial.println("!!!!");
  digitalWrite(UVLED, LOW);
}
