//
//  rede.c
//
//  Guilherme Sividal - 09054512 
//  Vitor Rodrigo Vezani - 10159861
//
//  Created by Vitor Vezani on 19/03/13.
//  Copyright (c) 2013 Vitor Vezani. All rights reserved.
//

#include "headers/rede.h"

void *iniciarRede() {

    int te, tr;
    pthread_t threadEnviarDatagramas, threadReceberDatagramas;

    //Inicia a thread enviarDatagramas
    te = pthread_create(&threadEnviarDatagramas, NULL, enviarDatagramas, NULL);

    if (te) {
        printf("ERRO: impossivel criar a thread : enviarDatagramas\n");
        exit(-1);
    }

    //Inicia a thread enviarDatagramas
    tr = pthread_create(&threadReceberDatagramas, NULL, receberDatagramas, NULL);

    if (tr) {
        printf("ERRO: impossivel criar a thread : receberDatagramas\n");
        exit(-1);
    }

    //Espera as threads terminarem
    pthread_join(threadEnviarDatagramas, NULL);
    pthread_join(threadReceberDatagramas, NULL);
}

void *enviarDatagramas() {

    char charopt[128];

    while (1) {

        //Trava o Mutex de sincronismo
        pthread_mutex_lock(&mutex_env1);

        usleep(300);

        fpurge(stdin);
        fflush(stdin);

        //Trava acesso exclusivo
        pthread_mutex_lock(&mutex_env3);

        if (shm_env.tam_buffer != 0) {

            printf("Rede.c (Enviar - Retorno) = > Type: '%d', Num nó: '%d', Data: '%s', Tamanho : '%d'\n", shm_env.type, shm_env.env_no, shm_env.buffer, shm_env.tam_buffer);

            //Testa o retorno da camada de enlace
            if (shm_env.erro == 0) {
                printf("Rede.c (Enviar - Retorno) = > OK\n\n");
            } else if (shm_env.erro == -1) {
                printf("Rede.c (Enviar - Retorno) = > Não há ligacao do nó: '%d'!\n\n", shm_env.env_no);
            } else if (shm_env.erro > 0) {
                printf("Rede.c (Enviar - Retorno) = > MTU excedido dividir o pacote no MAX em '%d' bytes\n\n", shm_env.erro);
            } else
                printf("Rede.c (Enviar - Retorno) = > Erro desconhecido\n\n");

            //Reseta os valores
            shm_env.tam_buffer = 0;
            shm_env.env_no = 0;
            strcpy(shm_env.buffer, "");
            shm_env.erro = 0;

        }

        //Pega os Dados digitado pelo usuario
        printf("Rede.c (Enviar) = > Digite o Conteudo de data: ");
        fgets(charopt, 127, stdin);
        charopt[strlen(charopt) - 1] = '\0';

        strcpy(shm_env.buffer, charopt);

        //Seta tipo de msg, tamanho da msg e nó para enviar
        shm_env.type = 2;
        shm_env.tam_buffer = strlen(shm_env.buffer);
        shm_env.env_no = 2;

        //Destrava acesso exclusivo
        pthread_mutex_unlock(&mutex_env3);

        //Destrava mutex de sincronismo
        pthread_mutex_unlock(&mutex_env2);

    }
}

void *receberDatagramas() {

    while (TRUE) {

        //Trava mutex de sincronismo
        pthread_mutex_lock(&mutex_rcv2);

        //Trava acesso exclusivo
        pthread_mutex_lock(&mutex_rcv3);

        if (shm_rcv.tam_buffer != 0) {

        printf("Rede.c (Receber) = > Type: '%d', Tam_buffer: '%d' Bytes, Buffer: '%s'\n", shm_rcv.type, shm_rcv.tam_buffer,
                shm_rcv.buffer);

        shm_rcv.erro = -1;

        }
        //Libera acesso exclusivo
        pthread_mutex_unlock(&mutex_rcv3);

        //Destrava mutex de sinconismo
        pthread_mutex_unlock(&mutex_rcv1);
    }
}