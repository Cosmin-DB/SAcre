# Pruebas independientes de SAcre

Autor: Cosmin Dobrescu. Destino: ESP32-PICO-D4, esquema/PCB de septiembre
de 2026. Cada carpeta es un proyecto PlatformIO que sustituye al anterior
al grabarlo. No hay una aplicación que combine los periféricos.

`common/board.h` concentra los GPIO, autosostén y apagado; `common/i2c_io.h`
contiene únicamente acceso a registros. Conservar `common/` junto a los
proyectos. Para integrarlos después, reutilizar sus rutinas de inicialización
y lectura, manteniendo un único arranque del latch y del bus I²C.

## Compilar y cargar

Instalar PlatformIO Core o la extensión PlatformIO de VS Code. Abrir la
carpeta de la prueba, o ejecutar desde la raíz del repositorio:

```text
pio run -d firmware/01_latch
pio run -d firmware/01_latch -t upload
pio device monitor -b 115200
```

Cambiar `01_latch` por la prueba elegida. Si hay varios puertos, añadir
`--upload-port COMx` al comando de carga y `-p COMx` al monitor.
Los comandos son letras/números ASCII; Intro es opcional.

Usar batería conectada. Mantener SW1 pulsado durante carga/reset y hasta
el arranque del programa; el USB no sustituye al pulsador de encendido.
En todas las pruebas, `x` apaga: soltar SW1 para que pueda cortar el latch.
Una operación en curso (por ejemplo, refrescar pantalla) termina antes de
atender el comando. Abrir el monitor puede perder el primer mensaje;
las pruebas de lectura siguen mostrando su estado periódicamente.

## Qué probar y qué observar

| Proyecto | Operación | Resultado esperado |
| --- | --- | --- |
| `01_latch` | Soltar SW1 después de arrancar; `x` apaga. `b` habilita lectura del botón; soltar, pulsar 4 s y soltar. | Sigue encendida sin pulsar; contador continuo, sin reinicios; apagado efectivo. |
| `02_pantalla` | `p` patrón, `w` blanco, `n` negro; ≥5 s entre refrescos. | Marco/texto/cuadrícula completos, blanco/negro uniformes. Imagen retenida al apagar. |
| `03_tactil` | Tocar, arrastrar y usar dos dedos; `r` repite reset. | Lecturas 0–2 puntos, coordenadas cambiantes, regreso a 0 al soltar; INT/flancos observables. |
| `04_motor` | `1`, `2`, `3`: pulso de 200 ms a 25 %, 50 %, 100 %; `0` para. | Vibra y se detiene; reposo de 2 s entre órdenes, sin reinicios. A PWM bajo puede no arrancar. |
| `05_acelerometro` | Girar y mover la placa. | Identidad `0x33`; en reposo módulo de aceleración ≈1000 mg; signos/ejes cambian; INT1 datos listos, INT2 al superar ≈1,28 g en algún eje. |
| `06_rtc` | `t` inicializa, `a` alarma al próximo minuto, `c` cancela/limpia. Apagar y volver a encender sin `t`. | Segundos avanzan y hora se conserva con batería; alarma da AF=1 e INT=0, `c` los libera. |
| `07_bateria` | Lectura automática cada 2 s; comparar con multímetro. | Tensión próxima a la batería; divisor se deshabilita tras cada lectura. |
| `08_bluetooth` | Cliente BLE: conectar a `SAcre-BLE-test`, leer/suscribirse y reconectar. | Contador ASCII creciente por lectura/notificación, reconexión y sin reinicios. |

BLE: servicio `b0d80001-6c3a-4d2e-a947-5ac000000001`, característica
`b0d80002-6c3a-4d2e-a947-5ac000000001`. Sin emparejamiento obligatorio;
usar un explorador GATT, no sólo la lista de dispositivos del sistema.

## GPIO contrastados con esquema y PCB

Son números de **GPIO**, no de patilla del encapsulado. Los 17 valores de
`common/board.h` se contrastan con redes de U2 y pads reales de la PCB.

| Función / red | GPIO |
| --- | --- |
| Autosostén `PWR_BTN` | 25 |
| Motor `Vibration_PWM` | 13 |
| Batería `VBAT_SENSE_EN` / `ADC_BATT` | 26 / 33 (ADC1) |
| I²C `SDA` / `SCL` | 21 / 22 |
| Táctil `RES_TOUCH_CTRL` / `INT_TOUCH` | 4 / 35 |
| Acelerómetro `ACC_INT1` / `ACC_INT2` | 14 / 34 |
| RTC `RTC_INT` | 27 |
| Pantalla `SS` / `DC` / `RES` | 5 / 10 / 9 |
| Pantalla `BUSY` / `SCK` / `MOSI` | 19 / 18 / 23 |

