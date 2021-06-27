#include "common.h"
#include "subscriber.h"
#include "commands.h"
#include "mqtt_message.h"

#define MQTT_BROKER_HOST CONFIG_EDGE_FORECAST_BROKER_HOST
#define MQTT_BROKER_PORT CONFIG_EDGE_FORECAST_BROKER_PORT

#define MQTT_SUB_TOPIC CONFIG_EDGE_FORECAST_BROKER_TOPIC

#define MQTT_BROKER_URI_BUFFER "mqtt://" MQTT_BROKER_HOST

#define FORECAST_MESSAGE_PREFIX "FORECAST:"
#define FORECAST_MESSAGE_PREFIX_LENGTH sizeof(FORECAST_MESSAGE_PREFIX)
#define FORECAST_MESSAGE_MIN_LENGTH FORECAST_MESSAGE_PREFIX_LENGTH + 1
#define FORECAST_MESSAGE_MAX_LENGTH FORECAST_MESSAGE_PREFIX_LENGTH + 2

static const char *TAG_SUB = "G4-EDGE-SUB";

static void custom_topic_handler(const char *data, int data_len)
{
	if (strncmp(data, FORECAST_MESSAGE_PREFIX, FORECAST_MESSAGE_PREFIX_LENGTH) == 0)
	{
		if (FORECAST_MESSAGE_MIN_LENGTH <= data_len && data_len <= FORECAST_MESSAGE_MAX_LENGTH){
			char buffer[data_len - FORECAST_MESSAGE_PREFIX_LENGTH + 1];
			strncpy(buffer, &data[FORECAST_MESSAGE_PREFIX_LENGTH], data_len-FORECAST_MESSAGE_PREFIX_LENGTH);
			buffer[data_len-FORECAST_MESSAGE_PREFIX_LENGTH] = '\0';
			forecast = atoi(buffer);
			xQueueSend(count_display_q, (const void *)&count, portMAX_DELAY);
		}
	}
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
	esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;
	esp_mqtt_client_handle_t client = event->client;
	int msg_id;
	switch (event->event_id)
	{
	case MQTT_EVENT_CONNECTED:
		ESP_LOGI(TAG_SUB, "MQTT_EVENT_CONNECTED");
		msg_id = esp_mqtt_client_subscribe(client, MQTT_SUB_TOPIC, 0);
		ESP_LOGI(TAG_SUB, "sent subscribe successful, msg_id=%d", msg_id);
		break;
	case MQTT_EVENT_DISCONNECTED:
		ESP_LOGI(TAG_SUB, "MQTT_EVENT_DISCONNECTED");
		break;
	case MQTT_EVENT_SUBSCRIBED:
		break;
	case MQTT_EVENT_UNSUBSCRIBED:
		ESP_LOGI(TAG_SUB, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
		msg_id = esp_mqtt_client_subscribe(client, MQTT_SUB_TOPIC, 0);
		ESP_LOGI(TAG_SUB, "sent subscribe successful, msg_id=%d", msg_id);
		break;
	case MQTT_EVENT_PUBLISHED:
		break;
	case MQTT_EVENT_DATA:
		if (strncmp(event->topic, MQTT_SUB_TOPIC, event->topic_len) == 0)
		{
			custom_topic_handler(event->data, event->data_len);
		}
		break;
	case MQTT_EVENT_ERROR:
		ESP_LOGI(TAG_SUB, "MQTT_EVENT_ERROR");
		break;
	default:
		ESP_LOGI(TAG_SUB, "Other event id:%d", event->event_id);
		break;
	}
}

void setup_edge_subscriber()
{

	ESP_LOGI(TAG_SUB, "Use URI: %s", MQTT_BROKER_URI_BUFFER);
	ESP_LOGI(TAG_SUB, "Use Topic: %s", MQTT_SUB_TOPIC);

	const esp_mqtt_client_config_t mqtt_cfg = {
		.uri = MQTT_BROKER_URI_BUFFER,
		.port = MQTT_BROKER_PORT
	};

	esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
	esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
	esp_mqtt_client_start(client);
}
