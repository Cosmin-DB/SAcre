#include "i2c_io.h"
#include <cstring>
constexpr uint8_t ADDRESS = 0x32;
constexpr uint8_t EXT = 0x0d, FLAGS = 0x0e, CONTROL = 0x0f;
constexpr uint8_t AF = 0x08, AIE = 0x08, V2F = 0x02;
uint8_t bcd(int value) { return uint8_t((value / 10) * 16 + value % 10); }
int decimal(uint8_t value) { return (value >> 4) * 10 + (value & 15); }
bool validBcd(uint8_t value, int low, int high) {
  return (value & 15) <= 9 && (value >> 4) <= 9 && decimal(value) >= low && decimal(value) <= high;
}

bool readTime(uint8_t* data) {
  if (!I2C::read(ADDRESS, 0x00, data, 7)) return false;
  return validBcd(data[0], 0, 59) && validBcd(data[1], 0, 59) &&
         validBcd(data[2], 0, 23) && validBcd(data[4], 1, 31) &&
         validBcd(data[5], 1, 12) && validBcd(data[6], 0, 99);
}

bool cancelAlarm() {
  uint8_t control;
  if (!I2C::readByte(ADDRESS, CONTROL, control)) return false;
  if (!I2C::writeByte(ADDRESS, CONTROL, control & ~AIE)) return false;
  // Writing 1 leaves other flags unchanged, including low-voltage evidence.
  return I2C::writeByte(ADDRESS, FLAGS, 0x3f & ~AF);
}

bool setBuildTime() {
  char monthName[4];
  int day, year, hour, minute, second;
  if (sscanf(__DATE__, "%3s %d %d", monthName, &day, &year) != 3 ||
      sscanf(__TIME__, "%d:%d:%d", &hour, &minute, &second) != 3) return false;
  const char* months = "JanFebMarAprMayJunJulAugSepOctNovDec";
  const char* pos = strstr(months, monthName);
  if (!pos) return false;
  const int month = (pos - months) / 3 + 1;
  const int offsets[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
  const int y = year - (month < 3);
  const int weekday = (y + y/4 - y/100 + y/400 + offsets[month-1] + day) % 7;
  const uint8_t time[] = {bcd(second), bcd(minute), bcd(hour), uint8_t(1 << weekday),
                          bcd(day), bcd(month), bcd(year % 100)};
  if (!I2C::writeByte(ADDRESS, CONTROL, 0x01)) {
    I2C::writeByte(ADDRESS, CONTROL, 0x00); // A failed readback may follow an accepted write.
    return false;
  }
  // Explicit command t resets the writable configuration, including offset.
  // Required after V2F/POR; never performed automatically on ESP32 reset.
  const uint8_t settings[] = {0x80, 0x80, 0x80, 0, 0, 0}; // 08h..0Dh
  bool ok = I2C::write(ADDRESS, 0x08, settings, sizeof(settings)) &&
            I2C::writeByte(ADDRESS, 0x2c, 0) &&
            I2C::writeByte(ADDRESS, 0x2f, 0) &&
            I2C::write(ADDRESS, 0x00, time, sizeof(time));
  const bool resumed = I2C::writeByte(ADDRESS, CONTROL, 0);
  if (!ok || !resumed) return false;
  return I2C::writeByte(ADDRESS, FLAGS, 0); // Clear invalid-time flags only after successful init.
}

bool armAlarm() {
  uint8_t data[7], flags, control, ext;
  if (!readTime(data) || !I2C::readByte(ADDRESS, FLAGS, flags) || (flags & V2F) ||
      !I2C::readByte(ADDRESS, CONTROL, control) || (control & 1) ||
      !I2C::readByte(ADDRESS, EXT, ext)) return false;
  if (!cancelAlarm()) return false;
  const int next = (decimal(data[2]) * 60 + decimal(data[1]) + 1) % (24 * 60);
  const uint8_t alarm[] = {bcd(next % 60), bcd(next / 60), 0x80};
  // Disable other interrupt sources so GPIO27 proves the alarm specifically.
  if (!I2C::writeByte(ADDRESS, CONTROL, 0) ||
      !I2C::writeByte(ADDRESS, EXT, ext & 0x0f) ||
      !I2C::write(ADDRESS, 0x08, alarm, sizeof(alarm)) ||
      !I2C::writeByte(ADDRESS, FLAGS, 0x3f & ~AF) ||
      !I2C::writeByte(ADDRESS, CONTROL, AIE)) return false;
  Serial.printf("Alarma a las %02d:%02d (proximo minuto); GPIO27 debe bajar.\n", next/60, next%60);
  return true;
}

void setup() {
  Board::begin("RTC RV-8803-C7");
  I2C::begin();
  pinMode(Pins::RTC_INT, INPUT); // R22 supplies the external pull-up.
  uint8_t flags;
  if (!I2C::readByte(ADDRESS, FLAGS, flags)) Board::fail("RTC ausente/no responde (montaje opcional)");
  Serial.println("Lectura sin cambiar hora. t: inicializar a fecha/hora de compilacion.");
  Serial.println("a: alarma proximo minuto; c: cancelar/limpiar alarma; x: apagar.");
  Serial.printf("Compilado: %s %s; flags iniciales=0x%02X\n", __DATE__, __TIME__, flags);
}

void loop() {
  const int c = Board::command();
  if (c == 't') Serial.println(setBuildTime() ? "Hora/configuracion inicializadas." : "ERROR al ajustar RTC.");
  if (c == 'a' && !armAlarm()) Serial.println("ERROR alarma; comprobar hora valida (t) e I2C.");
  if (c == 'c') Serial.println(cancelAlarm() ? "Alarma cancelada." : "ERROR cancelando alarma.");
  static uint32_t lastRead = 0;
  if (millis() - lastRead >= 1000) {
    lastRead = millis();
    uint8_t data[7], flags, control;
    if (!I2C::readByte(ADDRESS, FLAGS, flags) || !I2C::readByte(ADDRESS, CONTROL, control)) {
      Serial.println("ERROR I2C RTC"); return;
    }
    if (!readTime(data) || (flags & V2F) || (control & 1)) {
      Serial.printf("Hora NO valida/parada; flags=0x%02X control=0x%02X; t: inicializar.\n", flags, control);
      return;
    }
    Serial.printf("20%02d-%02d-%02d %02d:%02d:%02d flags=0x%02X INT=%d AF=%d\n",
                  decimal(data[6]), decimal(data[5]), decimal(data[4]), decimal(data[2]),
                  decimal(data[1]), decimal(data[0]), flags, digitalRead(Pins::RTC_INT), bool(flags & AF));
    // Leave AF latched for inspection. User clears it with c.
  }
  delay(5);
}
