#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>
#include <string.h>
#include "../include/simulation.h"
#include "../include/rngs.h"

/*
 * Initialize simulation with the given seed
 *
 * seed: seed
 *
 * return:
 */
void init_simulation(long seed){
    PlantSeeds(seed);
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

// ELEMENT -------------------------------------------------------------------------------------------------------------

typedef struct element{
    int begin;
    struct element *next;
} element;

/*
 * Creates a new element
 *
 * begin: time at which the passenger begins service
 *
 * return: pointer to the newly created element, NULL on error
 */
element *create_element(int begin){
    element  *new_elem;

    new_elem = (element *)malloc(sizeof(element));

    if(!new_elem){
        perror("Element creation error");
        return NULL;
    }

    new_elem->begin = begin;
    new_elem->next = NULL;
    
    return new_elem;
}

/*
 * Add a new element to the given list
 *
 * head: head of the element list
 * new_elem: new element to be added to the list
 *
 * return:
 */
void add_element(element **head, element *new_elem){
    if(*head == NULL){
        *head = new_elem;
        return;
    }

    new_elem->next = *head; // Add as first element
    *head = new_elem;
}

/*
 * Cleanup all elements in the list
 *
 * head: head of the list of elements
 *
 * return:
 */
void free_elements(element **head){
    element *prev, *e;

    prev = e = *head;

    while(e != NULL){
        e = e->next;
        free(prev);
        prev = e;
    }
}

// PASSENGER -----------------------------------------------------------------------------------------------------------

/*
 * Creates a new passenger
 *
 * type: passenger type
 * arrival: passenger arrival time
 * check_service: check-in service time
 * test_service: covid test service time
 *
 * return: A pointer to the new passenger, NULL on error
 */
passenger *create_passenger(int type, int arrival, int check_service, int test_service){
    passenger *new_passenger;

    new_passenger = (passenger *)malloc(sizeof(passenger));
    if(!new_passenger){
        perror("Passenger creation error");
        return NULL;
    }

    // Initializing passenger
    new_passenger->type = type;
    new_passenger->arrival = arrival; // If there's no covid test area all are considered as check-in arrivals

    if(test_service != -1){
        // Passenger needs the covid test
        new_passenger->greenpass = 0;
        new_passenger->test_arrival = arrival;
    }
    else{
        new_passenger->greenpass = 1;
        new_passenger->test_arrival = -1;
    }

    new_passenger->begin = 0;
    new_passenger->service = check_service;
    new_passenger->departure = 0;
    new_passenger->test_begin = 0;
    new_passenger->test_service = test_service;
    new_passenger->test_departure = 0;
    new_passenger->next = NULL;

    return new_passenger;
}

/*
 * Adds a new passenger to the given list ordered by arrival time
 *
 * head: head of the list
 * new_pass: new passenger
 *
 * return:
 */
void add_passenger(passenger **head, passenger *new_pass){
    passenger *prev, *curr;

    // No elements in the list
    if(*head == NULL){
        *head = new_pass;
        return;
    }

    // First element should be after the new passenger
    if((*head)->arrival > new_pass->arrival){
        new_pass->next = *head;
        *head = new_pass;
        return;
    }

    prev = *head;

    // Find correct place inside list
    for(curr=prev->next; curr!=NULL; curr=curr->next){
        if(curr->arrival > new_pass->arrival){
            new_pass->next = curr;
            prev->next = new_pass;
            return;
        }
        prev = curr;
    }

    prev->next = new_pass;
    new_pass->next = NULL;
}

/*
 * Removes the given passenger list
 *
 * pass: head of the list of passengers
 *
 * return:
 */
void free_passengers(passenger **pass){
    passenger *prev, *p;

    prev = p = *pass;

    while(p != NULL){
        p = p->next;
        free(prev);
        prev = p;
    }
}

/*
 * Reorder the passengers in the list
 *
 * head: head of the list of passengers
 *
 * return:
 */
void reorder_passengers(passenger **head){
    passenger *prev, *p, *temp;

    // Empty list
    if(!head) return;

    prev = NULL;
    p = *head;

    while(p){

        if(p->next != NULL && (p->arrival > (p->next)->arrival)){
            temp = p;
            p = p->next;

            if(prev == NULL){
                *head = p;
            }
            else{
                prev->next = p;
            }
            add_passenger(head, temp);

        }
        else{
            prev = p;
            p = p->next;
        }
    }
}

/*
 * Generates a list of random arrivals of passengers
 *
 * conf: pointer to a configuration structure
 * stop: end time of the simulation
 * list: head of the list of passengers
 *
 * return:
 */
void generate_arrivals(config * conf, int stop, passenger **list){
    int check_type, test_service, check_service;
    double curr_time, random;
    passenger *new_pass;

    curr_time = 0; // Initialize current time to zero

    while(curr_time < stop){
        curr_time += Exponential(1/conf->lambda, 0); // Update time

        // Green pass
        random = Random_num(2);
        if(random < conf->greenpass_p){
            test_service = -1;
        }
        else{
            test_service = Exponential(1/conf->test_mu, 1);
        }

        // Check-in type
        random = Random_num(3);
        if(random < conf->online_p){
            check_type = ONLINE;
            check_service = Exponential(1/conf->online_mu, 1);
        }
        else if(random < conf->online_p + conf->national_p){
            check_type = NATIONAL;
            check_service = Exponential(1/conf->national_mu, 1);
        }
        else{
            check_type = INTERNATIONAL;
            check_service = Exponential(1/conf->international_mu, 1);
        }

        new_pass = create_passenger(check_type, (int)curr_time, check_service, test_service);
        add_passenger(list, new_pass); // Add new passenger to the list
    }
}

// CHECK-IN AREA -------------------------------------------------------------------------------------------------------

/*
 * Find officer with the lowest departure time
 *
 * officers: array with last departure times for each officer
 * off_num: total number of officers for the queue
 *
 * return: officer number
 */
int find_officer(int *officers, int off_num){
    int i, num, dep;

    num = -1;
    dep = 1000000;  // High value

    for(i=0; i<off_num; i++){
        if(dep > officers[i]){
            dep = officers[i];
            num = i;
        }
    }

    return num;
}

/*
 * Find queue with the lowest number of passengers
 *
 * queues: array of passenger queues
 * queue_num: number of queues in the array
 * officers: array with last departure time for each officer
 * pass: passenger
 * wait: maximum waiting time in the queues
 *
 * return: queue number, -1 for withdrawal
 */
int find_queue(element **queues, int queue_num, int *officers, passenger *pass, int wait){
    int i, num, pass_num, lowest_pass_num;
    element *e;

    num = -1;
    lowest_pass_num = 1000000; // High value

    // Passenger has already left
    if(pass->arrival < 0){
        return -1;
    }

    for(i=0; i<queue_num; i++){
        if(queues[i] == NULL){
            // No passenger have used this queue yet
            pass->begin = pass->arrival;
            pass->departure = pass->begin + pass->service;
            officers[i] = pass->departure; // Update last departure time
            return i;
        }

        pass_num = 0;
        for(e=queues[i]; e!=NULL; e=e->next){
            if(e->begin > pass->arrival){
                // Passenger inside the queue at the moment of the new arrival
                pass_num++;
            }
        }

        if(pass_num < lowest_pass_num){
            lowest_pass_num = pass_num;
            num = i;
        }
    }

    if(officers[num] < pass->arrival){
        // New arrival after departure
        pass->begin = pass->arrival;
    }
    else{
        // New arrival before departure
        pass->begin = officers[num];
    }

    // Passenger withdrawal
    if(pass->begin - pass->arrival > wait){
        return -1;
    }

    pass->departure = pass->begin + pass->service;
    officers[num] = pass->departure; // Update last departure time

    return num;
}

/*
 * Handle a new passenger arrival
 *
 * officers: array with the last departure time for each officer
 * off_num: total number of officers for the queue
 * p: passenger
 * wait: maximum wait in the queues
 *
 * return: 0 if successful, -1 if passenger has withdrawn
 */
int handle_arrival(int *officers, int off_num, passenger *p, int wait){
    int i;

    // Passenger has already left
    if(p->arrival < 0){
        return -1;
    }

    // Find available officer
    i = find_officer(officers, off_num);

    if(officers[i] < p->arrival){
        // New arrival after departure
        p->begin = p->arrival;
    }
    else{
        // New arrival before departure
        p->begin = officers[i];
    }

    // Passenger withdrawal
    if(p->begin - p->arrival > wait){
        return -1;
    }

    p->departure = p->begin + p->service;
    officers[i] = p->departure; // Update last departure time

    return 0;
}

/*
 * Simulate a single queue
 *
 * off_num: number of officers
 * wait: maximum wait in the queue
 * pass: list of passengers
 * res: structure where the simulation results will be added
 *
 * return:
 */
void simulate_single_queue(int off_num, int wait, passenger *pass, result *res){
    int num_pass, num_withdrawal;
    int officers[off_num];
    double mwait, mresponse, mservice;
    passenger *p;

    memset(officers, 0, sizeof(int)*off_num);

    num_pass = 0;
    num_withdrawal = 0;
    mwait = 0;
    mresponse = 0;
    mservice = 0;

    for(p=pass; p!=NULL; p=p->next){
        // Handle passenger arrival
        if(handle_arrival(officers, off_num, p, wait) < 0){
            num_withdrawal++;
        }
        else{
            num_pass++;
            mwait += p->begin - p->arrival;
            mresponse += p->departure - p->arrival;
            mservice += p->service;
        }
    }

    // Add results
    res->type = SINGLE;
    res->num_officers = off_num;
    res->mwait = mwait/num_pass;
    res->mresponse = mresponse/num_pass;
    res->mservice = mservice/num_pass;
    res->withdrawal = (num_withdrawal/(double)(num_pass + num_withdrawal))*100;
}

/*
 * Simulate multi queue
 *
 * off_num: number of officers
 * wait: maximum wait in the queue
 * pass: list of passengers
 * res: structure where the simulation results will be added
 *
 * return:
 */
void simulate_multi_queue(int off_num, int wait, passenger *pass, result *res){
    int i, num_pass, num_withdrawal;
    double mwait, mresponse, mservice;
    int officers[off_num];
    element *queues[off_num];
    element *e;
    passenger *p;

    memset(officers, 0, sizeof(int)*off_num);
    memset(queues, 0, sizeof(element *)*off_num);
    num_withdrawal = 0;
    num_pass = 0;
    mwait = 0;
    mresponse = 0;
    mservice = 0;

    for(p=pass; p!=NULL; p=p->next){
        // Find shortest queue
        i = find_queue(queues, off_num, officers, p, wait);

        if(i < 0){
            num_withdrawal++;
        }
        else{
            // Add new element to queue
            e = (element *)malloc(sizeof(element));
            e->begin = p->begin;
            e->next = NULL;
            add_element(&queues[i], e);

            num_pass++;
            mwait += p->begin - p->arrival;
            mresponse += p->departure - p->arrival;
            mservice += p->service;
        }
    }

    // Cleanup element lists
    for(i=0; i<off_num; i++){
        free_elements(&queues[i]);
    }

    // Add results
    res->type = MULTI;
    res->num_officers = off_num;
    res->mwait = mwait/num_pass;
    res->mresponse = mresponse/num_pass;
    res->mservice = mservice/num_pass;
    res->withdrawal = (num_withdrawal/(double)(num_pass + num_withdrawal))*100;
}

/*
 * Simulate sita queue
 *
 * off_small: number of officers for small jobs
 * off_med: number of officers for medium jobs
 * off_big: number of officers for big jobs
 * wait: maximum wait in the queue
 * pass: list of passengers
 * res: structure where the simulation results will be added
 *
 * return:
 */
void simulate_sita_queue(int off_small, int off_med, int off_big, int wait, passenger *pass, result *res){
    int num_pass, num_pass_small, num_pass_med, num_pass_big;
    int withdrawal, withdrawal_small, withdrawal_med, withdrawal_big;
    int officers_small[off_small];
    int officers_med[off_med];
    int officers_big[off_big];
    double mwait, mwait_small, mwait_med, mwait_big;
    double mresponse, mresponse_small, mresponse_med, mresponse_big;
    double mservice, mservice_small, mservice_med, mservice_big;
    passenger *p;

    memset(officers_small, 0, sizeof(int)*off_small);
    memset(officers_med, 0, sizeof(int)*off_med);
    memset(officers_big, 0, sizeof(int)*off_big);
    num_pass = 0;
    mwait = 0;
    mresponse = 0;
    mservice = 0;
    withdrawal = 0;

    num_pass_small = 0;
    mwait_small = 0;
    mresponse_small = 0;
    mservice_small = 0;
    withdrawal_small = 0;

    num_pass_med = 0;
    mwait_med = 0;
    mresponse_med = 0;
    mservice_med = 0;
    withdrawal_med = 0;

    num_pass_big = 0;
    mwait_big = 0;
    mresponse_big = 0;
    mservice_big = 0;
    withdrawal_big = 0;

    for(p=pass; p!=NULL; p=p->next){
        // Pick the right queue
        if(p->type == ONLINE){
            if(handle_arrival(officers_small, off_small, p, wait) < 0){
                withdrawal++;
                withdrawal_small++;
            }
            else{
                num_pass++;
                num_pass_small++;
                mwait_small += p->begin - p->arrival;
                mresponse_small += p->departure - p->arrival;
                mservice_small += p->service;
                mwait += p->begin - p->arrival;
                mresponse += p->departure - p->arrival;
                mservice += p->service;
            }
        }
        else if(p->type == NATIONAL){
            if(handle_arrival(officers_med, off_med, p, wait) < 0){
                withdrawal++;
                withdrawal_med++;
            }
            else{
                num_pass++;
                num_pass_med++;
                mwait_med += p->begin - p->arrival;
                mresponse_med += p->departure - p->arrival;
                mservice_med += p->service;
                mwait += p->begin - p->arrival;
                mresponse += p->departure - p->arrival;
                mservice += p->service;
            }
        }
        else{
            if(handle_arrival(officers_big, off_big, p, wait) < 0){
                withdrawal++;
                withdrawal_big++;
            }
            else{
                num_pass++;
                num_pass_big++;
                mwait_big += p->begin - p->arrival;
                mresponse_big += p->departure - p->arrival;
                mservice_big += p->service;
                mwait += p->begin - p->arrival;
                mresponse += p->departure - p->arrival;
                mservice += p->service;
            }
        }
    }

    // Add results
    res->type = SITA;
    res->num_officers = off_small + off_med + off_big;
    res->mwait = mwait/num_pass;
    res->mresponse = mresponse/num_pass;
    res->mservice = mservice/num_pass;
    res->withdrawal = (withdrawal/(double)(num_pass + withdrawal))*100;
    res->num_off_small = off_small;
    res->mwait_small = mwait_small/num_pass_small;
    res->mresponse_small = mresponse_small/num_pass_small;
    res->mservice_small = mservice_small/num_pass_small;
    res->withdrawal_small = (withdrawal_small/(double)(num_pass_small + withdrawal_small))*100;
    res->num_off_med = off_med;
    res->mwait_med = mwait_med/num_pass_med;
    res->mresponse_med = mresponse_med/num_pass_med;
    res->mservice_med = mservice_med/num_pass_med;
    res->withdrawal_med = (withdrawal_med/(double)(num_pass_med + withdrawal_med))*100;
    res->num_off_big = off_big;
    res->mwait_big = mwait_big/num_pass_big;
    res->mresponse_big = mresponse_big/num_pass_big;
    res->mservice_big = mservice_big/num_pass_big;
    res->withdrawal_big = (withdrawal_big/(double)(num_pass_big + withdrawal_big))*100;
}

// COVID TEST AREA -----------------------------------------------------------------------------------------------------

/*
 * Handle a new passenger's covid test
 *
 * officers: array with the last departure time for each officer
 * off_num: total number of officers for the queue
 * p: passenger
 * wait: time a passenger has to wait for the test results
 * positive: probability of being positive to the test
 *
 * return: 0 if successful, -1 if passenger has withdrawn
 */
int handle_covid_test(int *officers, int off_num, passenger *p, int wait, double positive){
    int i;

    // Find available officer
    i = find_officer(officers, off_num);

    if(officers[i] < p->test_arrival){
        // New arrival after departure
        p->test_begin = p->test_arrival;
    }
    else{
        // New arrival before departure
        p->test_begin = officers[i];
    }

    // Passenger withdrawal
    if(p->test_begin - p->test_arrival > wait){
        p->arrival = -1;
        return -1;
    }

    p->test_departure = p->test_begin + p->test_service;
    officers[i] = p->test_departure; // Update last departure time

    if(Random_num(3) < positive){
        // Passenger it's positive
        p->arrival = -1;
        return -1;
    }

    p->arrival = p->test_departure + wait;
    p->greenpass = 1;

    return 0;
}

/*
 * Find covid test queue with the lowest number of passengers
 *
 * queues: array of passenger queues
 * queue_num: number of queues in the array
 * officers: array with last departure time for each officer
 * pass: passenger
 * wait: time a passenger has to wait for the test results
 * positive: probability of being positive to the covid test
 *
 * return: queue number, -1 for withdrawal
 */
int find_covid_test_queue(element **queues, int queue_num, int *officers, passenger *pass, int wait, double positive){
    int i, num, pass_num, lowest_pass_num;
    element *e;

    num = -1;
    lowest_pass_num = 1000000; // High value

    for(i=0; i<queue_num; i++){
        if(queues[i] == NULL){
            // No passenger have used this queue yet
            pass->test_begin = pass->test_arrival;
            pass->test_departure = pass->test_begin + pass->test_service;
            pass->arrival = pass->test_departure + wait;

            if(Random_num(3) < positive){
                pass->arrival = -1;
                return -1;
            }

            pass->greenpass = 1;
            officers[i] = pass->test_departure; // Update last departure time
            return i;
        }

        pass_num = 0;
        for(e=queues[i]; e!=NULL; e=e->next){
            if(e->begin > pass->test_arrival){
                // Passenger inside the queue at the moment of the new arrival
                pass_num++;
            }
        }

        if(pass_num < lowest_pass_num){
            lowest_pass_num = pass_num;
            num = i;
        }
    }

    if(officers[num] < pass->test_arrival){
        // New arrival after departure
        pass->test_begin = pass->test_arrival;
    }
    else{
        // New arrival before departure
        pass->test_begin = officers[num];
    }

    // Passenger withdrawal
    if(pass->begin - pass->arrival > wait){
        pass->arrival = -1;
        return -1;
    }

    pass->test_departure = pass->test_begin + pass->test_service;
    pass->arrival = pass->test_departure + wait;

    if(Random_num(3) < positive){
        pass->arrival = -1;
        return -1;
    }

    pass->greenpass = 1;
    officers[num] = pass->test_departure; // Update last departure time

    return num;
}

/*
 * Simulate the covid test queue as a single queue
 *
 * off_num: number of officers for the queue
 * pass: head of the list of passengers
 * res: pointer to a structure in which the results will be added
 * wait: how much time the passenger must wait for the test results
 * positive: probability of being positive to the covid test
 *
 * return:
 */
void simulate_single_test(int off_num, passenger **pass, result *res, int wait, double positive){
    int num_test_pass;
    int officers[off_num];
    double test_wait, test_response, test_service, withdrawal;
    passenger *prev, *p, *temp;

    memset(officers, 0, sizeof(int)*off_num);
    num_test_pass = 0;
    test_wait = 0;
    test_response = 0;
    test_service = 0;
    withdrawal = 0;
    prev = NULL;
    p = *pass;

    while(p){
        if(p->greenpass == 0){
            // Passenger who needs a covid test found
            if(handle_covid_test(officers, off_num, p, wait, positive) >= 0){
                num_test_pass++;
                test_wait += p->test_begin - p->test_arrival;
                test_response += p->test_departure - p->test_arrival;
                test_service += p->test_service;

                // Remove from queue and insert again in the correct place
                temp = p;
                p = p->next;

                if(!prev){
                    *pass = p;
                }
                else{
                    prev->next = p;
                }

                add_passenger(pass, temp);
            }
            else{
                withdrawal++;
            }
        }
        else{
            prev = p;
            p = p->next;
        }
    }

    if(num_test_pass > 0){
        test_wait = test_wait/num_test_pass;
        test_response = test_response/num_test_pass;
        test_service = test_service/num_test_pass;
        withdrawal = (withdrawal/(withdrawal+num_test_pass))*100;
    }

    res->test_type = SINGLE;
    res->test_officers = off_num;
    res->mwait_test = test_wait;
    res->mresponse_test = test_response;
    res->mservice_test = test_service;
    res->withdrawal_test = withdrawal;
}

/*
 * Simulate covid test multi queue
 *
 * off_num: number of officers
 * pass: head of the passenger list
 * res: structure where the simulation results will be added
 * wait: how much time the passenger must wait for the test results
 * positive: probability of being positive to the covid test
 *
 * return:
 */
void simulate_multi_test(int off_num, passenger **pass, result *res, int wait, double positive){
    int i, num_test_pass;
    double mwait, mresponse, mservice, withdrawal;
    int officers[off_num];
    element *queues[off_num];
    element *e;
    passenger *prev, *p, *temp;

    memset(officers, 0, sizeof(int)*off_num);
    memset(queues, 0, sizeof(element *)*off_num);
    num_test_pass = 0;
    mwait = 0;
    mresponse = 0;
    mservice = 0;
    withdrawal = 0;
    prev = NULL;
    p = *pass;

    while(p){
        if(p->greenpass == 0){
            // Find shortest queue
            i = find_covid_test_queue(queues, off_num, officers, p, wait, positive);

            if(i >= 0){
                // Add new element to queue
                e = (element *)malloc(sizeof(element));
                e->begin = p->test_begin;
                e->next = NULL;
                add_element(&queues[i], e);

                num_test_pass++;
                mwait += p->test_begin - p->test_arrival;
                mresponse += p->test_departure - p->test_arrival;
                mservice += p->test_service;

                // Remove from queue and insert again in the correct place
                temp = p;
                p = p->next;

                if(!prev){
                    *pass = p;
                }
                else{
                    prev->next = p;
                }

                add_passenger(pass, temp);
            }
        }
        else{
            withdrawal++;
            prev = p;
            p = p->next;
        }
    }

    if(num_test_pass > 0){
        mwait = mwait/num_test_pass;
        mresponse = mresponse/num_test_pass;
        mservice = mservice/num_test_pass;
        withdrawal = (withdrawal/(withdrawal+num_test_pass))*100;
    }

    for(i=0; i<off_num; i++){
        free_elements(&queues[i]);
    }

    res->test_type = MULTI;
    res->test_officers = off_num;
    res->mwait_test = mwait;
    res->mresponse_test = mresponse;
    res->mservice_test = mservice;
    res->withdrawal_test = withdrawal;
}