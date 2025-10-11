# High-performance 3-Axis SmartIndustrial™ Accelerometer MEMS Device for Industrial Applications

# GENERAL DESCRIPTION

The IIM- 42352 is a 3- axis accelerometer packaged in a small  $2.5 \text{mm} \times 3 \text{mm} \times 0.91 \text{mm}$  (14- pin LGA) package.

The IIM- 42352 includes multiple capabilities to enable easy, robust and accurate inertial and vibration measurements in Industrial applications:

Wide and flat frequency response range: from dc to 4 kHz  $(\pm 3$  dB point) Low noise:  $70\mu g / V H z$  Low power:  $0.3mA$  with all 3- axes delivering full performance Output data rate up to  $32kHz$  Highly accurate external clock input to increase ODR accuracy, reduce system level sensitivity error, and improve measurement impacts from device to device variation. 2 KB FIFO that can lower the traffic on the serial bus interface and reduce power consumption by allowing the system processor to burst read sensor data and then go into a low- power mode Wake- on- motion interrupt for low power operation of applications processor Operating temperature range:  $- 40^{\circ}C$  to  $105^{\circ}C$

The host interface can be configured to support  $13C^{SM}$  slave,  $1^2 C$  slave, or SPI slave modes. The  $13C^{SM}$  interface supports speeds up to  $12.5\mathrm{MHz}$  (data rates up to 12.5 Mbps in SDR mode, 25 Mbps in DDR mode), the  $1^2 C$  interface supports speeds up to 1 MHz, and the SPI interface supports speeds up to 24 MHz.

The device features an operating voltage range from 3.6V down to 1.71V.

# ORDERING INFORMATION

<table><tr><td>PART NUMBER</td><td>TEMPERATURE</td><td>PACKAGE</td></tr><tr><td>IIM-42352†</td><td>-40℃ to +105℃</td><td>14-pin LGA</td></tr></table>

†Denotes RoHS and Green- compliant package

# APPLICATIONS

Vibration measurement Predictive maintenance Tilt sensing Platform stabilization Robotics

# FEATURES

Digital- output X- , Y- , and Z- axis accelerometer with programmable full- scale range of  $\pm 2g,\pm 4g,$ $\pm 8g$  and  $\pm 16g$  User- programmable interrupts  $13C^{SM} / 1^2 C$  /SPI slave host interface Digital- output temperature sensor External clock input supports highly accurate clock input from 31 kHz to  $50kHz$  Small and thin package:  $2.5mm\times 3mm\times 0.91mm$  14- pin LGA)  $20,000g$  shock tolerant MEMS structure hermetically sealed and bonded at wafer level RoHS and Green compliant

# TYPICAL OPERATING CIRCUIT

![](images/77044b4681b1473e04c94d0548d74a11a9ff23d13a54fa1d7d695858e3878001.jpg)

Application Schematic (SPI interface to Host)

# LONGEVITY COMMITMENT

To provide the best service for customers developing products with a long- life cycle we have designed and engineered products with longevity in mind. These products are designed for harsher environments and are tested and manufactured to higher accuracy and stability. https://invensense.tdk.com/longevity/

# TABLE OF CONTENTS

General Description. 1 Ordering Information 1 Applications. 1 Features. 1 Typical Operating Circuit 1

# 1 Introduction. 8

1.1 Purpose and Scope 8  1.2 Product Overview 8  1.3 Applications 8

# 2 Features. 9

2.1 Accelerometer Features 9  2.2 Motion Features 9  2.3 Additional Features. 9

# 3 Electrical Characteristics. 10

3.1 Accelerometer Specifications 10  3.2 Electrical Specifications 11  3.3  $I^2 C$  Timing Characterization 13  3.4 SPI Timing Characterization - 4- Wire SPI Mode 14  3.5 SPI Timing Characterization - 3- Wire SPI Mode 15  3.6 RTC (CLKIN) Timing Characterization 16  3.7 Absolute Maximum Ratings 17

# 4 Applications Information. 18

4.1 Pin Out Diagram and Signal Description 18  4.2 Typical Operating Circuit. 19  4.3 Bill of Materials for External Components. 20  4.4 System Block Diagram 21  4.5 Overview. 21  4.6 Three- Axis MEMS Accelerometer with 16- bit ADCs and Signal Conditioning 21  4.7  $I^2 C^SM$ ,  $I^2 C$ , and SPI Host Interface. 21  4.8 Self- Test. 21  4.9 Clocking. 22  4.10 Sensor Data Registers. 22  4.11 Interrupts. 22  4.12 Digital- Output Temperature Sensor. 22  4.13 Bias and LDOs. 22  4.14 Charge Pump. 22

4.15 Standard Power Modes 23

5 Signal Path 24

5.1 Summary of Parameters Used to Configure the Signal Path 24

5.2 Anti- Alias Filter 24

5.3 User Programmable Offset 26

5.4 UI Filter Block 27

5.5 ODR And FSR Selection 31

6 FIFO 33

6.1 Packet Structure 33

6.2 FIFO Header 35

6.3 Maximum FIFO Storage 35

6.4 FIFO Configuration Registers 37

7 Programmable Interrupts 38

8 APEX Motion Functions 39

8.1 APEX DDR Support 39

8.2 DMP Power Save Mode 40

8.3 Pedometer Programming 40

8.4 Tilt Detection Programming 41

8.5 Freefall Detection Programming 42

8.6 Tap Detection Programming 43

8.7 Wake on Motion Programming 44

8.8 Significant Motion Detection Programming 44

9 Digital Interface 46

9.1  $13C^{SM}$ ,  $1^2 C$ , and SPI Serial Interfaces 46

9.2  $13C^{SM}$  interface 46

9.3  $1^2 C$  Interface 46

9.4  $1^2 C$  Communications Protocol 46

9.5  $1^2 C$  Terms 49

9.6 SPI Interface 49

10 Assembly 51

10.1 Orientation of Axes 51

10.2 Package Dimensions 52

11 Part Number Package Marking 54

12 Use Notes 55

12.1 Accelerometer Mode Transitions 55

12.2 Accelerometer Low Power (LP) Mode Averaging Filter Setting 55

12.3 Settings for  $1^2 C$ ,  $13C^{SM}$ , and SPI Operation 55

12.4 Anti- Alias Filter Operation. 55 12.5 external clock input effect on odr. 56 12.6 INT_ASYNC_RESET Configuration. 56 12.7 FIFO Timestamp Interval Scaling. 56 12.8 Supplementary Information for FIFO_HOLD_LAST_DATA_EN. 56

13 Register Map. 58

13.1 User Bank 0 Register Map. 58 13.2 User Bank 1 Register Map. 59 13.3 User Bank 2 Register Map. 59 13.4 User Bank 3 Register Map. 59 13.5 User Bank 4 Register Map. 60 13.6 Register Values Modification. 60

14 User Bank 0 Register Map - Descriptions. 61

14.1 DEVICE_CONFIG. 61 14.2 DRIVE_CONFIG. 61 14.3 INT_CONFIG. 62 14.4 FIFO_CONFIG. 62 14.5 TEMP_DATA1. 62 14.6 TEMP_DATA0. 63 14.7 ACCEL_DATA_X1. 63 14.8 ACCEL_DATA_X0. 63 14.9 ACCEL_DATA_Y1. 63 14.10 ACCEL_DATA_Y0. 63 14.11 ACCEL_DATA_Z1. 64 14.12 ACCEL_DATA_Z0. 64 14.13 TMST_FSYNCH. 64 14.14 TMST_FSYNCL. 64 14.15 INT_STATUS. 65 14.16 FIFO_COUNTH. 65 14.17 FIFO_COUNTL. 65 14.18 FIFO_DATA. 65 14.19 APEX_DATA0. 66 14.20 APEX_DATA1. 66 14.21 APEX_DATA2. 66 14.22 APEX_DATA3. 66 14.23 APEX_DATA4. 67 14.24 APEX_DATA5. 67

14.25 INT_STATUS2 . 67 14.26 INT_STATUS3 . 68 14.27 SIGNAL_PATH_RESET . 68 14.28 . 68 14.29 INTF_CONFIGO . 69 14.30 INTF_CONFIG1 . 70 14.31 PWR_MGMTO . 70 14.32 ACCEL_CONFIGO . 71 14.33 TEMP_FILT_CONFIG . 71 14.34 ACCEL_FILT_CONFIG . 72 14.35 ACCEL_CONFIG1 . 72 14.36 TMST_CONFIG . 73 14.37 APEX_CONFIGO . 73 14.38 SMOD_CONFIG . 74 14.39 FIFO_CONFIG1 . 74 14.40 FIFO_CONFIG2 . 74 14.41 FIFO_CONFIG3 . 75 14.42 FSYNC_CONFIG . 75 14.43 INT_CONFIGO . 76 14.44 INT_CONFIG1 . 76 14.45 INT_SOURCEO . 77 14.46 INT_SOURCE1 . 77 14.47 INT_SOURCE3 . 78 14.48 INT_SOURCE4 . 78 14.49 FIFO_LOST_PKT0 . 78 14.50 FIFO_LOST_PKT1 . 79 14.51 SELF_TEST_CONFIG . 79 14.52 WHO_AM_1 . 79 14.53 REG_BANK_SEL . 80 15 User Bank 1 Register Map - Descriptions . 81 15.1 SENSOR_CONFIGO . 81 15.2 TMSTVAL0 . 81 15.3 TMSTVAL1 . 81 15.4 TMSTVAL2 . 81 15.5 INTF_CONFIG4 . 82 15.6 INTF_CONFIG5 . 82 15.7 INTF_CONFIG6 . 82

16 User Bank 2 Register Map - Descriptions. 83

16.1 ACCEL_CONFIG_STATIC2. 83 16.2 ACCEL_CONFIG_STATIC3. 83 16.3 ACCEL_CONFIG_STATIC4. 83 16.4 XA_ST_DATA. 83 16.5 YA_ST_DATA. 84 16.6 ZA_ST_DATA. 84

17 User Bank 3 Register Map - Descriptions. 85

17.1 PU_PD_CONFIG1. 85 17.2 PU_PD_CONFIG2. 86

18 User Bank 4 Register Map - Descriptions. 87

18.1 FDR_CONFIG. 87 18.2 APEX_CONFIG1. 87 18.3 APEX_CONFIG2. 88 18.4 APEX_CONFIG3. 88 18.5 APEX_CONFIG4. 89 18.6 APEX_CONFIG5. 90 18.7 APEX_CONFIG6. 91 18.8 APEX_CONFIG7. 92 18.9 APEX_CONFIG8. 92 18.10 APEX_CONFIG9. 92 18.11 APEX_CONFIG10. 93 18.12 ACCEL_WOM_X_THR. 93 18.13 ACCEL_WOM_Y_THR. 94 18.14 ACCEL_WOM_Z_THR. 94 18.15 INT_SOURCE6. 94 18.16 INT_SOURCE7. 95 18.17 INT_SOURCE8. 95 18.18 INT_SOURCE9. 96 18.19 INT_SOURCE10. 96 18.20 OFFSET_USER4. 96 18.21 OFFSET_USER5. 97 18.22 OFFSET_USER6. 97 18.23 OFFSET_USER7. 97 18.24 OFFSET_USER8. 97

19 SmartIndustrial Family. 98

20 Reference. 99

21 Revision History. 100

# Table of Figures

Table of FiguresFigure 1.  $1^{\circ}$ C Bus Timing Diagram. 13Figure 2. 4- Wire SPI Bus Timing Diagram. 14Figure 3. 3- Wire SPI Bus Timing Diagram. 15Figure 4. RTC Timing Diagram. 16Figure 5. Pin Out Diagram for IIM- 42352 2.5 mm x 3.0 mm x 0.91 mm LGA. 19Figure 6. IIM- 42352 Application Schematic (I3C $^{SM}$ /I $^{2}$ C Interface to Host). 19Figure 7. IIM- 42352 Application Schematic (SPI Interface to Host). 20Figure 8. IIM- 42352 System Block Diagram. 21Figure 9. IIM- 42352 Signal Path. 24Figure 10. FIFO Packet Structure. 33Figure 11. Maximum FIFO Storage. 36Figure 12. START and STOP Conditions. 47Figure 13. Complete  $1^{\circ}$ C Data Transfer. 47Figure 14. Typical SPI Master/Slave Configuration. 50Figure 15. Orientation of Axes of Sensitivity and Polarity of Rotation. 51Figure 16. Package Dimensions. 52Figure 17. Part Number Package Marking. 54

# Table of Tables

Table of TablesTable 1. Accelerometer Specifications. 10Table 2. D.C. Electrical Characteristics. 11Table 3. A.C. Electrical Characteristics. 12Table 4.  $1^{\circ}$ C Timing Characteristics. 13Table 5. 4- Wire SPI Timing Characteristics (24- MHz Operation). 14Table 6. 3- Wire SPI Timing Characteristics (24- MHz Operation). 15Table 7. RTC Timing Characteristics. 16Table 8. Absolute Maximum Ratings. 17Table 9. Signal Descriptions. 18Table 10. Bill of Materials. 20Table 11. Standard Power Modes for IIM- 42352. 23Table 12. Parameters used to control the signal path. 24Table 13.  $1^{\circ}$ C Terms. 49Table 14. Package Dimensions. 53Table 15. Part Number Package Marking. 54

# 1 INTRODUCTION

# 1.1 PURPOSE AND SCOPE

This document is a product specification, providing a description, specifications, and design- related information on the IIM- 42352, a 3- axis accelerometer device targeted at Industrial applications. The device is housed in a small  $2.5\mathrm{mm}\times 3\mathrm{mm}\times 0.91\mathrm{mm}$  14- pin LGA package.

# 1.2 PRODUCT OVERVIEW

The IIM- 42352 is a 3- axis accelerometer in a small  $2.5\mathrm{mm}\times 3\mathrm{mm}\times 0.91\mathrm{mm}$  (14- pin LGA) package. It also features a 2K- byte FIFO that can lower the traffic on the serial bus interface and reduce power consumption by allowing the system processor to burst read sensor data and then go into a low- power mode. IM- 42352, with its 3- axis integration, enables manufacturers to eliminate the costly and complex selection, qualification, and system level integration of discrete devices, guaranteeing optimal motion performance for consumers.

The accelerometer supports four programmable full- scale range settings from  $\pm 2g$  to  $\pm 16g$  It includes multiple capabilities to enable tri- axial vibration measurements in Industrial applications:

Wide and flat frequency response range: from dc to 4 kHz (3 dB point) Low noise:  $70\mu g / VHz$  Low power:  $0.3mA$  with all 3- axes delivering full performance Output data rate up to  $32kHz$

IIM- 42352 also supports external clock input for highly accurate  $31~\mathrm{kHz}$  to  $50kHz$  clock, that helps to reduce system level sensitivity error, ODR sensitivity to temperature, and device to device variation.

Other industry- leading features include on- chip 16- bit ADCs, programmable digital filters, an embedded temperature sensor, and programmable interrupts. The device features  $13C^{SM}$ $1^{2}C_{i}$  and SPI serial interfaces, a VDD operating range of 1.71V to 3.6V, and a separate VDDIO operating range of 1.71V to 3.6V.

The host interface can be configured to support  $13C^{SM}$  slave,  $1^{2}C$  slave, or SPI slave modes. The  $13C^{SM}$  interface supports speeds up to 12.5 MHz (data rates up to 12.5 Mbps in SDR mode, 25 Mbps in DDR mode), the  $1^{2}C$  interface supports speeds up to 1 MHz, and the SPI interface supports speeds up to 24 MHz.

By leveraging its patented and volume- proven CMOS- MEMS fabrication platform, which integrates MEMS wafers with companion CMOS electronics through wafer- level bonding, InvenSense has driven the package size down to a footprint and thickness of  $2.5\mathrm{mm}\times 3\mathrm{mm}\times 0.91\mathrm{mm}$  (14- pin LGA), to provide a very small yet high- performance low- cost package. The device provides high robustness by supporting 20,000g shock reliability.

# 1.3 APPLICATIONS

Vibration measurement Predictive maintenance Tilt sensing Platform stabilization Robotics

# 2 FEATURES

# 2.1 ACCELEROMETER FEATURES

The triple- axis MEMS accelerometer in IIM- 42352 includes a wide range of features:

Digital- output X- , Y- , and Z- axis accelerometer with programmable full- scale range of  $\pm 2g,\pm 4g,\pm 8g$  and  $\pm 16g$  Low Noise (LN) and Low Power (LP) power modes support User- programmable interrupts Wake- on- motion interrupt for low power operation of applications processor Self- test

# 2.2 MOTION FEATURES

IIM- 42352 includes the following motion features, also known as APEX (Advanced Pedometer and Event Detection - neXt gen)

Pedometer: tracks step count, also issues step detect interrupt Tilt Detection: issues an interrupt when the tilt angle exceeds  $35^{\circ}$  for more than a programmable time. Freefall Detection: triggers an interrupt when device freefall is detected and outputs freefall duration Tap Detection: issues an interrupt when a tap is detected, along with the tap count Wake on Motion: detects motion when accelerometer data exceeds a programmable threshold Significant Motion Detection: detects significant motion if Wake on Motion events are detected during a programmable time window

# 2.3 ADDITIONAL FEATURES

IIM- 42352 includes the following additional features:

External clock input supports highly accurate clock input from  $31~\mathrm{kHz}$  to  $50~\mathrm{kHz}$  helps to reduce system level sensitivity error 2K- byte FIFO buffer enables the applications processor to read the data in bursts User- programmable digital filters for accelerometer and temperature sensor User configurable internal pull- up/pull- downs included on I/O interfaces to reduce system costs associated with external pull- ups/pull- downs 12.5 MHz  $\mathsf{BC}^{\mathsf{SM}}$  (data rates up to 12.5 Mbps in SDR mode, 25Mbps in DDR mode) / 1 MHz  $1^{2}C$  / 24 MHz SPI slave host interface Digital- output temperature sensor Smallest and thinnest LGA package for portable devices:  $2.5\mathrm{mm}\times 3\mathrm{mm}\times 0.91\mathrm{mm}$  (14- pin LGA) 20,000g shock tolerant MEMS structure hermetically sealed and bonded at wafer level RoHS and Green compliant

# 3 ELECTRICAL CHARACTERISTICS

# 3.1 ACCELEROMETER SPECIFICATIONS

Typical Operating Circuit of Section 3.2,  $\mathsf{VDD} = 1.8\mathsf{V}$  VDDIO  $= 1.8V$ $T_{A} = 25^{\circ}C$  unless otherwise noted.

<table><tr><td>PARAMETER</td><td colspan="2">CONDITIONS</td><td>MIN</td><td>TYP</td><td>MAX</td><td>UNITS</td><td>NOTES</td></tr><tr><td colspan="8">ACCELEROMETER SENSITIVITY</td></tr><tr><td rowspan="4">Full-Scale Range</td><td>ACCEL_FS_SEL =0</td><td></td><td>±16</td><td></td><td>g</td><td>2</td><td></td></tr><tr><td>ACCEL_FS_SEL =1</td><td></td><td>±8</td><td></td><td>g</td><td>2</td><td></td></tr><tr><td>ACCEL_FS_SEL =2</td><td></td><td>±4</td><td></td><td>g</td><td>2</td><td></td></tr><tr><td>ACCEL_FS_SEL =3</td><td></td><td>±2</td><td></td><td>g</td><td>2</td><td></td></tr><tr><td>ADC Word Length</td><td>Output in two&#x27;s complement format</td><td></td><td>16</td><td></td><td>bits</td><td>2,5</td><td></td></tr><tr><td rowspan="4">Sensitivity Scale Factor</td><td>ACCEL_FS_SEL =0</td><td></td><td>2,048</td><td></td><td>LSB/g</td><td>2</td><td></td></tr><tr><td>ACCEL_FS_SEL =1</td><td></td><td>4,096</td><td></td><td>LSB/g</td><td>2</td><td></td></tr><tr><td>ACCEL_FS_SEL =2</td><td></td><td>8,192</td><td></td><td>LSB/g</td><td>2</td><td></td></tr><tr><td>ACCEL_FS_SEL =3</td><td></td><td>16,384</td><td></td><td>LSB/g</td><td>2</td><td></td></tr><tr><td>Sensitivity Scale Factor Initial Tolerance</td><td>Component-level</td><td></td><td>±0.5</td><td></td><td>%</td><td>1</td><td></td></tr><tr><td>Sensitivity Change vs. Temperature</td><td>-40℃ to +105℃</td><td></td><td>±0.005</td><td></td><td>%/℃</td><td>3</td><td></td></tr><tr><td>Nonlinearity</td><td>Best Fit Straight Line, ±2g</td><td></td><td>±0.1</td><td></td><td>%</td><td>3</td><td></td></tr><tr><td>Cross-Axis Sensitivity</td><td></td><td></td><td>±1</td><td></td><td>%</td><td>3</td><td></td></tr><tr><td colspan="8">ZERO-G OUTPUT</td></tr><tr><td>Initial Tolerance</td><td>Board-level, all axes</td><td></td><td>±20</td><td></td><td>mg</td><td>3</td><td></td></tr><tr><td>Zero-G Level Change vs. Temperature</td><td>-40℃ to +105℃</td><td></td><td>±0.15</td><td></td><td>mg/℃</td><td>3,5</td><td></td></tr><tr><td colspan="8">OTHER PARAMETERS</td></tr><tr><td>Noise Power Spectral Density</td><td></td><td></td><td>70</td><td></td><td>μg/VHz</td><td>1</td><td></td></tr><tr><td>Total RMS</td><td>Bandwidth = 100 Hz</td><td></td><td>0.70</td><td></td><td>*s-rms</td><td>4</td><td></td></tr><tr><td rowspan="2">Signal bandwidth</td><td rowspan="2">±3 dB bandwidth</td><td>X &amp;amp; Y-axis</td><td></td><td>4.0</td><td></td><td>kHz</td><td>3,5</td></tr><tr><td>Z-axis</td><td></td><td>1.65</td><td></td><td>kHz</td><td>3,5</td></tr><tr><td rowspan="2">Low-Pass Filter Response</td><td>ODR &amp;lt; 1 kHz</td><td></td><td>5</td><td></td><td>500</td><td>Hz</td><td>2</td></tr><tr><td>ODR ≥ 1 kHz</td><td></td><td>42</td><td></td><td>3979</td><td>Hz</td><td>2</td></tr><tr><td>Accelerometer Startup Time</td><td>From sleep mode to valid data</td><td></td><td>10</td><td></td><td>ms</td><td>3</td><td></td></tr><tr><td>Output Data Rate</td><td></td><td>1.5625</td><td></td><td>32000</td><td>Hz</td><td>2</td><td></td></tr></table>

Notes: 1. Tested in production. 2. Guaranteed by design. 3. Derived from validation or characterization of parts, not tested in production. 4. Calculated from Power Spectral Density. 5. The recommended settings - ACCEL_ODR  $= 4$  (16Khz); ACCEL_AAF_DIS=0; ACCEL_AAF_DELT:63; ACCEL_AAF_DELTSQR: 3968; ACCEL_AAF_NITSHIFT  $= 3$  .With ACCEL_ODR  $= 3$  (8Khz), ACCEL_AAF_DIS  $= 0$  ACCEL_AAF_DELT:63; ACCEL_AAF_DELTSQR: 3968; ACCEL_AAF_NITSHIFT  $= 3$  settings, the 3dB bandwidth would be 2 kHz (X, Y axis), 1.3 kHz (Z- axis). 6. 20- bits data format supported in FIFO, see section 6.1.

# 3.2 ELECTRICAL SPECIFICATIONS

# 3.2.1 D.C. Electrical Characteristics

Typical Operating Circuit of Section 3.2, VDD  $= 1.8V$  VDDIO  $= 1.8V$ $T_{A} = 25^{\circ}C$  unless otherwise noted.

Table 2.D.C.Electrical Characteristics  

<table><tr><td>PARAMETER</td><td>CONDITIONS</td><td>MIN</td><td>TYP</td><td>MAX</td><td>UNITS</td><td>NOTES</td></tr><tr><td colspan="7">SUPPLY VOLTAGES</td></tr><tr><td>VDD</td><td></td><td>1.71</td><td>1.8</td><td>3.6</td><td>V</td><td>1</td></tr><tr><td>VDDIO</td><td></td><td>1.71</td><td>1.8</td><td>3.6</td><td>V</td><td>1</td></tr><tr><td colspan="7">SUPPLY CURRENTS</td></tr><tr><td>Low-Noise Mode</td><td>3-Axis Accelerometer</td><td></td><td>0.28</td><td></td><td>mA</td><td>2</td></tr><tr><td>Full-Chip Sleep Mode</td><td>At 259C</td><td></td><td>7.5</td><td></td><td>μA</td><td>2</td></tr><tr><td colspan="7">TEMPERATURE RANGE</td></tr><tr><td>Specified Temperature Range</td><td>Performance parameters are not applicable beyond Specified Temperature Range</td><td>-40</td><td></td><td>+105</td><td>℃</td><td>2</td></tr></table>

# Notes:

Notes:1. Guaranteed by design.2. Derived from validation or characterization of parts, not tested in production.

# 3.2.2 A.C. Electrical Characteristics

Typical Operating Circuit of Section 3.2, VDD  $= 1.8V$  VDDIO  $= 1.8V$ $T_{A} = 25^{\circ}C$  unless otherwise noted.

<table><tr><td>PARAMETER</td><td>CONDITIONS</td><td>MIN</td><td>TYP</td><td>MAX</td><td>UNITS</td><td>NOTES</td></tr><tr><td colspan="7">SUPPLIES</td></tr><tr><td>Supply Ramp Time</td><td>Monotonic ramp. Ramp rate is 10% to 90% of the final value</td><td>0.01</td><td></td><td>3</td><td>ms</td><td>1</td></tr><tr><td>Power Supply Noise</td><td>Up to 10 kHz</td><td></td><td>10</td><td></td><td>mV peak-peak</td><td>1</td></tr><tr><td colspan="7">TEMPERATURE SENSOR</td></tr><tr><td>Operating Range</td><td>Ambient</td><td>+40</td><td></td><td>105</td><td>℃</td><td>2</td></tr><tr><td>25℃ Output</td><td></td><td></td><td>0</td><td></td><td>LSB</td><td>3</td></tr><tr><td>ADC Resolution</td><td></td><td></td><td>16</td><td></td><td>bits</td><td>2</td></tr><tr><td>ODR</td><td>With Filter</td><td>25</td><td></td><td>8000</td><td>Hz</td><td>2</td></tr><tr><td>Room Temperature Offset</td><td>25℃</td><td>-5</td><td></td><td>5</td><td>℃</td><td>3</td></tr><tr><td>Stabilization Time</td><td></td><td></td><td></td><td>14000</td><td>μs</td><td>2</td></tr><tr><td>Sensitivity</td><td>Untrimmed</td><td></td><td>132.48</td><td></td><td>LSB/℃</td><td>1</td></tr><tr><td>Sensitivity for FIFO data</td><td></td><td></td><td>2.07</td><td></td><td>LSB/℃</td><td>1</td></tr><tr><td colspan="7">POWER-ON RESET</td></tr><tr><td>Start-up time for register read/write</td><td>From power-up</td><td></td><td></td><td>1</td><td>ms</td><td>1</td></tr><tr><td colspan="7">I2C ADDRESS</td></tr><tr><td>I2C ADDRESS</td><td>AP_ADO = 0
AP_ADO = 1</td><td></td><td>1101000
1101001</td><td></td><td></td><td></td></tr><tr><td colspan="7">DIGITAL INPUTS (FSYNC, SCLK, SDI, CS)</td></tr><tr><td>VIN, High Level Input Voltage</td><td></td><td>0.7*VDDIO</td><td></td><td></td><td>V</td><td rowspan="4">1</td></tr><tr><td>VIN, Low Level Input Voltage</td><td></td><td></td><td></td><td>0.3*VDDIO</td><td>V</td></tr><tr><td>CIN, Input Capacitance</td><td></td><td></td><td>&amp;lt; 10</td><td></td><td>pF</td></tr><tr><td>Input Leakage Current</td><td></td><td></td><td>100</td><td></td><td>nA</td></tr><tr><td colspan="7">DIGITAL OUTPUT (SDO, INT1, INT2)</td></tr><tr><td>VOH, High Level Output Voltage</td><td>RLOAD=1 MΩ;
RLOAD=1 MΩ;</td><td></td><td>0.9*VDDIO</td><td></td><td>V</td><td rowspan="5">1</td></tr><tr><td>VOL, LOW-Level Output Voltage</td><td>RLOAD=1 MΩ;</td><td></td><td></td><td>0.1*VDDIO</td><td>V</td></tr><tr><td>VOLINT, INT Low-Level Output Voltage</td><td>OPEN=1, 0.3 mA sink Current</td><td></td><td></td><td>0.1</td><td>V</td></tr><tr><td>Output Leakage Current</td><td>OPEN=1</td><td></td><td>100</td><td></td><td>nA</td></tr><tr><td>tINT, INT Pulse Width</td><td>int_pulse_duration=0, 1 (100 μs, 8 μs);</td><td>8</td><td></td><td>100</td><td>μs</td></tr><tr><td colspan="7">I2C I/O (SCL, SDA)</td></tr><tr><td>VIN, LOW-Level Input Voltage</td><td></td><td>-0.5 V</td><td></td><td>0.3*VDDIO</td><td>V</td><td rowspan="8">1</td></tr><tr><td>VIN, HIGH-Level Input Voltage</td><td></td><td>0.7*VDDIO</td><td></td><td>VDDIO + 0.5V</td><td>V</td></tr><tr><td>VIN, Hysteresis</td><td></td><td></td><td>0.1*VDDIO</td><td></td><td>V</td></tr><tr><td>VOI, LOW-Level Output Voltage</td><td>3 mA sink current</td><td>0</td><td></td><td>0.4</td><td>V</td></tr><tr><td>IOI, LOW-Level Output Current</td><td>VOI=0.4V</td><td></td><td>3</td><td></td><td>mA</td></tr><tr><td>VOI=0.6V</td><td></td><td></td><td>6</td><td></td><td>mA</td></tr><tr><td>Output Leakage Current</td><td></td><td></td><td>100</td><td></td><td>nA</td></tr><tr><td>tof, Output Fall Time from VIHmax to VILmax</td><td>Cb bus capacitance in pf</td><td>20+0.1Cb</td><td></td><td>300</td><td>ns</td></tr><tr><td colspan="7">INTERNAL CLOCK SOURCE</td></tr><tr><td>Clock Frequency Initial Tolerance</td><td>CLKSEL=2b00; 25℃</td><td>-3</td><td></td><td>+3</td><td>%</td><td>1</td></tr><tr><td>Frequency Variation over Temperature</td><td>CLKSEL=2b00; -40℃ to +85℃</td><td></td><td></td><td>+3</td><td>%</td><td>1</td></tr></table>

# Notes:

1. Expected results based on design, will be updated after characterization. Not tested in production. 
2. Guaranteed by design. 
3. Production tested.

# 3.3 I2C TIMING CHARACTERIZATION

Typical Operating Circuit of Section 3.2, VDD  $= 1.8V$  VDDIO  $= 1.8V$ $T_{A} = 25^{\circ}C$  unless otherwise noted.

