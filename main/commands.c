#include "common.h"
#include "commands.h"

static uint8_t expected_room_count = 0;

#define TEST_INIT_ROOM                                                  \
	do                                                                  \
	{                                                                   \
		assert_room_count(expected_room_count, "Test Initial Counter"); \
	} while (0);
#define TEST_ENTER_ROOM                                            \
	do                                                             \
	{                                                              \
		enterRoom();                                               \
		if (expected_room_count < MAX_ROOM_COUNT)                  \
			expected_room_count++;                                 \
		assert_room_count(expected_room_count, "Test Enter Room"); \
	} while (0);
#define TEST_LEAVE_ROOM                                            \
	do                                                             \
	{                                                              \
		leaveRoom();                                               \
		if (expected_room_count > MIN_ROOM_COUNT)                  \
			expected_room_count--;                                 \
		assert_room_count(expected_room_count, "Test Leave Room"); \
	} while (0);
#define TEST_PEAK_OUT_ROOM                                               \
	do                                                                   \
	{                                                                    \
		peakOutofRoom();                                                 \
		assert_room_count(expected_room_count, "Test Peak Out Of Room"); \
	} while (0);
#define TEST_PEAK_IN_ROOM                                              \
	do                                                                 \
	{                                                                  \
		peakIntoRoom();                                                \
		assert_room_count(expected_room_count, "Test Peak Into Room"); \
	} while (0);
#define TEST_HALFWAY_LEAVE_ROOM                                            \
	do                                                                     \
	{                                                                      \
		halfwayLeave();                                                    \
		assert_room_count(expected_room_count, "Test Halfway Leave Room"); \
	} while (0);
#define TEST_HALFWAY_ENTER_ROOM                                            \
	do                                                                     \
	{                                                                      \
		halfwayEnter();                                                    \
		assert_room_count(expected_room_count, "Test Halfway Enter Room"); \
	} while (0);
#define TEST_MANIPULATION_ENTER_ROOM                                            \
	do                                                                          \
	{                                                                           \
		manipulationEnter();                                                    \
		assert_room_count(expected_room_count, "Test Manipulation Enter Room"); \
	} while (0);
#define TEST_ALMOST_ENTER_ROOM                                            \
	do                                                                    \
	{                                                                     \
		almostEnterRoom();                                                \
		assert_room_count(expected_room_count, "Test Almost Enter Room"); \
	} while (0);
#define TEST_ALMOST_LEAVE_ROOM                                            \
	do                                                                    \
	{                                                                     \
		almostLeaveRoom();                                                \
		assert_room_count(expected_room_count, "Test Almost Leave Room"); \
	} while (0);

static void almostEnterRoom();
static void almostLeaveRoom();
static void peakIntoRoom();
static void peakOutofRoom();
static void halfwayLeave();
static void halfwayEnter();
static void manipulationEnter();
static void obstructionInside();
static void assert_room_count(uint8_t expected_count, const char *test_description);

void test_trigger_pins()
{
	in_testing_scenario = true;
	ESP_LOGI(TAG, "Start testing");
	ESP_LOGI(TAG, "Inner Trigger PIN: %d", TRIGGER_PIN_IN);
	ESP_LOGI(TAG, "Outer Trigger PIN: %d", TRIGGER_PIN_OUT);
	ESP_LOGI(TAG, "Inner Barrier PIN: %d", INNER_BARRIER_PIN);
	ESP_LOGI(TAG, "Outer Barrier PIN: %d", OUTER_BARRIER_PIN);
	expected_room_count = count;

	TEST_INIT_ROOM
	TEST_ENTER_ROOM
	TEST_PEAK_OUT_ROOM
	TEST_ALMOST_LEAVE_ROOM
	TEST_LEAVE_ROOM
	TEST_PEAK_IN_ROOM
	TEST_ALMOST_ENTER_ROOM
	TEST_HALFWAY_LEAVE_ROOM
	TEST_HALFWAY_ENTER_ROOM
	TEST_MANIPULATION_ENTER_ROOM

	// finish with normal behavior
	TEST_ENTER_ROOM
	TEST_LEAVE_ROOM

	in_testing_scenario = false;
	ESP_LOGI(TAG, "End testing");
}

