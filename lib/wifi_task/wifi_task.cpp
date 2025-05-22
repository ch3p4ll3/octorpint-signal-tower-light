#include <WiFi.h>
#include <DNSServer.h>
#include <ArduinoLog.h>
#include <config.h>

#include "wifi_task.h"

DNSServer dnsServer;
TaskHandle_t wifiTaskHandle = NULL;
TaskHandle_t dnsTaskHandle = NULL;

void dnsTask(void *pvParameters)
{
  Log.info("Starting DNS task\n");
  while (true)
  {
    dnsServer.processNextRequest();
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void setupWiFi(Config *args)
{
  Log.info("Starting WiFi connection task");

  WiFi.mode(WIFI_STA);
  WiFi.begin(args->wifi.ssid, args->wifi.password);

  Log.info(F("Trying to connect to \"%s\"\n"), args->wifi.ssid);

  unsigned long startAttemptTime = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 20000)
  {
    delay(500);
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Log.info(F("WiFi connected!\nIP Address: %s\n"), WiFi.localIP().toString().c_str());
  }
  else
  {
    Log.warning("nFailed to connect, starting AP mode\n");

    WiFi.mode(WIFI_AP);
    WiFi.softAP(args->ap_wifi.ssid, args->ap_wifi.password);

    IPAddress apIP = WiFi.softAPIP();

    Log.info(F("AP created!\nIP Address: %s\n"), apIP.toString().c_str());

    // Start DNS server to capture all domains and redirect to AP IP
    Log.info("Starting DNS server\n");
    dnsServer.start(53, "*", apIP);

    // Start DNS task to run concurrently
    xTaskCreatePinnedToCore(dnsTask, "dnsTask", 2048, NULL, 1, &dnsTaskHandle, 0);
  }
}
