EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 2 2
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text GLabel 6300 3650 0    50   Input ~ 0
SDA
Text GLabel 6300 3750 0    50   Input ~ 0
SCL
Text GLabel 5700 5900 3    50   Input ~ 0
RXP
Text GLabel 5900 5900 3    50   Input ~ 0
RXN
Text GLabel 5400 5900 3    50   Input ~ 0
TX2
Text GLabel 5750 5650 2    50   Input ~ 0
TX1
$Comp
L SmartFeedback-rescue:C_Small-Device C21
U 1 1 604E3900
P 6300 4500
F 0 "C21" H 6400 4600 50  0000 C CNN
F 1 "0.1uF" H 6400 4400 50  0000 C CNN
F 2 "Capacitor_SMD:C_0603_1608Metric_Pad1.05x0.95mm_HandSolder" H 6300 4500 50  0001 C CNN
F 3 "~" H 6300 4500 50  0001 C CNN
	1    6300 4500
	1    0    0    -1  
$EndComp
$Comp
L SmartFeedback-rescue:GND-power #PWR078
U 1 1 604E3907
P 6300 4700
F 0 "#PWR078" H 6300 4450 50  0001 C CNN
F 1 "GND" V 6305 4527 50  0000 C CNN
F 2 "" H 6300 4700 50  0001 C CNN
F 3 "" H 6300 4700 50  0001 C CNN
	1    6300 4700
	1    0    0    -1  
$EndComp
Wire Wire Line
	6300 4600 6300 4700
$Comp
L SmartFeedback-rescue:+3V3-power #PWR077
U 1 1 604E3BA8
P 5900 3950
F 0 "#PWR077" H 5900 3800 50  0001 C CNN
F 1 "+3V3" V 5915 4078 50  0000 L CNN
F 2 "" H 5900 3950 50  0001 C CNN
F 3 "" H 5900 3950 50  0001 C CNN
	1    5900 3950
	0    -1   -1   0   
$EndComp
Text GLabel 5550 5350 0    50   Input ~ 0
NFC_IRQ
Text GLabel 5550 5500 0    50   Input ~ 0
NFC_VEN
Wire Wire Line
	10000 3350 10000 3700
Wire Wire Line
	10000 3700 10600 3700
Wire Wire Line
	10600 3700 10600 4000
Wire Wire Line
	10600 4000 10000 4000
Wire Wire Line
	10000 4000 10000 4400
$Comp
L SamacSys_Parts:ST25DV64KC-IE6S3 IC?
U 1 1 63282771
P 8000 3950
F 0 "IC?" H 8850 4215 50  0000 C CNN
F 1 "ST25DV64KC-IE6S3" H 8850 4124 50  0000 C CNN
F 2 "SOIC127P600X175-8N" H 9550 4050 50  0001 L CNN
F 3 "https://www.st.com/resource/en/datasheet/st25dv04kc.pdf" H 9550 3950 50  0001 L CNN
F 4 "Dynamic NFC/RFID tag IC with 64-Kbit EEPROM, and fast transfer mode capability" H 9550 3850 50  0001 L CNN "Description"
F 5 "1.75" H 9550 3750 50  0001 L CNN "Height"
F 6 "511-ST25DV64KC-IE6S3" H 9550 3650 50  0001 L CNN "Mouser Part Number"
F 7 "https://www.mouser.co.uk/ProductDetail/STMicroelectronics/ST25DV64KC-IE6S3?qs=QNEnbhJQKvbtdVYNeWuKLg%3D%3D" H 9550 3550 50  0001 L CNN "Mouser Price/Stock"
F 8 "STMicroelectronics" H 9550 3450 50  0001 L CNN "Manufacturer_Name"
F 9 "ST25DV64KC-IE6S3" H 9550 3350 50  0001 L CNN "Manufacturer_Part_Number"
	1    8000 3950
	-1   0    0    1   
$EndComp
$Comp
L SmartFeedback-rescue:C_Small-Device C?
U 1 1 63299AD2
P 9050 3850
F 0 "C?" V 8821 3850 50  0000 C CNN
F 1 "C_Tune" V 8912 3850 50  0000 C CNN
F 2 "Capacitor_SMD:C_0603_1608Metric_Pad1.05x0.95mm_HandSolder" H 9050 3850 50  0001 C CNN
F 3 "~" H 9050 3850 50  0001 C CNN
	1    9050 3850
	-1   0    0    1   
