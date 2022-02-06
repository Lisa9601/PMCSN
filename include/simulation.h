// Service types
#define ONLINE 1
#define NATIONAL 2
#define INTERNATIONAL 3

// Queue configuration types
#define NONE NULL
#define SINGLE "single-queue"
#define MULTI "multi-queue"
#define SITA "sita-queue"

// Struct which keeps all the configuration parameters
typedef struct config {

    long seed;              // Initial seed
    long stop;              // Simulation end time
    long maxwait;           // Maximum time a passenger is willing to wait in the queues
    long testwait;          // Time to wait for the test results

    int off;                // Number of officers
    int online_off;         // Number of officers for the online queue
    int national_off;       // Number of officers for the national queue
    int international_off;  // Number of officers for the international queue
    int test_off;           // Number of officers for the test queue

    double lambda_1_5;      // Passenger arrival rate 1:00:01 - 5:00:00
    double lambda_5_9;      // Passenger arrival rate 5:00:01 - 9:00:00
    double lambda_9_13;     // Passenger arrival rate 9:00:01 - 13:00:00
    double lambda_13_17;    // Passenger arrival rate 13:00:01 - 17:00:00
    double lambda_17_21;    // Passenger arrival rate 17:00:01 - 21:00:00
    double lambda_21_1;     // Passenger arrival rate 21:00:01 - 1:00:00

    double test_mu;          // Covid test officers service rate
    double online_mu;        // Check-in online service rate
    double national_mu;      // Check-in national service rate
    double international_mu; // Check-in international service rate

    double greenpass_p;      // Probability of having the green_pass
    double positive_p;       // Probability of being positive to covid-19
    double online_p;         // Probability of online check-in
    double national_p;       // Probability of national check-in
    double international_p;  // Probability of international check-in

} config;

// Structure which keeps a passenger arrival info
typedef struct arrival{

    int test;           // Whether the passenger has to take a covid test or not
    int checkin_type;   // Passenger check-in type
    int time;           // Time of arrival
    struct arrival *next;

} arrival;

// Structure which keeps a passenger service info
typedef struct service{

    int test;           // Whether the passenger has to take a covid test or not
    int checkin_type;   // Type of service
    int arrival;        // Arrival time
    int start;          // Begin of service time
    int end;            // End of service time
    int withdrawal;     // Whether the passenger has withdrawn from the queue
    int officer;        // Number of officers that will serve the passenger
    struct service *next;

} service;

// Structure which keeps the simulation state
typedef struct state{

    long clock;             // Simulation clock
    int *officers;          // Array of officers
    long *queues;           // Array of queues
    int *test_officers;     // Array of test officers
    long *test_queues;      // Array of test queues
    int *low_officers;      // Array of low officers
    int *med_officers;      // Array of med officers
    int *high_officers;     // Array of high officers

    arrival *arr_list;      // List of passenger arrivals
    service *serv_list;     // List of passenger services
    long last_arr;          // Last arrival time

} state;

// Structure which keeps the test results
typedef struct result{

    char *type;             // Check-in area configuration type (SINGLE, MULTI, SITA)
    int num_officers;       // Total number of officers
    long num_pass;          // Total number of passengers
    double mwait;           // Average passenger wait
    double mresponse;       // Average passenger response time
    double mservice;        // Average service time
    double withdrawal;      // Percentage of passenger who withdrew from the queue

    // SITA configuration only
    int num_off_small;      // Number of officers for small jobs
    long num_pass_small;    // Number of passenger with small jobs
    double mwait_small;     // Average passenger wait for small jobs
    double mresponse_small; // Average passenger response time for small jobs
    double mservice_small;  // Average passenger service time for small jobs
    double withdrawal_small;// Percentage of passenger who withdrew from the small jobs queue

    int num_off_med;        // Number of officers for medium jobs
    long num_pass_med;      // Number of passenger with medium jobs
    double mwait_med;       // Average passenger wait for medium jobs
    double mresponse_med;   // Average passenger response time for medium jobs
    double mservice_med;    // Average passenger service time for medium jobs
    double withdrawal_med;  // Percentage of passenger who withdrew from the medium jobs queue

    int num_off_big;        // Number of officers for big jobs
    long num_pass_big;      // Number of passenger with big jobs
    double mwait_big;       // Average passenger wait for big jobs
    double mresponse_big;   // Average passenger response time for big jobs
    double mservice_big;    // Average passenger service time for big jobs
    double withdrawal_big;  // Percentage of passenger who withdrew from the big jobs queue

    // Covid test area
    char *test_type;        // Test area configuration type (SINGLE, MULTI, NONE)
    int test_officers;      // Total number of covid test officers
    long num_pass_test;     // Number of passenger who need a covid test
    double mwait_test;      // Average passenger covid test wait
    double mresponse_test;  // Average passenger covid test response time
    double mservice_test;   // Average passenger covid test service time
    double withdrawal_test; // Percentage of passenger who withdrew from the covid test queue

} result;

void init_simulation(long seed);
void simulate(config *conf, state *st, result *res, char *checkin, char *test);