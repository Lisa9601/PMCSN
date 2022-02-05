#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>
#include <string.h>
#include "../include/simulation.h"
#include "../include/rngs.h"

static long clock;      // Simulation clock, keeps track of the current time
static int *officers;   // Array of officers that can be busy (1) or not (0)
static long *queues;    // Array of queues, each element keeps the current number of enqueued passengers

// TEST
static int *test_officers;
static long *test_queues;

// SITA ONLY
static int *low_officers;
static int *med_officers;
static int *high_officers;


typedef struct arrival{

    int test;           // Whether the passenger has to take a covid test or not
    int checkin_type;   // Passenger check-in type
    int time;           // Time of arrival
    struct arrival *next;

} arrival;

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

/*
 * Initialize simulation with the given seed
 *
 * seed: seed
 *
 * return:
 */
void init_simulation(long seed){
    PlantSeeds(seed);
    clock = 0;
    officers = NULL;
    queues = NULL;
    low_officers = NULL;
    med_officers = NULL;
    high_officers = NULL;
}

/*
 *  Exponential
 *
 *  m: mean
 *  stream: stream number
 *
 *  return: random exponential number
 */
double Exponential(double m, int stream){
    SelectStream(stream);
    return -m*log(1.0 -Random());
}

/*
 * Random
 *
 * stream: stream number
 *
 * return: random number between 0 and 1
 */
double Random_num(int stream){
    SelectStream(stream);
    return 1.0 - Random();
}

// EVENT HANDLING ------------------------------------------------------------------------------------------------------

/*
 * Creates a new arrival event
 *
 * config: A pointer to a configuration structure
 * time: Arrival time
 *
 * return: A pointer to the new arrival, NULL on error
 */
arrival *create_arrival(config *conf, int time){
    arrival *new;
    double random;

    new = (arrival *)malloc(sizeof(arrival));
    if(!new){
        perror("New arrival creation error");
        return NULL;
    }

    // Initializing arrival
    new->next = NULL;

    if(time > 1*60*60 && time <= 5*60*60) {
        new->time += time + Exponential(1/conf->lambda_1_5, 0);
    }
    else if(time > 5*60*60 && time <= 9*60*60) {
        new->time += time + Exponential(1/conf->lambda_5_9, 0);
    }
    else if(time > 9*60*60 && time <= 13*60*60) {
        new->time += time + Exponential(1/conf->lambda_9_13, 0);
    }
    else if(time > 13*60*60 && time <= 17*60*60) {
        new->time += time + Exponential(1/conf->lambda_13_17, 0);
    }
    else if(time > 17*60*60 && time <= 21*60*60) {
        new->time += time + Exponential(1/conf->lambda_17_21, 0);
    }
    else{
        new->time += time + Exponential(1/conf->lambda_21_1, 0);
    }

    // Green pass
    random = Random_num(2);
    if(random < conf->greenpass_p){
        new->test = 0;
    }
    else{
        new->test = 1;
    }

    // Check-in type
    random = Random_num(3);
    if(random < conf->online_p){
        new->checkin_type = ONLINE;

    }
    else if(random < conf->online_p + conf->national_p){
        new->checkin_type = NATIONAL;
    }
    else{
        new->checkin_type = INTERNATIONAL;
    }

    return new;
}

/*
 * Adds a new arrival to the given list ordered by time
 *
 * head: head of the list
 * new: new arrival
 *
 * return:
 */
void add_arrival(arrival **head, arrival *new){
    arrival *prev, *curr;

    // No elements in the list
    if(*head == NULL){
        *head = new;
        return;
    }

    // First element should be after the new arrival
    if((*head)->time > new->time){
        new->next = *head;
        *head = new;
        return;
    }

    prev = *head;

    // Find correct place inside list
    for(curr=prev->next; curr!=NULL; curr=curr->next){
        if(curr->time > new->time){
            new->next = curr;
            prev->next = new;
            return;
        }
        prev = curr;
    }

    prev->next = new;
    new->next = NULL;
}

/*
 * Removes the first arrival from an arrivals list
 *
 * list: list of arrival events
 *
 * return:
 */
