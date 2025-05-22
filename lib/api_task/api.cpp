#include "api.h"
#include <ArduinoLog.h>


void setupApi(AsyncWebServer &server, JsonDocument &config) {
  Log.info("Initializing AsyncWebServer\n");
  // Serve main UI
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *req){
    req->send(LittleFS, "/index.html", "text/html");
  });

  // API: Get settings
  server.on("/api/settings", HTTP_GET, [&config](AsyncWebServerRequest *req){
    String json;
    serializeJson(config, json);
    req->send(200, "application/json", json);
  });

  // API: Save config and restart
  server.on("/api/settings", HTTP_POST, [](AsyncWebServerRequest *req){}, nullptr,
    [&config](AsyncWebServerRequest *req, uint8_t *data, size_t len, size_t, size_t){
      Serial.write(data, len);  // Print raw received data
      JsonDocument newCfg;
      DeserializationError err = deserializeJson(newCfg, data, len);
      if (err) {
        req->send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
        return;
      }

      Log.info("Received JSON: ");
      serializeJsonPretty(newCfg, Serial);

      config = newCfg;
      File file = LittleFS.open("/config.json", "w");
      if (!file) {
        req->send(500, "application/json", "{\"error\":\"Failed to save config\"}");
        return;
      }
      serializeJson(config, file);
      file.close();

      req->send(200, "application/json", "{\"status\":\"saved\", \"restarting\":true}");
      delay(500);
      ESP.restart();
    });

  // Serve static assets
  server.serveStatic("/", LittleFS, "/");

  // Catch-all fallback for SPA routing
  server.onNotFound([](AsyncWebServerRequest *req){
    req->send(LittleFS, "/index.html", "text/html");
  });

  Log.info("Starting AsyncWebServer\n");
  server.begin();
}
