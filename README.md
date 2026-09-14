# SAcre / SmartFeedbackDevice

Diseño electrónico de un dispositivo portátil basado en ESP32.

- Autor: Cosmin Dobrescu
- Herramienta: KiCad 10
- Proyecto principal: `sacre.kicad_pro`
- Estado: esquema y PCB corregidos; revisión digital completada (2026-09-13).
  Pendientes: aprobación física final, preparación del pedido y pruebas del prototipo.

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
9. La preparación de PCB autorizada el 2026-09-13 ya se completó. El enrutado
   y ajuste físico quedan al autor; cualquier nueva modificación de PCB por
   el asistente requiere un alcance autorizado. Comparar antes/después y no
   alterar cobre, contorno, conectores o keepout fuera de ese alcance.
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
15. No volver a listar como trabajo pendiente una corrección ya aplicada.
    Distinguir revisión digital, aprobación del autor y prueba física; los
    datos de versiones antiguas deben identificarse como históricos.

## Fuentes para la verificación

Los datasheets recopilados están en `documentation/datasheets/`; se conservan
también los documentos útiles que ya estaban en `documentation/` y `RV-8803/`.
`bom_jlcpb.csv` identifica las piezas del prototipo anterior: sirve para
investigar sus fallos, no para pedir la nueva placa. Para fabricar, generar
el BOM desde el diseño actual usando sus campos `LCSC Part #`; ni los BOM
históricos ni las tablas resumidas de este documento sustituyen esa exportación.

| Bloque | Referencias cubiertas | Estado |
| --- | --- | --- |
| MCU y RF | ESP32-PICO-D4, guía de hardware ESP32, AN012400C00R3200 | Completo |
| Pantalla y táctil | GDEY042T81-T02, SSD1683, FT6336U, AFC24-S06FIC-00, AFC24-S24FIA-00 | Completo |
| USB, sensores y mando | CP2104, conector USB-C, LIS3DH, RV-8803-C7, pulsador | Completo |
| Alimentación y protección | TLV62568, TP4054, FHD4020S-470MT, FTC252012S1R0MBCA, BAT54C, MBR0530, SP0503BAHTG | Completo |
| Transistores y motor | CJ2312, CJ3415, SI1308EDL, UMH3N, S8050 | Completo para los componentes montados |

Selección actual de referencias críticas (incluye las sustituciones del prototipo):

| Ref. | Pieza seleccionada | LCSC |
| --- | --- | --- |
| Q2 | R+O S8050 | C20069125 |
| U5 | JSMSEMI TP4054 | C5381776 |
| L1 | cjiang FHD4020S-470MT | C843300 |
| L3 | cjiang FTC252012S1R0MBCA | C5832370 |
| Q4 | TECH PUBLIC SI1308EDL | C7603347 |
| D1, D8, D9 | JSCJ MBR0530 | C77336 |
| D2–D4 | onsemi MBR0530T3G | C236079 |
| D5 | R+O BAT54C | C22466350 |

Los documentos de MCP73831 y LQH44PN quedan solo como comparación: no
describen los componentes seleccionados para la nueva fabricación.

