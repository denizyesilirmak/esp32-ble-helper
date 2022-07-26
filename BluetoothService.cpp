#include "BluetoothService.h"



//Battery level service
#define BATTERY_SERVICE_UUID "0000180f-0000-1000-8000-00805f9b34fb"
BLECharacteristic BatteryLevelCharacteristic(BLEUUID((uint16_t)0x2A19), BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
BLEDescriptor BatteryLevelDescriptor(BLEUUID((uint16_t)0x2901));

BLECharacteristic BatteryVoltageCharacteristic(BLEUUID((uint16_t)0x2AE1), BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
BLEDescriptor BatteryVoltageDescriptor(BLEUUID((uint16_t)0x2901));


//Device Info Service
#define DEVICE_INFO_UUID "91bad492-b950-4226-aa2b-4ede9fa42f59"
BLECharacteristic HardwareRevisionCharacteristic(BLEUUID((uint16_t)0x2A27), BLECharacteristic::PROPERTY_READ);
BLECharacteristic FirmwareRevisionCharacteristic(BLEUUID((uint16_t)0x2A26), BLECharacteristic::PROPERTY_READ);
BLECharacteristic ManufacturerNameCharacteristic(BLEUUID((uint16_t)0x2A29), BLECharacteristic::PROPERTY_READ);
BLECharacteristic SerialNumberCharacteristic(BLEUUID((uint16_t)0x2A25), BLECharacteristic::PROPERTY_READ);
BLECharacteristic UsageCounterCharacteristic("eceb51d6-0ce0-11ed-861d-0242ac120002", BLECharacteristic::PROPERTY_READ);
BLEDescriptor UsageCounterDescriptor(BLEUUID((uint16_t)0x2901));

//Sensor serial service
#define SENSOR_UUID "689e470c-0cc8-11ed-861d-0242ac120002"
BLECharacteristic SensorTXCharacteristic("8c820036-0cc9-11ed-861d-0242ac120002", BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
BLEDescriptor SensorServiceTXDescriptor(BLEUUID((uint16_t)0x2901));
BLECharacteristic SensorRXCharacteristic("b142e8e0-0cc9-11ed-861d-0242ac120002", BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
BLEDescriptor SensorServiceRXDescriptor(BLEUUID((uint16_t)0x2901));


Preferences preferences;


class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      Serial.println("connected");
    };
    void onDisconnect(BLEServer* pServer) {
      Serial.println("disconnected");
    }
};


BluetoothService::BluetoothService(
  String deviceName,
  String manufacturerName,
  String modelName,
  String ModelCode,
  String CountryCode,
  String HardwareRevision,
  String FirmwareRevision
) {
  init(
    deviceName,
    manufacturerName,
    modelName,
    ModelCode,
    CountryCode,
    HardwareRevision,
    FirmwareRevision
  );
}

void BluetoothService::init(
  String deviceName,
  String manufacturerName,
  String modelName,
  String ModelCode,
  String CountryCode,
  String HardwareRevision,
  String FirmwareRevision
) {
  BluetoothService::deviceName = deviceName;
  BluetoothService::manufacturerName = manufacturerName;
  BluetoothService::modelName = modelName;
  BluetoothService::ModelCode = ModelCode;
  BluetoothService::CountryCode = CountryCode;
  BluetoothService::HardwareRevision = HardwareRevision;
  BluetoothService::FirmwareRevision = FirmwareRevision;

}

void BluetoothService::setup() {

  Serial.println("BluetoothService::setup()");
  BLEDevice::init(BluetoothService::deviceName.c_str());
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *batteryService = pServer->createService(BATTERY_SERVICE_UUID);
  batteryService->addCharacteristic(&BatteryLevelCharacteristic);
  BatteryLevelDescriptor.setValue("Percentage 0 - 100");
  BatteryLevelCharacteristic.addDescriptor(&BatteryLevelDescriptor);
  BatteryLevelCharacteristic.addDescriptor(new BLE2902());

  batteryService->addCharacteristic(&BatteryVoltageCharacteristic);
  BatteryVoltageDescriptor.setValue("Voltage 0 - 5 Volt");
  BatteryVoltageCharacteristic.addDescriptor(&BatteryVoltageDescriptor);
  BatteryVoltageCharacteristic.addDescriptor(new BLE2902());


  BLEService *deviceInfoService = pServer->createService(DEVICE_INFO_UUID);
  deviceInfoService->addCharacteristic(&HardwareRevisionCharacteristic);
  HardwareRevisionCharacteristic.setValue(BluetoothService::HardwareRevision.c_str());
  deviceInfoService->addCharacteristic(&FirmwareRevisionCharacteristic);
  FirmwareRevisionCharacteristic.setValue(BluetoothService::FirmwareRevision.c_str());
  deviceInfoService->addCharacteristic(&ManufacturerNameCharacteristic);
  ManufacturerNameCharacteristic.setValue(BluetoothService::manufacturerName.c_str());

  deviceInfoService->addCharacteristic(&UsageCounterCharacteristic);
  UsageCounterDescriptor.setValue("Total usage time in minutes");
  UsageCounterCharacteristic.addDescriptor(&UsageCounterDescriptor);
  preferences.begin("store", false);
  unsigned long counter = preferences.getULong("counter", 0);
  preferences.end();
  String counterStr = (String)(counter/2.000);
  UsageCounterCharacteristic.setValue(counterStr.c_str());

  deviceInfoService->addCharacteristic(&SerialNumberCharacteristic);
  unsigned char mac_base[6] = {0, 0, 0, 0, 0, 0};
  esp_efuse_mac_get_default(mac_base);
  String serialNumber = String(mac_base[0], HEX)
                        + String(mac_base[1], HEX)
                        + String(mac_base[2], HEX)
                        + String(mac_base[3], HEX)
                        + String(mac_base[4], HEX)
                        + String(mac_base[5] + 2, HEX);

  //  std::string str = String(mac_base[0], HEX);
  SerialNumberCharacteristic.setValue(std::string(serialNumber.c_str()));

  BLEService *sensorService = pServer->createService(SENSOR_UUID);
  sensorService->addCharacteristic(&SensorTXCharacteristic);
  SensorServiceTXDescriptor.setValue("mobile-to-esp");
  SensorTXCharacteristic.addDescriptor(&SensorServiceTXDescriptor);

  sensorService->addCharacteristic(&SensorRXCharacteristic);
  SensorServiceRXDescriptor.setValue("esp-to-mobile-magnetic field-0-4096");
  SensorRXCharacteristic.addDescriptor(&SensorServiceRXDescriptor);


  deviceInfoService->start();
  batteryService->start();
  sensorService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(BATTERY_SERVICE_UUID);
  pAdvertising->addServiceUUID(DEVICE_INFO_UUID);



  pServer->getAdvertising()->start();

  Serial.println("Waiting a client connection to notify...");
}

void BluetoothService::notifyBatteryLevel(uint8_t level) {
  if (millis() - BluetoothService::batteryDebounceTime > BluetoothService::batteryDebounceDelay) {
    BluetoothService::batteryDebounceTime = millis();

    if (level >= 0 && level <= 100) {
      BatteryLevelCharacteristic.setValue(&level, 1);
    }
    else {
      uint8_t battery_level_range_error = 101;
      BatteryLevelCharacteristic.setValue(&battery_level_range_error, 1);
    }
    BatteryLevelCharacteristic.notify();
  }
}

void BluetoothService::notifyBatteryVoltage(float voltage) {
  if (millis() - BluetoothService::batteryVoltageDebounceTime > BluetoothService::batteryVoltageDebounceDelay) {
    BluetoothService::batteryVoltageDebounceTime = millis();

    String voltageString = String(voltage, 3);

    if (voltage >= 0 && voltage <= 5) {
      BatteryVoltageCharacteristic.setValue(std::string(voltageString.c_str()));
    }
    else {
      String battery_voltage_range_error = "5.1";
      BatteryVoltageCharacteristic.setValue(std::string(battery_voltage_range_error.c_str()));
    }
    BatteryVoltageCharacteristic.notify();
  }
}

void BluetoothService::setBatteryLevelDebounceTime(unsigned long dt) {
  BluetoothService::batteryDebounceDelay = dt;
}

void BluetoothService::updateUsageCounter() {
  if (millis() - BluetoothService::counterTimerDebounceTime > BluetoothService::counterTimerDebounceDelay) {
    BluetoothService::counterTimerDebounceTime = millis();

    preferences.begin("store", false);
    Serial.println("BluetoothService::updateUsageCounter()");
    unsigned long counter = preferences.getULong("counter", 0);

    counter++;


    preferences.putULong("counter", counter);
    preferences.end();
    String counterStr = (String)(counter/2.000);

    UsageCounterCharacteristic.setValue(counterStr.c_str());

  }
}
