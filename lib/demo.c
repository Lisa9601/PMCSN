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
void setup_config(config *conf);
void write_result(FILE *fptr, result *res);
void compare_configurations(FILE *fptr, config *conf, result *res);
void multiple_run(FILE *fptr, config *conf, result *res, char *checkin, char *test);

int main(void){
    FILE *fptr = NULL;
    config *conf = NULL;
    result *res = NULL;

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

    setup_config(conf);

    // Create new result structure
    res = (result *)malloc(sizeof(result));
    if(!res){
        perror("Result creation error");
        fclose(fptr);
        free(conf);
        return -1;
    }

    // Write header to the csv file
    if(fwrite(HEADER, 1, strlen(HEADER), fptr) == 0) perror("Error writing header to file");

    //compare_configurations(fptr, conf, res);
    multiple_run(fptr, conf, res, SINGLE, SINGLE);
    //multiple_run(fptr, conf, res, SINGLE, MULTI);
    //multiple_run(fptr, conf, res, MULTI, SINGLE);
    //multiple_run(fptr, conf, res, MULTI, MULTI);
    //multiple_run(fptr, conf, res, SITA, SINGLE);
    //multiple_run(fptr, conf, res, SITA, MULTI);

    fclose(fptr);
    free(conf);
    free(res);

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
 * Setup configuration
 *
 * conf: pointer to a configuration structure
 *
 * return:
 */
void setup_config(config *conf){

    conf->seed = SEED;
    conf->stop = STOP;
    conf->maxwait = MAXWAIT;
    conf->testwait = TESTWAIT;

    conf->off = OFF;
    conf->online_off = ONLINEOFF;
    conf->national_off = NATIONALOFF;
    conf->international_off = INTERNATIONALOFF;
    conf->test_off = TESTOFF;

    conf->lambda_1_5 = LAMBDA_1_5;
    conf->lambda_5_9 = LAMBDA_5_9;
    conf->lambda_9_13 = LAMBDA_9_13;
    conf->lambda_13_17 = LAMBDA_13_17;
    conf->lambda_17_21 = LAMBDA_17_21;
    conf->lambda_21_1 = LAMBDA_21_1;

    conf->test_mu = TESTMU;
    conf->online_mu = ONLINEMU;
    conf->national_mu = NATIONALMU;
    conf->international_mu = INTERNATIONALMU;

    conf->greenpass_p = GREENPASSP;
    conf->positive_p = POSITIVEP;
    conf->online_p = ONLINEP;
    conf->national_p = NATIONALP;
    conf->international_p = INTERNATIONALP;
}

/*
 * Writes the results to the file
 *
 * fptr: pointer to the results file
 * res: pointer to the result structure
 *
 * return:
 */
void write_result(FILE *fptr, result *res) {
    char line[MAXLINE];

    snprintf(line, sizeof(char) * MAXLINE,
             "\n%s,%d,%f,%f,%f,%f,%s,%d,%f,%f,%f,%f,%d,%f,%f,%f,%f,%d,%f,%f,%f,%f,%d,%f,%f,%f,%f",
             res->type, res->num_officers, res->mwait / 60, res->mresponse / 60, res->mservice / 60, res->withdrawal,
             res->test_type, res->test_officers, res->mwait_test / 60, res->mresponse_test / 60,
             res->mservice_test / 60, res->withdrawal_test,
             res->num_off_small, res->mwait_small / 60, res->mresponse_small / 60, res->mservice_small / 60,
             res->withdrawal_small,
             res->num_off_med, res->mwait_med / 60, res->mresponse_med / 60, res->mservice_med / 60,
             res->withdrawal_med,
             res->num_off_big, res->mwait_big / 60, res->mresponse_big / 60, res->mservice_big / 60,
             res->withdrawal_big);

    // Write results to file
    if (fwrite(line, 1, strlen(line), fptr) == 0) perror("Error writing new line to file");
}

/*
 * Compare different configurations
 *
 * fprt: pointer to the file in which the results will be written
 * conf: pointer to a configuration structure
 * res: pointer to a result structure
 *
 * return:
 */
void compare_configurations(FILE *fptr, config *conf, result *res){

    printf("Comparing different configurations:\n");
    printf("-----------------\n");

    printf("Check-in single / Test single\n");
    simulate(conf, NULL, res, SINGLE, SINGLE);
    write_result(fptr, res);

    printf("Check-in single / Test multi\n");
    simulate(conf , NULL, res, SINGLE, MULTI);
    write_result(fptr, res);

    printf("Check-in multi / Test single\n");
    simulate(conf, NULL, res, MULTI, SINGLE);
    write_result(fptr, res);

    printf("Check-in multi / Test multi\n");
    simulate(conf, NULL, res, MULTI, MULTI);
    write_result(fptr, res);

    printf("Check-in sita / Test single\n");
    simulate(conf, NULL, res, SITA, SINGLE);
    write_result(fptr, res);

    printf("Check-in sita / Test multi\n");
    simulate(conf, NULL, res, SITA, MULTI);
    write_result(fptr, res);

    printf("-----------------\n");
    printf("END\n");
}

/*
 * Run a single configuration multiple times
 *
 * fprt: pointer to the file in which the results will be written
 * conf: pointer to a configuration structure
 * res: pointer to a result structure
 * checkin: check-in configuration type
 * test: covid test configuration type
 *
 * return:
 */
void multiple_run(FILE *fptr, config *conf, result *res, char *checkin, char *test){
    state *st;
    int i;

    st = (state *)malloc(sizeof(state));
    if(!st){
        perror("Unable to create new state structure");
        return;
    }

    // Init state
    st->clock = 0;
    st->officers = NULL;
    st->queues = NULL;
    st->low_officers = NULL;
    st->med_officers = NULL;
    st->high_officers = NULL;
    st->test_officers = NULL;
    st->test_queues = NULL;
    st->arr_list = NULL;
    st->serv_list = NULL;
    st->last_arr = 0;

    printf("Simulating Check-in %s / Test %s :\n", checkin, test);
    printf("-----------------\n");

    for(i=0; i<ITERATIONS; i++){
        printf("Iteration %d\n", i+1);
        simulate(conf, st, res, checkin, test);
        write_result(fptr, res);
    }

    printf("-----------------\n");
    printf("END\n");

    free(st);
}