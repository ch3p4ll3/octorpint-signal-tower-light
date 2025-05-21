#include <Arduino.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <AsyncTCP.h>

#include <ESPAsyncWebServer.h>
#include <WiFi.h>

#include <config.cpp>
#include <wifi_task.h>
#include <lights.h>
#include <api.h>
#include <rest.h>
#include <mqtt.h>
#include <status.h>

Config config;
JsonDocument settings;
AsyncWebServer server(80);

void readSettings();

void setup()
{
  Serial.begin(115200);

  statusInit();

  readSettings();
  
  wifiTask(&config);

  Serial.println("Wi-Fi configured!");
  
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
    Serial.println("LittleFS mount failed");
    return;
  }

  File file = LittleFS.open("/config.json", "r");

  if (!file)
  {
    Serial.println("Failed to open config.json");
    return;
  }

  DeserializationError error = deserializeJson(settings, file);

  if (error)
  {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.f_str());
    return;
  }

  if (parseConfigFromJson(settings, config))
  {
    Serial.println("Config loaded successfully.");
  }
}