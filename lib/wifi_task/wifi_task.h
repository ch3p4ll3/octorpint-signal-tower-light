#pragma once

#include <WiFi.h>
#include <DNSServer.h>

#include <config.h>

struct CandidateNetwork
{
    String ssid;
    String password;
    int rssi;
};

extern std::vector<CandidateNetwork> candidates;

extern DNSServer dnsServer;
extern TaskHandle_t wifiTaskHandle;
extern TaskHandle_t dnsTaskHandle;

void dnsTask(void *pvParameters);
void setupWiFi(Config *args);
