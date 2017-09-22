/****************************************
Scrolling Sound Meter Sketch for the 
Adafruit Microphone Amplifier
****************************************/
 
#include <Wire.h>
 
const int maxScale = 8;
const int redZone = 5;
 
const int sampleWindow = 50; // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;
 
void setup() 
{
   Serial.begin(9600);
 
   //matrix.begin(0x70);  // pass in the address
}
 
 
void loop() 
{
  //Serial.println(Serial.list());
   unsigned long startMillis= millis();  // Start of sample window
   unsigned int peakToPeak = 0;   // peak-to-peak level
 
   unsigned int signalMax = 0;
   unsigned int signalMin = 1024;
 
   while (millis() - startMillis < sampleWindow)
   {
      sample = analogRead(0); 
      if (sample < 1024)  // toss out spurious readings
      {
         if (sample > signalMax)
         {
            signalMax = sample;  // save just the max levels
         }
         else if (sample < signalMin)
         {
            signalMin = sample;  // save just the min levels
         }
      }
   }
   peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
   double volts = (peakToPeak * 5.0) / 1024;  // convert to volts
 
   if(volts < 0.1){
      Serial.println("Silence");
   }
   
   else if(volts >= 0.1 && volts <= 1){
       Serial.println(volts);
   }
   else if(volts >= 2){
       Serial.println(volts);
   }

   
  
}
