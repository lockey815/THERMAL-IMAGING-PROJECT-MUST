# 🧠 Thermal Camera Firmware – Code Explanation

This document explains the firmware used in the **ESP8266 Thermal Imaging Device**.  
The code reads temperature data from an **AMG8833 thermal sensor**, interpolates the data to increase resolution, and displays the result as a **heatmap on an ILI9341 TFT screen**.

---

# 1. Library Imports

```cpp
#ifdef ESP8266
#include <TFT_eSPI.h>
#else
#include <Fonts/FreeMonoBoldOblique12pt7b.h>
#endif
#include <Adafruit_ILI9341.h>
#include <Adafruit_AMG88xx.h>
```

### Purpose

These libraries enable communication with the hardware components.

| Library | Function |
|------|------|
| `TFT_eSPI` | Fast TFT display driver optimized for ESP8266 |
| `Adafruit_ILI9341` | Controls the TFT display |
| `Adafruit_AMG88xx` | Interface for AMG8833 thermal sensor |

The `#ifdef` directive ensures the correct display driver is used depending on the microcontroller.

---

# 2. Pin Definitions

```cpp
#define TFT_CS   PIN_D6
#define TFT_DC   PIN_D4
#define TFT_RST  PIN_D3
#define PIN_INT A0
```

### Purpose

Defines hardware connections between the **NodeMCU ESP8266** and the **TFT display**.

| Pin | Function |
|----|----|
| D6 | Chip Select |
| D4 | Data / Command control |
| D3 | Display reset |
| A0 | Analog input used for battery monitoring |

---

# 3. User Interface Constants

```cpp
#define KEYPAD_TOP 15
#define KEYPAD_LEFT 50
#define BUTTON_W 60
#define BUTTON_H 30
```

### Purpose

Defines layout properties for the **touch keypad interface**, including:

- Button size
- Button spacing
- Position on the display

---

# 4. Display Initialization

```cpp
#ifdef ESP8266
TFT_eSPI Display = TFT_eSPI();
#else
Adafruit_ILI9341 Display = Adafruit_ILI9341(PIN_CS, PIN_DC);
#endif
```

### Purpose

Creates the **Display object** used to draw graphics and text.

Example usage:

```cpp
Display.print()
Display.fillRect()
Display.drawLine()
```

---

# 5. Color Definitions

```cpp
#define C_BLUE Display.color565(0,0,255)
#define C_RED Display.color565(255,0,0)
#define C_GREEN Display.color565(0,255,0)
```

### Purpose

Defines commonly used UI colors using **RGB565 format**.

RGB565 is used because TFT displays operate with **16-bit color depth**.

| Color | Meaning |
|------|------|
| Blue | Low temperature |
| Green | Normal temperature |
| Red | High temperature |

---

# 6. Global Variables

```cpp
boolean measure = true;
uint16_t centerTemp;
unsigned long tempTime = millis();
```

### Purpose

Stores global system states such as:

- Whether temperature measurement is active
- Current center temperature
- Timing variables for updates

---

# 7. Thermal Data Arrays

```cpp
float pixels[64];
float HDTemp[80][80];
```

### Purpose

Stores temperature data from the sensor.

| Variable | Description |
|------|------|
| `pixels` | Raw 8×8 thermal data |
| `HDTemp` | Interpolated high-resolution matrix |

The interpolation process converts:

```
8 × 8 → 70 × 70
```

Which increases resolution from **64 points to ~4900 points**.

---

# 8. Hardware Objects

```cpp
Adafruit_GFX_Button KeyPadBtn[12];
Adafruit_AMG88xx ThermalSensor;
```

### Purpose

Creates objects representing:

- Touchscreen buttons
- Thermal sensor hardware

---

# 9. Setup Function

```cpp
void setup()
```

### Purpose

Runs once when the microcontroller starts.

### Main tasks

1. Initialize serial communication
2. Initialize the display
3. Show splash screen
4. Initialize thermal sensor
5. Verify sensor readings
6. Draw UI elements and temperature legend

If the sensor fails to initialize, the system enters a **warning loop**.

---

# 10. Main Loop

```cpp
void loop()
```

### Purpose

Runs continuously and performs the main device operations.

Main operations:

1. Detect user interaction
2. Read thermal sensor values
3. Interpolate data
4. Render heatmap
5. Update battery indicator

---

# 11. Row Interpolation

```cpp
void InterpolateRows()
```

### Purpose

Expands the **8×8 sensor grid horizontally**.

Process:

1. Select two neighboring pixels
2. Calculate intermediate values
3. Store them in the HDTemp array

Result:

```
8 columns → 70 columns
```

---

# 12. Column Interpolation

```cpp
void InterpolateCols()
```

### Purpose

Expands the data vertically.

Final matrix size:

```
70 × 70
```

This produces a **smooth thermal image**.

---

# 13. Heatmap Rendering

```cpp
void DisplayGradient()
```

### Purpose

Displays the interpolated temperature values on the TFT screen.

Steps:

1. Loop through the 70×70 matrix
2. Convert temperature to color
3. Draw small rectangles representing each pixel

---

# 14. Color Mapping

```cpp
uint16_t GetColor(float val)
```

### Purpose

Converts temperature values into RGB colors.

Color gradient:

| Temperature | Color |
|---|---|
| Low | Blue |
| Medium | Green |
| High | Red |

This helps highlight **thermal anomalies**.

---

# 15. Temperature Auto Scaling

```cpp
void SetTempScale()
```

### Purpose

Automatically adjusts the temperature display range.

Steps:

1. Scan all sensor pixels
2. Determine minimum and maximum values
3. Expand the range slightly
4. Recalculate color thresholds

---

# 16. Color Threshold Calculation

```cpp
void Getabcd()
```

### Purpose

Calculates four transition points:

```
a
b
c
d
```

These control the **color gradient transitions**.

---

# 17. Color Legend

```cpp
void DrawLegend()
```

### Purpose

Draws the **temperature color scale** at the bottom of the display.

The legend visually represents:

```
Minimum temperature → Maximum temperature
```

---

# 18. Center Temperature Measurement

```cpp
void drawMeasurement()
```

### Purpose

Displays the temperature at the **center pixel**.

Features:

- Crosshair marker
- Real-time temperature value

---

# 19. Battery Measurement

```cpp
int measureBattery()
```

### Purpose

Reads the battery voltage using the **ESP8266 analog input**.

---

# 20. Battery Indicator

```cpp
void drawBattery()
```

### Purpose

Displays a battery icon on the screen.

| Voltage Level | Color |
|---|---|
| High | Green |
| Low | Red |

---

# System Workflow

```
AMG8833 Sensor
      ↓
Read 8×8 Temperature Data
      ↓
Row Interpolation
      ↓
Column Interpolation
      ↓
70×70 Thermal Matrix
      ↓
Color Mapping
      ↓
Display Heatmap
```

---

# Final Capabilities

| Feature | Value |
|---|---|
| Sensor Resolution | 8 × 8 |
| Interpolated Resolution | 70 × 70 |
| Thermal Points | ~4900 |
| Display | ILI9341 TFT |
| Microcontroller | ESP8266 |

---

# Note

This firmware is designed for **thermal visualization and research purposes** and can be adapted for other applications such as:

- medical screening
- thermal monitoring
- robotics
- building inspection