void remove_arrival(arrival **list){
    arrival *temp;

    // Empty list
    if(list == NULL) return;

    temp = *list;
    *list = temp->next;

    free(temp);
}

/*
 * Removes the given arrival list
 *
 * pass: head of the list of arrivals
 *
 * return:
 */
void free_arrivals(arrival **list){
    arrival *prev, *p;

    prev = p = *list;

    while(p != NULL){
        p = p->next;
        free(prev);
        prev = p;
    }
}

/*
 * Creates a new service event
 *
 * conf: Pointer to a configuration structure
 * checkin_type: type of service (ONLINE, NATIONAL, INTERNATIONAL)
 * test: Whether the passenger has to take a covid test or not
 * arrival: Arrival time
 * officer: Number of the officer that will serve the passenger
 *
 * return: A pointer to the new service, NULL on error
 */
service *create_service(config *conf, int checkin_type, int test, int arrival, int officer){
    service *new;

    new = (service *)malloc(sizeof(service));
    if(!new){
        perror("New service creation error");
        return NULL;
    }

    // Initialize service
    new->test = test;
    new->checkin_type = checkin_type;
    new->arrival = arrival;
    new->officer = officer;
    new->next = NULL;

    //Start
    if(clock > new->arrival){
        new->start = clock;
    }
    else{
        new->start = new->arrival;
    }

    if(test > 0){
        //Covid test
        if(Random_num(4) < conf->positive_p){
            new->withdrawal = 1;
        }
    }

    if(new->start - new->arrival > conf->maxwait){
        // Withdrawal
        new->withdrawal = 1;
        new->end = -1;
    }
    else{
        new->withdrawal = 0;

        // End time
        if(test > 0){
            new->end = new->start + Exponential(1/conf->test_mu, 1);
        }
        else{
            if(checkin_type == ONLINE){
                new->end = new->start + Exponential(1/conf->online_mu, 1);
            }
            else if(checkin_type == NATIONAL){
                new->end = new->start + Exponential(1/conf->national_mu, 1);
            }
            else{
                new->end = new->start + Exponential(1/conf->international_mu, 1);
            }
        }
    }


    return new;
}

/*
 * Adds a new service to the given list ordered by time
 *
 * head: head of the list
 * new: new service
 *
 * return:
 */
void add_service(service **head, service *new){
    service *prev, *curr;

    // No elements in the list
    if(*head == NULL){
        *head = new;
        return;
    }

    // First element should be after the new service
    if((*head)->end > new->end){
        new->next = *head;
        *head = new;
        return;
    }

    prev = *head;

    // Find correct place inside list
    for(curr=prev->next; curr!=NULL; curr=curr->next){
        if(curr->end > new->end){
            new->next = curr;
            prev->next = new;
            return;
        }
        prev = curr;
    }

    prev->next = new;
    new->next = NULL;
}

/*
 * Removes the given service list
 *
 * pass: head of the list of services
 *
 * return:
 */
void free_services(service **list){
    service *prev, *p;

    prev = p = *list;

    while(p != NULL){
        p = p->next;
        free(prev);
        prev = p;
    }
}

/*
 * Removes the first service from a services list
 *
 * list: list of service events
 *
 * return:
 */
void remove_service(service **list){
    service *temp;

    // Empty list
    if(list == NULL) return;

    temp = *list;
    *list = temp->next;

    free(temp);
}

// STATE UPDATE --------------------------------------------------------------------------------------------------------

/*
 * Find available officer
 *
 * officers: array of officers (0) free (1) busy
 * off_num: total number of officers
 *
 * return: officer number, -1 if no officer is available
 */
int find_officer(int *officers, int off_num){
    int i, num;

    num = -1;

    for(i=0; i<off_num; i++){
        if(officers[i] == 0){
            num = i;
            break;
        }
    }

    return num;
}

/*
 * Find queue with the lowest number of passengers
 *
 * queues: array with the number of passengers for each queue
 * queue_num: number of queues in the array
 *
 * return: queue number
 */
