/*
 * Open Source Magnetometer Arduino Code
 * based on Adafruit SSD1306 example program
 * 
 * by Lionel Birglen
 * Polytechnique Montreal
 * 2021
*/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for SSD1306 display connected using software SPI (default case):
#define OLED_MOSI   9
#define OLED_CLK   10
#define OLED_DC    11
#define OLED_CS    12
#define OLED_RESET 13
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
  OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

//Custom variables for magnetometer
int compte=30;
float zero;
int calibration=0;
int batterie=0;
float mini=0.87;
float maxi=4.20;


void setup() {
  //Initialize serial communication
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Clear the buffer
  display.clearDisplay();

  //Display that calibration is running
  display.setCursor(0,0); 
  display.setTextSize(1); 
  display.setTextColor(SSD1306_WHITE);
  display.println("Calibration...");

  //read 30 data points to compute the zero G offset, sensor must be away from magnetic sources
  calibration=analogRead(A0);
  for (int i = 1; i < compte; i++) {
    calibration=calibration+analogRead(A0);
    delay(2);
  }
  Serial.println(calibration);
  display.print("Zero = ");
  display.println(calibration);
  display.display();

  //Wait 2 seconds
  delay(2000);
}


void loop() {
  int capteur;
  
  //Read the sensor
  capteur=analogRead(A0);
  
  //Read the sensor "compte-1" more times
  for (int i = 1; i < compte; i++) {
      capteur=capteur+analogRead(A0);
      delay(2);
    }
  
  //Average the values read both before and after calibration is taken into account
  float tension=capteur*(5.0/1023.0)/compte;
  float mesure=(capteur-calibration)*(5.0/1023.0)/compte;

  //Clear and setup the display
  display.clearDisplay(); 
  display.setCursor(0,0); 
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.print(tension);
  display.println("V");
  display.setTextSize(2);
  
  //Voltages can be below or above calibration value depending if a north or south pole is measured
  if (mesure>0) {
    display.print("+");
  }
  else {
    display.print("-");
  }
  //Display the Gauss value
  display.print(abs(mesure)/0.0025,1);
  display.println("G");

  //Warn if saturation is reached, displayed value is unreliable
  if (tension<mini || tension>maxi)  {
    display.print("SATURATION");
  }

  //Display everything and wait 20ms
  display.display();
  delay(20); 
}
