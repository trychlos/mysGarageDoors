EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:arduinonano
EELAYER 25 0
EELAYER END
$Descr A3 16535 11693
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L ArduinoNano U1
U 1 1 5859636E
P 7250 5700
F 0 "U1" H 6900 5700 60  0000 C CNN
F 1 "ArduinoNano" H 6900 5800 60  0000 C CNN
F 2 "" H 7100 5300 60  0001 C CNN
F 3 "" H 7100 5300 60  0001 C CNN
	1    7250 5700
	1    0    0    -1  
$EndComp
$Comp
L NRF24L01 U2
U 1 1 58596512
P 1950 5900
F 0 "U2" H 1700 5850 60  0000 C CNN
F 1 "NRF24L01" H 1700 5950 60  0000 C CNN
F 2 "" H 1950 5900 60  0001 C CNN
F 3 "" H 1950 5900 60  0001 C CNN
	1    1950 5900
	0    1    1    0   
$EndComp
$Comp
L GND #PWR?
U 1 1 58597220
P 7500 9900
F 0 "#PWR?" H 7500 9650 50  0001 C CNN
F 1 "GND" H 7500 9750 50  0000 C CNN
F 2 "" H 7500 9900 50  0000 C CNN
F 3 "" H 7500 9900 50  0000 C CNN
	1    7500 9900
	1    0    0    -1  
$EndComp
Text Notes 14150 950  0    60   ~ 0
Door n° 2\nMotor +/- 24v
Text Notes 13350 10200 0    60   ~ 0
2017-03-26
$Comp
L R R6
U 1 1 58611324
P 15200 2100
F 0 "R6" V 15280 2100 50  0000 C CNN
F 1 "2,2k" V 15200 2100 50  0000 C CNN
F 2 "" V 15130 2100 50  0000 C CNN
F 3 "" H 15200 2100 50  0000 C CNN
	1    15200 2100
	1    0    0    -1  
$EndComp
Text Notes 12650 3550 0    60   ~ 0
A4
Text Notes 12650 4050 0    60   ~ 0
A5
$Comp
L R R1
U 1 1 589F4FB7
P 10850 1950
F 0 "R1" V 10930 1950 50  0000 C CNN
F 1 "4,7k" V 10850 1950 50  0000 C CNN
F 2 "" V 10780 1950 50  0000 C CNN
F 3 "" H 10850 1950 50  0000 C CNN
	1    10850 1950
	1    0    0    -1  
$EndComp
$Comp
L R R2
U 1 1 589F51D2
P 11150 1950
F 0 "R2" V 11230 1950 50  0000 C CNN
F 1 "1k" V 11150 1950 50  0000 C CNN
F 2 "" V 11080 1950 50  0000 C CNN
F 3 "" H 11150 1950 50  0000 C CNN
	1    11150 1950
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X02 P2
U 1 1 58A0C806
P 14500 1150
F 0 "P2" H 14500 1300 50  0000 C CNN
F 1 "CNX2" V 14600 1150 50  0000 C CNN
F 2 "" H 14500 1150 50  0000 C CNN
F 3 "" H 14500 1150 50  0000 C CNN
	1    14500 1150
	0    -1   -1   0   
$EndComp
$Comp
L R R4
U 1 1 58A20617
P 13750 3850
F 0 "R4" V 13830 3850 50  0000 C CNN
F 1 "10k" V 13750 3850 50  0000 C CNN
F 2 "" V 13680 3850 50  0000 C CNN
F 3 "" H 13750 3850 50  0000 C CNN
	1    13750 3850
	-1   0    0    1   
$EndComp
Text Notes 12600 6850 0    60   ~ 0
D3
$Comp
L R R8
U 1 1 58D895D0
P 14350 3300
F 0 "R8" V 14430 3300 50  0000 C CNN
F 1 "10k" V 14350 3300 50  0000 C CNN
F 2 "" V 14280 3300 50  0000 C CNN
F 3 "" H 14350 3300 50  0000 C CNN
	1    14350 3300
	-1   0    0    1   
