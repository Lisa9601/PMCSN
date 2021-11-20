#include <stdlib.h>

#define ONLINE 1            // Check-in online
#define NATIONAL 2          // National flight
#define INTERNATIONAL 3     // International flight

typedef struct passenger {

    int num;            // Passenger number

    int green_pass;     // If the passenger has the green pass this value it's set to 1, otherwise it' set to 0
    int check_in;       // Check-in type (online, national, international)

    int arrival;        // Arrival time (minutes)
    int begin;          // Begin of service time (minutes)
    int service;        // Service time (minutes)
    int departure;      // Departure time (minutes)

    int test_arrival;        // Covid test arrival time (minutes)
    int test_begin;          // Covid test begin of service time (minutes)
    int test_service;        // Covid test service time (minutes)
    int test_departure;      // Covid test departure time (minutes)

    struct passenger *next;

} passenger;

typedef struct queue {

    int num;                // Queue number

    int officers_num;       // Number of officers
    int passenger_num;      // Number of passengers in the queue
    passenger *passengers;  // List of passengers waiting in the queue

    struct queue *next;

} queue;

typedef struct config {

    char *name;                     // Configuration name

    int max_check_in_queue;         // Maximum num of passengers that can be queued for check-in
    int max_covid_test_queue;       // Maximum num of passengers that can be queued for the covid test

    queue *check_in_queues;         // List of check-in queues
    queue *covid_test_queues;       // List of covid test queues

} config;


/*
 * Creates a new passenger
 *
 * num: Passenger number
 * green_pass: Whether the user has a green pass or not
 * check_in: Check-in type
 * test_arrival: Test area arrival time
 * test_service: Test area service time
 * check_arrival: Check-in area arrival time
 * check_service: Check-in area service time
 *
 * return: A pointer to the new passenger, NULL on error
 */
passenger *create_passenger(int num, int green_pass, int check_in, int test_arrival, int test_service, int check_arrival, int check_service){
    passenger *new_passenger;

    new_passenger = (passenger *)malloc(sizeof(passenger));

    if(new_passenger == NULL){
        return NULL;
    }

    // Initializing passenger
    new_passenger->num = num;
    new_passenger->green_pass = green_pass;
    new_passenger->check_in = check_in;
    new_passenger->test_arrival = test_arrival;
    new_passenger->test_begin = 0;
    new_passenger->test_service = test_service;
    new_passenger->test_departure = 0;
    new_passenger->arrival = check_arrival;
    new_passenger->begin = 0;
    new_passenger->service = check_service;
    new_passenger->departure = 0;

    return new_passenger;
}

/*
 * Creates a new queue
 *
 * num: Queue number
 * off_num: Number of officers for the queue
 * pass_num: Number of passengers in the queue
 * first_pass: Pointer to the first passenger in the queue
 *
 * return: A pointer to the new queue, NULL on error
 */
queue *create_queue(int num, int off_num, int pass_num, passenger *first_pass){
    queue *new_queue;

    new_queue = (queue *)malloc(sizeof(queue));

    if(new_queue == NULL){
        return NULL;
    }

    // Initializing queue
    new_queue->num = num;
    new_queue->officers_num = off_num;
    new_queue->passenger_num = pass_num;
    new_queue->passengers = first_pass;

    return new_queue;
}

/*
 * Creates a new configuration
 *
 * name: Configuration name
 * max_check: Max number of passengers in the check-in queue
 * max_test: Max number of passengers in the covid test queue
 * check_queue: Pointer to the first check-in queue
 * test_queue: Pointer to the first test queue
 *
 * return: A pointer to the new configuration, NULL on error
 */
config *create_config(char *name, int max_check, int max_test, queue *check_queue, queue *test_queue){
    config *new_config

    new_config = (config *)malloc(sizeof(config));

    if(new_config == NULL){
        return NULL;
    }

    // Initializing configuration
    new_config->name = name;
    new_config->max_check_in_queue = max_check;
    new_config->max_covid_test_queue = max_test;
    new_config->check_in_queues = check_queue;
    new_config->covid_test_queues = test_queue;

    return new_config;
}