Auditoría de campos de compra: **85 componentes, 39 códigos distintos** con
`LCSC Part #` idéntico en esquema y PCB, incluida R28=`C21190` como R11.
No se añaden alias duplicados: Fabrication Toolkit, instalado en este equipo,
lee ese campo y lo exporta junto a `Designator`, `Footprint`, `Quantity` y
`Value`. Sincronizar esquema y PCB antes de exportar; comprobar códigos y
orientaciones en la previsualización de JLCPCB, no sólo la detección automática.
Chequeo del exportador y propuestas no aplicadas de reducción de variedad:
`REVISION_PROVISIONAL.md`, sección «BOM y posiciones de montaje» (2026-09-14).
Fuentes: [atributos del exportador](https://github.com/bennymeg/Fabrication-Toolkit#attributes)
y [guía KiCad de JLCPCB](https://jlcpcb.com/help/article/how-to-generate-the-bom-and-centroid-file-from-kicad).

H1–H4, J5, J6 y TP1 son elementos de PCB, sin referencia de compra y excluidos
del BOM. M1 se compra por AliExpress: excluido del BOM y del archivo de
posiciones de montaje en esquema y PCB; su huella y conexiones se conservan.
Quedan por confirmar modelo, corriente de arranque y encaje del motor.

Preparación del pedido (2026-09-14): JLCPCB emparejó correctamente las 85
referencias con los 39 códigos previstos para **4 placas ensambladas**. U2,
ESP32-PICO-D4 `C193707`, está clasificado como `Standard Only`, por lo que el
pedido debe usar **Standard PCBA, cara superior**. La propuesta de sustituir
R23/R24 por valores ya presentes reduciría dos alimentadores, pero fue
descartada por el autor: se conservan 24 kΩ/47 kΩ y 39 códigos. En la
previsualización, el autor corrigió manualmente sólo la orientación de Y1;
comprobarla otra vez en cualquier pedido futuro.

Stock consultado en la biblioteca de montaje JLCPCB el 2026-09-13:
39 referencias con existencias. AE1 tenía 8 unidades, pero el autor dispone
de acopio propio; Y1 tenía 2 y se acepta omitir el RTC si falta. SW1 tenía
970 unidades, cantidad aceptada por el autor. Reconfirmar disponibilidad al
pedir: la consulta no reserva piezas ni el stock de LCSC garantiza montaje
inmediato en JLCPCB. Preparar el pedido teniendo en cuenta el acopio y las
omisiones acordadas; no se ha cambiado ningún campo DNP del diseño.

## Fallos observados en el prototipo

Las correcciones indicadas ya están aplicadas al esquema y a la PCB.
Las casillas de aprobación del autor y las pruebas físicas siguen abiertas;
no significan que falte repetir el enrutado. La evidencia del fallo describe
la primera placa fabricada, no la versión actual.

- [ ] `TOUCH-01` SDA y SCL intercambiadas en el FPC táctil.
  - [x] Esquemático corregido: J1.1=`SCL` y J1.2=`SDA`.
  - [ ] PCB corregida y revisada por el autor.
  - `Estado`: CORREGIDO EN ESQUEMA Y PCB; prueba del táctil pendiente.
  - `Cierre`: comprobar detección del FT6336U y niveles/flancos de ambas
    señales con el flex conectado.
- [ ] `TOUCH-02` Revisar holgura, orientación y salida del flex táctil.
- [ ] `MOTOR-01` Incompatibilidad entre el pinout de Q2 y su huella.
  - [x] Esquemático corregido: Q2 es el S8050 SOT-23 montado, B1/E2/C3
    (`C20069125`).
  - [ ] PCB corregida y revisada por el autor.
  - `Estado`: CORREGIDO EN ESQUEMA Y PCB; validación del motor pendiente.
- [x] `TOUCH-03` Pull-up de la interrupción táctil descartado.
  - `Estado`: DESCARTADO; `INT_TOUCH` puede permanecer en GPIO35.
  - `Evidencia`: el FT6336U declara `INT` como E/S CMOS y no prescribe un
    pull-up externo. Verificar por medida sólo si aparecen flancos inestables.
- [ ] `PWR-02` El USB provoca arranque y ciclos rápidos de encendido/apagado.
  - `Estado`: CORREGIDO EN ESQUEMA Y PCB; validación del encendido pendiente.
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
  - `Estado`: CORREGIDO EN ESQUEMA Y PCB; validación con radio/táctil pendiente.
  - `Hipótesis`: U2 depende de condensadores de `+3V3` alejados y carece del
    par externo recomendado junto al encapsulado.
  - `Evidencia`: el ESP32-PICO-D4 une a `VDD33` los pines 1 `VDDA`, 3 y 4
    `VDDA3P3`, 19 `VDD3P3_RTC` y 37 `VDD3P3_CPU`; su referencia añade
    10 µF + 0,1 µF externos. En la PCB, C17 es de `EN`; C22 está junto al
    regulador y C3/C4 desacoplan el LIS3DH. El SiP incorpora desacoplo interno,
    pero no elimina ese par externo.
  - `Acción`: añadidos C26=10 µF y C27=0,1 µF entre `+3V3` y GND, colocados
    junto al grupo 1/3/4 de U2. Conexión corta y ancha a U2.3/4, con vía GND
    a unos 1 mm del centro de C27.2. Los
    pines 19 y 37 deben quedar unidos por un plano o recorrido de baja
    impedancia; no se prescriben condensadores adicionales junto a ellos
    porque el PICO-D4 ya integra 0,1 µF en ambos dominios.
  - `Cierre`: comprobar colocación y retornos en PCB y medir `+3V3` junto a U2
    con Bluetooth y táctil simultáneos, sin caída por debajo de 3,0 V ni
    brownout.
  - `Fuente`: [ESP32-PICO-D4, figuras 7 y 10](documentation/datasheets/ESP32-PICO-series.pdf).
  - [x] Esquemático corregido.
  - [ ] PCB corregida y revisada por el autor.

## Correcciones aplicadas por bloques

Esquemático sincronizado y PCB enrutada por el autor. Las tareas abiertas
de estos bloques son de firmware o validación física, salvo indicación expresa.

### 1. GPIO, reset táctil y RTC

- [x] `RES_TOUCH_CTRL` en GPIO4; GPIO16 queda sin conexión externa, reservado
  para la flash interna del PICO-D4. JP1 eliminado. R11 y R28, ambas de 1 kΩ
  (`C21190`), están en paralelo: 500 Ω efectivos entre GPIO4 y RSTN.
  Mejora preventiva del nivel bajo sin añadir otra referencia al BOM;
  omitiendo R28 se recupera 1 kΩ. Ambas están previstas para montar.
  C25=10 µF a GND y D8 descarga el nodo al caer `+3V3`. Firmware:
  GPIO4 en drenador abierto, nivel bajo ≥60 ms, liberar sin conducir nivel alto
  y esperar ≥300 ms antes de usar I²C.
- [ ] Validar en prototipo el reset: nivel bajo calculado ≈0,47 V a 3,3 V
  con R11/R28, por debajo de 0,3·IOVCC. Mejora aplicada en esquema y PCB;
  prueba física pendiente. La secuencia de firmware no cambia.
- [x] `ACC_INT2` movida de GPIO12 a GPIO34; se conservan ambas interrupciones.
- [x] `RTC_INT` permanece en GPIO27 con R22=10 kΩ a `+3V3`.
- [x] `RTC_EVI` desconectada de GPIO15, R21=100 kΩ a `VBAT_RAW` y TP1.
- [x] Etiqueta huérfana `GPO` eliminada; GPIO32 queda libre.
- [x] Y1 alimentado directamente por `VBAT_RAW`, sin regulador añadido;
  C2=0,1 µF como desacoplo local.

- [x] Esquemático corregido.
- [ ] PCB corregida y revisada por el autor.

La aprobación final de encaje mecánico corresponde al autor.

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
- [x] C26/C27 colocados y retornos revisados; C30 dispone de vía GND local.
- [x] Booster SSD1683 corregido en esquema: L1=47 µH, R10=2,2 Ω;
  C6 permanece en 4,7 µF/25 V; C8/C16 eliminados.
- [x] `L1` fijada como
  [`FHD4020S-470MT`](https://jlcpcb.com/partdetail/FHD4020S-470MT/C843300),
  LCSC `C843300`: 47 µH, 660 mA, Isat 1,3 A y DCR 950 mΩ. Huella de esquema
  `L_Changjiang_FNR4020S`. El campo `LCSC Part #` permite su selección
  automática en el BOM de JLCPCB.
- [x] Booster actualizado y revisado en PCB; Q4.2–R10.2 reducido a 2,34 mm.
  R9=1 MΩ entre GDR y GND, según la aplicación específica del panel.
- [x] Q2 corregido en esquema al pinout real B1/E2/C3 del S8050
  `C20069125`; se conservan R2=1 kΩ y D1.
- [x] Motor pasado de `+3V3` a `VSYS` mediante D9=MBR0530; `VMOTOR` alimenta
  M1, D1 y C1=0,1 µF. R25=100 kΩ mantiene la base de Q2 descargada. PWM
  opcional; encendido/apagado directo condicionado al motor indicado abajo.
- [x] Etapa dimensionada provisionalmente para el motor candidato: 3 V,
  rango 2,5–4 V y 70 mA nominales, suponiendo 90 mA de arranque. Confirmar
  este último dato en el motor comprado; no equivale a validación física.
- [ ] Confirmar modelo comprado, dimensiones/huella y corriente de arranque:
  no dar por corregido a 90 mA el dato ambiguo de 901 mA del anuncio.

### 4. Cargador y batería

- [x] C29=1 µF entre U5.4 `VDD` y GND y C28=10 µF entre U5.3 `VBAT` y
  GND añadidos y colocados junto a U5; retornos locales revisados en PCB.
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

Estado: **ENRUTADA Y REVISADA DIGITALMENTE** (2026-09-13), 93 componentes,
incluida R28 añadida y enrutada por el autor.

- [x] Sincronización, colocación y enrutado de las correcciones; revisión de
  desacoplos, retornos, GPIO y pinout de Q2. DRC sin conexiones pendientes.
- [x] Pista RF ajustada, J3 corregido a NPTH y vías separadas de la ranura.
- [x] Q4–R10 compactado; pasta bajo U2 conservada por decisión del autor.
  D6 no se mueve por la faja; U6 y SW1 se conservan según lo acordado.
- [ ] Comprobar físicamente flex de J1/pantalla, batería, motor y carcasa.
- [ ] Aprobar la PCB y preparar el pedido; ensayar el nuevo prototipo.

Histórico, no estado actual: `ae7a2bc` antes de preparar la PCB;
`92a95f1` colocación provisional; `0903610` enrutado y revisión final.
Las vistas [anterior](documentation/pcb_preparacion/antes.png),
[provisional](documentation/pcb_preparacion/despues.png) y de
[colocación](documentation/pcb_preparacion/colocacion.png) documentan sólo
la preparación inicial; no sirven como referencia de fabricación actual.

## Fabricación y cierre

### Configuración de fabricación — 2026-09-14

Enrutado completado por el autor y revisión digital final realizada.
La configuración de fabricación no cambia la geometría del circuito.

- Pedido objetivo: FR-4 estándar TG135, **4 capas, 1 mm nominal**, cobre
  exterior **1 oz** e interior **0,5 oz**, verde, serigrafía blanca, HASL con
  plomo y fresado normal. Sin prestaciones especiales añadidas.
- Montaje objetivo: **4 unidades, Standard PCBA, sólo cara superior**. U2
  `C193707` impide Economic PCBA. Confirmar el archivo de producción antes de
  fabricar; AE1 y Y1 se seleccionaron desde `My Parts` en el emparejamiento.
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
U2.49: el autor conserva las ventanas de pasta y las 16 vías recolocadas.
«Plugged» no garantiza el sellado de vías dentro del pad; se acepta esta
limitación sin añadir relleno epoxi/cobre. Calidad de soldadura pendiente
de validar; no es una recolocación o modificación de pasta pendiente.

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
- Este cálculo no valida la impedancia del lote TG135 ni la adaptación/alcance
  de la antena completa. Recalcular si cambian apilado, cobre, máscara, ancho,
  separación a masa o plano de referencia. Recordatorio junto a AE1 en
  `Cmts.User`, fuera del contorno; no forma parte de la serigrafía.
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

Las siguientes casillas son el cierre humano o experimental, no correcciones
eléctricas obligatorias abiertas:

- [ ] `PWR-01` Medir alimentación, carga y encendido/apagado sin bucles.
- [ ] `USB-01` Probar enumeración y programación, incluido el autosostén.
- [ ] `MCU-02` Probar arranque/reset y coexistencia de táctil y Bluetooth.
- [ ] `DISP-01` Probar refresco y tensiones auxiliares de la pantalla.
- [ ] `SENS-01` Probar sensores/RTC y calibrar el umbral de batería baja.
- [ ] `RF-01` Validar alcance de radio; el cálculo de pista ya está realizado.
- [ ] `PCB-01` Aprobación visual final del autor; revisión digital completada.
- [ ] `MECH-01` Confirmar encaje, alturas, flex y accesibilidad.
- [ ] `FAB-01` Aprobar exclusiones DRC y avisos ERC, comprobar BOM/posiciones
  y stock del pedido, regenerar/revisar el paquete definitivo y confirmar CAM.
  ERC: 122 desajustes de símbolos y aviso de AE1.2, conectado a GND;
  no se declara limpio ni se actualizan bibliotecas a ciegas.

## Uso

Abrir `sacre.kicad_pro`. Las librerías, huellas y modelos 3D del diseño están
incluidos en el repositorio; el estado de los datasheets figura arriba.

Cualquier exportación local en `production/` debe considerarse desactualizada
hasta completar `FAB-01`.
