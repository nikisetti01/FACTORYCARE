Industrial IoT Application for Hazard Detection - Industry 4.0
Overview

This repository contains an example Industrial IoT (IIoT) application designed for Industry 4.0. The application aims to detect hazardous zones using sensors and actuators, providing real-time monitoring and control capabilities. The system utilizes Contiki OS, the CoAP protocol, and incorporates machine learning models for predictive analytics.
Features

    Sensors:
        Thermometer: Measures the temperature.
        Air Quality Detector: Monitors air quality levels.

    Actuators:
        Ventilation System: Activates to regulate air quality.
        Sprinkler System: Activates in response to high temperature readings.

    Protocols:
        Contiki OS: An open-source operating system for the Internet of Things.
        CoAP Protocol: Used for efficient communication between sensors and actuators.

    Machine Learning:
        Integrated ML models for predicting hazardous temperature levels and critical air quality.
        Utilizes emlearn for lightweight, embedded machine learning.

    Database:
        MySQL: Stores sensor data and system logs.

    Applications:
        Java Monitoring Application: Monitors and logs sensor data in real-time.
        User Application: Provides an interface for active system monitoring and control.

System Architecture

    Sensor Nodes: Collect environmental data (temperature, air quality) and predict potential hazards using embedded ML models.
    Actuator Nodes: Respond to hazardous conditions by activating the ventilation and sprinkler systems.
    Communication Layer: Uses Contiki OS and CoAP protocol to facilitate communication between sensor nodes, actuator nodes, and the central server.
    Central Server: Runs the Java monitoring application, logs data into the MySQL database, and provides data to the User Application for active monitoring.
    User Interface: Displays real-time data and system status, allows users to manually control actuators.

Installation
Prerequisites

    Contiki OS
    Java Development Kit (JDK)
    MySQL Server
    Maven (for Java application)
    emlearn (for ML models)

Setup

    Clone the Repository:

    sh

git clone https://github.com/yourusername/industrial-iot-application.git
cd industrial-iot-application

Setup Contiki OS:

    Follow the instructions on the Contiki OS website to set up the development environment.

Deploy Sensor and Actuator Code:

    Navigate to the contiki_code directory and compile the sensor and actuator code.
    Upload the compiled code to the respective sensor and actuator nodes.

Configure MySQL Database:

    Install MySQL Server and create a database:

    sql

CREATE DATABASE iot_industry4;

Import the provided schema:

sh

    mysql -u username -p iot_industry4 < schema.sql

Build and Run Java Monitoring Application:

    Navigate to the java_monitoring_app directory.
    Build the application using Maven:

    sh

mvn clean install

Run the application:

sh

        java -jar target/monitoring-app.jar

    Start User Application:
        Navigate to the user_application directory.
        Follow the instructions in the README to set up and run the user interface.

Usage

    Monitoring:
        Launch the Java Monitoring Application to start collecting and logging sensor data.
        Access the User Application via your web browser to view real-time data and system status.

    Control:
        Use the User Application to manually activate or deactivate the ventilation and sprinkler systems.