<table><tr><td>PARAMETERS</td><td>CONDITIONS</td><td>MIN</td><td>TYPICAL</td><td>MAX</td><td>UNITS</td><td>NOTES</td></tr><tr><td>I2C TIMING</td><td>I2C FAST-MODE PLUS</td><td></td><td></td><td></td><td></td><td></td></tr><tr><td>fSCL, SCL Clock Frequency</td><td></td><td></td><td></td><td>1</td><td>MHz</td><td>1</td></tr><tr><td>tHD,STA, (Repeated) START Condition Hold Time</td><td></td><td>0.26</td><td></td><td></td><td>μs</td><td>1</td></tr><tr><td>tLOW, SCL Low Period</td><td></td><td>0.5</td><td></td><td></td><td>μs</td><td>1</td></tr><tr><td>tHIGH, SCL High Period</td><td></td><td>0.26</td><td></td><td></td><td>μs</td><td>1</td></tr><tr><td>tSULSTA, Repeated START Condition Setup Time</td><td></td><td>0.26</td><td></td><td></td><td>μs</td><td>1</td></tr><tr><td>tHD,DATA, SDA Data Hold Time</td><td></td><td>0</td><td></td><td></td><td>μs</td><td>1</td></tr><tr><td>tsULDATA, SDA Data Setup Time</td><td></td><td>50</td><td></td><td></td><td>ns</td><td>1</td></tr><tr><td>tr, SDA and SCL Rise Time</td><td>Cb bus cap. from 10 to 400 pF</td><td></td><td></td><td>120</td><td>ns</td><td>1</td></tr><tr><td>tr, SDA and SCL Fall Time</td><td>Cb bus cap. from 10 to 400 pF</td><td></td><td></td><td>120</td><td>ns</td><td>1</td></tr><tr><td>tsULSTO, STOP Condition Setup Time</td><td></td><td>0.5</td><td></td><td></td><td>μs</td><td>1</td></tr><tr><td>tBUF, Bus Free Time Between STOP and START Condition</td><td></td><td>0.5</td><td></td><td></td><td>μs</td><td>1</td></tr><tr><td>Cb, Capacitive Load for each Bus Line</td><td></td><td></td><td>&amp;lt; 400</td><td></td><td>pF</td><td>1</td></tr><tr><td>tVDD,DATA, Data Valid Time</td><td></td><td></td><td></td><td>0.45</td><td>μs</td><td>1</td></tr><tr><td>tVDDACK, Data Valid Acknowledge Time</td><td></td><td></td><td></td><td>0.45</td><td>μs</td><td>1</td></tr></table>

Notes: 1. Based on characterization of 5 parts over temperature and voltage as mounted on evaluation board or in sockets

![](images/3aee6d2bd2ba4bf9b465cc49e2202fae9ed4304913c82ec8d20a0ed9deb6a9a2.jpg)  
Table 4. I2C Timing Characteristics  Figure 1. I2C Bus Timing Diagram

# 3.4 SPI TIMING CHARACTERIZATION - 4-WIRE SPI MODE

Typical Operating Circuit of Section 3.2, VDD  $= 1.8V$  VDDO  $= 1.8V$ $T_{A} = 25^{\circ}C$  unless otherwise noted.

Table 5.4-Wire SPI Timing Characteristics (24-MHz Operation)  

<table><tr><td>PARAMETERS</td><td>CONDITIONS</td><td>MIN</td><td>TYP</td><td>MAX</td><td>UNITS</td><td>NOTES</td></tr><tr><td>SPI TIMING</td><td></td><td></td><td></td><td></td><td></td><td></td></tr><tr><td>fSPC, SCLK Clock Frequency</td><td>Default</td><td></td><td></td><td>24</td><td>MHz</td><td>1</td></tr><tr><td>tLOW, SCLK Low Period</td><td></td><td>17</td><td></td><td></td><td>ns</td><td>1</td></tr><tr><td>tHIGH, SCLK High Period</td><td></td><td>17</td><td></td><td></td><td>ns</td><td>1</td></tr><tr><td>tsU,CS, CS Setup Time</td><td></td><td>39</td><td></td><td></td><td>ns</td><td>1</td></tr><tr><td>tHD,CS, CS Hold Time</td><td></td><td>18</td><td></td><td></td><td>ns</td><td>1</td></tr><tr><td>tsU,SDI, SDI Setup Time</td><td></td><td>13</td><td></td><td></td><td>ns</td><td>1</td></tr><tr><td>tHD,SDI, SDI Hold Time</td><td></td><td>8</td><td></td><td></td><td>ns</td><td>1</td></tr><tr><td>tVD,SDO, SDO Valid Time</td><td>Cload = 20 pF</td><td></td><td></td><td>21.5</td><td>ns</td><td>1</td></tr><tr><td>tHD,SDO, SDO Hold Time</td><td>Cload = 20 pF</td><td>3.5</td><td></td><td></td><td>ns</td><td>1</td></tr><tr><td>tDIS,SDO, SDO Output Disable Time</td><td></td><td></td><td></td><td>28</td><td>ns</td><td>1</td></tr></table>

Notes: 1. Based on characterization of 5 parts over temperature and voltage as mounted on evaluation board or in sockets

![](images/c2057063731b053a2b583fb577a8bf5df53d887d90bcb52406cc3ebaae2a7448.jpg)  
Figure 2. 4-Wire SPI Bus Timing Diagram

# 3.5 SPI TIMING CHARACTERIZATION - 3-WIRE SPI MODE

Typical Operating Circuit of Section 3.2, VDD  $= 1.8V$  VDDO  $= 1.8V$ $T_{A} = 25^{\circ}C$  unless otherwise noted.

Table 6.3-Wire SPI Timing Characteristics (24-MHz Operation)  

<table><tr><td>PARAMETERS</td><td>CONDITIONS</td><td>MIN</td><td>TYP</td><td>MAX</td><td>UNITS</td><td>NOTES</td></tr><tr><td>SPI TIMING</td><td></td><td></td><td></td><td></td><td></td><td></td></tr><tr><td>fSPC, SCLK Clock Frequency</td><td>Default</td><td></td><td></td><td>24</td><td>MHz</td><td>1</td></tr><tr><td>tLOW, SCLK Low Period</td><td></td><td>17</td><td></td><td></td><td>ns</td><td>1</td></tr><tr><td>tHIGH, SCLK High Period</td><td></td><td>17</td><td></td><td></td><td>ns</td><td>1</td></tr><tr><td>tsU,CS, CS Setup Time</td><td></td><td>39</td><td></td><td></td><td>ns</td><td>1</td></tr><tr><td>tHD,CS, CS Hold Time</td><td></td><td>5</td><td></td><td></td><td>ns</td><td>1</td></tr><tr><td>tsU,SDIO, SDIO Input Setup Time</td><td></td><td>13</td><td></td><td></td><td>ns</td><td>1</td></tr><tr><td>tHD,SDIO, SDIO Input Hold Time</td><td></td><td>8</td><td></td><td></td><td>ns</td><td>1</td></tr><tr><td>tVD,SDIO, SDIO Output Valid Time</td><td>Cload = 20 pF</td><td></td><td></td><td>18.5</td><td>ns</td><td>1</td></tr><tr><td>tHD,SDIO, SDIO Output Hold Time</td><td>Cload = 20 pF</td><td>3.5</td><td></td><td></td><td>ns</td><td>1</td></tr><tr><td>tDIS,SDIO, SDIO Output Disable Time</td><td></td><td></td><td></td><td>28</td><td>ns</td><td>1</td></tr></table>

Notes: 1. Based on characterization of 5 parts over temperature and voltage as mounted on evaluation board or in sockets

![](images/c4c8434212cd7c69f9c9f23e7c5ba319141db8eafe8d1606b94954e91f60434d.jpg)  
Figure 3. 3-Wire SPI Bus Timing Diagram

# 3.6 RTC (CLKIN) TIMING CHARACTERIZATION

Typical Operating Circuit of section 4.2, VDD  $= 1.8V$  VDDIO  $= 1.8V$ $T_{A} = 25^{\circ}C_{1}$  unless otherwise noted.

Table 7.RTC Timing Characteristics  

<table><tr><td>PARAMETERS</td><td>CONDITIONS</td><td>MIN</td><td>TYP</td><td>MAX</td><td>UNITS</td><td>NOTES</td></tr><tr><td>RTC (CLKIN) TIMING</td><td></td><td></td><td></td><td></td><td></td><td></td></tr><tr><td>FRTC, RTC Clock Frequency</td><td>Default</td><td>31</td><td>32</td><td>50</td><td>kHz</td><td>1</td></tr><tr><td>tHIGHRTC, RTC Clock High Period</td><td></td><td>1</td><td></td><td></td><td>μs</td><td>1</td></tr><tr><td>tRISRTC, RTC Clock Rise Time</td><td></td><td>5</td><td></td><td>500</td><td>ns</td><td>1</td></tr><tr><td>tFALLRTC, RTC Clock Fall Time</td><td></td><td>5</td><td></td><td>500</td><td>ns</td><td>1</td></tr></table>

# Notes:

1. Based on characterization. Not tested in production.

![](images/d677fe1520faa5b661f1f5b4a25e1d02b5b80e6227b5e4da175624a4a1abbb3d.jpg)  
Figure 4. RTC Timing Diagram

# 3.7 ABSOLUTE MAXIMUM RATINGS

3.7 ABSOLUTE MAXIMUM RATINGSStress above those listed as "Absolute Maximum Ratings" may cause permanent damage to the device. These are stress ratings only and functional operation of the device at these conditions is not implied. Exposure to the absolute maximum ratings conditions for extended periods may affect device reliability. Prolonged exposure to acceleration ranges beyond  $\pm 60g$  may affect device reliability.

Table 8.Absolute Maximum Ratings  

<table><tr><td>PARAMETER</td><td>RATING</td></tr><tr><td>Supply Voltage, VDD</td><td>-0.5V to +4V</td></tr><tr><td>Supply Voltage, VDDIO</td><td>-0.5V to +4V</td></tr><tr><td>Input Voltage Level (FSYNC, SCL, SDA)</td><td>-0.5V to VDDIO + 0.5V</td></tr><tr><td>Acceleration (Any Axis, unpowered)</td><td>20,000g for 0.2 ms</td></tr><tr><td>Operating Temperature Range</td><td>-40℃ to +105℃</td></tr><tr><td>Storage Temperature Range</td><td>-40℃ to +125℃</td></tr><tr><td>Electrostatic Discharge (ESD) Protection</td><td>2 kV (HBM); 
500V (CDM)</td></tr><tr><td>Latch-up</td><td>JEDEC Class II (2), 125℃ 
±100 mA</td></tr></table>

# 4 APPLICATIONS INFORMATION

# 4.1 PIN OUT DIAGRAM AND SIGNAL DESCRIPTION

<table><tr><td>PIN NUMBER</td><td>PIN NAME</td><td>PIN DESCRIPTION</td><td>PIN STATUS</td></tr><tr><td>1</td><td>AP_SDO / AP_ADO</td><td>AP_SDO: AP SPI serial data output (4-wire mode);
AP_ADO: AP I3CSM / I2C slave address LSB</td><td>By default, pull-up/pull-down is disabled. Pull-up can be enabled by setting PIN1_PU_EN = 1 (register 0x0Eh in Bank 3). Pull-down can be enabled by setting PIN1_PD_EN = 1 (register 0x0Eh in Bank 3). Note that both pull-up and pull-down must not be simultaneously enabled for the same pin.</td></tr><tr><td>2</td><td>RESV</td><td>No Connect or Connect to GND</td><td>By default, pull-up is disabled. Pull-up can be enabled by setting PIN2_PU_EN = 1 (register 0x06h in Bank 3).</td></tr><tr><td>3</td><td>RESV</td><td>No Connect or Connect to GND</td><td>By default, pull-up is disabled. Pull-up can be enabled by setting PIN3_PU_EN = 1 (register 0x06h in Bank 3).</td></tr><tr><td>4</td><td>INT1 / INT</td><td>INT1: Interrupt 1 (Note: INT1 can be push-pull or open drain)
INT: All interrupts mapped to pin 4</td><td>By default, pull-down is disabled. Pull-down can be enabled by setting PIN4_PD_EN = 1 (register 0x06h in Bank 3).</td></tr><tr><td>5</td><td>VDDIO</td><td>IO power supply voltage</td><td></td></tr><tr><td>6</td><td>GND</td><td>Power supply ground</td><td></td></tr><tr><td>7</td><td>RESV</td><td>Connect to GND</td><td>By default, pull-up is disabled. Pull-up can be enabled by setting PIN7_PU_EN = 1 (register 0x06h in Bank 3) and it can be disabled by setting PIN7_PU_EN = 0.</td></tr><tr><td>8</td><td>VDD</td><td>Power supply voltage</td><td></td></tr><tr><td>9</td><td>INT2 / FSYNC / CLKIN</td><td>INT2: Interrupt 2 (Note: INT2 can be push-pull or open drain)
FSYNC: Frame sync input; Connect to GND if FSYNC not used
CLKIN: External Clock Input</td><td>By default, pull-down is disabled. Pull-down can be enabled by setting PIN9_PD_EN = 1 (register 0x06h in Bank 3).</td></tr><tr><td>10</td><td>RESV</td><td>No Connect or Connect to GND</td><td>By default, pull-up is enabled. Pull-up can be disabled by setting PIN10_PU_EN = 0 and it can be enabled by setting PIN10_PU_EN = 1 (register 0x06h in Bank 3).</td></tr><tr><td>11</td><td>RESV</td><td>No Connect or Connect to GND</td><td>By default, pull-up is enabled. Pull-up can be disabled by setting PIN11_PU_EN = 0 and it can be enabled by setting PIN11_PU_EN = 1 (register 0x06h in Bank 3).</td></tr><tr><td>12</td><td>AP_CS</td><td>AP SPI Chip select (AP SPI interface); Connect to VDDIO if using AP I3CSM / I2C interface</td><td>By default, pull-up is enabled. Pull-up can be disabled by setting PIN12_PU_EN = 0 (register 0x0Eh in Bank 3). Pull-down can be enabled by setting PIN12_PD_EN = 1 (register 0x0Eh in Bank 3). Note that both pull-up and pull-down must not be simultaneously enabled for the same pin.</td></tr><tr><td>13</td><td>AP_SCL / AP_SCLK</td><td>AP_SCL: AP I3CSM / I2C serial clock; AP_SCL: AP SPI serial clock</td><td>By default, pull-up/pull-down is disabled. Pull-up can be enabled by setting PIN13_PU_EN = 1 (register 0x0Eh in Bank 3). Pull-down can be enabled by setting PIN13_PD_EN = 1 (register 0x0Eh in Bank 3). Note that both pull-up and pull-down must not be simultaneously enabled for the same pin.</td></tr><tr><td>14</td><td>AP_SDA / AP_SDIO / AP_SDI</td><td>AP_SDA: AP I3CSM / I2C serial data; AP_SDIO: AP SPI serial data I/O (3-wire mode); AP_SDI: AP SPI serial data input (4-wire mode)</td><td>By default, pull-up/pull-down is disabled. Pull-up can be enabled by setting PIN14_PU_EN = 1 (register 0x0Eh in Bank 3). Pull-down can be enabled by setting PIN14_PD_EN = 1 (register 0x0Eh in Bank 3). Note that both pull-up and pull-down must not be simultaneously enabled for the same pin.</td></tr></table>

![](images/67f872da7dea7c7bcccc9b0d61b92e59dcbabec593546226e231d0ade4ccd814.jpg)  
Figure 5. Pin-Out Diagram for IIM-42352 2.5 mm x 3.0 mm x 6.91 mm LGA

# 4.2 TYPICAL OPERATING CIRCUIT

![](images/84a59af83aa205af7a20bea12191b91949a74237889aa6f462670d2f7d889f4b.jpg)  
Figure 6. IIM-42352 Application Schematic (I3C $^{SM}$ /I $^{2}$ C Interface to Host)

Note:  $I^{2}C$  lines are open drain and pull- up resistors (e.g.  $10k\Omega$ ) are required.

![](images/1dd4dbfd61068e01e4bf84f64b758b238b52dfa9d933f72aa2d64f4b74389bc1.jpg)  
Figure 7. IIM-42352 Application Schematic (SPI Interface to Host)

# 4.3 BILL OF MATERIALS FOR EXTERNAL COMPONENTS

Table 10.Bill of Materials  

<table><tr><td>COMPONENT</td><td>LABEL</td><td>SPECIFICATION</td><td>QUANTITY</td></tr><tr><td rowspan="2">VDD Bypass Capacitors</td><td>C1</td><td>X7R, 0.1μF ±10%</td><td>1</td></tr><tr><td>C2</td><td>X7R, 2.2μF ±10%</td><td>1</td></tr><tr><td>VDDIO Bypass Capacitor</td><td>C3</td><td>X7R, 10nF ±10%</td><td>1</td></tr></table>

# 4.4 SYSTEM BLOCK DIAGRAM

![](images/944311617b2252f35eaf83f7f38b6341e32b731935fc6e7efaa27fbaa46beb8c.jpg)  
Figure 8. IIM-42352 System Block Diagram

Note: The above block diagram is an example. Please refer to the pin- out (section 3.1) for other configuration options.

# 4.5 OVERVIEW

The IIM- 42352 is comprised of the following key blocks and functions:

Three- axis MEMS accelerometer sensor with 16- bit ADCs and signal conditioning O 20- bits data format support in FIFO for high- data resolution (see section 6 for details) I3Cm,  $1^{2}C,$  and SPI serial communications interfaces to Host Self- Test Clocking Sensor Data Registers FIFO Interrupts Digital- Output Temperature Sensor Bias and LDOS Charge Pump Standard Power Modes

# 4.6 THREE-AXIS MEMS ACCELEROMETER WITH 16-BIT ADCS AND SIGNAL CONDITIONING

The IIM- 42352 includes a 3- axis MEMS accelerometer. Acceleration along a particular axis induces displacement of a proof mass in the MEMS structure, and capacitive sensors detect the displacement. The IIM- 42352 architecture reduces the accelerometers' susceptibility to fabrication variations as well as to thermal drift. When the device is placed on a flat surface, it will measure  $0g$  on the  $X-$  and Y- axes and  $+1g$  on the Z- axis. The accelerometers' scale factor is calibrated at the factory and is nominally independent of supply voltage. The full- scale range of the digital output can be adjusted to  $\pm 2g,\pm 4g,\pm 8g$  and  $\pm 16g$

# 4.7  $13C^{SM},1^{2}C,$  AND SPI HOST INTERFACE

The IIM- 42352 communicates to the application processor using an  $13C^{SM},1^{2}C,$  or SPI serial interface. The IIM- 42352 always acts as a slave when communicating to the application processor.

# 4.8 SELF-TEST

Self- test allows for the testing of the mechanical and electrical portions of the sensors. The self- test for each measurement axis can be activated by means of the accelerometer self- test registers.

When the self- test is activated, the electronics cause the sensors to be actuated and produce an output signal. The output signal is used to observe the self- test response.

The self- test response is defined as follows:

SELF- TEST RESPONSE = SENSOR OUTPUT WITH SELF- TEST ENABLED - SENSOR OUTPUT WITH SELF- TEST DISABLED

When the value of the self- test response is within the specified min/max limits of the product specification, the part has passed self- test. When the self- test response exceeds the min/max values, the part is deemed to have failed self- test.

# 4.9 CLOCKING

The IIM- 42352 has a flexible clocking scheme, allowing the internal clock source to be used for the internal synchronous circuitry. This synchronous circuitry includes the signal conditioning and ADCs, and various control circuits and registers.

The internal clock for IIM- 42352 is generated by a relaxation oscillator. A highly accurate external clock may be used rather than the internal clock sources, if greater clock accuracy is desired. The CLKIN pin on IIM- 42352 provides the ability to input an external clock. External clock input supports highly accurate clock input from 31 kHz to  $50~\mathrm{kHz}$  resulting in improvement of the following:

a) ODR uncertainty due to process, temperature, and design limitations. This uncertainty can be as high as  $\pm 8\%$  in internal clock mode. The CLKIN, assuming a 50 ppm or better  $32.768~\mathrm{kHz}$  source, will improve the ODR accuracy from  $\pm 80,000$  ppm to  $\pm 50$  ppm in RCOSC mode. 
b) Sophisticated systems can estimate ODR inaccuracy to some extent but not to the extent improved by using CLKIN. 
c) System-level clock/sensor synchronization. When using CLKIN, the accelerometer is on the same clock as the host. There is no need to continually re-synchronize the sensor data as the sensor sample points and period are known to be in exact alignment with the common system clock. 
d) Other applications that benefit from CLKIN include navigation and robotics.

# 4.10 SENSOR DATA REGISTERS

The sensor data registers contain the latest accelerometer, and temperature measurement data. They are readonly registers and are accessed via the serial interface. Data from these registers may be read anytime.

# 4.11 INTERRUPTS

Interrupt functionality is configured via the Interrupt Configuration register. Items that are configurable include the interrupt pins configuration, the interrupt latching and clearing method, and triggers for the interrupt. Items that can trigger an interrupt are (1) new data is available to be read (from the FIFO and Data registers); (2) accelerometer event interrupts; (3) FIFO watermark; (4) FIFO overflow. The interrupt status can be read from the Interrupt Status register.

# 4.12 DIGITAL-OUTPUT TEMPERATURE SENSOR

An on- chip temperature sensor and ADC are used to measure the IIM- 42352 die temperature. The readings from the ADC can be read from the FIFO or the Sensor Data registers.

Temperature data value from the sensor data registers can be converted to degrees centigrade by using the following formula:

Temperature in Degrees Centigrade  $=$  (TEMP_DATA/132.48)+25

FIFO_TEMP_DATA, temperature data stored in FIFO, can be 8- bit or 16- it quantity. The 8- bit of temperature data stored in FIFO is limited to  $- 40^{\circ}C$  to  $85^{\circ}C$  range, while the 16- bit representation can support the full operating temperature range. It can be converted to degrees centigrade by using the following formula:

Temperature in Degrees Centigrade  $=$  (FIFO_TEMP_DATA/2.07)+25

# 4.13 BIAS AND LDOS

The bias and LDO section generate the internal supply and the reference voltages and currents required by the IIM- 42352.

# 4.14 CHARGE PUMP

An on- chip charge pump generates the high voltage required for the MEMS oscillator.

# 4.15 STANDARD POWER MODES

Table 11 lists the user- accessible power modes for IIM- 42352.

Table 11.Standard Power Modes for IIM-42352  

<table><tr><td>MODE</td><td>NAME</td><td>ACCEL</td></tr><tr><td>1</td><td>Sleep Mode</td><td>Off</td></tr><tr><td>2</td><td>Standby Mode</td><td>Off</td></tr><tr><td>3</td><td>Accelerometer Low-Power Mode</td><td>Duty-Cycled</td></tr><tr><td>4</td><td>Accelerometer Low-Noise Mode</td><td>On</td></tr></table>

# 5 SIGNAL PATH

Figure 9 shows a block diagram of the signal path for IIM- 42352.

![](images/3d20585a58d5fa0bfdd5c7a891ae1f6d9670cae9b797c95132b7e2c4f25221d0.jpg)  
Figure 9. IIM-42352 Signal Path

The signal path starts with ADCs for the accelerometer. Other components of the signal path are described below in further detail.

# 5.1 SUMMARY OF PARAMETERS USED TO CONFIGURE THE SIGNAL PATH

Table 12 shows the parameters that can control the signal path.

Table 12.Parameters used to control the signal path  

<table><tr><td>PARAMETER NAME</td><td>DESCRIPTION</td></tr><tr><td>ACCEL_AAF_DIS</td><td>Disables the Accelerometer Anti Alias Filter</td></tr><tr><td>ACCEL_AAF_DELT</td><td rowspan="3">Three parameters required to program the accelerometer AAF. This is a 2nd order filter with programmable low pass filter. This is a user programmable filter which can be used to select the desired BW. This filter allows trading off RMS noise vs. latency for a given ODR.</td></tr><tr><td>ACCEL_AAF_DELTSQR</td></tr><tr><td>ACCEL_AAF_BITSHIFT</td></tr></table>

# 5.2 ANTI-ALIAS FILTER

To program the anti- alias filter for a required bandwidth, use the table below to map the bandwidth to register values as shown:

1. Register bank 2, register  $0\times 03h$  , bits 6:1, ACCEL_AAF_DELT: Code from 1 to 63 that allows programming the bandwidth for accelerometer anti-alias filter 
2. Register bank 2, register  $0\times 04h$  , bits 7:0 and Bank 2, register  $0\times 05h$  , bits 3:0, ACCEL_AAF_DELTSQR: Square of the delt value for accelerometer 
3. Register bank 2, register  $0\times 05h$  , bits 7:4, ACCEL_AAF_BITSHIFT: Bitshift value for accelerometer used in hardware implementation

For vibration sensing applications, the recommended settings are ACCEL_AAF_DIS=0; ACCEL_AAF_DELT:63; ACCEL_AAF_DELTSQR: 3968; ACCEL_AAF_BITSHIFT=3.

<table><tr><td>3DB BANDWIDTH (HZ)</td><td>ACCEL_AAF_DELT</td><td>ACCEL_AAF_DELTSQR</td><td>ACCEL_AAF_BITSHIFT</td></tr><tr><td>42</td><td>1</td><td>1</td><td>15</td></tr><tr><td>84</td><td>2</td><td>4</td><td>13</td></tr><tr><td>126</td><td>3</td><td>9</td><td>12</td></tr><tr><td>170</td><td>4</td><td>16</td><td>11</td></tr><tr><td>213</td><td>5</td><td>25</td><td>10</td></tr><tr><td>258</td><td>6</td><td>36</td><td>10</td></tr><tr><td>303</td><td>7</td><td>49</td><td>9</td></tr><tr><td>348</td><td>8</td><td>64</td><td>9</td></tr><tr><td>394</td><td>9</td><td>81</td><td>9</td></tr><tr><td>441</td><td>10</td><td>100</td><td>8</td></tr><tr><td>488</td><td>11</td><td>122</td><td>8</td></tr><tr><td>536</td><td>12</td><td>144</td><td>8</td></tr><tr><td>585</td><td>13</td><td>170</td><td>8</td></tr><tr><td>634</td><td>14</td><td>196</td><td>7</td></tr><tr><td>684</td><td>15</td><td>224</td><td>7</td></tr><tr><td>734</td><td>16</td><td>256</td><td>7</td></tr><tr><td>785</td><td>17</td><td>288</td><td>7</td></tr><tr><td>837</td><td>18</td><td>324</td><td>7</td></tr><tr><td>890</td><td>19</td><td>360</td><td>6</td></tr><tr><td>943</td><td>20</td><td>400</td><td>6</td></tr><tr><td>997</td><td>21</td><td>440</td><td>6</td></tr><tr><td>1051</td><td>22</td><td>488</td><td>6</td></tr><tr><td>1107</td><td>23</td><td>528</td><td>6</td></tr><tr><td>1163</td><td>24</td><td>576</td><td>6</td></tr><tr><td>1220</td><td>25</td><td>624</td><td>6</td></tr><tr><td>1277</td><td>26</td><td>680</td><td>6</td></tr><tr><td>1336</td><td>27</td><td>736</td><td>5</td></tr><tr><td>1395</td><td>28</td><td>784</td><td>5</td></tr><tr><td>1454</td><td>29</td><td>848</td><td>5</td></tr><tr><td>1515</td><td>30</td><td>896</td><td>5</td></tr><tr><td>1577</td><td>31</td><td>960</td><td>5</td></tr><tr><td>1639</td><td>32</td><td>1024</td><td>5</td></tr><tr><td>1702</td><td>33</td><td>1088</td><td>5</td></tr><tr><td>1766</td><td>34</td><td>1152</td><td>5</td></tr><tr><td>1830</td><td>35</td><td>1232</td><td>5</td></tr><tr><td>1896</td><td>36</td><td>1296</td><td>5</td></tr><tr><td>1962</td><td>37</td><td>1376</td><td>4</td></tr><tr><td>2029</td><td>38</td><td>1440</td><td>4</td></tr><tr><td>2097</td><td>39</td><td>1536</td><td>4</td></tr><tr><td>2166</td><td>40</td><td>1600</td><td>4</td></tr></table>

<table><tr><td>3DB BANDWIDTH (HZ)</td><td>ACCEL_AAF_DELT</td><td>ACCEL_AAF_DELTSQR</td><td>ACCEL_AAF_BITSHIFT</td></tr><tr><td>2235</td><td>41</td><td>1696</td><td>4</td></tr><tr><td>2306</td><td>42</td><td>1760</td><td>4</td></tr><tr><td>2377</td><td>43</td><td>1856</td><td>4</td></tr><tr><td>2449</td><td>44</td><td>1952</td><td>4</td></tr><tr><td>2522</td><td>45</td><td>2016</td><td>4</td></tr><tr><td>2596</td><td>46</td><td>2112</td><td>4</td></tr><tr><td>2671</td><td>47</td><td>2208</td><td>4</td></tr><tr><td>2746</td><td>48</td><td>2304</td><td>4</td></tr><tr><td>2823</td><td>49</td><td>2400</td><td>4</td></tr><tr><td>2900</td><td>50</td><td>2496</td><td>4</td></tr><tr><td>2978</td><td>51</td><td>2592</td><td>4</td></tr><tr><td>3057</td><td>52</td><td>2720</td><td>4</td></tr><tr><td>3137</td><td>53</td><td>2816</td><td>3</td></tr><tr><td>3217</td><td>54</td><td>2944</td><td>3</td></tr><tr><td>3299</td><td>55</td><td>3008</td><td>3</td></tr><tr><td>3381</td><td>56</td><td>3136</td><td>3</td></tr><tr><td>3464</td><td>57</td><td>3264</td><td>3</td></tr><tr><td>3548</td><td>58</td><td>3392</td><td>3</td></tr><tr><td>3633</td><td>59</td><td>3456</td><td>3</td></tr><tr><td>3718</td><td>60</td><td>3584</td><td>3</td></tr><tr><td>3805</td><td>61</td><td>3712</td><td>3</td></tr><tr><td>3892</td><td>62</td><td>3840</td><td>3</td></tr><tr><td>3979</td><td>63</td><td>3968</td><td>3</td></tr></table>

The anti- alias filter can be selected or bypassed for the accelerometer by using the parameter ACCEL_AAF_DIS in register bank 2, register  $0\times 03h$  bit O as shown below.

<table><tr><td>ACCEL_AAF_DIS</td><td>FUNCTION</td></tr><tr><td>0</td><td>Enable accelerometer anti-aliasing filter</td></tr><tr><td>1</td><td>Disable accelerometer anti-aliasing filter</td></tr></table>

# 5.3 USER PROGRAMMABLE OFFSET

Accelerometer offsets can be programmed by the user by using registers OFFSET_USER4, through OFFSET_USER8, in bank 0, registers  $0\times 7Bh$  through  $0\times 7Fh$  (bank 4) as shown below.

<table><tr><td>REGISTER ADDRESS</td><td>REGISTER NAME</td><td>BITS</td><td>FUNCTION</td></tr><tr><td rowspan="2">0x7Bh</td><td rowspan="2">OFFSET_USER4</td><td>3:0</td><td>Reserved</td></tr><tr><td>7:4</td><td>Upper bits of X-accel offset programmed by user. Max value is ±1g, resolution is 0.5 mg.</td></tr><tr><td>0x7Ch</td><td>OFFSET_USER5</td><td>7:0</td><td>Lower bits of X-accel offset programmed by user. Max value is ±1g, resolution is 0.5 mg.</td></tr><tr><td>0x7Dh</td><td>OFFSET_USER6</td><td>7:0</td><td>Lower bits of Y-accel offset programmed by user. Max value is ±1g, resolution is 0.5 mg.</td></tr></table>

<table><tr><td>REGISTER ADDRESS</td><td>REGISTER NAME</td><td>BITS</td><td>FUNCTION</td></tr><tr><td rowspan="2">0x7Eh</td><td rowspan="2">OFFSET_USER7</td><td>3:0</td><td>Upper bits of Y-accel offset programmed by user. Max value is ±1g, resolution is 0.5 mg.</td></tr><tr><td>7:4</td><td>Upper bits of Z-accel offset programmed by user. Max value is ±1g, resolution is 0.5 mg.</td></tr><tr><td>0x7Fh</td><td>OFFSET_USER8</td><td>7:0</td><td>Lower bits of Z-accel offset programmed by user. Max value is ±1g, resolution is 0.5 mg.</td></tr></table>

