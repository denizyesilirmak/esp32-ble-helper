#include "BluetoothService.h"

BluetoothService BS(
  "Brand DeviceName", //Brand DeviceName
  "CompanyName", //Company Name
  "DeviceName", //Device Name
  "DeviceCode", //Device Code
  "USA", //Region Code
  "1.0.0", //Hardware Revision
  "1.0.0" //Firmware Revision
);

void setup() {
  Serial.begin(115200);
  //Allows the installation of the Ble server.
  BS.setup();

  //Sets how often the battery level is sent(ms). Low values can cause unnecessary resource consumption.
  BS.setBatteryLevelDebounceTime(5000);

  //Callback function that returns the client's connection status.
  BS.onDeviceConnectionChange(handleConnectionEvent);

  //Callback function that returns messages from the client.
  BS.onMessageFromClient(handleMessageEvent);
}

void handleConnectionEvent(String status)
{
  Serial.println("Client conneciton status: " + status);
}

void handleMessageEvent(String message)
{
  Serial.println("Message from client: " + message);
}

void loop() {
  delay(1000);
  BS.notifyBatteryLevel(random(0, 200)); // 101 if out of range.
  BS.notifyBatteryVoltage((float)(random(0, 20) / 2.0)); // 5.1 if out of range.
  BS.updateUsageCounter(); //To update the device usage time.

  uint16_t dummySensorReading = random(0, 4095);
  BS.sendSensorValue(dummySensorReading);

  uint8_t dummyButtonCode = random(0, 255);
  BS.sendButtonCode(dummyButtonCode);
}
