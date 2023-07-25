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
#include <esp_ver.h>

#include <esp_adc_cal.h>

#if defined(CONFIG_IDF_TARGET_ESP32)
;;
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
#include <FastLED.h>
// #include "esp32_digital_led_lib.h"
// #include "esp32_digital_led_funcs.h"
#endif

#ifdef __INC_FASTSPI_LED2_H
#define NUM_LEDS 1

// #define DATA_PIN 10 // GPIO38
#define DATA_PIN GPIO_NUM_48

typedef enum {
  RED = 0,
  GREEN = 1,
  BLUE = 2,
  NO_LED = -1
} led_colors_t;

CRGB leds[NUM_LEDS];

led_colors_t led_counter = RED;
#elif defined(ESP32_DIGITAL_LED_LIB_H)
#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"  // It's noisy here with `-Wall`

//strand_t strand = {.rmtChannel = 0, .gpioNum = 26, .ledType = LED_WS2812B_V3, .brightLimit = 32, .numPixels = 64};
strand_t strand = {.rmtChannel = 0, .gpioNum = 27, .ledType = LED_SK6812W_V1, .brightLimit = 64, .numPixels = 144};
strand_t * STRANDS [] = { &strand };
int STRANDCNT = COUNT_OF(STRANDS); 
#pragma GCC diagnostic pop
#endif

Wifimanager_wrapper wifiman;

// EOTAUpdate updater(UPDATE_URL, VERSION_NUMBER);
// EOTAUpdate updater(UPDATE_URL, VERSION_NUMBER, UPDATE_INTERVAL);
EOTAUpdate updater(UPDATE_URL, _esp_return_current_fw_version(), UPDATE_INTERVAL);

uint32_t update_checker_timer = millis();
uint32_t ADC_test_timer = millis();

uint16_t adc_test_mV = 0;
uint16_t adc_test_mV_old = 0;
uint16_t adc_Beed_IR_mV = 0;
uint16_t adc_Beed_IR_mV_old = 0;
uint16_t adc_SHARP_IR_mV = 0;
uint16_t adc_SHARP_IR_mV_old = 0;

#define slow_filter_k 8 // 561 samples
#define fast_filter_k 4 // 34 samples

uint32_t filter_reg = 0;
uint16_t filter_out = 0;

uint32_t current_filter_reg = 0;
uint16_t current_filter_out = 0;

const uint32_t DUTY_CYCLE_MAX = 16383; // 2^14-1
uint32_t duty_cycle = 0;

// use first channel of 16 channels (started from zero)
#define LEDC_CHANNEL_0     0

// use 14 bit precission for LEDC timer
#define LEDC_TIMER_14_BIT  14

// use 5000 Hz as a LEDC base frequency
#define LEDC_BASE_FREQ     1000

// fade LED PIN (replace with LED_BUILTIN constant for built-in LED)
#define LED_PIN            GPIO_NUM_7

uint8_t hash[32] = {0};
String fw_ver;

void setup()
{
  // put your setup code here, to run once:
  // initialize the LED digital pin as an output.
  pinMode(PIN_LED, OUTPUT);
  // pinMode(TRIGGER_PIN, INPUT_PULLUP);
  // pinMode(TRIGGER_PIN2, INPUT_PULLUP);

  pinMode(GPIO_NUM_4, INPUT);
  analogSetAttenuation(ADC_11db);
  adcAttachPin(GPIO_NUM_4);
  pinMode(GPIO_NUM_5, INPUT);
  analogSetAttenuation(ADC_11db);
  adcAttachPin(GPIO_NUM_5);
  pinMode(GPIO_NUM_6, INPUT);
  analogSetAttenuation(ADC_11db);
  adcAttachPin(GPIO_NUM_6);

  Serial.begin(115200);
  while (!Serial);


  Serial.printf("\nesp_adc_cal_check_efuse(): %u\r\n", esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP_FIT));
  
  esp_adc_cal_characteristics_t chars;
  Serial.printf("\nesp_adc_cal_characterize(): %u\r\n", esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_11db, ADC_WIDTH_BIT_12, ESP_ADC_CAL_VAL_EFUSE_TP_FIT, &chars));

  Serial.println(chars.coeff_a);
  Serial.println(chars.coeff_b);
  // // Setup timer and attach timer to a led pin
  // uint32_t led_success = ledcSetup(LEDC_CHANNEL_0, LEDC_BASE_FREQ, LEDC_TIMER_14_BIT);
  
  // if (led_success != LEDC_BASE_FREQ)
  // {
  //   Serial.printf("ledc returned %u instead of %u\n", led_success, LEDC_BASE_FREQ);
  //   while (1);
  // }

  // ledcAttachPin(LED_PIN, LEDC_CHANNEL_0);

