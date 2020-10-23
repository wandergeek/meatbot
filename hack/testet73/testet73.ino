 #include <ET73.h> // First we must include the ET73 Library found here
 
 #define MEAT_PROBE_PIN 34 //Now we define which analog pin we're going to read our probe from
 
 // Now we define an ET73 probe called meatProbe, and pass it the pin as the first argument, the rest of the arguments are optional
 // ET73( int probe pin, int Number of samples (optional, default:10) ,  int sample interval in ms (optional, default:10) , bool debug (optional, default:false) )
 ET73 meatProbe(MEAT_PROBE_PIN, 20, 200, false);  
 
 
 void setup() {
   Serial.begin(9600); //Start the Serial for testing purposes  
 }
 
 void loop() { 
   float myTemp = meatProbe.getTemp_C();  

   Serial.print("Temp: ");
   Serial.print(myTemp);
   Serial.print(" C\n");  

}
