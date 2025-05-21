#pragma once

#include <Arduino.h>
#include <LittleFS.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <AsyncTCP.h>

void setupApi(AsyncWebServer &server, JsonDocument &config);
