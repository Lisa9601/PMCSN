#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../config.h"
#include "../include/simulation.h"

#define MAXLINE 4096
#define HEADER "checkin_type,off,mwait,mresponse,mservice,withdrawal,test_type,test_off,test_mwait,test_mresponse,"\
                "test_mservice,test_withdrawal,off_small,small_mwait,small_mresponse,small_mservice,small_withdrawal,"\
                "off_med,med_mwait,med_mresponse,med_mservice,med_withdrawal,off_big,big_mwait,big_mresponse,big_service,"\
                "big_withdrawal"

#define NOQUEUE 0
#define SINGLETYPE 1
#define MULTITYPE 2
#define SITATYPE 3

void print_hello(void);
void write_and_reset(FILE *fptr, result *res, passenger *pass);
void simulate(FILE *fptr, passenger *pass, result *res, int check, int test);
void cleanup(FILE *fptr, config *conf, result *res, passenger **pass);


int main(void){
    FILE *fptr = NULL;
    config *conf = NULL;
    result *res = NULL;
    passenger *pass = NULL;

    print_hello();

    // INITIALIZATION
    printf("Initializing ...\n");

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

    printf("-----------------\n");

    simulate(fptr, pass, res, SINGLETYPE, NOQUEUE);
    simulate(fptr, pass, res, MULTITYPE, NOQUEUE);
    simulate(fptr, pass, res, SITATYPE, NOQUEUE);

    simulate(fptr, pass, res, SINGLETYPE, SINGLETYPE);
    simulate(fptr, pass, res, MULTITYPE, SINGLETYPE);
    simulate(fptr, pass, res, SITATYPE, SINGLETYPE);

    simulate(fptr, pass, res, SINGLETYPE, MULTITYPE);
    simulate(fptr, pass, res, MULTITYPE, MULTITYPE);
    simulate(fptr, pass, res, SITATYPE, MULTITYPE);

    printf("-----------------\n");
    printf("Cleaning up ...\n");

    cleanup(fptr, conf, res, &pass);
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------

/*
 * Print a simple hello message
 */
void print_hello(void) {
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

    snprintf(line, sizeof(char)*MAXLINE, "\n%s,%d,%f,%f,%f,%f,%s,%d,%f,%f,%f,%f,%d,%f,%f,%f,%f,%d,%f,%f,%f,%f,%d,%f,%f,%f,%f",
             res->type, res->num_officers, res->mwait/60, res->mresponse/60, res->mservice/60, res->withdrawal,
             res->test_type, res->test_officers, res->mwait_test/60, res->mresponse_test/60, res->mservice_test/60, res->withdrawal_test,
             res->num_off_small, res->mwait_small/60, res->mresponse_small/60, res->mservice_small/60, res->withdrawal_small,
             res->num_off_med, res->mwait_med/60, res->mresponse_med/60, res->mservice_med/60, res->withdrawal_med,
             res->num_off_big, res->mwait_big/60, res->mresponse_big/60, res->mservice_big/60, res->withdrawal_big);

    // Write results to file
    if(fwrite(line, 1, strlen(line), fptr) == 0) perror("Error writing new line to file");

    // Reset result
    res->type = NONE;
    res->num_officers = 0;
    res->mwait = 0;
    res->mresponse = 0;
    res->mservice = 0;
    res->withdrawal = 0;

    res->num_off_small = 0;
    res->mwait_small = 0;
    res->mresponse_small = 0;
    res->mservice_small = 0;
    res->withdrawal_small = 0;
    res->num_off_med = 0;
    res->mwait_med = 0;
    res->mresponse_med = 0;
    res->mservice_med = 0;
    res->withdrawal_med = 0;
    res->num_off_big = 0;
    res->mwait_big = 0;
    res->mresponse_big = 0;
    res->mservice_big = 0;
    res->withdrawal_big = 0;

    res->test_type = NONE;
    res->test_officers = 0;
    res->mwait_test = 0;
    res->mservice_test = 0;
    res->mresponse_test = 0;
    res->withdrawal_test = 0;

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
 * Simulate the specified configuration
 *
 * fptr: pointer to the results file
 * pass: pointer to the first element of the passengers' list
 * res: pointer to the result structure
 * check: check-in area type
 * test: test area type
 *
 * return:
 */
void simulate(FILE *fptr, passenger *pass, result *res, int check, int test){

    // Simulate covid test area
    if(test == SINGLETYPE){
        printf("Single queue covid test + ");
        simulate_single_test(TESTOFF, &pass, res, TESTWAIT, POSITIVEP);
    }
    else if(test == MULTITYPE){
        printf("Multi queue covid test + ");
        simulate_multi_test(TESTOFF, &pass, res, TESTWAIT, POSITIVEP);
    }

    // Simulate check-in area
    if(check == SINGLETYPE){
        printf("Single queue check-in ...\n");
        simulate_single_queue(OFF, MAXWAIT, pass, res);
    }
    else if(check == MULTITYPE){
        printf("Multi queue check-in ...\n");
        simulate_multi_queue(OFF, MAXWAIT, pass, res);
    }
    else if(check == SITATYPE){
        printf("Sita queue check-in ...\n");
        simulate_sita_queue(ONLINEOFF, NATIONALOFF, INTERNATIONALOFF, MAXWAIT, pass, res);
    }

    // Cleanup
    write_and_reset(fptr, res, pass);
    if(test != NOQUEUE){
        reorder_passengers(&pass);
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