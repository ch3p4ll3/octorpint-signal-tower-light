#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <config.h>
#include <status.h>

// Polling interval in milliseconds
const uint32_t pollIntervalMs = 5000;

extern TaskHandle_t octoPrintAPITaskHandle;

void octoPrintAPITask(void *pvParameters);