# 5.4 UI FILTER BLOCK

The UI filter block can be programmed to select filter order and bandwidth for accelerometer.

Accelerometer filter order can be selected by programming the parameter ACCEL_UI_FILT_ORD in register bank 0, register 0x53h, bits 4:3, as shown below.

<table><tr><td>ACCEL_UI_FILT_ORD</td><td>FILTER ORDER</td></tr><tr><td>00</td><td>1st order</td></tr><tr><td>01</td><td>2nd order</td></tr><tr><td>10</td><td>3rd order</td></tr><tr><td>11</td><td>Reserved</td></tr></table>

Accelerometer filter 3dB bandwidth can be selected by programming the parameter ACCEL_UI_FILT_BW in register bank 0, register 0x52h, bits 7:4, as shown below. The values shown in bold correspond to low noise and the values shown in italics correspond to low latency. User can select the appropriate setting based on the application requirements for power and latency. Corresponding Noise Bandwidth (NBW) and Group Delay values are also shown.

5.4.1 1st Order Filter 3dB Bandwidth, Noise Bandwidth (NBW), Group Delay  

<table><tr><td colspan="12">3DB BANDWIDTH (HZ) FOR ACCEL_UI_FILT_ORD=0 (1ST ORDER FILTER)</td></tr><tr><td></td><td></td><td colspan="10">ACCEL_UI_FILT_BW</td></tr><tr><td>ACCEL_ODR</td><td>ODR(HZ)</td><td>0</td><td>1</td><td>2</td><td>3</td><td>4</td><td>5</td><td>6</td><td>7</td><td>14</td><td>15</td></tr><tr><td>1</td><td>32000</td><td></td><td></td><td></td><td></td><td>8400.0</td><td></td><td></td><td></td><td></td><td></td></tr><tr><td>2</td><td>16000</td><td></td><td></td><td></td><td></td><td>4194.1</td><td></td><td></td><td></td><td></td><td></td></tr><tr><td>3</td><td>8000</td><td></td><td></td><td></td><td></td><td>2096.30</td><td></td><td></td><td></td><td></td><td></td></tr><tr><td>4</td><td>4000</td><td></td><td></td><td></td><td></td><td>1048.10</td><td></td><td></td><td></td><td></td><td></td></tr><tr><td>5</td><td>2000</td><td></td><td></td><td></td><td></td><td>524.00</td><td></td><td></td><td></td><td></td><td></td></tr><tr><td>6</td><td>1000</td><td>498.30</td><td>227.20</td><td>188.90</td><td>111.00</td><td>92.40</td><td>59.60</td><td>48.80</td><td>23.90</td><td>152.00</td><td>2096.30</td></tr><tr><td>15</td><td>500</td><td>249.10</td><td>113.60</td><td>94.40</td><td>55.50</td><td>46.20</td><td>29.80</td><td>24.40</td><td>11.90</td><td>131.00</td><td>1048.10</td></tr><tr><td>7</td><td>200</td><td>99.60</td><td>90.90</td><td>75.50</td><td>44.40</td><td>37.00</td><td>23.80</td><td>19.50</td><td>9.60</td><td>104.80</td><td>419.20</td></tr><tr><td>8</td><td>100</td><td>49.80</td><td>90.90</td><td>75.50</td><td>44.40</td><td>37.00</td><td>23.80</td><td>19.50</td><td>9.60</td><td>104.80</td><td>209.60</td></tr><tr><td>9</td><td>50</td><td>24.90</td><td>90.90</td><td>75.50</td><td>44.40</td><td>37.00</td><td>23.80</td><td>19.50</td><td>9.60</td><td>104.80</td><td>104.80</td></tr><tr><td>10</td><td>25</td><td>12.50</td><td>90.90</td><td>75.50</td><td>44.40</td><td>37.00</td><td>23.80</td><td>19.50</td><td>9.60</td><td>104.80</td><td>52.40</td></tr><tr><td>11</td><td>12.5</td><td>12.50</td><td>90.90</td><td>75.50</td><td>44.40</td><td>37.00</td><td>23.80</td><td>19.50</td><td>9.60</td><td>104.80</td><td>52.40</td></tr></table>

<table><tr><td></td><td colspan="11">NBW BANDWIDTH (HZ) FOR ACCEL_UI_FILT_ORD=0 (1ST ORDER FILTER)</td></tr><tr><td></td><td colspan="11">ACCEL_UI_FILT_BW</td></tr><tr><td>ACCEL_ODR</td><td>ODR(HZ)</td><td>0</td><td>1</td><td>2</td><td>3</td><td>4</td><td>5</td><td>6</td><td>7</td><td>14</td><td>15</td></tr><tr><td>1</td><td>32000</td><td colspan="10">8831.7</td></tr><tr><td>2</td><td>16000</td><td colspan="10">4410.6</td></tr><tr><td>3</td><td>8000</td><td colspan="10">2204.6</td></tr><tr><td>4</td><td>4000</td><td colspan="10">1102.2</td></tr><tr><td>5</td><td>2000</td><td colspan="10">551.1</td></tr><tr><td>6</td><td>1000</td><td>551.1</td><td>230.8</td><td>196.3</td><td>126.5</td><td>108.9</td><td>75.8</td><td>64.1</td><td>34.1</td><td>275.6</td><td>2204.6</td></tr><tr><td>15</td><td>500</td><td>280.5</td><td>115.4</td><td>98.2</td><td>63.3</td><td>54.5</td><td>37.9</td><td>32.1</td><td>17.1</td><td>137.8</td><td>1102.2</td></tr><tr><td>7</td><td>200</td><td>112.2</td><td>92.4</td><td>78.5</td><td>50.6</td><td>43.6</td><td>30.3</td><td>25.7</td><td>13.7</td><td>110.3</td><td>440.9</td></tr><tr><td>8</td><td>100</td><td>56.1</td><td>92.4</td><td>78.5</td><td>50.6</td><td>43.6</td><td>30.3</td><td>25.7</td><td>13.7</td><td>110.3</td><td>220.5</td></tr><tr><td>9</td><td>50</td><td>28.1</td><td>92.4</td><td>78.5</td><td>50.6</td><td>43.6</td><td>30.3</td><td>25.7</td><td>13.7</td><td>110.3</td><td>110.3</td></tr><tr><td>10</td><td>25</td><td>14.1</td><td>92.4</td><td>78.5</td><td>50.6</td><td>43.6</td><td>30.3</td><td>25.7</td><td>13.7</td><td>110.3</td><td>55.2</td></tr><tr><td>11</td><td>12.5</td><td>14.1</td><td>92.4</td><td>78.5</td><td>50.6</td><td>43.6</td><td>30.3</td><td>25.7</td><td>13.7</td><td>110.3</td><td>55.2</td></tr></table>

<table><tr><td></td><td colspan="11">GROUP DELAY @DC (MS) FOR ACCEL_UI_FILT_ORD=0 (1ST ORDER FILTER)</td></tr><tr><td></td><td colspan="11">ACCEL_UI_FILT_BW</td></tr><tr><td>ACCEL_ODR</td><td>ODR(HZ)</td><td>0</td><td>1</td><td>2</td><td>3</td><td>4</td><td>5</td><td>6</td><td>7</td><td>14</td><td>15</td></tr><tr><td>1</td><td>32000</td><td colspan="10">0.1</td></tr><tr><td>2</td><td>16000</td><td colspan="10">0.1</td></tr><tr><td>3</td><td>8000</td><td colspan="10">0.2</td></tr><tr><td>4</td><td>4000</td><td colspan="10">0.4</td></tr><tr><td>5</td><td>2000</td><td colspan="10">0.8</td></tr><tr><td>6</td><td>1000</td><td>0.6</td><td>1.8</td><td>2.0</td><td>2.8</td><td>3.1</td><td>4.1</td><td>4.7</td><td>8.1</td><td>1.5</td><td>0.2</td></tr><tr><td>15</td><td>500</td><td>1.1</td><td>3.6</td><td>4.0</td><td>5.5</td><td>6.1</td><td>8.1</td><td>9.3</td><td>16.2</td><td>3.0</td><td>0.4</td></tr><tr><td>7</td><td>200</td><td>2.7</td><td>4.4</td><td>5.0</td><td>6.8</td><td>7.6</td><td>10.2</td><td>11.7</td><td>20.3</td><td>3.8</td><td>1.0</td></tr><tr><td>8</td><td>100</td><td>5.3</td><td>4.4</td><td>5.0</td><td>6.8</td><td>7.6</td><td>10.2</td><td>11.7</td><td>20.3</td><td>3.8</td><td>1.9</td></tr><tr><td>9</td><td>50</td><td>10.5</td><td>4.4</td><td>5.0</td><td>6.8</td><td>7.6</td><td>10.2</td><td>11.7</td><td>20.3</td><td>3.8</td><td>3.8</td></tr><tr><td>10</td><td>25</td><td>21.0</td><td>4.4</td><td>5.0</td><td>6.8</td><td>7.6</td><td>10.2</td><td>11.7</td><td>20.3</td><td>3.8</td><td>7.5</td></tr><tr><td>11</td><td>12.5</td><td>21.0</td><td>4.4</td><td>5.0</td><td>6.8</td><td>7.6</td><td>10.2</td><td>11.7</td><td>20.3</td><td>3.8</td><td>7.5</td></tr></table>

# 5.4.2 2nd Order Filter 3dB Bandwidth, Noise Bandwidth (NBW), Group Delay

<table><tr><td></td><td colspan="11">3DB BANDWIDTH (HZ) FOR ACCEL_UI_FILT_ORD=1 (2ND ORDER FILTER)</td></tr><tr><td></td><td colspan="11">ACCEL_UI_FILT_BW</td></tr><tr><td>ACCEL_ODR</td><td>ODR(HZ)</td><td>1</td><td>2</td><td>3</td><td>4</td><td>5</td><td>6</td><td>7</td><td>14</td><td>15</td><td></td></tr><tr><td>1</td><td>32000</td><td colspan="10">8400.0</td></tr><tr><td>2</td><td>16000</td><td colspan="10">4194.1</td></tr><tr><td>3</td><td>8000</td><td colspan="10">2096.3</td></tr><tr><td>4</td><td>4000</td><td colspan="10">1048.1</td></tr><tr><td>5</td><td>2000</td><td colspan="10">524.0</td></tr><tr><td>6</td><td>1000</td><td>493.3</td><td>230.7</td><td>191.6</td><td>117.5</td><td>97.1</td><td>59.6</td><td>48.0</td><td>21.3</td><td>262.0</td><td>2096.3</td></tr></table>

<table><tr><td>15</td><td>500</td><td>246.7</td><td>115.3</td><td>95.8</td><td>58.8</td><td>48.5</td><td>29.8</td><td>24.0</td><td>10.6</td><td>131.0</td><td>1048.1</td></tr><tr><td>7</td><td>200</td><td>98.7</td><td>92.3</td><td>76.6</td><td>47.0</td><td>38.8</td><td>23.8</td><td>19.2</td><td>8.5</td><td>104.8</td><td>419.2</td></tr><tr><td>8</td><td>100</td><td>49.3</td><td>92.3</td><td>76.6</td><td>47.0</td><td>38.8</td><td>23.8</td><td>19.2</td><td>8.5</td><td>104.8</td><td>209.6</td></tr><tr><td>9</td><td>50</td><td>24.7</td><td>92.3</td><td>76.6</td><td>47.0</td><td>38.8</td><td>23.8</td><td>19.2</td><td>8.5</td><td>104.8</td><td>104.8</td></tr><tr><td>10</td><td>25</td><td>12.3</td><td>92.3</td><td>76.6</td><td>47.0</td><td>38.8</td><td>23.8</td><td>19.2</td><td>8.5</td><td>104.8</td><td>52.4</td></tr><tr><td>11</td><td>12.5</td><td>12.3</td><td>92.3</td><td>76.6</td><td>47.0</td><td>38.8</td><td>23.8</td><td>19.2</td><td>8.5</td><td>104.8</td><td>52.4</td></tr></table>

<table><tr><td></td><td colspan="11">NEW BANDWIDTH (HZ) FOR ACCEL_UI_FILT_ORD=1 (2ND ORDER FILTER)</td></tr><tr><td></td><td></td><td colspan="10">ACCEL_UI_FILT_BW</td></tr><tr><td>ACCEL_ODR</td><td>ODR(HZ)</td><td>0</td><td>1</td><td>2</td><td>3</td><td>4</td><td>5</td><td>6</td><td>7</td><td>14</td><td>15</td></tr><tr><td>1</td><td>32000</td><td colspan="10">8831.7</td></tr><tr><td>2</td><td>16000</td><td colspan="10">4410.6</td></tr><tr><td>3</td><td>8000</td><td colspan="10">2204.6</td></tr><tr><td>4</td><td>4000</td><td colspan="10">1102.2</td></tr><tr><td>5</td><td>2000</td><td colspan="10">551.1</td></tr><tr><td>6</td><td>1000</td><td>551.1</td><td>223.7</td><td>189.9</td><td>122.7</td><td>102.8</td><td>64.7</td><td>52.5</td><td>23.7</td><td>275.6</td><td>2204.6</td></tr><tr><td>15</td><td>500</td><td>259.6</td><td>111.9</td><td>95.0</td><td>61.4</td><td>51.4</td><td>32.4</td><td>26.3</td><td>11.9</td><td>137.8</td><td>1102.2</td></tr><tr><td>7</td><td>200</td><td>103.9</td><td>89.5</td><td>76.0</td><td>49.1</td><td>41.2</td><td>25.9</td><td>21.0</td><td>9.5</td><td>110.3</td><td>440.9</td></tr><tr><td>8</td><td>100</td><td>52.0</td><td>89.5</td><td>76.0</td><td>49.1</td><td>41.2</td><td>25.9</td><td>21.0</td><td>9.5</td><td>110.3</td><td>220.5</td></tr><tr><td>9</td><td>50</td><td>26.0</td><td>89.5</td><td>76.0</td><td>49.1</td><td>41.2</td><td>25.9</td><td>21.0</td><td>9.5</td><td>110.3</td><td>110.3</td></tr><tr><td>10</td><td>25</td><td>13.0</td><td>89.5</td><td>76.0</td><td>49.1</td><td>41.2</td><td>25.9</td><td>21.0</td><td>9.5</td><td>110.3</td><td>55.2</td></tr><tr><td>11</td><td>12.5</td><td>13.0</td><td>89.5</td><td>76.0</td><td>49.1</td><td>41.2</td><td>25.9</td><td>21.0</td><td>9.5</td><td>110.3</td><td>55.2</td></tr></table>

<table><tr><td></td><td colspan="11">GROUP DELAY @DC (MS) FOR ACCEL_UI_FILT_ORD=1 (2ND ORDER FILTER)</td></tr><tr><td></td><td></td><td colspan="10">ACCEL_UI_FILT_BW</td></tr><tr><td>ACCEL_ODR</td><td>ODR(HZ)</td><td>0</td><td>1</td><td>2</td><td>3</td><td>4</td><td>5</td><td>6</td><td>7</td><td>14</td><td>15</td></tr><tr><td>1</td><td>32000</td><td colspan="10">0.1</td></tr><tr><td>2</td><td>16000</td><td colspan="10">0.1</td></tr><tr><td>3</td><td>8000</td><td colspan="10">0.2</td></tr><tr><td>4</td><td>4000</td><td colspan="10">0.4</td></tr><tr><td>5</td><td>2000</td><td colspan="10">0.8</td></tr><tr><td>6</td><td>1000</td><td>0.7</td><td>2.1</td><td>2.4</td><td>3.2</td><td>3.7</td><td>5.2</td><td>6.1</td><td>12.0</td><td>1.5</td><td>0.2</td></tr><tr><td>15</td><td>500</td><td>1.3</td><td>4.1</td><td>4.7</td><td>6.4</td><td>7.3</td><td>10.4</td><td>12.2</td><td>24.0</td><td>3.0</td><td>0.4</td></tr><tr><td>7</td><td>200</td><td>3.3</td><td>5.1</td><td>5.8</td><td>8.0</td><td>9.1</td><td>12.9</td><td>15.3</td><td>30.0</td><td>3.8</td><td>1.0</td></tr><tr><td>8</td><td>100</td><td>6.5</td><td>5.1</td><td>5.8</td><td>8.0</td><td>9.1</td><td>12.9</td><td>15.3</td><td>30.0</td><td>3.8</td><td>1.9</td></tr><tr><td>9</td><td>50</td><td>12.9</td><td>5.1</td><td>5.8</td><td>8.0</td><td>9.1</td><td>12.9</td><td>15.3</td><td>30.0</td><td>3.8</td><td>3.8</td></tr><tr><td>10</td><td>25</td><td>25.7</td><td>5.1</td><td>5.8</td><td>8.0</td><td>9.1</td><td>12.9</td><td>15.3</td><td>30.0</td><td>3.8</td><td>7.5</td></tr><tr><td>11</td><td>12.5</td><td>25.7</td><td>5.1</td><td>5.8</td><td>8.0</td><td>9.1</td><td>12.9</td><td>15.3</td><td>30.0</td><td>3.8</td><td>7.5</td></tr></table>

5.4.3  $3^{\mathrm{rd}}$  Order Filter 3dB Bandwidth, Noise Bandwidth (NBW), Group Delay  

<table><tr><td></td><td colspan="11">3DB BANDWIDTH (HZ) FOR ACCEL_UI_FILT_ORD=2 (3RD ORDER FILTER)</td></tr><tr><td></td><td></td><td colspan="10">ACCEL_UI_FILT_BW</td></tr><tr><td>ACCEL_ODR</td><td>ODR(HZ)</td><td>0</td><td>1</td><td>2</td><td>3</td><td>4</td><td>5</td><td>6</td><td>7</td><td>14</td><td>15</td></tr><tr><td>1</td><td>32000</td><td></td><td></td><td></td><td></td><td>8400.0</td><td></td><td></td><td></td><td></td><td></td></tr><tr><td>2</td><td>16000</td><td></td><td></td><td></td><td></td><td>4194.1</td><td></td><td></td><td></td><td></td><td></td></tr><tr><td>3</td><td>8000</td><td></td><td></td><td></td><td></td><td>2096.3</td><td></td><td></td><td></td><td></td><td></td></tr><tr><td>4</td><td>4000</td><td></td><td></td><td></td><td></td><td>1048.1</td><td></td><td></td><td></td><td></td><td></td></tr><tr><td>5</td><td>2000</td><td></td><td></td><td></td><td></td><td>524.0</td><td></td><td></td><td></td><td></td><td></td></tr><tr><td>6</td><td>1000</td><td>492.9</td><td>234.7</td><td>195.8</td><td>118.9</td><td>97.9</td><td>60.8</td><td>46.8</td><td>25.2</td><td>262.0</td><td>2096.3</td></tr><tr><td>15</td><td>500</td><td>246.4</td><td>117.4</td><td>97.9</td><td>59.5</td><td>48.9</td><td>30.4</td><td>23.4</td><td>12.6</td><td>131.0</td><td>1048.1</td></tr><tr><td>7</td><td>200</td><td>98.6</td><td>93.9</td><td>78.3</td><td>47.6</td><td>39.2</td><td>24.3</td><td>18.7</td><td>10.1</td><td>104.8</td><td>419.2</td></tr><tr><td>8</td><td>100</td><td>49.3</td><td>93.9</td><td>78.3</td><td>47.6</td><td>39.2</td><td>24.3</td><td>18.7</td><td>10.1</td><td>104.8</td><td>209.6</td></tr><tr><td>9</td><td>50</td><td>24.6</td><td>93.9</td><td>78.3</td><td>47.6</td><td>39.2</td><td>24.3</td><td>18.7</td><td>10.1</td><td>104.8</td><td>104.8</td></tr><tr><td>10</td><td>25</td><td>12.3</td><td>93.9</td><td>78.3</td><td>47.6</td><td>39.2</td><td>24.3</td><td>18.7</td><td>10.1</td><td>104.8</td><td>52.4</td></tr><tr><td>11</td><td>12.5</td><td>12.3</td><td>93.9</td><td>78.3</td><td>47.6</td><td>39.2</td><td>24.3</td><td>18.7</td><td>10.1</td><td>104.8</td><td>52.4</td></tr></table>

<table><tr><td></td><td colspan="11">NBW BANDWIDTH (HZ) FOR ACCEL_UI_FILT_ORD=2 (3RD ORDER FILTER)</td><td></td></tr><tr><td></td><td></td><td colspan="10">ACCEL_UI_FILT_BW</td><td></td></tr><tr><td></td><td>ACCEL_ODR</td><td>ODR(HZ)</td><td>0</td><td>1</td><td>2</td><td>3</td><td>4</td><td>5</td><td>6</td><td>7</td><td>14</td><td>15</td></tr><tr><td>1</td><td>32000</td><td></td><td></td><td></td><td></td><td>8831.7</td><td></td><td></td><td></td><td></td><td></td><td></td></tr><tr><td>2</td><td>16000</td><td></td><td></td><td></td><td></td><td>4410.6</td><td></td><td></td><td></td><td></td><td></td><td></td></tr><tr><td>3</td><td>8000</td><td></td><td></td><td></td><td></td><td>2204.6</td><td></td><td></td><td></td><td></td><td></td><td></td></tr><tr><td>4</td><td>4000</td><td></td><td></td><td></td><td></td><td>1102.2</td><td></td><td></td><td></td><td></td><td></td><td></td></tr><tr><td>5</td><td>2000</td><td></td><td></td><td></td><td></td><td>551.1</td><td></td><td></td><td></td><td></td><td></td><td></td></tr><tr><td>6</td><td>1000</td><td>551.1</td><td>221.3</td><td>188.5</td><td>120.1</td><td>100.0</td><td>62.9</td><td>48.6</td><td>26.4</td><td>275.6</td><td>2204.6</td><td></td></tr><tr><td>15</td><td>500</td><td>252.0</td><td>110.7</td><td>94.3</td><td>60.1</td><td>50.0</td><td>31.5</td><td>24.3</td><td>13.2</td><td>137.8</td><td>1102.2</td><td></td></tr><tr><td>7</td><td>200</td><td>100.8</td><td>88.6</td><td>75.4</td><td>48.1</td><td>40.0</td><td>25.2</td><td>19.5</td><td>10.6</td><td>110.3</td><td>440.9</td><td></td></tr><tr><td>8</td><td>100</td><td>50.4</td><td>88.6</td><td>75.4</td><td>48.1</td><td>40.0</td><td>25.2</td><td>19.5</td><td>10.6</td><td>110.3</td><td>220.5</td><td></td></tr><tr><td>9</td><td>50</td><td>25.2</td><td>88.6</td><td>75.4</td><td>48.1</td><td>40.0</td><td>25.2</td><td>19.5</td><td>10.6</td><td>110.3</td><td>110.3</td><td></td></tr><tr><td>10</td><td>25</td><td>12.6</td><td>88.6</td><td>75.4</td><td>48.1</td><td>40.0</td><td>25.2</td><td>19.5</td><td>10.6</td><td>110.3</td><td>55.2</td><td></td></tr><tr><td>11</td><td>12.5</td><td>12.6</td><td>88.6</td><td>75.4</td><td>48.1</td><td>40.0</td><td>25.2</td><td>19.5</td><td>10.6</td><td>110.3</td><td>55.2</td><td></td></tr></table>

<table><tr><td></td><td colspan="11">GROUP DELAY @DC (MS) FOR ACCEL_UI_FILT_ORD=2 (3RD ORDER FILTER)</td></tr><tr><td></td><td></td><td colspan="10">ACCEL_UI_FILT_BW</td></tr><tr><td>ACCEL_0DR</td><td>ODR(HZ)</td><td>0</td><td>1</td><td>2</td><td>3</td><td>4</td><td>5</td><td>6</td><td>7</td><td>14</td><td>15</td></tr><tr><td>1</td><td>32000</td><td></td><td></td><td></td><td></td><td>0.1</td><td></td><td></td><td></td><td></td><td></td></tr><tr><td>2</td><td>16000</td><td></td><td></td><td></td><td></td><td>0.1</td><td></td><td></td><td></td><td></td><td></td></tr><tr><td>3</td><td>8000</td><td></td><td></td><td></td><td></td><td>0.2</td><td></td><td></td><td></td><td></td><td></td></tr><tr><td>4</td><td>4000</td><td></td><td></td><td></td><td></td><td>0.4</td><td></td><td></td><td></td><td></td><td></td></tr><tr><td>5</td><td>2000</td><td></td><td></td><td></td><td></td><td>0.8</td><td></td><td></td><td></td><td></td><td></td></tr></table>

<table><tr><td>6</td><td>1000</td><td>0.8</td><td>2.3</td><td>2.7</td><td>4.0</td><td>4.6</td><td>6.6</td><td>8.2</td><td>14.1</td><td>1.5</td><td>0.2</td></tr><tr><td>15</td><td>500</td><td>1.6</td><td>4.6</td><td>5.4</td><td>7.9</td><td>9.2</td><td>13.2</td><td>16.3</td><td>28.1</td><td>3.0</td><td>0.4</td></tr><tr><td>7</td><td>200</td><td>4.0</td><td>5.8</td><td>6.8</td><td>9.8</td><td>11.4</td><td>16.5</td><td>20.4</td><td>35.2</td><td>3.8</td><td>1.0</td></tr><tr><td>8</td><td>100</td><td>8.0</td><td>5.8</td><td>6.8</td><td>9.8</td><td>11.4</td><td>16.5</td><td>20.4</td><td>35.2</td><td>3.8</td><td>1.9</td></tr><tr><td>9</td><td>50</td><td>15.9</td><td>5.8</td><td>6.8</td><td>9.8</td><td>11.4</td><td>16.5</td><td>20.4</td><td>35.2</td><td>3.8</td><td>3.8</td></tr><tr><td>10</td><td>25</td><td>31.8</td><td>5.8</td><td>6.8</td><td>9.8</td><td>11.4</td><td>16.5</td><td>20.4</td><td>35.2</td><td>3.8</td><td>7.5</td></tr><tr><td>11</td><td>12.5</td><td>31.8</td><td>5.8</td><td>6.8</td><td>9.8</td><td>11.4</td><td>16.5</td><td>20.4</td><td>35.2</td><td>3.8</td><td>7.5</td></tr></table>

# 5.5 ODR AND FSR SELECTION

Accelerometer ODR can be selected by programming the parameter ACCEL_ODR in register bank 0, register  $0\times 50h$  bits 3:0 as shown below. Refer to Note 6 under Table 1 for ACCEL_ODR recommended settings to achieve high 3dB value for accelerometer signal bandwidth.

<table><tr><td>ACCEL_ODR</td><td>ACCELEROMETER ODR VALUE</td></tr><tr><td>0000</td><td>Reserved</td></tr><tr><td>0001</td><td>32 kHz (LN mode)</td></tr><tr><td>0010</td><td>16 kHz (LN mode)</td></tr><tr><td>0011</td><td>8 kHz (LN mode)</td></tr><tr><td>0100</td><td>4 kHz (LN mode)</td></tr><tr><td>0101</td><td>2 kHz (LN mode)</td></tr><tr><td>0110</td><td>1 kHz (LN mode) (default)</td></tr><tr><td>0111</td><td>200 Hz (LP or LN mode)</td></tr><tr><td>1000</td><td>100 Hz (LP or LN mode)</td></tr><tr><td>1001</td><td>50 Hz (LP or LN mode)</td></tr><tr><td>1010</td><td>25 Hz (LP or LN mode)</td></tr><tr><td>1011</td><td>12.5 Hz (LP or LN mode)</td></tr><tr><td>1100</td><td>Reserved</td></tr><tr><td>1101</td><td>Reserved</td></tr><tr><td>1110</td><td>Reserved</td></tr><tr><td>1111</td><td>500Hz (LP or LN mode)</td></tr></table>

Accelerometer FSR can be selected by programming the parameter ACCEL_FS_SEL in register bank 0, register 0x50h, bits 7:5 as shown below.

<table><tr><td>ACCEL_FS_SEL</td><td>ACCELEROMETER FSR VALUE</td></tr><tr><td>000</td><td>16g</td></tr><tr><td>001</td><td>8g</td></tr><tr><td>010</td><td>4g</td></tr><tr><td>011</td><td>2g</td></tr><tr><td>100</td><td>Reserved</td></tr><tr><td>101</td><td>Reserved</td></tr><tr><td>110</td><td>Reserved</td></tr><tr><td>111</td><td>Reserved</td></tr></table>

# 6 FIFO

6 FIFOThe IIM- 42352 contains a 2 kB FIFO register that is accessible via the serial interface. The FIFO configuration register determines which data is written into the FIFO. Possible choices include accelerometer data, temperature readings, and FSYNC input. A FIFO counter keeps track of how many bytes of valid data are contained in the FIFO.

# 6.1 PACKET STRUCTURE

6.1 PACKET STRUCTUREThe following figure shows the 8- and 16- byte FIFO packet structures supported in IIM- 42352. Base data format for accelerometer is 16- bits per element. 20- bits data format support is included in one of the packet structures. When 20- bits data format is used, accelerometer data consists of 18- bits of actual data and the two lowest order bits are always set to 0. When 20- bits data format is used, the only FSR settings that are operational is  $\pm 16g$  for accelerometer, even if the FSR selection register settings are configured for other FSR values. The accelerometer sensitivity scale factor value is and 8192 LSB/g.

![](images/bb0190849561ab17a8012979007eb2a575570ed728778cd39596bada0f8abab1.jpg)  
Figure 10. FIFO Packet Structure

Due to limitation on the number of bits, 8- bit Temperature data stored in FIFO is limited to  $- 40^{\circ}C$  to  $85^{\circ}C$  range. Either 16- bit Temperature data format (FIFO packet 3) or the value from the sensor data registers (TEMP_DATA) must be used to support a temperature measurements range of  $- 40^{\circ}$  to  $105^{\circ}C$

The rest of this sub- section describes how individual data is packaged in the different FIFO packet structures.

Packet 1: Individual data is packaged in Packet 1 as shown below.

<table><tr><td>BYTE</td><td>CONTENT</td></tr></table>

<table><tr><td>0x00</td><td>FIFO Header</td></tr><tr><td>0x01</td><td>Accel X [15:8]</td></tr><tr><td>0x02</td><td>Accel X [7:0]</td></tr><tr><td>0x03</td><td>Accel Y [15:8]</td></tr><tr><td>0x04</td><td>Accel Y [7:0]</td></tr><tr><td>0x05</td><td>Accel Z [15:8]</td></tr><tr><td>0x06</td><td>Accel Z [7:0]</td></tr><tr><td>0x07</td><td>Temperature [7:0]</td></tr></table>

Packet 2: Individual data is packaged in Packet 2 as shown below.

Packet 3: Individual data is packaged in Packet 3 as shown below.  

<table><tr><td>BYTE</td><td>CONTENT</td></tr><tr><td>0x00</td><td>FIFO Header</td></tr><tr><td>0x01</td><td>Accel X [15:8]</td></tr><tr><td>0x02</td><td>Accel X [7:0]</td></tr><tr><td>0x03</td><td>Accel Y [15:8]</td></tr><tr><td>0x04</td><td>Accel Y [7:0]</td></tr><tr><td>Ox05</td><td>Accel Z [15:8]</td></tr><tr><td>Ox06</td><td>Accel Z [7:0]</td></tr><tr><td>Ox07</td><td>Reserved</td></tr><tr><td>Ox08</td><td>Reserved</td></tr><tr><td>Ox09</td><td>Reserved</td></tr><tr><td>Ox0A</td><td>Reserved</td></tr><tr><td>OxOB</td><td>Reserved</td></tr><tr><td>OxOC</td><td>Reserved</td></tr><tr><td>OxOD</td><td>Temperature[7:0]</td></tr><tr><td>OxOE</td><td>TimeStamp[15:8]</td></tr><tr><td>OxOF</td><td>TimeStamp[7:0]</td></tr></table>

