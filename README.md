# CubeSat-V1.1
CubeSat prototype of 1U form-factor architecture, integrating inertial navigation, multi-sensor fusion, and quaternion-based EKF attitude estimation using ESP32-S3. Features GNSS time synchronization, solar-powered battery management, onboard imaging, magnetorquer-based ADCS, and long-range LoRa telemetry to a ground control station.

# Overview
The system is structured around three primary subsystems:
1. Navigation & Communication Subsystem (NCS)
2. Electrical Power Subsystem (EPS)
3. Attitude Determination & Control Subsystem (ADCS)

# Key Features
Multi-sensor integration (IMU, magnetometer, barometric sensor, GNSS), 
Quaternion-based attitude estimation with EKF, 
PPS-synchronized GNSS positioning, 
Long-range LoRa telemetry communication, 
Onboard camera image capture and transmission, 
Solar-powered battery charging and power monitoring, 
Magnetorquer-based magnetic attitude control.

# System Architecture
The ESP32-S3 acts as the central processing unit, handling:
I2C/SPI/UART-based sensor communication, 
Real-time data acquisition and calibration, 
Sensor fusion and state estimation, 
Telemetry data packaging and transmission, 
Power-aware embedded operation.

# Power System
The CubeSat prototype includes a solar-powered energy subsystem consisting of:
Solar panels for energy harvesting, 
LiPo battery for energy storage, 
Charging and protection circuitry, 
Embedded power monitoring.

# Attitude Determination & Control
Attitude estimation is performed using quaternion-based filtering techniques (including EKF).
Magnetorquers are implemented for magnetic actuation, providing a lightweight alternative to reaction wheels under CubeSat constraints.

# Applications
Educational CubeSat development, 
Inertial navigation experimentation, 
Embedded systems prototyping, 
Small satellite subsystem research.

# CubeSat Avionics System Architecture
<img width="4443" height="2644" alt="CubeSat_ProcessFlow" src="https://github.com/user-attachments/assets/0760f389-0d98-48cf-8b26-f86c2f4937ec" />
This diagram represents the complete avionics data and power architecture of the CubeSat. It shows how power is distributed, how sensors communicate with the flight computer, how attitude is estimated and controlled, and how telemetry and payload data are stored and transmitted to the ground station.

The system is centered around the **ESP32-S3 Sense Flight Computer**, which manages all onboard sensors, performs data fusion, controls the attitude system, and handles telemetry transmission.

# 1. Power System Architecture
The spacecraft power system begins with the **Power System**(Solar Panels), which supplies energy to the onboard **battery**. The battery stores electrical energy generated from solar panels and supplies it to the rest of the spacecraft.The battery output is regulated using **Boost/Buck Converters** to generate stable voltage rails required by different subsystems.

Two voltage rails are generated:
### 5V Rail
The **5V rail** powers components that require higher voltage operation. These include:
* ESP32-S3 Sense Flight Computer
* 5V Current Sensor (for monitoring power consumption)
* Motor drivers used for attitude control

### 3.3V Rail
The **3.3V rail** powers all digital electronics and sensors including:
* IMU sensor
* Magnetometer
* Pressure sensor
* ADC for analog sensors
* LoRa module

Current sensors on both rails continuously monitor power usage and report the values to the flight computer.

# 2. Flight Computer
The **ESP32-S3 Sense** serves as the central processing unit of the CubeSat.
Its responsibilities include:
* Sensor data collection
* Attitude determination
* Attitude control
* Payload management
* Telemetry generation
* Communication with the ground station

The flight computer communicates with different subsystems using three main communication interfaces:
* **UART**
* **I²C Bus**
* **SPI Bus**

# 3. GNSS Navigation System
The CubeSat uses the **AI Thinker GP-02 GNSS Receiver** for positioning and time synchronization.
Communication between the GNSS module and the flight computer occurs through **UART**.

The GNSS receiver provides:
* Position data (latitude, longitude, altitude)
* Precise time synchronization
* Velocity information

This information is used for:
* Orbit determination
* Timestamping telemetry
* Navigation reference

# 4. I²C Sensor Network
Most sensors are connected to the **I²C Bus**, which allows multiple devices to communicate using only two wires.

The following sensors are connected via I²C:
### MPU6050 Accelerometer + Gyroscope
Measures:
* Linear acceleration
* Angular velocity
These measurements provide motion data required for attitude estimation.