$EndComp
$Comp
L LTV-814 U1
U 1 1 5900A3ED
P 15100 2600
F 0 "U1" H 14900 2800 50  0000 L CNN
F 1 "LTV-814" H 15100 2800 50  0000 L CNN
F 2 "DIP-4" H 14900 2400 50  0000 L CIN
F 3 "" H 15125 2600 50  0000 L CNN
	1    15100 2600
	0    1    1    0   
$EndComp
$Comp
L D D1
U 1 1 5900A4E0
P 15200 1750
F 0 "D1" H 15200 1850 50  0000 C CNN
F 1 "1N4004" H 15200 1650 50  0000 C CNN
F 2 "" H 15200 1750 50  0000 C CNN
F 3 "" H 15200 1750 50  0000 C CNN
	1    15200 1750
	0    -1   -1   0   
$EndComp
$Comp
L D D?
U 1 1 5900BF06
P 13750 1750
F 0 "D?" H 13750 1850 50  0000 C CNN
F 1 "1N4004" H 13750 1650 50  0000 C CNN
F 2 "" H 13750 1750 50  0000 C CNN
F 3 "" H 13750 1750 50  0000 C CNN
	1    13750 1750
	0    -1   -1   0   
$EndComp
$Comp
L R R3
U 1 1 5900BFB6
P 13750 2100
F 0 "R3" V 13830 2100 50  0000 C CNN
F 1 "2,2k" V 13750 2100 50  0000 C CNN
F 2 "" V 13680 2100 50  0000 C CNN
F 3 "" H 13750 2100 50  0000 C CNN
	1    13750 2100
	1    0    0    -1  
$EndComp
$Comp
L LTV-814 U2
U 1 1 5900C031
P 13850 2600
F 0 "U2" H 13650 2800 50  0000 L CNN
F 1 "LTV-814" H 13850 2800 50  0000 L CNN
F 2 "DIP-4" H 13650 2400 50  0000 L CIN
F 3 "" H 13875 2600 50  0000 L CNN
	1    13850 2600
	0    1    1    0   
$EndComp
$Comp
L CONN_01X02 P1
U 1 1 5900C449
P 11000 1500
F 0 "P1" H 11000 1650 50  0000 C CNN
F 1 "CNX2" V 11100 1500 50  0000 C CNN
F 2 "" H 11000 1500 50  0000 C CNN
F 3 "" H 11000 1500 50  0000 C CNN
	1    11000 1500
	0    -1   -1   0   
$EndComp
Text Notes 10700 1400 2    60   ~ 0
Door n° 1\nAlim +24v
$Comp
L +24V #PWR?
U 1 1 5900D257
P 10950 1300
F 0 "#PWR?" H 10950 1150 50  0001 C CNN
F 1 "+24V" H 10950 1440 50  0000 C CNN
F 2 "" H 10950 1300 50  0000 C CNN
F 3 "" H 10950 1300 50  0000 C CNN
	1    10950 1300
	1    0    0    -1  
$EndComp
$Comp
L R R7
U 1 1 5900D88D
P 15000 3350
F 0 "R7" V 15080 3350 50  0000 C CNN
F 1 "10k" V 15000 3350 50  0000 C CNN
F 2 "" V 14930 3350 50  0000 C CNN
F 3 "" H 15000 3350 50  0000 C CNN
	1    15000 3350
	-1   0    0    1   
$EndComp
$Comp
L BS170 Q1
U 1 1 5900E780
P 14650 3450
F 0 "Q1" H 14850 3525 50  0000 L CNN
F 1 "BS170" H 14850 3450 50  0000 L CNN
F 2 "TO-92" H 14850 3375 50  0000 L CIN
F 3 "" H 14650 3450 50  0000 L CNN
	1    14650 3450
	0    -1   1    0   
$EndComp
$Comp
L R R5
U 1 1 5900F595
P 13100 3300
F 0 "R5" V 13180 3300 50  0000 C CNN
F 1 "10k" V 13100 3300 50  0000 C CNN
F 2 "" V 13030 3300 50  0000 C CNN
F 3 "" H 13100 3300 50  0000 C CNN
	1    13100 3300
	-1   0    0    1   