<table><tr><td>BYTE</td><td>CONTENT</td><td></td></tr><tr><td>0x00</td><td>FIFO Header</td><td></td></tr><tr><td>0x01</td><td>Accel X [19:12]</td><td></td></tr><tr><td>0x02</td><td>Accel X [11:4]</td><td></td></tr><tr><td>0x03</td><td>Accel Y [19:12]</td><td></td></tr><tr><td>0x04</td><td>Accel Y [11:4]</td><td></td></tr><tr><td>0x05</td><td>Accel Z [19:12]</td><td></td></tr><tr><td>0x06</td><td>Accel Z [11:4]</td><td></td></tr><tr><td>0x07</td><td>Reserved</td><td></td></tr><tr><td>0x08</td><td>Reserved</td><td></td></tr><tr><td>0x09</td><td>Reserved</td><td></td></tr><tr><td>0x0A</td><td>Reserved</td><td></td></tr><tr><td>0x0B</td><td>Reserved</td><td></td></tr><tr><td>0x0C</td><td>Reserved</td><td></td></tr><tr><td>0x0D</td><td>Temperature[15:8]</td><td></td></tr><tr><td>0x0E</td><td>Temperature[7:0]</td><td></td></tr><tr><td>0x0F</td><td>TimeStamp[15:8]</td><td></td></tr><tr><td>0x10</td><td>TimeStamp[7:0]</td><td></td></tr><tr><td>0x11</td><td>Accel X [3:0]</td><td>Reserved</td></tr><tr><td>0x12</td><td>Accel Y [3:0]</td><td>Reserved</td></tr></table>

# 6.2 FIFOHEADER

The following table shows the structure of the 1 byte FIFO header.

<table><tr><td>BIT FIELD</td><td>ITEM</td><td>DESCRIPTION</td></tr><tr><td>7</td><td>HEADER_MSG</td><td>1: FIFO is empty
0: Packet contains sensor data</td></tr><tr><td>6:4</td><td>HEADER_PACKET_FORMAT</td><td>Oxx: Reserved
100: Packet contains packet 1 format (accel + temp)
101: Reserved
110: Packet contains packet 2 format (accel + temp + timestamp)
111: Packet contains packet 3 format (hires accel + temp + timestamp)
*See detail packet structure at section 6.1</td></tr><tr><td>3:2</td><td>HEADER_TIMESTAMP_FSYNC</td><td>00: Packet does not contain timestamp or FSYNC time data
01: Reserved
10: Packet contains ODR Timestamp
11: Packet contains FSYNC time, and this packet is flagged as first ODR after FSYNC (only if FIFO, TMST, FSYNC_EN is 1)</td></tr><tr><td>1</td><td>HEADER_ODR_ACCEL</td><td>1: The ODR for accel is different for this accel data packet compared to the previous accel packet
0: The ODR for accel is the same as the previous packet with accel</td></tr><tr><td>0</td><td>-</td><td>Reserved</td></tr></table>

Note that HEADER_ACCEL must be set for a sensor data packet to be set.

# 6.3 MAXIMUMFIFOSTORAGE

The maximum number of packets that can be stored in FIFO is a variable quantity depending on the use case. As shown in Figure 11, the physical FIFO size is 2048 bytes. A number of bytes equal to the packet size selected (see section 5.1) is reserved to prevent reading a packet during write operation. Additionally, a read cache 2 packets wide is available.

When there is no serial interface operation, the read cache is not available for storing packets, being fed by the serial interface clock.

When serial interface operation happens, depending on the operation length and the packet size chosen, either 1 or 2 of the packet entries in read cache may become available for storing packets. In that case the total storage available is up to the maximum number of packets that can be accommodated in 2048 bytes  $+1$  packet size, depending on the packet size used.

Due to the non- deterministic nature of system operation, driver memory allocation should always be the largest size of 2080 bytes.

![](images/69b037726b5506821f474b795151be0c1852d02b7d3c903fbffbbe4abf7bc718.jpg)

Reserved to prevent reading a packet during write operation

Figure 11. Maximum FIFO Storage

# 6.4 FIFO CONFIGURATION REGISTERS

The following control bits in bank 0, register 0x5Fh determine what data is placed into the FIFO. The values of these bits may change while the FIFO is being filled without corruption of the FIFO.

<table><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>4</td><td>FIFO_HIRES_EN</td><td>O: Default setting; Sensor data have regular resolution
1: Sensor data in FIFO will have extended resolution enabling the 20 Bytes packet</td></tr><tr><td>3</td><td>FIFO_TMST_FSYNC_EN</td><td>O: FIFO will only contain ODR timestamp information
1: FIFO can also contain FSYNC time and FSYNC tag for one ODR after an FSYNC event</td></tr><tr><td>1: 0</td><td>FIFO_PACKET_EN</td><td>00: Disable packet goes to FIFO
01: Enable Packet 1 goes to FIFO
10: Reserved
11: Enable Packet 2 goes to FIFO when FIFO_HIRES_EN=0
Enable Packet 3 goes to FIFO when FIFO_HIRES_EN=1</td></tr></table>

Configuration register settings above impact FIFO header and FIFO packet size as follows:

<table><tr><td>FIFO_HIRES_EN</td><td>FIFO_TMST_FSYNC_EN</td><td>FIFO_PACKET_EN</td><td>HEADER</td><td>PACKETSIZE</td></tr><tr><td>1</td><td>0</td><td>2&#x27;b 11</td><td>8&#x27;b 0111 10xx</td><td>20 Bytes</td></tr><tr><td>1</td><td>1</td><td>2&#x27;b 11</td><td>8&#x27;b 0111 11xx</td><td>20 Bytes</td></tr><tr><td>0</td><td>0</td><td>2&#x27;b 11</td><td>8&#x27;b 0110 10xx</td><td>16 Bytes</td></tr><tr><td>0</td><td>1</td><td>2&#x27;b 11</td><td>8&#x27;b 0110 11xx</td><td>11 Bytes</td></tr><tr><td>0</td><td>X</td><td>2&#x27;b 01</td><td>8&#x27;b 0100 00xx</td><td>8 Bytes</td></tr><tr><td>0</td><td>X</td><td>2&#x27;b 00</td><td>No FIFO writes</td><td>No FIFO writes</td></tr></table>

# 7 PROGRAMMABLEINTERRUPTS

The IIM- 42352 has a programmable interrupt system that can generate an interrupt signal on the INT pins. Status flags indicate the source of an interrupt. Interrupt sources may be enabled and disabled individually. There are two interrupt outputs. Any interrupt may be mapped to either interrupt pin as explained in the register section. The following configuration options are available for the interrupts

INT1 and INT2 can be push- pull or open drain Level or pulse mode Active high or active low

Additionally, IIM- 42352 includes In- band Interrupt (IBI) support for the  $13C^{SM}$  interface.

# 8 APEX MOTION FUNCTIONS

The APEX (Advanced Pedometer and Event Detection - neXt gen) features of IIM- 42352 consist of:

8 APEX MOTION FUNCTIONSThe APEX (Advanced Pedometer and Event Detection - neXt gen) features of IIM- 42352 consist of:- Pedometer: tracks step count and issues a step detect interrupt- Tilt Detection: issues an interrupt when the tilt angle exceeds 35 degrees for more than a programmable time.- Freefall Detection: triggers an interrupt when device freefall is detected and outputs freefall duration.- Tap Detection: issues an interrupt when tap is detected, along with a register containing the tap count.- Wake on Motion (WoM): detects motion when accelerometer samples exceed a programmable threshold. This motion event can be used to enable chip operation from sleep mode.- Significant Motion Detector (SMD): detects motion if WoM events are detected during a programmable time window (2s or 4s).

# 8.1 APEXODRSUPPORT

8.1 APEX ODR SUPPORTAPEX algorithms are designed to work with the accelerometer, for a variety of ODR settings. However, there is a minimum ODR required for each algorithm. The following table shows the relationship between the available accelerometer ODRs and the operation of the APEX algorithms. To allow more flexible operation where we can control the ODR of the APEX algorithms independent of the accelerometer ODR, we allow for an additional selection determined by the field DMP_ODR (DMP stands for Digital Motion Processor™, an architectural component of APEX). The tables below show how DMP_ODR should be configured in relation to the accelerometer ODR and the expected performance.

<table><tr><td>ACCEL ODR</td><td>DMP_ODR</td><td>PEDOMETER</td><td>TILT DETECTION</td><td>FREEFALL DETECTION</td></tr><tr><td>&amp;lt; 25Hz</td><td>X</td><td>Disabled</td><td>Disabled</td><td>Disabled</td></tr><tr><td>≥ 25Hz</td><td>0 (25Hz)</td><td>Low Power</td><td>Low Power</td><td>Low Power</td></tr><tr><td>≥ 50Hz</td><td>2 (50Hz)</td><td>Normal</td><td>Normal</td><td>Low Power</td></tr><tr><td>100Hz</td><td>3 (100Hz)</td><td>Normal (50Hz)</td><td>High Performance (50 Hz)</td><td>Normal</td></tr><tr><td>500Hz</td><td>1 (500Hz)</td><td>Disabled</td><td>High Performance (50 Hz)</td><td>High Performance</td></tr></table>

<table><tr><td>ACCEL ODR</td><td>TAP DETECTION</td></tr><tr><td>200 Hz</td><td>Low Power</td></tr><tr><td>500 Hz</td><td>Normal</td></tr><tr><td>1 kHz</td><td>High Performance</td></tr><tr><td>&amp;gt; 1 kHz</td><td>Disabled</td></tr></table>

If the accelerometer ODR is set below the minimum DMP ODR  $(25\text{Hz})$ , the APEX features cannot be enabled.

When the accelerometer ODR needs to be set differently from the DMP ODR, only the integer multiple of DMP ODR for accelerometer sensor ODR is suitable to use with DMP. For example, when the accelerometer ODR is set as  $200\text{Hz}$ , the APEX features can be enabled with choices of  $25\text{Hz}$ , or  $50\text{Hz}$ , depending on the DMP_ODR register setting.

DMP ODR should not be changed on the fly. The following sequence should be followed for changing the DMP ODR:

1. Disable Pedometer and Tilt Detection if they are enabled  
2. Change DMP_ODR  
3. Set DMP_INIT_EN for one cycle (Register 0x4Bh in Bank 0)  
4. Unset DMP_INIT_EN (Register 0x4Bh in Bank 0)  
5. Enable APEX features of interest

# 8.2 DMP POWER SAVE MODE

DMP Power Save Mode can be enabled or disabled by DMP_POWER_SAVE (Register 0x56h in Bank 0). When the DMP Power Save Mode is enabled, APEX features are enabled only when WOM is detected. WOM must be explicitly enabled for the DMP to work in this mode. When WOM is not detected the APEX features are on pause. If the user does not want to use DMP Power Save Mode they may set DMP_POWER_SAVE = 0, and use APEX functions without WOM detection.

# 8.3 PEDOMETER PROGRAMMING

- Pedometer configuration parameters

1. LOW_ENERGY_AMP_TH_SEL (Register 0x40h in Bank 4)  
2. PED_AMP_TH_SEL (Register 0x41h in Bank 4)  
3. PED_STEP_CNT_TH_SEL (Register 0x41h in Bank 4)  
4. PED_HI_EN_TH_SEL (Register 0x42h in Bank 4)  
5. PED_SB_TIMER_TH_SEL (Register 0x42h in Bank 4)  
6. PED_STEP_DET_TH_SEL (Register 0x42h in Bank 4)  
7. SENSITIVITY_MODE (Register 0x48h in Bank 4)  
8. There are 2 ODR and 2 sensitivity modes

<table><tr><td>ACCEL ODR (DMP_ODR)</td><td>NORMAL</td><td>SLOW WALK</td></tr><tr><td>25 Hz (0)</td><td>low power</td><td>low power and slow walk</td></tr><tr><td>50 Hz (2)</td><td>high performance</td><td>slow walk</td></tr></table>

- Initialize Sensor in a typical configuration

1. Set a accelerometer ODR to  $50\text{Hz}$  (Register 0x50h in Bank 0)  
2. Set accelerometer to Low Power mode (Register 0x4Eh in Bank 0)  ACCEL_MODE = 2 and (Register 0x4Eh in Bank 0), ACCEL_LP_CLK_SEL = 0, for low power mode  
3. Set DMP_ODR = 50 Hz and turn on Pedometer feature (Register 0x56h in Bank 0)  
4. Wait 1 millisecond

- Initialize APEX hardware

1. Set DMP_MEM_RESET_EN to 1 (Register 0x4Bh in Bank 0)  
2. Wait 1 millisecond  
3. Set LOW_ENERGY_AMP_TH_SEL to 10 (Register 0x40h in Bank 4)  
4. Set PED_AMP_TH_SEL to 8 (Register 0x41h in Bank 4)  
5. Set PED_STEP_CNT_TH_SEL to 5 (Register 0x41h in Bank 4)  
6. Set PED_HI_EN_TH_SEL to 1 (Register 0x42h in Bank 4)  
7. Set PED_SB_TIMER_TH_SEL to 4 (Register 0x42h in Bank 4)

8. Set PED_STEP_DET_TH_SEL to 2 (Register 0x42h in Bank 4)

9. Set SENSITIVITY_MODE to 0 (Register 0x48h in Bank 4)

10. Set DMP_INIT_EN to 1 (Register 0x4Bh in Bank 0)

11. Wait 50 milliseconds

12. Enable STEP detection, source for INT1 by setting bit 5 in register INT_SOURCE6 (Register 0x4Dh in Bank 4) to 1. Or if INT2 is selected for STEP detection, enable STEP detection source by setting bit 5 in register INT_SOURCE7 (Register 0x4Eh in Bank 4) to 1.

13. As freefall and pedometer share the same output register, they cannot run concurrently. Disable freefall by setting FF_ENABLE to 0 (Register 0x56h in Bank 0)

14. Turn on the pedometer feature by setting PED_ENABLE to 1 (Register 0x56h in Bank 0)

Output registers

1. Read interrupt register (Register 0x38h in Bank 0) for STEP_DET_INT

2. If the step count is equal to or greater than 65535 (uint16), the STEP_CNT_OVF_INT (Register 0x38h in Bank 0) will be set to 1. Example:

Take 1 step  $\Rightarrow$  output step count  $= 65533$  (real step count is 65533) Take 1 step  $\Rightarrow$  output step count  $= 65534$  (real step count is 65534) Take 1 step  $\Rightarrow$  output step count  $= 0$  and interrupt is fired (real step count is  $65535 + 0 =$  65535) Take 1 step  $\Rightarrow$  output step count  $= 1$  (real step count is  $65535 + 1 = 65536)$

3. Read the step count in STEP_CNT (Register 0x31h and 0x32h in Bank 0)

4. Read the step cadence in STEP_CADENCE (Register 0x33h in Bank 0)

5. Read the activity class in ACTIVITY_CLASS (Register 0x34h in Bank 0)

# 8.4 TILT DETECTION PROGRAMMING

Tilt Detection configuration parameters

1. TILT_WAIT_TIME (Register 0x43h in Bank 4) This parameter configures how long of a delay after tilt is detected before interrupt is triggered Default is 2 (4s). Range is  $0 = 0s$ $1 = 2s$ $2 = 4s$ $3 = 6s$  For example, setting TILT_WAIT_TIME  $= 2$  is equivalent to 4 seconds for all ODRs

Initialize Sensor in a typical configuration

1. Set accelerometer ODR (Register 0x50h in Bank 0) ACCEL_ODR  $= 9$  for  $50Hz$  or 10 for  $25Hz$  
2. Set Accel to Low Power mode (Register 0x4Eh in Bank 0) ACCEL_MODE  $= 2$  and (Register 0x4Dh in Bank 0), ACCEL_LP_CLK_SEL  $= 0$  for low power mode 
3. Set DMP ODR (Register 0x56h in Bank 0) DMP_ODR  $= 0$  for  $25Hz$  2 for  $50Hz$  
4. Wait 1 millisecond

Initialize APEX hardware

1. Set DMP_MEM_RESET_EN to 1 (Register 0x4Bh in Bank 0) 
2. Wait 1 millisecond 
3. Set TILT_WAIT_TIME (Register 0x43h in Bank 4) if default value does not meet needs 
4. Wait 1 millisecond 
5. Set DMP_INIT_EN to 1 (Register 0x4Bh in Bank 0) 
6. Enable Tilt Detection, source for INT1 by setting bit 3 in register INT_SOURCE6 (Register 0x4Dh in Bank 4) to 1. Or if INT2 is selected for Tilt Detection, enable Tilt Detection source by setting bit 3 in register INT_SOURCE7 (Register 0x4Eh in Bank 4) to 1 
7. Wait 50 milliseconds

8. Turn on Tilt Detection feature by setting TILT_ENABLE to 1 (Register 0x56h in Bank 0)

Output registers

1. Read interrupt register (Register 0x38h in Bank 0) for TILT_DET_INT

# 8.5 FREEFALL DETECTION PROGRAMMING

8.5 FREEFALL DETECTION PROGRAMMINGFreefall Detection detects device freefall. It uses a low- g and a high- g detector to detect freefall start and freefall end. It provides a trigger indicating freefall event and the freefall duration. The duration is given in number of samples and can be converted to freefall distance in meters by applying the following formula:

FF_DISTANCE  $= 0.5 * 9.81 * (FF\_ DUR * DMP\_ ODR\_ S)^{\wedge}2$

Note: DMP_ODR_S is the duration of DMP_ODR expressed in seconds.

- Freefall Detection configuration parameters

1. LOWG_PEAK_TH_SEL (Register 0x44h in Bank 4)  
2. LOWG_TIME_TH_SEL (Register 0x44h in Bank 4)  
3. LOWG_PEAK_TH_HYST_SEL (Register 0x43h in Bank 4)  
4. HIGHG_PEAK_TH_SEL (Register 0x45h in Bank 4)  
5. HIGHG_TIME_TH_SEL (Register 0x45h in Bank 4)  
6. HIGHG_PEAK_TH_HYST_SEL (Register 0x43h in Bank 4)  
7. FF_MIN_DURATION_CM (Register 0x49h in Bank 4)  
8. FF_MAX_DURATION_CM (Register 0x49h in Bank 4)  
9. FF_DEBOUNCE_DURATION (Register 0x49h in Bank 4)

Initialize Sensor in a typical configuration

1. Set Accel ODR to  $500\mathrm{Hz}$  (Register 0x50h in Bank 0)  
2. Set AVG filtering to 1 sample to minimize power consumption (Register 0x52h in Bank 0)  ACCEL_UI_FILT_BW = 1  
3. Set Accel to Low Power mode (Register 0x4E in Bank 0)  ACCEL_MODE[1:0] = 2  
4. Set DMP ODR = 500 Hz (Register 0x56 in Bank 0)  DMP_ODR[1:0] = 1

Initialize APEX hardware

1. Set LOWG_PEAK_TH_SEL (Register 0x44h in Bank 4)  
2. Set LOWG_TIME_TH_SEL (Register 0x44h in Bank 4)  
3. Set LOWG_PEAK_TH_HYST_SEL (Register 0x43 in Bank 4)  
4. Set HIGHG_PEAK_TH_SEL (Register 0x45h in Bank 4)  
5. Set HIGHG_TIME_TH_SEL (Register 0x45h in Bank 4)  
6. Set HIGHG_PEAK_TH_HYST_SEL (Register 0x43h in Bank 4)  
7. Set FF_DEBOUNCE_DURATION (Register 0x49h in Bank 4)  
8. Set FF_MIN_DURATION_CM (Register 0x49h in Bank 4)  
9. Set FF_MAX_DURATION_CM (Register 0x49h in Bank 4)  
10. Set DMP_MEM_RESET_EN to 1 if DMP is started for the first time after reset (Register 0x4Bh in Bank 0)  
11. Wait 1 millisecond  
12. Set DMP_INIT_EN to 1 (Register 0x4Bh in Bank 0)  
13. Enable FREEFALL detection, source for INT1 by setting bit 1 in register INT_SOURCE6 (Register 0x4Dh in Bank 4) to 1. Or if INT2 is selected for FREEFALL detection, enable FREEFALL detection source by setting bit 1 in register INT_SOURCE7 (Register 0x4Eh in Bank 4) to 1.  
14. Wait 50 milliseconds

15. As freefall and pedometer share the same output register, they cannot run concurrently. Disable pedometer by setting PED_ENABLE to 0 (Register 0x56h in Bank 0)  
16. Set FF_ENABLE to 1 (Register 0x56h in Bank 0)

Output registers

1. Read interrupt register (Register 0x38h in Bank 0) for FF_DET_INT  
2. Read the freefall duration (Registers 0x31h and 0x32h in Bank 0)

Note: As freefall and pedometer share the same output register, they cannot be run concurrently.

# 8.6 TAP DETECTION PROGRAMMING

Tap Detection configuration parameters

1. TAP_TMAX (Register 0x47h in Bank 4)  
2. TAP_TMIN (Register 0x47h in Bank 4)  
3. TAP_TAVG (Register 0x47h in Bank 4)  
4. TAP_MIN_JERK_THR (Register 0x46h in Bank 4)  
5. TAP_MAX_PEAK_TOL (Register 0x46h in Bank 4)  
6. TAP_ENABLE (Register 0x56h in Bank 0)

Initialize Sensor in a typical configuration

1. Set accelerometer ODR (Register 0x50h in Bank 0)  ACCEL_ODR = 15 for 500 Hz (ODR of 200 Hz or 1 kHz may also be used)  
2. Set power modes and filter configurations as shown below  - For ODR up to 500 Hz, set Accel to Low Power mode (Register 0x4Eh in Bank 0)  ACCEL_MODE = 2 and ACCEL_UP_CLK_SEL = 0, (Register 0x4Dh in Bank 0) for low power mode  - Set filter settings as follows: ACCEL_DEC2_M2_ORD = 2 (Register 0x53h in Bank 0);  ACCEL_UI_FILT_BW = 4 (Register 0x52h in Bank 0)  - For ODR of 1 kHz, set Accel to Low Noise mode (Register 0x4Eh in Bank 0)  ACCEL_MODE = 3  - Set filter settings as follows: ACCEL_UI_FILT_ORD = 2 (Register 0x53h in Bank 0);  ACCEL_UI_FILT_BW = 0 (Register 0x52h in Bank 0)

3. Wait 1 millisecond

Initialize APEX hardware

1. Set TAP_TMAX to 2 (Register 0x47h in Bank 4)  
2. Set TAP_TMIN to 3 (Register 0x47h in Bank 4)  
3. Set TAP_TAVG to 3 (Register 0x47h in Bank 4)  
4. Set TAP_MIN_JERK_THR to 17 (Register 0x46h in Bank 4)  
5. Set TAP_MAX_PEAK_TOL to 2 (Register 0x46h in Bank 4)  
6. Wait 1 millisecond  
7. Enable TAP source for INT1 by setting bit 0 in register INT_SOURCE6 (Register 0x4Dh in Bank 4) to 1. Or if INT2 is selected for TAP, enable TAP source by setting bit 0 in register INT_SOURCE7 (Register 0x4Eh in Bank 4) to 1.  
8. Wait 50 milliseconds  
9. Turn on TAP feature by setting TAP_ENABLE to 1 (Register 0x56h in Bank 0)

Output registers

1. Read interrupt register (Register 0x38h in Bank 0) for TAP_DET_INT  
2. Read the tap count in TAP_NUM (Register 0x7Bh in Bank 0)  
3. Read the tap axis in TAP_AXIS (Register 0x7Bh in Bank 0)  
4. Read the polarity of tap pulse in TAP_DIR (Register 0x7Bh in Bank 0)

# 8.7 WAKE ON MOTION PROGRAMMING

Wake on Motion configuration parameters

1. WOM_X_TH (Register 0x4Ah in Bank 4)  
2. WOM_Y_TH (Register 0x4Bh in Bank 4)  
3. WOM_Z_TH (Register 0x4Ch in Bank 4)  
4. WOM_INT_MODE (Register 0x57h in Bank 0)  
5. WOM_MODE (Register 0x57h in Bank 0)  
6. SIMD_MODE (Register 0x57h in Bank 0)

Initialize Sensor in a typical configuration

1. Set accelerometer ODR (Register 0x50h in Bank 0)  ACCEL_ODR = 9 for 50 Hz  
2. Set Accel to Low Power mode (Register 0x4Eh in Bank 0)  ACCEL_MODE = 2 and (Register 0x4Dh in Bank 0), ACCEL_LP_CLK_SEL = 0, for low power mode  
3. Wait 1 millisecond

Initialize APEX hardware

1. Set WOM_X_TH to 98 (Register 0x4Ah in Bank 4)  
2. Set WOM_Y_TH to 98 (Register 0x4Bh in Bank 4)  
3. Set WOM_Z_TH to 98 (Register 0x4Ch in Bank 4)  
4. Wait 1 millisecond  
5. Enable all 3 axes as WOM sources for INT1 by setting bits 2:0 in register INT_SOURCE1 (Register 0x66h in Bank 0) to 1. Or if INT2 is selected for WOM, enable all 3 axes as WOM sources by setting bits 2:0 in register INT_SOURCE4 (Register 0x69h in Bank 0) to 1.  
6. Wait 50 milliseconds  
7. Turn on WOM feature by setting WOM_INT_MODE to 0, WOM_MODE to 1, SIMD_MODE to 1 (Register 0x56h in Bank 0)

Output registers

1. Read interrupt register (Register 0x37h in Bank 0) for WOM_X_INT  
2. Read interrupt register (Register 0x37h in Bank 0) for WOM_Y_INT  
3. Read interrupt register (Register 0x37h in Bank 0) for WOM_Z_INT

# 8.8 SIGNIFICANT MOTION DETECTION PROGRAMMING

Significant Motion Detection configuration parameters

1. WOM_X_TH (Register 0x4Ah in Bank 4)  
2. WOM_Y_TH (Register 0x4Bh in Bank 4)  
3. WOM_Z_TH (Register 0x4Ch in Bank 4)  
4. WOM_INT_MODE (Register 0x57h in Bank 0)  
5. WOM_MODE (Register 0x57h in Bank 0)  
6. SIMD_MODE (Register 0x57h in Bank 0)

Initialize Sensor in a typical configuration

1. Set accelerometer ODR (Register 0x50h in Bank 0)  ACCEL_ODR = 9 for 50 Hz  
2. Set Accel to Low Power mode (Register 0x4Eh in Bank 0)  ACCEL_MODE = 2 and (Register 0x4Dh in Bank 0), ACCEL_LP_CLK_SEL = 0, for low power mode  
3. Wait 2 millisecond

Initialize APEX hardware

1. Set WOM_X_TH to 98 (Register 0x4Ah in Bank 4)  
2. Set WOM_Y_TH to 98 (Register 0x4Bh in Bank 4)

3. Set WOM_Z_TH to 98 (Register 0x4Ch in Bank 4)  
4. Wait 1 millisecond  
5. Enable SMD source for INT1 by setting bit 3 in register INT_SOURCE1 (Register 0x66h in Bank 0) to 1. Or if INT2 is selected for SMD, enable SMD source by setting bit 3 in register INT_SOURCE4 (Register 0x69h in Bank 0) to 1.  
6. Wait 50 milliseconds  
7. Turn on SMD feature by setting WOM_INT_MODE to 0, WOM_MODE to 1, SMD_MODE to 3 (Register 0x56h in Bank 0)

Output registers

1. Read interrupt register (Register 0x37h in Bank 0) for SMD_INT

# 9 DIGITAL INTERFACE

# 9.1  $13C^{SM}$ ,  $1^{2}C$ , AND SPI SERIAL INTERFACES

The internal registers and memory of the IIM- 42352 can be accessed using  $13C^{SM}$  at 12.5 MHz (data rates up to 12.5 Mbps in SDR mode, 25 Mbps in DDR mode),  $1^{2}C$  at 1 MHz or SPI at 24 MHz. SPI operates in 3- wire or 4- wire mode. Pin assignments for serial interfaces are described in Section 3.1.

# 9.2  $13C^{SM}$  INTERFACE

$13C^{SM}$  is a new 2- wire digital interface comprised of the signals serial data (SDA) and serial clock (SCLK).  $13C^{SM}$  is intended to improve upon the  $1^{2}C$  interface, while preserving backward compatibility.

$13C^{SM}$  carries the advantages of  $1^{2}C$  in simplicity, low pin count, easy board design, and multi- drop (vs. point to point), but provides the higher data rates, simpler pads, and lower power of SPI.  $13C^{SM}$  adds higher throughput for a given frequency, in- band interrupts (from slave to master), dynamic addressing.

IIM- 42352 supports the following features of  $13C^{SM}$  ..

SDR data rate up to 12.5 Mbps DDR data rate up to 25 Mbps Dynamic address allocation In- band Interrupt (IBI) support Support for asynchronous timing control mode 0 Error detection (CRC and/or Parity) Common Command Code (CCC)

The IIM- 42352 always operates as an  $13C^{SM}$  slave device when communicating to the system processor, which thus acts as the  $13C^{SM}$  master.  $13C^{SM}$  master controls an active pull- up resistance on SDA, which it can enable and disable. The pull- up resistance may be a board level resistor controlled by a pin, or it may be internal to the  $13C^{SM}$  master.

# 9.3  $1^{2}C$  INTERFACE

$1^{2}C$  is a two- wire interface comprised of the signals serial data (SDA) and serial clock (SCL). In general, the lines are open- drain and bi- directional. In a generalized  $1^{2}C$  interface implementation, attached devices can be a master or a slave. The master device puts the slave address on the bus, and the slave device with the matching address acknowledges the master.

The IIM- 42352 always operates as a slave device when communicating to the system processor, which thus acts as the master. SDA and SCL lines typically need pull- up resistors to VDDIO. The maximum bus speed is 1 MHz.

The slave address of the IIM- 42352 is b110100X, which is 7 bits long. The LSB bit of the 7- bit address is determined by the logic level on pin AP_ADO. This allows two IIM- 42352s to be connected to the same  $1^{2}C$  bus. When used in this configuration, the address of one of the devices should be b1101000 (pin AP_ADO is logic low) and the address of the other should be b1101001 (pin AP_ADO is logic high).

# 9.4  $1^{2}C$  COMMUNICATIONS PROTOCOL

START (S) and STOP (P) Conditions

Communication on the  $1^{2}C$  bus starts when the master puts the START condition (S) on the bus, which is defined as a HIGH- to- LOW transition of the SDA line while SCL line is HIGH (see figure below). The bus is considered to be busy until the master puts a STOP condition (P) on the bus, which is defined as a LOW to HIGH transition on the SDA line while SCL is HIGH (see figure below).

Additionally, the bus remains busy if a repeated START (Sr) is generated instead of a STOP condition.

![](images/82f17bc074303c5d66bc15f69dd2baec26bac3af96b14782297e0e2f5d850985.jpg)  
Figure 12. START and STOP Conditions

# Data Format/Acknowledge

Data Format/Acknowledge $\mathsf{I}^2\mathsf{C}$  data bytes are defined to be 8- bits long. There is no restriction to the number of bytes transmitted per data transfer. Each byte transferred must be followed by an acknowledge (ACK) signal. The clock for the acknowledge signal is generated by the master, while the receiver generates the actual acknowledge signal by pulling down SDA and holding it low during the HIGH portion of the acknowledge clock pulse.

# Communications

CommunicationsAfter beginning communications with the START condition (S), the master sends a 7- bit slave address followed by an  $8^{\text{th}}$  bit, the read/write bit. The read/write bit indicates whether the master is receiving data from or is writing to the slave device. Then, the master releases the SDA line and waits for the acknowledge signal (ACK) from the slave device. Each byte transferred must be followed by an acknowledge bit. To acknowledge, the slave device pulls the SDA line LOW and keeps it LOW for the high period of the SCL line. Data transmission is always terminated by the master with a STOP condition (P), thus freeing the communications line. However, the master can generate a repeated START condition (Sr), and address another slave without first generating a STOP condition (P). A LOW to HIGH transition on the SDA line while SCL is HIGH defines the stop condition. All SDA changes should take place when SCL is low, with the exception of start and stop conditions.

