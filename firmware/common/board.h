#pragma once
#include <Arduino.h>
#include <esp_system.h>

// GPIO numbers, NOT package pin numbers. Checked against sacre.kicad_sch
// netlist and PCB on 2026-09-14. ESP32-PICO-D4; GPIO16 is internal flash.
namespace Pins {
constexpr uint8_t LATCH = 25;
constexpr uint8_t MOTOR = 13;
constexpr uint8_t BAT_ENABLE = 26;
constexpr uint8_t BAT_ADC = 33;
constexpr uint8_t SDA = 21, SCL = 22;
constexpr uint8_t TOUCH_RESET = 4, TOUCH_INT = 35;
constexpr uint8_t ACC_INT1 = 14, ACC_INT2 = 34;
constexpr uint8_t RTC_INT = 27;
constexpr uint8_t EPD_CS = 5, EPD_DC = 10, EPD_RESET = 9;
constexpr uint8_t EPD_BUSY = 19, EPD_SCK = 18, EPD_MOSI = 23;
}

namespace Board {
inline void begin(const char* test) {
  // First app action: self-hold, before Serial, I2C, SPI or radio.
  digitalWrite(Pins::LATCH, HIGH);
  pinMode(Pins::LATCH, OUTPUT);
  digitalWrite(Pins::MOTOR, LOW);
  pinMode(Pins::MOTOR, OUTPUT);
  digitalWrite(Pins::BAT_ENABLE, LOW);
  pinMode(Pins::BAT_ENABLE, OUTPUT);
  // Release touch reset; the external RC supplies the high level.
  pinMode(Pins::TOUCH_RESET, INPUT);
  digitalWrite(Pins::EPD_CS, HIGH);
  pinMode(Pins::EPD_CS, OUTPUT);
  Serial.begin(115200);
  delay(200);
  Serial.printf("\nSAcre / %s / reset reason=%d\n", test, int(esp_reset_reason()));
  Serial.println("GPIO25 HIGH. x: apagar (soltar antes el boton).");
}

[[noreturn]] inline void powerOff() {
  digitalWrite(Pins::MOTOR, LOW);
  digitalWrite(Pins::BAT_ENABLE, LOW);
  Serial.println("Apagando. Si sigue encendida, soltar el boton.");
  Serial.flush();
  digitalWrite(Pins::LATCH, LOW);
  for (;;) delay(1000); // Do not restart/reassert the latch if power persists.
}

inline int command() {
  if (!Serial.available()) return -1;
  const int c = Serial.read();
  if (c == 'x') powerOff();
  return c;
}

[[noreturn]] inline void fail(const char* message) {
  Serial.printf("ERROR: %s. Corregir y reiniciar; x: apagar.\n", message);
  for (;;) { command(); delay(10); }
}
}