$EndComp
$Comp
L BS170 Q2
U 1 1 5900E34F
P 13400 3950
F 0 "Q2" H 13600 4025 50  0000 L CNN
F 1 "BS170" H 13600 3950 50  0000 L CNN
F 2 "TO-92" H 13600 3875 50  0000 L CIN
F 3 "" H 13400 3950 50  0000 L CNN
	1    13400 3950
	0    -1   1    0   
$EndComp
Text Notes 14150 9650 0    60   ~ 0
Door n° 1\nMotor +/- 24v
$Comp
L R R9
U 1 1 5901608B
P 15200 8350
F 0 "R9" V 15280 8350 50  0000 C CNN
F 1 "2,2k" V 15200 8350 50  0000 C CNN
F 2 "" V 15130 8350 50  0000 C CNN
F 3 "" H 15200 8350 50  0000 C CNN
	1    15200 8350
	1    0    0    1   
$EndComp
$Comp
L CONN_01X02 P3
U 1 1 5901609D
P 14400 9300
F 0 "P3" H 14400 9450 50  0000 C CNN
F 1 "CNX2" V 14500 9300 50  0000 C CNN
F 2 "" H 14400 9300 50  0000 C CNN
F 3 "" H 14400 9300 50  0000 C CNN
	1    14400 9300
	0    -1   1    0   
$EndComp
$Comp
L R R13
U 1 1 590160A3
P 13750 6500
F 0 "R13" V 13830 6500 50  0000 C CNN
F 1 "10k" V 13750 6500 50  0000 C CNN
F 2 "" V 13680 6500 50  0000 C CNN
F 3 "" H 13750 6500 50  0000 C CNN
	1    13750 6500
	-1   0    0    -1  
$EndComp
$Comp
L R R11
U 1 1 590160A9
P 14300 7100
F 0 "R11" V 14380 7100 50  0000 C CNN
F 1 "10k" V 14300 7100 50  0000 C CNN
F 2 "" V 14230 7100 50  0000 C CNN
F 3 "" H 14300 7100 50  0000 C CNN
	1    14300 7100
	-1   0    0    -1  
$EndComp
$Comp
L LTV-814 U3
U 1 1 590160AF
P 15100 7850
F 0 "U3" H 14900 8050 50  0000 L CNN
F 1 "LTV-814" H 15100 8050 50  0000 L CNN
F 2 "DIP-4" H 14900 7650 50  0000 L CIN
F 3 "" H 15125 7850 50  0000 L CNN
	1    15100 7850
	0    1    -1   0   
$EndComp
$Comp
L D D4
U 1 1 590160B5
P 15200 8700
F 0 "D4" H 15200 8800 50  0000 C CNN
F 1 "1N4004" H 15200 8600 50  0000 C CNN
F 2 "" H 15200 8700 50  0000 C CNN
F 3 "" H 15200 8700 50  0000 C CNN
	1    15200 8700
	0    -1   1    0   
$EndComp
$Comp
L D D3
U 1 1 590160BD
P 13750 8700
F 0 "D3" H 13750 8800 50  0000 C CNN
F 1 "1N4004" H 13750 8600 50  0000 C CNN
F 2 "" H 13750 8700 50  0000 C CNN
F 3 "" H 13750 8700 50  0000 C CNN
	1    13750 8700
	0    -1   1    0   
$EndComp
$Comp
L R R12
U 1 1 590160C3
P 13750 8350
F 0 "R12" V 13830 8350 50  0000 C CNN
F 1 "2,2k" V 13750 8350 50  0000 C CNN
F 2 "" V 13680 8350 50  0000 C CNN
F 3 "" H 13750 8350 50  0000 C CNN
	1    13750 8350
	1    0    0    1   
$EndComp
$Comp
L LTV-814 U4
U 1 1 590160C9
P 13850 7850
F 0 "U4" H 13650 8050 50  0000 L CNN
F 1 "LTV-814" H 13850 8050 50  0000 L CNN
F 2 "DIP-4" H 13650 7650 50  0000 L CIN
F 3 "" H 13875 7850 50  0000 L CNN
	1    13850 7850
	0    1    -1   0   
