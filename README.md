# FactoryCare

## 🧑‍💻 Lorenzo Menchini, Niccolò Settimelli
### 📅 A.A. 2023/2024

## 📋 Project Overview

### 🌟 Introduction
FactoryCare tackles the rising rate of fatalities in confined spaces in Italy, often caused by toxic gas inhalation or high temperatures. Between 2005 and 2018, there were 37 incidents, with 66.7% involving asphyxiating gases, leading to 62 worker deaths. In 2024, 20% of workplace fatalities occur in these environments, with a mortality rate of about 2.7 deaths per incident. To address this, FactoryCare leverages IoT technologies to predict and detect hazardous situations, classify them, and alert workers immediately. The main goal is to prevent accidents through predictive detection and swift intervention.

### ⚙️ Application Logic
FactoryCare relies on sensor servers that monitor various environmental parameters. Each sensor features an embedded machine learning model:
- **Thermometers**: Predict future temperatures.
- **LPG Sensors**: Classify air risk levels.

Predictions are sent to actuators, which respond based on the detected risk level: Low Risk (LR), Moderate Risk (MR), or Critical Risk (CR). This ensures timely responses to prevent harmful incidents for workers.

### 🏗️ Architecture
The system architecture includes:

- **CoAP Network**: 
  - Uses NRF52849 dongles for real sensors.
  - Sensors and actuators communicate with each other and the external environment via a border router.

- **Java Cloud Application**:
  - Manages dynamic registration of sensors and actuators.
  - Monitors and stores data in a MySQL database.

- **Remote User Interface**:
  - Allows users to observe devices and interact with actuators.
  - Provides insights into recent risk moments and allows manual adjustments.

#### 🤖 Machine Learning Models
FactoryCare integrates TensorFlow models adapted to C using the emlearn library:
- **Temperature Prediction Model**: Utilizes historical temperature and humidity data to forecast future temperatures.
- **Air Danger Classification Model**: LPG sensors classify air risk levels based on environmental factors.

#### 🌐 CoAP Network
- **Client/Server Architecture**:
  - Sensors: Act as internal servers providing resources observable by actuators and the Java Cloud Application.
  - Actuators: Act as clients to sensors and provide externally modifiable resources upon requests from the User Application.

#### ☁️ Cloud Application and Database
- **Cloud Application**: 
  - Manages registrations of sensors and actuators.
  - Monitors sensor data and stores it in a MySQL database.

- **Registration Phase**: Sensors send a JSON packet with details to the Cloud Application. The application observes and inserts this data into the database.

#### 📱 Remote User Application
- **User Interaction**:
  - Monitor device status and communicate with actuators via COAP GET and POST requests.
  - Set temperature thresholds, check actuator status, get danger event counts, and turn off nodes.

#### 🔒 Data Encapsulation and Representation
- **Communication**: Uses simple PLAIN TEXT payloads for COAP message exchanges.
- **Data Format**: Encapsulated in JSON format, following the SenML object model for consistent data representation.

### 📊 Grafana Integration
FactoryCare integrates with Grafana for real-time monitoring and visualization of sensor data, offering an intuitive interface for users to analyze environmental conditions and system performance.

---

Feel free to reach out if you have any questions or need further information. Happy analyzing! 🚀
