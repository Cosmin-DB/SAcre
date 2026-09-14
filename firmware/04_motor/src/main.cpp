#include "board.h"
constexpr uint8_t CHANNEL = 0;
bool running = false;
uint32_t started = 0, stopped = 0;

void stopMotor() {
  ledcWrite(CHANNEL, 0);
  running = false;
  stopped = millis();
  Serial.println("Motor OFF.");
}

void setup() {
  Board::begin("motor");
  ledcSetup(CHANNEL, 20000, 8);
  ledcAttachPin(Pins::MOTOR, CHANNEL);
  ledcWrite(CHANNEL, 0);
  Serial.println("1/2/3: pulso 200 ms a 25/50/100%; 0: parar. Reposo 2 s.");
  Serial.println("PWM no limita la corriente de arranque. Confirmar motor del BOM.");
}

void loop() {
  // Stop PWM BEFORE common powerOff (digitalWrite cannot override LEDC).
  if (Serial.available() && Serial.peek() == 'x') stopMotor();
  const int c = Board::command();
  if (running && millis() - started >= 200) stopMotor();
  if (c == '0') stopMotor();
  if (c >= '1' && c <= '3') {
    if (running || millis() - stopped < 2000) {
      Serial.println("Esperar 2 s con motor parado.");
    } else {
      const uint8_t duty = c == '1' ? 64 : c == '2' ? 128 : 255;
      started = millis();
      running = true;
      ledcWrite(CHANNEL, duty);
      Serial.printf("Motor PWM=%u/255 por 200 ms.\n", duty);
    }
  }
  delay(1);
}