$EndComp
$Comp
L R R10
U 1 1 590160EF
P 15000 7050
F 0 "R10" V 15080 7050 50  0000 C CNN
F 1 "10k" V 15000 7050 50  0000 C CNN
F 2 "" V 14930 7050 50  0000 C CNN
F 3 "" H 15000 7050 50  0000 C CNN
	1    15000 7050
	-1   0    0    -1  
$EndComp
$Comp
L BS170 Q3
U 1 1 590160FB
P 14600 6950
F 0 "Q3" H 14800 7025 50  0000 L CNN
F 1 "BS170" H 14800 6950 50  0000 L CNN
F 2 "TO-92" H 14800 6875 50  0000 L CIN
F 3 "" H 14600 6950 50  0000 L CNN
	1    14600 6950
	0    -1   -1   0   
$EndComp
$Comp
L R R14
U 1 1 5901610A
P 13000 7100
F 0 "R14" V 13080 7100 50  0000 C CNN
F 1 "10k" V 13000 7100 50  0000 C CNN
F 2 "" V 12930 7100 50  0000 C CNN
F 3 "" H 13000 7100 50  0000 C CNN
	1    13000 7100
	-1   0    0    -1  
$EndComp
$Comp
L BS170 Q4
U 1 1 5901611E
P 13300 6400
F 0 "Q4" H 13500 6475 50  0000 L CNN
F 1 "BS170" H 13500 6400 50  0000 L CNN
F 2 "TO-92" H 13500 6325 50  0000 L CIN
F 3 "" H 13300 6400 50  0000 L CNN
	1    13300 6400
	0    -1   -1   0   
$EndComp
Text Notes 12600 6300 0    60   ~ 0
D2
$Comp
L +5V #PWR?
U 1 1 5902F5CC
P 10550 2050
F 0 "#PWR?" H 10550 1900 50  0001 C CNN
F 1 "+5V" H 10550 2190 50  0000 C CNN
F 2 "" H 10550 2050 50  0000 C CNN
F 3 "" H 10550 2050 50  0000 C CNN
	1    10550 2050
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 59039C78
P 11450 1300
F 0 "#PWR?" H 11450 1050 50  0001 C CNN
F 1 "GND" H 11450 1150 50  0000 C CNN
F 2 "" H 11450 1300 50  0000 C CNN
F 3 "" H 11450 1300 50  0000 C CNN
	1    11450 1300
	-1   0    0    1   
$EndComp
Text Notes 6250 1350 0    60   ~ 0
A4-A5 / D2-D3\nAu niveau haut au repos.\nAu niveau bas pendant le mouvement.
$Comp
L SPST SW1
U 1 1 59042167
P 13100 2050
F 0 "SW1" H 13100 2150 50  0000 C CNN
F 1 "SPST" H 13100 1950 50  0000 C CNN
F 2 "" H 13100 2050 50  0000 C CNN
F 3 "" H 13100 2050 50  0000 C CNN
	1    13100 2050
	0    -1   -1   0   
$EndComp
$Comp
L R R15
U 1 1 5904221C
P 13100 2800
F 0 "R15" V 13180 2800 50  0000 C CNN
F 1 "10k" V 13100 2800 50  0000 C CNN
F 2 "" V 13030 2800 50  0000 C CNN
F 3 "" H 13100 2800 50  0000 C CNN
	1    13100 2800
	-1   0    0    1   
$EndComp
Text Notes 6250 1700 0    60   ~ 0
A3 / D4 - Contact binaire\nAu niveau haut si la porte est ouverte (même partiellement).\nAu niveau bas si la porte est fermée
$Comp
L R R16
U 1 1 590454B2
P 13000 7600
F 0 "R16" V 13080 7600 50  0000 C CNN
F 1 "10k" V 13000 7600 50  0000 C CNN
F 2 "" V 12930 7600 50  0000 C CNN
F 3 "" H 13000 7600 50  0000 C CNN
	1    13000 7600
	-1   0    0    -1  
$EndComp
$Comp
L SPST SW2
U 1 1 5904B3D2
P 13000 8350
F 0 "SW2" H 13000 8450 50  0000 C CNN
F 1 "SPST" H 13000 8250 50  0000 C CNN
F 2 "" H 13000 8350 50  0000 C CNN
F 3 "" H 13000 8350 50  0000 C CNN
	1    13000 8350
	0    1    1    0   
