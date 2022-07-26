#ifndef BLUETOOTHSERVICE_H
#define BLUETOOTHSERVICE_H
#include "Arduino.h"
#include <Preferences.h>

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>



class BluetoothService {
    String deviceName;
    String manufacturerName;
    String modelName;
    String ModelCode;
    String CountryCode;
    String HardwareRevision;
    String FirmwareRevision;

  public:
    BluetoothService(
      String deviceName,
      String manufacturerName,
      String modelName,
      String ModelCode,
      String CountryCode,
      String HardwareRevision,
      String FirmwareRevision
    );
    void init(
      String deviceName,
      String manufacturerName,
      String modelName,
      String ModelCode,
      String CountryCode,
      String HardwareRevision,
      String FirmwareRevision
    );
    void setup();
    void notifyBatteryLevel(uint8_t level);
    void notifyBatteryVoltage(float voltage);
    void updateUsageCounter();
    void setBatteryLevelDebounceTime(unsigned long dt);




  private:
    String batteryLevelRangeError = "101";
    unsigned long batteryDebounceDelay = 5000;
    unsigned long batteryDebounceTime = 0;
    
    unsigned long batteryVoltageDebounceDelay = 5000;
    unsigned long batteryVoltageDebounceTime = 0;
    
    unsigned long counterTimerDebounceDelay = 30000;
    unsigned long counterTimerDebounceTime = 0;


};

#endif
