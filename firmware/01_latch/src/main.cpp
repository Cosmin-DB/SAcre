#include "board.h"

// Only this project briefly releases GPIO25 to sense SW1 through D5/R18.
// C18/R13 retains the power state (~30 ms RC). The 200 us window must be
// checked on real hardware; peripheral projects keep GPIO25 driven HIGH.
bool sampleButton() {
  noInterrupts();
  pinMode(Pins::LATCH, INPUT_PULLUP);
  delayMicroseconds(200);
  const bool pressed = digitalRead(Pins::LATCH) == LOW;
  digitalWrite(Pins::LATCH, HIGH);
  pinMode(Pins::LATCH, OUTPUT);
  interrupts();
  return pressed;
}

bool sensing = false, armed = false, waitingRelease = false;
bool candidate = false, pressed = false;
uint32_t changedAt = 0, pressedAt = 0, lastSample = 0, lastStatus = 0;

void setup() {
  Board::begin("latch");
  Serial.println("Soltar SW1: debe seguir encendida. b: probar lectura de boton.");
  Serial.println("Con b activo: soltar, mantener 4 s y soltar para apagar.");
}

void loop() {
  const int c = Board::command();
  if (c == 'b') { sensing = true; Serial.println("Lectura SW1 activada."); }
  const uint32_t now = millis();
  if (sensing && now - lastSample >= 50) {
    lastSample = now;
    const bool raw = sampleButton();
    if (raw != candidate) { candidate = raw; changedAt = now; }
    if (now - changedAt >= 100) {
      if (!candidate) armed = true;
      if (candidate != pressed) {
        pressed = candidate;
        pressedAt = now;
        Serial.println(pressed ? "SW1 pulsado" : "SW1 suelto");
      }
      if (armed && pressed && !waitingRelease && now - pressedAt >= 4000) {
        waitingRelease = true;
        Serial.println("Soltar SW1 para apagar.");
      }
      if (waitingRelease && !pressed) Board::powerOff();
    }
  }
  if (now - lastStatus >= 2000) {
    lastStatus = now;
    Serial.printf("Autososten activo; uptime=%lu s\n", (unsigned long)(now / 1000));
  }
  delay(1);
}
