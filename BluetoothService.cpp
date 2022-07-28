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

BLECharacteristic ButtonCharacteristic("b142e8e1-0cc9-11ed-861d-0242ac120002", BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
BLEDescriptor ButtonDescriptor(BLEUUID((uint16_t)0x2901));

Preferences preferences;

void (*_onDeviceConnectionCallback)(String status);
void (*_onMessageFromClientCallback)(String message);

/**
   @brief Callback function of client's connection and disconneciton events.
*/
class ServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      //Serial.println("connected");
      _onDeviceConnectionCallback("connected");
    };
    void onDisconnect(BLEServer* pServer) {
      //Serial.println("disconnected");
      _onDeviceConnectionCallback("disconnected");
    }
};

/**
   @brief Callback function of sensor characteristic for on incoming messages.
*/
class CharacteristicCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();
      if (rxValue.length() > 0) {
        //  Serial.print("Received Value: ");
        String received = "";
        for (int i = 0; i < rxValue.length(); i++) {
          received += rxValue[i];
        }
        //Serial.println(received);
        _onMessageFromClientCallback(received);
      }
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

  //Initialize ble device and its server.
  //Bindings for server callback class
  BLEDevice::init(BluetoothService::deviceName.c_str());
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new ServerCallbacks());

  //Initialize Battery service
  BLEService *batteryService = pServer->createService(BATTERY_SERVICE_UUID);

  //Battery level characteristic
  //Sends battery level percentage
  batteryService->addCharacteristic(&BatteryLevelCharacteristic);
  BatteryLevelDescriptor.setValue("Percentage 0 - 100");
  BatteryLevelCharacteristic.addDescriptor(&BatteryLevelDescriptor);
  BatteryLevelCharacteristic.addDescriptor(new BLE2902());

  //Battery voltage characteristic
  //Sends battery voltage information
  batteryService->addCharacteristic(&BatteryVoltageCharacteristic);
  BatteryVoltageDescriptor.setValue("Voltage 0 - 5 Volt");
  BatteryVoltageCharacteristic.addDescriptor(&BatteryVoltageDescriptor);
  BatteryVoltageCharacteristic.addDescriptor(new BLE2902());


  //Device Info Service
  BLEService *deviceInfoService = pServer->createService(DEVICE_INFO_UUID);

  //Characteristic of harware revision number
  deviceInfoService->addCharacteristic(&HardwareRevisionCharacteristic);
  HardwareRevisionCharacteristic.setValue(BluetoothService::HardwareRevision.c_str());

  //Characteristic of firmware revision number
  deviceInfoService->addCharacteristic(&FirmwareRevisionCharacteristic);
  FirmwareRevisionCharacteristic.setValue(BluetoothService::FirmwareRevision.c_str());

  //Characteristic of manufacturer name
  deviceInfoService->addCharacteristic(&ManufacturerNameCharacteristic);
  ManufacturerNameCharacteristic.setValue(BluetoothService::manufacturerName.c_str());

  //Characteristic of usage counter. 
  //It is responsible for sending information about how long the device has been on.
  deviceInfoService->addCharacteristic(&UsageCounterCharacteristic);
  UsageCounterDescriptor.setValue("Total usage time in minutes");
  UsageCounterCharacteristic.addDescriptor(&UsageCounterDescriptor);
  preferences.begin("store", false);
  unsigned long counter = preferences.getULong("counter", 0);
  preferences.end();
  String counterStr = (String)(counter / 2.000);
  UsageCounterCharacteristic.setValue(counterStr.c_str());

  //Characteristic of device serial number
  //We are using mac address as serial number
  //The mac address given by esp32 and the scanned mac address do not match
  //We use the mac address seen during scanning is done as the serial number.
  deviceInfoService->addCharacteristic(&SerialNumberCharacteristic);
  unsigned char mac_base[6] = {0, 0, 0, 0, 0, 0};
  esp_efuse_mac_get_default(mac_base);
  String serialNumber = String(mac_base[0], HEX)
                        + String(mac_base[1], HEX)
                        + String(mac_base[2], HEX)
                        + String(mac_base[3], HEX)
                        + String(mac_base[4], HEX)
                        + String(mac_base[5] + 2, HEX);

  SerialNumberCharacteristic.setValue(std::string(serialNumber.c_str()));


  //Sensor service
  BLEService *sensorService = pServer->createService(SENSOR_UUID);

  //Sends 16 bit sensor value.
  sensorService->addCharacteristic(&SensorTXCharacteristic);
  SensorTXCharacteristic.setCallbacks(new CharacteristicCallbacks());
  SensorServiceTXDescriptor.setValue("mobile-to-esp");
  SensorTXCharacteristic.addDescriptor(&SensorServiceTXDescriptor);

  //Reads the messages coming from client
  sensorService->addCharacteristic(&SensorRXCharacteristic);
  SensorServiceRXDescriptor.setValue("esp-to-mobile-magnetic field-0-4096");
  SensorRXCharacteristic.addDescriptor(&SensorServiceRXDescriptor);
  SensorRXCharacteristic.addDescriptor(new BLE2902());

  //Sends 8 bit button code information to client
  sensorService->addCharacteristic(&ButtonCharacteristic);
  ButtonDescriptor.setValue("push button notifications");
  ButtonCharacteristic.addDescriptor(&ButtonDescriptor);
  ButtonCharacteristic.addDescriptor(new BLE2902());

  //Starts all services.
  deviceInfoService->start();
  batteryService->start();
  sensorService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(BATTERY_SERVICE_UUID);
  pAdvertising->addServiceUUID(DEVICE_INFO_UUID);

  pServer->getAdvertising()->start();
}

