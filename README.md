# CO2 Greenhouse Controller System Documentation

**Authors:**  
Nadim Ahmed, Abhinav Kayastha, Aleksanteri Merilainen, Aaban Prasla

**Date:**  
October 18, 2024

---

## Table of Contents
1. [Abstract](#abstract)
2. [Introduction](#introduction)
3. [System Overview](#system-overview)
4. [System Flowchart](#system-flowchart)
    1. [Flowchart Description](#flowchart-description)
5. [FreeRTOS Task Overview](#freertos-task-overview)
    1. [EEPROM Task](#eeprom-task)
    2. [Modbus Task](#modbus-task)
    3. [CO2 Injector Task](#co2-injector-task)
    4. [Fan Speed Control Task](#fan-speed-control-task)
    5. [Pressure Task](#pressure-task)
    6. [UI Task](#ui-task)
    7. [Interrupt Handler Task](#interrupt-handler-task)
6. [Test Systems](#test-systems)
    1. [Full-Scale Test Bench](#full-scale-test-bench)
    2. [Miniature Test Bench](#miniature-test-bench)
7. [Class Diagram](#class-diagram)
8. [Appendix](#appendix)
    1. [Conversations and Development Process](#conversations-and-development-process)
    2. [Datasheets](#datasheets)
    3. [Project Development Notes](#project-development-notes)
9. [Conclusion](#conclusion)

---

## Abstract

The CO2 Greenhouse Controller System is designed to maintain optimal CO2 levels within a greenhouse using FreeRTOS tasks. The system manages CO2 injection, fan speed control, pressure sensing, and user interaction via an OLED display and rotary encoder. Key components include a Modbus interface for sensor communication, EEPROM for data persistence, and real-time environmental control tasks that ensure efficient CO2 regulation and user interaction.

## Introduction

This documentation outlines the design and implementation of the CO2 Greenhouse Controller System, which runs on a Raspberry Pi Pico with FreeRTOS. It manages environmental conditions in a greenhouse by adjusting fan speed, injecting CO2, and providing an interactive user interface. The system ensures efficient control through various tasks and real-time scheduling, using EEPROM to store critical information like CO2 setpoints and user credentials across reboots.

## System Overview

The CO2 Greenhouse Controller System operates using a combination of sensors, a Modbus communication system, and user input via a rotary encoder. FreeRTOS is used to manage concurrent tasks that include:

- Reading and processing sensor data (CO2, humidity, temperature, pressure).
- Controlling the fan speed based on CO2 levels.
- Injecting CO2 when levels are too low.
- Storing and retrieving setpoints and credentials from EEPROM.
- Providing an interactive user interface via OLED and rotary encoder.

### Key Components:

- **Vaisala GMP252 CO2 Sensor:** Modbus-based CO2 monitoring.
- **Sensirion SDP610 Pressure Sensor:** I2C-based pressure sensor for monitoring airflow.
- **Produal MIO 12-V:** Fan control via Modbus for managing airflow in the greenhouse.
- **EEPROM:** Persistent storage for CO2 setpoints and Wi-Fi credentials.

## System Flowchart

The system flowchart below illustrates the process flow from system initialization to user interaction and environmental control. The system starts by reading data from EEPROM, followed by sensor initialization. After initialization, the system enters a loop that continuously monitors sensor data and adjusts fan speed and CO2 injection as needed.

![System Flowchart](../Project/Documentation/images/flow_chart.png)

### Flowchart Description

The flowchart follows a systematic process:

1. **System Initialization:** When the system boots, it initializes all hardware components, including EEPROM, sensors, and the display.
2. **EEPROM Data Reading:** The system reads stored CO2 setpoints and network credentials from EEPROM. If no data is available, default values are used.
3. **Sensor Data Monitoring:** The system continuously reads CO2, temperature, humidity, and pressure data.
4. **CO2 Control:** If CO2 levels fall below the setpoint, the system opens the CO2 injection valve to increase CO2 concentration.
5. **Fan Speed Control:** The system adjusts fan speed based on CO2 levels to regulate the greenhouse environment.
6. **User Interaction:** The user can interact with the system through the OLED display and rotary encoder, adjusting setpoints and viewing real-time data.

## FreeRTOS Task Overview

The system uses multiple FreeRTOS tasks to manage concurrent operations. Each task is designed to handle a specific aspect of the system, ensuring efficient resource management and real-time responsiveness.

### EEPROM Task

The `eeprom_task` is responsible for reading and writing data to EEPROM. It stores CO2 setpoints and credentials, ensuring that the system can restore settings after a reboot. This task communicates with the I2C bus to interact with the EEPROM.

### Modbus Task

The `modbus_task` reads CO2, humidity, and temperature values from the Modbus-connected sensors. The task updates shared resources that other tasks use for decision-making (e.g., adjusting fan speed and CO2 injection).

### CO2 Injector Task

The `co2_injector` task manages the CO2 injection valve. When CO2 levels drop below the desired setpoint, this task opens the valve to release CO2. Once the target level is reached, the valve is closed to conserve CO2.

### Fan Speed Control Task

The `fanSpeedWrite` task dynamically adjusts the fan speed based on the current CO2 concentration. If CO2 levels exceed a predefined threshold, the task increases the fan speed to ventilate the greenhouse and maintain a safe environment.

### Pressure Task

The `read_pressur` task monitors airflow by reading data from the Sensirion SDP610 pressure sensor. The task ensures that the greenhouse ventilation is functioning as expected.

### UI Task

The `UI_task` handles all user interactions through the OLED display and rotary encoder. Users can navigate system settings, view real-time data, and adjust parameters such as the CO2 setpoint.

### Interrupt Handler Task

The `InterruptHandler` task captures user input through the rotary encoder and buttons. This task ensures that the system responds to user interactions in real time.

## Test Systems

The CO2 Greenhouse Controller was tested in both full-scale and miniature test setups to ensure the system functions correctly under different conditions.

### Full-Scale Test Bench

The full-scale test bench simulates the real-world conditions of a greenhouse, including dynamic CO2 levels and airflow adjustments. This setup allowed us to validate the system's ability to maintain stable CO2 levels and handle user input reliably.

![Full-Scale Test Bench](../Project/Documentation/images/large_testBench.png)

### Miniature Test Bench

The miniature test setup replicates the same functionality as the full-scale setup but in a more controlled environment. This setup was particularly useful for testing software changes and system adjustments without the complexities of a full greenhouse environment.

![Miniature Test Bench](../Project/Documentation/images/mini_testBench.png)

## Class Diagram

The following class diagram illustrates the structure of the CO2 Greenhouse Controller system, highlighting the key components, tasks, and shared resources used throughout the system.

![Class Diagram of the CO2 Greenhouse Controller System](../Project/Documentation/images/class_diagram.drawio23.png)

## Appendix

This appendix includes additional documentation, background, and reference material used during the development of the CO2 Greenhouse Controller system.

### Conversations and Development Process

The system was designed based on collaborative efforts and detailed discussions around embedded systems, FreeRTOS implementation, sensor interfacing, and CO2 regulation. Key decisions were made regarding the modular structure of the system, testing strategies (both full-scale and miniature setups), and efficient resource management using FreeRTOS.

### Datasheets

Below are the datasheets for the primary components used in the system:

- [**Vaisala GMP252 CO2 Sensor Datasheet**](https://docs.vaisala.com/v/u/B211567EN-G/en-US): Provides information on the sensor's range, accuracy, and Modbus communication.
- [**Vaisala HMP60 Humidity and Temperature Sensor Datasheet**](https://docs.vaisala.com/v/u/B210851EN-K/en-US): Describes the sensor's temperature and humidity measurement capabilities.
- [**Sensirion SDP610 Pressure Sensor Datasheet**](https://sensirion.com/media/documents/63859DD0/6166CF0E/Sensirion_Differential_Pressure_Datasheet_SDP600Series.pdf): Offers details on the pressure sensor's I2C communication and measurement accuracy.
- [**Produal MIO 12-V Datasheet**](https://www.produal.com/en/mio-12.html): Outlines the fan control system, including Modbus integration and voltage control.
- [**EEPROM Datasheet**](https://www.partco.fi/en/diy-kits/crowtail/23675-ect-ct010021e.html): Describes the memory capacity, read/write cycles, and I2C communication protocol of the EEPROM used for data storage.

### Project Development Notes

The development of the CO2 Greenhouse Controller involved several stages of testing and iteration. Some key milestones included:

- **Initial Sensor Integration:** Setting up the Modbus and I2C communication with the sensors and verifying data accuracy.
- **Task Scheduling in FreeRTOS:** Implementing the core system functions through individual tasks and ensuring synchronization using shared resources and mutexes.
- **UI Development:** Building