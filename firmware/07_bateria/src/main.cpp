#include "board.h"
constexpr float CALIBRATION = 1.0f; // Adjust against a multimeter, not battery %.

void setup() {
  Board::begin("bateria / ADC1 GPIO33");
  pinMode(Pins::BAT_ADC, INPUT);
  analogReadResolution(12);
  analogSetPinAttenuation(Pins::BAT_ADC, ADC_11db);
  Serial.println("R5/R6=10k/10k: Vbat=2*Vadc. Comparar con multimetro.");
  Serial.println("No estima porcentaje ni fija umbral de apagado sin calibracion.");
}

void loop() {
  Board::command();
  static uint32_t lastRead = 0;
  if (millis() - lastRead < 2000) { delay(5); return; }
  lastRead = millis();
  digitalWrite(Pins::BAT_ENABLE, HIGH);
  delay(10);
  analogRead(Pins::BAT_ADC); // Discard first sample after enabling divider.
  uint32_t total = 0;
  for (unsigned i = 0; i < 32; ++i) { total += analogReadMilliVolts(Pins::BAT_ADC); delay(1); }
  digitalWrite(Pins::BAT_ENABLE, LOW);
  const float adcMv = total / 32.0f;
  Serial.printf("ADC=%.1f mV; bateria=%.3f V; divisor OFF\n", adcMv, adcMv * 0.002f * CALIBRATION);
}
