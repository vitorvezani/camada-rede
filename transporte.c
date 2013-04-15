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

}

void *receberSegmentos(){
	
}