/**
   @brief Sends battery level percentage
   @param [in] level - battery level percentage (8 bit)
*/
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

/**
   @brief Sends battery level voltage
   @param [in] float voltage - battery voltage reading
*/
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

/**
   @brief Sets how often the battery level is sent
   @param unsigned long dt - debounce time in ms.
*/
void BluetoothService::setBatteryLevelDebounceTime(unsigned long dt) {
  BluetoothService::batteryDebounceDelay = dt;
}

/**
   @brief Update and stores device-on-time counter in every 30 seconds
*/
void BluetoothService::updateUsageCounter() {
  if (millis() - BluetoothService::counterTimerDebounceTime > BluetoothService::counterTimerDebounceDelay) {
    BluetoothService::counterTimerDebounceTime = millis();

    preferences.begin("store", false);
    Serial.println("BluetoothService::updateUsageCounter()");
    unsigned long counter = preferences.getULong("counter", 0);

    counter++;

    preferences.putULong("counter", counter);
    preferences.end();
    String counterStr = (String)(counter / 2.000);

    UsageCounterCharacteristic.setValue(counterStr.c_str());
  }
}

void BluetoothService::onDeviceConnectionChange(void (*onDeviceConnectionCallback)(String status)) {
  _onDeviceConnectionCallback = onDeviceConnectionCallback;
}

void BluetoothService::onMessageFromClient(void (*onMessageFromClientCallback)(String message)) {
  _onMessageFromClientCallback = onMessageFromClientCallback;
}

/**
   @brief Sends sensor readings.
   @param uint16_t sensorValue - sensor analog reading. (0 to 65535)
*/
void BluetoothService::sendSensorValue(uint16_t sensorValue) {
  SensorRXCharacteristic.setValue((uint16_t&)sensorValue);
  SensorRXCharacteristic.notify();
}

/**
   @brief Sends the button code when the buttons of the device are pressed.
   @param uint8_t buttonCode (0-255)
*/
void BluetoothService::sendButtonCode(uint8_t buttonCode) {
  ButtonCharacteristic.setValue(( uint8_t* )&buttonCode, 1);
  ButtonCharacteristic.notify();
}
