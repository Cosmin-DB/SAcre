#include "board.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <atomic>

constexpr char SERVICE[] = "b0d80001-6c3a-4d2e-a947-5ac000000001";
constexpr char COUNTER[] = "b0d80002-6c3a-4d2e-a947-5ac000000001";
std::atomic<bool> connected{false}, restartAdvertising{false};
BLECharacteristic* characteristic;

class Callbacks : public BLEServerCallbacks {
  void onConnect(BLEServer*) override { connected.store(true); }
  void onDisconnect(BLEServer*) override {
    connected.store(false);
    restartAdvertising.store(true);
  }
} callbacks;

void setup() {
  Board::begin("Bluetooth LE");
  BLEDevice::init("SAcre-BLE-test");
  BLEServer* server = BLEDevice::createServer();
  server->setCallbacks(&callbacks);
  BLEService* service = server->createService(SERVICE);
  characteristic = service->createCharacteristic(COUNTER,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
  characteristic->addDescriptor(new BLE2902());
  characteristic->setValue("0");
  service->start();
  BLEAdvertising* advertising = BLEDevice::getAdvertising();
  advertising->addServiceUUID(SERVICE);
  advertising->setScanResponse(true);
  advertising->start();
  Serial.println("Buscar SAcre-BLE-test con un cliente BLE; leer/suscribirse al contador ASCII.");
}

void loop() {
  Board::command();
  if (restartAdvertising.exchange(false)) BLEDevice::startAdvertising();
  static uint32_t lastUpdate = 0, count = 0;
  if (millis() - lastUpdate >= 1000) {
    lastUpdate = millis();
    char value[16];
    snprintf(value, sizeof(value), "%lu", (unsigned long)++count);
    characteristic->setValue(value);
    if (connected.load()) characteristic->notify();
    Serial.printf("BLE conectado=%d contador=%s\n", connected.load(), value);
  }
  delay(5);
}
