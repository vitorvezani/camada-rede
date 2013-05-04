//
//  enlace.h
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
#include <sys/socket.h>
#include <netinet/in.h>

#define TRUE            1
#define FALSE           0

#define NOS             1
#define ENLACES         2

#define TAM_MAX_BUFFER 1400

//#define DEBBUG_ENLACE

struct segmento {
    int tam_buffer;
    char buffer[TAM_MAX_BUFFER];
};

struct tabela_rotas{
    int tabela[7][7];
};

union segmento_tabela 
        { 
        struct segmento segmento;
        struct tabela_rotas tabela_rotas;
        };

struct datagrama {
    int tam_buffer;
    int offset;
    int id;
    int mf;
    int type;
    int env_no;
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

extern struct buffer_rede_enlace buffer_rede_enlace_env, buffer_rede_enlace_rcv;
extern struct file file_info;

extern struct ligacoes ligacao;

extern pthread_mutex_t mutex_rede_enlace_env1, mutex_rede_enlace_env2,mutex_rede_enlace_env3;
extern pthread_mutex_t mutex_rede_enlace_rcv1, mutex_rede_enlace_rcv2,mutex_rede_enlace_rcv3;

void colocarArquivoStruct(FILE * fp);
void retirarEspaco(char * string);
void colocarBufferFrame(struct frame *frame);
void colocarDatagramaBuffer(struct frame frame);
void *enviarFrames();
void *receberFrames();
int checkSum(struct datagrama datagram);
