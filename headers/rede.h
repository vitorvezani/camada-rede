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

// Defines
#define TRUE 	1
#define FALSE	0
#define MAX_BUFFERS_DESFRAG 5

#define INFINITO 999999
#define TAM_MAX_BUFFER 1400

//#define DEBBUG_REDE_FRAGMENTAR
//#define DEBBUG_REDE_DESFRAGMENTAR
//#define DEBBUG_MONTAR_TABELA

/* Tabela de Rotas */
struct tabela_rotas{
    int quem_enviou;
    int destino;
    int custo;
    int saida;
};

/* Estrutura do Segmento */
struct segmento {
    int tam_buffer;
    char buffer[TAM_MAX_BUFFER];
};

/* Uniao entre segmento e tabela de rotas */
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

/* Estrutura do buffer entre transporte e rede */
struct buffer_trans_rede {
    int tam_buffer;
    int env_no;
    int retorno;
    struct segmento data;
};

/* Estrutura do buffer entre rede e enlace */
struct buffer_rede_enlace {
    int tam_buffer;
    int env_no;
    int retorno;
    struct datagrama data;
};

/* Estrutura do arquivo */
struct file {
    char file_name[20];
    int num_no;
};

// Variaveis Globais à Camada de Rede e Main

extern struct buffer_rede_enlace buffer_rede_enlace_env, buffer_rede_enlace_rcv;
extern struct buffer_trans_rede buffer_trans_rede_env, buffer_trans_rede_rcv;

extern struct file file_info;   //Info do arquivo
int nos_vizinhos[6];            //Nós vizinhos
extern struct ligacoes ligacao; // ligacao dos nós e enlaces

extern pthread_mutex_t mutex_rede_enlace_env1, mutex_rede_enlace_env2,mutex_rede_enlace_env3;
extern pthread_mutex_t mutex_rede_enlace_rcv1, mutex_rede_enlace_rcv2,mutex_rede_enlace_rcv3;
extern pthread_mutex_t mutex_rede_rede_atualizei1, mutex_rede_rede_atualizei2;
extern pthread_mutex_t mutex_rede_rede_receberotas2;
extern pthread_mutex_t mutex_rede_rede_env1, mutex_rede_rede_env2,mutex_rede_rede_env3;
extern pthread_mutex_t mutex_rede_rede_rcv1, mutex_rede_rede_rcv2,mutex_rede_rede_rcv3;
extern pthread_mutex_t mutex_trans_rede_env1, mutex_trans_rede_env2,mutex_trans_rede_env3;
extern pthread_mutex_t mutex_trans_rede_rcv1, mutex_trans_rede_rcv2,mutex_trans_rede_rcv3;


//Variaveis globais à camada de rede

int id = 1; // Inicializa ID em 1
int iniciei = 1; // Enviar tabela de rotas à vizinhos
struct tabela_rotas tabela_rotas[6]; // Tabela de Rotas Interna do nó 
int saida = 0; //Qual nó enviar

struct datagrama buffers_fragmentacao[MAX_BUFFERS_DESFRAG]; // Buffer interno de fragmentos
struct datagrama buffer_rede_rede_env, buffer_rede_rede_rcv; // Buffer interno entre threads

//Threads
void *enviarTabelaRotas();
void *receberTabelaRotas();
void *receberDatagramas();
void *receberSegmento();
void *enviarDatagrama();
void *enviarSegmento();

//Funcoes
void atualizarTabelaRotas(struct datagrama datagram);
void fragmentarDatagramaEnv(struct datagrama datagram);
void desfragmentarDatagramaRcv(struct datagrama datagram, int *index);
void enviarDatagramaNoNaoV(struct datagrama datagram);
void retirarDatagramaBufferRedeRedeRcv(struct datagrama *datagram);
void retirarDatagramaBufferRedeRedeEnv(struct datagrama *datagram);
void retirarDatagramaBufferRedeEnlaceRcv(struct datagrama *datagram);
void retirarDatagramaBufferTransRedeEnv(struct datagrama *datagram);
void colocarDatagramaBufferRedeRedeEnv(struct datagrama datagrama);
void colocarDatagramaBufferRedeRedeRcv(struct datagrama datagrama);
void colocarDatagramaBufferTransRedeEnv(struct datagrama datagrama);
void colocarDatagramaBufferTransRedeRcv(struct datagrama datagrama);
int retornoEnlace(struct datagrama datagram);
void resetarBuffer(struct datagrama *datagram);
void montarTabelaRotasInicial();
void montarDatagramaTabelaRotas(struct datagrama *datagram);
void enviarTabelaRotasVizinhos(struct datagrama *datagram);