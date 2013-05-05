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

// Defines
#define TRUE 	1
#define FALSE	0

#define TAM_MAX_BUFFER 1400

/* Estrutura do Segmento */
struct segmento {
    int tam_buffer;
    char buffer[TAM_MAX_BUFFER];
};

/* Estrutura do buffer entre a camada de transporte e rede*/
struct buffer_trans_rede {
    int tam_buffer;
    int env_no;
    int retorno;
    struct segmento data;
};

/* Estrutura do datagrama */
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

/* Estrutura do arquivo */
struct file {
    char file_name[20];
    int num_no;
};

// Variaveis Globais à Camada de Rede e Main

extern struct buffer_trans_rede buffer_trans_rede_env, buffer_trans_rede_rcv;
extern struct file file_info;

extern pthread_mutex_t mutex_trans_rede_env1, mutex_trans_rede_env2, mutex_trans_rede_env3;
extern pthread_mutex_t mutex_trans_rede_rcv1, mutex_trans_rede_rcv2, mutex_trans_rede_rcv3;

// Threads
void *enviarSegmentos();
void *receberSegmentos();

//Funcoes

void montarSegmento(struct segmento *segment);