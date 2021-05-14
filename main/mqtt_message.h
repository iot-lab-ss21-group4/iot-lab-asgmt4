#ifndef MQTT_MESSAGE_H
#define MQTT_MESSAGE_H

#define MQTT_COUNT_MESSAGE_PATTERN "{\"username\":\"%s\",\"%s\":%d,\"device_id\":%d,\"timestamp\":%llu}"
#define MQTT_COUNT_MESSAGE_BUFFER_SIZE 100
static char MQTT_COUNT_MESSAGE_BUFFER[MQTT_COUNT_MESSAGE_BUFFER_SIZE];

#define ENTERED_MSG "enter"
#define EXITED_MSG "leave"
// more corner cases will come her

#endif
