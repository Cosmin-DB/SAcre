#include "board.h"
#include <SPI.h>
#include <GxEPD2_BW.h>
#include <gdey/GxEPD2_420_GDEY042T81.h>

GxEPD2_BW<GxEPD2_420_GDEY042T81, GxEPD2_420_GDEY042T81::HEIGHT> display(
    GxEPD2_420_GDEY042T81(Pins::EPD_CS, Pins::EPD_DC,
                         Pins::EPD_RESET, Pins::EPD_BUSY));
bool initialized = false;
uint32_t lastRefresh = 0;

void setup() {
  Board::begin("pantalla GDEY042T81 / SSD1683");
  SPI.begin(Pins::EPD_SCK, -1, Pins::EPD_MOSI, Pins::EPD_CS);
  Serial.println("p: patron; w: blanco; n: negro. Un refresco por orden; esperar 5 s.");
}

void loop() {
  const int c = Board::command();
  if (c == 'p' || c == 'w' || c == 'n') {
    if (initialized && millis() - lastRefresh < 5000) {
      Serial.println("Esperar 5 s entre refrescos.");
      return;
    }
    Serial.println("Refresco iniciado; revisar mensajes BUSY del driver.");
    if (!initialized) {
      display.epd2.selectFastFullUpdate(false); // Normal full refresh for first hardware validation.
      display.init(115200, true, 10, false);
      initialized = true;
    }
    display.setRotation(0);
    display.setFullWindow();
    display.firstPage();
    do {
      display.fillScreen(c == 'n' ? GxEPD_BLACK : GxEPD_WHITE);
      if (c == 'p') {
        display.drawRect(0, 0, display.width(), display.height(), GxEPD_BLACK);
        display.setTextColor(GxEPD_BLACK);
        display.setTextSize(2);
        display.setCursor(12, 15);
        display.print("SAcre 400x300 / SSD1683");
        for (int y = 60; y < display.height() - 10; y += 30)
          for (int x = 10; x < display.width() - 30; x += 30)
            if (((x / 30) + (y / 30)) % 2 == 0)
              display.fillRect(x, y, 25, 25, GxEPD_BLACK);
      }
    } while (display.nextPage());
    display.hibernate(); // Stop booster; image remains visible without power.
    lastRefresh = millis();
    Serial.println("Driver finalizado. Confirmar imagen; esto no detecta panel ausente.");
  }
  delay(5);
}