//////COMMANDS//////

void pingBuiltInLed()
{
	gpio_set_direction(ONBOARD_LED_PIN, GPIO_MODE_OUTPUT);
	gpio_set_level(ONBOARD_LED_PIN, 1);
	vTaskDelay(1000 / portTICK_PERIOD_MS);
	gpio_set_level(ONBOARD_LED_PIN, 0);
	vTaskDelay(1000 / portTICK_PERIOD_MS);
}

void enterRoom()
{
	ESP_LOGI(TAG, "Command: Enter");
	gpio_set_level(TRIGGER_PIN_OUT, 1);
	vTaskDelay(200 / portTICK_RATE_MS);
	gpio_set_level(TRIGGER_PIN_IN, 1);
	vTaskDelay(200 / portTICK_RATE_MS);
	gpio_set_level(TRIGGER_PIN_OUT, 0);
	vTaskDelay(200 / portTICK_RATE_MS);
	gpio_set_level(TRIGGER_PIN_IN, 0);
	vTaskDelay(200 / portTICK_RATE_MS);
}

void leaveRoom()
{
	ESP_LOGI(TAG, "Command: Leave");
	gpio_set_level(TRIGGER_PIN_IN, 1);
	vTaskDelay(200 / portTICK_RATE_MS);
	gpio_set_level(TRIGGER_PIN_OUT, 1);
	vTaskDelay(200 / portTICK_RATE_MS);
	gpio_set_level(TRIGGER_PIN_IN, 0);
	vTaskDelay(200 / portTICK_RATE_MS);
	gpio_set_level(TRIGGER_PIN_OUT, 0);
	vTaskDelay(200 / portTICK_RATE_MS);
}

static void almostEnterRoom()
{
	ESP_LOGI(TAG, "Command: Almost enter");
	gpio_set_level(TRIGGER_PIN_OUT, 1);
	vTaskDelay(200 / portTICK_RATE_MS);
	gpio_set_level(TRIGGER_PIN_IN, 1);
	vTaskDelay(200 / portTICK_RATE_MS);
	gpio_set_level(TRIGGER_PIN_OUT, 0);
	vTaskDelay(200 / portTICK_RATE_MS);

	gpio_set_level(TRIGGER_PIN_OUT, 1);
	vTaskDelay(200 / portTICK_RATE_MS);
	gpio_set_level(TRIGGER_PIN_IN, 0);
	vTaskDelay(200 / portTICK_RATE_MS);
	gpio_set_level(TRIGGER_PIN_OUT, 0);
	vTaskDelay(200 / portTICK_RATE_MS);
}

static void almostLeaveRoom()
{
	ESP_LOGI(TAG, "Command: Almost leave");
	gpio_set_level(TRIGGER_PIN_IN, 1);
	vTaskDelay(200 / portTICK_RATE_MS);
	gpio_set_level(TRIGGER_PIN_OUT, 1);
	vTaskDelay(200 / portTICK_RATE_MS);
	gpio_set_level(TRIGGER_PIN_IN, 0);
	vTaskDelay(200 / portTICK_RATE_MS);

	gpio_set_level(TRIGGER_PIN_IN, 1);
	vTaskDelay(200 / portTICK_RATE_MS);
	gpio_set_level(TRIGGER_PIN_OUT, 0);
	vTaskDelay(200 / portTICK_RATE_MS);
	gpio_set_level(TRIGGER_PIN_IN, 0);
	vTaskDelay(200 / portTICK_RATE_MS);
}

