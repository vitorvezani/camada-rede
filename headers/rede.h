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
#define MAX_BUFFERS_DESFRAG 5

#define DEBBUG_REDE_FRAGMENTAR
#define DEBBUG_REDE_DESFRAGMENTAR

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
    int retorno;
    struct segmento data;
};

struct buffer_rede_enlace {
    int tam_buffer;
    int env_no;
    int retorno;
    struct datagrama data;
};

struct file {
    char file_name[20];
    int num_no;
};

extern struct buffer_rede_enlace buffer_rede_enlace_env, buffer_rede_enlace_rcv;
extern struct buffer_trans_rede buffer_trans_rede_env, buffer_trans_rede_rcv;
extern struct file file_info;

extern pthread_mutex_t mutex_rede_enlace_env1, mutex_rede_enlace_env2,mutex_rede_enlace_env3;
extern pthread_mutex_t mutex_rede_enlace_rcv1, mutex_rede_enlace_rcv2,mutex_rede_enlace_rcv3 ;
extern pthread_mutex_t mutex_trans_rede_env1, mutex_trans_rede_env2,mutex_trans_rede_env3;
extern pthread_mutex_t mutex_trans_rede_rcv1, mutex_trans_rede_rcv2,mutex_trans_rede_rcv3;

int id = 1; // Inicializa ID em 1

struct datagrama buffers_fragmentacao[MAX_BUFFERS_DESFRAG];

void *receberSegmento();
void *receberDatagramas();
void fragmentarDatagrama(struct datagrama datagram);
void enviarDatagrama(struct datagrama datagrama_env);
int montarDatagramaRcv(struct datagrama *datagram);
int retornoEnlace(struct datagrama datagram);
void enviarSegmento(struct datagrama datagram);
void montarDatagramaEnv(struct datagrama *datagram);
void desfragmentarDatagrama(struct datagrama datagram, int *index);