$EndComp
Text Notes 12600 7800 0    60   ~ 0
D4
Text Notes 12650 2600 0    60   ~ 0
A3
$Comp
L MEDER_DIP05-1A72-12D K1
U 1 1 59051A4E
P 6000 3050
F 0 "K1" H 6000 3450 50  0000 C CNN
F 1 "MEDER_DIP05-1A72-12D" H 6050 2600 50  0000 C CNN
F 2 "" H 5300 3050 50  0000 C CNN
F 3 "" H 5300 3050 50  0000 C CNN
	1    6000 3050
	-1   0    0    1   
$EndComp
$Comp
L CONN_01X02 P4
U 1 1 5905582C
P 4700 3050
F 0 "P4" H 4700 3200 50  0000 C CNN
F 1 "CNX2" V 4800 3050 50  0000 C CNN
F 2 "" H 4700 3050 50  0000 C CNN
F 3 "" H 4700 3050 50  0000 C CNN
	1    4700 3050
	-1   0    0    1   
$EndComp
Text Notes 3850 3150 0    60   ~ 0
Door n° 2\nBP commande
$Comp
L MEDER_DIP05-1A72-12D K2
U 1 1 5905B645
P 6050 8650
F 0 "K2" H 6050 9050 50  0000 C CNN
F 1 "MEDER_DIP05-1A72-12D" H 6100 8200 50  0000 C CNN
F 2 "" H 5350 8650 50  0000 C CNN
F 3 "" H 5350 8650 50  0000 C CNN
	1    6050 8650
	-1   0    0    1   
$EndComp
$Comp
L CONN_01X02 P6
U 1 1 5905E73B
P 4800 8650
F 0 "P6" H 4800 8800 50  0000 C CNN
F 1 "CNX2" V 4900 8650 50  0000 C CNN
F 2 "" H 4800 8650 50  0000 C CNN
F 3 "" H 4800 8650 50  0000 C CNN
	1    4800 8650
	-1   0    0    1   
$EndComp
$Comp
L CONN_01X02 P5
U 1 1 59064343
P 4700 3700
F 0 "P5" H 4700 3850 50  0000 C CNN
F 1 "CNX2" V 4800 3700 50  0000 C CNN
F 2 "" H 4700 3700 50  0000 C CNN
F 3 "" H 4700 3700 50  0000 C CNN
	1    4700 3700
	-1   0    0    1   
$EndComp
Text Notes 3550 3800 0    60   ~ 0
Shorten to GND\nto disable the door
$Comp
L CONN_01X02 P7
U 1 1 5906597D
P 4800 7900
F 0 "P7" H 4800 8050 50  0000 C CNN
F 1 "CNX2" V 4900 7900 50  0000 C CNN
F 2 "" H 4800 7900 50  0000 C CNN
F 3 "" H 4800 7900 50  0000 C CNN
	1    4800 7900
	-1   0    0    1   
$EndComp
Wire Wire Line
	6000 5100 6000 4100
Wire Wire Line
	6000 4100 3100 4100
Wire Wire Line
	3100 4100 3100 5100
Wire Wire Line
	3100 5100 2600 5100
Wire Wire Line
	6150 6150 6150 7100
Wire Wire Line
	6150 7100 3100 7100
Wire Wire Line
	3100 7100 3100 5250
Wire Wire Line
	3100 5250 2600 5250
Wire Wire Line
	2600 5400 3150 5400
Wire Wire Line
	3150 5400 3150 7050
Wire Wire Line
	3150 7050 6000 7050
Wire Wire Line
	6000 7050 6000 6150
Wire Wire Line
	2600 5700 3200 5700
Wire Wire Line
	3200 5700 3200 7000
Wire Wire Line
	3200 7000 5850 7000
Wire Wire Line
	5850 7000 5850 6150
Wire Wire Line
	2600 5850 3250 5850
Wire Wire Line
	3250 5850 3250 4150
Wire Wire Line
	3250 4150 5850 4150
Wire Wire Line
	5850 4150 5850 5100
Wire Wire Line
	2600 6000 3000 6000
Wire Wire Line
	3000 6000 3000 7200
Wire Wire Line
	3000 7200 6300 7200