![](images/8823528e7fc7123f1e19da3d573d365d56da621dd310e305eb0c068a060ccda5.jpg)  
Figure 13. Complete  $\mathsf{I}^2\mathsf{C}$  Data Transfer

Figure 13. Complete  $\mathsf{I}^2\mathsf{C}$  Data TransferTo write the internal IIM- 42352 registers, the master transmits the start condition (S), followed by the  $\mathsf{I}^2\mathsf{C}$  address and the write bit (0). At the  $9^{\text{th}}$  clock cycle (when the clock is high), the IIM- 42352 acknowledges the transfer. Then the master puts the register address (RA) on the bus. After the IIM- 42352 acknowledges the reception of the register address, the master puts the register data onto the bus. This is followed by the ACK signal, and data transfer may be concluded by the stop condition (P). To write multiple bytes after the last ACK signal, the master can continue outputting data rather than transmitting a stop signal. In this case, the IIM- 42352 automatically increments the register address and loads the data to the appropriate register. The following figures show single and two- byte write sequences.

# Single-Byte Write Sequence

<table><tr><td>Master</td><td>S</td><td>AD+W</td><td></td><td>RA</td><td></td><td>DATA</td><td></td><td>P</td></tr><tr><td>Slave</td><td></td><td></td><td>ACK</td><td></td><td>ACK</td><td></td><td>ACK</td><td></td></tr></table>

# Burst Write Sequence

<table><tr><td>Master</td><td>S</td><td>AD+W</td><td></td><td>RA</td><td></td><td>DATA</td><td></td><td>DATA</td><td></td><td>P</td></tr><tr><td>Slave</td><td></td><td></td><td>ACK</td><td></td><td>ACK</td><td></td><td>ACK</td><td></td><td>ACK</td><td></td></tr></table>

To read the internal IIM- 42352 registers, the master sends a start condition, followed by the  $1^{\circ}C$  address and a write bit, and then the register address that is going to be read. Upon receiving the ACK signal from the IIM- 42352, the master transmits a start signal followed by the slave address and read bit. As a result, the IIM- 42352 sends an ACK signal and the data. The communication ends with a not acknowledge (NACK) signal and a stop bit from master. The NACK condition is defined such that the SDA line remains high at the  $9^{\text{th}}$  clock cycle. The following figures show single- and two- byte read sequences.

# Single-Byte Read Sequence

<table><tr><td>Master</td><td>S</td><td>AD+W</td><td></td><td>RA</td><td></td><td>S</td><td>AD+R</td><td></td><td></td><td>NACK</td><td>P</td></tr><tr><td>Slave</td><td></td><td></td><td>ACK</td><td></td><td>ACK</td><td></td><td></td><td>ACK</td><td>DATA</td><td></td><td></td></tr></table>

# Burst Read Sequence

<table><tr><td>Master</td><td>S</td><td>AD+W</td><td></td><td>RA</td><td></td><td>S</td><td>AD+R</td><td></td><td></td><td>ACK</td><td></td><td>NACK</td><td>P</td></tr><tr><td>Slave</td><td></td><td></td><td>ACK</td><td></td><td>ACK</td><td></td><td></td><td>ACK</td><td>DATA</td><td></td><td>DATA</td><td></td><td></td></tr></table>

# 9.5 I2C TERMS

Table 13.I2C Terms  

<table><tr><td>SIGNAL</td><td>DESCRIPTION</td></tr><tr><td>S</td><td>Start Condition: SDA goes from high to low while SCL is high</td></tr><tr><td>AD</td><td>Slave I2C address</td></tr><tr><td>W</td><td>Write bit (0)</td></tr><tr><td>R</td><td>Read bit (1)</td></tr><tr><td>ACK</td><td>Acknowledge: SDA line is low while the SCL line is high at the 9th clock cycle</td></tr><tr><td>NACK</td><td>Not-Acknowledge: SDA line stays high at the 9th clock cycle</td></tr><tr><td>RA</td><td>IIM-42352 internal register address</td></tr><tr><td>DATA</td><td>Transmit or received data</td></tr><tr><td>P</td><td>Stop condition: SDA going from low to high while SCL is high</td></tr></table>

# 9.6 SPI INTERFACE

The IIM- 42352 supports 3- wire or 4- wire SPI for the host interface. The IIM- 42352 always operates as a Slave device during standard Master- Slave SPI operation.

With respect to the Master, the Serial Clock output (SCLK), the Serial Data Output (SDO), the Serial Data Input (SDI), and the Serial Data IO (SDIO) are shared among the Slave devices. Each SPI slave device requires its own Chip Select (CS) line from the master.

CS goes low (active) at the start of transmission and goes back high (inactive) at the end. Only one CS line is active at a time, ensuring that only one slave is selected at any given time. The CS lines of the non- selected slave devices are held high, causing their SDO lines to remain in a high- impedance (high- z) state so that they do not interfere with any active devices.

# SPI Operational Features

1. Data is delivered MSB first and LSB last 
2. Data is latched on the rising edge of SCLK 
3. Data should be transitioned on the falling edge of SCLK 
4. The maximum frequency of SCLK is 24 MHz

5. SPI read operations are completed in 16 or more clock cycles (two or more bytes). The first byte contains the SPI Address, and the following byte(s) contain(s) the SPI data. The first bit of the first byte contains the Read/Write bit and indicates the Read (1) operation. The following 7 bits contain the Register Address. In cases of multiple-byte Reads, data is two or more bytes:

SPI Address format  

<table><tr><td>MSB</td><td></td><td></td><td></td><td></td><td></td><td></td><td>LSB</td></tr><tr><td>R/W</td><td>A6</td><td>A5</td><td>A4</td><td>A3</td><td>A2</td><td>A1</td><td>A0</td></tr></table>

SPI Data format  

<table><tr><td>MSB</td><td></td><td></td><td></td><td></td><td></td><td></td><td>LSB</td></tr><tr><td>D7</td><td>D6</td><td>D5</td><td>D4</td><td>D3</td><td>D2</td><td>D1</td><td>D0</td></tr></table>

6. SPI write operations are completed in 16 clock cycles (two bytes). The first byte contains the SPI Address, and the second byte contains the SPI data. The first bit of the first byte contains the Read/Write bit and indicates the Write (0) operation. The following 7 bits contain the Register Address.

7. Supports Single or Burst Reads and Single Writes.

![](images/27b055a99fd09cee3f6329f5055804216e2b8bdbe4c5d60e8c750175c2904055.jpg)  
Figure 14. Typical SPI Master/Slave Configuration

# 10 ASSEMBLY

10 ASSEMBLYThis section provides general guidelines for assembling InvenSense Micro Electro- Mechanical Systems (MEMS) accelerometer packaged in LGA package.

# 10.1 ORIENTATION OF AXES

The diagram below shows the orientation of the axes of sensitivity and the polarity of rotation. Note the pin 1 identifier  $(\bullet)$  in the figure.

![](images/3a603ee0755abf31bad1b50388157d6eea76300abaeb513163f87b19171f2aff.jpg)  
Figure 15. Orientation of Axes of Sensitivity and Polarity of Rotation

# 10.2 PACKAGE DIMENSIONS

14 Lead LGA (2.5x3x0.91) mm NiAu pad finish

![](images/36fbb3e52872d6763d319b82dffa99064b712b58c6fb5e9f9c3eaa5dcdd9ca18.jpg)  
Figure 16. Package Dimensions

Table 14.Package Dimensions  

<table><tr><td></td><td></td><td colspan="3">DIMENSIONS IN MILLIETERS</td></tr><tr><td></td><td>SYMBOLS</td><td>MIN</td><td>NOM</td><td>MAX</td></tr><tr><td>Total Thickness</td><td>A</td><td>0.85</td><td>0.91</td><td>0.97</td></tr><tr><td>Substrate Thickness</td><td>A1</td><td></td><td>0.105</td><td>REF</td></tr><tr><td>Mold Thickness</td><td>A2</td><td></td><td>0.8</td><td>REF</td></tr><tr><td rowspan="2">Body Size</td><td>D</td><td></td><td>2.5</td><td>BSC</td></tr><tr><td>E</td><td></td><td>3</td><td>BSC</td></tr><tr><td>Lead Width</td><td>W</td><td>0.2</td><td>0.25</td><td>0.3</td></tr><tr><td>Lead Length</td><td>L</td><td>0.425</td><td>0.475</td><td>0.525</td></tr><tr><td>Lead Pitch</td><td>e</td><td></td><td>0.5</td><td>BSC</td></tr><tr><td>Lead Count</td><td>n</td><td colspan="3">14</td></tr><tr><td rowspan="2">Edge Pin Center to Center</td><td>D1</td><td>1.5</td><td>BSC</td><td></td></tr><tr><td>E1</td><td>1</td><td>BSC</td><td></td></tr><tr><td>Body Center to Contact Pin</td><td>SD</td><td>0.25</td><td>BSC</td><td></td></tr><tr><td>Package Edge Tolerance</td><td>aaa</td><td>0.1</td><td></td><td></td></tr><tr><td>Mold Flatness</td><td>bbb</td><td>0.2</td><td></td><td></td></tr><tr><td>Coplanarity</td><td>ddd</td><td>0.08</td><td></td><td></td></tr></table>

# 11 PART NUMBER PACKAGE MARKING

The part number package marking for IIM- 42352 devices is summarized below:

Table 15.Part Number Package Marking  

<table><tr><td>PART NUMBER</td><td>PART NUMBER PACKAGE MARKING</td></tr><tr><td>IIM-42352</td><td>I4352</td></tr></table>

![](images/24eb8c98703e5606374c82d94a1a7fda75ddb0938d661d839e1cb9ddad6d2509.jpg)  
Figure 17.Part Number Package Marking

# 12 USE NOTES

# 12.1 ACCELEROMETER MODE TRANSITIONS

When transitioning from accelerometer Low Power (LP) mode to accelerometer Low Noise (LN) mode, if ODR is  $6.25\mathrm{Hz}$  or lower, software should change ODR to a value of  $12.5\mathrm{Hz}$  or higher, because accelerometer LN mode does not support ODR values below  $12.5\mathrm{Hz}$

When transitioning from accelerometer LN mode to accelerometer LP mode, if ODR is greater than  $500\mathrm{Hz}$  software should change ODR to a value of  $500\mathrm{Hz}$  or lower, because accelerometer LP mode does not support ODR values above  $500\mathrm{Hz}$

# 12.2 ACCELEROMETER LOW POWER (LP) MODE AVERAGING FILTER SETTING

Software drivers provided with the device use Averaging Filter setting of 16x. This setting is recommended for meeting Android noise requirements in LP mode, and to minimize accelerometer offset variation when transitioning from LP to Low Noise (LN) mode. 1x averaging filter can be used by following the setting configuration shown in section 14.39.

# 12.3 SETTINGS FOR  $1^{2}C_{i}$ $13C^{\mathrm{SM}}$  AND SPI OPERATION

Upon bootup the device comes up in SPI mode. The following settings should be used for  $1^{2}C_{i}$ $13C^{\mathrm{SM}}$  , and SPI operation.

Scenario 1: INT1/INT2 pins are used for interrupt assertion in  $13C^{\mathrm{SM}}$  mode.

<table><tr><td>REGISTER FIELD</td><td>I2C DRIVER SETTING</td><td>I3C*DRIVER SETTING</td><td>SPI DRIVER SETTING</td></tr><tr><td>I3C_EN (bit 4, register INTF_CONFIG6, address 0x7C, bank 1)</td><td>1</td><td>1</td><td>1</td></tr><tr><td>I3C_SDR_EN (bit 0, register INTF_CONFIG6, address 0x7C, bank 1)</td><td>0</td><td>1</td><td>1</td></tr><tr><td>I3C_DDR_EN (bit 1, register INTF_CONFIG6, address 0x7C, bank 1)</td><td>0</td><td>0</td><td>1</td></tr><tr><td>I3C_BUS_MODE (bit 6, register INTF_CONFIG4, address 0x7A, bank 1)</td><td>0</td><td>0</td><td>0</td></tr><tr><td>I2C_SLEW_RATE (bits 5:3, register DRIVE_CONFIG, address 0x13, bank 0)</td><td>1</td><td>0</td><td>0</td></tr><tr><td>SPI_SLEW_RATE (bits 2:0, register DRIVE_CONFIG, address 0x13, bank 0)</td><td>1</td><td>3</td><td>5</td></tr></table>

Scenario 2: IBl is used for interrupt assertion in  $13C^{\mathrm{SM}}$  mode.

<table><tr><td>REGISTER FIELD</td><td>I2C DRIVER SETTING</td><td>I3C*DRIVER SETTING</td><td>SPI DRIVER SETTING</td></tr><tr><td>I3C_EN (bit 4, register INTF_CONFIG6, address 0x7C, bank 1)</td><td>1</td><td>1</td><td>1</td></tr><tr><td>I3C_SDR_EN (bit 0, register INTF_CONFIG6 address 0x7C, bank 1)</td><td>0</td><td>1</td><td>1</td></tr><tr><td>I3C_DDR_EN (bit 1, register INTF_CONFIG6, address 0x7C, bank 1)</td><td>0</td><td>1</td><td>1</td></tr><tr><td>I3C_BUS_MODE (bit 6, register INTF_CONFIG4, address 0x7A, bank 1)</td><td>0</td><td>0</td><td>0</td></tr><tr><td>I2C_SLEW_RATE (bits 5:3, register DRIVE_CONFIG, address 0x13, bank 0)</td><td>1</td><td>0</td><td>0</td></tr><tr><td>SP1_SLEW_RATE (bits 2:0, register DRIVE_CONFIG, address 0x13, bank 0)</td><td>1</td><td>5</td><td>5</td></tr></table>

# 12.4 ANTI-ALIAS FILTER OPERATION

Use of Anti- Alias Filter is supported only for Low Noise (LN) mode operation. The host is responsible for keeping the UI path in LN mode while Anti- Alias Filter are turned on.

# 12.5 EXTERNAL CLOCK INPUT EFFECT ON ODR

12.5 EXTERNAL CLOCK INPUT EFFECT ON ODRODR values supported by the device scale with external clock frequency, if external clock input is used. The ODR values shown in the datasheet are supported with external clock input frequency of  $32\mathrm{kHz}$ . For any other external clock input frequency, these ODR values will scale by a factor of (External clock value in kHz/32). For example, if an external clock frequency of  $32.768\mathrm{kHz}$  is used, instead of ODR value of  $500\mathrm{Hz}$ , it will be  $500 * (32.768 / 32) = 512\mathrm{Hz}$ .

# 12.6 INT_ASYNC_RESET CONFIGURATION

For register INT_CONFIG1 (bank 0 register  $0\times 64$  ) bit 4 INT_ASYNC_RESET, user should change setting to 0 from default setting of 1, for proper INT1 and INT2 pin operation.

# 12.7 FIFO TIMESTAMP INTERVAL SCALING

When RTC_MODE  $= 1$  (bank 0 register  $0\times 40$  bit2) and register INTF_CONFIG5 (bank 1 register  $0\times 78$  ) bit 2:1 (PIN9_FUNCTION) is set to 10 for CLKIN input;

# THEN

THENTimestamp interval reported in FIFO requires scaling by a factor of  $32.768 / 30$ . For example, when  $\mathsf{ODR} = 1\mathrm{kHz}$ , the true timestamp interval should be  $1000\mu s$ . But the value in FIFO toggles between 915 and  $916\mu s$ . After scaling  $915.5 * 32.768 / 30 = 1000\mu s$ .

# ELSE

ELSETimestamp interval reported in FIFO requires scaling by a factor of  $32 / 30$ . For example when  $\mathsf{ODR} = 1\mathrm{kHz}$ , the true timestamp interval should be  $1000\mu s$ . But the value in FIFO toggles between 937 and  $938\mu s$ . After scaling  $937.5 * 32 / 30 = 1000\mu s$ .

# 12.8 SUPPLEMENTARY INFORMATION FOR FIFO_HOLD_LAST_DATA_EN

This section contains supplementary information for using register field FIFO_HOLD_LAST_DATA_EN (bit 7) of register INTF_CONFIGO (address  $0\times 4C$  bank O).

The following table shows the values in FIFO:

<table><tr><td colspan="2">FIFO_HOLD_LAST_DATA_EN</td><td>16-BIT FIFO PACKET</td><td>20-BIT FIFO PACKET</td></tr><tr><td rowspan="2">0 (Insert Invalid code)</td><td>Valid sample</td><td>All values in: {-32766 to +32767}</td><td>Every Other Even number in {-524256 to +524284}
Example: {-524256, -524252, -524248, -524244 ....+524280, +524284}</td></tr><tr><td>Invalid sample</td><td>-32768</td><td>-524288</td></tr><tr><td rowspan="2">1 (“copy last valid” mode: 
No invalid code insertion)</td><td>Valid sample</td><td>All values in: {-32768 to +32767}</td><td>Every Other Even number in {-524288 to +524284}
Example: {-524288, -524284, -524280, ....+524280, +524284}</td></tr><tr><td>Invalid sample</td><td colspan="2">Copy last valid sample</td></tr></table>

The following table shows the values in sense registers on reset:

<table><tr><td></td><td>FIFO_HOLD_LAST_DATA_EN = 0</td><td>FIFO_HOLD_LAST_DATA_EN = 1</td></tr><tr><td>POWER ON RESET
TILL FIRST SAMPLE</td><td>Accel /Temperature Sensor = -32768</td><td>Accel/Temperature Sensor = 0</td></tr></table>

The following table shows the values in sense registers after first sample is received. As shown in table, the combination of FIFO_HOLD_LAST_DATA_EN and FSYNC Tag determine the range of values read for valid samples and invalid samples.

<table><tr><td rowspan="2" colspan="2">FIFO_HOLD_LAST_DATA_EN</td><td rowspan="2">FSYNC TAG DISABLED</td><td colspan="3">FSYNC ENABLED ON ONE SENSOR</td></tr><tr><td colspan="2">SENSOR SELECTED FOR FSYNC TAG</td><td>OTHER SENSOR NOT SELECTED FOR FSYNC TAGGING</td></tr><tr><td colspan="2"></td><td></td><td>FSYNC TAGGED</td><td>FSYNC NOT TAGGED</td><td></td></tr><tr><td rowspan="2">0 (Insert Invalid code)</td><td>Valid sample</td><td>All values in: 
{-32766 to +32767}</td><td>All ODD values in: 
{-32766 to +32767}</td><td>All EVEN values in: 
{-32766 to +32766}</td><td>All values in: 
{-32766 to +32767}</td></tr><tr><td>Invalid sample</td><td colspan="4">Registers do not receive invalid samples. Registers hold last valid sample until new one arrives</td></tr><tr><td rowspan="2">1 (“copy last valid” mode: No invalid code insertion)</td><td>Valid sample</td><td>All values in: 
{-32768 to +32767}</td><td>All ODD values in: 
{-32767 to +32767}</td><td>All EVEN values in: 
{-32768 to +32766}</td><td>All values in: 
{-32768 to +32767}</td></tr><tr><td>Invalid sample</td><td colspan="4">Registers do not receive invalid samples. Registers hold last valid sample until new one arrives</td></tr></table>

# 13 REGISTER MAP

This section lists the register map for the IIM- 42352, for user banks 0, 1, 2, 4.

# 13.1 USER BANK 0 REGISTER MAP

<table><tr><td>Addr (Hex)</td><td>Addr (Dec.)</td><td>Register Name</td><td>Serial I/F</td><td>Bit7</td><td>Bit6</td><td>Bit5</td><td>Bit4</td><td>Bit3</td><td>Bit2</td><td>Bit1</td><td>Bit0</td></tr><tr><td>11</td><td>17</td><td>DEVICE_CONFIG</td><td>R/W</td><td colspan="3">-</td><td>SPI_MODE</td><td colspan="3">-</td><td>SOFRETS_CONFIG</td></tr><tr><td>13</td><td>19</td><td>DRIVE_CONFIG</td><td>R/W</td><td colspan="2">-</td><td colspan="3">I2C_SLEW_RATE</td><td colspan="3">SPI_SLEW_RATE</td></tr><tr><td>14</td><td>20</td><td>INT_CONFIG</td><td>R/W</td><td colspan="2">-</td><td>INT2_MODE</td><td>INT2_DRIVER_CIRCUIT</td><td>INT2_POLARITY</td><td>INT1_MODE</td><td>INT1_DRIVER_CIRCUIT</td><td>INT1_POLARITY</td></tr><tr><td>16</td><td>22</td><td>FIFO_CONFIG</td><td>R/W</td><td colspan="2">FIFO_MODE</td><td colspan="6">-</td></tr><tr><td>1D</td><td>29</td><td>TEMP_DATA1</td><td>SYNCR</td><td colspan="8">TEMP_DATA[15:8]</td></tr><tr><td>1E</td><td>30</td><td>TEMP_DATA0</td><td>SYNCR</td><td colspan="8">TEMP_DATA[7:0]</td></tr><tr><td>1F</td><td>31</td><td>ACCEL_DATA_X1</td><td>SYNCR</td><td colspan="8">ACCEL_DATA_X[15:8]</td></tr><tr><td>20</td><td>32</td><td>ACCEL_DATA_X0</td><td>SYNCR</td><td colspan="8">ACCEL_DATA_X[7:0]</td></tr><tr><td>21</td><td>33</td><td>ACCEL_DATA_Y1</td><td>SYNCR</td><td colspan="8">ACCEL_DATA_Y[15:8]</td></tr><tr><td>22</td><td>34</td><td>ACCEL_DATA_Y0</td><td>SYNCR</td><td colspan="8">ACCEL_DATA_Y[7:0]</td></tr><tr><td>23</td><td>35</td><td>ACCEL_DATA_Z1</td><td>SYNCR</td><td colspan="8">ACCEL_DATA_Z[15:8]</td></tr><tr><td>24</td><td>36</td><td>ACCEL_DATA_Z0</td><td>SYNCR</td><td colspan="8">ACCEL_DATA_Z[7:0]</td></tr><tr><td>2B</td><td>43</td><td>TMST_FSYNCH</td><td>SYNCR</td><td colspan="8">TMST_FSYNCH_DATA[15:8]</td></tr><tr><td>2C</td><td>44</td><td>TMST_FSYNCL</td><td>SYNCR</td><td colspan="8">TMST_FSYNCL_DATA[7:0]</td></tr><tr><td>2D</td><td>45</td><td>INT_COUNT</td><td>R/C</td><td>-</td><td>UI_FSYNCH_T</td><td>-</td><td>RESET_DONE_INT</td><td>DATA_RDY_I</td><td>FIFO_DIS_IN</td><td>FIFO_FULL_I</td><td>AGC_RDY_IN</td></tr><tr><td>2E</td><td>46</td><td>FIFO_COUNT</td><td>R</td><td colspan="8">FIFO_COUNT[15:8]</td></tr><tr><td>2F</td><td>47</td><td>FIFO_COUNT</td><td>R</td><td colspan="8">FIFO_COUNT[7:0]</td></tr><tr><td>30</td><td>48</td><td>FIFO_DATA</td><td>R</td><td colspan="8">FIFO_DATA</td></tr><tr><td>31</td><td>49</td><td>APEX_DATA0</td><td>SYNCR</td><td colspan="8">STEP_CNT[7:0] / FF_DUR[7:0]</td></tr><tr><td>32</td><td>50</td><td>APEX_DATA1</td><td>SYNCR</td><td colspan="8">STEP_CNT[15:8] / FF_DUR[15:8]</td></tr><tr><td>33</td><td>51</td><td>APEX_DATA2</td><td>R</td><td colspan="8">STEP_CADENCE</td></tr><tr><td>34</td><td>52</td><td>APEX_DATA3</td><td>R</td><td colspan="5">-</td><td>DMP_IDLE</td><td colspan="2">ACTIVITY_CLASS</td></tr><tr><td>35</td><td>53</td><td>APEX_DATA4</td><td>R</td><td colspan="3">-</td><td>TAP_NUM</td><td>TAP_AXIS</td><td>TAP_AXIS</td><td colspan="2">TAP_DIR</td></tr><tr><td>36</td><td>54</td><td>APEX_DATA5</td><td>R</td><td colspan="3">-</td><td colspan="5">DOUBLE_TAPTiming</td></tr><tr><td>37</td><td>55</td><td>INT_STATUS2</td><td>R/C</td><td colspan="3">-</td><td>SMD_INT</td><td>WOM_Z_INT</td><td>WOM_Y_INT</td><td colspan="2">WOM_X_INT</td></tr><tr><td>38</td><td>56</td><td>INT_STATUS3</td><td>R/C</td><td colspan="2">-</td><td>STEP_DET_INT</td><td>STEP_CNT_OVF_INT</td><td>TILT_DET_INT</td><td>-</td><td>FF_DET_INT</td><td>TAP_DET_INT</td></tr><tr><td>4B</td><td>75</td><td>SIGNAL_PATH_RESET</td><td>W/C</td><td>-</td><td>DMP_INIT_E</td><td>DMP_MEM_RESET</td><td>-</td><td>ABORT_AND_RESET</td><td>TMST_STROB</td><td>FIFO_FLUSH</td><td>-</td></tr><tr><td rowspan="2">4C</td><td rowspan="2">76</td><td rowspan="2">INT_CONFIG</td><td rowspan="2">R/W</td><td>FIFO_HOLD_L</td><td>FIFO_COUNT</td><td>FIFO_COUNT</td><td>SENSOR_DAT</td><td rowspan="2">-</td><td rowspan="2" colspan="3">UI_SIFS_CFG</td></tr><tr><td>AST_DATA_E</td><td>REC</td><td>RENDIAN</td><td>A_RENDIAN</td></tr><tr><td>4D</td><td>77</td><td>INT_CONFIG</td><td>R/W</td><td colspan="4">-</td><td>ACCEL_LP_CL_K_SEL</td><td>RTC_MODE</td><td colspan="2">CLKSEL</td></tr><tr><td>4E</td><td>78</td><td>PWR_MGMTO</td><td>R/W</td><td colspan="2">-</td><td>TEMP_DIS</td><td>IDLE</td><td colspan="2">-</td><td colspan="2">ACCEL_MODE</td></tr><tr><td>50</td><td>80</td><td>ACCEL_CONFIG</td><td>R/W</td><td colspan="3">ACCEL_FS_SEL</td><td>-</td><td colspan="4">ACCEL_ODR</td></tr><tr><td>51</td><td>81</td><td>TEMP_FILT_CONFIG</td><td>R/W</td><td colspan="3">TEMP_FILT_BW</td><td>-</td><td colspan="2">-</td><td colspan="2">-</td></tr><tr><td>52</td><td>82</td><td>ACCEL_FILT_CONFIG</td><td>R/W</td><td colspan="4">ACCEL_UI_FILT_BW</td><td colspan="4">-</td></tr><tr><td>53</td><td>83</td><td>ACCEL_CONFIG1</td><td>R/W</td><td colspan="3">-</td><td>ACCEL_UI_FILT_ODR</td><td colspan="2">ACCEL_DEC2_M2_ODR</td><td colspan="2">-</td></tr><tr><td>54</td><td>84</td><td>TMST_CONFIG</td><td>R/W</td><td colspan="3">-</td><td>TMST_TO_REGS_EN</td><td>TMST_RES</td><td>TMST_DELTA_EN</td><td>TMST_FSYNC_EN</td><td>TMST_EN</td></tr><tr><td>56</td><td>86</td><td>APEX_CONFIG0</td><td>R/W</td><td>DMP_SAVE</td><td>TAP_ENABLE</td><td>PED_ENABLE</td><td>TILT_ENABLE</td><td>-</td><td>FF_ENABLE</td><td colspan="2">DMP_ODR</td></tr><tr><td>57</td><td>87</td><td>SMD_CONFIG</td><td>R/W</td><td colspan="4">-</td><td>WOM_INT_MODE</td><td>WOM_MODE</td><td colspan="2">SMD_MODE</td></tr><tr><td>5F</td><td>95</td><td>FIFO_CONFIG1</td><td>R/W</td><td>-</td><td>FIFO_RESUM</td><td>FIFO_WM_G</td><td>FIFO_HIRES_EN</td><td>FIFO_TMTS_F</td><td>FIFO_TEMP_EN</td><td colspan="2">FIFO_PACKET_EN</td></tr></table>

<table><tr><td>Addr (Hex)</td><td>Addr (Dec.)</td><td>Register Name</td><td>Serial I/F</td><td>Bit7</td><td>Bit6</td><td>Bit5</td><td>Bit4</td><td>Bit3</td><td>Bit2</td><td>Bit1</td><td>Bit0</td><td></td><td></td><td></td><td></td></tr><tr><td>60</td><td>96</td><td>FIFO_CONFIG2</td><td>R/W</td><td colspan="8">FIFO_WM[7:0]</td><td></td><td></td><td></td><td></td></tr><tr><td>61</td><td>97</td><td>FIFO_CONFIG3</td><td>R/W</td><td colspan="4">-</td><td colspan="4">FIFO_WM[11:8]</td><td></td><td></td><td></td><td></td></tr><tr><td>62</td><td>98</td><td>FSYNC_CONFIG</td><td>R/W</td><td>-</td><td colspan="3">FSYNC_UI_SEL</td><td colspan="2">-</td><td>FSYNC_UI_FL_AG_CLEAR_S_E</td><td>FSYNC_POLARITY</td><td></td><td></td><td></td><td></td></tr><tr><td>63</td><td>99</td><td>INT_CONFIG0</td><td>R/W</td><td>-</td><td colspan="3">UI_DRDY_INT_CLEAR</td><td colspan="2">FIFO_THS_INT_CLEAR</td><td colspan="2">FIFO_FULL_INT_CLEAR</td><td></td><td></td><td></td><td></td></tr><tr><td>64</td><td>100</td><td>INT_CONFIG1</td><td>R/W</td><td>-</td><td>INT_TPULS_ON_DURATION</td><td>INT_TDEASSE RETR_DISABLE</td><td colspan="2">INT_ASYNCREN</td><td colspan="3">-</td><td></td><td></td><td></td><td></td></tr><tr><td>65</td><td>101</td><td>INT_SOURCE0</td><td>R/W</td><td>-</td><td colspan="2">UI_FSYNC_ON_T1_EN</td><td colspan="2">RESET_DONE_INT1_EN</td><td colspan="2">UI_DRDY_INT_T1_EN</td><td colspan="2">UI_AGC_RDY_INT1_EN</td><td></td><td></td><td></td></tr><tr><td>66</td><td>102</td><td>INT_SOURCE1</td><td>R/W</td><td>-</td><td colspan="2">ISC_PROTOCOL_OLE_ERROR_IUT1_EN</td><td colspan="2">-</td><td>SMD_INT1_E N</td><td colspan="2">WOM_Z_INT 1_EN</td><td colspan="2">WOM_Y_INT 1_EN</td><td></td><td></td></tr><tr><td>68</td><td>104</td><td>INT_SOURCE3</td><td>R/W</td><td>-</td><td colspan="2">UI_FSYNC_T2_EN</td><td colspan="2">RESET_DONE_INT2_EN</td><td colspan="2">UI_DRDY_INT 2_EN</td><td colspan="2">UI_FULIINT2_EN</td><td colspan="2">UI_AGC_RDY_INT2_EN</td><td></td></tr><tr><td>69</td><td>105</td><td>INT_SOURCE4</td><td>R/W</td><td>-</td><td colspan="2">ISC_PROTOCOL_OLE_ERROR_IUT2_EN</td><td colspan="2">-</td><td>SMD_INT1_E N</td><td colspan="2">WOM_Z_INT 2_EN</td><td colspan="2">WOM_Y_INT 2_EN</td><td colspan="2">WOM_X_INT 2_EN</td></tr><tr><td>6C</td><td>108</td><td>FIFO_LOST_PKT0</td><td>R</td><td colspan="9">FIFO_LOST_PKT_CNT[15:8]</td><td></td><td></td><td></td></tr><tr><td>6D</td><td>109</td><td>FIFO_LOST_PKT1</td><td>R</td><td colspan="9">FIFO_LOST_PKT_CNT[7:0]</td><td></td><td></td><td></td></tr><tr><td>70</td><td>112</td><td>SELF_TEST_CONFIG</td><td>R/W</td><td colspan="2">ACCEL_ST_OWER</td><td>EN_AZ_ST</td><td>EN_AY_ST</td><td>EN_AX_ST</td><td>-</td><td>-</td><td>-</td><td>-</td><td></td><td></td><td></td></tr><tr><td>75</td><td>117</td><td>WHOL_AM_I</td><td>R</td><td colspan="9">WHOAMI</td><td></td><td></td><td></td></tr><tr><td>76</td><td>118</td><td>REG_BANK_SEL</td><td>R/W</td><td colspan="5">-</td><td colspan="4">BANK_SEL</td><td></td><td></td><td></td></tr></table>

