#include <stdint.h>
#include <stdio.h>

#include "esp_bt.h"
#include "esp_check.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_random.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "host/ble_hs.h"
#include "memory.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "nvs_flash.h"
#include "services/gap/ble_svc_gap.h"

static const char* TAG = "AxonOnOnOn";

#define ADV_DURATION_MS 500
#define PAUSE_DURATION_MS 500

#define ENABLE_FUZZING 0

static const uint8_t kAxonBlePrefix[] = {0x00, 0x25, 0xdf};
static const uint16_t kAxonServiceUuid = 0xFE6C;
static const uint8_t kAxonBaseServiceData[] = {
    0x01, 0x58, 0x38, 0x37, 0x30, 0x30, 0x32, 0x46, 0x50, 0x34, 0x01, 0x02,
    0x00, 0x00, 0x00, 0x00, 0xCE, 0x1B, 0x33, 0x00, 0x00, 0x02, 0x00, 0x00};

esp_err_t ble_init(void);
esp_err_t ble_start_axonononon();
esp_err_t ble_stop_axonononon(void);

static uint16_t fuzz_value = 0xd34d;

void app_main(void) {
  // Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  ESP_ERROR_CHECK(ble_init());

  while (1) {
    ble_start_axonononon(kAxonBlePrefix);
    vTaskDelay(pdMS_TO_TICKS(ADV_DURATION_MS));
    ble_stop_axonononon();
    vTaskDelay(pdMS_TO_TICKS(PAUSE_DURATION_MS));
  }
}

// Infinite task to run the NimBLE stack
static void host_task(void* param) {
  ESP_LOGI(TAG, "BLE Host Task Started");
  nimble_port_run();  // This function will return only when nimble_port_stop()
                      // is called
  nimble_port_freertos_deinit();
}

esp_err_t ble_init(void) {
  ESP_RETURN_ON_ERROR(nimble_port_init(), TAG, "Failed to initialize NimBLE");
  nimble_port_freertos_init(host_task);
  esp_log_level_set("NimBLE", ESP_LOG_ERROR);

  ESP_RETURN_ON_ERROR(
      esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, ESP_PWR_LVL_P20), TAG,
      "Failed to set BLE TX power");
  return ESP_OK;
}

esp_err_t ble_set_mac_prefix(const uint8_t prefix[]) {
  ble_addr_t custom_mac;
  ble_hs_id_gen_rnd(0, &custom_mac);
  custom_mac.val[5] = prefix[0];
  custom_mac.val[4] = prefix[1];
  custom_mac.val[3] = prefix[2];

  ESP_LOGI(TAG, "Setting custom ble MAC address: %02x:%02x:%02x:%02x:%02x:%02x",
           custom_mac.val[5], custom_mac.val[4], custom_mac.val[3],
           custom_mac.val[2], custom_mac.val[1], custom_mac.val[0]);
  ESP_RETURN_ON_ERROR(ble_hs_id_set_rnd(custom_mac.val), TAG,
                      "Failed to set custom MAC address");

  return ESP_OK;
}

esp_err_t ble_start_axonononon() {
  ESP_LOGI(TAG, "Starting AxonOnOnOn advertising");
  struct ble_gap_adv_params adv_params;

  ble_set_mac_prefix(&kAxonBlePrefix[0]);

  uint8_t raw_adv_data[29];
  size_t offset = 0;
  size_t data_len =
      sizeof(kAxonBaseServiceData) + 3;  // type(1) + uuid(2) + service data
  if (data_len > sizeof(raw_adv_data)) {
    ESP_LOGE(TAG, "Service data too large for advertising packet");
    return ESP_ERR_INVALID_SIZE;
  }

  raw_adv_data[offset++] = data_len;
  raw_adv_data[offset++] = 0x16;
  raw_adv_data[offset++] = (kAxonServiceUuid & 0xFF);
  raw_adv_data[offset++] = ((kAxonServiceUuid >> 8) & 0xFF);
  memcpy(&raw_adv_data[offset], kAxonBaseServiceData,
         sizeof(kAxonBaseServiceData));
#if ENABLE_FUZZING
  // Fuzz strategy: Modify bytes at different positions based on fuzz value
  // Position 10-11 seem to be command bytes (0x01 0x02 in original)
  // Position 20-21 also has values (0x00 0x02 in original)

  // Increment byte at position 10 (command byte 1)
  raw_adv_data[10 + offset] = ((fuzz_value >> 8) & 0xFF);
  raw_adv_data[11 + offset] = (fuzz_value & 0xFF);

  // Also vary the last few bytes
  raw_adv_data[20 + offset] = ((fuzz_value >> 4) & 0xFF);
  raw_adv_data[21 + offset] = ((fuzz_value << 4) & 0xFF);

  fuzz_value = (fuzz_value + 1) & 0xFFFF;
#endif
  offset += sizeof(kAxonBaseServiceData);

  ESP_RETURN_ON_ERROR(ble_gap_adv_set_data(raw_adv_data, offset), TAG,
                      "Failed to set adv data");

  memset(&adv_params, 0, sizeof(adv_params));

  adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
  adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;

  // Start Advertising
  ESP_RETURN_ON_ERROR(ble_gap_adv_start(BLE_ADDR_RANDOM, NULL, BLE_HS_FOREVER,
                                        &adv_params, NULL, NULL),
                      TAG, "Failed to start advertising");

  return ESP_OK;
}

esp_err_t ble_stop_axonononon(void) {
  ESP_LOGI(TAG, "Stopping AxonOnOnOn advertising");
  ESP_RETURN_ON_ERROR(ble_gap_adv_stop(), TAG, "Failed to stop advertising");

  return ESP_OK;
}
