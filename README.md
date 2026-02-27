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
