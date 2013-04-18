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
    pthread_t threadReceberDatagramas, threadReceberSegmento;

    //Inicia a thread enviarDatagramas
    te = pthread_create(&threadReceberSegmento, NULL, receberSegmento, NULL);

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
    pthread_join(threadReceberSegmento, NULL);
    pthread_join(threadReceberDatagramas, NULL);
}

void *receberSegmento(){

    while (1) {

        //Trava o Mutex de sincronismo
        pthread_mutex_lock(&mutex_trans_rede_env2);


        //Destrava mutex de sincronismo
        pthread_mutex_unlock(&mutex_trans_rede_env1);

        enviarDatagramas();

    }
}

void *enviarVetorDistancia(){

    while (1) {


        colocarDatagramaBufferInterno();


    }
}

void *receberDatagramas(){

    while (1) {


        colocarDatagramaBufferInterno();


    }
}

void colocarDatagramaBufferInterno() {

        if (/* condition */)
            pthread_mutex_lock(&mutex_rede_enlace_env4);
            /*TODO*/

        else if (/* condition */)
            pthread_mutex_lock(&mutex_rede_enlace_env6);
            /*TODO*/

        else if (/* condition */)
            pthread_mutex_lock(&mutex_rede_enlace_env7);
            /*TODO*/
        else
            exit(1);



        //Destrava mutex de sincronismo
        pthread_mutex_unlock(&mutex_rede_enlace_env5);
    }
}

void enviarDatagramas() {


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
                montarDatagramas(&datagrama_rcv);
                enviarDatagramas();
                
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

void enviarSegmento() {

    //Trava o Mutex de sincronismo
    pthread_mutex_lock(&mutex_trans_rede_env1);

    //Trava acesso exclusivo
    pthread_mutex_lock(&mutex_trans_rede_env3);

    enviarDatagramas

    //Destrava acesso exclusivo
    pthread_mutex_unlock(&mutex_trans_rede_env3);

    //Destrava mutex de sincronismo
    pthread_mutex_unlock(&mutex_trans_rede_env2);

}
