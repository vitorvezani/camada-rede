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

struct frame {
    int tam_buffer_frame;
    struct datagrama data;
    int ecc;
};

#define DEBBUG_ENLACE

#define TRUE 			1
#define FALSE 			0

#define NOS 			1
#define ENLACES 		2

extern struct datagrama shm_env, shm_rcv;
extern struct file file_info;
pthread_mutex_t mutex_env1, mutex_env2, mutex_env3;
pthread_mutex_t mutex_rcv1, mutex_rcv2, mutex_rcv3;

void colocarArquivoStruct(FILE * fp, struct ligacoes * ligacao);
void retirarEspaco(char * string);
void montarFrame(struct frame *datagram);
void montarDatagrama(struct frame datagram);
void *enviarFrames(void *param);
void *receberFrames(void *param);
int checkSum(struct datagrama datagram);