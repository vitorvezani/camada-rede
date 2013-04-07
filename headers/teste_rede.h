//
//  teste_rede.h
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

#define TRUE 	1
#define FALSE	0

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

extern struct datagrama shm_env, shm_rcv;
extern struct file file_info;
extern pthread_mutex_t mutex_env1, mutex_env2, mutex_env3;
extern pthread_mutex_t mutex_rcv1, mutex_rcv2, mutex_rcv3;

void *enviarDatagramas();
void *receberDatagramas();