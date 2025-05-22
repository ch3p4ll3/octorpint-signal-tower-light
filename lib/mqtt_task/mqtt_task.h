#pragma once

#include <PubSubClient.h>
#include <WiFi.h>

extern WiFiClient wifiClient;
extern PubSubClient mqttClient;
extern TaskHandle_t mqttTaskHandle;

void mqttTask(void *pvParameters);
void mqtt_callback(char *topic, byte *payload, unsigned int length);