#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>
#include "../../include/simulation.h"
#include "../../include/structs.h"
#include "../../include/rngs.h"
#include "../../config.h"

#define ARRIVAL 0
#define DEPARTURE 1

typedef struct event{

    int time;
    int type;
    int queue;
    passenger *pass;
    struct event *next;

} event;


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
 * return: pointer to the new arrival event, NULL on error
 */
event *generate_arrival(int num, int *curr_time, int stop){
    int green_pass, check_type, check_arrival, check_service, test_arrival, test_service;
    passenger *new_pass;
    event *e;

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

    new_pass = create_passenger(num, green_pass, check_type, test_arrival, test_service, check_arrival, check_service);

    if(new_pass == NULL) return NULL;

    e = (event *)malloc(sizeof(event));
    if(e == NULL){
        free(new_pass);
        return NULL;
    }

    e->time = *curr_time;
    e->type = ARRIVAL;
    e->queue = -1;
    e->pass = new_pass;
    e->next = NULL;

    return e;
}

/*
 * Generate new departure
 *
 * time: departure time of a passenger
 * queue: number of the queue to which the passenger was assigned
 *
 * return: pointer to the new departure event, NULL on error
 */
event *generate_departure(int time, int queue){
    event *e;

    e = (event *)malloc(sizeof(event));
    if(e != NULL){
        e->time = time;
        e->type = DEPARTURE;
        e->queue = queue;
        e->pass = NULL;
        e->next = NULL;
    }
}

/*
 * Adds a new event to the given list
 *
 * start: first element of the list
 * new_event: new event
 *
 * return: the start of the list with the new event
 */
