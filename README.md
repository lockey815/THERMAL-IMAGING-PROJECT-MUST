# 🎗️ Development of a Low-Cost Thermal Imaging Device for Breast Cancer Screening

![Project Status](https://img.shields.io/badge/Status-Prototype-orange)
![Institution](https://img.shields.io/badge/Institution-MUST-blue)
![License](https://img.shields.io/badge/License-MUST-green)

This project is a functional medical screening prototype designed to provide a **non-invasive, non-ionizing, and affordable alternative for early breast cancer detection in low-resource settings**.

Developed at **Mbarara University of Science and Technology (MUST)**.

---

# 📖 Project Overview

Breast cancer is a leading cause of cancer-related deaths among women worldwide. In Sub-Saharan Africa (SSA), more than **70% of patients are diagnosed at advanced stages** due to the high cost of traditional diagnostics like Mammography and MRI.

This device utilizes **Infrared Thermography (IRT)** to detect physiological changes such as **angiogenesis and increased metabolic activity** by capturing heat signatures from the skin surface.

It is designed to be a **portable, safe, and cost-effective tool for community-level screening**.

---

# 👥 The Team

- **Researchers:** Lofoga Innocent & Ayikoru Winnie  
- **Department:** Biomedical Sciences and Engineering  
- **Supervisor:** Dr. Mulerwa Martha  
- **Institution:** Mbarara University of Science and Technology (MUST)

---

# ✨ Technical Features

- **HD Upscaling:** Real-time bilinear interpolation upscales raw 8×8 sensor data into a smooth **70×70 heat map** (4,900 pixels).
- **Dynamic Auto-Scaling:** Automatically adjusts the temperature palette for optimal thermal contrast.
- **Center-Point Measurement:** Crosshair with real-time temperature readout.
- **Battery Management:** Visual UI indicator for battery level.
- **Non-Ionizing:** Safe for repeated screening with **no radiation exposure**.

---

# 🛠️ Hardware Specifications

| Component | Model | Purpose |
|---|---|---|
| **Microcontroller** | NodeMCU ESP8266 | System logic and WiFi processing |
| **Thermal Sensor** | Panasonic AMG8833 | 8×8 Infrared Thermopile Array |
| **Display** | ILI9341 2.8" TFT LCD | Real-time thermal visualization |
| **Power Supply** | 3.7V Lithium-Ion | Portable rechargeable power |
| **Charging** | TP4056 Module | Battery protection and charging |
| **Housing** | 3D Printed PLA | Insulated handheld enclosure |

---

# 🔌 Wiring Guide

| Sensor/Display | NodeMCU Pin | Description |
|---|---|---|
| SDA / SCL | D2 / D1 | I2C Communication |
| TFT CS | D6 | Display Chip Select |
| TFT DC | D4 | Data / Command |
| TFT RST | D3 | Reset Pin |
| Battery Sense | A0 | Voltage Monitoring |

---

# 💻 Software Logic & Algorithms

The core intelligence of the device is the **Upscaling Engine**, which converts low-resolution thermal data into a high-quality visualization.

## 1️⃣ HD Interpolation Engine

The AMG8833 provides a raw **8×8 grid (64 pixels)**.

The algorithm performs **two-pass interpolation**:

- **Horizontal interpolation:** Generates intermediate points between pixels.
- **Vertical interpolation:** Interpolates rows to create a final **70×70 grid (4,900 pixels)**.

This reduces block artifacts and improves visualization of thermal gradients.

---

## 2️⃣ Adaptive Color Mapping

Temperature values are mapped to a **16-bit RGB565 color palette** using dynamic thresholds.

```
a = MinTemp + (MaxTemp - MinTemp) * 0.2121
b = MinTemp + (MaxTemp - MinTemp) * 0.3182
c = MinTemp + (MaxTemp - MinTemp) * 0.4242
d = MinTemp + (MaxTemp - MinTemp) * 0.8182
```

Temperature zones:

- **Blue:** Normal skin temperature
- **Green:** Moderate thermal activity
- **Red:** Potential abnormal thermal region

---

# 🧪 Methodology

The project follows a **five-stage research approach**.

### 1️⃣ System Requirements Analysis

- Clinical safety requirements  
- Low-cost sensing design  
- Minimal training usability

### 2️⃣ Hardware Design

Component selection optimized for:

- Radiometric resolution
- Processing performance
- Cost efficiency

### 3️⃣ Software Development

Algorithms implemented:

- Sensor data acquisition
- Thermal interpolation

### 4️⃣ Prototype Integration

Integration of:

- Sensor module
- Microcontroller
- 3D printed insulated casing

### 5️⃣ Testing & Evaluation

Validation performed using:

- Black-body temperature references
- Benchmarking with **DMR-IR dataset**

---

# 📊 Project Budget

Total estimated cost:

## **545,000 UGX (~$150 USD)**

| Component | Example Model | Cost (UGX) |
|---|---|---|
| Microcontroller | NodeMCU ESP8266 | 26,000 |
| Infrared Sensor | AMG8833 | 125,000 |
| Display | ILI9341 TFT | 50,400 |
| Power System | Li-Ion + TP4056 | 48,000 |
| Enclosure | PLA Filament | 250,000 |
| Miscellaneous | Wiring & assembly | 15,000 |
| Contingency | Price variations | 20,000 |
| **TOTAL** | | **545,000 UGX** |

---

# 📅 Work Plan

| Month | Milestone | Task |
|---|---|---|
| Month 1 | Design Finalized | Literature review and sensor selection |
| Month 2–3 | Assembly | Hardware procurement and coding |
| Month 4 | Prototype | System integration |
| Month 5 | Validation | Accuracy evaluation |
| Month 6 | Submission | Final report and presentation |

---

# ⚖️ Ethical Considerations

## Safety
Uses **non-ionizing infrared sensing**, ensuring **zero radiation exposure**.

## Consent
All clinical studies require **documented informed consent**.

## Privacy
Thermal data:

- Is anonymized
- Stores no identifying patient information
- Collected only with permission

---

# 📚 References

1. Pace, L. E., & Shulman, R. (2016). Breast cancer in sub-Saharan Africa. *Cancer*.
2. Galukande, M. et al. (2014). Patient delay in accessing breast cancer care in Uganda. *BMC Health Services Research*.
3. Roslidar, N. R. et al. (2020). Thermal imaging for breast cancer detection. *IEEE Access*.
4. World Health Organization (2023). Global Cancer Observatory.

---

# 🏛️ Institutional Affiliation

**Mbarara University of Science and Technology (MUST)**  
Faculty of Applied Sciences and Technology  
Department of Biomedical Sciences and Engineering

**Supervisor1:** Dr. Mulerwa Martha  
**Email:** mmulerwa@must.ac.ug

**Supervisor2:** Joshua Biryomumeisho 

**Email:** jbiryomumeisho@must.ac.ug

---

# 🔬 Project Status

🚧 **Current Phase:** Research Prototype  
📍 **Location:** Mbarara University of Science and Technology

---

# 🤝 Contributions

This project contributes to **low-cost medical technology development for underserved regions**, improving **early disease detection accessibility**.

---

# 📜 License

This project is released under the **MUST License** for research and academic purposes.