int find_queue(long *queues, int queue_num){
    int i, num;
    long lowest;

    num = -1;

    if(queue_num > 0){
        lowest = queues[0];
        num = 0;

        for(i=1; i<queue_num; i++){
            if(queues[i] == 0){
                // Queues it's empty
                return i;
            }
            else if(queues[i] < lowest){
                lowest = queues[i];
                num = i;
            }
        }
    }

    for(i=0; i<queue_num; i++){
        printf("%ld ", queues[i]);
    }
    printf("\n");

    return num;
}

/*
 * Handle a new passenger arrival (SINGLE)
 *
 * conf: pointer to a configuration structure
 * arr: pointer to an arrival structure
 * list: list of service events
 *
 * return: 0 on success, negative value on error
 */
int handle_single_arrival(config *conf, arrival *arr, service **list){
    int i;
    service *new;

    i = find_officer(officers, conf->off);

    if(i < 0){
        // No officer available
        return -1;
    }

    officers[i] = 1; // Make busy

    new = create_service(conf, arr->checkin_type, arr->test, arr->time, i);

    if(!new){
        perror("Passenger service it's lost!");
        officers[i] = 0;
        return -2;
    }
    else if(new->withdrawal > 0){
        officers[i] = 0;
    }

    add_service(list, new);
    return 0;
}

/*
 * Handle a new passenger covid test arrival (SINGLE)
 *
 * conf: pointer to a configuration structure
 * arr: pointer to an arrival structure
 * list: list of service events
 *
 * return: 0 on success, negative value on error
 */
int handle_single_test_arrival(config *conf, arrival *arr, service **list){
    int i;
    service *new;

    i = find_officer(test_officers, conf->test_off);

    if(i < 0){
        // No officer available
        return -1;
    }

    test_officers[i] = 1; // Make busy

    new = create_service(conf, arr->checkin_type, arr->test, arr->time, i);

    if(!new){
        perror("Passenger service it's lost!");
        test_officers[i] = 0;
        return -2;
    }
    else if(new->withdrawal > 0){
        test_officers[i] = 0;
    }

    add_service(list, new);
    return 0;
}

/*
 * Handle a new passenger arrival (MULTI)
 *
 * conf: pointer to a configuration structure
 * arr: pointer to an arrival structure
 * list: list of service events
 *
 * return: 0 on success, negative value on error
 */
int handle_multi_arrival(config *conf, arrival *arr, service **list){
    int i;
    service *new;

    // Check-in arrival
    i = find_queue(queues, conf->off);

    if(officers[i] > 0){
        // Officer it's busy
        return -1;
    }

    officers[i] = 1; // Make busy
    queues[i]++;

    new = create_service(conf, arr->checkin_type, arr->test, arr->time, i);

    if(!new){
        perror("Passenger service it's lost!");
        officers[i] = 0;
        queues[i]--;
        return -2;
    }
    else if(new->withdrawal > 0){
        officers[i] = 0;
    }

    add_service(list, new);
    return 0;
}

/*
 * Handle a new passenger covid test arrival (MULTI)
 *
 * conf: pointer to a configuration structure
 * arr: pointer to an arrival structure
 * list: list of service events
 *
 * return: 0 on success, negative value on error
 */
int handle_multi_test_arrival(config *conf, arrival *arr, service **list){
    int i;
    service *new;


    // Covid test arrival
    i = find_queue(test_queues, conf->test_off);

    if(test_officers[i] > 0){
        // Officer it's busy
        return -1;
    }

    test_officers[i] = 1; // Make busy
    test_queues[i]++;

    new = create_service(conf, arr->checkin_type, arr->test, arr->time, i);

    if(!new){
        perror("Passenger service it's lost!");
        test_officers[i] = 0;
        test_queues[i]--;
        return -2;
    }
    else if(new->withdrawal > 0){
        test_officers[i] = 0;
    }

    add_service(list, new);
    return 0;
}

/*
 * Handle a new passenger arrival (SITA)
 *
 * conf: pointer to a configuration structure
 * arr: pointer to an arrival structure
 * list: list of service events
 *
 * return: 0 on success, negative value on error
 */
