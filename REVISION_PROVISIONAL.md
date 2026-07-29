# Revisión provisional independiente

**Autor:** Cosmin Dobrescu
**Fecha:** 2026-07-29
**Estado:** pendiente de validación del autor. No aplicar correcciones desde este documento.

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

**Corrección prevista, no autorizada**

- Sustituir L2 por una bobina de potencia de 1 µH, baja DCR y corriente
  admisible superior al pico; las referencias del ADP2108 parten de 800 mA.

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
  Dentro del multiplicador aparecen tres discrepancias adicionales:
  - C6 es 4,7 µF; el circuito SSD1683 prescribe 1 µF para ese condensador
    volante.
  - C8 carga con 1 µF el pin 4 del panel, declarado `NC / keep open`.
  - C16 carga con 1 µF `VPP`, declarado pin de prueba y dejado abierto en la
    aplicación SSD1683.
- El táctil FT6336U es un bloque I²C independiente. El booster no explica por sí
  solo su fallo salvo por una caída compartida de `+3V3`.

**Corrección prevista, no autorizada**

- Para el SSD1683: `L1 = 47 µH`, apantallada y con corriente admisible de al
  menos 500 mA; `R10 = 2,2 Ω`, 1 %, al menos 0,05 W.
- Cambiar C6 a 1 µF y retirar C8/C16 tras confirmar que la revisión física del
  panel probado corresponde al pinout archivado.

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

**Corrección prevista, no autorizada**

- Añadir un pull-up externo de `RTC_INT` a `+3V3`; dimensionarlo antes de fijar
  el valor.

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

**Corrección prevista, no autorizada**

- Mantener `VDD` y `VIO` unidos y añadir en esa red `1–5 µF` más `0,1 µF`,
  próximos a U3.

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

**Corrección prevista, no autorizada**

- Eliminar por completo `RES_TOUCH` de IO16 y su stub.
- Llevar el reset táctil a un GPIO libre y seguro, por ejemplo GPIO4, con un
  nivel definido que lo mantenga bajo durante el encendido; elevarlo por
  firmware después de al menos 5 ms.

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

**Corrección prevista, no autorizada**

- Mover `ACC_INT2` a un GPIO de entrada sin función de strap; GPIO34 está libre
  en el diseño actual.
- Cierre: comprobar arranque en frío y reset sólo de `EN` con INT2 activo.

**Fuentes:** [LIS3DH](documentation/lis3dh_datasheet.pdf), [guía de hardware ESP32](documentation/esp32_hardware_design_guidelines_en.pdf).

### RV-07 — Falta desacoplo local en la entrada del TP4054

**Confianza:** muy alta · **Impacto posible:** carga inestable

- U5 tiene los 10 µF de batería/salida en C2, pero no el condensador de 1 µF
  recomendado junto a `VCC`.
- C20 también es de 1 µF sobre `USB_VBUS`, pero está colocado junto al CP2104,
  a unos 32 mm de U5; no constituye desacoplo local del cargador.

**Corrección prevista, no autorizada**

- Añadir 1 µF cerámico entre U5.4 y GND, pegado al encapsulado y con retorno
  corto.
- Cierre: comprobar tensión estable en U5.4 durante conexión y carga.

**Fuente:** [TP4054 JSMSEMI, aplicación típica](documentation/datasheets/TP4054-JSMSEMI-C5381776.pdf).

### RV-08 — El cargador no separa la batería de la carga del sistema

**Confianza:** muy alta sobre la topología · **Impacto condicionado:** alto

- U5 conecta `BAT` directamente a `VBAT_RAW`; no existe power-path ni reparto
  de carga.
- Si el equipo permanece encendido mientras carga, el TP4054 mide conjuntamente
  batería y consumo del sistema. La terminación puede retrasarse o no producirse
  y la batería puede aportar los picos que excedan la corriente del cargador.
- R19 = 10 kΩ configura aproximadamente 100 mA; falta validar ese valor con la
  batería exacta.

**Decisión necesaria**

