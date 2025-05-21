#pragma once

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

extern SemaphoreHandle_t statusMutex;
extern String jobStatus;

void statusInit();
String getJobStatus();
void setJobStatus(const String &status);