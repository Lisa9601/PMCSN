#include <stdlib.h>
#include <stdio.h>
#include "../../include/passenger.h"
#include "../../include/structs.h"

/*
 * Adds a new passenger to the given list
 *
 * list: list of passengers
 * new_pass: new passenger element
 *
 * return:
 */
void add_passenger(passenger *list, passenger *new_pass){
    passenger *prev, *curr;

    if(list == NULL){
        list = new_pass;
    }
    else if(list->arrival > new_pass->arrival){
        new_pass->next = list;
        list = new_pass;
    }
    else{
        prev = list;
        for(curr=list->next; curr!= NULL; curr=curr->next){

            // Order by arrival
            if(curr->arrival > new_pass->arrival){
                new_pass->next = curr;
                prev->next = new_pass;
                return;
            }
            prev = curr;
        }

        prev->next = new_pass;
    }

}

/*
 * Remove passenger with specified num
 *
 * list: list of passengers
 * num: passenger number
 *
 * return: 0 success, -1 not found
 */
int rm_passenger(passenger *list, int num){
    passenger *prev, *curr;

    if(list == NULL){
        return -1;
    }
    else if(list->num == num){
        curr = list;
        list = list->next;
        free(curr);
        return 0;
    }
    else{
        prev = list;
        for(curr=list->next; curr!=NULL; curr=curr->next){

            if (curr->num == num){
                prev->next = curr->next;
                free(curr);
                return 0;
            }
        }
        return -1;
    }

}