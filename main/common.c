#include "common.h"

const char *TAG = "ASGM4";
volatile uint8_t count = 0;
xQueueHandle barrier_evt_q = NULL;
xQueueHandle count_display_q = NULL;
xQueueHandle count_publish_q = NULL;
volatile bool in_testing_scenario = false;

void init_logging()
{
    esp_log_level_set(TAG, ESP_LOG_INFO);
    ESP_LOGI(TAG, "Start");
}

void init_common_queues()
{
    if (barrier_evt_q == NULL)
    {
        barrier_evt_q = xQueueCreate(BARRIER_EVT_Q_SIZE, sizeof(barrier_evt_q_item));
    }
    if (count_display_q == NULL)
    {
        count_display_q = xQueueCreate(COUNT_DISPLAY_Q_SIZE, sizeof(count_display_q_item));
    }
    if (count_publish_q == NULL)
	{
    	count_publish_q = xQueueCreate(COUNT_DISPLAY_Q_SIZE, sizeof(count_display_q_item));
	}
}

void init_esp_dependencies()
{
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_ERROR_CHECK(esp_netif_init());
}
