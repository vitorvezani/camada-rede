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

        //Trava o Mutex de sincronismo
        pthread_mutex_lock(&mutex_trans_rede_env2);

        //Trava acesso exclusivo
        pthread_mutex_lock(&mutex_trans_rede_env3);

        enviarDatagrama();

        //Destrava acesso exclusivo
        pthread_mutex_unlock(&mutex_trans_rede_env3);

        //Destrava mutex de sincronismo
        pthread_mutex_unlock(&mutex_trans_rede_env1);

    }
}

void *receberDatagramas() {

    while (TRUE) {

        //Trava mutex de sincronismo
        pthread_mutex_lock(&mutex_rede_enlace_rcv2);

        //Trava acesso exclusivo
        pthread_mutex_lock(&mutex_rede_enlace_rcv3);

        if (buffer_rede_enlace_rcv.tam_buffer != 0) {

            if (buffer_rede_enlace_rcv.retorno == 0)
            {
                montarDatagrama(&datagrama_rcv);
                
                printf("Rede.c (Receber) = > Type: '%d', Tam_buffer: '%d' Bytes, Buffer: '%s'\n", datagrama_rcv.type, datagrama_rcv.tam_buffer,
                datagrama_rcv.buffer);
            }
            else if (buffer_rede_enlace_rcv.retorno == -1){
                printf("Rede.c (Receber) = > ERRO: Datagrama descartado!\n");
            }
        }
        //Libera acesso exclusivo
        pthread_mutex_unlock(&mutex_rede_enlace_rcv3);

        //Destrava mutex de sinconismo
        pthread_mutex_unlock(&mutex_rede_enlace_rcv1);
    }
}

void *enviarDatagrama() {

    pthread_mutex_lock(&mutex_rede_enlace_env1);

        buffer_rede_enlace_rcv = ;

        montarDatagrama();

    pthread_mutex_unlock(&mutex_rede_enlace_env1);

}

void enviarSegmento() {

}

void montarDatagrama(struct datagrama *datagram){

    memcpy(datagram, &buffer_rede_enlace_rcv.datagrama, sizeof (buffer_rede_enlace_rcv.datagrama));

}
