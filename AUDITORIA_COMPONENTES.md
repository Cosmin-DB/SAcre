# Auditoría eléctrica por componentes

**Autor:** Cosmin Dobrescu
**Fecha:** 2026-08-03
**Alcance:** esquema actual, netlist exportado con KiCad 10 y datasheets del
fabricante. Se revisan también dependencias entre bloques. No se modifica el
esquema ni la PCB.

Estados: `CORRECTO`, `CONDICIONAL`, `CORREGIR` y `NO VERIFICABLE`.

## Resultado ejecutivo

### Debe corregirse

1. **Salida del TLV (`R26`, `R27`, FT6336U)**. El divisor actual produce
   3,318 V nominal y aproximadamente 3,22–3,42 V incluyendo tolerancias. El
   FT6336U sólo especifica 2,8–3,3 V en funcionamiento; 3,6 V es máximo
   absoluto, no tensión de diseño. Debe bajarse el objetivo de `+3V3`. Como
   referencia, 432 kΩ/100 kΩ con R26 de 0,1 % daría 3,192 V nominal y
   aproximadamente 3,10–3,29 V, manteniendo margen para todos los bloques.
### Corregido en esta revisión

1. **Booster de pantalla (`R9`)**. R9 queda en 1 MΩ entre `GDR` y
   GND, en paralelo con la puerta de Q4, como R1 en la figura 12 del datasheet
   específico del GDEY042T81-T02. Reutiliza el valor de R15 y no añade otro
   valor al BOM.

### Decisiones aceptadas

- **RTC (`Y1`)**: se conserva la alimentación permanente desde `VBAT_RAW` y el
  bus I²C a `+3V3`; no se trata como fallo pendiente.
- **Cargador (`U5`)**: se acepta el TP4054 montado y se cierra la revisión de su
  tensión final.
- **RF (`U2`, `AE1`)**: se conserva la conexión directa mediante pista de 50 Ω.
  No se reserva una red π porque no se dispone de medida RF para determinar sus
  valores. El riesgo se acepta y se deja para una futura revisión con VNA.

### Debe decidirse o medirse

1. **Motor (`M1`, `Q2`)**. Falta referencia exacta. Si el arranque real es
   unos 90 mA, Q2, D1 y D9 son razonables. Si es realmente 901 mA, supera los
   500 mA absolutos del S8050 y el conjunto no es válido.
2. **Bus I²C a 400 kHz (`R7`, `R8`)**. 5,1 kΩ da un máximo teórico aproximado
   de 69 pF para cumplir 300 ns de subida. Hay cuatro dispositivos, PCB y flex;
   no puede afirmarse 400 kHz sin estimar o medir la capacitancia. Todos los
   dispositivos admiten 100 kHz; FT6336U declara 400 kHz pero conserva varios
   tiempos mínimos propios de modo estándar. Usar 100 kHz hasta medir.
3. **Reset táctil (`R11`, `C25`, `D8`)**. El FT6336U incorpora un pull-up de
   3 kΩ en RSTN. Con R11=1 kΩ, el nivel bajo nominal es 0,825 V frente a un
   máximo permitido de 0,99 V: funciona nominalmente, pero el margen depende
   de la tolerancia no especificada del pull-up interno. C25 produce unos
   30 ms nominales y D8 acelera la descarga. Validar `RSTN < 0,3·IOVCC`; si no,
   reducir R11.
5. **RF y mecánica**. Impedancia de pista, keepout, salida del flex y huellas se
   cierran en la revisión PCB, no mediante ERC. La decisión de omitir la red π
   ya está cerrada.

## Revisión por referencias

### Procesador, alimentación de 3,3 V y RF