event *add_event(event *start, event *new_event){
    event *prev, *curr;

    if(start == NULL){
        return new_event;
    }
    else if(start->time > new_event->time){
        new_event->next = start;
        return new_event;
    }
    else{
        prev = start;
        for(curr=start->next; curr!= NULL; curr=curr->next){

            // Order by arrival
            if(curr->time > new_event->time){
                new_event->next = curr;
                prev->next = new_event;
                return start;
            }
            prev = curr;
        }

        prev->next = new_event;
        return start;
    }

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
 * Removes a configuration
 *
 * conf: configuration to remove
 *
 * return:
 */
void remove_conf(config *conf){
    int i;
    queue *queue;
    passenger *p, *temp;

    if(conf->check_in_queues != NULL){

        for(i=0; i<conf->check_in_queue_num; i++){
            queue = conf->check_in_queues[i];
            p=queue->passengers;

            while(p != NULL){
                temp = p;
                p = p->next;

                free(temp);
            }

            free(queue);
        }
    }

    if(conf->covid_test_queues != NULL){

        for(i=0; i<conf->covid_test_queue_num; i++){
            queue = conf->covid_test_queues[i];
            p=queue->passengers;

            while(p != NULL){
                temp = p;
                p = p->next;

                free(temp);
            }

            free(queue);
        }
    }

    free(conf);
}

/*
 * Generate new queues
 *
 * array: Array in which the queues will be added
 * num: Number of queues
 * test_off: Number of officers for each queue
 *
 * return: 0 success, -1 failure
 */
int generate_queues(queue **array, int num, int off){
    int i;

    for(i=0; i<num; i++){
        array[i] = create_queue(i, off, 0);

        if(array[i] == NULL){
            perror();
            return -1;
        }
    }

    return 0;
}

/*
 * Choose one queue based on the number of passengers
 *
 * queues: array of queues
 * queue_num: number of queues
 *
 * return: number of the chosen queue
 */
int choose_queue(queue **queues, int queue_num){
    int i, num, pass_num;

    pass_num = 100000;
    num = -1;

    for(i=0; i<queue_num; i++){
        if(queues[i]->passenger_num < pass_num){
            num = i;
            pass_num = queues[i]->passenger_num;
        }
    }

    return num;
}

/*
 *  Finds the begin time of service for a passenger
 *
 *  arrival: time of arrival
 *  service: service time
 *  last_dep: array of departure times of the last served passenger for each officer
 *  num_off: number of officers
 *
 *  return:
 */
void find_begin_time(int arrival, int service, int *last_dep, int num_off, int max_wait){
    int i, min_dep, min_num, begin;

    // Find officer with min last departure time
    min_dep = 100000;
    min_num = -1;

    for(i=0; i<num_off; i++){
        if(last_dep[i] <  min_dep){
            min_num = i;
            min_dep = last_dep[i];
        }
    }

    if(min_dep < arrival){
        // Officer is idle
        begin = arrival;
    }
    else if(min_dep - arrival > max_wait){
        // Passenger quits
        begin = -1;
    }
    else{
        // Officer is working
        begin = arrival + (min_dep - arrival);
    }

    last_dep[min_num] = begin + service;

    return begin;
}

/*
 *
 */
void simulate(int stop, int test_wait, char *file){
    int i, num, curr_time;
    config *new_config;
    passenger *p, *chek_in, *new_pass;
    queue *q;
    queue *check_queues[CHECK_QUEUE_NUM] = {NULL};
    queue *test_queues[TEST_QUEUE_NUM] = {NULL};
}

/*
 *
 */
config *create_simulation(int stop, int test_wait){
    int i, num, curr_time;
    config *new_config;
    passenger *p, *chek_in, *new_pass;
    queue *q;
    queue *check_queues[CHECK_QUEUE_NUM] = {NULL};
    queue *test_queues[TEST_QUEUE_NUM] = {NULL};
    event *new_event, *event_list;

    // Creating a new empty configuration
    new_config = create_config(name, MAX_CHECK_QUEUE, MAX_TEST_QUEUE);

    if(new_config == NULL) goto exit;

    // Creating covid test queues
    if(generate_queues(test_queues, TEST_QUEUE_NUM, TEST_OFF) != 0){
        remove_conf(new_config);
        goto exit;
    }

    // Creating check-in queues
    if(generate_queues(check_queues, CHECK_QUEUE_NUM, CHECK_OFF) != 0){
        remove_conf(new_config);
        goto exit;
    }

    i = 0;
    curr_time = 0;
    check_in = NULL;

    // Generating passengers arrivals
    while(1){
        new_event = generate_arrival(i, &curr_time, stop);

        if(new_event == NULL){
            if(curr_time > stop){
                // End of simulation reached
                goto exit;
            }
            continue;
        }
        else{
            add_event(event_list, new_event);
        }

    }

    // Simulating covid tests
    simulate_covid_test(test_queues, TEST_QUEUE_NUM, test_wait);

    // Adding passengers who have done the covid test to the check-in queue
    for(i=0; i<TEST_QUEUE_NUM; i++){
        q = test_queues[i];

        for(p=q->passengers; p!=NULL; p=p->next){
            add_passenger(check_in, p);
        }
    }

    // Choosing queues
    if(QUEUE_TYPE == SITA_QUEUE){

        for(p=check_in; p!=NULL; p=p->next){
            if(new_pass->check_in == ONLINE){
                num = 0;
            }
            else if(new_pass->check_in == NATIONAL){
                num = 1;
            }
            else{
                num = 2;
            }

            add_passenger(check_queues[num]->passengers, new_pass);
            check_queues[num]->passenger_num += 1;
        }
    }
    else{
        for(p=check_in; p!=NULL; p=p->next){
            num = choose_queue(check_queues, CHECK_QUEUE_NUM);
            add_passenger(check_queues[num]->passengers, new_pass);
            check_queues[num]->passenger_num += 1;
        }
    }

    exit:
    return new_config;
}



/*
 * Simulates the covid test queues
 *
 * test_queues: array of queues for covid testing
 * num: number of queues in the array
 * results: minutes to wait for the test results
 * check-queues:
 *
 * return:
 */
void simulate_covid_test(queue **queues, int num, int results){
    int i;
    queue *q;
    passenger *p;

    for(i=0; i<num; i++){
        q = queues[i];

        for(p=q->passengers; p!=NULL; p=p->next){
            p->test_begin = find_begin_time(p->test_arrival, p->test_service, q->last_dep, q->officers_num);

            if(p->test_begin >= 0){
                p->test_departure = p->test_begin + p->test_service;
                p->arrival = p->test_departure + 15; // Taking into consideration the minutes required to have the test results
            }
            else{
                p->arrival = -1;
            }
        }
    }

}

/*
 * Simulate check-in queues
 *
 * queues: array of queues
 * num: number of queues in the array
 *
 * return:
 */
void simulate_check_in(queue **queues, int num){
    int i;
    queue *q;
    passenger *p;

    for(i=0; i<num; i++){
        q = queues[i];

        for(p=q->passengers; p!=NULL; p=p->next){
            if(p->arrival >= 0){
                p->begin = find_begin_time(p->arrival, p->service, q->last_dep, q->officers_num);

                if(p->begin >= 0){
                    p->departure = p->begin + p->service;
                }
            }
        }
    }

}



/*
 *
 */
config *create_simulation(int stop, int test_wait){
    int i, num, curr_time;
    config *new_config;
    passenger *p, *chek_in, *new_pass;
    queue *q;
    queue *check_queues[CHECK_QUEUE_NUM] = {NULL};
    queue *test_queues[TEST_QUEUE_NUM] = {NULL};

    // Creating a new empty configuration
    new_config = create_config(name, MAX_CHECK_QUEUE, MAX_TEST_QUEUE);

    if(new_config == NULL) goto exit;

    // Creating covid test queues
    if(generate_queues(test_queues, TEST_QUEUE_NUM, TEST_OFF) != 0){
        remove_conf(new_config);
        goto exit;
    }

    // Creating check-in queues
    if(generate_queues(check_queues, CHECK_QUEUE_NUM, CHECK_OFF) != 0){
        remove_conf(new_config);
        goto exit;
    }

    i = 0;
    curr_time = 0;
    check_in = NULL;

    // Generating passengers arrivals
    while(1){
        new_pass = generate_arrival(i, &curr_time, stop);

        if(new_pass == NULL){
            if(curr_time > stop){
                // End of simulation reached
                goto exit;
            }
            continue;
        }
        else if(new_pass->green_pass == 1){
            add_passenger(check_in, new_pass);
        }
        else{
            num = choose_queue(test_queues, TEST_QUEUE_NUM);
            add_passenger(test_queues[num]->passengers, new_pass);
            test_queues[num]->passenger_num += 1;
        }
    }

    // Simulating covid tests
    simulate_covid_test(test_queues, TEST_QUEUE_NUM, test_wait);

    // Adding passengers who have done the covid test to the check-in queue
    for(i=0; i<TEST_QUEUE_NUM; i++){
        q = test_queues[i];

        for(p=q->passengers; p!=NULL; p=p->next){
            add_passenger(check_in, p);
        }
    }

    // Choosing queues
    if(QUEUE_TYPE == SITA_QUEUE){

        for(p=check_in; p!=NULL; p=p->next){
            if(new_pass->check_in == ONLINE){
                num = 0;
            }
            else if(new_pass->check_in == NATIONAL){
                num = 1;
            }
            else{
                num = 2;
            }

            add_passenger(check_queues[num]->passengers, new_pass);
            check_queues[num]->passenger_num += 1;
        }
    }
    else{
        for(p=check_in; p!=NULL; p=p->next){
            num = choose_queue(check_queues, CHECK_QUEUE_NUM);
            add_passenger(check_queues[num]->passengers, new_pass);
            check_queues[num]->passenger_num += 1;
        }
    }

exit:
    return new_config;
}



/*
 * Creates a new sita queue configuration
 *
 * stop: time to end the simulation
 *
 * return: pointer to the new configuration, NULL on error
 */
config *create_sita_queue(int stop) {
    int i, num, curr_time;
    config *new_config;
    passenger *new_pass;
    queue *check_queues[3] = {NULL};
    queue *test_queues[TEST_QUEUE_NUM] = {NULL};

    // Creating a new empty configuration
    new_config = create_config(name, MAX_CHECK_QUEUE, MAX_TEST_QUEUE);

    if (new_config == NULL) goto exit_sita;

    // Creating different check-in queues
    check_queues[0] = create_queue(0, CHECK_OFF, 0);
    check_queues[1] = create_queue(1, CHECK_OFF, 0);
    check_queues[2] = create_queue(2, CHECK_OFF, 0);

    if(check_queues[0] == NULL || check_queues[1] == NULL || check_queues[2] == NULL){
        remove_conf(new_config);
        goto exit_sita;
    }

    // Creating covid test queues;
    if (generate_queues(test_queues, TEST_QUEUE_NUM, TEST_OFF) != 0) {
        remove_conf(new_config);
        goto exit_sita;
    }

    new_config->check_in_queues = check_queues;
    new_config->covid_test_queues = test_queues;

    i = 0;
    curr_time = 0;

    // Generating passengers arrivals
    while (1) {
        new_pass = generate_arrival(i, &curr_time, stop);

        if (new_pass == NULL) {
            if (curr_time > stop) {
                goto exit_sita;
            }
            continue;
        } else if (new_pass->green_pass == 1) {
            if(new_pass->check_in == ONLINE){
                num = 0;
            }
            else if(new_pass->check_in == NATIONAL){
                num = 1;
            }
            else{
                num = 2;
            }

            add_passenger(check_queues[num]->passengers, new_pass);
            check_queues[num]->passenger_num += 1;
        } else {
            num = choose_queue(test_queues, TEST_QUEUE_NUM);
            add_passenger(test_queues[num]->passengers, new_pass);
            test_queues[num]->passenger_num += 1;
        }
    }

exit_sita:
    return new_config;
}


/*
 * Simulation using given configuration
 *
 * conf: configuration to simulate
 *
 * return:
 */
void simulate(config *conf){
    int i, num_off, begin;
    float mean_wait, mean_response;
    queue *q;
    passenger *p;

    // Simulate covid test area
    for(i=0; i<conf->covid_test_queue_num; i++){
        q = conf->covid_test_queues[i];
        num_off = q->officers_num;

        for(p=q->passengers; p!= NULL; p=p->next){
            begin = find_begin_time(p->test_arrival, p->test_service, q->last_dep, q->officers_num);
            p->test_begin = begin;
            p->test_departure = begin + p->test_service;
            p->arrival = p->test_departure + 15; // Taking into account the time to have the test results

            // DEVI INSERIRLI PRIMA, OGNI PASSEGGERO CHECK_IN PRIMA CONTROLLA CHI ARRIVA DA TEST
        }

    }


    // Simulate check-in area
    for(i=0; i<conf->check_in_queue_num; i++){
        q = conf->check_in_queues[i];
        num_off = q->officers_num;
        mean_wait = 0;
        mean_response = 0;

        for(p=q->passengers; p!= NULL; p=p->next){
            begin = find_begin_time(p->arrival, p->service, q->last_dep, q->officers_num);
            p->begin = begin;
            p->departure = begin + p->test_service;

            mean_wait += p->begin - p->arrival;
            mean_response += p->departure - p->arrival;
        }

        mean_wait = mean_wait/(float)q->passenger_num;
        mean_response = mean_response/(float)q->passenger_num;

        // SCRIVI DA QUALCHE PARTE

        // RICORDATI DI CONSIDERARE CHI ABBANDONA
    }

    // Produce results
}