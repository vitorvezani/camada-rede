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

// Defines
#define TAM_MAX_BUFFER 1400

struct tabela_rotas{
    int destino;
    int custo;
    int saida;
};

struct segmento {
    int tam_buffer;
    char buffer[TAM_MAX_BUFFER];
};

union segmento_tabela 
        { 
        struct segmento segmento;
        struct tabela_rotas tabela_rotas[6];
        };

struct datagrama {
    int tam_buffer;
    int offset;
    int id;
    int mf;
    int type;
    int env_no;
    int num_no;
    int retorno;
    union segmento_tabela data;
};

struct buffer_trans_rede {
    int tam_buffer;
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

// Variaveis Globais

struct buffer_rede_enlace buffer_rede_enlace_env, buffer_rede_enlace_rcv;
struct buffer_trans_rede buffer_trans_rede_env, buffer_trans_rede_rcv;

struct file file_info;

struct ligacoes ligacao;

pthread_mutex_t mutex_rede_enlace_env1, mutex_rede_enlace_env2,mutex_rede_enlace_env3;
pthread_mutex_t mutex_rede_enlace_rcv1, mutex_rede_enlace_rcv2,mutex_rede_enlace_rcv3;
pthread_mutex_t mutex_rede_rede_atualizei1, mutex_rede_rede_atualizei2;
pthread_mutex_t mutex_rede_rede_receberotas2;
pthread_mutex_t mutex_rede_rede_env1, mutex_rede_rede_env2,mutex_rede_rede_env3;
pthread_mutex_t mutex_rede_rede_rcv1, mutex_rede_rede_rcv2,mutex_rede_rede_rcv3;
pthread_mutex_t mutex_trans_rede_env1, mutex_trans_rede_env2,mutex_trans_rede_env3;
pthread_mutex_t mutex_trans_rede_rcv1, mutex_trans_rede_rcv2,mutex_trans_rede_rcv3;

// Threads

void *iniciarTransporte();
void *iniciarEnlace();
void *iniciarRede();