$EndComp
$Comp
L SmartFeedback-rescue:C_Small-Device C?
U 1 1 632A1ECF
P 9400 3850
F 0 "C?" V 9171 3850 50  0000 C CNN
F 1 "C_Tune" V 9262 3850 50  0000 C CNN
F 2 "Capacitor_SMD:C_0603_1608Metric_Pad1.05x0.95mm_HandSolder" H 9400 3850 50  0001 C CNN
F 3 "~" H 9400 3850 50  0001 C CNN
	1    9400 3850
	-1   0    0    1   
$EndComp
$Comp
L SmartFeedback-rescue:GND-power #PWR?
U 1 1 632AE2F3
P 8000 3650
F 0 "#PWR?" H 8000 3400 50  0001 C CNN
F 1 "GND" V 8005 3477 50  0000 C CNN
F 2 "" H 8000 3650 50  0001 C CNN
F 3 "" H 8000 3650 50  0001 C CNN
	1    8000 3650
	-1   0    0    1   
$EndComp
Connection ~ 6300 3950
Wire Wire Line
	6300 3950 6300 4400
Wire Wire Line
	6300 3950 5950 3950
$Comp
L SmartFeedback-rescue:C_Small-Device C?
U 1 1 632BCE28
P 5950 4500
F 0 "C?" H 6050 4600 50  0000 C CNN
F 1 "10uF" H 6050 4400 50  0000 C CNN
F 2 "Capacitor_SMD:C_0603_1608Metric_Pad1.05x0.95mm_HandSolder" H 5950 4500 50  0001 C CNN
F 3 "~" H 5950 4500 50  0001 C CNN
	1    5950 4500
	1    0    0    -1  
$EndComp
Wire Wire Line
	5950 4400 5950 3950
Connection ~ 5950 3950
Wire Wire Line
	5950 3950 5900 3950
$Comp
L SmartFeedback-rescue:GND-power #PWR?
U 1 1 632BDC9C
P 5950 4700
F 0 "#PWR?" H 5950 4450 50  0001 C CNN
F 1 "GND" V 5955 4527 50  0000 C CNN
F 2 "" H 5950 4700 50  0001 C CNN
F 3 "" H 5950 4700 50  0001 C CNN
	1    5950 4700
	1    0    0    -1  
$EndComp
Wire Wire Line
	5950 4700 5950 4600
$Comp
L SmartFeedback-rescue:C_Small-Device C?
U 1 1 632BF9BE
P 8700 3850
F 0 "C?" V 8471 3850 50  0000 C CNN
F 1 "C_Tune" V 8562 3850 50  0000 C CNN
F 2 "Capacitor_SMD:C_0603_1608Metric_Pad1.05x0.95mm_HandSolder" H 8700 3850 50  0001 C CNN
F 3 "~" H 8700 3850 50  0001 C CNN
	1    8700 3850
	-1   0    0    1   
$EndComp
Wire Wire Line
	8700 3350 8700 3750
Wire Wire Line
	8700 3350 9050 3350
Wire Wire Line
	8000 3750 8700 3750
Connection ~ 8700 3750
Wire Wire Line
	9050 3750 9050 3350
Connection ~ 9050 3350
Wire Wire Line
	9050 3350 9400 3350
Wire Wire Line
	9400 3750 9400 3350
Connection ~ 9400 3350
Wire Wire Line
	9400 3350 10000 3350
Wire Wire Line
	9400 4400 9400 3950
Wire Wire Line
	9400 4400 10000 4400
Wire Wire Line
	9400 4400 9050 4400
Wire Wire Line
	9050 4400 9050 3950
Connection ~ 9400 4400
Wire Wire Line
	9050 4400 8700 4400
Wire Wire Line
	8700 4400 8700 3950
Connection ~ 9050 4400
Wire Wire Line
	8700 3950 8400 3950
Wire Wire Line
	8400 3950 8400 3850
Wire Wire Line
	8400 3850 8000 3850
Connection ~ 8700 3950
Text GLabel 8000 4100 3    50   Input ~ 0
VEH
Text GLabel 5800 3800 0    50   Input ~ 0
GPO
$EndSCHEMATC
