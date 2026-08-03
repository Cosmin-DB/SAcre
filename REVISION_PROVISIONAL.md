# Revisión provisional independiente

**Autor:** Cosmin Dobrescu
**Fecha:** 2026-07-29
**Estado:** registro de verificación en curso. El README mantiene el checklist
vigente; cada hallazgo indica aquí si su corrección ya fue aplicada.

No se repiten aquí los cinco fallos ya anotados en el esquemático.

## Evidencia del prototipo

### OBS-01 — Pantalla funcional; táctil y Bluetooth no coexistían correctamente

**Relato del autor (2026-07-28)**

- El conjunto usado era la pantalla táctil de 4,2 pulgadas declarada en el
  diseño; la pantalla funcionaba.
- El táctil y Bluetooth no funcionaban correctamente al usarlos
  simultáneamente. Falta precisar si ambos funcionaban por separado.

**Separación de bloques**

- La tinta electrónica `GDEY042T81-T02` usa `SSD1683`, conector J2 de 24 pines
  y SPI. El booster sólo interviene al generar las tensiones de refresco.
- El táctil `FT6336U` usa el conector J1 de 6 pines, bus I²C y `+3V3`. Su
  consumo activo típico es 4,32 mA.
- Bluetooth es interno al ESP32 y no comparte los GPIO del I²C. No aparece un
  conflicto directo de pines entre ambas funciones.
- Táctil, ESP32 y booster reciben energía de la misma red `+3V3`.

**Fuentes:** [esquemático](sacre.kicad_sch), [panel](documentation/datasheets/GDEY042T81-T02.pdf), [FT6336U](documentation/datasheets/FT6336U.pdf), [ESP32-PICO](documentation/datasheets/ESP32-PICO-series.pdf).

**Interpretación provisional**

- `TOUCH-01` —SDA y SCL intercambiadas en el FPC— explica un fallo directo del
  táctil, pero no un fallo propio de Bluetooth.
- `RV-05` es ahora la causa lógica más fuerte del táctil si JP1 estaba cerrado:
  su reset comparte por error el `FLASH_CS` interno del ESP32.
- `RV-01` es una causa común plausible: la actividad de radio eleva la demanda
  del ESP32 y una caída de `+3V3` puede reiniciar o bloquear tanto el MCU como
  el FT6336U.
- `RV-02` sólo explicaría la coincidencia si se estaba refrescando la pantalla:
  el booster toma pulsos de corriente de `+3V3`.
- Siguen abiertas causas de firmware, concurrencia y desacoplo local del ESP32.

**Comprobación necesaria**

- [ ] Registrar qué funcionaba por separado: táctil, Bluetooth y refresco.
- [ ] Repetir una matriz táctil activo/inactivo × Bluetooth activo/inactivo,
      sin refrescar la pantalla.
- [ ] Repetir durante un refresco y medir mínimo/rizado de `+3V3`.
- [ ] Registrar brownouts, resets, errores I²C y desconexiones Bluetooth.

## Hallazgos para validar

### RV-01 — L2 no es apta para el regulador de 3,3 V

**Confianza:** muy alta · **Impacto posible:** crítico

- El BOM fabricado monta en L2 `C1042`, identificado como `SDFL2012Q1R0KTF`.
- Su datasheet especifica `Ir = 50 mA` y `DCR = 0,4 Ω`.
- U4 es un ADP2108 de hasta 600 mA. Su datasheet exige que la corriente DC admisible de la bobina sea mayor que la corriente de pico; sus referencias de 1 µH parten de 800 mA.
- L2 alimenta toda la red `+3V3`, incluido el ESP32. Es una incompatibilidad objetiva aunque la placa llegue a arrancar.
- Espressif especifica una fuente capaz de entregar al menos 0,5 A al
  ESP32-PICO. La corriente nominal de L2 es diez veces menor incluso antes de
  sumar pantalla, táctil, sensores y motor.

**Implicaciones**

- `50 mA` no es un corte: L2 puede conducir más corriente, pero fuera de su
  especificación puede perder inductancia, calentarse y elevar el rizado.
- Los síntomas esperables son brownouts o resets del ESP32, fallos de radio,
  periféricos inestables y comportamiento dependiente de batería, temperatura
  o unidad fabricada.
- Es una explicación plausible para la parte Bluetooth de `OBS-01`, no una
  causalidad demostrada.

**Corrección aplicada en el esquemático**

- L2 queda como `FTC252012S1R0MBCA`, LCSC `C5832370`: 1 µH, 4 A, Isat
  5,6 A y DCR 35 mΩ, con huella `L_Changjiang_FTC252012S` en el esquema.
