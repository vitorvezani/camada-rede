//
//  transporte.c
//
//  Guilherme Sividal - 09054512 
//  Vitor Rodrigo Vezani - 10159861
//
//  Created by Vitor Vezani on 07/04/13.
//  Copyright (c) 2013 Vitor Vezani. All rights reserved.
//

#include "headers/transporte.h"

void *iniciarTransporte() {

	int te, tr;
    pthread_t threadEnviarSegmentos, threadReceberSegmentos;

    //Inicia a thread enviarDatagramas
    te = pthread_create(&threadEnviarSegmentos, NULL, enviarSegmentos, NULL);

    if (te) {
        printf("ERRO: impossivel criar a thread : enviarSegmentos\n");
        exit(-1);
    }

    //Inicia a thread enviarDatagramas
    tr = pthread_create(&threadReceberSegmentos, NULL, receberSegmentos, NULL);

    if (tr) {
        printf("ERRO: impossivel criar a thread : receberSegmentos\n");
        exit(-1);
    }

    //Espera as threads terminarem
    pthread_join(threadEnviarSegmentos, NULL);
    pthread_join(threadReceberSegmentos, NULL);

}

void *enviarSegmentos(){

	char dados_aux[128];
    char *pch;

    while (1) {

        struct segmento segmento_env;

        //Trava o Mutex de sincronismo
        pthread_mutex_lock(&mutex_trans_rede_env1);

        usleep(300);

        fpurge(stdin);
        fflush(stdin);

        //Trava acesso exclusivo
        pthread_mutex_lock(&mutex_trans_rede_env3);

            //Pega os Dados digitado pelo usuario
            printf("Transporte.c (Enviar) = > Digite nó e data: ");
            fgets(dados_aux, 127, stdin);
            dados_aux[strlen(dados_aux) - 1] = '\0';

        if (isdigit(dados_aux[0]))
        {
            pch = strtok(dados_aux, " ");

            buffer_trans_rede_env.env_no = atoi(pch);

            pch = strtok(NULL, "");

            strcpy(segmento_env.buffer,pch);

            //Seta tipo de msg, tamanho da msg e nó para enviar
            segmento_env.tam_buffer = strlen(segmento_env.buffer);

            //Colocar no Buffer
            buffer_trans_rede_env.tam_buffer = segmento_env.tam_buffer;
            memcpy(&buffer_trans_rede_env.data, &segmento_env, sizeof(segmento_env));
        }else
            printf("data[0] :'%c' não é um int\n", dados_aux[0]);

        //Destrava acesso exclusivo
        pthread_mutex_unlock(&mutex_trans_rede_env3);

        //Destrava mutex de sincronismo
        pthread_mutex_unlock(&mutex_trans_rede_env2);

        // TESTE DE RETORNO DA CAMADA DE REDE
        /*
        pthread_mutex_lock(&mutex_trans_rede_env1);

        //Testa o retorno da camada de enlace
        if (buffer_trans_rede_env.retorno == 0) {
            printf("Transporte.c (Enviar - Retorno) = > OK\n\n");
        } else if (buffer_trans_rede_env.retorno == -1) {
            printf("Transporte.c (Enviar - Retorno) = > Não é nó vizinho: '%d'!\n\n", buffer_trans_rede_env.env_no);
        }else {
            printf("Transporte.c (Enviar - Retorno) = > Erro fatal(2)\n\n");
        }

        pthread_mutex_unlock(&mutex_trans_rede_env1);
        */

    }

}

void *receberSegmentos(){

    while (TRUE) {

        struct segmento segmento_rcv;

        //Trava mutex de sincronismo
        pthread_mutex_lock(&mutex_trans_rede_rcv2);

        //Trava acesso exclusivo
        pthread_mutex_lock(&mutex_trans_rede_rcv3);

        if (buffer_trans_rede_rcv.tam_buffer != 0) {

            montarSegmento(&segmento_rcv);
                
            printf("Transporte.c (Receber) = > Tam_buffer: '%d' Bytes, Buffer: '%s'\n", segmento_rcv.tam_buffer,
            segmento_rcv.buffer);

        }
        //Libera acesso exclusivo
        pthread_mutex_unlock(&mutex_trans_rede_rcv3);

        //Destrava mutex de sinconismo
        pthread_mutex_unlock(&mutex_trans_rede_rcv1);
    }

}

void montarSegmento(struct segmento *segment){

	segment->tam_buffer = buffer_trans_rede_rcv.tam_buffer;
    strcpy(segment->buffer, buffer_trans_rede_rcv.data.buffer);

}