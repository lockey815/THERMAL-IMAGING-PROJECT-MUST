# THERMAL-IMAGING-PROJECT-MUST

/**
 * THERMAL CAMERA PROJECT
 * Hardware: AMG8833 (8x8 Sensor) + ILI9341 (320x240 Display)
 * 
 * Features: 
 * - Linear interpolation (Upscaling 8x8 to 70x70)
 * - Dynamic color scaling
 * - Battery monitoring
 * - Center-point measurement
 */

// ==========================================
// 1. LIBRARIES & HARDWARE COMPATIBILITY
// ==========================================
#ifdef ESP8266
#include <TFT_eSPI.h>               // Fast driver for ESP8266
#else
#include <Fonts/FreeMonoBoldOblique12pt7b.h>
#endif
#include <Adafruit_ILI9341.h>       // Standard driver for Arduino/Teensy
#include <Adafruit_AMG88xx.h>       // Panasonic Grid-EYE library

// ==========================================
// 2. PIN DEFINITIONS & UI CONSTANTS
// ==========================================
#define TFT_CS   PIN_D6
#define TFT_DC   PIN_D4
#define TFT_RST  PIN_D3
#define PIN_INT A0                  // Used for mode switching/interaction

// Keypad layout dimensions
#define KEYPAD_TOP 15
#define KEYPAD_LEFT 50
#define BUTTON_W 60
#define BUTTON_H 30
#define BUTTON_SPACING_X 10
#define BUTTON_SPACING_Y 10
#define BUTTON_TEXTSIZE 2

// ==========================================
// 3. DISPLAY & COLOR INITIALIZATION
// ==========================================
#ifdef ESP8266
TFT_eSPI Display = TFT_eSPI();
#else
Adafruit_ILI9341 Display = Adafruit_ILI9341(PIN_CS, PIN_DC);
#endif

// Define 16-bit 565 colors for the UI
#define C_BLUE Display.color565(0,0,255)
#define C_RED Display.color565(255,0,0)
#define C_GREEN Display.color565(0,255,0)
#define C_WHITE Display.color565(255,255,255)
#define C_BLACK Display.color565(0,0,0)
#define C_LTGREY Display.color565(200,200,200)
#define C_DKGREY Display.color565(80,80,80)
#define C_GREY Display.color565(127,127,127)

// ==========================================
// 4. GLOBAL STATE & DATA ARRAYS
// ==========================================
boolean measure = true;
uint16_t centerTemp;
unsigned long tempTime = millis();
unsigned long batteryTime = 1;
#define METRIC;

// Thermal scale range (Celsius)
uint16_t MinTemp = 25;
uint16_t MaxTemp = 35;

// Variables for RGB interpolation logic
byte red, green, blue;
byte i, j, k, row, col, incr;
float intPoint, val, a, b, c, d, ii;
byte aLow, aHigh;

// Display "Pixel" size for the thermal grid
byte BoxWidth = 3;
byte BoxHeight = 3;
int ShowGrid = -1;
int DefaultTemp = -1;

float pixels[64];       // Stores raw 8x8 sensor readings
float HDTemp[80][80];   // Stores upscaled 70x70 data points

// Hardware Objects
Adafruit_GFX_Button KeyPadBtn[12];
Adafruit_AMG88xx ThermalSensor;

// ==========================================
// 5. SYSTEM SETUP
// ==========================================
void setup() {
  Serial.begin(115200);
  pinMode(A0, INPUT);

  Display.begin();
  Display.fillScreen(C_BLACK);
  Display.setRotation(0);

  // Render Splash Screen
  Display.setTextSize(2);
  Display.setCursor(62, 61);
  Display.setTextColor(C_WHITE, C_BLACK);
  Display.print("Thermal");
  Display.setCursor(60, 60);
  Display.setTextColor(C_BLUE);
  Display.print("Thermal");
  Display.setCursor(92, 101);
  Display.setTextColor(C_WHITE, C_BLACK);
  Display.print("Camera");
  Display.setCursor(90, 100);
  Display.setTextColor(C_RED);
  Display.print("Camera");

  // Initialize Thermal Sensor
  bool status = ThermalSensor.begin();
  delay(100);

  // Critical Error Check: Sensor Connection
  if (!status) {
    while (1) {
      Display.setCursor(20, 180);
      Display.setTextColor(C_RED, C_BLACK);
      Display.print("Sensor: FAIL");
      delay(500);
      Display.setCursor(20, 180);
      Display.setTextColor(C_BLACK, C_BLACK);
      Display.print("Sensor: FAIL");
      delay(500);
    }
  } else {
    Display.setCursor(20, 180);
    Display.setTextColor(C_GREEN, C_BLACK);
    Display.print("Sensor: FOUND");
  }

  // Critical Error Check: Data Integrity
  ThermalSensor.readPixels(pixels);
  if (pixels[0] < 0) {
    while (1) {
      Display.setCursor(20, 210);
      Display.setTextColor(C_RED, C_BLACK);
      Display.print("Readings: FAIL");
      delay(500);
      Display.setCursor(20, 210);
      Display.setTextColor(C_BLACK, C_BLACK);
      Display.print("Readings: FAIL");
      delay(500);
    }
  } else {
    Display.setCursor(20, 210);
    Display.setTextColor(C_GREEN, C_BLACK);
    Display.print("Readings: OK");
    delay(2000);
  }

  Display.fillScreen(C_BLACK);
  Getabcd();      // Setup initial color thresholds
  DrawLegend();   // Render color scale on side/bottom
  Display.fillRect(10, 10, 220, 220, C_WHITE); // Thermal Viewport Border
}

