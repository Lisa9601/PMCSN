// File
#define FILE_NAME "prova"       // Name to assign to the new csv file

// Parameters
#define LAMBDA 1
#define TEST_MEAN 5
#define ONLINE_MEAN 2
#define NATIONAL_MEAN 5
#define INTERNATIONAL_MEAN 10

// Configuration
#define QUEUE_TYPE 1            // 1 = SINGLE_QUEUE, 2 = MULTI_QUEUE, 3 = SITA_QUEUE
#define TEST 1                  // 1 = Test area , 0 = No test area

// Check-in area
#define CHECK_OFF 5
#define MAX_CHECK_QUEUE 200
#define CHECK_QUEUE_NUM 5       // Only used in multi queue and sita queue
#define ONLINE_P 0.5
#define NATIONAL_P 0.3

// Covid test area
#define TEST_OFF 5
#define MAX_TEST_QUEUE 200
#define TEST_QUEUE_NUM 5
#define GREEN_P 0.8