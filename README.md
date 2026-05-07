# DRYBOX

DRYBOX is an open-source, ESP8266/ESP32-powered smart filament drying solution designed for 3D printing enthusiasts. It provides precise temperature and humidity control, real-time monitoring via an asynchronous web interface, and advanced features like weight-based moisture loss tracking.

## 🚀 Features

*   **Asynchronous Web Interface:** Real-time monitoring and control using `ESPAsyncWebServer`.
*   **Automated Climate Control:** Precise regulation of temperature and humidity for various filament types.
*   **WiFi Captive Portal:** Easy network setup and configuration via `AsyncWiFiManager`.
*   **OTA Updates:** Hassle-free firmware and filesystem updates over the air.
*   **Hardware Safety:** Designed for use with Solid State Relays (SSR) and internal cooling fans.
*   **Expandable:** Support for load cells (HX711) to monitor real-time weight loss and drying efficiency.

## 🛠 Hardware (Bill of Materials)

| Component | Description | Links |
| :--- | :--- | :--- |
| **Power Input** | Tripolar Panel Socket with Fuse | Option 1 / Option 2 / Option 3 |
| **Power Supply** | 220V AC to 5V DC Module | AliExpress |
| **Relay** | Solid State Relay (SSR) | AliExpress |
| **Heating** | Heating Lamp or PTC Element | Lamp / Element |
| **Airflow** | 120mm Cooling Fan (110/220V) | Mercado Livre |
| **Scale** | Load Cell with HX711 | AliExpress |
| **Sensor** | SHT-40 Temp/Humidity Sensor | AliExpress |
| **Fixings** | M3 Threaded Inserts | AliExpress |

## 💻 Software Stack

This project is built using **PlatformIO** and relies on the following key libraries:
*   **ESPAsyncWebServer:** High-performance asynchronous web server.
*   **ArduinoJson:** For configuration and API response management.
*   **ESPAsyncWiFiManager:** For captive portal WiFi configuration.
*   **ESPAsyncHTTPUpdateServer:** For OTA updates via the browser.

## 📸 Gallery

<p align="center">
  <img src="https://github.com/user-attachments/assets/9b974aaa-467c-44ba-a33f-4f70752442f6" width="45%" />
  <img src="https://github.com/user-attachments/assets/18a33049-ba05-400a-8d1e-2e25acbca712" width="45%" />
</p>
<p align="center">
  <img src="https://github.com/user-attachments/assets/ea8dfc6d-2056-4e65-a351-4de44dd146e0" width="45%" />
  <img src="https://github.com/user-attachments/assets/ffa24013-786e-4481-8d79-32a7b0545378" width="45%" />
</p>

## 🗺 Roadmap

Detailed improvements are tracked in `TODO.md`. Upcoming features include:
*   [ ] Integration of Filament Weight Sensors.
*   [ ] Preset profiles for PLA, PETG, ABS, and Nylon.
*   [ ] Adaptive drying cycles based on ambient humidity trends.
*   [ ] Physical controls (OLED + Button).
*   [ ] Software and Hardware Over-Temperature Protection.

## 📜 License

This project is licensed under the MIT License.

---
*Made with ❤️ for the 3D Printing Community.*