- La actualización y colocación en PCB permanecen pendientes.

**Fuentes:** [BOM fabricado](bom_jlcpb.csv), [L2 C1042, pág. 4](documentation/datasheets/SDFL2012Q1R0KTF-C1042.pdf), [ADP2108, pág. 13](documentation/datasheets/ADP2108.pdf), [ESP32-PICO, tabla 17](documentation/datasheets/ESP32-PICO-series.pdf).

**Comprobación en la placa**

- [ ] Confirmar que la unidad ensayada corresponde al BOM con `C1042`.
- [ ] Registrar mínimo y rizado de `+3V3` durante arranque, radio activa y refresco de pantalla.
- [ ] Decisión: confirmar / descartar / condicionar.

### RV-02 — Booster de la pantalla configurado como UC, pero el panel usa SSD1683

**Confianza:** muy alta sobre la incompatibilidad; causalidad no demostrada · **Impacto posible:** alto, crítico si impide el refresco

- El diseño y el BOM usan `L1 = 10 µH` y `R10 = 0,47 Ω`.
- El panel declarado `GDEY042T81-T02` usa controlador `SSD1683`.
- La guía vigente de Good Display para DESPI-C02 asigna `10 µH + 0,47 Ω` a controladores UC y `47 µH + 2,2 Ω` a controladores SSD. Advierte que una selección incorrecta puede impedir el refresco.
- El esquema oficial DESPI-C02 V1.0 de 2018 usa una bobina fija de 10 µH y
  permite seleccionar `0,47 Ω` o `3 Ω`. La placa conserva precisamente la
  rama de `0,47 Ω`; éste es el origen más probable del valor.
- El circuito de aplicación del SSD1683 confirma `47 µH + 2,2 Ω`. La
  documentación posterior aclara que los valores antiguos no son universales.

**Funcionamiento e implicaciones**

- El SSD1683 controla Q4 mediante `GDR`. L1 almacena energía y los diodos y
  condensadores generan `PREVGH`, `PREVGL` y `VCOM`.
- `RESE` mide sobre R10 la corriente de Q4. Para una misma tensión de control,
  `0,47 Ω` permite unas 4,7 veces más corriente que `2,2 Ω`; con 10 µH la
  corriente también crece unas 4,7 veces más rápido que con 47 µH.
- Esto no implica 4,7 veces más tensión de salida: implica un lazo de corriente
  distinto y mayor esfuerzo durante el arranque del booster.
- Puede causar ausencia o degradación del refresco, poco contraste, ghosting,
  calentamiento y picos sobre `+3V3`. También puede funcionar por tolerancias y
  regulación interna, como ocurrió en el prototipo, sin validar sus márgenes.
- El funcionamiento observado de la pantalla reduce la probabilidad de un
  fallo inmediato, pero no refuta la incompatibilidad ni descarta estrés.

**Alcance revisado**

- Q4 `SI1308EDL` y los Schottky `MBR0530` coinciden con la recomendación
  actual.
- El mapeo completo del FPC confirma su orientación inversa respecto a J2.
  El circuito específico del panel GDEY042T81-T02 resuelve la diferencia entre
  el circuito genérico del SSD1683 y esta aplicación:
  - C6=4,7 µF/25 V coincide con el circuito de referencia del panel y se
    conserva; el valor genérico de 1 µF no prevalece sobre la aplicación
    específica.
  - C8 cargaba con 1 µF J2.21, correspondiente al pin 4 del panel, declarado
    `NC / keep open`.
  - C16 cargaba con 1 µF J2.6, correspondiente al pin 19 `VPP`, declarado
    `FOR TEST` y dejado abierto en el circuito de referencia del panel.
- El táctil FT6336U es un bloque I²C independiente. El booster no explica por sí
  solo su fallo salvo por una caída compartida de `+3V3`.

**Corrección aplicada en el esquemático**

- `L1 = 47 µH` y `R10 = 2,2 Ω`. L1 queda como `FHD4020S-470MT`, LCSC
  `C843300`: 660 mA, Isat 1,3 A y DCR 950 mΩ, con huella
  `L_Changjiang_FNR4020S` en el esquema.
- C6 se conserva en 4,7 µF/25 V; C8 y C16 se eliminan.
- La actualización, colocación y revisión en PCB permanecen pendientes.

