# SAcre / SmartFeedbackDevice

Diseño electrónico de un dispositivo portátil basado en ESP32.

- Autor: Cosmin Dobrescu
- Herramienta: KiCad 10
- Proyecto principal: `sacre.kicad_pro`
- Estado: revisión posterior a la primera fabricación

## Mecanismo de trabajo

1. Leer este archivo completo antes de continuar una revisión.
2. Usar este README como única fuente de verdad. Las hipótesis nuevas del
   asistente se mantienen únicamente en `REVISION_PROVISIONAL.md` hasta que el
   autor las acepte o descarte; no crear otros informes auxiliares.
3. Mantener un identificador estable para cada incidencia.
4. Tratar toda incidencia inicialmente como una hipótesis, no como un fallo
   confirmado.
5. Intentar refutarla contrastando esquemático, PCB, símbolo, huella, datasheet
   y, cuando exista, evidencia del prototipo.
6. Registrar únicamente:
   - `Hipótesis`: afirmación que se comprueba.
   - `Evidencia`: datos concretos que sostienen el dictamen.
   - `Acción`: cambio exacto, solo si procede.
   - `Cierre`: comprobación necesaria para darlo por válido.
7. Usar estos estados:
   `PENDIENTE → CONFIRMADO / DESCARTADO / CONDICIONAL → CORREGIDO → VALIDADO`.
8. Durante la verificación no modificar el diseño.
9. Autorización del autor (2026-09-13): el asistente puede sincronizar la PCB,
   retirar cobre obsoleto y colocar provisionalmente componentes por zonas.
   Conservar contorno, taladros, conectores y keepout RF. Guardar comparativas
   por capas y commits por etapa. El enrutado y ajuste final quedan al autor.
10. Registrar por separado en cada incidencia:
    - `[ ] Esquemático corregido`
    - `[ ] PCB corregida y revisada por el autor`
11. Corregir únicamente incidencias `CONFIRMADO`; no corregir las
    `CONDICIONAL` sin resolver antes su condición.
12. Marcar `VALIDADO` solo después de aplicar el criterio de cierre y las
    comprobaciones ERC/DRC pertinentes.
13. Mantener separadas las mejoras preventivas de los fallos demostrados.
14. Evitar duplicar información y conservar las entradas breves, verificables
    y comprensibles sin contexto externo.

## Fuentes para la verificación

Los datasheets recopilados están en `documentation/datasheets/`; se conservan
también los documentos útiles que ya estaban en `documentation/` y `RV-8803/`.
`bom_jlcpb.csv` identifica las piezas de la fabricación ensayada y prevalece
sobre referencias genéricas, salvo las sustituciones cerradas en esta revisión.

| Bloque | Referencias cubiertas | Estado |
| --- | --- | --- |
| MCU y RF | ESP32-PICO-D4, guía de hardware ESP32, AN012400C00R3200 | Completo |
| Pantalla y táctil | GDEY042T81-T02, SSD1683, FT6336U, AFC24-S06FIC-00, AFC24-S24FIA-00 | Completo |
| USB, sensores y mando | CP2104, conector USB-C, LIS3DH, RV-8803-C7, pulsador | Completo |
| Alimentación y protección | TLV62568, TP4054, FHD4020S-470MT, FTC252012S1R0MBCA, BAT54C, MBR0530, SP0503BAHTG | Completo |
| Transistores y motor | CJ2312, CJ3415, SI1308EDL, UMH3N, S8050 | Completo para los componentes montados |

Referencias críticas resueltas por el BOM:

| Ref. | Pieza montada | LCSC |
| --- | --- | --- |
| Q2 | R+O S8050 | C20069125 |
| U5 | JSMSEMI TP4054 | C5381776 |
| L1 | cjiang FHD4020S-470MT | C843300 |
| L3 | cjiang FTC252012S1R0MBCA | C5832370 |
| Q4 | TECH PUBLIC SI1308EDL | C7603347 |
| D1–D4 | JSCJ MBR0530 | C77336 |
| D5 | R+O BAT54C | C22466350 |