| Referencias | Estado | Justificación |
| --- | --- | --- |
| U2 | CORRECTO | ESP32-PICO-D4, 3,0–3,6 V. GPIO34/35 se usan sólo como entradas. GPIO9 y GPIO10 pueden usarse libremente en el PICO-D4; la flash interna usa otro mapeo del SiP. Los pines de alimentación y masa coinciden con el datasheet. |
| C26, C27 | CORRECTO | 10 µF + 100 nF sobre `+3V3`, combinación indicada por el circuito periférico de Espressif. Deben quedar junto al grupo de alimentación del PICO. |
| C30 | CORRECTO | 100 nF exclusivamente entre `VDD_SDIO` y GND, como indica Espressif. No debe unirse `VDD_SDIO` a `+3V3`. |
| R12, C17 | CORRECTO | 10 kΩ + 1 µF en EN produce unos 10 ms y mantiene el pin definido. Es compatible con el auto-reset; el latch puede exigir mantener pulsado durante programación. |
| U6 | CORRECTO | TLV62568DBVR: VIN 2,5–5,5 V, salida de 1 A, EN definido y unido a VIN. `VSYS` de 0–4,2 V entra en rango. |
| R26, R27 | CORREGIR | 453 kΩ/100 kΩ produce 3,318 V nominal y aproximadamente 3,22–3,42 V con tolerancias. R27 cumple el máximo de 200 kΩ de TI, pero el extremo alto supera los 3,3 V de funcionamiento del FT6336U. Bajar el divisor; 432 kΩ/100 kΩ con R26 de 0,1 % es una referencia conservadora que debe cerrarse al seleccionar componente. |
| L3 | CORRECTO | 1 µH; 3,6 A térmicos mínimos, 5 A de saturación mínimos y DCR máxima de 42 mΩ. Supera ampliamente el límite de corriente del TLV. TI valida 1 µH con 10 µF para salidas de al menos 1,8 V. |
| C31, C32 | CORRECTO | 10 µF X5R/X7R de baja ESR. La entrada supera los 4,7 µF recomendados y la salida está dentro de 10–47 µF. El derating por polarización debe considerarse en PCB/BOM. |
| U6, L3, C31, C32, R26, R27 | CONDICIONAL PCB | C31 debe quedar junto a VIN/GND; SW-L3-C32 debe formar un bucle corto y ancho; FB debe alejarse de SW. Puede añadirse 6,8 pF en paralelo con R26 para mejorar transitorios, pero TI lo presenta como opcional, no como requisito de estabilidad. |
| AE1 | DECISIÓN ACEPTADA | Antena JAE AN012400C00 de 2,4–2,5 GHz y 50 Ω. Se conecta directamente a U2 mediante una pista corta y controlada de 50 Ω. Se omite deliberadamente la red π porque sus valores exigirían medida RF; una revisión futura podrá incorporarla y ajustarla con VNA. |

**Dependencia de batería baja:** el TLV entra en ciclo del 100 % cuando
`VBAT_RAW` se aproxima a 3,3 V. Aproximadamente, `VOUT = VIN - I·(RDS(on)+DCR)`;
la protección de la celda puede cortar demasiado tarde para el ESP32. El
firmware debe apagar mediante el latch antes de brownouts repetidos.

### Pantalla GDEY042T81-T02 y booster SSD1683

| Referencias | Estado | Justificación |
| --- | --- | --- |
| J2 | CORRECTO | La numeración está invertida respecto al FPC físico de forma intencionada. El netlist reproduce los 24 pines del panel: VCOM→J2.1 hasta NC→J2.24. VDDIO y VCI van a `+3V3`; BS1 va a GND para SPI de 4 hilos; los NC permanecen abiertos. |
| C5, C6 | CORRECTO | 4,7 µF/25 V en entrada y bomba, exactamente los valores C4 y C3 del circuito específico del panel. |
| C7, C9–C15 | CORRECTO | 1 µF/50 V; cubren PREVGH, VDD, VSH1/VSH2, VSL, VGL y VCOM con margen sobre los 25 V prescritos. |
| L1 | CORRECTO | FHD4020S-470MT, 47 µH, 660 mA nominal y 1,3 A de saturación. Coincide con los 47 µH/500 mA del panel. |
| D2–D4 | CORRECTO | C236079 es onsemi MBR0530T3G: 30 V, 500 mA, 5,5 A de pulso y `VF <= 0,43 V` a 500 mA. Cumple tanto el panel como la tabla genérica del SSD1683. Orientaciones verificadas contra el circuito de referencia. |
| Q4 | CORRECTO | SI1308EDL C7603347, exactamente el modelo pedido por Good Display: 30 V, 1,4 A y pinout G1/S2/D3. |
| R10 | CORRECTO | 2,2 Ω entre RESE y GND, valor exacto del panel. |
| R9 | CORREGIDO | 1 MΩ entre `GDR` y GND, en paralelo con la puerta de Q4. Corresponde a R1 de la figura 12 del datasheet específico del panel. Usa C22935, igual que R15. |
| GPIO5/9/10/18/19/23 | CORRECTO | CS, RES, DC, SCK, BUSY y MOSI son compatibles. GPIO5 es pin de strapping, pero el panel sólo presenta una entrada; debe comprobarse que no fuerza nivel durante reset. GPIO9/10 son libres en ESP32-PICO-D4. |

