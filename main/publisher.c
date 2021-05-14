#include "common.h"
#include "time_management.h"


#define IOT_PLATFORM_GATEWAY_IP CONFIG_IOT_PLATFORM_GATEWAY_IP
#define IOT_PLATFORM_GATEWAY_PORT CONFIG_IOT_PLATFORM_GATEWAY_PORT
#define IOT_PLATFORM_GATEWAY_USERNAME CONFIG_IOT_PLATFORM_GATEWAY_USERNAME
#define IOT_PLATFORM_GATEWAY_PASSWORD CONFIG_IOT_PLATFORM_GATEWAY_PASSWORD

#define IOT_PLATFORM_GROUP CONFIG_IOT_PLATFORM_GROUP

#define IOT_PLATFORM_COUNT_USER_ID CONFIG_IOT_PLATFORM_COUNT_USER_ID
#define IOT_PLATFORM_COUNT_DEVICE_ID CONFIG_IOT_PLATFORM_COUNT_DEVICE_ID
#define IOT_PLATFORM_COUNT_SENSOR_NAME CONFIG_IOT_PLATFORM_COUNT_SENSOR_NAME

#define MQTT_BROKER_URI_PATTERN "mqtt://%s"
#define MQTT_BROKER_URI_BUFFER_SIZE 30
static char MQTT_BROKER_URI_BUFFER[MQTT_BROKER_URI_BUFFER_SIZE];

#define MQTT_COUNT_MESSAGE_PATTERN "{\"username\":\"%s\",\"%s\":%d,\"device_id\":%d,\"timestamp\":%llu}"
#define MQTT_COUNT_MESSAGE_BUFFER_SIZE 100
static char MQTT_COUNT_MESSAGE_BUFFER[MQTT_COUNT_MESSAGE_BUFFER_SIZE];


#define MQTT_TOPIC_PATTERN "%d_%d"
#define MQTT_TOPIC_BUFFER_SIZE 10
static char MQTT_TOPIC_BUFFER[MQTT_TOPIC_BUFFER_SIZE];



#define TIMER_INTERVAL 60000000

static esp_mqtt_client_handle_t count_mqtt_client;


static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event)
{

    switch (event->event_id)
    {
    case MQTT_EVENT_CONNECTED:
		ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
		break;
	case MQTT_EVENT_DISCONNECTED:
		ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
		break;
	case MQTT_EVENT_SUBSCRIBED:
		ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
		break;
	case MQTT_EVENT_UNSUBSCRIBED:
		ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
		break;
	case MQTT_EVENT_PUBLISHED:
		ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
		break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }

    return ESP_OK;
}

void publish_count()
{
	unsigned long long current_epoch_time = read_epoch_time_in_msec();
	snprintf(MQTT_COUNT_MESSAGE_BUFFER, MQTT_COUNT_MESSAGE_BUFFER_SIZE, MQTT_COUNT_MESSAGE_PATTERN,
			IOT_PLATFORM_GROUP, IOT_PLATFORM_COUNT_SENSOR_NAME, count, IOT_PLATFORM_COUNT_DEVICE_ID, current_epoch_time);
	ESP_LOGI(TAG, "Publish message:%s", MQTT_COUNT_MESSAGE_BUFFER);
    esp_mqtt_client_publish(count_mqtt_client, MQTT_TOPIC_BUFFER, MQTT_COUNT_MESSAGE_BUFFER, 0, 1, 0);

}

static void periodic_timer_callback(void* arg)
{
    int64_t time_since_boot = esp_timer_get_time();
    ESP_LOGI(TAG, "Periodic timer called, time since boot: %lld us", time_since_boot);
    publish_count();
}

void setup_publisher()
{
	// Create URI
	snprintf(MQTT_BROKER_URI_BUFFER, MQTT_BROKER_URI_BUFFER_SIZE, MQTT_BROKER_URI_PATTERN, IOT_PLATFORM_GATEWAY_IP);

	// Create topic
	snprintf(MQTT_TOPIC_BUFFER, MQTT_TOPIC_BUFFER_SIZE, MQTT_TOPIC_PATTERN,
				IOT_PLATFORM_COUNT_USER_ID, IOT_PLATFORM_COUNT_DEVICE_ID);

	ESP_LOGI(TAG, "Uri: %s", MQTT_BROKER_URI_BUFFER);
	ESP_LOGI(TAG, "Topic: %s", MQTT_TOPIC_BUFFER);

	const esp_mqtt_client_config_t mqtt_cfg = {
			.uri = MQTT_BROKER_URI_BUFFER,
			.port = IOT_PLATFORM_GATEWAY_PORT,
			.username = IOT_PLATFORM_GATEWAY_USERNAME,
			.password = IOT_PLATFORM_GATEWAY_PASSWORD,
			.event_handle = mqtt_event_handler
		};

	count_mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
	esp_mqtt_client_start(count_mqtt_client);


	// Create timer
	const esp_timer_create_args_t periodic_timer_args = {
	            .callback = &periodic_timer_callback,
	            /* name is optional, but may help identify the timer when debugging */
	            .name = "periodic"
	    };
	esp_timer_handle_t periodic_timer;
	ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
	ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, 6000000));
}
