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

    while (1) {

        //Trava o Mutex de sincronismo
        pthread_mutex_lock(&mutex_trans_rede_env1);

        usleep(300);

        fpurge(stdin);
        fflush(stdin);

        //Trava acesso exclusivo
        pthread_mutex_lock(&mutex_trans_rede_env3);

        if (buffer_trans_rede_env.tam_buffer != 0) {

            //Testa o retorno da camada de enlace
            if (buffer_trans_rede_env.retorno == 0) {
                printf("Transporte.c (Enviar - Retorno) = > OK\n\n");
            } else if (buffer_trans_rede_env.retorno == -1) {
                printf("Transporte.c (Enviar - Retorno) = > Não há ligacao do nó: '%d'!\n\n", buffer_trans_rede_env.env_no);
            } else if (buffer_trans_rede_env.retorno > 0) {
                printf("Transporte.c (Enviar - Retorno) = > MTU excedido dividir o pacote no MAX em '%d' bytes\n\n", buffer_trans_rede_env.retorno);
            } else
                printf("Transporte.c (Enviar - Retorno) = > Erro desconhecido\n\n");

            //Reseta os valores
            segmento_env.tam_buffer = 0;
            strcpy(segmento_env.buffer, "");
            buffer_trans_rede_env.retorno = 0;

        }

        //Pega os Dados digitado pelo usuario
        printf("Transporte.c (Enviar) = > Digite o Conteudo de data: ");
        fgets(dados_aux, 127, stdin);
        dados_aux[strlen(dados_aux) - 1] = '\0';

        strcpy(segmento_env.buffer, dados_aux);

        //Seta tipo de msg, tamanho da msg e nó para enviar
        segmento_env.tam_buffer = strlen(segmento_env.buffer);

        //Colocar no Buffer
        buffer_trans_rede_env.env_no = 2;
        buffer_trans_rede_env.tam_buffer = segmento_env.tam_buffer;
        memcpy(&buffer_trans_rede_env.segmento, &segmento_env, sizeof(segmento_env));

        //Destrava acesso exclusivo
        pthread_mutex_unlock(&mutex_trans_rede_env3);

        //Destrava mutex de sincronismo
        pthread_mutex_unlock(&mutex_trans_rede_env2);

    }

}

void *receberSegmentos(){

    while (TRUE) {

        //Trava mutex de sincronismo
        pthread_mutex_lock(&mutex_trans_rede_rcv2);

        //Trava acesso exclusivo
        pthread_mutex_lock(&mutex_trans_rede_rcv3);

        if (buffer_trans_rede_rcv.tam_buffer != 0) {

            if (buffer_trans_rede_rcv.retorno == 0)
            {
                montarSegmento(&segmento_rcv);
                
                printf("Transporte.c (Receber) = > Tam_buffer: '%d' Bytes, Buffer: '%s'\n", segmento_rcv.tam_buffer,
                segmento_rcv.buffer);
            }
            else if (buffer_trans_rede_rcv.retorno == -1){
                printf("Transporte.c (Receber) = > ERRO: Datagrama descartado!\n");
            }else{
                printf("Erro de MTU, fragmentar o segmento no maximo em '%d' bytes\n", buffer_trans_rede_rcv.retorno);
            }

        }
        //Libera acesso exclusivo
        pthread_mutex_unlock(&mutex_trans_rede_rcv3);

        //Destrava mutex de sinconismo
        pthread_mutex_unlock(&mutex_trans_rede_rcv1);
    }

}

void montarSegmento(){

	memcpy(datagram, &buffer_rede_enlace_rcv.datagrama, sizeof (buffer_rede_enlace_rcv.datagrama));

}