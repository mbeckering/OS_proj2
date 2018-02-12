/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   master.c
 * Author: Michael Beckering
 * Created: Monday, Feb 12, 2018
 *
 * Created on February 12, 2018, 10:19 AM
 */

#include <stdio.h>
#include <stdlib.h>

/*
 * 
 */
int main(int argc, char** argv) {
    
    //initialize variables
    int option;
    int hflag = 0;
    int nflag = 0;
    int n;
    
    //getopt loop to parse command line options
    while ((option = getopt(argc, argv, "hn")) != -1) {
        switch(option) {
            case 'h':
                hflag = 1;     
                break;
            case 'n':
                nflag = 1;
                break;
            default:
                nflag = 1;
                hflag = 1;
                break;
        }
    }
    
    //if only -h is selected, print usage
    if ( (hflag == 1) && (nflag ==0) ) {
        printf("Usage: %s -n <# of processes>\n", argv[0]);
        return 0;
    }
    
    else if ( (nflag == 1) && (hflag == 0) ) {
        //if no argument, use default of 10 processes
        if ( argv[2] == NULL ) {
            n = 10;
            printf("%s: Proceeding with default of %d consumer processes\n",
                    argv[0], n);
        }
        //continue with argument if it's positive
        else if (( n = atoi(argv[2])) >= 1) {
            printf("%s: Proceeding with %d consumer processes\n",
                    argv[0], n);
        }
        //otherwise exit with error message
        else {
            printf("%s: Error: Non-zero argument required for option -n\n", argv[0]);
            return 0;
        }
        //MAIN LOOP HERE
    }
    
    //if no options are selected, print usage and exit
    else {
        printf("Usage: %s -n <# of processes>\n", argv[0]);
        return 0;
    }

    return (EXIT_SUCCESS);
}

