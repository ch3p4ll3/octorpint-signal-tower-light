#include "status.h"

SemaphoreHandle_t statusMutex;
String jobStatus;

void statusInit() {
  statusMutex = xSemaphoreCreateMutex();
  jobStatus = ""; // initialize status
}

String getJobStatus() {
  String result;
  if (xSemaphoreTake(statusMutex, portMAX_DELAY)) {
    result = jobStatus;
    xSemaphoreGive(statusMutex);
  }
  return result;
}

void setJobStatus(const String &status) {
  if (xSemaphoreTake(statusMutex, portMAX_DELAY)) {
    jobStatus = status;
    xSemaphoreGive(statusMutex);
  }
}