Los documentos de MCP73831 y LQH44PN quedan solo como comparación: no
describen los componentes montados en esta fabricación.

Auditoría LCSC del esquema: todos los componentes con referencia de compra
cerrada tienen el campo `LCSC Part #` normalizado. Se completaron R10=`C22939`, R23=`C23352`,
R24=`C25819` y la antena exacta AE1=`C5943777`; esta última tiene poca
disponibilidad local y debe comprobarse antes de fabricar. J5, J6 y TP1 son
elementos de PCB y quedan fuera del BOM. Solo M1 permanece sin referencia de
compra hasta validar dimensiones y huella del motor.

## Fallos observados en el prototipo

- [ ] `TOUCH-01` SDA y SCL intercambiadas en el FPC táctil.
  - [x] Esquemático corregido: J1.1=`SCL` y J1.2=`SDA`.
  - [ ] PCB corregida y revisada por el autor.
  - `Estado`: CORREGIDO EN ESQUEMA; PCB PENDIENTE.
  - `Nota`: sobre el enrutado experimental de `d874702` se ha eliminado el
    bucle redundante de `SCL` y dos microcodos. Queda pendiente la revisión
    física del autor y no se considera validado.
  - `Cierre`: comprobar detección del FT6336U y niveles/flancos de ambas
    señales con el flex conectado.
- [ ] `TOUCH-02` Revisar holgura, orientación y salida del flex táctil.
- [ ] `MOTOR-01` Incompatibilidad entre el pinout de Q2 y su huella.
  - [x] Esquemático corregido: Q2 es el S8050 SOT-23 montado, B1/E2/C3
    (`C20069125`).
  - [ ] PCB corregida y revisada por el autor.
  - `Estado`: CORREGIDO EN ESQUEMA; PCB Y VALIDACIÓN DEL MOTOR PENDIENTES.
- [x] `TOUCH-03` Pull-up de la interrupción táctil descartado.
  - `Estado`: DESCARTADO; `INT_TOUCH` puede permanecer en GPIO35.
  - `Evidencia`: el FT6336U declara `INT` como E/S CMOS y no prescribe un
    pull-up externo. Verificar por medida sólo si aparecen flancos inestables.
