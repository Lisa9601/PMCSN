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
    int *last_dep;          // Array of departure times of the last passenger processed by each officer
    int passenger_num;      // Number of passengers in the queue
    passenger *passengers;  // List of passengers waiting in the queue

} queue;

typedef struct config {

    char *name;                     // Configuration name

    int max_check_in_queue;         // Maximum num of passengers that can be queued for check-in
    int max_covid_test_queue;       // Maximum num of passengers that can be queued for the covid test

    int check_in_queue_num;         // Number of check-in queues
    int covid_test_queue_num;       // Number of covid test queues

    queue **check_in_queues;        // List of check-in queues
    queue **covid_test_queues;      // List of covid test queues

} config;

typedef struct result{



    int type;           // Configuration type (SINGLE, MULTI, SITA)
    int num_queues;     // Number of queues
    int num_officers;   // Total number of officers
    double mwait;       // Average passenger wait in the queues
    double mresponse;   // Average passenger response time

    // SITA configuration only
    int num_off_small;      // Number of officers for small jobs
    int num_off_med;        // Number of officers for medium jobs
    int num_off_big;        // Number of officers for big jobs

    double mwait_small;     // Average passenger wait for small jobs
    double mwait_med;       // Average passenger wait for medium jobs
    double mwait_big;       // Average passenger wait for big jobs
    double mresponse_small; // Average passenger response time for small jobs
    double mresponse_med;   // Average passenger response time for medium jobs
    double mresponse_big;   // Average passenger response time for big jobs

    // Covid test area
    int type;               // Configuration type (SINGLE, MULTI)
    int test_queues;        // Number of covid test queues
    int test_officers;      // Total number of covid test officers
    double mwait_test;      // Average passenger wait in the covid test queues
    double mresponse_test;  // Average passenger covid test response time

} result;

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
    new_passenger->next = NULL;

    return new_passenger;
}

/*
 * Adds a new passenger to the given list
 *
 * start: first element of the list
 * new_pass: new passenger
 *
 * return: the start of the list with the new passenger
 */
passenger *add_passenger(passenger *start, passenger *new_pass){
    passenger *prev, *curr;

    if(start == NULL){
        return new_pass;
    }
    else if(start->arrival > new_pass->arrival){
        new_pass->next = start;
        return new_pass;
    }
    else{
        prev = start;
        for(curr=start->next; curr!= NULL; curr=curr->next){

            // Order by arrival
            if(curr->arrival > new_pass->arrival){
                new_pass->next = curr;
                prev->next = new_pass;
                return start;
            }
            prev = curr;
        }

        prev->next = new_pass;
        return start;
    }

}

/*
 * Creates a new queue
 *
 * num: Queue number
 * off_num: Number of officers for the queue
 * pass_num: Number of passengers in the queue
 *
 * return: A pointer to the new queue, NULL on error
 */
queue *create_queue(int num, int off_num, int pass_num){
    queue *new_queue;
    int last_dep[off_num] = {0};

    new_queue = (queue *)malloc(sizeof(queue));

    if(new_queue == NULL){
        return NULL;
    }

    // Initializing queue
    new_queue->num = num;
    new_queue->officers_num = off_num;
    new_queue->passengers = last_dep;
    new_queue->passenger_num = pass_num;
    new_queue->passengers = NULL;

    return new_queue;
}

/*
 * Creates a new configuration
 *
 * name: Configuration name
 * max_check: Max number of passengers in the check-in queue
 * max_test: Max number of passengers in the covid test queue
 * check_num: Number of check in queues
 * test_num: Number of covid test queues
 *
 * return: A pointer to the new configuration, NULL on error
 */
config *create_config(char *name, int max_check, int max_test, int check_num, int test_num){
    config *new_config

    new_config = (config *)malloc(sizeof(config));

    if(new_config == NULL){
        return NULL;
    }

    // Initializing configuration
    new_config->name = name;
    new_config->max_check_in_queue = max_check;
    new_config->max_covid_test_queue = max_test;
    new_config->check_in_queue_num = check_num;
    new_config->covid_test_queue_num = test_num;
    new_config->check_in_queues = NULL;
    new_config->covid_test_queues = NULL;

    return new_config;
}