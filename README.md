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
9. Corregir únicamente incidencias `CONFIRMADO`; no corregir las
   `CONDICIONAL` sin resolver antes su condición.
10. Marcar `VALIDADO` solo después de aplicar el criterio de cierre y las
    comprobaciones ERC/DRC pertinentes.
11. Mantener separadas las mejoras preventivas de los fallos demostrados.
12. Evitar duplicar información y conservar las entradas breves, verificables
    y comprensibles sin contexto externo.

## Fuentes para la verificación

Los datasheets recopilados están en `documentation/datasheets/`; se conservan
también los documentos útiles que ya estaban en `documentation/` y `RV-8803/`.
`bom_jlcpb.csv` identifica las piezas de la fabricación ensayada y prevalece
sobre referencias genéricas o previstas del esquemático.

| Bloque | Referencias cubiertas | Estado |
| --- | --- | --- |
| MCU y RF | ESP32-PICO-D4, guía de hardware ESP32, AN012400C00R3200 | Completo |
| Pantalla y táctil | GDEY042T81-T02, SSD1683, FT6336U, AFC24-S06FIC-00, AFC24-S24FIA-00 | Completo |
| USB, sensores y mando | CP2104, conector USB-C, LIS3DH, RV-8803-C7, pulsador | Completo |
| Alimentación y protección | ADP2108, TP4054, FHD4020S-100MT, SDFL2012Q1R0KTF, BAT54C, MBR0530, SP0503BAHTG | Completo |
| Transistores y motor | CJ2312, CJ3415, SI1308EDL, UMH3N, S8050 | Completo para los componentes montados |

Referencias críticas resueltas por el BOM:

| Ref. | Pieza montada | LCSC |
| --- | --- | --- |
| Q2 | R+O S8050 | C20069125 |
| U5 | JSMSEMI TP4054 | C5381776 |
| L1 | cjiang FHD4020S-100MT | C602032 |
| L2 | Sunlord SDFL2012Q1R0KTF | C1042 |
| Q4 | TECH PUBLIC SI1308EDL | C7603347 |
| D1–D4 | JSCJ MBR0530 | C77336 |
| D5 | R+O BAT54C | C22466350 |

Los documentos de MCP73831 y LQH44PN quedan solo como comparación: no
describen los componentes montados en esta fabricación.

La referencia exacta de batería y motor solo será necesaria si se validan
corriente de carga, corriente de arranque o márgenes térmicos.

## Fallos observados en el prototipo

- [x] `TOUCH-01` SDA y SCL intercambiadas en el FPC táctil.
  - `Estado`: CORREGIDO; pendiente de validación con el panel.
  - `Acción`: J1.1 pasa a `SCL` y J1.2 a `SDA`. El cruce de `SDA` se resuelve
    localmente en B.Cu con dos vías; `SCL` permanece en F.Cu.
  - `Cierre`: comprobar detección del FT6336U y niveles/flancos de ambas
    señales con el flex conectado.
- [ ] `TOUCH-02` Revisar holgura, orientación y salida del flex táctil.
- [ ] `MOTOR-01` Corregir la incompatibilidad entre el pinout de Q2 y su huella.
- [ ] `TOUCH-03` Añadir y dimensionar el pull-up de la interrupción táctil.
- [ ] `PWR-02` El USB provoca arranque y ciclos rápidos de encendido/apagado.
  - `Estado`: CONDICIONAL; el síntoma y la vía de realimentación están
    confirmados, pero falta demostrar la secuencia causal.
  - `Hipótesis`: U3 queda alimentado directamente por `USB_VBUS` mientras U2
    está apagado. `TXD` llega sin aislamiento a `U0RXD` y `DTR/RTS` cruzan
    también hacia `EN/GPIO0` mediante Q7; cualquiera de esas vías puede
    realimentar parcialmente el ESP32 y `+3V3`. Esa tensión puede activar
    IO25/Q6 y el latch. Al caer de nuevo la alimentación, el USB repite el
    proceso.
  - `Evidencia`: U3 está configurado como bus-powered; `TXD` y `U0RXD` están
    unidos directamente. Q7 contiene resistencias de base internas de 4,7 kΩ,
    pero no aísla un dominio alimentado de otro apagado. IO25 controla la
    puerta de Q6 mediante R18, Q6 tiene un umbral de 0,45–1 V y R14 sólo lo
    descarga con 100 kΩ. R13/C18 forman una constante de 30 ms. L2 puede
    agravar la caída, pero no explica el disparo inicial desde USB.
  - `Decisión`: adoptar la configuración self-powered de U3. El coste aceptado
    es mantener pulsado el botón durante una programación si el reset del
    ESP32 hace caer el autosostenimiento.
  - `Acción`: alimentar `VDD`, `VIO` y `REGIN` de U3 desde el `+3V3` conmutado;
    detectar `USB_VBUS` mediante 24 kΩ/47 kΩ; añadir 1–5 µF + 100 nF locales
    en `VDD/VIO` y declarar el descriptor USB como self-powered. Si el
    descriptor se programa en la propia placa, montar además 4,7 µF en `VPP`.
    Verificar el consumo de U3 con USB ausente. No usar C18 ni el
    autosostenimiento temprano de IO25 como corrección del cruce de dominios.
  - `Cierre`: al conectar USB con el latch apagado, observar `VSYS`, `+3V3`,
    puerta de Q6 y puerta de Q5. Si `+3V3` sube antes que `VSYS`, queda
    confirmada la realimentación desde U3; repetir aislando `TXD` o forzando
    la puerta de Q6 a GND.
  - `Fuentes`: [CP2104, figura 9](documentation/cp2104-1397921.pdf),
    [ESP32-PICO-D4](documentation/datasheets/ESP32-PICO-series.pdf),
    [CJ2312](documentation/datasheets/CJ2312.pdf) y
    [circuito de referencia](https://circuitcellar.com/resources/quickbits/soft-latching-power-circuits/).
- [ ] `MCU-01` Desacoplo externo local insuficiente en U2.
  - `Estado`: CONFIRMADO.
  - `Hipótesis`: U2 depende de condensadores de `+3V3` alejados y carece del
    par externo recomendado junto al encapsulado.
  - `Evidencia`: el ESP32-PICO-D4 une a `VDD33` los pines 1 `VDDA`, 3 y 4
    `VDDA3P3`, 19 `VDD3P3_RTC` y 37 `VDD3P3_CPU`; su referencia añade
    10 µF + 100 nF externos. En la PCB, C17 es de `EN`; C22 está junto al
    regulador y C3/C4 desacoplan el LIS3DH. El SiP incorpora desacoplo interno,
    pero no elimina ese par externo.
  - `Acción`: añadir 10 µF + 100 nF entre `+3V3` y GND, pegados al grupo de
    pines 1/3/4 de U2, con pista corta y retorno/vía de masa inmediatos. Los
    pines 19 y 37 deben quedar unidos por un plano o recorrido de baja
    impedancia; no se prescriben condensadores adicionales junto a ellos
    porque el PICO-D4 ya integra 100 nF en ambos dominios.
  - `Cierre`: comprobar colocación y retornos en PCB y medir `+3V3` junto a U2
    con Bluetooth y táctil simultáneos, sin caída por debajo de 3,0 V ni
    brownout.
  - `Fuente`: [ESP32-PICO-D4, figuras 7 y 10](documentation/datasheets/ESP32-PICO-series.pdf).

## Revisión sistemática pendiente

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
