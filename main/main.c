#include "common.h"
#include "oled.h"
#include "transitions.h"
#include "home_wifi.h"
#include "time_management.h"
#include "commands.h"
#include "subscriber.h"
#include "publisher.h"

void app_main(void)
{
	init_logging();
	init_common_queues();
	init_esp_dependencies();

	setup_wifi();
	setup_time_management();
	setup_oled();
	setup_transitions();

#if !(PUBLISHER)
	setup_subscriber();
	test_trigger_pins();
#endif
	setup_publisher();

	while (1)
	{
		loop_oled();
		loop_transitions();
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}