# 13.2 USER BANK 1 REGISTER MAP

<table><tr><td>Addr (Hex)</td><td>Addr (Dec.)</td><td>Register Name</td><td>Serial I/F</td><td>Bit7</td><td>Bit6</td><td>Bit5</td><td>Bit4</td><td>Bit3</td><td>Bit2</td><td>Bit1</td><td>Bit0</td></tr><tr><td>03</td><td>03</td><td>SENSOR_CONFIG0</td><td>R/W</td><td>-</td><td></td><td></td><td></td><td></td><td>ZA_DISABLE</td><td>YA_DISABLE</td><td>XA_DISABLE</td></tr><tr><td>62</td><td>98</td><td>TMSTVAL0</td><td>R</td><td></td><td></td><td></td><td>TMST_VALUE[7:0]</td><td></td><td></td><td></td><td></td></tr><tr><td>63</td><td>99</td><td>TMSTVAL1</td><td>R</td><td></td><td></td><td></td><td>TMST_VALUE[15:8]</td><td></td><td></td><td></td><td></td></tr><tr><td>64</td><td>100</td><td>TMSTVAL2</td><td>R</td><td></td><td></td><td></td><td></td><td></td><td>TMST_VALUE[19:16]</td><td></td><td></td></tr><tr><td>7A</td><td>122</td><td>INTF_CONFIG4</td><td>R/W</td><td>-</td><td>13C_BUS_M0 DE</td><td colspan="4">-</td><td>SPL_AP_4WIR E</td><td>-</td></tr><tr><td>7B</td><td>123</td><td>INTF_CONFIG5</td><td>R/W</td><td colspan="5">-</td><td colspan="2">PIN9_FUNCTION</td><td>-</td></tr><tr><td>7C</td><td>124</td><td>INTF_CONFIG6</td><td>R/W</td><td colspan="3">-</td><td>13C_EN</td><td>13C_IBEI_EN</td><td>13C_IDEI_EN</td><td>13C_DDR_EN</td><td>13C_SDR_EN</td></tr></table>

# 13.3 USER BANK 2 REGISTER MAP

<table><tr><td>Addr (Hex)</td><td>Addr (Dec.)</td><td>Register Name</td><td>Serial I/F</td><td>Bit7</td><td>Bit6</td><td>Bit5</td><td>Bit4</td><td>Bit3</td><td>Bit2</td><td>Bit1</td><td>Bit0</td></tr><tr><td>03</td><td>03</td><td>ACCEL_CONFIG_STATIC2</td><td>R/W</td><td>-</td><td colspan="5">ACCEL_AAF_DELT</td><td>ACCEL_AAF_DIS</td><td></td></tr><tr><td>04</td><td>04</td><td>ACCEL_CONFIG_STATIC3</td><td>R/W</td><td colspan="7">ACCEL_AAF_DELTSQR[7:0]</td><td></td></tr><tr><td>05</td><td>05</td><td>ACCEL_CONFIG_STATIC4</td><td>R/W</td><td colspan="3">ACCEL_AAF_BITSHIFT</td><td colspan="4">ACCEL_AAF_DELTSQR[11:8]</td><td></td></tr><tr><td>3B</td><td>59</td><td>XA_ST_DATA</td><td>R/W</td><td colspan="7">XA_ST_DATA</td><td></td></tr><tr><td>3C</td><td>60</td><td>YA_ST_DATA</td><td>R/W</td><td colspan="7">YA_ST_DATA</td><td></td></tr><tr><td>3D</td><td>61</td><td>ZA_ST_DATA</td><td>R/W</td><td colspan="7">ZA_ST_DATA</td><td></td></tr></table>

# 13.4 USER BANK 3 REGISTER MAP

<table><tr><td>Addr (Hex)</td><td>Addr (Dec.)</td><td>Register Name</td><td>Serial I/F</td><td>Bit7</td><td>Bit6</td><td>Bit5</td><td>Bit4</td><td>Bit3</td><td>Bit2</td><td>Bit1</td><td>Bit0</td></tr><tr><td>06</td><td>06</td><td>PU_PD_CONFIG1</td><td>R/W</td><td>PIN11_PD_E N</td><td>PIN7_PD_EN</td><td>-</td><td>PIN9_PD_EN</td><td>PIN10_PD_E N</td><td>PIN3_PD_EN</td><td>PIN2_PD_EN</td><td>PIN4_PD_EN</td></tr><tr><td>0E</td><td>14</td><td>PU_PD_CONFIG2</td><td>R/W</td><td>PIN1_PD_EN</td><td>PIN1_PD_EN</td><td>PIN12_PD_E N</td><td>PIN12_PD_E N</td><td>PIN14_PD_E N</td><td>PIN14_PD_E N</td><td>PIN13_PD_E N</td><td>PIN13_PD_E N</td></tr></table>

# 13.5 USER BANK 4 REGISTER MAP

<table><tr><td>Addr (Hex)</td><td>Addr (Dec.)</td><td>Register Name</td><td>Serial I/F</td><td>Bit7</td><td>Bit6</td><td>Bit5</td><td>Bit4</td><td>Bit3</td><td>Bit2</td><td>Bit1</td><td>Bit0</td></tr><tr><td>09</td><td>09</td><td>FDR_CONFIG</td><td>R/W</td><td colspan="8">FDR_SEL</td></tr><tr><td>40</td><td>64</td><td>APEX_CONFIG1</td><td>R/W</td><td colspan="4">LOW_ENRGY_AMP_TH_SEL</td><td colspan="4">DMP_POWER_SAVE_TIME_SEL</td></tr><tr><td>41</td><td>65</td><td>APEX_CONFIG2</td><td>R/W</td><td colspan="4">PED_AMP_TH_SEL</td><td colspan="4">PED_STEP_CNT_TH_SEL</td></tr><tr><td>42</td><td>66</td><td>APEX_CONFIG3</td><td>R/W</td><td colspan="4">PED_STEP_DET_TH_SEL</td><td colspan="4">PED_SB_TIMER_TH_SEL</td></tr><tr><td>43</td><td>67</td><td>APEX_CONFIG4</td><td>R/W</td><td colspan="2">TILT_WAIT_TIME_SEL</td><td colspan="2">LOWG_PEAR_TH_HYST_SEL</td><td colspan="4">HIGHG_PEAR_TH_HYST_SEL</td></tr><tr><td>44</td><td>68</td><td>APEX_CONFIG5</td><td>R/W</td><td colspan="4">LOWG_PEAR_TH_SEL</td><td colspan="4">LOWG_TIME_TH_SEL</td></tr><tr><td>45</td><td>69</td><td>APEX_CONFIG6</td><td>R/W</td><td colspan="4">HIGHG_PEAR_TH_SEL</td><td colspan="4">HIGHG_TIME_TH_SEL</td></tr><tr><td>46</td><td>70</td><td>APEX_CONFIG7</td><td>R/W</td><td colspan="4">TAP_MIN_JERK_THR</td><td colspan="4">TAP_MAX_PEAR_THL</td></tr><tr><td>47</td><td>71</td><td>APEX_CONFIG8</td><td>R/W</td><td>-</td><td colspan="2">TAP_TMAX</td><td colspan="2">TAP_TAVG</td><td colspan="3">TAP_TMIN</td></tr><tr><td>48</td><td>72</td><td>APEX_CONFIG9</td><td>R/W</td><td colspan="7">-</td><td>SENSITIVITY_MODE</td></tr><tr><td>49</td><td>73</td><td>APEX_CONFIG10</td><td>R/W</td><td colspan="2">FF_MIN_DURATION_CM</td><td colspan="2">FF_MAX_DURATION_CM</td><td colspan="4">FF_DEBounce_DURATION</td></tr><tr><td>4A</td><td>74</td><td>ACCEL_WDM_Y_THR</td><td>R/W</td><td colspan="8">WOM_X_TH</td></tr><tr><td>4B</td><td>75</td><td>ACCEL_WDM_Y_THR</td><td>R/W</td><td colspan="8">WOM_Y_TH</td></tr><tr><td>4C</td><td>76</td><td>ACCEL_WDM_Z_THR</td><td>R/W</td><td colspan="8">WOM_Z_TH</td></tr><tr><td>4D</td><td>77</td><td>INT_SOURCE6</td><td>R/W</td><td>-</td><td>STEP_DET_IN T1_EN</td><td>STEP_CNT_O FL_INT1_EN</td><td>TILT_DET_IN T1_EN</td><td>-</td><td>FREEFALL_DE T_INT1_EN</td><td>TAP_DET_INT 1_EN</td><td></td></tr><tr><td>4E</td><td>78</td><td>INT_SOURCE7</td><td>R/W</td><td>-</td><td>STEP_DET_IN T2_EN</td><td>STEP_CNT_O FL_INT2_EN</td><td>TILT_DET_IN T2_EN</td><td>-</td><td>FREEFALL_DE T_INT2_EN</td><td>TAP_DET_INT 2_EN</td><td></td></tr><tr><td>4F</td><td>79</td><td>INT_SOURCE8</td><td>R/W</td><td>-</td><td>FSYNC_IIBI_N</td><td>UI_RDRY_IIBI_EN</td><td>FIQ_THS_IIBI_EN</td><td>FIQ_FULL_IIBI_EN</td><td>AGC_RDY_IIBI_EN</td><td></td><td></td></tr><tr><td>50</td><td>80</td><td>INT_SOURCE9</td><td>R/W</td><td>I3C PROTO OL_ERROR_I BI_EN</td><td>-</td><td>SMD_IIBI_EN</td><td>WOM_Z_IIBI_EN</td><td>WOM_Y_IIBI_EN</td><td>WOM_X_IIBI_EN</td><td>-</td><td></td></tr><tr><td>51</td><td>81</td><td>INT_SOURCE10</td><td>R/W</td><td>-</td><td>STEP_DET_IIB IEN</td><td>STEP_CNT_O FL_IIBI_EN</td><td>TILT_DET_IIBI_EN</td><td>-</td><td>FREEFALL_DE T_IIBI_EN</td><td>TAP_DET_IIBI_EN</td><td></td></tr><tr><td>7B</td><td>123</td><td>OFFSET_USER4</td><td>R/W</td><td colspan="4">ACCEL_X_OFFUSER[11:8]</td><td colspan="4">Reserved</td></tr><tr><td>7C</td><td>124</td><td>OFFSET_USER5</td><td>R/W</td><td colspan="8">ACCEL_X_OFFUSER[7:0]</td></tr><tr><td>7D</td><td>125</td><td>OFFSET_USER6</td><td>R/W</td><td colspan="8">ACCEL_Y_OFFUSER[7:0]</td></tr><tr><td>7E</td><td>126</td><td>OFFSET_USER7</td><td>R/W</td><td colspan="4">ACCEL_Z_OFFUSER[11:8]</td><td colspan="4">ACCEL_Y_OFFUSER[11:8]</td></tr><tr><td>7F</td><td>127</td><td>OFFSET_USER8</td><td>R/W</td><td colspan="8">ACCEL_Z_OFFUSER[7:0]</td></tr></table>

Detailed register descriptions are provided in the sections that follow. Please note the following regarding Clock Domain for each register:

Clock Domain: SCLK_UI means that the register is controlled from the UI interface

Register fields marked as Reserved must not be modified by the user. The Reset Value of the register can be used to determine the default value of reserved register fields, and unless otherwise noted this default value must be maintained even if the values of other register fields are modified by the user.

# 13.6 REGISTER VALUES MODIFICATION

The only register settings that user can modify during sensor operation are for ODR selection, FSR selection, and sensor mode changes (register parameters ACCEL_ODR, ACCEL_FS_SEL, ACCEL_MODE). User must not modify any other register values during sensor operation. The following procedure must be used for other register values modification.

- Turn Accel Off- Modify register values- Turn Accel On

# 14 USER BANK O REGISTER MAP - DESCRIPTIONS

This section describes the function and contents of each register within USR Bank 0.

Note: The device powers up in sleep mode.

# 14.1 DEVICE_CONFIG

<table><tr><td colspan="3">Name: DEVICE_CONFIG
Address: 17 (11h)
Serial IF: R/W
Reset value: 0x00
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:5</td><td>-</td><td>Reserved</td></tr><tr><td>4</td><td>SPI_MODE</td><td>SPI mode selection
0: Mode 0 and Mode 3 (default)
1: Mode 1 and Mode 2</td></tr><tr><td>3:1</td><td>-</td><td>Reserved</td></tr><tr><td>0</td><td>SOFT_RESET_CONFIG</td><td>Software reset configuration
0: Normal (default)
1: Enable reset
After writing 1 to this bitfield, wait 1ms for soft reset to be effective, before attempting any other register access</td></tr></table>

# 14.2 DRIVE_CONFIG

<table><tr><td colspan="3">Name: DRIVE_CONFIG
Address: 19 (13h)
Serial IF: R/W
Reset value: 0x05
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:6</td><td>-</td><td>Reserved</td></tr><tr><td>5:3</td><td>I2C_SLEW_RATE</td><td>Controls slew rate for output pin 14 in I2C mode only
000: 20ns-60ns
001: 12ns-36ns
010: 6ns-18ns
011: 4ns-12ns
100: 2ns-6ns
101: &amp;lt; 2ns
110: Reserved
111: Reserved</td></tr><tr><td>2:0</td><td>SPI_SLEW_RATE</td><td>Controls slew rate for output pin 14 in SPI or I3CSM mode, and for all other output pins
000: 20ns-60ns
001: 12ns-36ns
010: 6ns-18ns
011: 4ns-12ns
100: 2ns-6ns
101: &amp;lt; 2ns
110: Reserved
111: Reserved</td></tr></table>

# 14.3 INT_CONFIG

<table><tr><td colspan="3">Name: INT_CONFIG
Address: 20 (14h)
Serial IF: R/W
Reset value: 0x00
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:6</td><td>-</td><td>Reserved</td></tr><tr><td>5</td><td>INT2_MODE</td><td>INT2 interrupt mode
0: Pulsed mode
1: Latched mode</td></tr><tr><td>4</td><td>INT2_DRIVE_CIRCUIT</td><td>INT2 drive circuit
0: Open drain
1: Push pull</td></tr><tr><td>3</td><td>INT2_POLARITY</td><td>INT2 interrupt polarity
0: Active low (default)
1: Active high</td></tr><tr><td>2</td><td>INT1_MODE</td><td>INT1 interrupt mode
0: Pulsed mode
1: Latched mode</td></tr><tr><td>1</td><td>INT1_DRIVE_CIRCUIT</td><td>INT1 drive circuit
0: Open drain
1: Push pull</td></tr><tr><td>0</td><td>INT1_POLARITY</td><td>INT1 interrupt polarity
0: Active low (default)
1: Active high</td></tr></table>

# 14.4 FIFO_CONFIG

<table><tr><td colspan="3">Name: FIFO_CONFIG
Address: 22 (16h)
Serial IF: R/W
Reset value: 0x00
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:6</td><td>FIFO_MODE</td><td>00: Bypass Mode (default)
01: Stream-to-FIFO Mode
10: STOP-on-FULL Mode
11: STOP-on-FULL Mode</td></tr><tr><td>5:0</td><td>-</td><td>Reserved</td></tr></table>

# 14.5 TEMP_DATA1

<table><tr><td>Name: TEMP_DATA1
Address: 29 (1Dh)
Serial IF: SYNCR
Reset value: 0x80</td><td></td><td></td></tr><tr><td colspan="3">Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:0</td><td>TEMP_DATA[15:8]</td><td>Upper byte of temperature data</td></tr></table>

# 14.6 TEMP_DATAO

<table><tr><td colspan="3">Name: TEMP_DATA0</td></tr><tr><td colspan="3">Address: 30 (1Eh)</td></tr><tr><td colspan="3">Serial IF: SYNCR</td></tr><tr><td colspan="3">Reset value: 0x00</td></tr><tr><td colspan="3">Clock Domain: SCLK, UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:0</td><td>TEMP_DATA[7:0]</td><td>Lower byte of temperature data</td></tr></table>

Temperature data value from the sensor data registers can be converted to degrees centigrade by using the following formula:

Temperature in Degrees Centigrade  $=$  TEMP_DATA/132.48)+25

FIFO_TEMP_DATA, temperature data stored in FIFO, can be 8- bit or 16- bit quantity. The 8- bit of temperature data stored in FIFO is limited to  $- 40^{\circ}C$  to  $85^{\circ}C$  range, while the 16- bit representation can support the full operating temperature range. It can be converted to degrees centigrade by using the following formula:

Temperature in Degrees Centigrade  $=$  (FIFO_TEMP_DATA/2.07)+25

# 14.7 ACCEL_DATA_X1

<table><tr><td colspan="3">Name: ACCEL_DATA_X1</td></tr><tr><td colspan="3">Address: 31 (1Fh)</td></tr><tr><td colspan="3">Serial IF: SYNCR</td></tr><tr><td colspan="3">Reset value: 0x80</td></tr><tr><td colspan="3">Clock Domain: SCLK, UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:0</td><td>ACCEL_DATA_X[15:8]</td><td>Upper byte of Accel X-axis data</td></tr></table>

# 14.8 ACCEL_DATA_XO

<table><tr><td colspan="3">Name: ACCEL_DATA_XO</td></tr><tr><td colspan="3">Address: 32 (20h)</td></tr><tr><td colspan="3">Serial IF: SYNCR</td></tr><tr><td colspan="3">Reset value: 0x00</td></tr><tr><td colspan="3">Clock Domain: SCLK, UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:0</td><td>ACCEL_DATA_X[7:0]</td><td>Lower byte of Accel X-axis data</td></tr></table>

# 14.9 ACCEL_DATA_Y1

<table><tr><td colspan="3">Name: ACCEL_DATA_Y1</td></tr><tr><td colspan="3">Address: 33 (21h)</td></tr><tr><td colspan="3">Serial IF: SYNCR</td></tr><tr><td colspan="3">Reset value: 0x80</td></tr><tr><td colspan="3">Clock Domain: SCLK, UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:0</td><td>ACCEL_DATA_Y[15:8]</td><td>Upper byte of Accel Y-axis data</td></tr></table>

# 14.10 ACCEL_DATA_YO

<table><tr><td colspan="3">Name: ACCEL_DATA_YO</td></tr><tr><td colspan="3">Address: 34 (22h)</td></tr><tr><td colspan="3">Serial IF: SYNCR</td></tr><tr><td colspan="3">Reset value: 0x00</td></tr><tr><td colspan="3">Clock Domain: SCLK, UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:0</td><td>ACCEL_DATA_Y[7:0]</td><td>Lower byte of Accel Y-axis data</td></tr></table>

# 14.11 ACCEL_DATA_Z1

<table><tr><td colspan="3">Name: ACCEL_DATA_Z1</td></tr><tr><td colspan="3">Address: 35 (23h)</td></tr><tr><td colspan="3">Serial IF: SYNCR</td></tr><tr><td colspan="3">Reset value: 0x80</td></tr><tr><td colspan="3">Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:0</td><td>ACCEL_DATA_Z[15:8]</td><td>Upper byte of Accel Z-axis data</td></tr></table>

# 14.12 ACCEL_DATA_Z0

<table><tr><td colspan="3">Name: ACCEL_DATA_Z0</td></tr><tr><td colspan="3">Address: 36 (24h)</td></tr><tr><td colspan="3">Serial IF: SYNCR</td></tr><tr><td colspan="3">Reset value: 0x00</td></tr><tr><td colspan="3">Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:0</td><td>ACCEL_DATA_Z[7:0]</td><td>Lower byte of Accel Z-axis data</td></tr></table>

# 14.13 TMST_FSYNCH

<table><tr><td colspan="3">Name: TMST_FSYNCH</td></tr><tr><td colspan="3">Address: 43 (2Bh)</td></tr><tr><td colspan="3">Serial IF: SYNCR</td></tr><tr><td colspan="3">Reset value: 0x00</td></tr><tr><td colspan="3">Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:0</td><td>TMST_FSYNC_DATA[15:8]</td><td>Stores the upper byte of the time delta from the rising edge of FSYNC to the latest ODR until the UI interface reads the FSYNC tag in the status register</td></tr></table>

# 14.14 TMST_FSYNCL

<table><tr><td colspan="3">Name: TMST_FSYNCL</td></tr><tr><td colspan="3">Address: 44 (2Ch)</td></tr><tr><td colspan="3">Serial IF: SYNCR</td></tr><tr><td colspan="3">Reset value: 0x00</td></tr><tr><td colspan="3">Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:0</td><td>TMST_FSYNC_DATA[7:0]</td><td>Stores the lower byte of the time delta from the rising edge of FSYNC to the latest ODR until the UI interface reads the FSYNC tag in the status register</td></tr></table>

# 14.15 INT_STATUS

<table><tr><td colspan="3">Name: INT_STATUS
Address: 45 (2Dh)
Serial IF: R/C
Reset value: 0x10
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7</td><td>-</td><td>Reserved</td></tr><tr><td>6</td><td>UI_FSYNC_INT</td><td>This bit automatically sets to 1 when a UI FSYNC interrupt is generated. The bit clears to 0 after the register has been read.</td></tr><tr><td>5</td><td>-</td><td>Reserved</td></tr><tr><td>4</td><td>RESET_DONE_INT</td><td>This bit automatically sets to 1 when software reset is complete. The bit clears to 0 after the register has been read.</td></tr><tr><td>3</td><td>DATA_RDY_INT</td><td>This bit automatically sets to 1 when a Data Ready interrupt is generated. The bit clears to 0 after the register has been read.</td></tr><tr><td>2</td><td>FIFO_THS_INT</td><td>This bit automatically sets to 1 when the FIFO buffer reaches the threshold value. The bit clears to 0 after the register has been read.</td></tr><tr><td>1</td><td>FIFO_FULL_INT</td><td>This bit automatically sets to 1 when the FIFO buffer is full. The bit clears to 0 after the register has been read.</td></tr><tr><td>0</td><td>AGC_RDY_INT</td><td>This bit automatically sets to 1 when an AGC Ready interrupt is generated. The bit clears to 0 after the register has been read.</td></tr></table>

# 14.16 FIFO_COUNTH

<table><tr><td colspan="3">Name: FIFO_COUNTH
Address: 46 (2Eh)
Serial IF: R
Reset value: 0x00
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:0</td><td>FIFO_COUNT[15:8]</td><td>High Bits, count indicates the number of records or bytes available in FIFO according to FIFO_COUNT_REG setting.
Note: Must read FIFO_COUNTL to latch new data for both FIFO_COUNTH and FIFO_COUNTL.</td></tr></table>

# 14.17 FIFO_COUNTL

<table><tr><td colspan="3">Name: FIFO_COUNTL
Address: 47 (2Fh)
Serial IF: R
Reset value: 0x00
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:0</td><td>FIFO_COUNT[7:0]</td><td>Low Bits, count indicates the number of records or bytes available in FIFO according to FIFO_COUNT_REG setting.
Reading this byte latches the data for both FIFO_COUNTH, and FIFO_COUNTL.</td></tr></table>

# 14.18 FIFO_DATA

<table><tr><td colspan="3">Name: FIFO_DATA
Address: 48 (30h)
Serial IF: R
Reset value: 0xFF
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:0</td><td>FIFO_DATA</td><td>FIFO data port</td></tr></table>

# 14.19 APEX_DATA0

<table><tr><td colspan="3">Name: APEX_DATA0
Address: 49 (31h)
Serial IF: SYNCR
Reset value: 0x00
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:0</td><td>STEP_CNT[7:0]</td><td>Pedometer Output: Lower byte of Step Count measured by pedometer</td></tr></table>

# 14.20 APEX_DATA1

<table><tr><td colspan="3">Name: APEX_DATA1
Address: 50 (32h)
Serial IF: SYNCR
Reset value: 0x00
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:0</td><td>STEP_CNT[15:8]</td><td>Pedometer Output: Upper byte of Step Count measured by pedometer</td></tr></table>

# 14.21 APEX_DATA2

<table><tr><td colspan="3">Name: APEX_DATA2
Address: 51 (33h)
Serial IF: R
Reset value: 0x00
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:0</td><td>STEP_CADENCE</td><td>Pedometer Output: Walk/run cadence in number of samples. Format is u6.2. e.g. At 50 Hz ODR and 2 Hz walk frequency, the cadence is 25 samples. The register will output 100.</td></tr></table>

# 14.22 APEX_DATA3

<table><tr><td colspan="3">Name: APEX_DATA3
Address: 52 (34h)
Serial IF: R
Reset value: 0x04
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:3</td><td>-</td><td>Reserved</td></tr><tr><td>2</td><td>DMP_IDLE</td><td>0: Indicates DMP is running
1: Indicates DMP is idle</td></tr><tr><td>1:0</td><td>ACTIVITY_CLASS</td><td>Pedometer Output: Detected activity
00: Unknown
01: Walk
10: Run
11: Reserved</td></tr></table>

# 14.23 APEX_DATA4

<table><tr><td colspan="3">Name: APEX_DATA4
Address: 53 (35h)
Serial IF: R
Reset value: 0x00
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:5</td><td>-</td><td>Reserved</td></tr><tr><td>4:3</td><td>TAP_NUM</td><td>Tap Detection Output: Number of taps in the current Tap event
00: No tap
01: Single tap
10: Double tap
11: Reserved</td></tr><tr><td>2:1</td><td>TAP_AXIS</td><td>Tap Detection Output: Represents the accelerometer axis on which tap energy is concentrated
00: X-axis
01: Y-axis
10: Z-axis
11: Reserved</td></tr><tr><td>0</td><td>TAP_DIR</td><td>Tap Detection Output: Polarity of tap pulse
0: Current accelerometer value – Previous accelerometer value is a positive value
1: Current accelerometer value – Previous accelerometer value is a negative value or zero</td></tr></table>

# 14.24 APEX_DATAS

<table><tr><td colspan="3">Name: APEX_DATA5
Address: 54 (36h)
Serial IF: R
Reset value: 0x00
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:6</td><td>-</td><td>Reserved</td></tr><tr><td>5:0</td><td>DOUBLE_TAP_TIMING</td><td>DOUBLE_TAP_TIMING measures the time interval between the two taps when double tap is detected. It counts every 16 accelerometer samples as one unit between the 2 tap pulses. Therefore, the value is related to the accelerometer ODR.
Time in seconds = DOUBLE_TAP_TIMING * 16 / ODR
For example, if the accelerometer ODR is 500 Hz, and the DOUBLE_TAP_TIMING register reading is 6, the time interval value is 6*16/500 = 0.192 seconds.</td></tr></table>

# 14.25 INT_STATUS2

<table><tr><td colspan="3">Name: INT_STATUS2
Address: 55 (37h)
Serial IF: R/C
Reset value: 0x00
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:4</td><td>-</td><td>Reserved</td></tr><tr><td>3</td><td>SMD_INT</td><td>Significant Motion Detection Interrupt, clears on read</td></tr><tr><td>2</td><td>WOM_Z_INT</td><td>Wake on Motion Interrupt on Z-axis, clears on read</td></tr><tr><td>1</td><td>WOM_Y_INT</td><td>Wake on Motion Interrupt on Y-axis, clears on read</td></tr><tr><td>0</td><td>WOM_X_INT</td><td>Wake on Motion Interrupt on X-axis, clears on read</td></tr></table>

# 14.26 INT_STATUS3

<table><tr><td colspan="3">Name: INT_STATUS3
Address: 56 (38h)
Serial IF: R/C
Reset value: 0x00
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:6</td><td>-</td><td>Reserved</td></tr><tr><td>5</td><td>STEP_DET_INT</td><td>Step Detection Interrupt, clears on read</td></tr><tr><td>4</td><td>STEP_CNT_DVF_INT</td><td>Step Count Overflow Interrupt, clears on read</td></tr><tr><td>3</td><td>tilt_DET_INT</td><td>Tilt Detection Interrupt, clears on read</td></tr><tr><td>2</td><td>-</td><td>Reserved</td></tr><tr><td>1</td><td>FF_DET_INT</td><td>Freefall Interrupt, clears on read</td></tr><tr><td>0</td><td>TAP_DET_INT</td><td>Tap Detection Interrupt, clears on read</td></tr></table>

# 14.27 SIGNAL_PATH_RESET

<table><tr><td colspan="3">Name: SIGNAL_PATH_RESET</td></tr><tr><td colspan="3">Address: 75 (4Bh)</td></tr><tr><td colspan="3">Serial IF: W/C</td></tr><tr><td colspan="3">Reset value: 0x00</td></tr><tr><td colspan="3">Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7</td><td>-</td><td>Reserved</td></tr><tr><td>6</td><td>DMP_INIT_EN</td><td>When this bit is set to 1, the DMP is enabled</td></tr><tr><td>5</td><td>DMP_MEM_RESET_EN</td><td>When this bit is set to 1, the DMP memory is reset</td></tr><tr><td>4</td><td>-</td><td>Reserved</td></tr><tr><td>3</td><td>ABORT_AND_RESET</td><td>When this bit is set to 1, the signal path is reset by restarting the ODR counter and signal path controls</td></tr><tr><td>2</td><td>TMST_STROBE</td><td>When this bit is set to 1, the time stamp counter is latched into the time stamp register. This is a write on clear bit</td></tr><tr><td>1</td><td>FIFO_FLUSH</td><td>When set to 1, FIFO will get flushed</td></tr><tr><td>0</td><td>-</td><td>Reserved</td></tr></table>

# 14.28

# 14.29 INTF_CONFIGO

<table><tr><td colspan="3">Name: INTF_CONFIGO
Address: 76 (4Ch)
Serial IF: R/W
Reset value: 0x30
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7</td><td>FIFO_HOLD_LAST_DATA_EN</td><td>Setting 0 corresponds to the following:
Sense Registers from Power on Reset till first sample:
• Invalid Samples Value: -32768
Sense Registers after first sample received:
• Sense Registers Valid Sample Values:
    ○ Range limited from -32766 to +32767 when FSYNC tag is disabled, or for sensor not selected for FSYNC tag
    ○ Range limited from -32765 to +32767 (odd values) for sensor selected for FSYNC tag, and FSYNC is tagged
    ○ Range limited from -32766 to +32766 (even values) for sensor selected for FSYNC tag, but FSYNC is not tagged
• Sense Registers Invalid Sample Values:
    ○ -32768 when FSYNC tag is disabled, or for sensor not selected for FSYNC tag, or for sensor selected for FSYNC tag but FSYNC is not tagged
    ○ -32767 for sensor selected for FSYNC tag, and FSYNC is tagged</td></tr></table>

FIFO:
• Invalid Sample Value: -32768
• Valid Sample Values: -32766 to +32767

Setting 1 corresponds to the following:
Sense Registers from Power on Reset till first sample:
• Invalid Samples Value: 0

