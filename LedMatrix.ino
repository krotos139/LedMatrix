#include "esp32_digital_led_lib.h"

#include "SSD1306.h" // alias for #include "SSD1306Wire.h"

/*
 * Simplest possible example shows a single strand of NeoPixels. See Demo1 for multiple strands and other devices.
 */

strand_t pStrand = {.rmtChannel = 0, .gpioNum = 16, .ledType = LED_WS2812B_V3, .brightLimit = 32, .numPixels = 64,
   .pixels = nullptr, ._stateVars = nullptr};

int stepper = 0;
int colord = 0;
uint32_t displayData[56][24];

SSD1306Wire  display(0x3c, 5, 4);

void setup()
{
  Serial.begin(115200);
  Serial.println("Initializing...");

  pinMode (16, OUTPUT);
  digitalWrite (16, LOW);

  if (digitalLeds_initStrands(&pStrand, 1)) {
    Serial.println("Init FAILURE: halting");
    while (true) {};
  }
  
  digitalLeds_resetPixels(&pStrand);

    // Initialising the UI will init the display too.
  display.init();
  display.setContrast(255);
  display.flipScreenVertically();
  //display.setFont(ArialMT_Plain_10);
}

uint8_t t=0;
void loop()
{
//    pStrand.pixels[stepper] = pixelFromRGBW(55, colord, 0, 0);
//
//    stepper++;
//    
//    if(stepper > 15) {
//      stepper = 0;
//      colord += 1;
//    }
//
//    if(colord > 60) 
//      colord = 0;
//    
//    digitalLeds_updatePixels(&pStrand);
  //colorWipe(255, 0, 0, 50); // Red
  //colorWipe(0, 255, 0, 50); // Green
  //colorWipe(0, 0, 255, 50); // Blue
  //theaterChase(pixelFromRGB(127, 127, 127), 250); // White
  //theaterChase(pixelFromRGB(127, 0, 0), 250); // Red
  //theaterChase(pixelFromRGB(0, 0, 127), 250); // Blue

  plazma();
  
  display.clear();

  for (int16_t x=0 ; x<56 ; x++) {
    for (int16_t y=0 ; y<24 ; y++) {
      if ((displayData[x][y] & 0xFF) > 100) {
        display.setPixel(x*2,y*2);
        //display.setPixel(x*2+1,y*2);
        //display.setPixel(x*2+1,y*2+1);
        //display.setPixel(x*2,y*2+1);
      }
      if (((displayData[x][y] >> 8) & 0xFF) > 100) {
        //display.setPixel(x*2,y*2);
        display.setPixel(x*2+1,y*2);
        //display.setPixel(x*2+1,y*2+1);
        //display.setPixel(x*2,y*2+1);
      }
      if (((displayData[x][y] >> 16) & 0xFF) > 100) {
        //display.setPixel(x*2,y*2);
        //display.setPixel(x*2+1,y*2);
        display.setPixel(x*2+1,y*2+1);
        //display.setPixel(x*2,y*2+1);
      }
    }
  }
  display.display();

  drawSegment(2, 1, 0);

  digitalLeds_updatePixels(&pStrand);
  //rainbow(t);
  
  delay(20);

  t++;
}

void drawSegment(int x, int y, int offset) {
  int px = x*8;
  int py = y*8;
  int pi = offset*64;
  for(int i=0; i<8; i++) {
      for(int k=0; k<8; k++) {
        int j = 0;
        if (i % 2 == 0) {
          j = k;
        } else {
          j = 7-k;
        }
        pStrand.pixels[i*8+k+pi].num =  displayData[j+px][i+py];
      }
    }
}

// Fill the dots one after the other with a color
void colorWipe(uint8_t r, uint8_t g, uint8_t b, uint8_t wait) {
  for(uint16_t i=0; i<pStrand.numPixels; i++) {
    pStrand.pixels[i] =  pixelFromRGB(r, g, b);
    digitalLeds_updatePixels(&pStrand);
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(pixelColor_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < pStrand.numPixels; i=i+3) {
        //strip.setPixelColor(i+q, c);    //turn every third pixel on
        pStrand.pixels[i+q] =  c;
      }
      digitalLeds_updatePixels(&pStrand);

      delay(wait);

      for (uint16_t i=0; i < pStrand.numPixels; i=i+3) {
        //strip.setPixelColor(i+q, 0);        //turn every third pixel off
        pStrand.pixels[i+q].num =  0;
      }
    }
  }
}