Wire Wire Line
	6300 7200 6300 6150
Wire Wire Line
	2600 6150 2950 6150
Wire Wire Line
	2950 6150 2950 9700
Wire Wire Line
	2950 9700 7500 9700
Wire Wire Line
	7500 6150 7500 9900
Connection ~ 7500 9700
Wire Wire Line
	15200 1900 15200 1950
Wire Wire Line
	15200 2250 15200 2300
Wire Wire Line
	13750 1600 13750 1550
Wire Wire Line
	13750 1900 13750 1950
Wire Wire Line
	13750 2250 13750 2300
Wire Wire Line
	13950 1450 13950 2300
Wire Wire Line
	13750 1550 15000 1550
Wire Wire Line
	15000 1550 15000 2300
Wire Wire Line
	14450 1350 14450 1550
Connection ~ 14450 1550
Wire Wire Line
	13950 1450 15200 1450
Wire Wire Line
	15200 1450 15200 1600
Wire Wire Line
	14550 1350 14550 1450
Connection ~ 14550 1450
Wire Wire Line
	15000 4050 13600 4050
Wire Wire Line
	15000 3500 15000 6900
Wire Wire Line
	15200 3050 15200 2900
Connection ~ 13950 3050
Wire Wire Line
	15000 3150 14700 3150
Connection ~ 15000 3150
Wire Wire Line
	14850 3550 15000 3550
Connection ~ 15000 3550
Wire Wire Line
	11150 1800 11150 1750
Wire Wire Line
	11450 1750 11050 1750
Wire Wire Line
	11050 1750 11050 1700
Wire Wire Line
	10950 1700 10950 1750
Wire Wire Line
	10950 1750 10850 1750
Wire Wire Line
	10850 1750 10850 1800
Wire Wire Line
	11150 2150 11150 2100
Wire Wire Line
	10550 2150 11150 2150
Wire Wire Line
	10850 2150 10850 2100
Wire Wire Line
	11000 2150 11000 7350
Connection ~ 11000 2150
Wire Wire Line
	14700 3150 14700 3250
Wire Wire Line
	6900 3550 14450 3550
Wire Wire Line
	15200 8550 15200 8500
Wire Wire Line
	15200 8200 15200 8150
Wire Wire Line
	13750 8850 13750 8900
Wire Wire Line
	13750 8550 13750 8500
Wire Wire Line
	13750 8200 13750 8150
Wire Wire Line
	13950 9000 13950 8150
Wire Wire Line
	13750 8900 15000 8900
Wire Wire Line
	15000 8900 15000 8150
Wire Wire Line
	14350 9100 14350 8900
Connection ~ 14350 8900
Wire Wire Line
	13950 9000 15200 9000
Wire Wire Line
	15200 9000 15200 8850
Wire Wire Line
	14450 9100 14450 9000
Connection ~ 14450 9000
Wire Wire Line
	13500 6300 15000 6300
Wire Wire Line
	15000 7200 15000 7550
Connection ~ 15000 7250
Wire Wire Line
	14800 6850 15000 6850
Connection ~ 15000 6850
Wire Wire Line
	13350 6600 13350 6750
Wire Wire Line
	13350 6750 13750 6750
Connection ~ 13750 6750
Wire Wire Line
	14650 7150 14650 7250
Wire Wire Line
	7350 6300 13100 6300
Wire Wire Line
	7200 6850 14400 6850
Wire Wire Line
	14300 6850 14300 6950
Wire Wire Line
	13750 6650 13750 7550
Wire Wire Line
	7050 4050 7050 5100
Wire Wire Line
	6900 3550 6900 5100
Wire Wire Line
	7350 6300 7350 6150
Connection ~ 13000 6300
Wire Wire Line
	7200 6850 7200 6150
Wire Wire Line
	13750 6300 13750 6350
Connection ~ 13750 6300
Wire Wire Line
	13750 4000 13750 4050
Connection ~ 14350 3550
Wire Wire Line
	13750 2900 13750 3700
Connection ~ 13750 4050
Wire Wire Line
	13450 3750 13450 3650
Wire Wire Line
	13450 3650 13750 3650
Connection ~ 13750 3650
Wire Wire Line
	7050 4050 13200 4050
