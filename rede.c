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

    char dados_aux[128];

    while (1) {

        //Trava o Mutex de sincronismo
        pthread_mutex_lock(&mutex_env1);

        usleep(300);

        fpurge(stdin);
        fflush(stdin);

        //Trava acesso exclusivo
        pthread_mutex_lock(&mutex_env3);

        if (buffer_rede_enlace_env.tam_buffer != 0) {

            //Testa o retorno da camada de enlace
            if (buffer_rede_enlace_env.retorno == 0) {
                printf("Rede.c (Enviar - Retorno) = > OK\n\n");
            } else if (buffer_rede_enlace_env.retorno == -1) {
                printf("Rede.c (Enviar - Retorno) = > Não há ligacao do nó: '%d'!\n\n", buffer_rede_enlace_env.env_no);
            } else if (buffer_rede_enlace_env.retorno > 0) {
                printf("Rede.c (Enviar - Retorno) = > MTU excedido dividir o pacote no MAX em '%d' bytes\n\n", buffer_rede_enlace_env.retorno);
            } else
                printf("Rede.c (Enviar - Retorno) = > Erro desconhecido\n\n");

            //Reseta os valores
            datagrama_env.tam_buffer = 0;
            strcpy(datagrama_env.buffer, "");
            buffer_rede_enlace_env.retorno = 0;

        }

        //Pega os Dados digitado pelo usuario
        printf("Rede.c (Enviar) = > Digite o Conteudo de data: ");
        fgets(dados_aux, 127, stdin);
        dados_aux[strlen(dados_aux) - 1] = '\0';

        strcpy(datagrama_env.buffer, dados_aux);

        //Seta tipo de msg, tamanho da msg e nó para enviar
        datagrama_env.type = 2;
        datagrama_env.tam_buffer = strlen(datagrama_env.buffer);

        //Colocar no Buffer
        buffer_rede_enlace_env.env_no = 2;
        buffer_rede_enlace_env.tam_buffer = datagrama_env.tam_buffer;
        memcpy(&buffer_rede_enlace_env.datagrama, &datagrama_env, sizeof(datagrama_env));

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

        if (buffer_rede_enlace_rcv.tam_buffer != 0) {

            if (buffer_rede_enlace_rcv.retorno == 0)
            {
                montarDatagrama(&datagrama_rcv);
                
                printf("Rede.c (Receber) = > Type: '%d', Tam_buffer: '%d' Bytes, Buffer: '%s'\n", datagrama_rcv.type, datagrama_rcv.tam_buffer,
                datagrama_rcv.buffer);
            }
            else if (buffer_rede_enlace_rcv.retorno == -1){
                printf("Rede.c (Receber) = > ERRO: Datagrama descartado!\n");
            }else{
                printf("Erro de MTU, fragmentar o datagrama no maximo em '%d' bytes\n", buffer_rede_enlace_rcv.retorno);
            }

        }
        //Libera acesso exclusivo
        pthread_mutex_unlock(&mutex_rcv3);

        //Destrava mutex de sinconismo
        pthread_mutex_unlock(&mutex_rcv1);
    }
}

void montarDatagrama(struct datagrama *datagram){

    memcpy(datagram, &buffer_rede_enlace_rcv.datagrama, sizeof (buffer_rede_enlace_rcv.datagrama));

}