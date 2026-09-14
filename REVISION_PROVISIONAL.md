# Revisión provisional independiente

**Autor:** Cosmin Dobrescu
**Última actualización:** 2026-09-14
**Estado:** revisión digital completada; pruebas del prototipo pendientes.
El README mantiene el checklist vigente y las decisiones aceptadas.

No se repiten aquí los cinco fallos ya anotados en el esquemático.

## BOM y posiciones de montaje — 2026-09-14

Revisión con Fabrication Toolkit instalado, traducción automática activada.
Campos corregidos y documentados; las propuestas siguientes **no se han
aplicado**. No se han cambiado valores, huellas, giros ni pistas en esta pasada.

### BOM-01 — Reducir variedad sin modificar el enrutado

- **Estado: DESCARTADA PARA ESTA REVISIÓN por el autor (2026-09-14).** Se
  evaluó cambiar R23 de 24 kΩ a
  5,1 kΩ (`C23186`) y R24 de 47 kΩ a 10 kΩ (`C25804`), ambas 0603, 1 %.
  Reutiliza dos referencias existentes y elimina `C23352` y `C25819`:
  **39 → 37 códigos**, manteniendo la cantidad de componentes.
- **Evidencia:** el divisor de detección USB pasa de 3,3099 V a 3,3113 V
  con entrada de 5 V. A 5,25 V y tolerancias de 1 % en el peor sentido,
  pasa de 3,4988 V a 3,5002 V, por debajo del límite de 3,6 V con U3 apagado.
  No amplía el rango USB admitido ni protege frente a sobretensiones.
- **Coste eléctrico:** consumo del divisor de 70 a 331 µA mientras hay USB
  (+0,261 mA desde USB, no desde la batería). La resistencia equivalente
  baja de 15,89 a 3,38 kΩ; no empeora el error por fugas de entrada.
