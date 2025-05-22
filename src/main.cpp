#include <Arduino.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <ArduinoLog.h>
#include <AsyncTCP.h>

#include <ESPAsyncWebServer.h>
#include <WiFi.h>

#include <config.cpp>
#include <wifi_task.h>
#include <lights_task.h>
#include <api_task.h>
#include <octoprint_API_task.h>
#include <mqtt_task.h>
#include <status.h>

Config config;
JsonDocument settings;
AsyncWebServer server(80);

void readSettings();

void setup()
{
  Serial.begin(115200);
  Log.begin(LOG_LEVEL_VERBOSE, &Serial);

  statusInit();

  readSettings();

  setupWiFi(&config);

  setupApi(server, settings);

  xTaskCreatePinnedToCore(lightTask, "lightTask", 4096, &config, 1, &lightsTaskHandle, 1);

  if (config.useMqtt)
  {
    xTaskCreatePinnedToCore(mqttTask, "mqttTask", 4096, &config, 1, &mqttTaskHandle, 1);
  }
  else
  {
    xTaskCreatePinnedToCore(octoPrintAPITask, "octoPrintAPITask", 4096, &config, 1, &octoPrintAPITaskHandle, 1);
  }
}

void loop()
{
}

void readSettings()
{
  if (!LittleFS.begin())
  {
    Log.error("LittleFS mount failed!\n");
    return;
  }

  File file = LittleFS.open("/config.json", "r");

  if (!file)
  {
    Log.error("Failed to open config.json!\n");
    return;
  }

  DeserializationError error = deserializeJson(settings, file);

  if (error)
  {
    Log.error(F("deserializeJson() failed: %s\n"), error.c_str());
    return;
  }

  if (parseConfigFromJson(settings, config))
  {
    Log.info("Config loaded successfully.\n");
  }
}