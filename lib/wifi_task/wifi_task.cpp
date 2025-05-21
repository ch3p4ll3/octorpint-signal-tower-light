#include <WiFi.h>
#include <DNSServer.h>
#include <config.h>

#include "wifi_task.h"

DNSServer dnsServer;
TaskHandle_t wifiTaskHandle = NULL;
TaskHandle_t dnsTaskHandle = NULL;

void dnsTask(void *pvParameters)
{
  while (true)
  {
    dnsServer.processNextRequest();
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void wifiTask(Config *args)
{
  Serial.println("Starting WiFi connection task...");

  WiFi.mode(WIFI_STA);
  WiFi.begin(args->wifi.ssid, args->wifi.password);

  unsigned long startAttemptTime = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000)
  {
    Serial.print(".");
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("\nWiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  else
  {
    Serial.println("\nFailed to connect, starting AP mode...");

    WiFi.mode(WIFI_AP);
    WiFi.softAP(args->ap_wifi.ssid, args->ap_wifi.password);

    IPAddress apIP = WiFi.softAPIP();

    Serial.print("AP IP address: ");
    Serial.println(apIP);

    // Start DNS server to capture all domains and redirect to AP IP
    dnsServer.start(53, "*", apIP);

    // Start DNS task to run concurrently
    xTaskCreatePinnedToCore(dnsTask, "dnsTask", 2048, NULL, 1, &dnsTaskHandle, 0);
  }
}