- [ ] `PWR-02` El USB provoca arranque y ciclos rápidos de encendido/apagado.
  - `Estado`: CORREGIDO EN ESQUEMA; PCB y validación pendientes.
  - `Hipótesis`: en el diseño fabricado U3 queda alimentado directamente por
    `USB_VBUS` mientras U2 está apagado. `TXD` llega sin aislamiento a `U0RXD`
    y `DTR/RTS` cruzan
    también hacia `EN/GPIO0` mediante Q7; cualquiera de esas vías puede
    realimentar parcialmente el ESP32 y `+3V3`. Esa tensión puede activar
    IO25/Q6 y el latch. Al caer de nuevo la alimentación, el USB repite el
    proceso.
  - `Evidencia`: en el diseño fabricado U3 está configurado como bus-powered;
    `TXD` y `U0RXD` están unidos directamente. Q7 contiene resistencias de base
    internas de 4,7 kΩ,
    pero no aísla un dominio alimentado de otro apagado. IO25 controla la
    puerta de Q6 mediante R18, Q6 tiene un umbral de 0,45–1 V y R14 sólo lo
    descarga con 100 kΩ. R13/C18 forman una constante de 30 ms. L2 puede
    agravar la caída, pero no explica el disparo inicial desde USB.
  - `Decisión`: U3 queda self-powered físicamente. `VDD`, `VIO` y `REGIN` van
    a `+3V3`; R23/R24=24 kΩ/47 kΩ detectan `USB_VBUS` sin realimentación;
    C23=1 µF y C20=0,1 µF desacoplan U3. C17=1 µF mejora `EN`.
    R13/C18 permanecen en 300 kΩ/100 nF; no se añade supervisor ni histéresis.
    El firmware debe activar IO25 pronto y puede exigir mantener el pulsador
    durante la programación.
  - `Cierre`: al conectar USB con el latch apagado, observar `VSYS`, `+3V3`,
    puerta de Q6 y puerta de Q5. Si `+3V3` sube antes que `VSYS`, queda
    confirmada la realimentación desde U3; repetir aislando `TXD` o forzando
    la puerta de Q6 a GND.
  - `Fuentes`: [CP2104, figura 9](documentation/cp2104-1397921.pdf),
    [ESP32-PICO-D4](documentation/datasheets/ESP32-PICO-series.pdf),
    [CJ2312](documentation/datasheets/CJ2312.pdf) y
    [circuito de referencia](https://circuitcellar.com/resources/quickbits/soft-latching-power-circuits/).
  - [x] Esquemático corregido.
  - [ ] PCB corregida y revisada por el autor.
- [ ] `MCU-01` Desacoplo externo local insuficiente en U2.
  - `Estado`: CORREGIDO EN ESQUEMA; PCB y validación pendientes.
  - `Hipótesis`: U2 depende de condensadores de `+3V3` alejados y carece del
    par externo recomendado junto al encapsulado.
  - `Evidencia`: el ESP32-PICO-D4 une a `VDD33` los pines 1 `VDDA`, 3 y 4
    `VDDA3P3`, 19 `VDD3P3_RTC` y 37 `VDD3P3_CPU`; su referencia añade
    10 µF + 0,1 µF externos. En la PCB, C17 es de `EN`; C22 está junto al
    regulador y C3/C4 desacoplan el LIS3DH. El SiP incorpora desacoplo interno,
    pero no elimina ese par externo.
  - `Acción`: añadidos C26=10 µF y C27=0,1 µF entre `+3V3` y GND. En PCB deben
    quedar pegados al grupo de pines 1/3/4 de U2, con pista corta y retorno/vía
    de masa inmediatos. Los
    pines 19 y 37 deben quedar unidos por un plano o recorrido de baja
    impedancia; no se prescriben condensadores adicionales junto a ellos
    porque el PICO-D4 ya integra 0,1 µF en ambos dominios.
  - `Cierre`: comprobar colocación y retornos en PCB y medir `+3V3` junto a U2
    con Bluetooth y táctil simultáneos, sin caída por debajo de 3,0 V ni
    brownout.
  - `Fuente`: [ESP32-PICO-D4, figuras 7 y 10](documentation/datasheets/ESP32-PICO-series.pdf).
  - [x] Esquemático corregido.
  - [ ] PCB corregida y revisada por el autor.

## Correcciones pendientes agrupadas

Esquemático aplicado a la PCB el 2026-09-13. La colocación es provisional;
el autor ajustará posiciones y completará el enrutado y la validación física.

### 1. GPIO, reset táctil y RTC

- [x] `RES_TOUCH_CTRL` en GPIO4; GPIO16 queda libre. JP1 eliminado, R11=1 kΩ
  en serie, C25=10 µF a GND y D8 descarga el nodo al caer `+3V3`. Firmware:
  GPIO4 en drenador abierto, nivel bajo ≥60 ms, liberar sin conducir nivel alto
  y esperar ≥300 ms antes de usar I²C.
- [x] `ACC_INT2` movida de GPIO12 a GPIO34; se conservan ambas interrupciones.
- [x] `RTC_INT` permanece en GPIO27 con R22=10 kΩ a `+3V3`.
- [x] `RTC_EVI` desconectada de GPIO15, R21=100 kΩ a `VBAT_RAW` y TP1.
- [x] Etiqueta huérfana `GPO` eliminada; GPIO32 queda libre.
- [x] Y1 alimentado directamente por `VBAT_RAW`, sin regulador añadido;
  C2=0,1 µF como desacoplo local.

- [x] Esquemático corregido.
- [ ] PCB corregida y revisada por el autor.

El autor realiza el ajuste final de colocación, enrutado y revisión de la PCB.

### 2. USB, latch, C18 y EN

- [x] U3 alimentado desde `+3V3`; `USB_VBUS` detectado mediante
  R23/R24=24 kΩ/47 kΩ.
- [x] C23=1 µF y C20=0,1 µF locales; C24=4,7 µF/25 V en `VPP`.
- [x] C17=1 µF con R12=10 kΩ en `EN`.
- [x] R13/C18 conservados en 300 kΩ/100 nF; supervisor e histéresis descartados.
- [ ] Firmware: activar IO25 pronto; mantener pulsado durante programación si
  el reset hace caer el latch. Requisito anotado junto al circuito del latch.
- [x] Esquemático corregido.
- [ ] PCB corregida y revisada por el autor.

### 3. Regulador, ESP32, pantalla y motor

- [x] Pull-ups I²C R7/R8 cambiados de 10 kΩ a 5,1 kΩ para dar margen a
  400 kHz reutilizando el valor existente en el BOM.
- [x] ADP2108 sustituido por U6=`TLV62568DBVR` (`C163219`): buck de 1 A,
  salida nominal de 3,318 V con R26/R27=453 kΩ/100 kΩ. C31/C32=10 µF.
  Se elige por margen de corriente, disponibilidad y coste; el menor consumo
  en reposo y la salida fija del ADP2108 no justifican mantenerlo aquí.
- [x] `L3` fijada como
  [`FTC252012S1R0MBCA`](https://jlcpcb.com/partdetail/FTC252012S1R0MBCA/C5832370),
  LCSC `C5832370`: 1 µH, 4 A, Isat 5,6 A y DCR 35 mΩ. Huella de esquema
  `L_Changjiang_FTC252012S`; `C1042` queda solo como pieza histórica no válida.
  El campo `LCSC Part #` permite su selección automática en el BOM de JLCPCB.
- [x] C26=10 µF y C27=0,1 µF añadidos junto a U2 en el esquemático.
- [ ] Colocar C27 junto a U2.1/U2.3/U2.4 y C26 contiguo a C27, con
  retorno corto a masa.
- [x] Booster SSD1683 corregido en esquema: L1=47 µH, R10=2,2 Ω;
  C6 permanece en 4,7 µF/25 V; C8/C16 eliminados.
- [x] `L1` fijada como
  [`FHD4020S-470MT`](https://jlcpcb.com/partdetail/FHD4020S-470MT/C843300),
  LCSC `C843300`: 47 µH, 660 mA, Isat 1,3 A y DCR 950 mΩ. Huella de esquema
  `L_Changjiang_FNR4020S`. El campo `LCSC Part #` permite su selección
  automática en el BOM de JLCPCB.
- [ ] Actualizar y revisar el booster en PCB cuando se reabra la fase física.
- [x] Q2 corregido en esquema al pinout real B1/E2/C3 del S8050
  `C20069125`; se conservan R2=1 kΩ y D1.
- [x] Motor pasado de `+3V3` a `VSYS` mediante D9=MBR0530; `VMOTOR` alimenta
  M1, D1 y C1=0,1 µF. R25=100 kΩ mantiene la base de Q2 descargada. PWM
  opcional; encendido/apagado directo válido.
- [x] Etapa dimensionada provisionalmente para el motor candidato: 3 V,
  rango 2,5–4 V y 70 mA nominales, suponiendo 90 mA de arranque. Confirmar
  este último dato en el motor comprado; no equivale a validación física.
- [ ] Fijar referencia de compra y comprobar dimensiones/huella del motor.

### 4. Cargador y batería

- [x] C29=1 µF entre U5.4 `VDD` y GND y C28=10 µF entre U5.3 `VBAT` y
  GND añadidos en el esquemático; colocación local en PCB pendiente.
- [x] R19=5,1 kΩ: unos 196 mA para la batería prevista de 600–700 mAh;
  reutiliza el mismo valor que R16/R17.
- [x] Protección integrada en la batería confirmada por el autor.
- [ ] Firmware: habilitar la medida con GPIO26, leer `ADC_BATT`, deshabilitarla
  y apagar antes de ciclos repetidos de brownout. Requisito anotado en esquema;
  umbral pendiente de medida.
- [x] Power-path descartado. U5 carga directamente `VBAT_RAW`; carga normal con
  el equipo apagado y encendido temporal permitido para programación.
- [x] Mantener TP4054 `C5381776`: LCSC/JLCPCB lo especifican a 4,2 V. La tabla
  ambigua del datasheet se conserva como cautela de validación, no como fallo
  que obligue a cambiar U5.

### 5. PCB y mecánica

Estado: **PREPARADA PARA ENRUTADO MANUAL** (2026-09-13). Colocación provisional,
sin pistas nuevas; todavía no es una PCB lista para fabricar.

- Referencia anterior a la preparación: `ae7a2bc`; DRC inicial: 0 infracciones
  y 0 conexiones pendientes (todavía con el circuito antiguo en PCB).
- [Vista original por capas](documentation/pcb_preparacion/antes.png): capa
  activa en color y las demás en gris, sin rellenos para ver las pistas.
- [Vista final por capas](documentation/pcb_preparacion/despues.png), con el
  mismo encuadre, y [colocación con referencias](documentation/pcb_preparacion/colocacion.png).
- Inventario: 22 componentes añadidos, 7 retirados y cambio de huella de L1.
  Se retiraron 181 segmentos/vías; los 805 conservados mantienen geometría y red.
  RF, USB D+/D− y SDA/SCL se conservan íntegros. Contorno, taladros, conectores,
  ESP32, antena y contornos de zonas/keepout no se han movido.
- Comprobación final: 0 infracciones DRC, 0 discrepancias con el esquema y
  50 conexiones pendientes de enrutar. Zonas rellenadas; componentes nuevos
  dentro del contorno y sin solapamiento de courtyards.
- Checkpoints: `6c13108` referencia visual; `2be720c` sincronización;
  `a26d5b2` limpieza de cobre. La colocación se guarda en el commit siguiente.

| Zona | Cambios aplicados en PCB |
| --- | --- |
| Regulador | U6 en el centro del antiguo U4: (70,4; 132,4) mm. L3, C31/C32 y R26/R27 alrededor. Retirados U4, L2, C21/C22 y sus ramas obsoletas. |
| USB | C23/C24 y R23/R24 junto a U3. Retiradas las conexiones antiguas de alimentación; USB D+/D− y auto-reset conservados. |
| Táctil y RTC | C25/D8 junto a J1; R21/R22/TP1 junto a Y1. JP1 retirado; reset, EVI y ACC_INT2 pendientes de enrutar con los GPIO actuales. |
| ESP32 | C26/C27 a la derecha de U2; C30 a la izquierda, junto al lado de VDD_SDIO. Ajustar distancias y retornos al enrutar. |
| Carga y motor | C28/C29 junto a U5; D9/R25 en la zona de M1/Q2. Cobre antiguo del pinout de Q2 y alimentación del motor retirado. |
| Pantalla | Huella de L1 sustituida en la misma posición. C8/C16 y sus pistas retirados. R9/R10 actualizados; resto del booster conservado. |

- [x] Sincronizar componentes, valores, huellas y redes desde el esquemático.
  Coincidencia verificada pad a pad; 92 componentes.
- [x] Retirar cobre obsoleto y colocar provisionalmente las piezas nuevas por zonas.
- [ ] Recolocar J1 y despejar la salida del flex.
- [ ] Colocar desacoplos y diseñar los bucles de regulador, booster y motor.
- [ ] Adaptar las pistas a los nuevos GPIO y al pinout real de Q2.
- [ ] Revisar DRC, planos, retornos, 3D y archivos de fabricación.

## Revisión sistemática pendiente

### Configuración de fabricación — 2026-09-13

Enrutado completado por el autor; revisión de fabricación en curso. Los datos
de «preparación para enrutado» anteriores son el checkpoint histórico, no el
estado actual. No se han modificado pistas, vías, colocación ni contorno al
aplicar esta configuración.

- Pedido objetivo: FR-4 estándar TG135, **4 capas, 1 mm nominal**, cobre
  exterior **1 oz** e interior **0,5 oz**, verde, serigrafía blanca, HASL con
  plomo y fresado normal. Sin prestaciones especiales añadidas.
- Apilado: **JLC04101H-7628**. La consulta pública de JLCPCB para esos
  parámetros devuelve esta estructura tanto en «No requirement» como al
  seleccionarla explícitamente, con `fixedFee=0`, `coefficient=0` y fabricación
  rápida habilitada. Confirmar precio final en el pedido; no se ha comprado nada.
- KiCad conserva estos parámetros también en las variables de texto del
  proyecto. Elegir el apilado explícito en el pedido para evitar ambigüedad.

| Capa | Espesor configurado | Material / εr de referencia |
| --- | --- | --- |
| F.Mask / B.Mask | 0,0152 mm cada una | Verde / 3,8 |
| F.Cu / B.Cu | 0,035 mm cada una | Cobre exterior 1 oz |
| Dieléctrico 1 / 3 | 0,2104 mm cada uno | FR4 7628 / 4,4 |
| In1.Cu / In2.Cu | 0,0152 mm cada una | Cobre interior 0,5 oz, tras procesado |
| Dieléctrico 2 | 0,5000 mm | Núcleo FR4 / 4,6 nominal |

La estructura publicada suma 1,0212 mm sin máscara; con la máscara modelada,
KiCad muestra **1,0516 mm**. Se pide **1,0 mm nominal**, tolerancia estándar
±10 %. No ajustar artificialmente los dieléctricos para que la suma dé 1,000.

**Límite del modelo RF:** εr son referencias publicadas, no caracterización
del lote TG135. La calculadora actual de JLC usa NP-155F para 4–8 capas y
40,64 µm de cobre exterior, frente a los 35 µm de su tabla de apilado. El
cálculo de la pista RF indicado abajo utiliza el modelo de fabricación de
JLC; no se ha cambiado a TG155 ni simulado la antena completa. `tanδ=0,02`
se conserva como valor orientativo, no como dato confirmado del laminado.

DRC configurado: pista/separación absolutas ≥0,10 mm, cobre–borde ≥0,20 mm,
taladro de vía ≥0,20 mm, diámetro ≥0,40 mm y anular ≥0,05 mm. La regla de
`sacre.kicad_dru` exige diámetro ≥0,45 mm si el taladro es <0,30 mm para
evitar recargo por vías pequeñas. Se mantienen las separaciones de red de
0,15 mm y vías preferidas de 0,50/0,30 mm.
Apertura de máscara–cobre ajeno ≥0,09 mm; texto ≥1 mm y trazo ≥0,15 mm.
No se han cambiado las aperturas de máscara ni las exclusiones existentes.
Los límites específicos de ranuras, PTH y procesos no quedan todos cubiertos
por estas reglas globales: ver las condiciones de fabricación en
`REVISION_PROVISIONAL.md`.

**Resultado:** 0 conexiones pendientes, 0 diferencias con el esquema;
0 infracciones no excluidas y 18 exclusiones previas tras corregir J3 y
adaptar el límite de las vías pequeñas. Los tres errores de
cobre–ranura y los dos avisos de serigrafía fueron corregidos por el autor.
Límites reaplicados con KiCad cerrado y DRC repetido tras detectar que un
guardado posterior había restaurado las restricciones antiguas del proyecto.
Tapado de vías pendiente: se conserva el tenting de KiCad; «Plugged» del
pedido anterior no cubre las vías dentro del pad U2.49. No se activa relleno
epoxi/cobre ni se añade coste sin decidirlo con el autor.

Fuentes: [apilados JLCPCB](https://jlcpcb.com/impedance),
[capacidades](https://jlcpcb.com/capabilities/pcb-capabilities),
[modelo de cálculo](https://jlcpcb.com/help/article/user-guide-to-the-jlcpcb-impedance-calculator)
y [tratamiento de vías](https://jlcpcb.com/help/article/pcb-via-covering).

### Pista RF — ajuste autorizado el 2026-09-13 (`RV-11`)

- `/ANT_RF`: ancho **0,26 mm** en sus tres segmentos y dos arcos; recorrido,
  huella de antena, meandro y máscara sin cambios. Zonas rellenadas; separación
  lateral a GND ≈0,1505 mm y plano In1 continuo bajo la pista externa.
- Modelo JLC `CoatedCoplanarWaveguideWithLowerGnd1B`: H=0,2104 mm, εr=4,4,
  cobre acabado=40,64 µm, ancho superior 12,7 µm menor que el inferior;
  máscara εr=3,8, 25,4 µm sobre sustrato y 15,24 µm sobre cobre. Son los
  parámetros actuales del servicio, no todos coinciden con su página de ayuda.
- Resultado nominal con hueco lateral de 0,15 mm: **50,15 Ω** para 0,26 mm,
  frente a 44,87 Ω para el ancho anterior de 0,3341 mm. La aproximación previa
  sin máscara de 50,8 Ω queda sustituida por este contraste más completo.
- DRC posterior: 0 conexiones pendientes, 0 diferencias con el esquema,
  0 infracciones no excluidas; 22 exclusiones intactas. No valida por sí solo
  la impedancia del lote TG135 ni la adaptación/alcance de la antena completa.
- [ ] PCB revisada visualmente por el autor; validación RF física pendiente.
- Fuente: [calculadora JLCPCB](https://jlcpcb.com/pcb-impedance-calculator).

### Checklist de cierre

- Revisión final de PCB (2026-09-13): 0 conexiones pendientes, 0 diferencias
  con el esquema y 0 infracciones no excluidas tras ajustar las reglas de vía;
  se mantienen 18 exclusiones previas. Q4–R10 ya corregido; pasta de U2 conservada por
  decisión del autor. Gerber/taladros revisados en `tmp/final-review-fab/`,
  no publicados como fabricación definitiva. Resultado y límites en
  `REVISION_PROVISIONAL.md`, «Cierre de revisión sobre el guardado actual».
- `RV-15`: agujeros de posicionamiento de J3 corregidos a NPTH de 0,70 mm
  en PCB y biblioteca; posiciones y anclajes soldados intactos. DRC sin
  infracciones nuevas por este cambio; revisión visual del autor pendiente.

- [ ] `PWR-01` Alimentación, carga, encendido y apagado.
- [ ] `USB-01` USB-C, CP2104 y programación automática.
- [ ] `MCU-02` Pines de arranque, reset y señales del ESP32.
- [ ] `DISP-01` Pantalla y generación de tensiones auxiliares.
- [ ] `SENS-01` RTC, acelerómetro y medida de batería.
- [ ] `RF-01` Antena, adaptación, keepout y retorno de RF.
- [ ] `PCB-01` Colocación, planos, retornos, pistas y vías.
- [ ] `MECH-01` Contorno, taladros, conectores, alturas y accesibilidad.
- [ ] `FAB-01` ERC, DRC, huellas, BOM y archivos de fabricación.

## Uso

Abrir `sacre.kicad_pro`. Las librerías, huellas y modelos 3D del diseño están
incluidos en el repositorio; el estado de los datasheets figura arriba.

Cualquier exportación local en `production/` debe considerarse desactualizada
hasta completar `FAB-01`.