int handle_sita_arrival(config *conf, arrival *arr, service **list){
    int i;
    service *new;

    if(arr->checkin_type == ONLINE){
        // Online test arrival
        i = find_officer(low_officers, conf->online_off);

        if(i < 0){
            // No officer available
            return -1;
        }

        low_officers[i] = 1; // Make busy
    }
    else if(arr->checkin_type == NATIONAL){
        // Online test arrival
        i = find_officer(med_officers, conf->online_off);

        if(i < 0){
            // No officer available
            return -1;
        }

        med_officers[i] = 1; // Make busy
    }
    else if(arr->checkin_type == INTERNATIONAL){
        // Online test arrival
        i = find_officer(high_officers, conf->online_off);

        if(i < 0){
            // No officer available
            return -1;
        }

        high_officers[i] = 1; // Make busy
    }
    else{
        perror("Unknown service type!");
        return -2;
    }

    if(clock < arr->time){
        clock = arr->time; // Update clock
    }

    new = create_service(conf, arr->checkin_type, arr->test, arr->time, i);

    if(!new){
        perror("Passenger service it's lost!");
        return -2;
    }
    else if(new->withdrawal > 0){
        if(new->checkin_type == ONLINE){
            low_officers[i] = 0;
        }
        else if (new->checkin_type == NATIONAL){
            med_officers[i] = 0;
        }
        else if(new->checkin_type == INTERNATIONAL){
            high_officers[i] = 0;
        }
        else{
            perror("Unknown service type!");
            return -2;
        }
    }

    add_service(list, new);
    return 0;
}

/*
 * Handle a new passenger arrival
 *
 * conf: pointer to a configuration structure
 * arr: pointer to an arrival structure
 * serv_list: list of service events
 * checkin: check-in configuration type
 * test: covid test configuration type
 *
 * return: 0 on success, negative value on error
 */
int handle_arrival(config *conf, arrival *arr, service **serv_list, char *checkin, char *test) {

    if(arr->test > 0){
        // Covid test arrival
        if(strcmp(test, SINGLE) == 0){
            return handle_single_test_arrival(conf, arr, serv_list);
        }
        else if(strcmp(test, MULTI) == 0){
            return handle_multi_test_arrival(conf, arr, serv_list);
        }
        else{
            perror("Unknown covid test configuration type");
            return -2;
        }
    }
    else{
        // Check-in arrival
        if(strcmp(checkin, SINGLE) == 0){
            return handle_single_arrival(conf, arr, serv_list);
        }
        else if(strcmp(checkin, MULTI) == 0){
            return handle_multi_arrival(conf, arr, serv_list);
        }
        else if(strcmp(checkin, SITA) == 0){
            return handle_sita_arrival(conf, arr, serv_list);
        }
        else{
            perror("Unknown check-in configuration type");
            return -2;
        }
    }
}

/*
 * Handle a new passenger service (SINGLE)
 *
 * serv: pointer to a service event
 * res: pointer to a result structure
 *
 * return: 0 on success, -1 on error
 */
int handle_single_service(service *serv, result *res){

    if(serv->withdrawal > 0){
        res->withdrawal++;
    }
    else{
        if(clock < serv->end){
            clock = serv->end; // Update clock
        }

        officers[serv->officer] = 0; // Free officer
        res->num_pass++;
        res->mwait += serv->start - serv->arrival;
        res->mresponse += serv->end - serv->arrival;
        res->mservice += serv->end - serv->start;
    }

    return 0;
}

/*
 * Handle a new passenger covid test service (SINGLE)
 *
 * conf: pointer to a configuration structure
 * serv: pointer to a service structure
 * list: pointer to a list of arrivals
 * res: pointer to a result structure
 *
 * return: 0 on success, -1 on error
 */
