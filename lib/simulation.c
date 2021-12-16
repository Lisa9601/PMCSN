#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include "../include/simulation.h"
#include "../include/structs.h"
#include "../include/rngs.h"

#define SINGLE 0
#define MULTI 1
#define SITA 2

/*
 *  Exponential density function
 */
double Exponential(double m, int stream){
    SelectStream(stream);
    return -m*log(1.0 -Random());
}

/*
 * Generates a new random arrival of a passenger
 *
 * num: new passenger's number
 * curr_time: variable that keeps track of time
 * stop: end time of the simulation
 *
 * return: pointer to the new passenger, NULL on error
 */
passenger *generate_arrival(int num, int *curr_time, int stop){
    int green_pass, check_type, check_arrival, check_service, test_arrival, test_service;
    passenger *new_pass;

    *curr_time += Exponential(1/(double)LAMBDA, 0); // Update time

    if(*curr_time > stop){
        // Can't generate new passengers after simulation has ended
        return NULL;
    }

    // Green pass
    if(Random() < GREEN_P){
        green_pass = 1;
        test_arrival = 0;
        test_service = 0;
        check_arrival = *curr_time;

    }
    else{
        green_pass = 0;
        test_arrival = *curr_time;
        test_service = Exponential(1/(double)TEST_MEAN, 1);
        check_arrival = 0;
    }

    // Check-in type
    if(Random() < ONLINE_P){
        check_type = ONLINE;
        check_service = Exponential(1/(double)ONLINE_MEAN, 2)
    }
    else if(Random() < NATIONAL_P){
        check_type = NATIONAL;
        check_service = Exponential(1/(double)NATIONAL_MEAN, 2)
    }
    else{
        check_type = INTERNATIONAL;
        check_service = Exponential(1/(double)INTERNATIONAL_MEAN, 2)
    }

    return create_passenger(num, green_pass, check_type, test_arrival, test_service, check_arrival, check_service);
}

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
        if(dep < officers[i]){
            dep = officers[i];
            num = i;
        }
    }

    return num;
}

/*
 * Find queue with the lowest number of passengers
 *
 * queues: array of passengers' queues
 * queue_num: number of queues in the array
 * officers: array with last departure time for each officer
 * pass: passenger
 *
 * return: queue number
 */
int find_queue(passenger **queues, int queue_num, int *officers, passenger *pass){
    int i, num, pass_num, lowest_pass_num;
    passenger *p;

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
        for(p=queues[i]; p!=NULL; p=p->next){
            if(p->begin > pass->arrival){
                // Passenger inside the queue at the moment of the new arrival
                pass_num++;
            }
        }

        if(pass_num < lowest_pass_num){
            lowest_pass_num = pass_num;
            num = i;
        }
    }

    pass->begin = officers[num];
    pass->departure = pass->begin + pass->service;

    return num;
}

/*
 * Handle a new passenger arrival for the covid test
 *
 * officers: array with the last departure time for each officer
 * off_num: total number of officers for the queue
 * p: passenger
 * wait: time to wait for the test results
 *
 * return:
 */