- **Justificación:** cálculo propio sobre la figura 9, nota 5 del
  [CP2104](https://www.silabs.com/documents/public/data-sheets/cp2104.pdf),
  que permite un divisor funcionalmente equivalente; 5,1 kΩ/10 kΩ no es
  una pareja prescrita por el fabricante. No se promete ahorro económico
  concreto: la mejora comprobada es reducir variedad.
- **Decisión:** conservar R23=24 kΩ (`C23352`) y R24=47 kΩ (`C25819`). El
  ahorro observado era de unos 2,6 EUR por pedido Standard, insuficiente para
  cambiar una PCB ya terminada. Para diseños futuros, priorizar piezas Basic,
  compatibilidad Economic y reutilización de referencias desde el inicio.

Otros resultados del chequeo:

- **Sólo agrupación:** el exportador produce 41 líneas para 39 códigos porque
  C17/C23/C29 dicen `1uF` y sus equivalentes `1uF/50V`; C30 dice `100nF` y
  sus equivalentes `0.1uF`. Uniformar esos textos reduciría a 39 líneas,
  pero no elimina ninguna referencia de compra; ya comparten código LCSC.
- **Diodos: no unificar de oficio.** D1/D8/D9 y D2–D4 usan dos MBR0530 de
  fabricantes distintos. D9 se eligió para reducir la tensión del motor:
  no basta compartir encapsulado y tensión nominal para asegurar la misma
  caída. Cambiar sólo D1/D8 no reduce variedad porque D9 conserva su código.
- **C18:** su 100 nF es 0402, frente a los demás 0603. Unificarlo exige
  cambiar huella y revisar espacio/pistas; no compensa tocar esta PCB sólo
  por ese motivo. No se propone alterar temporizaciones, feedback o
  componentes de potencia para reutilizar valores a costa de su función.

### FAB-02 — Orientaciones y centros del archivo de posiciones

- **Estado: exportador comprobado y previsualización revisada por el autor.**
  Las 85 referencias del BOM tienen posición, todas en cara superior y sin
  duplicados. El CPL incluye además H1–H4, J5/J6 y TP1, excluidos del BOM:
  son siete entradas sin pieza a montar, no siete componentes sin código.
  M1 queda fuera de ambos archivos por compra/montaje independiente.
- El exportador usa el centro de la caja de pads y el origen auxiliar,
  invirtiendo Y para el CPL. Respecto al ancla de KiCad desplaza el centro
  calculado de J3 0,175 mm a la izquierda, SW1 0,075 mm a la izquierda y
  AE1 0,025 mm a la izquierda/0,030 mm arriba. No demuestra descentrado:
  comprobar el cuerpo sobre sus pads, no igualar coordenadas por intuición.
- Se contrastaron las geometrías numeradas de 26 componentes con bibliotecas
  EasyEDA descargadas por el MCP. **Su cero angular no certifica el cero de
  montaje de JLCPCB.** Q4 no tiene regla SOT-323 en el exportador; Y1 usa
  una huella personalizada; D6 recibe la regla genérica SOT-143 y U2/U3 la
  QFN. Son comprobaciones prioritarias, no giros erróneos demostrados.

Referencia rápida para la previsualización, mirando la PCB por arriba con
la misma orientación que KiCad (X crece a la derecha, Y hacia abajo):

| Pieza | Ángulo CPL actual | Referencia física que debe coincidir con la PCB |
| --- | --- | --- |
| Q4 | 270° | Gate/pad 1 arriba a la derecha; drenador/pad 3 abajo |
| D6 | 270° | Común GND/pad 1 abajo a la izquierda |
| U2 | 270° | Pad 1 en el extremo inferior del lateral derecho |
| U3 | 90° | Pad 1 en el extremo superior del lateral izquierdo |
| Y1, si se monta | 180° | Pad 1 abajo a la derecha |
| D7 | 180° | Cátodo a la derecha, en el pad 1 de KiCad |

En D7 se descartó una falsa alarma de 180° al comprobar el símbolo:
EasyEDA llama **2** al cátodo y KiCad **1**. Comparar sólo los números
habría confundido una convención distinta con un fallo de polaridad.
J1/J2/J3 y SW1 coinciden en orientación de pads con sus huellas importadas;
revisar también la boca del USB y las entradas de los flex en el visor.
No se pudo obtener la huella EasyEDA de AE1: contrastar con su plano y el
componente del acopio al preparar su montaje.

**Resultado de la previsualización (2026-09-14):** el autor encontró correcto
el posicionamiento del resto de componentes y corrigió manualmente únicamente
la orientación de Y1. El ángulo de corrección no quedó registrado, por lo que
todavía no se puede fijar un `FT Rotation Offset` fiable para automatizar Y1
en otro pedido. No se ha girado su huella física ni fijado offsets nuevos.
En futuros pedidos, revisar de nuevo la previsualización con el código exacto
de cada pieza antes de aceptar el montaje.
Los ángulos de esta tabla dependen de mantener activada la traducción
automática de este exportador. No son una tabla para el exportador nativo.
Fuentes: [atributos de Fabrication Toolkit](https://github.com/bennymeg/Fabrication-Toolkit#attributes)
y [guía de corrección de posiciones de JLCPCB](https://github.com/JLCPCB/JLCPCB-SMT-Assembly-Components-orientation-fix).

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

## Antecedentes del prototipo y correcciones RV-01–RV-10

Las descripciones de fallos siguientes corresponden al diseño anterior.
Las correcciones indicadas en esquema ya se trasladaron a la PCB y se
revisaron digitalmente; las comprobaciones físicas no se dan por superadas.
RV-08 es una decisión aceptada, no una propuesta de rediseño pendiente.

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

- El conjunto U4/L2 fue sustituido por U6=`TLV62568DBVR` y
  L3=`FTC252012S1R0MBCA`, LCSC `C5832370`: 1 µH, 4 A, Isat 5,6 A y
  DCR 35 mΩ, huella `L_Changjiang_FTC252012S`. Aplicado también en PCB.

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
- Aplicado también en PCB; el retorno Q4–R10 está compactado (`RV-13`).

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
  Mejora posterior aplicada por el autor: R28=1 kΩ en paralelo con R11
  reduce la resistencia efectiva a 500 Ω y aumenta el margen del nivel bajo.

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
  GND, colocados junto a U5 con retornos locales revisados en PCB.
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

- Pull-ups I²C R7/R8=5,1 kΩ ya aplicados. Usar 100 kHz hasta validar los
  flancos del conjunto PCB/flex; no se garantiza 400 kHz sólo por ese valor.
- **Regulador: selección cerrada.** U6=`TLV62568DBVR` de 1 A sustituye al
  ADP2108; L3 y C31/C32 son los componentes actuales. El motor toma energía
  de `VSYS`. Presupuesto y márgenes en `AUDITORIA_COMPONENTES.md`.
- Con batería baja, verificar el apagado por firmware antes de perder el
  margen de alimentación del ESP32; la protección de batería no sustituye
  ese umbral, pendiente de calibración.
- La medida de batería en ADC1 es válida; añadir 100 nF local sería una mejora
  de filtrado, no un fallo demostrado.
- `MOTOR-01`: Q2 ya está corregido al pinout B1/E2/C3 del S8050 montado
  (`C20069125`). El motor toma energía de `VSYS` mediante D9=MBR0530;
  C1=0,1 µF y R25=100 kΩ. PCB aplicada; dimensionado condicionado al candidato
  de 70 mA nominales y 90 mA de arranque. Quedan referencia de compra,
  corriente real y comprobación mecánica; no se considera ensayado.

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

## Revisión de enrutado — 2026-09-13

### Cierre de revisión sobre el guardado actual (2026-09-13)

Último contraste de documentación, netlist actual y referencias de fabricante:
sin incompatibilidad grave nueva detectada. DRC repetido con relleno de zonas
en memoria: 0 infracciones activas, 18 exclusiones, 0 conexiones pendientes y
0 diferencias con el esquema. La mejora posterior R11/R28 ya está aplicada
por el autor en esquema y PCB; el asistente completa sólo el campo de compra
de R28 y excluye M1 de BOM/posiciones, sin tocar geometría ni conexiones.
85 componentes tienen código de compra coincidente en esquema y PCB.
Persisten las condiciones del motor y las pruebas del reset descritas en
`AUDITORIA_COMPONENTES.md`; no se considera superada ninguna prueba física.
Corregida en README la selección de D2–D4 a `C236079` y separada del BOM histórico.

Referencia histórica de cobre revisado, anterior a R28 y a la nota RF en `Cmts.User`:
SHA256 `e97267a49af78ba223983d335d8b887f7e7fd721cb2c1a0c7f3eb2a174cb04b4`.
La nota documental no cambia cobre, componentes, taladros ni máscaras.

- **DRC:** 0 conexiones pendientes, 0 diferencias con el esquema y 0 infracciones
  no excluidas tras el ajuste autorizado de reglas. Taladro mínimo de 0,20 mm;
  `sacre.kicad_dru` exige diámetro exterior ≥0,45 mm si el taladro es <0,30 mm.
  Quedan 18 avisos excluidos previos, no 22 tras corregir J3. Prueba aislada:
  admite 0,20/0,45 y 0,30/0,40 mm; rechaza 0,20/0,40 y taladro de 0,15 mm.
  Geometría de PCB intacta.
- **Fabricación:** las 159 vías son pasantes: 135 de 0,30/0,50 mm, 20 de
  0,40/0,80 mm y 4 de 0,20/0,45 mm (taladro/diámetro exterior). Ninguna
  incumple las combinaciones sin recargo por vía pequeña publicadas por JLC.
- **RV-13:** Q4.2–R10.2 corregido por el autor: 2,34 mm a 0,25 mm, sin vías,
  frente a 15,94 mm. R10 retorna al cobre local GND. No queda propuesta de
  recolocación; validar el booster en el prototipo.
- **RV-17:** ahora hay 16 vías bajo U2.49; ningún taladro se solapa con las
  nueve ventanas de pasta. Margen mínimo de las cuatro interiores: 0,020–0,034 mm.
  El autor decide conservar la pasta y asumir este margen; no modificarla
  ni contratar relleno automáticamente. No equivale a soldadura validada.
- **ERC:** 122 avisos de desajuste de símbolos y 1 aviso de pin no alimentado
  en AE1.2. Este pin está modelado como entrada y unido a GND en esquema/PCB;
  no se ha encontrado una desconexión física que justifique cambiar el circuito.
  El ERC no se declara limpio. Las seis huellas con aviso de biblioteca
  conservan numeración, posición, tamaño, forma y taladro de sus pads frente
  a la biblioteca; no se han actualizado indiscriminadamente.
- **Gerber/taladros:** exportados de nuevo a `tmp/final-review-fab/` para
  inspección, con zonas recalculadas en memoria. Revisadas las cuatro capas
  de cobre, máscaras, pasta, serigrafía y contorno con un lector independiente.
  NPTH: cuatro agujeros de 2,70 mm y los dos de J3 de 0,70 mm; cuatro ranuras
  USB permanecen metalizadas. No usar los archivos antiguos de `production/`.
- **Retornos/RF:** In1 y B.Cu mantienen planos GND continuos; pista RF de
  0,26 mm y 14,80 mm con In1 debajo. Desacoplos y bloques USB/regulador/booster
  revisados sin otro fallo concreto detectado. Se conservan las decisiones
  aceptadas sobre D6, U6 y SW1. Encaje físico, EMC/RF y funcionamiento real
  siguen pendientes del autor/prototipo; no los certifica esta revisión.

### RV-11 — Comprobar los 50 Ω de la pista RF con el apilado de fabricación

- **Estado: CORREGIDO EN PCB por autorización del autor; revisión visual y
  validación RF pendientes.** Decisión y parámetros definitivos de este ajuste
  registrados en README, sección «Pista RF».
- **Evidencia:** con JLC04101H-7628, H=0,2104 mm y separación lateral de
  0,15 mm, el servicio JLC con máscara devuelve 44,87 Ω para 0,3341 mm y
  50,15 Ω para 0,26 mm. Es cálculo nominal, no un fallo funcional medido.
  Las aproximaciones anteriores de 33 Ω (apilado antiguo de KiCad) y
  50,8 Ω (modelo simplificado sin máscara) no describen este cálculo final.
- **Acción aplicada:** tres segmentos y dos arcos de `/ANT_RF` pasan a
  0,26 mm; zonas rellenadas. Recorrido de 14,80 mm, huella/meandro y resto
  del diseño conservados. GND lateral ≈0,1505 mm, In1 continuo debajo.
- **Cierre parcial:** DRC sin infracciones no excluidas, sin conexiones
  pendientes ni discrepancias de esquema. El lote
  TG135 y la antena completa siguen necesitando validación física.
- [ ] PCB corregida y revisada por el autor; esquemático sin cambio propuesto.

### RV-12 — Retornos locales de C31/U6 y C30

- **Estado: C31/C30 CORREGIDOS; U6 CONSERVADO.** Mejora preventiva de
  retornos, no fallo funcional demostrado. C31 tiene vía GND a 1,14 mm del
  centro de su pad de masa (antes 4,16 mm), y C30 a 1,62 mm (antes 4,67 mm).
  Ambas conectan al cobre local. U6.2 conserva la vía a 2,48 mm y cobre GND
  continuo; el autor indica que no puede acercarlo más. No queda recolocación
  solicitada ni hay que volver a añadir las vías de C31/C30.
  C26/C27 están razonablemente colocados: conexión ancha y corta a U2.3/4,
  con una vía GND aproximadamente a 1 mm de C27.2.
- **Cierre:** inspección de los retornos con zonas rellenadas y DRC; no atribuir
  a este detalle los reinicios antiguos sin medir. Fuentes:
  [TLV62568, §10](https://www.ti.com/lit/ds/symlink/tlv62568.pdf) y
  [guía ESP32, colocación del desacoplo](documentation/esp32_hardware_design_guidelines_en.pdf).
- [ ] PCB corregida y revisada por el autor; esquemático sin cambio propuesto.

### RV-13 — Compactar el nodo conmutado del booster

- **Estado: CORREGIDO EN PCB; prueba de refresco pendiente.** Se ha acortado
  Q4.2–R10.2 de 15,94 mm a 2,34 mm, ahora a 0,25 mm y sin vías. Ese tramo
  conduce la corriente del transistor: no es sólo una señal de medida.
- **Evidencia actual:** Q4.3–L1.1 mide 8,88 mm; el nodo `Net-(D4-A)` suma
  13,00 mm a 0,60 mm. C5.1–L1.2 mide 5,19 mm. El retorno de R10 llega al
  cobre GND local. Se revisó el conjunto tras la recolocación, no sólo R10.
- **Acción:** ninguna recolocación adicional solicitada. Fue una mejora
  preventiva; no demuestra la causa de los fallos del prototipo anterior.
- **Cierre:** conexiones y DRC comprobados; validar refresco en prototipo.
  Criterio general:
  [TI SNVA731, §6](https://www.ti.com/lit/an/snva731/snva731.pdf), aplicado al
  circuito específico del panel, no como sustituto de su esquema.
- [ ] PCB corregida y revisada por el autor; esquemático sin cambio propuesto.

### RV-14 — D6 está junto al CP2104, no a la entrada USB

- **Revisión sobre guardado 19:29 (2026-09-13):** D6 mantiene su vía GND a
  1,11 mm y cobre local conectado. No se identifica otra corrección concreta
  necesaria en esta inspección; mantener posición por la faja. El ensayo ESD
  permanece pendiente, no equivale a un trabajo de enrutado pendiente.
- **Estado: POSICIÓN ACEPTADA; ensayo ESD pendiente.** Hipótesis: la descarga puede
  recorrer y acoplarse a más circuito antes de alcanzar el protector.
- **Evidencia:** los pads de datos de J3 están en x=55,66 mm y los de D6 en
  x=66,35 mm: unos 10,7 mm de separación horizontal. U3 queda inmediatamente
  después. D6 sí está conectado y dispone de una vía GND a unos 1,1 mm.
- **Decisión del autor:** la faja de pantalla impide acercar D6. Se retira la
  propuesta de recolocación; conservar la posición y revisar únicamente los
  ramales y el retorno GND. Restricción mecánica aceptada, no validación ESD.
- **Cierre:** revisar entrada/retorno de descarga y DRC; la inmunidad sólo se
  valida mediante ensayo ESD. [Criterio de colocación del fabricante Toshiba](https://toshiba.semicon-storage.com/eu/semiconductor/knowledge/e-learning/basics-of-tvs-diodes/chap5/chap5-1.html).
- [ ] PCB corregida y revisada por el autor; esquemático sin cambio propuesto.

### RV-15 — Cuatro taladros de posicionamiento excluidos del DRC

- **Corrección autorizada (2026-09-13):** los dos agujeros de posicionamiento
  de J3 pasan a NPTH en PCB y biblioteca local, conservando diámetro de
  0,70 mm y posiciones. Anclajes soldados y SW1 intactos. Desaparecen las
  cuatro infracciones de J3 por anular/padstack; sin errores nuevos.
- **Estado: J3 CORREGIDO; SW1 CONSERVADO POR DECISIÓN DEL AUTOR.** La huella
  importada trataba los agujeros mecánicos como PTH sin anular. El plano de
  J3 representa los posicionadores separados de los anclajes soldados.
- **Aclaración del autor:** los avisos de SW1 se excluyeron por cómo venía la
  huella. No modificar SW1 ni sus exclusiones en esta fase.
- **Cierre parcial:** los dos agujeros de J3 salen en el archivo NPTH y sus
  cuatro anclajes permanecen PTH. Aprobación física del autor pendiente.
- [ ] PCB corregida y revisada por el autor; esquemático sin cambio propuesto.

**Sin otro problema concreto detectado en esta pasada:** conexiones actuales
de GPIO/reset/latch, polaridad y pinout del motor, alimentación principal y
desacoplo local C26/C27. In1 y B.Cu mantienen planos de masa conectados; esto
no garantiza por sí solo todos los retornos. No se certifican impedancias USB,
EMC, temperaturas ni encaje de batería/flex/carcasa mediante DRC o estas vistas.

## Compatibilidad JLCPCB — configuración de 1 mm (2026-09-13)

Apilado y reglas actuales en README. Los informes DRC rellenan zonas en
memoria sin guardar cobre: al preparar el pedido, rellenar zonas y generar
archivos nuevos. Las exportaciones temporales no son el paquete definitivo.

### RV-16 — Tres vías GND demasiado próximas a la ranura

- **Estado: CORREGIDO por el autor; verificado con DRC** sobre el guardado
  19:29 del 2026-09-13. Las tres infracciones de borde ya no aparecen con
  límite 0,20 mm. La evidencia siguiente describe el estado anterior.
- **Evidencia:** JLC exige cobre–borde fresado ≥0,20 mm. Las vías de diámetro
  0,50 mm en (89,12;148,77), (91,65;148,77) y (95,77;148,77) mm dejan
  0,1904–0,1923 mm según DRC. Antes se comprobaban sólo 0,10 mm.
- **Acción aplicada:** el autor separó ligeramente las vías de la ranura.
- **Cierre:** rellenar zonas y repetir DRC con la regla de 0,20 mm.
- [ ] PCB corregida y revisada por el autor; esquemático sin cambio propuesto.

### RV-17 — «Plugged» no resuelve las vías dentro del pad del ESP32

- **Estado: GEOMETRÍA Y PASTA ACEPTADAS; soldadura pendiente de validar.**
  Se conservan las nueve ventanas de pasta. El autor ha dejado 12 vías
  exteriores de 0,30/0,50 mm y cuatro interiores de 0,20/0,45 mm, sin solapar
  los agujeros con la pasta. Margen interior de 0,020–0,034 mm aceptado.
- **Limitación:** JLC excluye del relleno con tinta las vías dentro de pads
  o próximas a sus aperturas. «Plugged» no garantiza su sellado.
- **Decisión:** no modificar pasta ni añadir relleno epoxi/cobre de oficio.
  La existencia de estas vías no demuestra por sí sola un fallo de soldadura.
- **Cierre:** comprobar calidad de montaje del prototipo.
  [Requisitos de vías JLCPCB](https://jlcpcb.com/help/article/pcb-via-covering).
- [ ] PCB/proceso revisados por el autor; esquemático sin cambio propuesto.

**Resto del contraste:** tamaños de vía y resultado DRC actuales en el resumen
de cierre anterior. Los avisos de texto de C2 y el solapamiento Q4–D4 ya no
aparecen tras los ajustes del autor.

La ranura fresada estrecha y el tratamiento HASL/máscara deben confirmarse en
CAM con sus tolerancias; los Gerber anteriores no sirven para esta versión.
La expansión global de máscara de 0,051 mm se conserva para no alterar las
aperturas en esta fase; JLC publica actualmente 1:1 y puede compensar en CAM.
El DRC con 0,09 mm entre apertura y cobre ajeno no añade errores.
No se ha pedido control de impedancia de precisión ni cambiado el laminado
económico TG135 a otro de más prestaciones. Fuente de límites:
[capacidades de JLCPCB](https://jlcpcb.com/capabilities/pcb-capabilities).
