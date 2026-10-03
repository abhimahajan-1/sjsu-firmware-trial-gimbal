# SJSU Robotics Firmware Trial Project

## Single-Axis Auto-Adjusting Gimbal

This project implements a single-axis auto-adjusting gimbal using an Arduino, GY-521 MPU-6050, and servo motor.

The MPU-6050 is interfaced directly over I2C using the Arduino Wire library without the use of an MPU-6050 library.

## Features

- Direct MPU-6050 communication over I2C
- Accelerometer-based tilt measurement
- Automatic servo compensation
- User-defined angle offset through the Serial Monitor
- Serial output of MPU-6050 positional feedback
- Servo command limiting

## Libraries

- Wire
- Servo

## Hardware

- Arduino Nano
- GY-521 MPU-6050
- Servo motor
- Breadboard
- Jumper wires

## Serial Input

The desired gimbal offset can be entered in degrees through the Serial Monitor.

For example:

0
45
-30

An offset of 0 corresponds to the default upright orientation.

## Current Status

Firmware compiles successfully for the Arduino Nano. Physical hardware integration and calibration work successfully as well.
