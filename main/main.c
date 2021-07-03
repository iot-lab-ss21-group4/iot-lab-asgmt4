#include "common.h"
#include "oled.h"
#include "transitions.h"
#include "home_wifi.h"
#include "time_management.h"
#include "commands.h"
#include "subscriber.h"
#include "publisher.h"
#include "subscriber_edge.h"

void app_main(void)
{
	init_logging();
	init_common_queues();
	init_esp_dependencies();
	init_counter();

	setup_wifi();
	setup_time_management();
	setup_oled();
	setup_transitions();

#if GET_CLOUD_EVENTS
	setup_subscriber();
#if TEST_ROOM_COMMANDS
	test_trigger_pins();
	test_milestone_one();
#endif
#endif

#if GET_EDGE_EVENTS
	setup_edge_subscriber();
#endif

	setup_publisher();

	// while (1)
	// {
	// 	loop_transitions();
	// 	vTaskDelay(1000 / portTICK_PERIOD_MS);
	// }
}