**Fuentes:** [BOM fabricado](bom_jlcpb.csv), [panel GDEY042T81-T02](documentation/datasheets/GDEY042T81-T02.pdf), [SSD1683, circuito de aplicación pág. 46](documentation/datasheets/SSD1683.pdf), [producto oficial: IC SSD1683](https://www.good-display.com/product/473.html), [DESPI-C02 V1.0 de 2018](https://v4.cecdn.yun300.cn/100001_1909185148/DESPI-C02_SCH%20V1.0.pdf), [guía oficial DESPI-C02, secciones 3.1 y 4.1–4.4](https://www.good-display.com/companyfile/29.html).

**Comprobación en la placa**

- [ ] Confirmar referencia/revisión del panel realmente probado.
- [ ] Precisar si «funcionaba» incluía refrescos completos y repetibles.
- [ ] Medir `PREVGH`, `PREVGL`, `VCOM` y `+3V3` durante un refresco.
- [ ] Decisión: confirmar / descartar / condicionar.

### RV-03 — Falta el pull-up de `RTC_INT`

**Confianza:** muy alta · **Impacto posible:** alarma/temporizador RTC inoperante o inestable

- `RTC_INT` conecta únicamente `Y1.6` con `U2.IO27`; no hay resistencia de pull-up.
- La salida `INT` del RV-8803-C7 es open-drain y el fabricante indica expresamente que requiere pull-up.
- Un pull-up interno habilitado por firmware puede ocultar el fallo durante algunas pruebas, pero no existe durante reset ni está garantizado por el hardware.

**Corrección aplicada en el esquemático**

- R22=10 kΩ entre `RTC_INT` y `+3V3`; Y1.6 permanece en GPIO27.

**Fuentes:** [RV-8803-C7](documentation/datasheets/RV-8803-C7-datasheet.pdf), [manual de aplicación oficial, pág. 64](https://www.microcrystal.com/fileadmin/Media/Products/RTC/App.Manual/RV-8803-C7_App-Manual.pdf).

**Comprobación en la placa**

- [ ] Medir `RTC_INT` en reposo con el GPIO sin pull-up interno.
- [ ] Provocar una alarma/temporizador y comprobar nivel alto estable y flanco bajo.
- [ ] Decisión: confirmar / descartar / condicionar.

### RV-04 — Falta desacoplo en la salida interna del CP2104

**Confianza:** muy alta · **Impacto posible:** enumeración USB o UART intermitente

- Los pines `VDD` y `VIO` de U3 están unidos entre sí, pero esa red no llega a ningún condensador.
- `REGIN/VBUS` sí tiene C20; no sustituye el desacoplo de `VDD/VIO`.
- El circuito bus-powered del fabricante coloca `1–5 µF` y `0,1 µF` entre `VDD` y GND.

**Implicaciones**

- Aunque no se use para alimentar circuitos externos, `VDD` es la salida del
  regulador interno que alimenta al propio CP2104; `VIO` fija su alimentación
  de E/S.
- La omisión puede producir rizado, resets del CP2104, enumeración USB
  intermitente o programación UART poco fiable. No implica un fallo seguro.

**Corrección aplicada en el esquemático**

- U3 queda alimentado desde `+3V3`; C23=1 µF y C20=0,1 µF son sus desacoplos
  locales. `USB_VBUS` se detecta mediante R23/R24 sin alimentar U3.

**Fuente:** [CP2104, figura 8, pág. 17](documentation/cp2104-1397921.pdf).

**Comprobación en la placa**

- [ ] Medir U3 pin 6 (`VDD`) al conectar USB y durante enumeración/tráfico.
- [ ] Registrar fallos de enumeración, resets o rizado.
- [ ] Decisión: confirmar / descartar / condicionar.

### RV-05 — El reset táctil está conectado a `FLASH_CS` interno

**Confianza:** muy alta · **Impacto posible:** crítico

- `U2.IO16`, pin físico 25 del ESP32-PICO-D4, no es un GPIO disponible:
  está conectado internamente a `FLASH_CS`.
- Esa señal sale como `RES_TOUCH` hasta JP1. Con JP1 cerrado, comparte el reset
  del FT6336U y su pull-up R11 de 10 kΩ. La actividad de la flash puede resetear
  el táctil y éste carga una señal interna crítica del ESP32.
- Con JP1 abierto no existe esa carga directa: R11 mantiene alto el reset del
  táctil. Aun así, el FT6336U no recibe su secuencia recomendada de reset bajo
  durante el encendido y queda un stub de PCB conectado a `FLASH_CS`.
- Es una explicación fuerte de `OBS-01` únicamente si la placa ensayada tenía
  JP1 cerrado.

**Corrección aplicada en el esquemático**

- JP1 y la conexión a IO16 se eliminan. `RES_TOUCH_CTRL` pasa a GPIO4 mediante
  R11=1 kΩ, C25=10 µF y D8; el firmware lo maneja en drenador abierto.

**Fuentes:** [ESP32-PICO-D4, tabla de pines](documentation/datasheets/ESP32-PICO-series.pdf), [FT6336U](documentation/datasheets/FT6336U.pdf).

**Dato pendiente**

- [ ] Confirmar si JP1 estaba soldado/cerrado en la unidad probada.
- [ ] Cierre: verificar en esquema y PCB que IO16 queda sin red, pista ni
      componente externo.

### RV-06 — `ACC_INT2` puede forzar el strap de 1,8 V de la flash

**Confianza:** muy alta sobre la conexión · **Impacto condicionado:** crítico

- `LIS3DH.INT2` llega a `GPIO12/MTDI` del ESP32.
- El ESP32 muestrea GPIO12 en cada reset: bajo selecciona 3,3 V para
  `VDD_SDIO`; alto selecciona 1,8 V. La flash interna del PICO-D4 trabaja a
  3,3 V.
- El LIS3DH arranca con INT2 push-pull a nivel bajo, por lo que un encendido en
  frío puede funcionar. Si el firmware deja INT2 alto y se reinicia sólo el
  ESP32 sin cortar `+3V3`, el sensor conserva ese estado durante el muestreo y
  puede provocar brownout o impedir el arranque.

**Corrección aplicada en el esquemático**

- `ACC_INT2` pasa a GPIO34, entrada sin función de strap.
- Cierre: comprobar arranque en frío y reset sólo de `EN` con INT2 activo.

**Fuentes:** [LIS3DH](documentation/lis3dh_datasheet.pdf), [guía de hardware ESP32](documentation/esp32_hardware_design_guidelines_en.pdf).

### RV-07 — Faltan los condensadores locales del TP4054

**Confianza:** muy alta · **Impacto posible:** carga inestable

- La aplicación típica del componente exacto pide 1 µF en su entrada de
  alimentación y 10 µF en la salida de batería, ambos locales a U5. En el
  símbolo del esquema son `VDD` y `VBAT`. Ninguno está presente.
- C2 es 0,1 µF sobre `VBAT_RAW` para el RTC; no sustituye los 10 µF locales de
  U5. C20 y C23 desacoplan `+3V3` en el CP2104, no `USB_VBUS`.

**Corrección aplicada en el esquemático**

- C29=1 µF entre U5.4 (`VDD`) y GND y C28=10 µF entre U5.3 (`VBAT`) y
  GND. En PCB deben quedar pegados a U5 y con retornos cortos.
- Cierre: comprobar estabilidad en U5.4 y U5.3 durante conexión y carga.

**Fuente:** [TP4054 JSMSEMI, aplicación típica](documentation/datasheets/TP4054-JSMSEMI-C5381776.pdf).

### RV-08 — El cargador no separa la batería de la carga del sistema

**Confianza:** muy alta sobre la topología · **Impacto condicionado:** alto

- U5 conecta `BAT` directamente a `VBAT_RAW`; no existe power-path ni reparto
  de carga.
- Si el equipo permanece encendido mientras carga, el TP4054 mide conjuntamente
  batería y consumo del sistema. La terminación puede retrasarse o no producirse
  y la batería puede aportar los picos que excedan la corriente del cargador.
- R19 se cambia de 10 kΩ a 5,1 kΩ: programa unos 196 mA para la batería prevista
  de 600–700 mAh y reutiliza el valor de R16/R17. La terminación ocurre
  alrededor de C/10, por lo que una carga activa superior a unos 20 mA puede
  impedirla.
- La batería incorpora protección propia, confirmado por el autor.
- El datasheet del TP4054 contiene una tabla ambigua con un límite de 4,28 V y
  grados A/B poco claros. Sin embargo, LCSC y JLCPCB identifican el componente
  exacto `C5381776` como 4,2 V. Se mantiene U5; medir la tensión final forma
  parte de la validación, no de una corrección del esquema.

**Decisión cerrada**

- No añadir power-path. U5 permanece conectado directamente a `VBAT_RAW`.
- Especificar carga normal con el equipo apagado. Mantenerlo encendido
  temporalmente para programar es admisible, aunque no garantiza terminación
  de carga mientras exista consumo.
- Cierre: validar terminación y corriente real con el sistema apagado y, si se
  permite, encendido.

### RV-09 — `RTC_EVI` queda sin nivel definido con el MCU apagado

**Confianza:** muy alta · **Impacto posible:** eventos espurios del RTC

- `Y1.EVI` conecta únicamente con GPIO15. El RV-8803 permanece alimentado desde
  `VBAT_RAW`, mientras el ESP32 y sus polarizaciones desaparecen al apagar el
  latch.
- El fabricante indica que EVI no debe quedar flotante.

**Corrección aplicada en el esquemático**

- `RTC_EVI` se desconecta de GPIO15 y queda inactiva mediante R21=100 kΩ a
  `VBAT_RAW`, con TP1 para un posible uso futuro.
- Cierre: medir un nivel definido en EVI con el latch encendido y apagado.

**Fuente:** [manual RV-8803-C7](RV-8803/RV-8803-C7_App-Manual.pdf).

### RV-10 — El RC de `EN` conserva la recomendación antigua de 100 nF

**Confianza:** muy alta sobre la desviación · **Impacto posible:** arranque menos robusto

- R3 = 10 kΩ y C17 = 100 nF forman el reset de encendido del ESP32.
- La guía vigente de Espressif recomienda 10 kΩ + 1 µF para mejorar la
  estabilidad de encendido.

**Corrección aplicada en el esquemático**

- C17=1 µF con R12=10 kΩ. El arranque será algo más lento y puede exigir
  mantener el pulsador durante más tiempo; no sustituye la corrección de
  `PWR-02`.
- Cierre: comprobar subida limpia de `EN`, arranque manual y reset de
  programación.

**Fuente:** [guía de hardware ESP32](documentation/esp32_hardware_design_guidelines_en.pdf).

## Condiciones abiertas, no clasificadas como fallo

- Los pull-ups I²C R7/R8 se cambian de 10 kΩ a 5,1 kΩ para aumentar el margen
  de subida a 400 kHz sin añadir un valor nuevo al BOM.
- **U4 / presupuesto de `+3V3`: CERRADO.** Sin motor, el pico documentado
  queda en unos 401 mA: ESP32 Wi-Fi 370 mA, panel 5,6 mA típicos, táctil
  4,32 mA típicos, CP2104 18,73 mA máximos y unos 2 mA para LIS3DH y
  polarizaciones. El motor ya toma energía de `VSYS`, por lo que no carga U4.
  El ADP2108 de 600 mA y C21/C22=10 µF son adecuados para esta topología.
- Con batería baja, U4 entra correctamente en modo 100 % de ciclo, pero deja
  de regular a 3,3 V y la salida sigue a la batería con pérdidas. Debe
  verificarse que el firmware apaga el equipo antes de que `+3V3` caiga por
  debajo de los 3,0 V recomendados para el ESP32; la protección de la batería
  por sí sola puede actuar demasiado tarde.
- La medida de batería en ADC1 es válida; añadir 100 nF local sería una mejora
  de filtrado, no un fallo demostrado.
- `MOTOR-01`: Q2 ya está corregido al pinout B1/E2/C3 del S8050 montado
  (`C20069125`). El motor toma energía de `VSYS` mediante D9=MBR0530;
  C1=0,1 µF y R25=100 kΩ. La etapa queda validada eléctricamente para el
  candidato de 70 mA nominales y 90 mA de arranque. Quedan la referencia de
  compra, la comprobación mecánica y la PCB.

## Comprobaciones descartadas

- **D5/BAT54C:** el pin común 3 va al pulsador; D5.1 sí permite descargar la puerta de Q5. La topología de encendido es coherente.
- **LIS3DH:** símbolo, desacoplo, `CS` y `SDO/SA0` son coherentes; `ACC_INT2`
  ya está corregida de GPIO12 a GPIO34 según `RV-06`.
- **ESP32-PICO-D4:** `SD2/GPIO9` y `SD3/GPIO10` no están ocupados por la flash interna de esta variante. El condensador de `VDD_SDIO` mostrado en el esquema interno forma parte del SiP; no se cuenta como componente externo ausente.
- **CP2104:** el pad expuesto central es GND opcional según el fabricante; que
  esté sin conectar no constituye un fallo. El pinout y la función de Q7 son
  correctos.
- **USB-C y RF:** CC1/CC2, protección ESD, pares USB, adaptación integrada del
  PICO-D4 y keepout de antena no muestran otro fallo eléctrico evidente.

## Chequeos automáticos

- ERC: sin fallo eléctrico útil; 105 de 107 avisos son desajustes de símbolos de librería.
- DRC: 0 conexiones sin enrutar; las 24 infracciones están excluidas y la paridad con el esquemático sólo muestra diferencias de metadatos.