### Táctil e I²C

| Referencias | Estado | Justificación |
| --- | --- | --- |
| J1 | CORRECTO EN ESQUEMA | J1.1=SCL, J1.2=SDA, J1.3=INT, J1.4=RSTN, J1.5=`+3V3`, J1.6=GND. Mecánica y correspondencia del flex siguen pendientes de PCB. |
| R7, R8 | CONDICIONAL | 5,1 kΩ limita la corriente LOW a unos 0,65 mA, válida para todos los dispositivos. 400 kHz depende de la capacitancia total; 100 kHz es el ajuste conservador. |
| R11, C25, D8 | CONDICIONAL | Forman el reset táctil descrito arriba. El FT6336U sólo exige `Trst >= 5 ms` y esperar al menos 300 ms; mantener LOW 60 ms es conservador. Controlar GPIO4 como drenador abierto evita conducir contra el pull-up interno. |
| INT_TOUCH/GPIO35 | CORRECTO | GPIO35 es de entrada y la salida INT del FT6336U es CMOS; no se exige pull-up externo. |
| Direcciones I²C | CORRECTO | LIS3DH=0x19 por SDO interno alto, RV-8803=0x32 y FT6336U=0x38; no hay colisión. |

### Acelerómetro

| Referencias | Estado | Justificación |
| --- | --- | --- |
| U1 | CORRECTO | LIS3DH alimentado a 3,3 V. VDD y VDD_IO aparecen simultáneamente. CS está alto para I²C; SDO puede quedar abierto porque incorpora pull-up y selecciona 0x19. ADC1–ADC3 pueden quedar abiertos. |
| C3, C4 | CORRECTO | 100 nF + 10 µF sobre el mismo `+3V3`, combinación prescrita por ST junto al pin 14. C3 y C4 no son redundantes: cubren alta frecuencia y reserva local. |
| R1 | CORRECTO | 10 kΩ mantiene CS alto y fija modo I²C. |
| ACC_INT1, ACC_INT2 | CORRECTO | GPIO14 admite E/S y GPIO34 es sólo entrada; ambas señales son salidas programables del LIS3DH. |

### RTC

| Referencias | Estado | Justificación |
| --- | --- | --- |
| Y1 | DECISIÓN ACEPTADA | Se mantiene alimentado permanentemente desde `VBAT_RAW` y conectado al I²C de `+3V3`. La revisión queda cerrada por decisión de diseño. |
| C2 | CORRECTO | 100 nF local; Micro Crystal recomienda 10–100 nF. |
| R22 | CORRECTO | 10 kΩ a `+3V3` para `INT`, salida open-drain. Evita aplicar `VBAT_RAW` al ESP32. |
| R21, TP1 | CORRECTO | EVI es una entrada y no debe flotar. R21 la eleva a `VBAT_RAW`; TP1 permite un futuro evento activo bajo. |
| CLKOE/CLKOUT | CORRECTO | CLKOE a GND y CLKOUT sin conectar deshabilitan una salida no utilizada. |

### USB, CP2104 y programación

