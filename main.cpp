#include "BluetoothService.h"

BluetoothService BS(
  "CompanyName DeviceName",
  "CompanyName",
  "DeviceName",
  "DeviceCode",
  "USA",
  "1.0.0",
  "1.0.0"
);

void setup() {
  Serial.begin(115200);
  //Allows the installation of the Ble server.
  BS.setup();
  //Sets how often the battery level is sent(ms). Low values can cause unnecessary resource consumption.
  BS.setBatteryLevelDebounceTime(5000);

}

void loop() {
  delay(1000);
  BS.notifyBatteryLevel(random(0, 200)); // 101 if out of range.
  BS.notifyBatteryVoltage((float)(random(0, 20)/2.0)); // 5.1 if out of range.
  BS.updateUsageCounter(); //To update the device usage time.
}
