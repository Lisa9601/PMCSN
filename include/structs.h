#define ONLINE 1            // Check-in online
#define NATIONAL 2          // National flight
#define INTERNATIONAL 3     // International flight

typedef struct passenger {

    int num;            // Passenger number

    int green_pass;     // If the passenger has the green pass this value it's set to 1, otherwise it' set to 0
    int check_in;       // Check-in type (online, national, international)

    int test_arrival;   // Test area arrival time (minutes)
    int test_service;   // Test area begin of service time (minutes)
    int test_departure; // Test area departure time (minutes)

    int arrival;        // Check-in area arrival time (minutes)
    int service;        // Check-in area begin of service time (minutes)
    int departure;      // System departure time (minutes)

    struct passenger *next;

} passenger;

typedef struct queue {

    int num;            // Queue number

    passenger *first;   // Pointer to the first element of the list of passengers waiting in the queue

} queue;

typedef struct officer {

    int num;        // Officer number

    queue *queue;   // Pointer to the queue of passengers to be served

    struct officer *next;

} officer;

typedef struct config {

    char *name;                         // Configuration name

    officer *first_check_in_officer;    // Pointer to the first check-in officer
    officer *first_covid_test_officer;  // Pointer to the first covid test officer

} config;