int handle_single_test_service(config *conf, service *serv, arrival **list, result *res){
    arrival *new;

    if(serv->withdrawal > 0){
        res->withdrawal_test++;
    }
    else{
        if(clock < serv->end){
            clock = serv->end; // Update clock
        }

        test_officers[serv->officer] = 0; // Free officer
        res->num_pass_test++;
        res->mwait_test += serv->start - serv->arrival;
        res->mresponse_test += serv->end - serv->arrival;
        res->mservice_test += serv->end - serv->start;
    }

    // Schedule new check-in arrival
    new = create_arrival(conf, serv->end + conf->testwait);
    new->test = 0;

    add_arrival(list, new);

    return 0;
}

/*
 * Handle a new passenger service (MULTI)
 *
 * serv: pointer to a service event
 * res: pointer to a result structure
 *
 * return: 0 on success, -1 on error
 */
int handle_multi_service(service *serv, result *res){
    // Check-in service
    queues[serv->officer]--;  // Reduce queue

    if(serv->withdrawal > 0){
        res->withdrawal++;
    }
    else{
        if(clock < serv->end){
            clock = serv->end; // Update clock
        }

        officers[serv->officer] = 0; // Free officer
        res->num_pass++;
        res->mwait += serv->start - serv->arrival;
        res->mresponse += serv->end - serv->arrival;
        res->mservice += serv->end - serv->start;
    }

    return 0;
}

/*
 * Handle a new passenger covid test service (MULTI)
 *
 * conf: pointer to a configuration structure
 * serv: pointer to a service structure
 * list: pointer to a list of arrivals
 * res: pointer to a result structure
 *
 * return: 0 on success, -1 on error
 */
int handle_multi_test_service(config *conf, service *serv, arrival **list, result *res){
    arrival *new;

    // Covid test service
    test_queues[serv->officer]--;  // Reduce queue

    if(serv->withdrawal > 0){
        res->withdrawal_test++;
    }
    else{
        if(clock < serv->end){
            clock = serv->end; // Update clock
        }

        test_officers[serv->officer] = 0; // Free officer
        res->num_pass_test++;
        res->mwait_test += serv->start - serv->arrival;
        res->mresponse_test += serv->end - serv->arrival;
        res->mservice_test += serv->end - serv->start;
    }

    // Schedule new check-in arrival
    new = create_arrival(conf, serv->end + conf->testwait);
    new->test = 0;

    add_arrival(list, new);


    return 0;
}

/*
 * Handle a new passenger service (SITA)
 *
* serv: pointer to a service event
 * res: pointer to a result structure
 *
 * return: 0 on success, -1 on error
 */
int handle_sita_service(service *serv, result *res){

    if(serv->checkin_type == ONLINE){

        if(serv->withdrawal > 0){
            res->withdrawal_small++;
            res->withdrawal++;
        }
        else{
            if(clock < serv->end){
                clock = serv->end; // Update clock
            }

            low_officers[serv->officer] = 0; // Free officer
            res->num_pass_small++;
            res->num_pass++;
            res->mwait_small += serv->start - serv->arrival;
            res->mresponse_small += serv->end - serv->arrival;
            res->mservice_small += serv->end - serv->start;

            res->mwait += serv->start - serv->arrival;
            res->mresponse += serv->end - serv->arrival;
            res->mservice += serv->end - serv->start;
        }

    }
    else if(serv->checkin_type == NATIONAL){

        if(serv->withdrawal > 0){
            res->withdrawal_med++;
            res->withdrawal++;
        }
        else{
            if(clock < serv->end){
                clock = serv->end; // Update clock
            }

            med_officers[serv->officer] = 0; // Free officer
            res->num_pass_med++;
            res->num_pass++;
            res->mwait_med += serv->start - serv->arrival;
            res->mresponse_med += serv->end - serv->arrival;
            res->mservice_med += serv->end - serv->start;

            res->mwait += serv->start - serv->arrival;
            res->mresponse += serv->end - serv->arrival;
            res->mservice += serv->end - serv->start;
        }
    }
    else if(serv->checkin_type == INTERNATIONAL){

        if(serv->withdrawal > 0){
            res->withdrawal_big++;
            res->withdrawal++;
        }
        else{
            if(clock < serv->end){
                clock = serv->end; // Update clock
            }

            high_officers[serv->officer] = 0; // Free officer
            res->num_pass_big++;
            res->num_pass++;
            res->mwait_big += serv->start - serv->arrival;
            res->mresponse_big += serv->end - serv->arrival;
            res->mservice_big += serv->end - serv->start;

            res->mwait += serv->start - serv->arrival;
            res->mresponse += serv->end - serv->arrival;
            res->mservice += serv->end - serv->start;
        }
    }
    else{
        perror("Unknown service type!");
        return -1;
    }

    return 0;
}

