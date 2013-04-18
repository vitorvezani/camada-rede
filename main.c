//
//  main.c
//
//  Guilherme Sividal - 09054512 
//  Vitor Rodrigo Vezani - 10159861
//
//  Created by Vitor Vezani on 19/03/13.
//  Copyright (c) 2013 Vitor Vezani. All rights reserved.
//

#include "headers/main.h"

int main(int argc, char const *argv[]) {

    int te, tr, ttr;
    pthread_t threadIniciaEnlace, threadIniciaRede, threadIniciaTransporte;

    //Testa Parametros
    if (argc != 3) {
        printf("Use: %s 'file_name.ini' 'numero_nó'\n", argv[0]);
        exit(1);
    }

    //Copia para as Variaveis
    strcpy(file_info.file_name, argv[1]);
    file_info.num_no = atoi(argv[2]);

    printf("nome do arquivo: '%s'\n num do nó: '%d'\n", file_info.file_name, file_info.num_no);
    
    //inicializacao do buffer Trans->Rede
    buffer_trans_rede_env.tam_buffer = 0;
    buffer_trans_rede_rcv.tam_buffer = 0;

    //inicializacao do buffer Rede->Enlace
    buffer_rede_enlace_env.tam_buffer = 0;
    buffer_rede_enlace_rcv.tam_buffer = 0;

    //inicializacao do datagrama Rede
    datagrama_env.tam_buffer = 0;
    datagrama_rcv.tam_buffer = 0;

    //inicializacao do segmento Transporte
    segmento_env.tam_buffer = 0;
    segmento_rcv.tam_buffer = 0;

    //Inicializar Mutex Rede->Enlace Enviar
    pthread_mutex_init(&mutex_rede_enlace_env1, NULL);
    pthread_mutex_init(&mutex_rede_enlace_env2, NULL);
    pthread_mutex_init(&mutex_rede_enlace_env3, NULL);
    //Inicializar Mutex Rede->Enlace Receber
    pthread_mutex_init(&mutex_rede_enlace_rcv1, NULL);
    pthread_mutex_init(&mutex_rede_enlace_rcv2, NULL);
    pthread_mutex_init(&mutex_rede_enlace_rcv3, NULL);
    //Inicializar Mutex Trans->Rede Enviar
    pthread_mutex_init(&mutex_trans_rede_env1, NULL);
    pthread_mutex_init(&mutex_trans_rede_env2, NULL);
    pthread_mutex_init(&mutex_trans_rede_env3, NULL);
    //Inicializar Mutex Trans->Rede Receber
    pthread_mutex_init(&mutex_trans_rede_rcv1, NULL);
    pthread_mutex_init(&mutex_trans_rede_rcv2, NULL);
    pthread_mutex_init(&mutex_trans_rede_rcv3, NULL);
    //Inicializar Mutex Rede
    pthread_mutex_init(&mutex_rede1, NULL);
    pthread_mutex_init(&mutex_rede2, NULL);


    //Inicia a thread iniciarEnlace
    te = pthread_create(&threadIniciaEnlace, NULL, iniciarEnlace, NULL);

    if (te) {
        printf("ERRO: impossivel criar a thread : iniciarEnlace\n");
        exit(-1);
    }

    usleep(800);

    //Inicia a thread iniciarRede
    tr = pthread_create(&threadIniciaRede, NULL, iniciarRede, NULL);

    if (tr) {
        printf("ERRO: impossivel criar a thread : iniciarRede\n");
        exit(-1);
    }

    usleep(800);

    //Inicia a thread iniciarTransporte
    ttr = pthread_create(&threadIniciaTransporte, NULL, iniciarTransporte, NULL);

    if (ttr) {
        printf("ERRO: impossivel criar a thread : iniciarTransporte\n");
        exit(-1);
    }

    //Espera as threads terminarem
    pthread_join(threadIniciaEnlace, NULL);
    pthread_join(threadIniciaRede, NULL);
    pthread_join(threadIniciaTransporte, NULL);

    //Destroi o Mutex Rede->Enlace env
    pthread_mutex_destroy(&mutex_rede_enlace_env1);
    pthread_mutex_destroy(&mutex_rede_enlace_env2);
    pthread_mutex_destroy(&mutex_rede_enlace_env3);
    //Destroi o Mutex Rede->Enlace rcv
    pthread_mutex_destroy(&mutex_rede_enlace_rcv1);
    pthread_mutex_destroy(&mutex_rede_enlace_rcv2);
    pthread_mutex_destroy(&mutex_rede_enlace_rcv3);
    //Destroi o Mutex Trans->Rede env
    pthread_mutex_destroy(&mutex_trans_rede_env1);
    pthread_mutex_destroy(&mutex_trans_rede_env2);
    pthread_mutex_destroy(&mutex_trans_rede_env3);
    //Destroi o Mutex Trans->Rede rcv
    pthread_mutex_destroy(&mutex_trans_rede_rcv1);
    pthread_mutex_destroy(&mutex_trans_rede_rcv2);
    pthread_mutex_destroy(&mutex_trans_rede_rcv3);

    return 0;
}