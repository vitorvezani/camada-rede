//
//  rede.h
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
    char buffer[100];
};

struct buffer_rede {
    int tam_buffer;
    int env_no;
    struct datagrama datagrama;
    int retorno;
};

struct file {
    char file_name[20];
    int num_no;
};

extern struct datagrama datagrama_env,datagrama_rcv;
extern struct buffer_rede buffer_rede_enlace_env, buffer_rede_enlace_rcv;
extern struct file file_info;
extern pthread_mutex_t mutex_env1, mutex_env2, mutex_env3;
extern pthread_mutex_t mutex_rcv1, mutex_rcv2, mutex_rcv3;

void *enviarDatagramas();
void *receberDatagramas();
void montarDatagrama(struct datagrama *datagram);