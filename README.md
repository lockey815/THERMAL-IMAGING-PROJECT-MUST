# 🌡️ AMG8833 Thermal Camera with HD Interpolation

A high-performance DIY thermal imaging camera project using the **Panasonic AMG8833 Grid-EYE** sensor and an **ILI9341 TFT display**. This project features real-time bilinear interpolation to upscale a raw 8x8 sensor grid into a smooth 70x70 thermal heat map.

## 🚀 Features
*   **HD Upscaling:** Interpolates 64 raw pixels into 4,900 smoothed pixels for a "high-definition" thermal view.
*   **Dynamic Color Scaling:** Automatically adjusts the color palette (Blue-Green-Red) based on the scene's temperature range.
*   **Center-Point Measurement:** Displays the exact temperature of the object in the crosshair.
*   **Dual Platform Support:** Optimized for **ESP8266** (using `TFT_eSPI`) and **Arduino/Teensy**.
*   **Battery Monitoring:** Visual battery indicator with color-coded status (Green/Red) via analog pin A0.
*   **Splash Screen:** Professional boot sequence with sensor and reading health checks.

---

## 🛠️ Hardware Requirements
| Component | Specification |
| :--- | :--- |
| **Sensor** | Panasonic AMG8833 (8x8 Infrared Array) |
| **Display** | ILI9341 240x320 SPI TFT LCD |
| **MCU** | ESP8266 (NodeMCU/Wemos) or Arduino/Teensy |
| **Battery Sensing** | 130kOhm resistor divider to Pin A0 |

### Wiring (ESP8266 Example)
*   **TFT_CS:** D6
*   **TFT_DC:** D4
*   **TFT_RST:** D3
*   **Sensor:** I2C (SDA/SCL)
*   **User Button:** A0 (Shared with battery sense)

---

## 📂 Project Logic

### 1. The Interpolation Engine
The AMG8833 sensor only outputs an 8x8 grid. To avoid a "blocky" look, the code uses two specific functions:
*   `InterpolateRows()`: Adds 10 data points between every horizontal sensor pixel.
*   `InterpolateCols()`: Adds 10 data points between every vertical pixel.
This creates a **70x70** matrix, resulting in a smooth gradient that is much easier to interpret.

### 2. Adaptive Temperature Scale
The function `SetTempScale()` scans the current environment and identifies the hottest and coldest points. It then re-calculates the color thresholds (`Getabcd()`) so that the camera always provides the best possible contrast, regardless of the ambient temperature.

### 3. Visual Feedback
*   **Crosshair:** Measured at the center of the 8x8 grid (Index 27).
*   **Legend:** A vertical color bar at the bottom showing the current `Min` and `Max` temperature assignments.

---

## ⚙️ Software Setup

### Required Libraries
Ensure you have the following installed in your Arduino IDE:
1.  `Adafruit_AMG88xx` (Sensor)
2.  `Adafruit_GFX` (Graphics Core)
3.  `Adafruit_ILI9341` (Display driver for Arduino)
4.  `TFT_eSPI` (High-speed driver for ESP8266)

### Installation
1.  Clone the repository:
    ```bash
    git clone https://github.com/YOUR_USERNAME/Thermal-Camera.git
    ```
2.  Open the `.ino` file in Arduino IDE.
3.  Adjust the `TFT_eSPI` user setup file if using an ESP8266.
4.  Upload to your board.

---

## 📸 Screenshots
*(Tip: Upload a photo of your camera working to your GitHub repo and link it here!)*
`![Thermal Camera in Action](link-to-your-image.jpg)`

---

## 📜 License
This project is open-source. Feel free to use and modify it for your own DIY builds!
