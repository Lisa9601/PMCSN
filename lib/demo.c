#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "../config.h"
#include "../include/simulation.h"

#define DNAME "demo"
#define MAXLINE 4096
#define HEADER "checkin_type,queues,off,mwait,mresponse,withdrawal,test_type,test_queues,test_off,test_mwait,test_mresponse,"\
                "off_small,off_med,off_big,small_mwait,med_mwait,big_mwait,small_mresponse,med_mresponse,big_mresponse, "    \
                "withd_small, withd_med, withd_big"

void print_hello(void);
void write_and_reset(FILE *fptr, result *res, passenger *pass);
void simulate_without_test(FILE *fptr, passenger *pass, result *res);
void simulate_single_queue_test(FILE *fptr, passenger *pass, result *res);
void simulate_multi_queue_test(FILE *fptr, passenger *pass, result *res);
void cleanup(FILE *fptr, config *conf, result *res, passenger **pass);

int main(void){
    FILE *fptr = NULL;
    config *conf = NULL;
    result *res = NULL;
    passenger *pass = NULL;

    print_hello();

    // INITIALIZATION
    printf("%s: Initializing ...\n", DNAME);

    // Open file
    fptr = (FILE *)fopen(FILENAME, "w");
    if(!fptr){
        perror("File opening error");
        return -1;
    }

    // Create new configuration
    conf = (config *)malloc(sizeof(config));
    if(!conf){
        perror("Config creation error");
        fclose(fptr);
        return -1;
    }

    conf->lambda = LAMBDA;
    conf->test_mu = TESTMU;
    conf->online_mu = ONLINEMU;
    conf->national_mu = NATIONALMU;
    conf->international_mu = INTERNATIONALMU;
    conf->greenpass_p = GREENPASSP;
    conf->online_p = ONLINEP;
    conf->national_p = NATIONALP;
    conf->international_p = INTERNATIONALP;

    // Create new structure in which the results will be kept
    res = (result *)malloc(sizeof(result));
    if(!res){
        perror("Result creation error");
        fclose(fptr);
        free(conf);
        return -1;
    }

    init_simulation(SEED);  // Initializing seed

    // Write header to the csv file
    if(fwrite(HEADER, 1, strlen(HEADER), fptr) == 0) perror("Error writing header to file");

    // Generate passenger arrivals
    generate_arrivals(conf, STOP, &pass);
    if(pass == NULL){
        fclose(fptr);
        free(conf);
        free(res);
        return -1;
    }

    printf("%s: -----------------\n", DNAME);

    simulate_without_test(fptr, pass, res);
    simulate_single_queue_test(fptr, pass, res);
    //simulate_multi_queue_test(fptr, pass, res);

    printf("%s: -----------------\n", DNAME);
    printf("%s: Cleaning up ...\n", DNAME);

    cleanup(fptr, conf, res, &pass);
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------

/*
 * Print a simple hello message
 */
void print_hello(void){
    system("clear");

    printf(" ------------------------ \n");
    printf("|   AIRPORT SIMULATION   |\n");
    printf(" ------------------------ \n\n");
}

/*
 * Writes the results to the file and cleans up the result and passenger structs
 *
 * fptr: pointer to the results file
 * res: pointer to the result structure
 * pass: pointer to the list of passengers
 *
 * return:
 */
void write_and_reset(FILE *fptr, result *res, passenger *pass){
    char line[MAXLINE];
    passenger *p;

    snprintf(line, sizeof(char)*MAXLINE, "\n%s,%d,%d,%f,%f,%f,%s,%d,%d,%f,%f,%d,%d,%d,%f,%f,%f,%f,%f,%f,%f,%f,%f",
             res->type, res->num_queues, res->num_officers, res->mwait, res->mresponse, res->withdrawal, res->test_type,
             res->test_queues, res->test_officers, res->mwait_test, res->mresponse_test, res->num_off_small,
             res->num_off_med, res->num_off_big, res->mwait_small, res->mwait_med, res->mwait_big, res->mresponse_small,
             res->mresponse_med, res->mresponse_big, res->withdrawal_small, res->withdrawal_med, res->withdrawal_big);

    // Write results to file
    if(fwrite(line, 1, strlen(line), fptr) == 0) perror("Error writing new line to file");

    // Reset result
    res->type = NONE;
    res->num_queues = 0;
    res->num_officers = 0;
    res->mwait = 0;
    res->mresponse = 0;
    res->withdrawal = 0;

    res->test_type = NONE;
    res->test_queues = 0;
    res->test_officers = 0;
    res->mwait_test = 0;
    res->mresponse_test = 0;

    res->num_off_small = 0;
    res->mwait_small = 0;
    res->mresponse_small = 0;
    res->withdrawal_small = 0;
    res->num_off_med = 0;
    res->mwait_med = 0;
    res->mresponse_med = 0;
    res->withdrawal_med = 0;
    res->num_off_big = 0;
    res->mwait_big = 0;
    res->mresponse_big = 0;
    res->withdrawal_big = 0;

    // Reset passenger list
    for(p=pass; p!=NULL; p=p->next){
        p->begin = 0;
        p->departure = 0;

        p->test_begin = 0;
        p->test_departure = 0;

        if(p->test_arrival >= 0){
            p->greenpass = 0;
            p->arrival = p->test_arrival;
        }
    }
}

/*
 * Simulates all queue configurations without the covid test area
 *
 * fptr: pointer to the results file
 * pass: pointer to the first element of the passengers' list
 * res: pointer to the result structure
 *
 * return:
 */
void simulate_without_test(FILE *fptr, passenger *pass, result *res){
    int i, j, k;

    printf("%s: Simulating single queue ...\n", DNAME);

    for(i=0; i<MAXOFF; i++){
        simulate_single_queue(i+1, MAXWAIT, pass, res);
        write_and_reset(fptr, res, pass);
    }

    printf("%s: Simulating multi queue ...\n", DNAME);

    for(i=0; i<MAXOFF; i++){
        simulate_multi_queue(i+1, MAXWAIT, pass, res);
        write_and_reset(fptr, res, pass);
    }

    printf("%s: Simulating sita queue ...\n", DNAME);

    for(i=0; i<MAXOFF/3; i++){
        for(j=0; j<MAXOFF/3; j++){
            for(k=0; k<MAXOFF/3; k++){
                simulate_sita_queue(i+1, j+1, k+1, MAXWAIT, pass, res);
                write_and_reset(fptr, res, pass);
            }
        }
    }
}

/*
 * Simulates all queue configurations with a single queue covid test area
 *
 * fptr: pointer to the results file
 * pass: pointer to the first element of the passengers' list
 * res: pointer to the result structure
 *
 * return:
 */
void simulate_single_queue_test(FILE *fptr, passenger *pass, result *res){
    int i, j, k, t;

    printf("%s: Simulating single queue covid test + single queue...\n", DNAME);

    for(i=0; i<MAXOFF; i++){
        simulate_single_test(i+1, pass, res, TESTWAIT);
        simulate_single_queue(i+1, MAXWAIT, pass, res);
        write_and_reset(fptr, res, pass);
    }

    printf("%s: Simulating single queue covid test + multi queue ...\n", DNAME);

    for(i=0; i<MAXOFF; i++){
        simulate_single_test(i+1, pass, res, TESTWAIT);
        simulate_multi_queue(i+1, MAXWAIT, pass, res);
        write_and_reset(fptr, res, pass);
    }

    printf("%s: Simulating single queue covid test + sita queue ...\n", DNAME);

    for(t=0; t<MAXOFF; t++){
        for(i=0; i<MAXOFF/3; i++){
            for(j=0; j<MAXOFF/3; j++){
                for(k=0; k<MAXOFF/3; k++){
                    simulate_single_test(t+1, pass, res, TESTWAIT);
                    simulate_sita_queue(i+1, j+1, k+1, MAXWAIT, pass, res);
                    write_and_reset(fptr, res, pass);
                }
            }
        }
    }
}

/*
 * Simulates all queue configurations with a multi queue covid test area
 *
 * fptr: pointer to the results file
 * pass: pointer to the first element of the passengers' list
 * res: pointer to the result structure
 *
 * return:
 */
void simulate_multi_queue_test(FILE *fptr, passenger *pass, result *res){
    int i, j, k, t;

    printf("%s: Simulating multi queue covid test + single queue...\n", DNAME);

    for(i=0; i<MAXOFF; i++){
        simulate_multi_test(i+1, pass, res, TESTWAIT);
        simulate_single_queue(i+1, MAXWAIT, pass, res);
        write_and_reset(fptr, res, pass);
    }

    printf("%s: Simulating multi queue covid test + multi queue ...\n", DNAME);

    for(i=0; i<MAXOFF; i++){
        simulate_multi_test(i+1, pass, res, TESTWAIT);
        simulate_multi_queue(i+1, MAXWAIT, pass, res);
        write_and_reset(fptr, res, pass);
    }

    printf("%s: Simulating multi queue covid test + sita queue ...\n", DNAME);

    for(t=0; t<MAXOFF; t++){
        for(i=0; i<MAXOFF/3; i++){
            for(j=0; j<MAXOFF/3; j++){
                for(k=0; k<MAXOFF/3; k++){
                    simulate_multi_test(t+1, pass, res, TESTWAIT);
                    simulate_sita_queue(i+1, j+1, k+1, MAXWAIT, pass, res);
                    write_and_reset(fptr, res, pass);
                }
            }
        }
    }
}

/*
 * Clean up the simulation
 *
 * fprt: pointer to the results file
 * conf: pointer to the configuration structure
 * res: pointer to the result structure
 * pass: pointer to the head of the list of passengers
 *
 * return:
 */
void cleanup(FILE *fptr, config *conf, result *res, passenger **pass){
    fclose(fptr);
    free(conf);
    free(res);
    free_passengers(pass);
}