Sense Registers after first sample received:
• Sense Registers Valid Sample Values:
    ○ Range limited from -32768 to +32767 when FSYNC tag is disabled, or for sensor not selected for FSYNC tag
    ○ Range limited from -32767 to +32767 (odd values) for sensor selected for FSYNC tag, and FSYNC is tagged
    ○ Range limited from -32768 to +32766 (even values) for sensor selected for FSYNC tag, but FSYNC is not tagged
• Sense Registers Invalid Sample Values:
    ○ Registers hold last valid sample until new one arrives

<table><tr><td>&lt;fcel&gt;</td><td>&lt;fcel&gt;</td><td>&lt;nl&gt;</td></tr><tr><td>&lt;fcel&gt;</td><td>&lt;fcel&gt;</td><td>&lt;nl&gt;</td></tr></table>

<table><tr><td></td><td></td><td>1: FIFO count is reported in Big Endian format (default)</td></tr><tr><td>4</td><td>SENSOR_DATA_ENDIAN</td><td>0: Sensor data is reported in Little Endian format
1: Sensor data is reported in Big Endian format (default)</td></tr><tr><td>3:2</td><td>-</td><td>Reserved</td></tr><tr><td>1:0</td><td>UI_SIFS_CFG</td><td>0x: Reserved
10: Disable SPI
11: Disable I2C</td></tr></table>

Invalid Data Generation: FIFO/Sense Registers may contain invalid data under the following conditions:

a) From power on reset to first ODR sample of any sensor (accel, temp sensor)  
b) When any sensor is disabled (accel, temp sensor)  
c) When accel is enabled with different ODRs. In this case, the sensor with lower ODR will generate invalid samples when it has no new data.

Invalid data can take special values or can hold last valid sample received. For - 32768 to be used as a flag for invalid accel samples, the valid accel sample range is limited in such case as well. Bit 7 of INTF_CONFIG0 controls what values invalid (and valid) samples can take as shown above.

# 14.30 INTF_CONFIG1

<table><tr><td colspan="3">Name: INTF_CONFIG1
Address: 77 (4Dh)
Serial IF: R/W
Reset value: 0x90
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:4</td><td>-</td><td>Reserved</td></tr><tr><td>3</td><td>ACCEL_LP_CLK_SEL</td><td>0: Accelerometer LP mode uses Wake Up oscillator clock
1: Accelerometer LP mode uses RC oscillator clock
0: No input RTC clock is required
1: RTC clock input is required</td></tr><tr><td>2</td><td>RTC_MODE</td><td rowspan="2">00: Always select internal RC oscillator
01: Reserved
10: Reserved
11: Disable all clocks</td></tr><tr><td>1:0</td><td>CLKSEL</td></tr></table>

# 14.31 PWR_MGMTO

<table><tr><td colspan="3">Name: PWR_MGMTO
Address: 78 (4Eh)
Serial IF: R/W
Reset value: 0x00
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:6</td><td>-</td><td>Reserved</td></tr><tr><td>5</td><td>TEMP_DIS</td><td>0: Temperature sensor is enabled (default)
1: Temperature sensor is disabled</td></tr><tr><td>4</td><td>IDLE</td><td>If this bit is set to 1, the RC oscillator is powered on even if Accel is powered off. 
Nominally this bit is set to 0, so when Accel is powered off, 
the chip will go to OFF state, since the RC oscillator will also be powered off</td></tr><tr><td>3:2</td><td>Reserved</td><td>00: default</td></tr><tr><td>1:0</td><td>ACCEL_MODE</td><td>00: Turns accelerometer off (default)
01: Turns accelerometer off
10: Places accelerometer in Low Power (LP) Mode
11: Places accelerometer in Low Noise (LN) Mode</td></tr></table>

When transitioning from OFF to any of the other modes, do not issue any register writes for 200μs.<nl>

# 14.32 ACCEL_CONFIGO

<table><tr><td colspan="3">Name: ACCEL_CONFIG0
Address: 80 (50h)
Serial IF: R/W
Reset value: 0x03
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:5</td><td>ACCEL_FS_SEL</td><td>Full scale select for accelerometer UI interface output
000: ±16g (default)
001: ±8g
010: ±4g
011: ±2g
100: Reserved
101: Reserved
110: Reserved
111: Reserved</td></tr><tr><td>4</td><td>Reserved</td><td>Reserved</td></tr><tr><td>3:0</td><td>ACCEL_ODR</td><td>Accelerometer ODR selection for UI interface output
0000: Reserved
0001: 32kHz
0010: 16kHz
0011: 8kHz (LN mode)
0100: 4kHz (LN mode)
0101: 2kHz (LN mode)
0110: 1kHz (LN mode) (Default)
0111: 200Hz (LP or LN mode)
1000: 100Hz (LP or LN mode)
1001: 50Hz (LP or LN mode)
1010: 25Hz (LP or LN mode)
1011: 12.5Hz (LP or LN mode)
1100: 6.25Hz (LP mode)
1101: 3.125Hz (LP mode)
1110: 1.5625Hz (LP mode)
1111: 500Hz (LP or LN mode)</td></tr></table>

Refer to Note 6 under Table 1 for ACCEL_ODR recommended settings to achieve high 3dB value for accelerometer signal bandwidth.

# 14.33 TEMP_FILT_CONFIG

<table><tr><td colspan="3">Name: TEMP_FILT_CONFIG
Address: 81 (51h)
Serial IF: R/W
Reset value: 0x16
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:5</td><td>TEMP_FILT_BW</td><td>Sets the bandwidth of the temperature signal DLPF
000: DLPF BW = 4000Hz; DLPF Latency = 0.125ms (default)
001: DLPF BW = 170Hz; DLPF Latency = 1.0ms
010: DLPF BW = 82Hz; DLPF Latency = 2ms
011: DLPF BW = 40Hz; DLPF Latency = 4ms
100: DLPF BW = 20Hz; DLPF Latency = 8ms
101: DLPF BW = 10Hz; DLPF Latency = 16ms
110: DLPF BW = 5Hz; DLPF Latency = 32ms
111: DLPF BW = 5Hz; DLPF Latency = 32ms</td></tr><tr><td>4</td><td>-</td><td>Reserved</td></tr><tr><td>3:2</td><td>-</td><td>Reserved</td></tr><tr><td>1:0</td><td>-</td><td>Reserved</td></tr></table>

# 14.34 ACCEL_FILT_CONFIG

<table><tr><td colspan="3">Name: ACCEL_FILT_CONFIG
Address: 82 (52h)
Serial IF: R/W
Reset value: 0x11
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:4</td><td>ACCEL_UI_FILT_BW</td><td>LN Mode:
Bandwidth for Accel LPF
0 BW=ODR/2
1 BW=max(400Hz, ODR)/4 (default)
2 BW=max(400Hz, ODR)/5
3 BW=max(400Hz, ODR)/8
4 BW=max(400Hz, ODR)/10
5 BW=max(400Hz, ODR)/16
6 BW=max(400Hz, ODR)/20
7 BW=max(400Hz, ODR)/40
8 to 13: Reserved
14 Low Latency option: Trivial decimation @ ODR of Dec2 filter output. Dec2 runs at max(400Hz, ODR)
15 Low Latency option: Trivial decimation @ ODR of Dec2 filter output. Dec2 runs at max(200Hz, 8*ODR)</td></tr><tr><td>3:0</td><td>-</td><td>LP Mode:
0 Reserved
1 1x AVG filter (default)
2 to 5 Reserved
6 16x AVG filter
7 to 15 Reserved</td></tr></table>

# 14.35 ACCEL_CONFIG1

<table><tr><td colspan="3">Name: ACCEL_CONFIG1
Address: 83 (53h)
Serial IF: R/W
Reset value: 0x0D
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:5</td><td>-</td><td>Reserved</td></tr><tr><td>4:3</td><td>ACCEL_UI_FILT_ORD</td><td>Selects order of ACCEL UI filter
00: 1st Order
01: 2nd Order
10: 3rd Order
11: Reserved</td></tr><tr><td>2:1</td><td>ACCEL_DEC2_M2_ORD</td><td>Order of Accelerometer DEC2_M2 filter
00: Reserved
01: Reserved
10: 3rd order
11: Reserved</td></tr><tr><td>0</td><td>-</td><td>Reserved</td></tr></table>

# 14.36 TMST_CONFIG

<table><tr><td colspan="3">Name: TMST_CONFIG
Address: 84 (54h)
Serial IF: R/W
Reset value: 0x23
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:5</td><td>-</td><td>Reserved</td></tr><tr><td>4</td><td>TMST_TO_REGS_EN</td><td>0: TMST_VALUE[19:0] read always returns 0s
1: TMST_VALUE[19:0] read returns timestamp value</td></tr><tr><td>3</td><td>TMST_RES</td><td>Time Stamp resolution: When set to 0 (default), time stamp resolution is 1μs. When set to 1, resolution is 16μs</td></tr><tr><td>2</td><td>TMST_DELTA_EN</td><td>Time Stamp delta enable: When set to 1, the time stamp field contains the measurement of time since the last occurrence of ODR.</td></tr><tr><td>1</td><td>TMST_FSYNC_EN</td><td>Time Stamp register FSYNC enable (default). When set to 1, the contents of the Timestamp feature of FSYNC is enabled. The user also needs to select FIFO_TMST_FSYNC_EN in order to propagate the timestamp value to the FIFO.</td></tr><tr><td>0</td><td>TMST_EN</td><td>0: Time Stamp register disable
1: Time Stamp register enable (default)</td></tr></table>

# 14.37 APEX_CONFIGO

<table><tr><td colspan="3">Name: APEX_CONFIG0
Address: 86 (56h)
Serial IF: R/W
Reset value: 0x82
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7</td><td>DMP_POWER_SAVE</td><td>0: DMP power save mode not active
1: DMP power save mode active (default)</td></tr><tr><td>6</td><td>TAP_ENABLE</td><td>0: Tap Detection not enabled
1: Tap Detection enabled when accelerometer ODR is set to one of the ODR values supported by Tap Detection (200 Hz, 500 Hz, 1 kHz)</td></tr><tr><td>5</td><td>PED_ENABLE</td><td>0: Pedometer not enabled
1: Pedometer enabled</td></tr><tr><td>4</td><td>TILT_ENABLE</td><td>0: Tilt Detection not enabled
1: Tilt Detection enabled</td></tr><tr><td>3</td><td>-</td><td>Reserved</td></tr><tr><td>2</td><td>FF_ENABLE</td><td>0: Freefall Detection not enabled
1: Freefall Detection enabled</td></tr><tr><td>1:0</td><td>DMP_ODR</td><td>00: 25Hz
01: 500Hz
10: 50Hz
11: 100Hz</td></tr></table>

# 14.38 SMD_CONFIG

<table><tr><td colspan="3">Name: SMD_CONFIG
Address: 87 (57h)
Serial IF: R/W
Reset value: 0x00
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:4</td><td>-</td><td>Reserved</td></tr><tr><td>3</td><td>WOM_INT_MODE</td><td>0: Set WoM interrupt on the OR of all enabled accelerometer thresholds
1: Set WoM interrupt on the AND of all enabled accelerometer threshold</td></tr><tr><td>2</td><td>WOM_MODE</td><td>0: Initial sample is stored. Future samples are compared to initial sample
1: Compare current sample to previous sample</td></tr><tr><td>1:0</td><td>SMD_MODE</td><td>00: SMD disabled
01: WOM mode
10: SMD short (1 sec wait) An SMD event is detected when two WOM are detected 1 sec apart
11: SMD long (3 sec wait) An SMD event is detected when two WOM are detected 3 sec apart</td></tr></table>

# 14.39 FIFO_CONFIG1

<table><tr><td colspan="3">Name: FIFO_CONFIG1
Address: 95 (5Fh)
Serial IF: R/W
Reset value: 0x00
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7</td><td>-</td><td>Reserved</td></tr><tr><td>6</td><td>FIFO_RESUME_PARTIAL_RD</td><td>0: Partial FIFO read disabled, requires re-reading of the entire FIFO
1: FIFO read can be partial, and resume from last read point</td></tr><tr><td>5</td><td>FIFO_WM_QT_TH</td><td>Trigger FIFO watermark interrupt on every ODR (DMA write) if FIFO_COUNT ≥ FIFO_WM_TH</td></tr><tr><td>4</td><td>FIFO_HIRES_EN</td><td>0: Default setting; Sensor data have regular resolution
1: Sensor data in FIFO will have extended resolution enabling the 20 Bytes packet</td></tr><tr><td>3</td><td>FIFO_TMST_FSYNC_EN</td><td>Must be set to 1 for all FIFO use cases when FSYNC is used</td></tr><tr><td>2</td><td>FIFO_TEMP_EN</td><td>Enable temperature sensor packets to go to FIFO</td></tr><tr><td>1:0</td><td>FIFO_PACKET_EN</td><td>00: Disable packet goes to FIFO
01: Enable Packet 1 goes to FIFO
10: Reserved
11: Enable Packet 2 goes to FIFO when FIFO_HIRES_EN=0
Enable Packet 3 goes to FIFO when FIFO_HIRES_EN=1</td></tr></table>

# 14.40 FIFO_CONFIG2

<table><tr><td colspan="3">Name: FIFO_CONFIG2
Address: 96 (60h)
Serial IF: R/W
Reset value: 0x00
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:0</td><td>FIFO_WM[7:0]</td><td>Lower bits of FIFO watermark. Generate interrupt when the FIFO reaches or exceeds FIFO_WM size in bytes or records according to FIFO_COUNT_REC setting. FIFO_WM_EN must be zero before writing this register. Interrupt only fires once. This register should be set to non-zero value, before choosing this interrupt source.</td></tr></table>

# 14.41 FIFO_CONFIG3

<table><tr><td colspan="3">Name: FIFO_CONFIG3
Address: 97 (61h)
Serial IF: R/W
Reset value: 0x00
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:4</td><td>-</td><td>Reserved</td></tr><tr><td>3:0</td><td>FIFO_WM[11:8]</td><td>Upper bits of FIFO watermark. Generate interrupt when the FIFO reaches or exceeds FIFO_WM size in bytes or records according to FIFO_COUNT_REC setting. FIFO_WM_EN must be zero before writing this register. Interrupt only fires once. This register should be set to non-zero value, before choosing this interrupt source.</td></tr></table>

Note:Do not set FIFO_WM to value 0.

# 14.42FSYNC_CONFIG

<table><tr><td colspan="3">Name: FSYNC_CONFIG
Address: 98 (62h)
Serial IF: R/W
Reset value: 0x10
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7</td><td>-</td><td>Reserved</td></tr><tr><td>6:4</td><td>FSYNC_UI_SEL</td><td>000: Do not tag FSYNC flag
001: Tag FSYNC flag to TEMP_OUT LSB
101: Tag FSYNC flag to ACCEL_XOUT LSB
110: Tag FSYNC flag to ACCEL_YOUT LSB
111: Tag FSYNC flag to ACCEL_ZOUT LSB</td></tr><tr><td>3:2</td><td>-</td><td>Reserved</td></tr><tr><td>1</td><td>FSYNC_UI_FLAG_CLEAR_SEL</td><td>0: FSYNC flag is cleared when UI sensor register is updated
1: FSYNC flag is cleared when UI interface reads the sensor register LSB of FSYNC tagged axis</td></tr><tr><td>0</td><td>FSYNC_POLARITY</td><td>0: Start from Rising edge of FSYNC pulse to measure FSYNC interval
1: Start from Falling edge of FSYNC pulse to measure FSYNC interval</td></tr></table>

# 14.43 INT_CONFIGO

<table><tr><td colspan="3">Name: INT_CONFIGO
Address: 99 (63h)
Serial IF: R/W
Reset value: 0x00
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:6</td><td>-</td><td>Reserved</td></tr><tr><td>5:4</td><td>UI_DRDY_INT_CLEAR</td><td>Data Ready Interrupt Clear Option (latched mode)
00: Clear on Status Bit Read (default)
01: Clear on Status Bit Read
10: Clear on Sensor Register Read
11: Clear on Status Bit Read AND on Sensor Register read</td></tr><tr><td>3:2</td><td>FIFO_THS_INT_CLEAR</td><td>FIFO Threshold Interrupt Clear Option (latched mode)
00: Clear on Status Bit Read (default)
01: Clear on Status Bit Read
10: Clear on FIFO data 1Byte Read
11: Clear on Status Bit Read AND on FIFO data 1 byte read</td></tr><tr><td>1:0</td><td>FIFO_FULL_INT_CLEAR</td><td>FIFO Full Interrupt Clear Option (latched mode)
00: Clear on Status Bit Read (default)
01: Clear on Status Bit Read
10: Clear on FIFO data 1Byte Read
11: Clear on Status Bit Read AND on FIFO data 1 byte read</td></tr></table>

# 14.44 INT_CONFIG1

<table><tr><td colspan="3">Name: INT_CONFIG1
Address: 100 (64h)
Serial IF: R/W
Reset value: 0x10
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7</td><td>-</td><td>Reserved</td></tr><tr><td>6</td><td>INT TPULSE_DURATION</td><td>Interrupt pulse duration
0: Interrupt pulse duration is 100μs. Use only if ODR &amp;lt; 4kHz. (Default)
1: Interrupt pulse duration is 8μs. Required if ODR ≥ 4kHz, optional for ODR &amp;lt; 4kHz.</td></tr><tr><td>5</td><td>INT TDEASSERT_DISABLE</td><td>Interrupt de-assertion duration
0: The interrupt de-assertion duration is set to a minimum of 100μs. Use only if ODR &amp;lt; 4kHz. (Default)
1: Disables de-assertion duration. Required if ODR ≥ 4kHz, optional for ODR &amp;lt; 4kHz.</td></tr><tr><td>4</td><td>INT ASYNC_RESET</td><td>User should change setting to 0 from default setting of 1, for proper INT1 and INT2 pin operation</td></tr><tr><td>3:0</td><td>-</td><td>Reserved</td></tr></table>

# 14.45 INT_SOURCEO

<table><tr><td colspan="3">Name: INT_SOURCE0
Address: 101 (65h)
Serial IF: R/W
Reset value: 0x10
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7</td><td>-</td><td>Reserved</td></tr><tr><td>6</td><td>UI_FSYNC_INT1_EN</td><td>0: UI FSYNC interrupt not routed to INT1
1: UI FSYNC interrupt routed to INT1</td></tr><tr><td>5</td><td>-</td><td>Reserved</td></tr><tr><td>4</td><td>RESET_DONE_INT1_EN</td><td>0: Reset done interrupt not routed to INT1
1: Reset done interrupt routed to INT1</td></tr><tr><td>3</td><td>UI_DRDY_INT1_EN</td><td>0: UI data ready interrupt not routed to INT1
1: UI data ready interrupt routed to INT1</td></tr><tr><td>2</td><td>FIFO_THS_INT1_EN</td><td>0: FIFO threshold interrupt not routed to INT1
1: FIFO threshold interrupt routed to INT1</td></tr><tr><td>1</td><td>FIFO_FULL_INT1_EN</td><td>0: FIFO full interrupt not routed to INT1
1: FIFO full interrupt routed to INT1</td></tr><tr><td>0</td><td>UI_AGC_RDY_INT1_EN</td><td>0: UI AGC ready interrupt not routed to INT1
1: UI AGC ready interrupt routed to INT1</td></tr></table>

# 14.46 INT_SOURCE1

<table><tr><td colspan="3">Name: INT_SOURCE1
Address: 102 (66h)
Serial IF: R/W
Reset value: 0x00
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7</td><td>-</td><td>Reserved</td></tr><tr><td>6</td><td>I3C_PROTOCOL_ERROR_INT1_EN</td><td>0: I3CS^ protocol error interrupt not routed to INT1
1: I3CS^ protocol error interrupt routed to INT1</td></tr><tr><td>5:4</td><td>-</td><td>Reserved</td></tr><tr><td>3</td><td>SMD_INT1_EN</td><td>0: SMD interrupt not routed to INT1
1: SMD interrupt routed to INT1</td></tr><tr><td>2</td><td>WOM_Z_INT1_EN</td><td>0: Z-axis WOM interrupt not routed to INT1
1: Z-axis WOM interrupt routed to INT1</td></tr><tr><td>1</td><td>WOM_Y_INT1_EN</td><td>0: Y-axis WOM interrupt not routed to INT1
1: Y-axis WOM interrupt routed to INT1</td></tr><tr><td>0</td><td>WOM_X_INT1_EN</td><td>0: X-axis WOM interrupt not routed to INT1
1: X-axis WOM interrupt routed to INT1</td></tr></table>

# 14.47 INT_SOURCES

<table><tr><td colspan="3">Name: INT_SOURCE3</td></tr><tr><td colspan="3">Address: 104 (68h)</td></tr><tr><td colspan="3">Serial IF: R/W</td></tr><tr><td colspan="3">Reset value: 0x00</td></tr><tr><td colspan="3">Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7</td><td>-</td><td>Reserved</td></tr><tr><td>6</td><td>UI_FSYNC_INT2_EN</td><td>0: UI FSYNC interrupt not routed to INT2
1: UI FSYNC interrupt routed to INT2</td></tr><tr><td>5</td><td>-</td><td>Reserved</td></tr><tr><td>4</td><td>RESET_DONE_INT2_EN</td><td>0: Reset done interrupt not routed to INT2
1: Reset done interrupt routed to INT2</td></tr><tr><td>3</td><td>UI_DRDY_INT2_EN</td><td>0: UI data ready interrupt not routed to INT2
1: UI data ready interrupt routed to INT2</td></tr><tr><td>2</td><td>FIFO_THS_INT2_EN</td><td>0: FIFO threshold interrupt not routed to INT2
1: FIFO threshold interrupt routed to INT2</td></tr><tr><td>1</td><td>FIFO_FULL_INT2_EN</td><td>0: FIFO full interrupt not routed to INT2
1: FIFO full interrupt routed to INT2</td></tr><tr><td>0</td><td>UI_AGC_RDY_INT2_EN</td><td>0: UI AGC ready interrupt not routed to INT2
1: UI AGC ready interrupt routed to INT2</td></tr></table>

# 14.48 INT_SOURCE4

<table><tr><td colspan="3">Name: INT_SOURCE4</td></tr><tr><td colspan="3">Address: 105 (69h)</td></tr><tr><td colspan="3">Serial IF: R/W</td></tr><tr><td colspan="3">Reset value: 0x00</td></tr><tr><td colspan="3">Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7</td><td>-</td><td>Reserved</td></tr><tr><td>6</td><td>I3C PROTOCOL_ERROR_INT2
EN</td><td>0: I3CSM protocol error interrupt not routed to INT2
1: I3CSM protocol error interrupt routed to INT2</td></tr><tr><td>5:4</td><td>-</td><td>Reserved</td></tr><tr><td>3</td><td>SMD_INT2_EN</td><td>0: SMD interrupt not routed to INT2
1: SMD interrupt routed to INT2</td></tr><tr><td>2</td><td>WOM_Z_INT2_EN</td><td>0: Z-axis WOM interrupt not routed to INT2
1: Z-axis WOM interrupt routed to INT2</td></tr><tr><td>1</td><td>WOM_Y_INT2_EN</td><td>0: Y-axis WOM interrupt not routed to INT2
1: Y-axis WOM interrupt routed to INT2</td></tr><tr><td>0</td><td>WOM_X_INT2_EN</td><td>0: X-axis WOM interrupt not routed to INT2
1: X-axis WOM interrupt routed to INT2</td></tr></table>

# 14.49 FIFO_LOST_PKT0

<table><tr><td colspan="3">Name: FIFO_LOST_PKT0</td></tr><tr><td colspan="3">Address: 108 (6Cb)</td></tr><tr><td colspan="3">Serial IF: R</td></tr><tr><td colspan="3">Reset value: 0x00</td></tr><tr><td colspan="3">Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:0</td><td>FIFO_LOST_PKT_CNT[7:0]</td><td>Low byte, number of packets lost in the FIFO</td></tr></table>

# 14.50 FIFO_LOST_PKT1

<table><tr><td colspan="3">Name: FIFO_LOST_PKT1</td></tr><tr><td colspan="3">Address: 109 (6Dh)</td></tr><tr><td colspan="3">Serial IF: R</td></tr><tr><td colspan="3">Reset value: 0x00</td></tr><tr><td colspan="3">Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:0</td><td>FIFO_LOST_PKT_CNT[15:8]</td><td>High byte, number of packets lost in the FIFO</td></tr></table>

# 14.51 SELF_TEST_CONFIG

<table><tr><td colspan="3">Name: SELF_TEST_CONFIG</td></tr><tr><td colspan="3">Address: 112 (70h)</td></tr><tr><td colspan="3">Serial IF: R/W</td></tr><tr><td colspan="3">Reset value: 0x00</td></tr><tr><td colspan="3">Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7</td><td>-</td><td>Reserved</td></tr><tr><td>6</td><td>ACCEL_ST_POWER</td><td>Set to 1 for accel self-test
Otherwise set to 0; Set to 0 after self-test is completed</td></tr><tr><td>5</td><td>EN_AZ_ST</td><td>Enable Z-accel self-test</td></tr><tr><td>4</td><td>EN_AY_ST</td><td>Enable Y-accel self-test</td></tr><tr><td>3</td><td>EN_AX_ST</td><td>Enable X-accel self-test</td></tr><tr><td>2</td><td>Reserved</td><td>0</td></tr><tr><td>1</td><td>Reserved</td><td>0</td></tr><tr><td>0</td><td>Reserved</td><td>0</td></tr></table>

# 14.52 WHO_AM_I

<table><tr><td colspan="3">Name: WHO_AM_I</td></tr><tr><td colspan="3">Address: 117 (75h)</td></tr><tr><td colspan="3">Serial IF: R</td></tr><tr><td colspan="3">Reset value: 0x6D</td></tr><tr><td colspan="3">Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:0</td><td>WHOAMI</td><td>Register to indicate to user which device is being accessed</td></tr></table>

This register is used to verify the identity of the device. The contents of WHOAMI is an 8- bit device ID. The default value of the register is  $0\times 60$  . This is different from the  $1^{2}C$  address of the device as seen on the slave  $1^{2}C$  controller by the applications processor.

# 14.53 REG_BANK_SEL

Note: This register is accessible from all register banks

<table><tr><td colspan="3">Name: REG_BANK_SEL</td></tr><tr><td colspan="3">Address: 118 (76h)</td></tr><tr><td colspan="3">Serial IF: R/W</td></tr><tr><td colspan="3">Reset value: 0x00</td></tr><tr><td colspan="3">Clock Domain: ALL</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:3</td><td>-</td><td>Reserved</td></tr><tr><td>2:0</td><td>BANK_SEL</td><td>Register bank selection
000: Bank 0 (default)
001: Bank 1
010: Bank 2
011: Bank 3
100: Bank 4
101: Reserved
110: Reserved
111: Reserved</td></tr></table>

# 15 USER BANK 1 REGISTER MAP - DESCRIPTIONS

This section describes the function and contents of each register within USR Bank 1.

# 15.1 SENSOR_CONFIGO

<table><tr><td colspan="3">Name: SENSOR_CONFIGO
Address: 03 (03h)
Serial IF: R/W
Reset value: 0xB8
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:3</td><td>-</td><td>Reserved</td></tr><tr><td>2</td><td>ZA_DISABLE</td><td>0: Z accelerometer is on
1: Z accelerometer is disabled</td></tr><tr><td>1</td><td>YA_DISABLE</td><td>0: Y accelerometer is on
1: Y accelerometer is disabled</td></tr><tr><td>0</td><td>XA_DISABLE</td><td>0: X accelerometer is on
1: X accelerometer is disabled</td></tr></table>

# 15.2 TMSTVALO

<table><tr><td colspan="3">Name: TMSTVALO
Address: 98 (62h)
Serial IF: R
Reset value: 0x00
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:0</td><td>TMST_VALUE[7:0]</td><td>When TMST_STROBE is programmed, the current value of the internal counter is latched to this register. Allows the full 20-bit precision of the time stamp to be read back.</td></tr></table>

# 15.3 TMSTVAL1

<table><tr><td colspan="3">Name: TMSTVAL1
Address: 99 (63h)
Serial IF: R
Reset value: 0x00
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:0</td><td>TMST_VALUE[15:8]</td><td>When TMST_STROBE is programmed, the current value of the internal counter is latched to this register. Allows the full 20-bit precision of the time stamp to be read back.</td></tr></table>

# 15.4 TMSTVAL2

<table><tr><td colspan="3">Name: TMSTVAL2
Address: 100 (64h)
Serial IF: R
Reset value: 0x00
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:4</td><td>-</td><td>Reserved</td></tr><tr><td>3:0</td><td>TMST_VALUE[19:16]</td><td>When TMST_STROBE is programmed, the current value of the internal counter is latched to this register. Allows the full 20-bit precision of the time stamp to be read back.</td></tr></table>

# 15.5 INTF_CONFIG4

<table><tr><td colspan="3">Name: INTF_CONFIG4
Address: 122 (7Ah)
Serial IF: R/W
Reset value: 0x03
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7</td><td>-</td><td>Reserved</td></tr><tr><td>6</td><td>I3C_BUS_MODE</td><td>0: Device is on a bus with I2C and I3CSM devices
1: Device is on a bus with I3CSM devices only</td></tr><tr><td>5:2</td><td>-</td><td>Reserved</td></tr><tr><td>1</td><td>SPI_AP_4WIRE</td><td>0: AP interface uses 8-wire SPI mode
1: AP interface uses 4-wire SPI mode (default)</td></tr><tr><td>0</td><td>-</td><td>Reserved</td></tr></table>

# 15.6 INTF_CONFIG5

<table><tr><td colspan="3">Name: INTF_CONFIG5
Address: 123 (7Bh)
Serial IF: R/W
Reset value: 0x20
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:3</td><td>-</td><td>Reserved</td></tr><tr><td>2:1</td><td>PIN9_FUNCTION</td><td>Selects among the following functionalities for pin 9
00: INT2
01: FSYNC
10: CLKIN
11: Reserved</td></tr><tr><td>0</td><td>-</td><td>Reserved</td></tr></table>

# 15.7 INTF_CONFIG6

<table><tr><td colspan="3">Name: INTF_CONFIG6
Address: 124 (7Ch)
Serial IF: R/W
Reset value: 0x5F
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:5</td><td>-</td><td>Reserved</td></tr><tr><td>4</td><td>I3C_EN</td><td>0: I3CSM slave not enabled
1: I3CSM slave enabled</td></tr><tr><td>3</td><td>I3C_IBI_BYTE_EN</td><td>0: I3CSM IBI payload function not enabled
1: I3CSM IBI payload function enabled</td></tr><tr><td>2</td><td>I3C_IBI_EN</td><td>0: I3CSM IBI function not enabled
1: I3CSM IBI function enabled</td></tr><tr><td>1</td><td>I3C_DDR_EN</td><td>0: I3CSM DDR mode not enabled
1: I3CSM DDR mode enabled</td></tr><tr><td>0</td><td>I3C_SDR_EN</td><td>0: I3CSM SDR mode not enabled
1: I3CSM SDR mode enabled</td></tr></table>

# 16 USER BANK 2 REGISTER MAP - DESCRIPTIONS

This section describes the function and contents of each register within USR Bank 2.

# 16.1 ACCEL CONFIG_STATIC2

<table><tr><td colspan="3">Name: ACCEL_CONFIG_STATIC2
Address: 03 (03h)
Serial IF: R/W
Reset value: 0x7E
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7</td><td>-</td><td>Reserved</td></tr><tr><td>6:1</td><td>ACCEL_AAF_DELT</td><td>Controls bandwidth of the accelerometer anti-alias filter
See section 5.2 for details</td></tr><tr><td>0</td><td>ACCEL_AAF_DIS</td><td>0: Enable accelerometer anti-aliasing filter
1: Disable accelerometer anti-aliasing filter</td></tr></table>