void rainbow(uint8_t j) {
  uint16_t i;
    for(i=0; i<pStrand.numPixels; i++) {
      //strip.setPixelColor(i, Wheel((i+j) & 255));
      pStrand.pixels[i].num =  Wheel((i*8+j) & 255);
    }
    digitalLeds_updatePixels(&pStrand);
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  uint32_t col = 0;
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    col = ((255 - WheelPos * 3) << 16) +  (0 << 8) +  ((WheelPos * 3) << 0);
    return col;
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    col = (0 << 16) +  ((WheelPos * 3) << 8) +  ((255 - WheelPos * 3) << 0);
    return col;
  }
  WheelPos -= 170;
  col = ((WheelPos * 3) << 16) +  ((255 - WheelPos * 3) << 8) +  (0 << 0);
  return col;
}

void flash(uint8_t wait) {
  uint16_t i, j;

    for(i=0; i<pStrand.numPixels; i++) {
      pStrand.pixels[i].num = 0x00FFFFFF;
    }
    digitalLeds_updatePixels(&pStrand);
    delay(5);
    for(i=0; i<pStrand.numPixels; i++) {
      pStrand.pixels[i].num =  0;
    }
    digitalLeds_updatePixels(&pStrand);
    delay(wait);
}

void drawFontFaceDemo() {
    // Font Demo1
    // create more fonts at http://oleddisplay.squix.ch/
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0, "Hello world");
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 10, "Hello world");
    display.setFont(ArialMT_Plain_24);
    display.drawString(0, 26, "Hello world");
}


float phase = 0.0;
float phaseIncrement = 0.03;  // Controls the speed of the moving points. Higher == faster. I like 0.08 .
float colorStretch = 0.3; // Higher numbers will produce tighter color bands. I like 0.11 .

struct Point {
  float x;
  float y;
};
// This function is called every frame.
void plazma() {
  uint32_t color;
  phase += phaseIncrement;
  
  // The two points move along Lissajious curves, see: http://en.wikipedia.org/wiki/Lissajous_curve
  // We want values that fit the LED grid: x values between 0..13, y values between 0..8 .
  // The sin() function returns values in the range of -1.0..1.0, so scale these to our desired ranges.
  // The phase value is multiplied by various constants; I chose these semi-randomly, to produce a nice motion.
  Point p1 = { (sin(phase*1.000)+1.0) * 4.5, (sin(phase*1.310)+1.0) * 4.0 };
  Point p2 = { (sin(phase*1.770)+1.0) * 4.5, (sin(phase*2.865)+1.0) * 4.0 };
  Point p3 = { (sin(phase*0.250)+1.0) * 4.5, (sin(phase*0.750)+1.0) * 4.0 };
  
  byte row, col;
  
  // For each row...
  for( row=0; row<56; row++ ) {
    float row_f = float(row)-28;  // Optimization: Keep a floating point value of the row number, instead of recasting it repeatedly.
    
    // For each column...
    for( col=0; col<24; col++ ) {
      float col_f = float(col)-12;  // Optimization.
      
      // Calculate the distance between this LED, and p1.
      Point dist1 = { col_f - p1.x, row_f - p1.y };  // The vector from p1 to this LED.
      float distance1 = sqrt( dist1.x*dist1.x + dist1.y*dist1.y )/2;
      
      // Calculate the distance between this LED, and p2.
      Point dist2 = { col_f - p2.x, row_f - p2.y };  // The vector from p2 to this LED.
      float distance2 = sqrt( dist2.x*dist2.x + dist2.y*dist2.y )/2;
      
      // Calculate the distance between this LED, and p3.
      Point dist3 = { col_f - p3.x, row_f - p3.y };  // The vector from p3 to this LED.
      float distance3 = sqrt( dist3.x*dist3.x + dist3.y*dist3.y )/2;
      
      // Warp the distance with a sin() function. As the distance value increases, the LEDs will get light,dark,light,dark,etc...
      // You can use a cos() for slightly different shading, or experiment with other functions. Go crazy!
      float color_1 = distance1;  // range: 0.0...1.0
      float color_2 = distance2;
      float color_3 = distance3;
      float color_4 = (sin( distance1 * distance2 * colorStretch )) + 2.0 * 0.5;
      
      // Square the color_f value to weight it towards 0. The image will be darker and have higher contrast.
      color_1 *= color_1 * color_4;
      color_2 *= color_2 * color_4;
      color_3 *= color_3 * color_4;
      color_4 *= color_4;
 
      // Scale the color up to 0..7 . Max brightness is 7.
      //strip.setPixelColor(col + (8 * row), strip.Color(color_4, 0, 0) );
      //strip.setPixelColor(col + (8 * row), strip.Color(color_1, color_2, color_3));
      color = color_1;
      color = (color << 8) + color_2;
      color = (color << 8) + color_3;
      displayData[row][col] = color;
    }
  }
  //strip.show();
}

