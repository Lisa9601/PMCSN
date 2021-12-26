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
            test_service = Exponential(1/conf->test_mu, 1);
        }
        else{
            test_service = -1;
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
 *
 * return: queue number
 */
int find_queue(element **queues, int queue_num, int *officers, passenger *pass){
    int i, num, pass_num, lowest_pass_num;
    element *e;

    num = -1;
    lowest_pass_num = 1000000; // High value

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
 *
 * return:
 */
void handle_arrival(int *officers, int off_num, passenger *p){
    int i;

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

    p->departure = p->begin + p->service;
    officers[i] = p->departure; // Update last departure time
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
    double mwait, mresponse;
    passenger *p;

    memset(officers, 0, sizeof(int)*off_num);

    num_pass = 0;
    num_withdrawal = 0;
    mwait = 0;
    mresponse = 0;

    for(p=pass; p!=NULL; p=p->next){
        // Handle passenger arrival
        handle_arrival(officers, off_num, p);

        if(p->begin - p->arrival > wait){
            // Withdrawal
            num_withdrawal++;
        }
        else{
            num_pass++;
            mwait += p->begin - p->arrival;
            mresponse += p->departure - p->arrival;
        }
    }

    // Add results
    res->type = SINGLE;
    res->num_queues = 1;
    res->num_officers = off_num;
    res->mwait = mwait/num_pass;
    res->mresponse = mresponse/num_pass;
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
    double mwait, mresponse;
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

    for(p=pass; p!=NULL; p=p->next){
        // Find shortest queue
        i = find_queue(queues, off_num, officers, p);
        // Add new element to queue
        e = (element *)malloc(sizeof(element));
        e->begin = p->begin;
        e->next = NULL;
        add_element(&queues[i], e);

        if(p->begin - p->arrival > wait){
            num_withdrawal++;
        }
        else{
            num_pass++;
            mwait += p->begin - p->arrival;
            mresponse += p->departure - p->arrival;
        }
    }

    // Cleanup element lists
    for(i=0; i<off_num; i++){
        free_elements(&queues[i]);
    }

    // Add results
    res->type = MULTI;
    res->num_queues = off_num;
    res->num_officers = off_num;
    res->mwait = mwait/num_pass;
    res->mresponse = mresponse/num_pass;
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
    passenger *p;

    memset(officers_small, 0, sizeof(int)*off_small);
    memset(officers_med, 0, sizeof(int)*off_med);
    memset(officers_big, 0, sizeof(int)*off_big);
    num_pass = 0;
    mwait = 0;
    mresponse = 0;
    withdrawal = 0;

    num_pass_small = 0;
    mwait_small = 0;
    mresponse_small = 0;
    withdrawal_small = 0;

    num_pass_med = 0;
    mwait_med = 0;
    mresponse_med = 0;
    withdrawal_med = 0;

    num_pass_big = 0;
    mwait_big = 0;
    mresponse_big = 0;
    withdrawal_big = 0;

    for(p=pass; p!=NULL; p=p->next){
        // Pick the right queue
        if(p->type == ONLINE){
            handle_arrival(officers_small, off_small, p);

            if(p->begin - p->arrival > wait){
                withdrawal++;
                withdrawal_small++;
            }
            else{
                num_pass++;
                num_pass_small++;
                mwait_small += p->begin - p->arrival;
                mresponse_small += p->departure - p->arrival;
            }
        }
        else if(p->type == NATIONAL){
            handle_arrival(officers_med, off_med, p);

            if(p->begin - p->arrival > wait){
                withdrawal++;
                withdrawal_med++;
            }
            else{
                num_pass++;
                num_pass_med++;
                mwait_med += p->begin - p->arrival;
                mresponse_med += p->departure - p->arrival;
            }
        }
        else{
            handle_arrival(officers_big, off_big, p);

            if(p->begin - p->arrival > wait){
                withdrawal++;
                withdrawal_big++;
            }
            else{
                num_pass++;
                num_pass_big++;
                mwait_big += p->begin - p->arrival;
                mresponse_big += p->departure - p->arrival;
            }
        }

        mwait += p->begin - p->arrival;
        mresponse += p->departure - p->arrival;
    }

    // Add results
    res->type = SITA;
    res->num_queues = 3;
    res->num_officers = off_small + off_med + off_big;
    res->mwait = mwait/num_pass;
    res->mresponse = mresponse/num_pass;
    res->withdrawal = (withdrawal/(double)(num_pass + withdrawal))*100;
    res->num_off_small = off_small;
    res->mwait_small = mwait_small/num_pass_small;
    res-> mresponse_small = mresponse_small/num_pass_small;
    res->withdrawal_small = (withdrawal_small/(double)(num_pass_small + withdrawal_small))*100;
    res->num_off_med = off_med;
    res->mwait_med = mwait_med/num_pass_med;
    res->mresponse_med = mresponse_med/num_pass_med;
    res->withdrawal_med = (withdrawal_med/(double)(num_pass_med + withdrawal_med))*100;
    res->num_off_big = off_big;
    res->mwait_big = mwait_big/num_pass_big;
    res->mresponse_big = mresponse_big/num_pass_big;
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
 *
 * return:
 */
void handle_covid_test(int *officers, int off_num, passenger *p, int wait){
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

    p->test_departure = p->test_begin + p->test_service;
    officers[i] = p->test_departure; // Update last departure time
    p->arrival = p->test_departure + wait;
    p->greenpass = 1;
}

/*
 * Find covid test queue with the lowest number of passengers
 *
 * queues: array of passenger queues
 * queue_num: number of queues in the array
 * officers: array with last departure time for each officer
 * pass: passenger
 * wait: time a passenger has to wait for the test results
 *
 * return: queue number
 */
int find_covid_test_queue(element **queues, int queue_num, int *officers, passenger *pass, int wait){
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

    pass->test_departure = pass->test_begin + pass->test_service;
    pass->arrival = pass->test_departure + wait;
    pass->greenpass = 1;
    officers[num] = pass->test_departure; // Update last departure time

    return num;
}

/*
 * Simulate the covid test queue as a single queue
 *
 * off_num: number of officers for the queue
 * pass: pointer to the first passenger in the list
 * res: pointer to a structure in which the results will be added
 * wait: how much time the passenger must wait for the test results
 *
 * return:
 */
void simulate_single_test(int off_num, passenger *pass, result *res, int wait){
    int num_test_pass, test_withdrawal;
    int officers[off_num];
    double test_wait, test_response;
    passenger *prev, *p;

    memset(officers, 0, sizeof(int)*off_num);
    num_test_pass = 0;
    test_withdrawal = 0;
    test_wait = 0;
    test_response = 0;
    prev = pass;

    for(p=pass; p!=NULL; p=p->next){
        if(p->greenpass == 0){
            // Passenger who needs a covid test found
            handle_covid_test(officers, off_num, p, wait);

            if(p->test_begin - p->test_arrival > wait){
                test_withdrawal++;
            }
            else{
                num_test_pass++;
                test_wait += p->test_begin - p->test_arrival;
                test_response += p->test_departure - p->test_arrival;
            }

            // Remove from queue
            prev->next = p->next;

            add_passenger(&pass, p); // Insert again in the correct place
            p = prev;
        }

        prev = p;
    }

    if(num_test_pass > 0){
        test_wait = test_wait/num_test_pass;
        test_response = test_response/num_test_pass;
    }

    res->test_type = SINGLE;
    res->test_queues = 1;
    res->test_officers = off_num;
    res->mwait_test = test_wait;
    res->mresponse_test = test_response;
}

/*
 * Simulate covid test multi queue
 *
 * off_num: number of officers
 * pass: list of passengers
 * res: structure where the simulation results will be added
 * wait: how much time the passenger must wait for the test results
 *
 * return:
 */
void simulate_multi_test(int off_num, passenger *pass, result *res, int wait){
    int i, num_test_pass;
    double mwait, mresponse;
    int officers[off_num];
    element *queues[off_num];
    element *e;
    passenger *prev, *p;

    memset(officers, 0, sizeof(int)*off_num);
    memset(queues, 0, sizeof(element *)*off_num);
    num_test_pass = 0;
    mwait = 0;
    mresponse = 0;
    prev = pass;

    for(p=pass; p!=NULL; p=p->next){
        if(p->greenpass == 0){
            // Find shortest queue
            i = find_covid_test_queue(queues, off_num, officers, p, wait);
            // Add new element to queue
            e = (element *)malloc(sizeof(element));
            e->begin = p->test_begin;
            e->next = NULL;
            add_element(&queues[i], e);

            num_test_pass++;
            mwait += p->test_begin - p->test_arrival;
            mresponse += p->test_departure - p->test_arrival;

            // Remove from queue
            prev->next = p->next;

            add_passenger(&pass, p); // Insert again in the correct place
            p = prev;
        }

        prev = p;
    }

    if(num_test_pass > 0){
        mwait = mwait/num_test_pass;
        mresponse = mresponse/num_test_pass;
    }

    for(i=0; i<off_num; i++){
        free_elements(&queues[i]);
    }

    res->test_type = MULTI;
    res->test_queues = off_num;
    res->test_officers = off_num;
    res->mwait_test = mwait;
    res->mresponse_test = mresponse;
}