/****************************************
Scrolling Sound Meter Sketch for the 
Adafruit Microphone Amplifier
****************************************/
 
#include <Wire.h>
#include <sound_meter.h> 

const int sampleWindow = 50; // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;


 
void setup() 
{
   Serial.begin(9600);

}

int tempo = 0;
 
void loop() 
{
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

   tempo+= 50;
   peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
   double volts = (peakToPeak * 5.0) / 1024;  // convert to volts
 
   if(volts > 0.1){
      double *pVolts = &volts;
       Serial.println(volts);
       Serial.println(get_abs_db(pVolts));
       Serial.println("Tempo (ms): " + tempo);
   }
   
   else if(volts >= 0.1 && volts <= 1){
       //Serial.println(volts);
       //Serial.println(get_abs_db(volts));
   }
   else if(volts >= 2){
       
   }

   
  
}
