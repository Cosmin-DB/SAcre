#include "i2c_io.h"
uint8_t address;
volatile bool int1Edge = false, int2Edge = false;
void IRAM_ATTR onInt1() { int1Edge = true; }
void IRAM_ATTR onInt2() { int2Edge = true; }

void setup() {
  Board::begin("acelerometro LIS3DH");
  I2C::begin();
  pinMode(Pins::ACC_INT1, INPUT);
  pinMode(Pins::ACC_INT2, INPUT);
  unsigned found = 0;
  for (uint8_t a : {uint8_t(0x18), uint8_t(0x19)}) {
    uint8_t id = 0;
    if (I2C::readByte(a, 0x0f, id) && id == 0x33) { address = a; ++found; }
  }
  if (found != 1) Board::fail("LIS3DH: se esperaba WHO_AM_I=0x33 en UNA direccion");
  // SDO/SA0 has an internal pull-up (datasheet table 3): normally 0x19.
  Serial.printf("LIS3DH en 0x%02X; esperado 0x19 por pull-up interno de SA0.\n", address);
  // Power-down first; 10 Hz, +/-2g, high resolution, BDU, little endian.
  // INT1 = data ready; INT2 = |X/Y/Z| above ~1.28 g, latched until SRC read.
  const uint8_t config[][2] = {
    {0x20, 0x00}, {0x21, 0x00}, {0x22, 0x10}, {0x23, 0x88},
    {0x24, 0x02}, {0x25, 0x20}, {0x2e, 0x00}, {0x30, 0x00},
    {0x34, 0x3f}, {0x36, 80}, {0x37, 0}, {0x20, 0x27}
  };
  for (const auto& entry : config) {
    uint8_t actual;
    if (!I2C::writeByte(address, entry[0], entry[1]) ||
        !I2C::readByte(address, entry[0], actual) || actual != entry[1])
      Board::fail("Configuracion LIS3DH no confirmada");
  }
  attachInterrupt(Pins::ACC_INT1, onInt1, RISING);
  attachInterrupt(Pins::ACC_INT2, onInt2, RISING);
  delay(150);
  Serial.println("Reposo: modulo cercano a 1000 mg. Girar y mover para probar ejes/INT2.");
}

void loop() {
  Board::command();
  static uint32_t lastRead = 0;
  if (millis() - lastRead < 100) { delay(1); return; }
  lastRead = millis();
  uint8_t data[6], status, source;
  if (!I2C::readByte(address, 0x27, status)) { Serial.println("ERROR I2C"); return; }
  if (!(status & 0x08)) return;
  const int level1 = digitalRead(Pins::ACC_INT1);
  const int level2 = digitalRead(Pins::ACC_INT2);
  if (!I2C::read(address, 0x28 | 0x80, data, sizeof(data)) ||
      !I2C::readByte(address, 0x35, source)) { Serial.println("ERROR lectura ejes"); return; }
  int mg[3];
  for (int i = 0; i < 3; ++i)
    mg[i] = int16_t(uint16_t(data[2*i]) | (uint16_t(data[2*i+1]) << 8)) / 16;
  noInterrupts();
  const bool edge1 = int1Edge, edge2 = int2Edge;
  int1Edge = int2Edge = false;
  interrupts();
  Serial.printf("mg: X=%d Y=%d Z=%d | INT1=%d INT2=%d flancos=%d/%d SRC2=0x%02X\n",
                mg[0], mg[1], mg[2], level1, level2, edge1, edge2, source);
}