static void peakIntoRoom()
{
	/*reach out one's head towards room, then lean back*/
	ESP_LOGI(TAG, "Command: Peak In");
	gpio_set_level(TRIGGER_PIN_OUT, 1);
	vTaskDelay(100 / portTICK_RATE_MS);
	gpio_set_level(TRIGGER_PIN_OUT, 0);
	vTaskDelay(500 / portTICK_RATE_MS);
}

static void peakOutofRoom()
{
	ESP_LOGI(TAG, "Command: Peak Out");
	gpio_set_level(TRIGGER_PIN_IN, 1);
	vTaskDelay(100 / portTICK_RATE_MS);
	gpio_set_level(TRIGGER_PIN_IN, 0);
	vTaskDelay(500 / portTICK_RATE_MS);
}

// halfwayEnter in forum but name makes no sense
static void halfwayLeave()
{
	/*someone go to the middle of the doorway, and then turns around*/
	ESP_LOGI(TAG, "Command: Half Leave");
	gpio_set_level(TRIGGER_PIN_IN, 1);
	vTaskDelay(100 / portTICK_RATE_MS);
	gpio_set_level(TRIGGER_PIN_OUT, 1);
	vTaskDelay(50 / portTICK_RATE_MS);
	gpio_set_level(TRIGGER_PIN_OUT, 0);
	vTaskDelay(100 / portTICK_RATE_MS);
	gpio_set_level(TRIGGER_PIN_IN, 0);
	vTaskDelay(500 / portTICK_RATE_MS);
}

static void halfwayEnter()
{
	/*someone go to the middle of the doorway, and then turns around*/
	ESP_LOGI(TAG, "Command: Half Enter");
	gpio_set_level(TRIGGER_PIN_OUT, 1);
	vTaskDelay(100 / portTICK_RATE_MS);
	gpio_set_level(TRIGGER_PIN_IN, 1);
	vTaskDelay(50 / portTICK_RATE_MS);
	gpio_set_level(TRIGGER_PIN_IN, 0);
	vTaskDelay(100 / portTICK_RATE_MS);
	gpio_set_level(TRIGGER_PIN_OUT, 0);
	vTaskDelay(500 / portTICK_RATE_MS);
}

static void manipulationEnter()
{
	/*Someone is trying to manipulate the count by waving their arm through the barrier towards the inside
 Sequence is not possible if a person enters*/
	ESP_LOGI(TAG, "Command: Manipulation Enter");
	gpio_set_level(TRIGGER_PIN_IN, 1);
	vTaskDelay(15 / portTICK_RATE_MS);
	gpio_set_level(TRIGGER_PIN_IN, 0);
	vTaskDelay(15 / portTICK_RATE_MS);
	gpio_set_level(TRIGGER_PIN_OUT, 1);
	vTaskDelay(15 / portTICK_RATE_MS);
	gpio_set_level(TRIGGER_PIN_OUT, 0);
	vTaskDelay(500 / portTICK_RATE_MS);
}

static void obstructionInside()
{
	/*Someone is standing in the inside barrier, making counting impossible*/
	//TODO test needs separate task
	ESP_LOGI(TAG, "Command: Obstruction Inside");
	gpio_set_level(TRIGGER_PIN_IN, 1);
	vTaskDelay(6000 / portTICK_RATE_MS);
	gpio_set_level(TRIGGER_PIN_OUT, 1);
	vTaskDelay(3000 / portTICK_RATE_MS);
	/*resolve obstruction*/
	gpio_set_level(TRIGGER_PIN_IN, 0);
	gpio_set_level(TRIGGER_PIN_OUT, 0);
	vTaskDelay(500 / portTICK_RATE_MS);
}

///////////////////////////////////////

static void assert_room_count(uint8_t expected_count, const char *test_description)
{
	if (count == expected_count)
	{
		ESP_LOGI(TAG, "%s: Ok", test_description);
	}
	else
	{
		ESP_LOGE(TAG, "%s: Fail", test_description);
	}
}