- O bien impedir por especificación que el sistema funcione durante la carga,
  o bien rediseñar con power-path/load sharing. Confirmar también que la
  batería incorpora protección contra sobredescarga y cortocircuito: el TP4054
  sólo es cargador.
- Cierre: validar terminación y corriente real con el sistema apagado y, si se
  permite, encendido.

### RV-09 — `RTC_EVI` queda sin nivel definido con el MCU apagado

**Confianza:** muy alta · **Impacto posible:** eventos espurios del RTC

- `Y1.EVI` conecta únicamente con GPIO15. El RV-8803 permanece alimentado desde
  `VBAT_RAW`, mientras el ESP32 y sus polarizaciones desaparecen al apagar el
  latch.
- El fabricante indica que EVI no debe quedar flotante.

**Corrección prevista, no autorizada**

- Añadir una polarización externa que mantenga EVI inactivo incluso con el
  ESP32 apagado; evitar que esa polarización pueda realimentar `+3V3`.
- Cierre: medir un nivel definido en EVI con el latch encendido y apagado.

**Fuente:** [manual RV-8803-C7](RV-8803/RV-8803-C7_App-Manual.pdf).

### RV-10 — El RC de `EN` conserva la recomendación antigua de 100 nF

**Confianza:** muy alta sobre la desviación · **Impacto posible:** arranque menos robusto

- R3 = 10 kΩ y C17 = 100 nF forman el reset de encendido del ESP32.
- La guía vigente de Espressif recomienda 10 kΩ + 1 µF para mejorar la
  estabilidad de encendido.

**Corrección prevista, no autorizada**

- Cambiar C17 a 1 µF. El arranque será algo más lento y puede exigir mantener
  el pulsador durante más tiempo; no sustituye la corrección de `PWR-02`.
- Cierre: comprobar subida limpia de `EN`, arranque manual y reset de
  programación.

**Fuente:** [guía de hardware ESP32](documentation/esp32_hardware_design_guidelines_en.pdf).

## Condiciones abiertas, no clasificadas como fallo

- Los pull-ups I²C de 10 kΩ pueden ser débiles a 400 kHz por la longitud del
  bus y los flex; resolver con frecuencia real y medida de tiempo de subida.
- El ADP2108 entrega como máximo 600 mA a ESP32, pantalla y motor. Cerrar el
  presupuesto con la corriente real del motor y los picos de refresco.
- La medida de batería en ADC1 es válida; añadir 100 nF local sería una mejora
  de filtrado, no un fallo demostrado.
- Q2 debe corregirse por `MOTOR-01`; al hacerlo, verificar corriente de
  arranque, saturación y si 1 µF directamente sobre el motor es compatible con
  el PWM.

## Comprobaciones descartadas

- **D5/BAT54C:** el pin común 3 va al pulsador; D5.1 sí permite descargar la puerta de Q5. La topología de encendido es coherente.
- **LIS3DH:** símbolo, desacoplo, `CS` y `SDO/SA0` son coherentes. El problema
  restante es su conexión externa a GPIO12, documentada en `RV-06`.
- **ESP32-PICO-D4:** `SD2/GPIO9` y `SD3/GPIO10` no están ocupados por la flash interna de esta variante. El condensador de `VDD_SDIO` mostrado en el esquema interno forma parte del SiP; no se cuenta como componente externo ausente.
- **CP2104:** el pad expuesto central es GND opcional según el fabricante; que
  esté sin conectar no constituye un fallo. El pinout y la función de Q7 son
  correctos.
- **USB-C y RF:** CC1/CC2, protección ESD, pares USB, adaptación integrada del
  PICO-D4 y keepout de antena no muestran otro fallo eléctrico evidente.

## Chequeos automáticos

- ERC: sin fallo eléctrico útil; 105 de 107 avisos son desajustes de símbolos de librería.
- DRC: 0 conexiones sin enrutar; las 24 infracciones están excluidas y la paridad con el esquemático sólo muestra diferencias de metadatos.