// ==========================================
// 6. MAIN EXECUTION LOOP
// ==========================================
void loop() {
  // Mode Selection: Check trigger for Auto-scaling/Toggle
  if (digitalRead(PIN_INT) == false) {
    SetTempScale();
    if (millis() - tempTime > 2000) {
      measure = !measure;
      tempTime = millis();
      Display.fillRect (0, 300, 100, 16, ILI9341_BLACK);
    }
  } else {
    tempTime = millis();
  }

  // 1. Capture: Get 8x8 raw data
  ThermalSensor.readPixels(pixels);

  // 2. Process: Interpolate rows then columns to upscale image
  InterpolateRows();
  InterpolateCols();

  // 3. Render: Draw the processed grid to the display
  DisplayGradient();

  // 4. Power Monitor: Update battery status every 30s
  if (batteryTime < millis()) {
    drawBattery();
    batteryTime = millis() + 30000;
  }
}

// ==========================================
// 7. MATHEMATICAL INTERPOLATION
// ==========================================

// Upscale the 8 horizontal pixels into 70 data points
void InterpolateRows() {
  for (row = 0; row < 8; row ++) {
    for (col = 0; col < 70; col ++) {
      aLow =  col / 10 + (row * 8);
      aHigh = (col / 10) + 1 + (row * 8);
      intPoint = (( pixels[aHigh] - pixels[aLow] ) / 10.0 );
      incr = col % 10;
      val = (intPoint * incr ) +  pixels[aLow];
      HDTemp[ (7 - row) * 10][col] = val; // Transpose and store
    }
  }
}

// Upscale the rows vertically to complete the 70x70 grid
void InterpolateCols() {
  for (col = 0; col < 70; col ++) {
    for (row = 0; row < 70; row ++) {
      aLow =  (row / 10 ) * 10;
      aHigh = aLow + 10;
      intPoint = (( HDTemp[aHigh][col] - HDTemp[aLow][col] ) / 10.0 );
      incr = row % 10;
      val = (intPoint * incr ) +  HDTemp[aLow][col];
      HDTemp[ row ][col] = val;
    }
  }
}

// ==========================================
// 8. GRAPHICS & RENDERING
// ==========================================

// Draw the interpolated data as colorful blocks on screen
void DisplayGradient() {
  for (row = 0; row < 70; row ++) {
    // Determine block size (reduced for grid lines if ShowGrid is active)
    if (ShowGrid < 0) {
      BoxWidth = 3;
    } else {
      BoxWidth = (row % 10 == 9) ? 2 : 3;
    }

    for (col = 0; col < 70; col++) {
      if (ShowGrid < 0) {
        BoxHeight = 3;
      } else {
        BoxHeight = (col % 10 == 9) ? 2 : 3;
      }
      
      // Calculate color and draw pixel block
      Display.fillRect((row * 3) + 15, (col * 3) + 15, BoxWidth, BoxHeight, GetColor(HDTemp[row][col]));

      // Overlay center measurement crosshair
      if (measure == true && row == 36 && col == 36) {
        drawMeasurement();
      }
    }
  }
}

