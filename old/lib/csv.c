#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "../../include/csv.h"

/*
 * Opens a csv file with the given name
 *
 * name : csv file name
 *
 * return : pointer to the file or NULL in case of error
 */
FILE * open_csv(char *name){
    int i;
    FILE * fptr;

    fptr = fopen(name, "w");

    if(fptr == NULL) perror();

    return fptr;
}

/*
 * Closes the given csv file
 *
 * FILE *: file to be closed
 *
 * return: 0 success, -1 failure
 */
int close_csv(File *fptr){

    if(fclose(fptr) != 0){
        perror();
        return -1;
    }

    return 0;
}

/*
 * Writes a new line to the csv file
 *
 * csv_file: csv file
 * line: line to write
 *
 * return : 0 success, -1 failure
 */
int write_csv_line(FILE *csv_file, char *line){

/*
    char new_line[LEN];

    snprintf(new_line, LEN, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",queue_num, off_num, pass->green_pass, pass->check_in,
             pass->test_arrival, pass->test_begin, pass->test_service, pass->test_departure,
             pass->arrival, pass->begin, pass->service, pass->departure);
*/

    if(fwrite(line, 1, strlen(line), csv_file) == 0){
        perror();
        return -1;
    }

    return 0;
}