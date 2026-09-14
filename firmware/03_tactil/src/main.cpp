#include "i2c_io.h"
constexpr uint8_t ADDRESS = 0x38;
volatile bool touchEdge = false;
void IRAM_ATTR onTouch() { touchEdge = true; }

void resetTouch() {
  // R11 || R28 = 500 ohm, C25 = 10 uF. Never drive reset push-pull HIGH.
  digitalWrite(Pins::TOUCH_RESET, LOW);
  pinMode(Pins::TOUCH_RESET, OUTPUT_OPEN_DRAIN);
  delay(70);
  digitalWrite(Pins::TOUCH_RESET, HIGH); // Open drain release, external pull-up.
  delay(350);
}

void setup() {
  Board::begin("tactil FT6336U");
  pinMode(Pins::TOUCH_INT, INPUT); // GPIO35 has no internal pull-up.
  resetTouch();
  I2C::begin();
  attachInterrupt(Pins::TOUCH_INT, onTouch, FALLING);
  Serial.println("Tocar/esquinar/arrastrar: coordenadas nativas, sin rotar. r: reset.");
}

void loop() {
  if (Board::command() == 'r') { resetTouch(); Serial.println("Reset completado."); }
  static uint32_t lastRead = 0, lastStatus = 0;
  static uint8_t previousCount = 0;
  if (millis() - lastRead < 50) { delay(1); return; }
  lastRead = millis();
  uint8_t data[13]; // TD_STATUS + two 6-byte point records; always bounded.
  if (!I2C::read(ADDRESS, 0x02, data, sizeof(data))) {
    if (millis() - lastStatus >= 1000) {
      lastStatus = millis();
      Serial.println("ERROR I2C tactil 0x38; comprobar flex, alimentacion y reset.");
    }
    return;
  }
  const uint8_t count = data[0] & 0x0f;
  if (count > 2) {
    Serial.printf("ERROR: TD_STATUS=0x%02X (maximo 2 puntos).\n", data[0]);
    return;
  }
  if (count || count != previousCount || millis() - lastStatus >= 1000) {
    lastStatus = millis();
    noInterrupts();
    const bool edge = touchEdge;
    touchEdge = false;
    interrupts();
    Serial.printf("puntos=%u INT=%d flanco=%d", count, digitalRead(Pins::TOUCH_INT), edge);
    for (uint8_t i = 0; i < count; ++i) {
      const uint8_t* p = data + 1 + 6 * i;
      Serial.printf(" | id=%u evento=%u x=%u y=%u", p[2] >> 4, p[0] >> 6,
                    ((p[0] & 15) << 8) | p[1], ((p[2] & 15) << 8) | p[3]);
    }
    Serial.println();
  }
  previousCount = count;
}