Wire Wire Line
	13100 3450 13100 4050
Connection ~ 13100 4050
Wire Wire Line
	14350 3450 14350 3550
Wire Wire Line
	13950 3050 13950 2900
Wire Wire Line
	15000 2900 15000 3200
Wire Wire Line
	14350 3050 14350 3150
Connection ~ 14350 3050
Wire Wire Line
	13100 2950 13100 3150
Connection ~ 13100 3050
Wire Wire Line
	15200 7350 15200 7550
Wire Wire Line
	11000 7350 15200 7350
Connection ~ 11000 3050
Wire Wire Line
	13950 7550 13950 7350
Connection ~ 13950 7350
Wire Wire Line
	14650 7250 15000 7250
Connection ~ 14300 6850
Wire Wire Line
	14300 7250 14300 7350
Connection ~ 14300 7350
Wire Wire Line
	13000 6950 13000 6300
Wire Wire Line
	13000 7250 13000 7450
Connection ~ 13000 7350
Wire Wire Line
	10550 2050 10550 2150
Connection ~ 10850 2150
Wire Wire Line
	11000 3050 15200 3050
Wire Wire Line
	11450 1300 11450 8900
Connection ~ 11150 1750
Wire Wire Line
	7500 5100 7500 4850
Wire Wire Line
	7800 2850 7800 5100
Wire Wire Line
	7800 4950 15000 4950
Connection ~ 15000 4050
Connection ~ 15000 6300
Connection ~ 15000 4950
Connection ~ 11450 4950
Connection ~ 11450 1750
Wire Wire Line
	7500 4850 11000 4850
Connection ~ 11000 4850
Wire Wire Line
	13100 2550 13100 2650
Wire Wire Line
	13100 2600 6750 2600
Wire Wire Line
	6750 2600 6750 5100
Connection ~ 13100 2600
Wire Wire Line
	13100 1550 13100 1500
Wire Wire Line
	13100 1500 11450 1500
Connection ~ 11450 1500
Wire Wire Line
	13000 7750 13000 7850
Wire Wire Line
	13000 7800 7050 7800
Wire Wire Line
	7050 7800 7050 6150
Connection ~ 13000 7800
Wire Wire Line
	13000 8850 13000 8900
Wire Wire Line
	13000 8900 11450 8900
Connection ~ 7800 4950
Wire Wire Line
	6600 3250 6600 5100
Wire Wire Line
	6450 3250 6600 3250
Wire Wire Line
	6450 2850 7800 2850
Wire Wire Line
	4900 3000 5150 3000
Wire Wire Line
	5150 3000 5150 2750
Wire Wire Line
	5150 2750 5450 2750
Wire Wire Line
	4900 3100 5150 3100
Wire Wire Line
	5150 3100 5150 3350
Wire Wire Line
	5150 3350 5450 3350
Wire Wire Line
	6900 6150 6900 8850
Wire Wire Line
	6900 8850 6500 8850
Wire Wire Line
	6500 8450 7500 8450
Connection ~ 7500 8450
Wire Wire Line
	5000 8600 5250 8600
Wire Wire Line
	5250 8600 5250 8350
Wire Wire Line
	5250 8350 5500 8350
Wire Wire Line
	5000 8700 5250 8700
Wire Wire Line
	5250 8700 5250 8950
Wire Wire Line
	5250 8950 5500 8950
Wire Wire Line
	4900 3650 7800 3650
Connection ~ 7800 3650
Wire Wire Line
	4900 3750 6450 3750
Wire Wire Line
	6450 3750 6450 5100
Wire Wire Line
	5000 7950 7500 7950
Connection ~ 7500 7950
Wire Wire Line
	5000 7850 6750 7850
Wire Wire Line
	6750 7850 6750 6150
Text Notes 3900 8750 0    60   ~ 0
Door n° 1\nBP commande
Text Notes 3650 8000 0    60   ~ 0
Shorten to GND\nto disable the door
Text Notes 12050 2000 0    60   ~ 0
Contact door n° 2\nis closed
Text Notes 11900 8450 0    60   ~ 0
Contact door n° 1\nis closed
$EndSCHEMATC
