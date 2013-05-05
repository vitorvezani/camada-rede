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

/* Estrutura da tabela de rotas */
struct tabela_rotas{
    int destino;
    int custo;
    int saida;
};

/* Estrutura do segmento */
struct segmento {
    int tam_buffer;
    char buffer[TAM_MAX_BUFFER];
};

/* Union entre segmento ou tabela de rotas */
union segmento_tabela 
        { 
        struct segmento segmento;
        struct tabela_rotas tabela_rotas[6];
        };

/* Estrutura do datagrama */
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

/* Estrutura do buffer entre rede e enlace */
struct buffer_rede_enlace {
    int tam_buffer;
    int env_no;
    int retorno;
    struct datagrama data;
};

/* Estrutura do Frame */
struct frame {
    int tam_buffer;
    int ecc;
    struct datagrama data;
};

struct file {
    char file_name[20];
    int num_no;
};

// Variaveis globais à camada de rede e a main
extern struct buffer_rede_enlace buffer_rede_enlace_env, buffer_rede_enlace_rcv;
extern struct file file_info;

extern struct ligacoes ligacao;

extern pthread_mutex_t mutex_rede_enlace_env1, mutex_rede_enlace_env2,mutex_rede_enlace_env3;
extern pthread_mutex_t mutex_rede_enlace_rcv1, mutex_rede_enlace_rcv2,mutex_rede_enlace_rcv3;

// Threads
void *enviarFrames();
void *receberFrames();

//Funcoes
void colocarArquivoStruct(FILE * fp);
void retirarEspaco(char * string);
void colocarBufferFrame(struct frame *frame);
void colocarDatagramaBuffer(struct frame frame);
int checkSum(struct datagrama datagram);