void handle_test_arrival(int *officers, int off_num, passenger *p, int wait){
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
    p->arrival = p->test_departure + wait;
    officers[i] = p->test_departure; // Update last departure time
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

// SIMULATIONS ---------------------------------------------------------------------------------------------------------

/*
 * Simulate covid test queues
 *
 * off_num: number of officers
 * pass: list of passengers
 * res: structure where the simulation results will be added
 * test_wait: time to wait for the test results
 *
 * return:
 */
void simulate_covid_test(int off_num, passenger *pass, result *res, int test_wait){
    int num_pass;
    int officers[off_num] = {0};
    double mwait, mresponse;
    passenger *p;

    num_pass = 0;
    mwait = 0;
    mresponse = 0;

    for(p=pass; p!=NULL; p=p->next){
        // Handle passenger arrival
        handle_test_arrival(officers, off_num, p, test_wait);

        num_pass++;
        mwait += p->test_begin - p->test_arrival;
        mresponse += p->test_departure - p->test_arrival;
    }

    mwait = mwait/num_pass;
    mresponse = mresponse/num_pass;

    // Add results
    res->test_queues = 1;
    res->test_officers = off_num;
    res->mwait_test = mwait;
    res->mresponse_test = mresponse;
}

/*
 * Simulate a single queue
 *
 * off_num: number of officers
 * pass: list of passengers
 * res: structure where the simulation results will be added
 *
 * return:
 */
void simulate_single_queue(int off_num, passenger *pass, result *res){
    int num_pass;
    int officers[off_num] = {0};
    double mwait, mresponse;
    passenger *p;

    num_pass = 0;
    mwait = 0;
    mresponse = 0;

    for(p=pass; p!=NULL; p=p->next){
        // Handle passenger arrival
        handle_arrival(officers, off_num, p);

        num_pass++;
        mwait += p->begin - p->arrival;
        mresponse += p->departure - p->arrival;
    }

    mwait = mwait/num_pass;
    mresponse = mresponse/num_pass;

    // Add results
    res->type = SINGLE;
    res->num_queues = 1;
    res->num_officers = off_num;
    res->mwait = mwait;
    res->mresponse = mresponse;
}

/*
 * Simulate multi queue
 *
 * off_num: number of officers
 * pass: list of passengers
 * res: structure where the simulation results will be added
 *
 * return:
 */
void simulate_multi_queue(int off_num, passenger *pass, result *res){
    int i, num_pass;
    double mwait, mresponse;
    int officers[off_num] = {0};
    passenger *queues[off_num] = {NULL};
    passenger *p;

    num_pass = 0;
    mwait = 0;
    mresponse = 0;

    for(p=pass; p!=NULL; p=p->next){
        // Find shortest queue
        i = find_queue(queues, off_num, officers, p);

        // Add passenger to queue
        queues[i] = add_passenger(queues[i], p);

        mwait += p->begin - p->arrival;
        mresponse += p->departure - p->arrival;
    }

    mwait = mwait/num_pass;
    mresponse = mresponse/num_pass;

    // Add results
    res->type = MULTI;
    res->num_queues = off_num;
    res->num_officers = off_num;
    res->mwait = mwait;
    res->mresponse = mresponse;
}

/*
 * Simulate sita queue
 *
 * off_small: number of officers for small jobs
 * off_med: number of officers for medium jobs
 * off_big: number of officers for big jobs
 * pass: list of passengers
 * res: structure where the simulation results will be added
 *
 * return:
 */
void simulate_sita_queue(int off_small, int off_med, int off_big, passenger *pass, result *res){
    int i, num_pass, num_pass_small, num_pass_med, num_pass_big;
    int officers_small[off_small] = {0};
    int officers_med[off_med] = {0};
    int officers_big[off_big] = {0};
    double mwait, mwait_small, mwait_med, mwait_big;
    double mresponse, mresponse_small, mresponse_med, mresponse_big;
    passenger *p;

    num_pass = 0;
    mwait = 0;
    mwait_small = 0;
    mwait_med = 0;
    mwait_big = 0;
    mresponse = 0;
    mresponse_small = 0;
    mresponse_med = 0;
    mresponse_big = 0;

    for(p=pass; p!=NULL; p=p->next){

        // Pick the right queue
        if(p->check_in == ONLINE){

            handle_arrival(officers_small, off_small, p);

            num_pass_small++;
            mwait_small += p->begin - p->arrival;
            mresponse_small += p->departure - p->arrival;
        }
        else if(p->check_in == NATIONAL){
            handle_arrival(officers_med, off_med, p);

            num_pass_med++;
            mwait_med += p->begin - p->arrival;
            mresponse_med += p->departure - p->arrival;
        }
        else{
            handle_arrival(officers_big, off_big, p);

            num_pass_big++;
            mwait_big += p->begin - p->arrival;
            mresponse_big += p->departure - p->arrival;
        }

        num_pass++;
        mwait += p->begin - p->arrival;
        mresponse += p->departure - p->arrival;
    }

    mwait = mwait/num_pass;
    mwait_small = mwait_small/num_pass_small;
    mwait_med = mwait_med/num_pass_med;
    mwait_big = mwait_big/num_pass_big;

    mresponse = mresponse/num_pass;
    mresponse_small = mresponse_small/num_pass_small;
    mresponse_med = mresponse_med/num_pass_med;
    mresponse_big = mresponse_big/num_pass_big;

    // Add results
    res->type = SITA;
    res->num_queues = 3;
    res->num_officers = off_small + off_med + off_big;
    res->mwait = mwait;
    res->mresponse = mresponse;
    res->num_off_small = off_small;
    res->num_off_med = off_med;
    res->num_off_big = off_big;
    res->mwait_small = mwait_small;
    res->mwait_med = mwait_med;
    res->mwait_big = mwait_big;
    res-> mresponse_small = mresponse_small;
    res->mresponse_med = mresponse_med;
    res->mresponse_big = mresponse_big;
}

/*
 * Start a new simulation
 */
result *start_simulation(int type,){

}