Refer to Note 6 under Table 1 for ACCEL_AAF_DIS and ACCEL_AAF_DELT recommended settings to achieve high 3dB value for accelerometer signal bandwidth.

# 16.2 ACCEL CONFIG_STATIC3

<table><tr><td colspan="3">Name: ACCEL_CONFIG_STATIC3
Address: 04 (04h)
Serial IF: R/W
Reset value: 0x80
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:0</td><td>ACCEL_AAF_DELTSQR[7:0]</td><td>Controls bandwidth of the accelerometer anti-alias filter
See section 5.2 for details</td></tr></table>

Refer to Note 6 under Table 1 for ACCEL_AAF_DELTSQR recommended setting to achieve high 3dB value for accelerometer signal bandwidth.

# 16.3 ACCEL CONFIG_STATIC4

<table><tr><td colspan="3">Name: ACCEL_CONFIG_STATIC4
Address: 05 (05h)
Serial IF: R/W
Reset value: 0x3F
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:4</td><td>ACCEL_AAF_BITSHIFT</td><td>Controls bandwidth of the accelerometer anti-alias filter
See section 5.2 for details</td></tr><tr><td>3:0</td><td>ACCEL_AAF_DELTSQR[11:8]</td><td>Controls bandwidth of the accelerometer anti-alias filter
See section 5.2 for details</td></tr></table>

Refer to Note 6 under Table 1 for ACCEL_AAF_BITSHIFT and ACCEL_AAF_DELTSQR recommended setting to achieve high 3dB value for accelerometer signal bandwidth.

# 16.4 XA_ST_DATA

<table><tr><td colspan="3">Name: XA_ST_DATA
Address: 59 (3Bh)
Serial IF: R/W
Reset value: 0xXX (The value in this register indicates the self-test output generated during manufacturing tests)</td></tr><tr><td colspan="3">Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:0</td><td>XA_ST_DATA</td><td>X-accel self-test data</td></tr></table>

# 16.5 YA_ST_DATA

<table><tr><td colspan="3">Name: YA_ST_DATA
Address: 60 (3Ch)
Serial IF: R/W
Reset value: 0xXX (The value in this register indicates the self-test output generated during manufacturing tests)
Clock Domain: SCLK, UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:0</td><td>YA_ST_DATA</td><td>Y-accel self-test data</td></tr></table>

# 16.6 za_St_DATA

<table><tr><td colspan="3">Name: ZA_ST_DATA
Address: 61 (3Dh)
Serial IF: R/W
Reset value: 0xXX (The value in this register indicates the self-test output generated during manufacturing tests)
Clock Domain: SCLK, UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:0</td><td>ZA_ST_DATA</td><td>Z-accel self-test data</td></tr></table>

# 17 USER BANK 3 REGISTER MAP - DESCRIPTIONS

This section describes the function and contents of each register within USR Bank 3.

# 17.1 PU_PD_CONFIG1

<table><tr><td colspan="3">Name: PU_PD_CONFIG1
Address: 06 (06h)
Serial IF: R/W
Reset value: 0x88
Clock Domain: SCLK, UI</td></tr><tr><td>BIT</td><td>NAME</td><td>PUNCTION</td></tr><tr><td>7</td><td>PIN11_PU_EN</td><td>Pull-up control for pin 11
0: Pull-up is disabled
1: Pull-up is enabled</td></tr><tr><td>6</td><td>PIN7_PU_EN</td><td>Pull-up control for pin 7 if triple interface mode is used. Must be set to 0 if single/dual interface mode is used.
0: Pull-up is disabled
1: Pull-up is enabled</td></tr><tr><td>5</td><td>-</td><td>Reserved</td></tr><tr><td>4</td><td>PIN9_PD_EN</td><td>Pull-down control for pin 9 in single/dual interface mode. Must be set to 0 if triple interface mode is used.
0: Pull-down is disabled
1: Pull-down is enabled</td></tr><tr><td>3</td><td>PIN10_PU_EN</td><td>Pull-up control for pin 10
0: Pull-up is disabled
1: Pull-up is enabled</td></tr><tr><td>2</td><td>PIN3_PU_EN</td><td>Pull-up control for pin 3
0: Pull-up is disabled
1: Pull-up is enabled</td></tr><tr><td>1</td><td>PIN2_PU_EN</td><td>Pull-up control for pin 2
0: Pull-up is disabled
1: Pull-up is enabled</td></tr><tr><td>0</td><td>PIN4_PD_EN</td><td>Pull-down control for pin 4
0: Pull-down is disabled
1: Pull-down is enabled</td></tr></table>

# 17.2 PU_PD_CONFIG2

<table><tr><td colspan="3">Name: PU_PD_CONFIG2
Address: 14 (0Eh)
Serial IF: R/W
Reset value: 0x20
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7</td><td>PIN1_PU_EN</td><td>Pull-up control for pin 1. See bit 6 description for pull-down control for pin 1. Note that both pull-up and pull-down must not be simultaneously enabled for the same pin.
0: Pull-up is disabled
1: Pull-up is enabled</td></tr><tr><td>6</td><td>PIN1_PD_EN</td><td>Pull-down control for pin 1. See bit 7 description for pull-up control for pin 1. Note that both pull-up and pull-down must not be simultaneously enabled for the same pin.
0: Pull-down is disabled
1: Pull-down is enabled</td></tr><tr><td>5</td><td>PIN12_PU_EN</td><td>Pull-up control for pin 12. See bit 4 description for pull-down control for pin 12. Note that both pull-up and pull-down must not be simultaneously enabled for the same pin.
0: Pull-up is disabled
1: Pull-up is enabled</td></tr><tr><td>4</td><td>PIN12_PD_EN</td><td>Pull-down control for pin 12. See bit 5 description for pull-up control for pin 12. Note that both pull-up and pull-down must not be simultaneously enabled for the same pin.
0: Pull-down is disabled
1: Pull-down is enabled</td></tr><tr><td>3</td><td>PIN14_PU_EN</td><td>Pull-up control for pin 14. See bit 2 description for pull-down control for pin 14. Note that both pull-up and pull-down must not be simultaneously enabled for the same pin.
0: Pull-up is disabled
1: Pull-up is enabled</td></tr><tr><td>2</td><td>PIN14_PD_EN</td><td>Pull-down control for pin 14. See bit 3 description for pull-up control for pin 14. Note that both pull-up and pull-down must not be simultaneously enabled for the same pin.
0: Pull-down is disabled
1: Pull-down is enabled</td></tr><tr><td>1</td><td>PIN13_PU_EN</td><td>Pull-up control for pin 13. See bit 0 description for pull-down control for pin 13. Note that both pull-up and pull-down must not be simultaneously enabled for the same pin.
0: Pull-up is disabled
1: Pull-up is enabled</td></tr><tr><td>0</td><td>PIN13_PD_EN</td><td>Pull-down control for pin 13. See bit 1 description for pull-up control for pin 13. Note that both pull-up and pull-down must not be simultaneously enabled for the same pin.
0: Pull-down is disabled
1: Pull-down is enabled</td></tr></table>

# 18 USER BANK 4 REGISTER MAP - DESCRIPTIONS

This section describes the function and contents of each register within USR Bank 4.

# 18.1 FDR_CONFIG

<table><tr><td colspan="3">Name: FDR_CONFIG
Address: 09h
Serial IF: R/W
Reset value: 0x00
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7</td><td>-</td><td>Reserved</td></tr><tr><td>6:0</td><td>FDR_SEL</td><td>FIFO packet rate decimation factor. Sets the number of discarded FIFO packets.
Valid range is 0 to 127. User must disable sensors when initializing FDR_SEL value or making changes to it.
0000000: Decimation is disabled, all packets are sent to FIFO
0000001: 1 packet out of 2 is sent to FIFO
0000010: 1 packet out of 3 is sent to FIFO
0000011: 1 packet out of 4 is sent to FIFO
... 
1111111: 1 packet out of 128 is sent to FIFO</td></tr></table>

# 18.2 APEX_CONFIG1

<table><tr><td colspan="3">Name: APEX_CONFIG1
Address: 64 (40h)
Serial IF: R/W
Reset value: 0xA2
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:4</td><td>LOW_ENERGY_AMP_TH_SEL</td><td>Pedometer Low Energy mode amplitude threshold selection
Use default value 1010b</td></tr><tr><td>3:0</td><td>DMP_POWER_SAVE_TIME_SEL</td><td>When the DMP is in power save mode, it is awakened by the WOM and will wait for a certain duration before going back to sleep. This bitfield configures this duration.
0000: 0 seconds
0001: 4 seconds
0010: 8 seconds (default)
0011: 12 seconds
0100: 16 seconds
0101: 20 seconds
0110: 24 seconds
0111: 28 seconds
1000: 32 seconds
1001: 36 seconds
1010: 40 seconds
1011: 44 seconds
1100: 48 seconds
1101: 52 seconds
1110: 56 seconds
1111: 60 seconds</td></tr></table>

# 18.3 APEX_CONFIG2

<table><tr><td colspan="3">Name: APEX_CONFIG2
Address: 65 (41h)
Serial IF: R/W
Reset value: 0x85
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:4</td><td>PED_AMP_TH_SEL</td><td>Pedometer amplitude threshold selection
Use default value 1000b</td></tr><tr><td>3:0</td><td>PED_STEP_CNT_TH_SEL</td><td>Pedometer step count detection window
Use default value 0101b
0000: 0 steps
0001: 1 step
0010: 2 steps
0011: 3 steps
0100: 4 steps
0101: 5 steps (default)
0110: 6 steps
0111: 7 steps
1000: 8 steps
1001: 9 steps
1010: 10 steps
1011: 11 steps
1100: 12 steps
1101: 13 steps
1110: 14 steps
1111: 15 steps</td></tr></table>

# 18.4 APEX_CONFIG3

<table><tr><td colspan="3">Name: APEX_CONFIG3
Address: 66 (42h)
Serial IF: R/W
Reset value: 0x51
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:5</td><td>PED_STEP_DET_TH_SEL</td><td>Pedometer step detection threshold selection
Use default value 010b
000: 0 steps
001: 1 step
010: 2 steps (default)
011: 3 steps
100: 4 steps
101: 5 steps
110: 6 steps
111: 7 steps</td></tr><tr><td>4:2</td><td>PED_SB_TIMER_TH_SEL</td><td>Pedometer step buffer timer threshold selection
Use default value 100b
000: 0 samples
001: 1 sample
010: 2 samples
011: 3 samples
100: 4 samples (default)
101: 5 samples
110: 6 samples
111: 7 samples</td></tr><tr><td>1:0</td><td>PED_HI_EN_TH_SEL</td><td>Pedometer high energy threshold selection
Use default value 01b</td></tr></table>

# 18.5 APEX_CONFIG4

<table><tr><td colspan="3">Name: APEX_CONFIG4
Address: 67 (43h)
Serial IF: R/W
Reset value: 0xA4
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:6</td><td>TILT_WAIT_TIME_SEL</td><td>Configures duration of delay after tilt is detected before interrupt is triggered
00: 0s
01: 2s
10: 4s (default)
11: 6s</td></tr><tr><td>5:3</td><td>LOWG_PEAR_TH_HYST_SEL</td><td>This threshold is added to the LOWG peak threshold after the initial threshold is met. The threshold values corresponding to parameter values are shown below:
000: 31mgee
001: 63mgee
010: 94mgee
011: 125mgee
100: 156mgee (default)
101: 188mgee
110: 219mgee
111: 250mgee</td></tr><tr><td>2:0</td><td>HIGHG_PEAR_TH_HYST_SEL</td><td>This threshold is added to the HIGHG peak threshold after the initial threshold is met. The threshold values corresponding to parameter values are shown below:
000: 31mgee
001: 63mgee
010: 94mgee
011: 125mgee
100: 156mgee (default)
101: 188mgee
110: 219mgee
110: 250mgee</td></tr></table>

# 18.6 APEX_CONFIG5

<table><tr><td colspan="3">Name: APEX_CONFIG5
Address: 68 (44h)
Serial IF: R/W
Reset value: 0x8C
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:3</td><td>LOWG_PEAH_TH_SEL</td><td>This parameter defines the threshold for accelerometer values below which the algorithm considers it has entered low-g state. The threshold values corresponding to parameter values are shown below:
00000: 31mgee
00001: 63mgee
00010: 94mgee
00011: 125mgee
00100: 156mgee
00101: 188mgee
00110: 219mgee
00111: 250mgee
01000: 281mgee
01001: 313mgee
01010: 344mgee
01011: 375mgee
01100: 406mgee
01101: 438mgee
01110: 469mgee
01111: 500mgee
10000: 531mgee
10001: 563mgee (default)
10010: 594mgee
10011: 625mgee
10100: 656mgee
10101: 688mgee
10110: 719mgee
10111: 750mgee
11000: 781mgee
11001: 813mgee
11010: 844mgee
11011: 875mgee
11100: 906mgee
11101: 938mgee
11110: 969mgee
11111: 1000mgee</td></tr><tr><td>2:0</td><td>LOWG_TIME_TH_SEL</td><td>This parameter defines the number of samples for which the device should stay in low-g before triggering interrupt.
Number of samples = LOWG_TIME_TH_SEL + 1
Default value is 4 (i.e. 5 samples)</td></tr></table>

# 18.7 APEX_CONFIG6

<table><tr><td colspan="3">Name: APEX_CONFIG6
Address: 69 (45h)
Serial IF: R/W
Reset value: 0x5C
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:3</td><td>HIGHG_PEAK_TH_SEL</td><td>This parameter defines the threshold for accelerometer values above which the algorithm considers it has entered high-g state. The threshold values corresponding to parameter values are shown below:
00000: 250mgee
00001: 500mgee
00010: 750mgee
00011: 1000mgee
00100: 1250mgee
00101: 1500mgee
00110: 1750mgee
00111: 2000mgee
01000: 2250mgee
01001: 2500mgee
01010: 2750mgee
01011: 3000mgee (default)
01100: 3250mgee
01101: 3500mgee
01110: 3750mgee
01111: 4000mgee
10000: 4250mgee
10001: 4500mgee
10010: 4750mgee
10011: 5000mgee
10100: 5250mgee
10101: 5500mgee
10110: 5750mgee
10111: 6000mgee
11000: 6250mgee
11001: 6500mgee
11010: 6750mgee
11011: 7000mgee
11100: 7250mgee
11101: 7500mgee
11110: 7750mgee
11111: 8000mgee</td></tr><tr><td>2:0</td><td>HIGHG_TIME_TH_SEL</td><td>This parameter defines the number of samples for which the device should stay in high-g before triggering interrupt.
Number of samples = HIGHG_TIME_TH_SEL + 1
Default value is 4 (i.e. 5 samples)</td></tr></table>

# 18.8 APEX_CONFIG7

<table><tr><td colspan="3">Name: APEX_CONFIG7
Address: 70 (46h)
Serial IF: R/W
Reset value: 0x45
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:2</td><td>TAP_MIN_JERK_THR</td><td>Tap Detection minimum jerk threshold
Use default value 010001b</td></tr><tr><td>1:0</td><td>TAP_MAX_PEAK_TOI</td><td>Tap Detection maximum peak tolerance
Use default value 01b</td></tr></table>

# 18.9 APEX_CONFIG8

<table><tr><td colspan="3">Name: APEX_CONFIG8
Address: 71 (47h)
Serial IF: R/W
Reset value: 0x5B
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7</td><td>-</td><td>Reserved</td></tr><tr><td>6:5</td><td>TAP_TMAX</td><td>Tap measurement window (number of samples)
Use default value 10b</td></tr><tr><td>4:3</td><td>TAP_TAVG</td><td>Tap energy measurement window (number of samples)
Use default value 11b</td></tr><tr><td>2:0</td><td>TAP_TMIN</td><td>Single tap window (number of samples)
Use default value 011b</td></tr></table>

# 18.10APEX_CONFIG9

<table><tr><td colspan="3">Name: APEX_CONFIG9
Address: 72 (48h)
Serial IF: R/W
Reset value: 0x00
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:1</td><td>-</td><td>Reserved</td></tr><tr><td>0</td><td>SENSITIVITY_MODE</td><td>0: Low power mode at accelerometer ODR 25Hz; High performance mode at accelerometer ODR ≥ 50Hz
1: Low power and slow walk mode at accelerometer ODR 25Hz; Slow walk mode at accelerometer ODR ≥ 50Hz</td></tr></table>

# 18.11APEX_CONFIG10

<table><tr><td colspan="3">Name: APEX_CONFIG10
Address: 73 (49h)
Serial IF: R/W
Reset value: 0x00
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:5</td><td>FF_MIN_DURATION_CM</td><td>This parameter defines the minimum freefall length that the algorithm should report. Freefalls smaller than this value are ignored. Freefall lengths corresponding to parameter values are shown below:
000: 13cm (default)
001: 19cm
010: 28cm
011: 38cm
100: 50cm
101: 64cm
110: 78cm
111: 95cm</td></tr><tr><td>4:2</td><td>FF_MAX_DURATION_CM</td><td>This parameter defines the maximum freefall length that the algorithm should report. Freefalls longer than this value are ignored. Freefall lengths corresponding to parameter values are shown below:
000: 113cm (default)
001: 154cm
010: 201cm
011: 255cm
100: 314cm
101: 380cm
110: 452cm
111: 531cm</td></tr><tr><td>1:0</td><td>FF_DEBounce_DURATION</td><td>This parameter defines the time during which low-g and high-g events are not taken into account after a high-g event. It helps to avoid detecting bounces as free fall. Debounce durations corresponding to parameter values are shown below:
00: 0s (default)
01: 1s
10: 2s
11: 3s</td></tr></table>

# 18.12 ACCEL_WOM_X_THR

<table><tr><td colspan="3">Name: ACCEL_WOM_X_THR
Address: 74 (4Ah)
Serial IF: R/W
Reset value: 0x00
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:0</td><td>WOM_X_TH</td><td>Threshold value for the Wake on Motion Interrupt for X-axis accelerometer
WOM thresholds are expressed in fixed “mg” independent of the selected Range [2g: 1g]; Resolution 1g/256=“3.9mg”</td></tr></table>

# 18.13 ACCEL_WOM_Y_THR

<table><tr><td colspan="3">Name: ACCEL_WOM_Y_THR
Address: 75 (4Bh)
Serial IF: R/W
Reset value: 0x00
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:0</td><td>WOM_Y_TH</td><td>Threshold value for the Wake on Motion Interrupt for Y-axis accelerometer
WoM thresholds are expressed in fixed “mg” independent of the selected Range [0g: 1g]; Resolution 1g/256= ~3.9mg</td></tr></table>

# 18.14 ACCEL_WOM_Z_THR

<table><tr><td colspan="3">Name: ACCEL_WOM_Z_THR
Address: 76 (4Ch)
Serial IF: R/W
Reset value: 0x00
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:0</td><td>WOM_Z_TH</td><td>Threshold value for the Wake on Motion Interrupt for Z-axis accelerometer
WoM thresholds are expressed in fixed “mg” independent of the selected Range [0g: 1g]; Resolution 1g/256= ~3.9mg</td></tr></table>

# 18.15 INT_SOURCE6

<table><tr><td colspan="3">Name: INT_SOURCE6
Address: 77 (4Dh)
Serial IF: R/W
Reset value: 0x00
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:6</td><td>-</td><td>Reserved</td></tr><tr><td>5</td><td>STEP_DET_INT1_EN</td><td>0: Step detect interrupt not routed to INT1
1: Step detect interrupt routed to INT1</td></tr><tr><td>4</td><td>STEP_CNT_DFL_INT1_EN</td><td>0: Step count overflow interrupt not routed to INT1
1: Step count overflow interrupt routed to INT1</td></tr><tr><td>3</td><td>TILT_DET_INT1_EN</td><td>0: Tilt detect interrupt not routed to INT1
1: Tile detect interrupt routed to INT1</td></tr><tr><td>2</td><td>-</td><td>Reserved</td></tr><tr><td>1</td><td>FREEFALL_DET_INT1_EN</td><td>0: Freefall detect interrupt not routed to INT1
1: Freefall detect interrupt routed to INT1</td></tr><tr><td>0</td><td>TAP_DET_INT1_EN</td><td>0: Tap detect interrupt not routed to INT1
1: Tap detect interrupt routed to INT1</td></tr></table>

# 18.16 INT_SOURCE7

<table><tr><td colspan="3">Name: INT_SOURCE7
Address: 78 (4Eh)
Serial IF: R/W
Reset value: 0x00
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:6</td><td>-</td><td>Reserved</td></tr><tr><td>5</td><td>STEP_DET_INT2_EN</td><td>0: Step detect interrupt not routed to INT2
1: Step detect interrupt routed to INT2</td></tr><tr><td>4</td><td>STEP_CNT_DFL_INT2_EN</td><td>0: Step count overflow interrupt not routed to INT2
1: Step count overflow interrupt routed to INT2</td></tr><tr><td>3</td><td>TILT_DET_INT2_EN</td><td>0: Tilt detect interrupt not routed to INT2
1: Tile detect interrupt routed to INT2</td></tr><tr><td>2</td><td>-</td><td>Reserved</td></tr><tr><td>1</td><td>FREEFALL_DET_INT2_EN</td><td>0: Freefall detect interrupt not routed to INT2
1: Freefall detect interrupt routed to INT2</td></tr><tr><td>0</td><td>TAP_DET_INT2_EN</td><td>0: Tap detect interrupt not routed to INT2
1: Tap detect interrupt routed to INT2</td></tr></table>

# 18.17 INT_SOURCE8

<table><tr><td colspan="3">Name: INT_SOURCE8
Address: 79 (4Fh)
Serial IF: R/W
Reset value: 0x00
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:6</td><td>-</td><td>Reserved</td></tr><tr><td>5</td><td>FSYNC_IBI_EN</td><td>0: FSYNC interrupt not routed to IBI
1: FSYNC interrupt routed to IBI</td></tr><tr><td>4</td><td>-</td><td>Reserved</td></tr><tr><td>3</td><td>UI_DRDY_IBI_EN</td><td>0: UI data ready interrupt not routed to IBI
1: UI data ready interrupt routed to IBI</td></tr><tr><td>2</td><td>FIFO_THS_IBI_EN</td><td>0: FIFO threshold interrupt not routed to IBI
1: FIFO threshold interrupt routed to IBI</td></tr><tr><td>1</td><td>FIFO_FULL_IBI_EN</td><td>0: FIFO full interrupt not routed to IBI
1: FIFO full interrupt routed to IBI</td></tr><tr><td>0</td><td>AGC_RDY_IBI_EN</td><td>0: AGC ready interrupt not routed to IBI
1: AGC ready interrupt routed to IBI</td></tr></table>

# 18.18 INT_SOURCE9

<table><tr><td colspan="3">Name: INT_SOURCE9
Address: 80 (50h)
Serial IF: R/W
Reset value: 0x00
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7</td><td>I3C_PROTOCOL_ERROR_IBI_EN</td><td>0: I3CSM protocol error interrupt not routed to IBI
1: I3CSM protocol error interrupt routed to IBI</td></tr><tr><td>6:5</td><td>-</td><td>Reserved</td></tr><tr><td>4</td><td>SMD_IBI_EN</td><td>0: SMD interrupt not routed to IBI
1: SMD interrupt routed to IBI</td></tr><tr><td>3</td><td>WOM_Z_IBI_EN</td><td>0: Z-axis WOM interrupt not routed to IBI
1: Z-axis WOM interrupt routed to IBI</td></tr><tr><td>2</td><td>WOM_Y_IBI_EN</td><td>0: Y-axis WOM interrupt not routed to IBI
1: Y-axis WOM interrupt routed to IBI</td></tr><tr><td>1</td><td>WOM_X_IBI_EN</td><td>0: X-axis WOM interrupt not routed to IBI
1: X-axis WOM interrupt routed to IBI</td></tr><tr><td>0</td><td>-</td><td>Reserved</td></tr></table>

# 18.19 INT_SOURCE10

<table><tr><td colspan="3">Name: INT_SOURCE10
Address: 81 (51h)
Serial IF: R/W
Reset value: 0x00
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:6</td><td>-</td><td>Reserved</td></tr><tr><td>5</td><td>STEP_DET_IBI_EN</td><td>0: Step detect interrupt not routed to IBI
1: Step detect interrupt routed to IBI</td></tr><tr><td>4</td><td>STEP_CNT_CFL_IBI_EN</td><td>0: Step count overflow interrupt not routed to IBI
1: Step count overflow interrupt routed to IBI</td></tr><tr><td>3</td><td>TILT_DET_IBI_EN</td><td>0: Tilt detect interrupt not routed to IBI
1: Tile detect interrupt routed to IBI</td></tr><tr><td>2</td><td>-</td><td>Reserved</td></tr><tr><td>1</td><td>FREEFALL_DET_IBI_EN</td><td>0: Freefall detect interrupt not routed to IBI
1: Freefall detect interrupt routed to IBI</td></tr><tr><td>0</td><td>TAP_DET_IBI_EN</td><td>0: Tap detect interrupt not routed to IBI
1: Tap detect interrupt routed to IBI</td></tr></table>

# 18.20 OFFSET_USER4

<table><tr><td colspan="3">Name: OFFSET_USER4
Address: 123 (7Bh)
Serial IF: R/W
Reset value: 0x00
Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:4</td><td>ACCEL_X_OFFSET[11:8]</td><td>Upper bits of X-accel offset programmed by user. Max value is ±1g, resolution is 0.5mg.</td></tr><tr><td>3:0</td><td>Reserved</td><td></td></tr></table>

# 18.21 OFFSET_USERS

<table><tr><td colspan="3">Name: OFFSET_USER5</td></tr><tr><td colspan="3">Address: 124 (7Ch)</td></tr><tr><td colspan="3">Serial IF: R/W</td></tr><tr><td colspan="3">Reset value: 0x00</td></tr><tr><td colspan="3">Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:0</td><td>ACCEL_X_OFFSET[7:0]</td><td>Lower bits of X-accel offset programmed by user. Max value is ±1g, resolution is 0.5mg.</td></tr></table>

# 18.22 OFFSET_USERS

<table><tr><td colspan="3">Name: OFFSET_USER6</td></tr><tr><td colspan="3">Address: 125 (7Dh)</td></tr><tr><td colspan="3">Serial IF: R/W</td></tr><tr><td colspan="3">Reset value: 0x00</td></tr><tr><td colspan="3">Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:0</td><td>ACCEL_Y_OFFSET[7:0]</td><td>Lower bits of Y-accel offset programmed by user. Max value is ±1g, resolution is 0.5mg.</td></tr></table>

# 18.23 OFFSET_USER7

<table><tr><td colspan="3">Name: OFFSET_USER7</td></tr><tr><td colspan="3">Address: 126 (7Eh)</td></tr><tr><td colspan="3">Serial IF: R/W</td></tr><tr><td colspan="3">Reset value: 0x00</td></tr><tr><td colspan="3">Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:4</td><td>ACCEL_Z_OFFSET[11:8]</td><td>Upper bits of Z-accel offset programmed by user. Max value is ±1g, resolution is 0.5mg.</td></tr><tr><td>3:0</td><td>ACCEL_Y_OFFSET[11:8]</td><td>Upper bits of Y-accel offset programmed by user. Max value is ±1g, resolution is 0.5mg.</td></tr></table>

# 18.24 OFFSET_USER8

<table><tr><td colspan="3">Name: OFFSET_USER8</td></tr><tr><td colspan="3">Address: 127 (7Fh)</td></tr><tr><td colspan="3">Serial IF: R/W</td></tr><tr><td colspan="3">Reset value: 0x00</td></tr><tr><td colspan="3">Clock Domain: SCLK_UI</td></tr><tr><td>BIT</td><td>NAME</td><td>FUNCTION</td></tr><tr><td>7:0</td><td>ACCEL_Z_OFFSET[7:0]</td><td>Lower bits of Z-accel offset programmed by user. Max value is ±1g, resolution is 0.5mg.</td></tr></table>

# 19 SMARTINDUSTRIAL FAMILY

TDK's SmartIndustrial™ portfolio of 6- axis IMU and 3- axis Accelerometer products delivers the precise motion, vibration and inclination measurements that industrial applications need. These products offer the ability to take precise measurements in harsh environments with vibration and wide temperature variations.

TDK's broad portfolio of Industrial Motion Sensing solutions offers customers a range of performance and cost choices, enabling a wide variety of Industrial navigation, stabilization, and monitoring applications.

By combining its innovative MEMS Motion Sensor technologies with its expertise of Industrial applications, TDK offers unique capabilities such as Fault- tolerant motion sensing solution.

# 20 REFERENCE

Please refer to "InvenSense MEMS Handling Application Note (AN- IVS- 0002A- 00)" for the following information:

Manufacturing Recommendations- Assembly Guidelines and Recommendations- PCB Design Guidelines and Recommendations- MEMS Handling Instructions- ESD Considerations- Reflow Specification- Storage Specifications- Package Marking Specification- Tape & Reel Specification- Reel & Pizza Box Label- Packaging- Representative Shipping Carton Label

# Compliance

Environmental Compliance- DRC Compliance- Compliance Declaration Disclaimer

# 21 REVISION HISTORY

<table><tr><td>REVISION DATE</td><td>REVISION</td><td>DESCRIPTION</td></tr><tr><td>01/07/2021</td><td>1.0</td><td>Initial release</td></tr><tr><td>02/25/2022</td><td>1.1</td><td>Updated formatting
Updated longevity info</td></tr><tr><td>03/29/2022</td><td>1.2</td><td>Updated table 1 MIN ODR typo, added Total RMS specification
Update notes in table 1 and table 2
Updated table 2 minor typo
Updated table 3 power supply</td></tr><tr><td>09/27/2022</td><td>1.3</td><td>Updated figure 5 with the correct Y-axis direction
Fixed tap detection address in the ACCEL_MODE from 1 to 3</td></tr><tr><td>11/17/2022</td><td>1.4</td><td>Fixed orientation of figure 5.
Fixed figure 8.
Fixed section 8.6.</td></tr></table>

![](images/369c9d72f7269c35d2569c41acd52b1a8a5f48213ad46ec4d1ba1e3e1cc3a419.jpg)

https://invensense.tdk.com/longevity/

This infomation furnished by nvenSene or its affliates (TDK nvenSene) is believed to be accurate and reliable. However, no responsibilit is assumed by TDK nvenSense for its use, or for any infringements of patents or other rights of third parties that may result from its use. Specifications are subject to change without notice. TDK nvenSense reserves the right to make changes to this product, including its circuits and software, in order to improve its design and/or performance, without prior notice. TDK nvenSense makes no warranties, neither expressed nor implied, regarding the information and specifications contained in this document. TDK nvenSense assumes no responsibility for any claims or damages arising from information contained in this document, or from the use of products and services detailed therein. This includes, but is not limited to, claims or damages based on the infringement of patents, copyrights, mask work and/or other intellectual property rights.

Certain intellectual property owned by nvenSense and described in this document is patent protected. No license is granted by implication or otherwise under any patent or patent rights of nvenSense. This publication supersedes and replaces all information previously supplied. Trademarks that are registered trademarks are the property of their respective companies. TDK nvenSense sensors should not be used or sold in the development, storage, production or utilization of any conventional or mass- destructive weapons or for any other weapons or life threatening applications, as well as in any other life critical applications such as medical equipment, transportation, aerospace and nuclear instruments, undersea equipment, power plant equipment, disaster prevention and crime prevention equipment.

$⑤2021$  2022 nvenSense. All rights reserved. nvenSense, SmartMotion, MotionProcessing, MotionProcessor, SensorStudio, UltraPrint, MotionTracking, CHIRP Microsystems, SmartBug, SonicLink, Digital Motion Processor, AAR, and the nvenSense logo are registered trademarks of nvenSense, Inc. The TDK logo is a trademark of TDK Corporation. Other company and product names may be trademarks of the respective companies with which they are associated.

# ATDK InvenSense

$①2021$  2022 InvenSense. All rights reserved.