GPIO34/35 se usan sólo como entradas, sin pull-up interno. GPIO16 y las
señales internas de flash no se usan. GPIO9/10 sí están disponibles en el
PICO-D4 con esta configuración DIO. `RTC_EVI` no está conectado al ESP32.

## Decisiones que hay que conservar al integrar

- **Latch:** GPIO25 HIGH es la primera acción de `setup()`. Los proyectos de
  periféricos mantienen HIGH continuamente. Sólo `01_latch`, tras `b`, usa
  ventanas de entrada de 200 µs para leer SW1 a través de D5/R18; dependen
  del circuito R13/C18 (~30 ms) y necesitan validación física. No se hereda
  automáticamente esta lectura del botón en el resto de pruebas.
- **Táctil:** GPIO4 open-drain LOW 70 ms, liberar y esperar 350 ms, conforme
  a R11||R28=500 Ω y C25=10 µF. Nunca conducir HIGH en push-pull. Se lee el
  FT6336U en `0x38`, 100 kHz, con transferencias acotadas y errores explícitos.
  Las coordenadas son nativas; no se presupone orientación del flex/pantalla.
- **Acelerómetro:** SA0/SDO tiene pull-up interno según el datasheet, por lo
  que se espera `0x19` aunque no tenga conexión externa. Se comprueban
  `0x18/0x19` y WHO_AM_I antes de escribir. Modo ±2 g, 10 Hz, 12 bits,
  BDU; INT1 indica datos listos e INT2 movimiento por umbral.
- **RTC opcional:** si falta, el programa informa y permite apagar.
  Arrancar no cambia la hora. `t` reinicializa la configuración escribible,
  incluido offset=0, a fecha/hora de **compilación**, no a la hora actual del
  PC al cargar. `V2F`/RESET impiden presentar la hora como válida.
  Cada escritura termina con lectura por la errata I²C del RV-8803;
  no se generan pulsos de recuperación indiscriminados. `a` dedica INT a la
  alarma; se deja AF activo hasta `c`, y puede repetirse al día siguiente
  si se deja armada. La batería mantiene esos registros al apagar el ESP32.
- **Pantalla:** driver específico GDEY042T81/SSD1683, sólo refresco completo,
  hibernación al terminar. El SPI no identifica el panel: fin del driver
  no demuestra imagen correcta; comprobar BUSY y aspecto físico.
- **Motor:** apagado inicial y pulsos sólo por orden. PWM no limita el pico
  de arranque ni protege un motor incompatible. Modelo/corriente del motor
  siguen condicionados a lo anotado en el README de hardware.
- **Batería:** R5/R6=10 kΩ/10 kΩ, factor 2; ADC en mV y atenuación 11 dB.
  Ajustar `CALIBRATION` con multímetro. No inventar porcentaje ni umbral de
  apagado: queda para la integración, tras medir la batería y la caída bajo carga.

## Verificación y fuentes

Verificación 2026-09-14: ocho proyectos compilados y enlazados para `pico32`;
17 GPIO contrastados automáticamente con netlist del esquema y pads de la PCB.
Sin avisos en el código propio; dos avisos de variables sin uso en drivers
ajenos al panel elegido dentro de GxEPD2. No probado en placa:
compilación y contraste de GPIO no validan el funcionamiento físico
ni la coexistencia de periféricos. La integración y pruebas combinadas quedan
para una fase posterior.

Base fijada: PlatformIO espressif32 6.1.0 / Arduino-ESP32 2.0.7, `pico32`,
flash 4 MB DIO. Pantalla: GxEPD2 1.6.4, Adafruit GFX 1.11.11 y BusIO 1.16.1.
El resto usa Arduino/Wire/BLE incluidos en esa plataforma.

Referencias: [diseño y decisiones actuales](../README.md),
[LIS3DH](../documentation/lis3dh_datasheet.pdf),
[RTC: mapa, alarmas e inicialización](../RV-8803/RV-8803-C7_App-Manual.pdf),
[errata RTC](../documentation/datasheets/RV-8803-C7-errata.pdf),
[FT6336U](../documentation/datasheets/FT6336U.pdf),
[panel](../documentation/datasheets/GDEY042T81-T02.pdf),
[driver GxEPD2](https://github.com/ZinggJM/GxEPD2/tree/1.6.4),
[formato de registros táctiles](https://github.com/bitbank2/FT6236G/blob/master/src/FT6236G.cpp).
Se toma como referencia funcional el proyecto local de mayo/junio de 2026
`260527-095526-pico32`; sus pines antiguos no son la referencia de esta versión.
