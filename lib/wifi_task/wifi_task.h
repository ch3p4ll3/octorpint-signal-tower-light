#pragma once

#include <WiFi.h>
#include <DNSServer.h>

extern DNSServer dnsServer;
extern TaskHandle_t wifiTaskHandle;
extern TaskHandle_t dnsTaskHandle;

void dnsTask(void* pvParameters);
void wifiTask(Config *args);