/*
 * Handle a new passenger service
 *
 * conf: pointer to a configuration structure
 * res: pointer to a result structure
 * serv: pointer to a service structure
 * arr_list: list of arrival events
 * checkin: check-in configuration type
 * test: covid test configuration type
 *
 * return: 0 on success, negative value on error
 */
int handle_service(config *conf, result *res, service *serv, arrival **arr_list, char *checkin, char *test) {

    if(serv->test > 0){
        // Covid test arrival
        if(strcmp(test, SINGLE) == 0){
            return handle_single_test_service(conf, serv, arr_list, res);
        }
        else if(strcmp(test, MULTI) == 0){
            return handle_multi_test_service(conf, serv, arr_list, res);
        }
        else{
            perror("Unknown covid test configuration type");
            return -2;
        }
    }
    else{
        // Check-in arrival
        if(strcmp(checkin, SINGLE) == 0){
            return handle_single_service(serv, res);
        }
        else if(strcmp(checkin, MULTI) == 0){
            return handle_multi_service(serv, res);
        }
        else if(strcmp(checkin, SITA) == 0){
            return handle_sita_service(serv, res);
        }
        else{
            perror("Unknown check-in configuration type");
            return -2;
        }
    }
}

// RESULT --------------------------------------------------------------------------------------------------------------

/*
 * Resets a result structure
 *
 * res: pointer to a result structure
 *
 * return: pointer to the new structure, NULL on error
 */
void reset_result(result *res){

    res->type = NULL;
    res->num_officers = 0;
    res->num_pass = 0;
    res->mwait = 0;
    res->mresponse = 0;
    res->mservice = 0;
    res->withdrawal = 0;

    // SITA only
    res->num_off_small = 0;
    res->num_pass_small = 0;
    res->mwait_small = 0;
    res->mresponse_small = 0;
    res->mservice_small = 0;
    res->withdrawal_small = 0;

    res->num_off_med = 0;
    res->num_pass_med = 0;
    res->mwait_med = 0;
    res->mresponse_med = 0;
    res->mservice_med = 0;
    res->withdrawal_med = 0;

    res->num_off_big = 0;
    res->num_pass_big = 0;
    res->mwait_big = 0;
    res->mresponse_big = 0;
    res->mservice_big = 0;
    res->withdrawal_big = 0;

    res->test_type = NULL;
    res->test_officers = 0;
    res->num_pass_test = 0;
    res->mwait_test = 0;
    res->mresponse_test = 0;
    res->mservice_test = 0;
    res->withdrawal_test = 0;

}

/*
 * Updates the values in the result structure after the new simulation
 *
 * conf: pointer to a configuration structure
 * res: pointer to a result structure
 * checkin: check-in configuration type
 * test: covid test configuration type
 *
 * return:
 */
