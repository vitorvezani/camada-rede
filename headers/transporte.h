//
//  transporte.h
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

struct segmento {
    int tam_buffer;
    char buffer[100];
};

struct buffer_trans_rede {
    int tam_buffer;
    int env_no;
    int retorno;
    struct segmento data;
};

struct datagrama {
    int tam_buffer;
    int offset;
    int id;
    int tamanho_total;
    int mf;
    int type;
    int env_no;
    struct segmento data;
};

struct file {
    char file_name[20];
    int num_no;
};

extern struct buffer_trans_rede buffer_trans_rede_env, buffer_trans_rede_rcv;
extern struct file file_info;

extern pthread_mutex_t mutex_trans_rede_env1, mutex_trans_rede_env2, mutex_trans_rede_env3;
extern pthread_mutex_t mutex_trans_rede_rcv1, mutex_trans_rede_rcv2, mutex_trans_rede_rcv3;

void *enviarSegmentos();
void *receberSegmentos();
void montarSegmento(struct segmento *segment);