| Referencias | Estado | Justificación |
| --- | --- | --- |
| J3 | CORRECTO | USB-C 2.0: pares A6/B6 y A7/B7 unidos; SBU abiertos; VBUS y GND correctos. |
| R16, R17 | CORRECTO | 5,1 kΩ a GND en CC1/CC2: anuncio Rd de dispositivo consumidor. |
| D6 | CORRECTO | SP0503BAHTG es el protector que recomienda Silicon Labs; D+, D− y VBUS protegidos, común a GND. Debe quedar junto al conector. |
| R15, C19 | CORRECTO | 1 MΩ en paralelo con 4,7 nF entre shield y GND: descarga electrostática y acoplo de alta frecuencia sin unión DC rígida. Su efectividad depende del trazado. |
| U3 | CORRECTO ELÉCTRICAMENTE | Conexión self-powered con regulador bypass: VIO/VDD/REGIN a `+3V3`, exactamente como figura 9 del CP2104. |
| C20, C23 | CORRECTO | 100 nF + 1 µF locales en la alimentación del CP2104. |
| R23, R24 | CORRECTO | Divisor 24 kΩ/47 kΩ desde VBUS, exactamente el recomendado cuando VBUS puede existir con VDD apagado. Da unos 3,31 V con VBUS=5 V. |
| C24 | CORRECTO | 4,7 µF en VPP. Este pin no se utiliza en el funcionamiento previsto y el condensador no afecta al USB normal. |
| Q7 | CORRECTO | UMH3N con pinout E1/B1/C2/E2/B2/C1; la conexión cruzada DTR/RTS evita que ambas señales mantengan simultáneamente EN y GPIO0 en reset. |

### Cargador y batería

| Referencias | Estado | Justificación |
| --- | --- | --- |
| U5 | CORRECTO / DECISIÓN ACEPTADA | TP4054 C5381776, pinout y aplicación coincidentes. Entrada USB de 5 V en rango, regulación térmica y terminación C/10. Se acepta el componente montado y se cierra esta revisión. |
| R19 | CORRECTO | 5,1 kΩ produce unos 196 mA por `IBAT=1000/RPROG`. Con la dispersión indicada por el fabricante puede apartarse sensiblemente del nominal, pero sigue siendo moderado para 600–700 mAh. |
| C29, C28 | CORRECTO | 1 µF en VDD y 10 µF en BAT coinciden con la aplicación del fabricante. Deben quedar junto a U5. |
| D7, R20 | CORRECTO | LED desde VBUS hacia STAT open-drain mediante 1 kΩ. Corriente compatible con STAT y visible sin una carga relevante. |
| Sin power-path | DECISIÓN ACEPTADA | Con el equipo encendido, U5 no distingue corriente de batería y corriente del sistema: la carga puede ralentizarse, no terminar o ciclar. No crea una conexión prohibida, pero es una consecuencia funcional deliberada. |
| Batería | NO VERIFICABLE | Se conoce formato y capacidad aproximada, pero no modelo, corriente máxima, umbrales de protección ni tensión de carga admitida. |

### Latch, botón y medida de batería

| Referencias | Estado | Justificación |
| --- | --- | --- |
| Q5 | CORRECTO | CJ3415 P-MOS como interruptor high-side, source a `VBAT_RAW` y drain a `VSYS`. Tensiones y corriente tienen margen. |
| Q6 | CORRECTO | CJ2312 N-MOS tira de la puerta de Q5; admite mando de 3,3 V. |
| D5 | CORRECTO | BAT54C de cátodo común: el pulsador descarga las puertas de Q5/Q6 por las dos ramas sin unirlas directamente. |
| R13, C18 | CORRECTO CON LIMITACIÓN | 300 kΩ y 100 nF producen 30 ms nominales. Retrasan la liberación de Q5, pero no son un supervisor ni garantizan inmunidad frente a toda alimentación parcial. |
| R14, R18 | CORRECTO | 100 kΩ mantiene Q6 apagado y 1 kΩ limita el mando desde GPIO25. El firmware debe elevar GPIO25 antes de soltar el pulsador. |
| SW1 | CORRECTO | Contacto de encendido a GND y shield a GND; pinout compatible con TSC02-TDW1FH3.5. |
| Q1, Q3 | CORRECTO | CJ2312 + CJ3415 conmutan el divisor desde el lado alto; apagado deja ADC_BATT a GND por R6 y evita consumo permanente. |
| R3, R4 | CORRECTO | Pull-ups de 100 kΩ mantienen apagados Q3 y Q1 respectivamente. |
| R5, R6 | CORRECTO | 10 kΩ/10 kΩ divide 4,2 V a 2,1 V, dentro del pin del ESP32. Firmware debe usar atenuación ADC suficiente y calibración; GPIO33 pertenece a ADC1 y puede medirse con Wi-Fi activo. |

### Motor

