#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "../include/simulation.h"
#include "../include/structs.h"
#include "../include/rngs.h"

/*
 * Generates a new random arrival of a passenger
 *
 * num: new passenger's number
 *
 * return: pointer to the new passenger, NULL on error
 */
passenger *generate_arrival(int num){
    int green_pass, check_type, check_arrival, check_service, test_arrival, test_service;
    passenger *new_pass;

    // Random green pass

    // Random check-in type

    // Exponential arrivals and services

    //new_pass = create_passenger(num, green_pass, check_type, test_arrival, test_service, check_arrival, check_service);

    return NULL;
}





/*
 * Setup a new single queue configuration
 *
 * name: name of the new configuration
 * check_off: number of check-in officers
 * test_off: number of covid test officers
 * max_check: max num of passengers allowed in the check-in queues
 * max_test: max num of passengers allowed in the test queues
 *
 * return: pointer to the new config structure, NULL in case of error
 */
config *setup_single_queue(char *name, int check_off, int test_off, int max_check, int max_test){
    int i;
    config *new_config;
    queue *new_queue;
    officer *p, *new_off;

    new_config = (config *)malloc(sizeof(config));

    if(new_config == NULL){
        perror();
        return NULL;
    }

    // Initializing configuration
    new_config->name = name;
    new_config->check_in_officer_num = check_off;
    new_config->covid_test_officer_num = test_off;
    new_config->max_check_in_queue = max_check;
    new_config->max_covid_test_queue = max_test;
    new_config->first_check_in_officer = NULL;
    new_config->first_covid_test_officer = NULL;

    // Creating single check-in queue
    new_queue = malloc(sizeof());


    // Creating check-in officers
    new_off = (officer *)malloc(sizeof(officer));

    if(new_off == NULL) continue;

    new_off->num = 0;
    new_off->queue = NULL;
    new_off->next = NULL;



    for(i=1; i<check_off; i++){

        new_off = (officer *)malloc(sizeof(officer));

        if(new_off == NULL) continue;

        new_off->num = i;
        new_off->queue = NULL;
        new_off->next = NULL;

    }

    // Creating covid test officers
    for(i=0; i<test_off; i++){

        // DA FARE

    }

    return new_config;
}

void simulate_single_queue(){

}