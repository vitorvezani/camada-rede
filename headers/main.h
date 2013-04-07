//
//  main.h
//
//  Guilherme Sividal - 09054512 
//  Vitor Rodrigo Vezani - 10159861
//
//  Created by Vitor Vezani on 19/03/13.
//  Copyright (c) 2013 Vitor Vezani. All rights reserved.
//

#include <stdio.h>		    /* for printf() */
#include <stdlib.h>			/* for exit() */
#include <string.h>
#include <pthread.h>     	/* para poder manipular threads */

struct datagrama {
    int type;
    int tam_buffer;
    int env_no;
    char buffer[100];
    int erro;
};

struct file {
    char file_name[20];
    int num_no;
};

struct datagrama shm_env, shm_rcv;
struct file file_info;
pthread_mutex_t mutex_env1, mutex_env2, mutex_env3;
pthread_mutex_t mutex_rcv1, mutex_rcv2, mutex_rcv3;

void *iniciarEnlace();
void *iniciarTesteEnlace();