#include "wifi_task.h"

#include <ArduinoLog.h>
#include <DNSServer.h>
#include <WiFi.h>


std::vector<CandidateNetwork> candidates;
DNSServer dnsServer;
TaskHandle_t wifiTaskHandle = NULL;
TaskHandle_t dnsTaskHandle = NULL;

void dnsTask(void *pvParameters) {
  Log.info("Starting DNS task\n");
  while (true) {
    dnsServer.processNextRequest();
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void setupWiFi(Config *args) {
  Log.info("Starting WiFi connection task\n");

  WiFi.mode(WIFI_STA);
  WiFi.disconnect(); // Clear old settings
  delay(100);

  Log.info("Scanning for WiFi networks...\n");
  int numNetworks = WiFi.scanNetworks();
  if (numNetworks == 0) {
    Log.warning("No WiFi networks found.\n");
  } else {
    Log.info("Scan complete. %d networks found.\n", numNetworks);
  }

  for (int i = 0; i < numNetworks; ++i) {
    String scannedSSID = WiFi.SSID(i);
    int rssi = WiFi.RSSI(i);

    // Match scanned SSID with known list
    for (auto &known : args->wifi) {
      if (scannedSSID == known.ssid) {
        candidates.push_back({known.ssid, known.password, rssi});
        Log.info("Matched known network: %s (%d dBm)\n", known.ssid.c_str(),
                 rssi);
        break;
      }
    }
  }

  if (candidates.empty()) {
    Log.warning("No known networks found in range.\n");
  } else {
    // Sort by strongest signal
    std::sort(candidates.begin(), candidates.end(),
              [](const CandidateNetwork &a, const CandidateNetwork &b) {
                return a.rssi > b.rssi;
              });

    // Try to connect to each
    for (auto &net : candidates) {
      Log.info("Trying to connect to \"%s\"\n", net.ssid.c_str());
      WiFi.begin(net.ssid.c_str(), net.password.c_str());

      unsigned long startAttemptTime = millis();
      while (WiFi.status() != WL_CONNECTED &&
             millis() - startAttemptTime < 20000) {
        delay(500);
      }

      if (WiFi.status() == WL_CONNECTED) {
        Log.info("Connected to %s!\nIP Address: %s\n", net.ssid.c_str(),
                 WiFi.localIP().toString().c_str());
        return;
      } else {
        Log.warning("Failed to connect to \"%s\"\n", net.ssid.c_str());
      }
    }
  }

  // Fallback to Access Point
  Log.warning("Could not connect to any known network. Starting AP mode.\n");

  WiFi.mode(WIFI_AP);
  WiFi.softAP(args->ap_wifi.ssid, args->ap_wifi.password);

  IPAddress apIP = WiFi.softAPIP();
  Log.info("AP created!\nIP Address: %s\n", apIP.toString().c_str());

  dnsServer.start(53, "*", apIP);
  xTaskCreatePinnedToCore(dnsTask, "dnsTask", 2048, NULL, 1, &dnsTaskHandle, 0);
}
