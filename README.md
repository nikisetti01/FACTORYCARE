# FactoryCare

## Lorenzo Menchini, Niccolò Settimelli
### A.A. 2023/2024

## Project Overview

### Introduction
FactoryCare addresses the increasing rate of fatalities in confined spaces in Italy, often caused by toxic gas inhalation or high temperatures. From 2005 to 2018, there were 37 incidents, with 66.7% involving the inhalation of asphyxiating gases, leading to 62 worker deaths. In 2024, 20% of workplace fatalities occur in these environments, with a mortality rate of about 2.7 deaths per incident. To combat this, FactoryCare uses IoT technologies to predict and detect hazardous situations, classify them, and directly alert workers. The primary goal is to prevent accidents through predictive detection and immediate intervention.

### Application Logic
FactoryCare is centered around sensor servers that sample various environmental parameters. Each sensor has an embedded machine learning model. Thermometers predict future temperatures, while LPG sensors classify air risk levels. These predictions are sent to actuators, which respond according to the detected risk level, categorized as Low Risk (LR), Moderate Risk (MR), or Critical Risk (CR). The system ensures prompt responses to prevent harmful incidents for workers.

### Architecture
The device network consists of a CoAP network using NRF52849 dongles for real sensors. Sensors and actuators communicate with each other and the external environment through a border router. Initially, sensors and actuators register dynamically with the Java Cloud Application, which monitors data and stores it in a MySQL database. Users interact through a user interface to observe devices and communicate directly with actuators, providing insights into recent risk moments and allowing manual adjustments.

#### Machine Learning Models
FactoryCare incorporates TensorFlow models adapted to C using the emlearn library. The temperature prediction model uses historical temperature and humidity data to predict future temperatures within the thermometer sensor. For air danger classification, the LPG sensor classifies risk levels based on various environmental factors.

#### CoAP Network
The CoAP network consists of client/server sensors and actuators. Sensors act as internal servers, offering resources observable by both actuators and the Java Cloud Application. Actuators act as clients to sensors and provide externally modifiable resources upon request from the User Application.

#### Cloud Application and Database
The Cloud Application manages sensor and actuator registrations and monitors sensor data, storing it in a MySQL database. During the registration phase, sensors send a JSON packet with their details to the Cloud Application. Once registered, the application observes sensor data and inserts it into the database.

#### Remote User Application
The Remote User Application allows monitoring of device status and direct communication with actuators via COAP GET and POST requests. Users can set temperature thresholds, check actuator status, get the number of danger events, and turn off nodes.

#### Data Encapsulation and Representation
COAP message exchanges use simple PLAIN TEXT payloads for straightforward communication. During registration and monitoring phases, data is encapsulated in JSON format. The communication model follows the SenML object model for consistent and accurate data representation.

### Grafana Integration
FactoryCare integrates with Grafana for real-time monitoring and visualization of sensor data, providing an intuitive interface for users to analyze environmental conditions and system performance.
