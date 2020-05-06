//premetto subito che questo codice presenta degli errori.
//progetto realizzato da Matteo Faggian.

//richiamo le librerie.
#include <CapacitiveSensor.h>
#include <Adafruit_NeoPixel.h> 
#include "FastLED.h"
#define NUM_LEDS 140 //definisco il numero di led impiegati.
CRGB leds[NUM_LEDS];

#ifdef __AVR__
#include <avr/power.h> //Molti AVR contengono un registro di riduzione della potenza (PRR) o registri (PRRx) che consente di ridurre il consumo di energia disabilitando o abilitando varie periferiche di bordo secondo necessità.
#endif

const int ledPin = 6;     //definisco dovè collegato il pin delle strisce.
const int numLeds = 140;  //definisco il numero di led impiegati.
int I;
CapacitiveSensor   cs_4_2 = CapacitiveSensor(4,2); //definisco i deu pin di collegamento per il sensore capacitivo.
Adafruit_NeoPixel strip = Adafruit_NeoPixel(numLeds, ledPin, NEO_GRB + NEO_KHZ800);
void setup()                    
{
   
  strip.begin();
  strip.setBrightness(155);  // imposto la luminostità massima (va da 0 a 256)
  cs_4_2.set_CS_AutocaL_Millis(0xFFFFFFFF);     //eseguo la calibrazione del sensore capacitivo.

  
}
 
void loop()                    
{
  
  
    long total1 =  cs_4_2.capacitiveSensor(30);
 
//inizio la funzione di switch.

if (total1>500){  
  
  if(++I > 4) I = 0;
}
delay(200); // per evitare il debounce.
Serial.println(total1);
switch(I){
 
        case 0:
          colorWipe(strip.Color(  0,   0,   0), 0);    // tutti i led spenti
          break;
        case 1:
             rainbow(0);  //modalità arcobaleno
          break;
        case 2:
          colorWipe(strip.Color(  255, 255, 255), 0); //colore bianco   
          break;
         
        case 3:
          Fire(55,70,20) ;  //modalità fuoco     
          break ;         
             
        case 4:           
           colorWipe(strip.Color(  125,  30, 0), 0); //colore luce calda
           break;
  
  

  
  }


}
//esco dal Loop e iniziano le varie funzioni delle animazioni.

void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}



// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(int wait) {
  
  // Hue of first pixel runs 3 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 3*65536. Adding 256 to firstPixelHue each time
  // means we'll make 3*65536/256 = 768 passes through this outer loop:
  for(long firstPixelHue = 0; firstPixelHue < 1*65536; firstPixelHue += 256) {
    for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the strip
      // (strip.numPixels() steps):
      int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the single-argument hue variant. The result
      // is passed through strip.gamma32() to provide 'truer' colors
      // before assigning to each pixel:
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
    strip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}

// Rainbow-enhanced theater marquee. Pass delay time (in ms) between frames.
void Fire(int Cooling, int Sparking, int SpeedDelay) {
  
  static byte heat[NUM_LEDS];
  int cooldown;
  
  // Step 1.  Cool down every cell a little
  for( int i = 0; i < NUM_LEDS; i++) {
    cooldown = random(0, ((Cooling * 10) / NUM_LEDS) + 2);
    
    if(cooldown>heat[i]) {
      heat[i]=0;
    } else {
      heat[i]=heat[i]-cooldown;
    }
   
  }
  
  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for( int k= NUM_LEDS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  
  }
    
  // Step 3.  Randomly ignite new 'sparks' near the bottom
  if( random(255) < Sparking ) {
    int y = random(7);
    heat[y] = heat[y] + random(160,255);
    //heat[y] = random(160,255);
  }

  // Step 4.  Convert heat to LED colors
  for( int j = 0; j < NUM_LEDS; j++) {
    setPixelHeatColor(j, heat[j] );
  
  }

  showStrip();
  delay(SpeedDelay);
}

void setPixelHeatColor (int Pixel, byte temperature) {
  // Scale 'heat' down from 0-255 to 0-191
  byte t192 = round((temperature/255.0)*191);
 
  // calculate ramp up from
  byte heatramp = t192 & 0x3F; // 0..63
  heatramp <<= 2; // scale up to 0..252
 
  // figure out which third of the spectrum we're in:
  if( t192 > 0x80) {                     // hottest
    setPixel(Pixel, 255, 255, heatramp);
  } else if( t192 > 0x40 ) {             // middle
    setPixel(Pixel, 255, heatramp, 0);
  } else {                               // coolest
    setPixel(Pixel, heatramp, 0, 0);
  }
}
// *** REPLACE TO HERE ***

void showStrip() {
 #ifdef ADAFRUIT_NEOPIXEL_H 
   // NeoPixel
   strip.show();
 #endif
 #ifndef ADAFRUIT_NEOPIXEL_H
   // FastLED
   FastLED.show();
 #endif
}

void setPixel(int Pixel, byte red, byte green, byte blue) {
 #ifdef ADAFRUIT_NEOPIXEL_H 
   // NeoPixel
   strip.setPixelColor(Pixel, strip.Color(red, green, blue));
 #endif
 #ifndef ADAFRUIT_NEOPIXEL_H 
   // FastLED
   leds[Pixel].r = red;
   leds[Pixel].g = green;
   leds[Pixel].b = blue;
 #endif
}

void setAll(byte red, byte green, byte blue) {
  for(int i = 0; i < NUM_LEDS; i++ ) {
    setPixel(i, red, green, blue); 
  delay(500);
  }
  showStrip();
}
