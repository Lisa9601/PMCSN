#include <stdio.h>
#include <stdlib.h>
#include "../config.h"
#include "../include/simulation.h"

int get_config();
int get_covid();

int main(){
    int config, covid;

    system("clear");

#ifdef DEBUG
    printf(" --------------------------------- \n");
    printf("| AIRPORT SIMULATION - DEBUG MODE |\n");
    printf(" --------------------------------- \n\n");
#else
    printf(" -------------------- \n");
    printf("| AIRPORT SIMULATION |\n");
    printf(" -------------------- \n\n");
#endif

    config = get_config();
    getchar();
    covid = get_covid();

    return 0;
}

/*
 * Get user configuration choice
 *
 * return: choice
 */
int get_config(){
    int choice;

    printf("Choose one of the following configurations:\n");
    printf("%d) single queue\n", SINGLE_QUEUE);
    printf("%d) multi queue\n", MULTI_QUEUE);
    printf("%d) sita queue\n", SITA_QUEUE);

    while(1){
        printf("> ");
        scanf("%d", &choice);

        if(choice == SINGLE_QUEUE || choice == MULTI_QUEUE || choice == SITA_QUEUE){
            return choice;
        }
    }

}

/*
 * Get user choice on whether to include a covid test
 *
 * return: choice
 */
int get_covid(){
    char choice;

    printf("Would you like to add a covid test queue? (y or n)\n");

    while(1){
        printf("> ");
        scanf("%c", &choice);

        if(choice == 'y'){
            return 1;
        }
        else if(choice == 'n'){
            return 0;
        }
    }
}