### QMC5883L Magnetometer
Measures the **Earth’s magnetic field vector**, which is used for:
* Determining spacecraft heading
* Supporting attitude determination algorithms

### BMP280 Pressure Sensor
Measures atmospheric pressure and temperature.

Although pressure sensing has limited use in orbit, it can assist during **ground testing and launch phase monitoring**.

### MCP23017 GPIO Expander

Provides additional digital input/output pins to the flight computer.
This allows the system to connect extra sensors or control lines, in this case Motor Drivers, because exhaustion of the microcontroller GPIO pins.

### ADS1115 Analog Sensor Interface
The **ADS1115** is a high-resolution analog-to-digital converter.
It reads analog signals from **LDR Sun Sensors**.

# 5. Sun Sensor System
Multiple **LDR-based sun sensors** are mounted on different faces of the CubeSat.
Their readings are processed to determine the **direction of the Sun relative to the spacecraft**.
Processing flow:
```
LDR Sun Sensors
      ↓
ADS1115 ADC
      ↓
Sun Direction Detection
```

The calculated sun vector helps determine spacecraft orientation relative to the Sun.

# 6. 10-DOF IMU Fusion
The CubeSat performs **10 Degree-of-Freedom sensor fusion** using data from:

* MPU6050 (Accelerometer + Gyroscope)
* QMC5883L (Magnetometer)
* BMP280 (Pressure & Temperature)

This fusion algorithm estimates the spacecraft’s **attitude and orientation in space**.

Outputs include:
* Roll
* Pitch
* Yaw
* Orientation quaternion or Euler angles

# 7. Orientation Estimation
The **orientation estimation module** combines:
* IMU data
* Magnetic field data
* Sun direction vector

Using sensor fusion algorithms such as:
* Complementary filter
* Extended Kalman filter
* Madgwick filter

The output is a stable estimate of spacecraft attitude.

# 8. Attitude Control System
The spacecraft controls its orientation using **magnetorquers**.
The control loop operates as follows:
```
Orientation Estimation
        ↓
Magnetorquer Control
        ↓
DRV Motor Drivers
```
The **DRV motor drivers** control the current flowing through magnetorquer coils, generating magnetic dipoles that interact with Earth’s magnetic field to rotate the spacecraft.

# 9. Payload System (Camera)
The CubeSat includes an **OV2640 Camera Module** connected via the **SPI bus**.
The payload operation is scheduled to capture images periodically:
```
OV2640 Camera
      ↓
Capture Image Every 15 Minutes
      ↓
Store to SD Card
```
Images are then stored for later downlink.

# 10. Data Storage
All mission data is stored on the **on-board SD card**.

The SD card stores:
* Sensor data
* GNSS data
* Attitude information
* Captured images
* System logs

This ensures that no critical data is lost even if communication with the ground station is temporarily unavailable.

# 11. Telemetry Generation
Telemetry packets are prepared using the collected data.
The telemetry preparation stage includes:
* Sensor readings
* Attitude estimates
* Power system data
* GNSS position
* System health status
```
Log All Telemetry Data
        ↓
Prepare Telemetry Packet
```

# 12. Communication System
Telemetry is transmitted using the **LoRa SX1278 Radio Module**.
Communication flow:

```
Telemetry Packet
       ↓
LoRa SX1278 Radio
       ↓
Ground Control Station
```
The ground station receives the packets for:
* Mission monitoring
* Data analysis
* Command uplink


# 13. SPI Bus Conflict Control
The **SPI bus** is shared between:
* Camera module
* SD card
* LoRa radio

A **conflict control mechanism** ensures that only one device communicates on the SPI bus at a time, preventing data collisions.

# 14. Complete System Operation
The overall spacecraft operation loop follows this sequence:
1. Power system distributes regulated voltages.
2. Flight computer collects data from all sensors.
3. IMU fusion estimates spacecraft orientation.
4. Sun sensors determine sun direction.
5. Magnetorquer control adjusts spacecraft attitude.
6. Camera captures images periodically.
7. All data is logged to the SD card.
8. Telemetry packets are prepared.
9. LoRa radio transmits data to the ground station.

 This architecture ensures:

* Reliable **attitude determination**
* Efficient **power monitoring**
* Robust **data storage**
* Continuous **telemetry transmission**
* Integrated **Attitude Determination and Control System**