#ifdef __INC_FASTSPI_LED2_H
  FastLED.addLeds<SK6812, DATA_PIN, GRB>(leds, NUM_LEDS);  // GRB ordering is typical
  // FastLED.addLeds<SK6822, DATA_PIN, RGB>(leds, NUM_LEDS);
#elif defined(ESP32_DIGITAL_LED_LIB_H)
  digitalLeds_initDriver();

  gpioSetup(strand.gpioNum, OUTPUT, LOW);
  int rc = digitalLeds_addStrands(STRANDS, STRANDCNT);
  if (rc) {
    Serial.print("Init rc = ");
    Serial.println(rc);
  }

  if (digitalLeds_initDriver()) {
    Serial.println("Init FAILURE: halting");
    while (true) {};
  }
  digitalLeds_resetPixels(STRANDS, STRANDCNT);
#endif

  // Serial.println(MYSTRING);
  Serial.println(CDN_URL);

  fw_ver = _esp_return_current_fw_version();
  Serial.printf("flashed FW version: %s\r\n", fw_ver);

  updater.print_versions();

  delay(200);

  // wifiman.wifimanager_init_prints();

  // wifiman.wifimanager_config_and_initialize();
}

void loop()
{
  if (millis() - update_checker_timer > 500)
  {
    // eota_reponses_t response;
    // response = updater.Check();
    // // Serial.printf("Checking for updates. URL: %s\r\n", UPDATE_URL);
    // // Serial.printf("Current version: %s\r\n", fw_ver);
    // // Serial.printf("recieved response from library: %s (%u)\r\n", eota_responses_strings[response], response);
    // if (response == eota_ok)
    // {
    //   Serial.printf("Found update, trying to perform OTA\n");
    //   updater.CheckAndUpdate(true);
    // }
    update_checker_timer = millis();

#ifdef __INC_FASTSPI_LED2_H
    switch (led_counter)
    {
    case RED:
      leds[0] = CRGB::Red;
      led_counter = GREEN;
      break;
    case GREEN:
      leds[0] = CRGB::Green;
      led_counter = BLUE;
      break;
    case BLUE:
      leds[0] = CRGB::Blue;
      led_counter = NO_LED;
      break;
    case NO_LED:
      leds[0] = CRGB::Black;
      led_counter = RED;
      break;
    
    default:
      leds[0] = CRGB::Black;
      break;
    }
    FastLED.show();
#elif defined(ESP32_DIGITAL_LED_LIB_H)
  //  randomStrands(STRANDS, STRANDCNT, 200, 10000);
  rainbows(STRANDS, STRANDCNT, 1, 0);
  //simpleStepper(STRANDS, STRANDCNT, 0, 0);
#endif
  }

  adc_test_mV = analogReadMilliVolts(GPIO_NUM_4);
  adc_SHARP_IR_mV = analogReadMilliVolts(GPIO_NUM_5);
  adc_Beed_IR_mV = analogReadMilliVolts(GPIO_NUM_6);

  filter_reg = filter_reg - (filter_reg >> slow_filter_k) + adc_Beed_IR_mV;
	filter_out = filter_reg >> slow_filter_k;

  if (millis() - ADC_test_timer > 500)
  {
    // if (adc_test_mV != adc_test_mV_old)
    // {
    //   adc_test_mV_old = adc_test_mV;
    //   Serial.printf("pot: %u mV\r\n", adc_test_mV);
    // }
    if (adc_SHARP_IR_mV != adc_SHARP_IR_mV_old)
    {
      adc_SHARP_IR_mV_old = adc_SHARP_IR_mV;
      Serial.printf("SHARP IR: %u mV\r\n", adc_SHARP_IR_mV);
    }
    if (filter_out != adc_Beed_IR_mV_old)
    {
      adc_Beed_IR_mV_old = filter_out;
      Serial.printf("Beed IR: %u mV\r\n", filter_out);
    }
  }


  if (millis() - ADC_test_timer > 500)
  {
  //   if (duty_cycle < DUTY_CYCLE_MAX)
  //     duty_cycle += 100;
  //   else
  //     duty_cycle = 0;
    
  //   // write duty to LEDC
  //   ledcWrite(LEDC_CHANNEL_0, duty_cycle);

    ADC_test_timer = millis();
  }

  // // is configuration portal requested?
  // if ((digitalRead(TRIGGER_PIN) == LOW) || (digitalRead(TRIGGER_PIN2) == LOW))
  // {
  //   Serial.println(F("\nConfiguration portal requested."));
  //   wifiman.wifimanager_start_portal();
  // }

  // // put your main code here, to run repeatedly
  // wifiman.check_status();

}