| Referencias | Estado | Justificación |
| --- | --- | --- |
| M1 | NO VERIFICABLE | Sin modelo ni especificación cerrada; la huella actual tampoco demuestra compatibilidad mecánica. |
| Q2 | CONDICIONAL | S8050 C20069125, pinout B1/E2/C3, 500 mA absolutos. R2=1 kΩ entrega unos 2,6 mA de base; es razonable para 70–90 mA, pero no garantiza 901 mA. |
| R2, R25 | CORRECTO PARA 90 mA | 1 kΩ limita base y 100 kΩ mantiene Q2 apagado durante reset. Si el motor real exige mucha más corriente debe rediseñarse el driver, no sólo cambiar R2. |
| D1 | CORRECTO PARA 90 mA | MBR0530 como rueda libre, 30 V/500 mA y orientación correcta. |
| D9 | CORRECTO PARA 90 mA | MBR0530 en serie desde `VSYS` a `VMOTOR`. A 70–100 mA su caída mantiene un motor de rango 2,5–4 V por debajo o cerca de 4 V con batería llena. No protege un motor de 901 mA. |
| C1 | CORRECTO | 100 nF en paralelo con M1 reduce ruido de escobillas. PWM es opcional; D1 mantiene continuidad de corriente. |

### Pasivos y encapsulados restantes

| Referencias | Estado | Justificación |
| --- | --- | --- |
| C1–C4, C17, C20, C23, C25–C32 | CORRECTO | Tensiones nominales de los MLCC superan 4,2/5 V. Los valores y dieléctricos corresponden a sus funciones; la validez final depende de colocación y derating DC. |
| C5–C7, C9–C15, C24 | CORRECTO | 25 o 50 V según redes auxiliares de pantalla/VPP; no deben sustituirse por MLCC de menor tensión sin revisar derating. |
| R1–R27 salvo R26 y R27 | CORRECTO | Valores, potencia 0603 y tolerancias son suficientes para las corrientes calculadas. R26/R27 se evalúan aparte con la salida del TLV. |
| J5, J6, TP1 | CONDICIONAL PCB | Son conexiones/puntos de prueba; eléctricamente correctos, pero su existencia y acceso dependen de la PCB final. |

## Dependencias cruzadas verificadas

- `+3V3` actual es compatible con ESP32, CP2104, LIS3DH y el panel, pero no
  queda dentro del máximo operativo del FT6336U en toda la tolerancia.
- El motor ya no carga `+3V3`; toma energía de `VSYS` a través de D9.
- El booster de pantalla coincide con el circuito específico de Good Display,
  incluida R9 ya corregida a 1 MΩ.
- GPIO9/10 no interfieren con la flash interna del ESP32-PICO-D4.
- Las direcciones I²C no colisionan. La interfaz del RTC se conserva como está
  por decisión de diseño.
- CP2104 apagado con VBUS presente queda en la topología self-powered prevista
  por Silicon Labs y ya no debe realimentar directamente `+3V3`.
- La carga simultánea sin power-path es eléctricamente posible, pero modifica
  terminación y tiempo de carga.

## Fuentes principales

- [ESP32-PICO series](documentation/datasheets/ESP32-PICO-series.pdf) y
  [guía de hardware](documentation/esp32_hardware_design_guidelines_en.pdf).
- [GDEY042T81-T02](documentation/datasheets/GDEY042T81-T02.pdf) y
  [SSD1683](documentation/datasheets/SSD1683.pdf).
- [FT6336U](documentation/datasheets/FT6336U.pdf),
  [LIS3DH](documentation/lis3dh_datasheet.pdf) y
  [RV-8803-C7 application manual](RV-8803/RV-8803-C7_App-Manual.pdf).
- [CP2104](documentation/cp2104-1397921.pdf),
  [TP4054 exacto](documentation/datasheets/TP4054-JSMSEMI-C5381776.pdf),
  [S8050 exacto](documentation/datasheets/S8050-R+O-C20069125.pdf) y
  [SI1308EDL exacto](documentation/datasheets/SI1308EDL-TECH-PUBLIC-C7603347.pdf).
- [TLV62568, TI](https://www.ti.com/lit/ds/symlink/tlv62568.pdf) y
  [MBR0530T3G, onsemi](https://www.onsemi.com/download/data-sheet/pdf/mbr0530t1-d.pdf).
