#include <stdio.h>

int main(){

#ifdef DEBUG
    printf("Debug mode activated!\n");
#endif

    printf("Hello world!\n");
    return 0;
}

