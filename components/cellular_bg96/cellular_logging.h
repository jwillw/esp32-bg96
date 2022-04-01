
#ifndef CELLULAR_LOGGING_H
#define CELLULAR_LOGGING_H

#include "esp_log.h"

#define CELLULAR_TAG "CELLULAR"

#define CELL_ERROR(...)  ESP_LOGE(CELLULAR_TAG, __VA_ARGS__)
#define CELL_WARN(...)   ESP_LOGW(CELLULAR_TAG, __VA_ARGS__)
#define CELL_INFO(...)   ESP_LOGI(CELLULAR_TAG, __VA_ARGS__)
#define CELL_DEBUG(...)  ESP_LOGD(CELLULAR_TAG, __VA_ARGS__)

#define LogError(X)  CELL_ERROR X
#define LogWarn(X)   CELL_WARN  X
#define LogInfo(X)   CELL_INFO  X
#define LogDebug(X)  CELL_DEBUG X

#endif  /* CELLULAR_LOGGING_H */
