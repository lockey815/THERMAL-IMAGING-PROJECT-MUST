// Libraries
#ifdef ESP8266
#include <TFT_eSPI.h>
#else
#include <Fonts/FreeMonoBoldOblique12pt7b.h>
#endif
#include <Adafruit_ILI9341.h>
#include <Adafruit_AMG88xx.h>

// Pin Definitions
#define TFT_CS   PIN_D6
#define TFT_DC   PIN_D4
#define TFT_RST  PIN_D3
#define PIN_INT A0

// Keypad UI Constants
#define KEYPAD_TOP 15
#define KEYPAD_LEFT 50
#define BUTTON_W 60
#define BUTTON_H 30
#define BUTTON_SPACING_X 10
#define BUTTON_SPACING_Y 10
#define BUTTON_TEXTSIZE 2

// Display Driver Initialization
#ifdef ESP8266
TFT_eSPI Display = TFT_eSPI();
#else
Adafruit_ILI9341 Display = Adafruit_ILI9341(PIN_CS, PIN_DC);
#endif

// UI Colors
#define C_BLUE Display.color565(0,0,255)
#define C_RED Display.color565(255,0,0)
#define C_GREEN Display.color565(0,255,0)
#define C_WHITE Display.color565(255,255,255)
#define C_BLACK Display.color565(0,0,0)
#define C_LTGREY Display.color565(200,200,200)
#define C_DKGREY Display.color565(80,80,80)
#define C_GREY Display.color565(127,127,127)

// Global Variables
boolean measure = true;
uint16_t centerTemp;
unsigned long tempTime = millis();
unsigned long batteryTime = 1;
#define METRIC;

// Keypad Setup
char KeyPadBtnText[12][5] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "Done", "0", "Clr" };
uint16_t KeyPadBtnColor[12] = {C_BLUE, C_BLUE, C_BLUE, C_BLUE, C_BLUE, C_BLUE, C_BLUE, C_BLUE, C_BLUE, C_GREEN, C_BLUE, C_RED };

// Thermal Settings
uint16_t MinTemp = 25;
uint16_t MaxTemp = 35;

// Interpolation & Rendering Variables
byte red, green, blue;
byte i, j, k, row, col, incr;
float intPoint, val, a, b, c, d, ii;
byte aLow, aHigh;
byte BoxWidth = 3;
byte BoxHeight = 3;
int x, y;
char buf[20];
int ShowGrid = -1;
int DefaultTemp = -1;

float pixels[64];       // Raw 8x8 sensor data
float HDTemp[80][80];   // Interpolated 70x70 data

// Object Instances
Adafruit_GFX_Button KeyPadBtn[12];
Adafruit_AMG88xx ThermalSensor;

void setup() {
  Serial.begin(115200);
  pinMode(A0, INPUT);

  Display.begin();
  Display.fillScreen(C_BLACK);
  Display.setRotation(0);

  // Splash Screen
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

  // Sensor Initialization
  bool status = ThermalSensor.begin();
  delay(100);

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

  // Initial Read Check
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
  Getabcd();      // Initialize color thresholds
  DrawLegend();   // Draw color scale
  Display.fillRect(10, 10, 220, 220, C_WHITE); // Frame for thermal view
}

void loop() {
  // Handle interaction and toggle measurement
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

  // Process Thermal Data
  ThermalSensor.readPixels(pixels);
  InterpolateRows();
  InterpolateCols();
  DisplayGradient();

  // Periodic battery update
  if (batteryTime < millis()) {
    drawBattery();
    batteryTime = millis() + 30000;
  }
}

// Expand 8 rows of data into 70 columns
void InterpolateRows() {
  for (row = 0; row < 8; row ++) {
    for (col = 0; col < 70; col ++) {
      aLow =  col / 10 + (row * 8);
      aHigh = (col / 10) + 1 + (row * 8);
      intPoint = (( pixels[aHigh] - pixels[aLow] ) / 10.0 );
      incr = col % 10;
      val = (intPoint * incr ) +  pixels[aLow];
      HDTemp[ (7 - row) * 10][col] = val;
    }
  }
}

// Expand columns to create final 70x70 grid
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

// Render the thermal array to the screen
void DisplayGradient() {
  for (row = 0; row < 70; row ++) {
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
      
      Display.fillRect((row * 3) + 15, (col * 3) + 15, BoxWidth, BoxHeight, GetColor(HDTemp[row][col]));

      if (measure == true && row == 36 && col == 36) {
        drawMeasurement();
      }
    }
  }
}

// Map temperature value to 16-bit RGB color
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

// Auto-adjust min/max scale based on sensor range
void SetTempScale() {
  if (false) { 
    MinTemp = 25;
    MaxTemp = 35;
    Getabcd();
    DrawLegend();
  } else {
    MinTemp = 255;
    MaxTemp = 0;

    for (i = 0; i < 64; i++) {
      if ((uint16_t)pixels[i] < MinTemp) { MinTemp = (uint16_t)pixels[i];}
      if ((uint16_t)pixels[i] > MaxTemp) { MaxTemp = (uint16_t)pixels[i];}
    }

    MaxTemp = MaxTemp + 5.0;
    MinTemp = MinTemp + 3.0;
    Getabcd();
    DrawLegend();
  }
}

// Calculate color transition points
void Getabcd() {
  a = MinTemp + (MaxTemp - MinTemp) * 0.2121;
  b = MinTemp + (MaxTemp - MinTemp) * 0.3182;
  c = MinTemp + (MaxTemp - MinTemp) * 0.4242;
  d = MinTemp + (MaxTemp - MinTemp) * 0.8182;
}

// Draw UI color scale legend
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

// Display center-point temperature
void drawMeasurement() {
  Display.drawCircle(120, 120, 3, ILI9341_WHITE);
  centerTemp = pixels[27];

  #ifdef IMPERIAL
    centerTemp = centerTemp * 1.8 + 32;
  #endif
  
  Display.setCursor(10, 300);
  Display.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  Display.setTextSize(2);
  sprintf(buf, "%s:%2d", "Temp", centerTemp);
  Display.print(buf);
}

// Calculate battery voltage via A0
int measureBattery() {
  uint16_t adcValue = analogRead(A0);
  return adcValue / 102.3 * 4.5;
}

// Draw battery status icon
void drawBattery()  {
  int volt = measureBattery() - 32; 
  volt = constrain (volt, 1, 10);

  Display.drawRect(198, 304, 30, 10, C_WHITE);
  Display.fillRect(227, 306, 3, 6, C_WHITE);
  Display.fillRect(199, 305, 28, 8, C_BLACK);
  
  uint16_t bColor = (volt > 3) ? C_GREEN : C_RED;
  Display.fillRect(199, 305, volt * 3 - 2, 8, bColor);
}
