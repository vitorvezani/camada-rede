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

//#define DEBBUG_ENLACE

struct segmento {
    int tam_buffer;
    char buffer[100];
};

struct datagrama {
    int type;
    int tam_buffer;
    int offset;
    int id;
    int tamanho_total;
    int mf;
    struct segmento data;
};

struct buffer_rede_enlace {
    int tam_buffer;
    int env_no;
    int retorno;
    struct datagrama data;
};

struct frame {
    int tam_buffer;
    int ecc;
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
