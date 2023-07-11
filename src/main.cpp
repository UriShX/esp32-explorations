/****************************************************************************************************************************
  ConfigOnSwitch.ino
  For ESP8266 / ESP32 boards
  
  ESP_WiFiManager is a library for the ESP8266/ESP32 platform (https://github.com/esp8266/Arduino) to enable easy
  configuration and reconfiguration of WiFi credentials using a Captive Portal. Inspired by:
  http://www.esp8266.com/viewtopic.php?f=29&t=2520
  https://github.com/chriscook8/esp-arduino-apboot
  https://github.com/esp8266/Arduino/blob/master/libraries/DNSServer/examples/CaptivePortalAdvanced/
  
  Modified from Tzapu https://github.com/tzapu/WiFiManager
  and from Ken Taylor https://github.com/kentaylor
  
  Built by Khoi Hoang https://github.com/khoih-prog/ESP_WiFiManager
  Licensed under MIT license
 *****************************************************************************************************************************/
/****************************************************************************************************************************
   This example will open a configuration portal when no WiFi configuration has been previously entered or when a button is pushed.
   It is the easiest scenario for configuration but requires a pin and a button on the ESP8266 device.
   The Flash button is convenient for this on NodeMCU devices.

   Also in this example a password is required to connect to the configuration portal
   network. This is inconvenient but means that only those who know the password or those
   already connected to the target WiFi network can access the configuration portal and
   the WiFi network credentials will be sent from the browser over an encrypted connection and
   can not be read by observers.
 *****************************************************************************************************************************/
#include <Arduino.h>
#include <pins_config.h>
#include <ota_config.h>
#include <wifimanager_handler.h>


#include <EOTAUpdate.h>

// EOTAUpdate updater(UPDATE_URL, VERSION_NUMBER);
EOTAUpdate updater(UPDATE_URL, VERSION_NUMBER, UPDATE_INTERVAL);

uint32_t update_checker_timer = millis();

void setup()
{
  // put your setup code here, to run once:
  // initialize the LED digital pin as an output.
  pinMode(PIN_LED, OUTPUT);
  pinMode(TRIGGER_PIN, INPUT_PULLUP);
  pinMode(TRIGGER_PIN2, INPUT_PULLUP);

  Serial.begin(115200);
  while (!Serial);

  delay(200);

  wifimanager_init_prints();

  wifimanager_config_and_initialize();
}

void loop()
{
  if (millis() - update_checker_timer > 500)
  {
    Serial.printf("Checking for updates. URL: %s\r\n", UPDATE_URL);
    Serial.printf("Current version: %u\r\n", VERSION_NUMBER);
    updater.CheckAndUpdate();
    update_checker_timer = millis();
  }
  // is configuration portal requested?
  if ((digitalRead(TRIGGER_PIN) == LOW) || (digitalRead(TRIGGER_PIN2) == LOW))
  {
    Serial.println(F("\nConfiguration portal requested."));
    wifimanager_start_portal();
  }

  // put your main code here, to run repeatedly
  check_status();

}