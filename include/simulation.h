// Passenger check-in types
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

    double lambda;           // Passenger arrival rate
    double test_mu;          // Covid test officers service rate
    double online_mu;        // Check-in online service rate
    double national_mu;      // Check-in national service rate
    double international_mu; // Check-in international service rate

    double greenpass_p;      // Probability of having the green_pass
    double online_p;         // Probability of online check-in
    double national_p;       // Probability of national check-in
    double international_p;  // Probability of international check-in

} config;


// Struct which identifies a passenger
typedef struct passenger {

    int type;           // Passenger type (ONLINE, NATIONAL, INTERNATIONAL)
    int greenpass;      // Whether the passenger has a green pass or not
    int withdrawal;     // Whether the passenger has withdrawn

    int arrival;        // Arrival time (minutes)
    int begin;          // Begin of service time (minutes)
    int service;        // Service time (minutes)
    int departure;      // Departure time (minutes)

    int test_arrival;   // Covid test arrival time (minutes)
    int test_begin;     // Covid test begin of service time (minutes)
    int test_service;   // Covid test service time (minutes)
    int test_departure; // Covid test departure time (minutes)

    struct passenger *next;

} passenger;


// Structure which keeps the test results
typedef struct result{

    char *type;             // Check-in area configuration type (SINGLE, MULTI, SITA)
    int num_officers;       // Total number of officers
    double mwait;           // Average passenger wait
    double mresponse;       // Average passenger response time
    double mservice;        // Average service time
    double withdrawal;      // Percentage of passenger who withdrew from the queue

    // SITA configuration only
    int num_off_small;      // Number of officers for small jobs
    double mwait_small;     // Average passenger wait for small jobs
    double mresponse_small; // Average passenger response time for small jobs
    double mservice_small;  // Average passenger service time for small jobs
    double withdrawal_small;// Percentage of passenger who withdrew from the small jobs queue

    int num_off_med;        // Number of officers for medium jobs
    double mwait_med;       // Average passenger wait for medium jobs
    double mresponse_med;   // Average passenger response time for medium jobs
    double mservice_med;    // Average passenger service time for medium jobs
    double withdrawal_med;  // Percentage of passenger who withdrew from the medium jobs queue

    int num_off_big;        // Number of officers for big jobs
    double mwait_big;       // Average passenger wait for big jobs
    double mresponse_big;   // Average passenger response time for big jobs
    double mservice_big;    // Average passenger service time for big jobs
    double withdrawal_big;  // Percentage of passenger who withdrew from the big jobs queue

    // Covid test area
    char *test_type;        // Test area configuration type (SINGLE, MULTI, NONE)
    int test_officers;      // Total number of covid test officers
    double mwait_test;      // Average passenger covid test wait
    double mresponse_test;  // Average passenger covid test response time
    double mservice_test;   // Average passenger covid test service time

} result;

void init_simulation(long seed);
void reorder_passengers(passenger **head);
void generate_arrivals(config *conf, int stop, passenger **pass);
void free_passengers(passenger **pass);

void simulate_single_queue(int off_num, int wait, passenger *pass, result *res);
void simulate_multi_queue(int off_num, int wait, passenger *pass, result *res);
void simulate_sita_queue(int off_small, int off_med, int off_big, int wait, passenger *pass, result *res);
void simulate_single_test(int off_num, passenger **pass, result *res, int wait, double positive);
void simulate_multi_test(int off_num, passenger **pass, result *res, int wait, double positive);
