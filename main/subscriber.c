#include "common.h"
#include "subscriber.h"
#include "commands.h"


#define MQTT_BROKER_URI CONFIG_IOT_PLATFORM_ROOM_BROKER_URI

#define MQTT_SUB_TOPIC CONFIG_IOT_PLATFORM_ROOM_TOPIC
#define MQTT_SUB_TOPIC_USERNAME CONFIG_IOT_PLATFORM_ROOM_USERNAME
#define MQTT_SUB_TOPIC_PASSWORD CONFIG_IOT_PLATFORM_ROOM_PASSWORD

#define ENTERED_MSG "enter"
#define EXITED_MSG "leave"


static void custom_topic_handler(const char *data, int data_len)
{
	if (strncmp(data, ENTERED_MSG, data_len) == 0)
	{
		enterRoom();
	}
	else if (strncmp(data, EXITED_MSG, data_len) == 0)
	{
		leaveRoom();
	}
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
	esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;
	esp_mqtt_client_handle_t client = event->client;
	int msg_id;
	// your_context_t *context = event->context;
	switch (event->event_id)
	{
	case MQTT_EVENT_CONNECTED:
		ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
		msg_id = esp_mqtt_client_subscribe(client, MQTT_SUB_TOPIC, 0);
		ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
		break;
	case MQTT_EVENT_DISCONNECTED:
		ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
		break;
	case MQTT_EVENT_SUBSCRIBED:
		break;
	case MQTT_EVENT_UNSUBSCRIBED:
		ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
		msg_id = esp_mqtt_client_subscribe(client, MQTT_SUB_TOPIC, 0);
		ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
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
		ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
		break;
	default:
		ESP_LOGI(TAG, "Other event id:%d", event->event_id);
		break;
	}
}

void setup_subscriber()
{
	gpio_set_direction(TRIGGER_PIN_IN, GPIO_MODE_OUTPUT);
	gpio_set_direction(TRIGGER_PIN_OUT, GPIO_MODE_OUTPUT);

	// TODO: using the breadboard with I---[][]---O connections sometimes the signal is not going from I PIN to O PIN

	esp_mqtt_client_config_t mqtt_cfg = {
		.uri = MQTT_BROKER_URI,

	};
	esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
	esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
	esp_mqtt_client_start(client);
}
