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
    pthread_t threadReceberSegmento, threadReceberDatagramas;

    //Inicia a thread receberSegmento
    te = pthread_create(&threadReceberSegmento, NULL, receberSegmento, NULL);

    if (te) {
        printf("ERRO: impossivel criar a thread : receberSegmento\n");
        exit(-1);
    }

    //Inicia a thread receberDatagramas
    tr = pthread_create(&threadReceberDatagramas, NULL, receberDatagramas, NULL);

    if (tr) {
        printf("ERRO: impossivel criar a thread : receberDatagramas\n");
        exit(-1);
    }

    //Espera as threads terminarem
    pthread_join(threadReceberSegmento, NULL);
    pthread_join(threadReceberDatagramas, NULL);
}

void *receberSegmento() {

    while (1) {

        struct datagrama datagrama_env;

        montarDatagramaEnv(&datagrama_env);

        enviarDatagrama(datagrama_env);

    }
}

void *receberDatagramas() {

    while (TRUE) {

        struct datagrama datagrama_rcv;

        montarDatagramaRcv(&datagrama_rcv);

        enviarSegmento(datagrama_rcv);

    }
}

void enviarDatagrama(struct datagrama datagrama_env) {

        //Produz
        pthread_mutex_lock(&mutex_rede_enlace_env1);

        buffer_rede_enlace_env.tam_buffer = datagrama_env.tam_buffer;
        buffer_rede_enlace_env.env_no = datagrama_env.env_no;
        buffer_rede_enlace_env.retorno = -1;

        memcpy(&buffer_rede_enlace_env.data,&datagrama_env, sizeof(datagrama_env));

        pthread_mutex_unlock(&mutex_rede_enlace_env2);

        //Consome
        pthread_mutex_lock(&mutex_rede_enlace_env1);

        if (buffer_rede_enlace_env.tam_buffer != 0)
        {
            //Testa o retorno da camada de enlace
            if (buffer_rede_enlace_env.retorno == 0) {
            printf("Rede.c (Enviar - Retorno) = > OK\n\n");
            } else if (buffer_rede_enlace_env.retorno == -1) {
            printf("Rede.c (Enviar - Retorno) = > Não há ligacao do nó: '%d'!\n\n", buffer_rede_enlace_env.env_no);
            } else if (buffer_rede_enlace_env.retorno > 0) {
            printf("Rede.c (Enviar - Retorno) = > MTU excedido dividir o pacote no MAX em '%d' bytes\n\n", buffer_rede_enlace_env.retorno);
            } else{
            printf("Rede.c (Enviar - Retorno) = > Erro Fatal(1)\n\n");
            exit(1);
            }
        }

        pthread_mutex_unlock(&mutex_rede_enlace_env1);
}

void montarDatagramaRcv(struct datagrama *datagram){

        //Trava mutex de sincronismo
        pthread_mutex_lock(&mutex_rede_enlace_rcv2);

        //Trava acesso exclusivo
        pthread_mutex_lock(&mutex_rede_enlace_rcv3);

        if (buffer_rede_enlace_rcv.tam_buffer != 0){

            printf("Rede.c (Receber) = > Datagrama Recebido com sucesso\n");

            if (buffer_rede_enlace_rcv.retorno == 0)
            {

                memcpy(datagram, &buffer_rede_enlace_rcv.data, sizeof (buffer_rede_enlace_rcv.data));
                
                printf("Rede.c (Receber) = > Type: '%d', Tam_buffer: '%d' Bytes, Tam Total: '%d', ID: '%d', offset: '%d', MF: '%d'\n", datagram->type, datagram->tam_buffer,
                datagram->tamanho_total,datagram->id,datagram->offset,datagram->mf);

            }
            else if (buffer_rede_enlace_rcv.retorno == -1){
                printf("Rede.c (Receber) = > ERRO: Datagrama corrompido e descartado!\n");
            }
        }

        //Libera acesso exclusivo
        pthread_mutex_unlock(&mutex_rede_enlace_rcv3);

        //Destrava mutex de sinconismo
        pthread_mutex_unlock(&mutex_rede_enlace_rcv1);

}

void enviarSegmento(struct datagrama datagram) {

    pthread_mutex_lock(&mutex_trans_rede_rcv1);

    buffer_trans_rede_rcv.tam_buffer = datagram.tam_buffer;
    memcpy(&buffer_trans_rede_rcv.data, &datagram.data, sizeof (datagram.data));

    pthread_mutex_unlock(&mutex_trans_rede_rcv2);

}

void montarDatagramaEnv(struct datagrama *datagram){

    //Trava o Mutex de sincronismo
    pthread_mutex_lock(&mutex_trans_rede_env2);

    //Trava acesso exclusivo
    pthread_mutex_lock(&mutex_trans_rede_env3);

        datagram->type = 1;
        datagram->env_no = buffer_trans_rede_env.env_no;
        datagram->tam_buffer = buffer_trans_rede_env.tam_buffer;
        datagram->offset = -1;
        datagram->id = -1;
        datagram->tamanho_total = -1;
        datagram->mf = -1; //More Fragments

        memcpy(&(datagram->data), &buffer_trans_rede_env.data, sizeof (buffer_trans_rede_env.data));

    //Destrava acesso exclusivo
    pthread_mutex_unlock(&mutex_trans_rede_env3);

    //Destrava mutex de sincronismo
    pthread_mutex_unlock(&mutex_trans_rede_env1);

}
