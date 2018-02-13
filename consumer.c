/* 
 * File:   consumer.c
 * Author: Michael Beckering
 *
 * Created on February 13, 2018, 10:10 AM
 */

#include <stdio.h>
#include <stdlib.h>

/*
 * 
 */
int main(int argc, char** argv) {
    
    printf("consumer launched. pid: %ld\n", getpid());
    printf("consumer shutting down. pid: %ld\n", getpid());

    return (EXIT_SUCCESS);
}