void update_result(config *conf, result *res, char *checkin, char *test){

    res->type = checkin;
    res->num_officers = conf->off;
    res->mwait = res->mwait/res->num_pass;
    res->mservice = res->mservice/res->num_pass;
    res->mresponse = res->mresponse/res->num_pass;
    res->withdrawal = (res->withdrawal/(res->withdrawal + res->num_pass))*100;

    if(strcmp(checkin, SITA) == 0){
        res->num_off_small = conf->online_off;

        if(res->num_pass_small > 0){
            res->mwait_small = res->mwait_small/res->num_pass_small;
            res->mservice_small = res->mservice_small/res->num_pass_small;
            res->mresponse_small = res->mresponse_small/res->num_pass_small;
            res->withdrawal_small = (res->withdrawal_small/(res->withdrawal_small + res->num_pass_small))*100;
        }

        res->num_off_med = conf->national_off;

        if(res->num_pass_med > 0){
            res->mwait_med = res->mwait_med/res->num_pass_med;
            res->mservice_med = res->mservice_med/res->num_pass_med;
            res->mresponse_med = res->mresponse_med/res->num_pass_med;
            res->withdrawal_med = (res->withdrawal_med/(res->withdrawal_med + res->num_pass_med))*100;
        }

        res->num_off_big = conf->international_off;

        if(res->num_pass_big > 0){
            res->mwait_big = res->mwait_big/res->num_pass_big;
            res->mservice_big = res->mservice_big/res->num_pass_big;
            res->mresponse_big = res->mresponse_big/res->num_pass_big;
            res->withdrawal_big = (res->withdrawal_big/(res->withdrawal_big + res->num_pass_big))*100;
        }
    }

    res->test_type = test;
    res->test_officers = conf->test_off;

    if(res->num_pass_test > 0){
        res->mwait_test = res->mwait_test/res->num_pass_test;
        res->mservice_test = res->mservice_test/res->num_pass_test;
        res->mresponse_test = res->mresponse_test/res->num_pass_test;
        res->withdrawal_test = (res->withdrawal_test/(res->withdrawal_test + res->num_pass_test))*100;
    }
}

// SIMULATION ----------------------------------------------------------------------------------------------------------

/*
 * Simulate the given configuration
 *
 * conf: pointer to a configuration structure
 * res: pointer to a result structure
 * checkin: check-in area configuration type
 * test: test area configuration type
 *
 * return: pointer to a result structure, NULL on error
 */
result *simulate(config *conf, result *res, char *checkin, char *test){
    long last_arr;
    arrival *parr, *arr_list;
    service *pser, *serv_list;
    int sim_officers[conf->off];
    long sim_queues[conf->off];
    int sim_low_officers[conf->online_off];
    int sim_med_officers[conf->national_off];
    int sim_high_officers[conf->international_off];
    int sim_test_officers[conf->test_off];
    long sim_test_queues[conf->test_off];

    // Initialize
    init_simulation(conf->seed);
    memset(sim_officers, 0, sizeof(int)*conf->off);
    memset(sim_queues, 0, sizeof(long)*conf->off);
    memset(sim_low_officers, 0, sizeof(int)*conf->online_off);
    memset(sim_med_officers, 0, sizeof(int)*conf->national_off);
    memset(sim_high_officers, 0, sizeof(int)*conf->international_off);
    memset(sim_test_officers, 0, sizeof(int)*conf->test_off);
    memset(sim_test_queues, 0, sizeof(int)*conf->test_off);
    arr_list = NULL;
    serv_list = NULL;
    last_arr = 0;

    // Assign to global variables
    officers = sim_officers;
    queues = sim_queues;
    low_officers = sim_low_officers;
    med_officers = sim_med_officers;
    high_officers = sim_high_officers;
    test_officers = sim_test_officers;
    test_queues = sim_test_queues;

    // Reset result structure
    reset_result(res);

    while (clock <= conf->stop) {

        parr = arr_list;

        if(!parr){
            parr = create_arrival(conf, last_arr); // Generate new arrival
            add_arrival(&arr_list, parr);
            last_arr = parr->time;
        }

        while (handle_arrival(conf, parr, &serv_list, checkin, test) < 0) {

            if (serv_list) {
                pser = serv_list;
                handle_service(conf, res, pser, &arr_list, checkin, test);
                remove_service(&serv_list);
            } else {
                perror("Unable to handle new arrival even though an officer should be available");
                break;
            }

        }

        remove_arrival(&arr_list);
    }

    // Handle eventual remaining services
    while(serv_list){
        pser = serv_list;

        if(pser->end > conf->stop) break;

        handle_service(conf, res, pser, &arr_list, checkin, test);
        remove_service(&serv_list);
    }

    update_result(conf, res, checkin, test); // Update result structure with new data

    return res;
}