// Convert temperature values to 16-bit RGB (Cold = Blue, Hot = Red)
uint16_t GetColor(float val) {
  red = constrain(255.0 / (c - b) * val - ((b * 255.0) / (c - b)), 0, 255);

  if ((val > MinTemp) & (val < a)) {
    green = constrain(255.0 / (a - MinTemp) * val - (255.0 * MinTemp) / (a - MinTemp), 0, 255);
  } else if ((val >= a) & (val <= c)) {
    green = 255;
  } else if (val > c) {
    green = constrain(255.0 / (c - d) * val - (d * 255.0) / (c - d), 0, 255);
  } else if ((val > d) | (val < a)) {
    green = 0;
  }

  if (val <= b) {
    blue = constrain(255.0 / (a - b) * val - (255.0 * b) / (a - b), 0, 255);
  } else if ((val > b) & (val <= d)) {
    blue = 0;
  } else if (val > d) {
    blue = constrain(240.0 / (MaxTemp - d) * val - (d * 240.0) / (MaxTemp - d), 0, 240);
  }

  return Display.color565(red, green, blue);
}

// ==========================================
// 9. TEMPERATURE SCALE & UI HELPERS
// ==========================================

// Automatically adjust the color range based on current scene
void SetTempScale() {
  if (false) { 
    MinTemp = 25;
    MaxTemp = 35;
    Getabcd();
    DrawLegend();
  } else {
    MinTemp = 255;
    MaxTemp = 0;

    // Find min and max in the raw 8x8 data
    for (i = 0; i < 64; i++) {
      if ((uint16_t)pixels[i] < MinTemp) { MinTemp = (uint16_t)pixels[i];}
      if ((uint16_t)pixels[i] > MaxTemp) { MaxTemp = (uint16_t)pixels[i];}
    }

    // Add padding to the scale for stability
    MaxTemp = MaxTemp + 5.0;
    MinTemp = MinTemp + 3.0;
    Getabcd();
    DrawLegend();
  }
}

// Calculate the 4 internal transition points for the RGB gradient
void Getabcd() {
  a = MinTemp + (MaxTemp - MinTemp) * 0.2121;
  b = MinTemp + (MaxTemp - MinTemp) * 0.3182;
  c = MinTemp + (MaxTemp - MinTemp) * 0.4242;
  d = MinTemp + (MaxTemp - MinTemp) * 0.8182;
}

// Render the visual color scale legend at bottom of screen
void DrawLegend() {
  j = 0;
  float inc = (MaxTemp - MinTemp ) / 220.0;

  for (ii = MinTemp; ii < MaxTemp; ii += inc) {
    Display.drawFastVLine(10 + j++, 255, 30, GetColor(ii));
  }

  #ifdef IMPERIAL
    MinTemp = MinTemp * 1.8 + 32;
    MaxTemp = MaxTemp * 1.8 + 32;
  #endif

  int xpos = (MaxTemp > 99) ? 184 : 196;

  // Print Min/Max labels
  Display.setTextSize(2);
  Display.setCursor(10, 235);
  Display.setTextColor(C_WHITE, C_BLACK);
  sprintf(buf, "%2d", MinTemp);
  Display.print(buf);

  Display.setCursor(xpos, 235);
  Display.setTextColor(C_WHITE, C_BLACK);
  sprintf(buf, " %2d", MaxTemp);
  Display.print(buf);
}

// Display temperature of the center pixel
void drawMeasurement() {
  Display.drawCircle(120, 120, 3, ILI9341_WHITE);
  centerTemp = pixels[27]; // Index 27 is roughly the center of 8x8 grid

  #ifdef IMPERIAL
    centerTemp = centerTemp * 1.8 + 32;
  #endif
  
  Display.setCursor(10, 300);
  Display.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  Display.setTextSize(2);
  sprintf(buf, "%s:%2d", "Temp", centerTemp);
  Display.print(buf);
}

// ==========================================
// 10. POWER MANAGEMENT
// ==========================================

// Read analog voltage from A0
int measureBattery() {
  uint16_t adcValue = analogRead(A0);
  return adcValue / 102.3 * 4.5;
}

// Render a battery icon with color coding (Green = OK, Red = Low)
void drawBattery()  {
  int volt = measureBattery() - 32; // Offset for Lithium range
  volt = constrain (volt, 1, 10);

  Display.drawRect(198, 304, 30, 10, C_WHITE);
  Display.fillRect(227, 306, 3, 6, C_WHITE);
  Display.fillRect(199, 305, 28, 8, C_BLACK);
  
  uint16_t bColor = (volt > 3) ? C_GREEN : C_RED;
  Display.fillRect(199, 305, volt * 3 - 2, 8, bColor);
}
