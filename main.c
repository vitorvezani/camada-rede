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
#include "headers/arquivo.h"

int main(int argc, char const *argv[]) {
    int i, j;
    int te, tr, ttr;
    pthread_t threadIniciaEnlace, threadIniciaRede, threadIniciaTransporte;

    /* Testa Parametros */
    if (argc != 3) {
        printf("Use: %s 'file_name.ini' 'numero_nó'\n", argv[0]);
        exit(1);
    }

    /* Copia para as Variaveis */
    strcpy(file_info.file_name, argv[1]);
    file_info.num_no = atoi(argv[2]);

    printf("Nome do arquivo: '%s'\n Numero nó: '%d'\n", file_info.file_name, file_info.num_no);

    /* Inicializar Mutex Rede->Enlace Enviar */
    pthread_mutex_init(&mutex_rede_enlace_env1, NULL);
    pthread_mutex_init(&mutex_rede_enlace_env2, NULL);
    /* Inicializar Mutex Rede->Enlace Receber */
    pthread_mutex_init(&mutex_rede_enlace_rcv1, NULL);
    pthread_mutex_init(&mutex_rede_enlace_rcv2, NULL);
    /* Inicializar Mutex Rede->Rede Enviar */
    pthread_mutex_init(&mutex_rede_rede_env1, NULL);
    pthread_mutex_init(&mutex_rede_rede_env2, NULL);
    /* Inicializar Mutex Rede->Rede Receber */
    pthread_mutex_init(&mutex_rede_rede_rcv1, NULL);
    pthread_mutex_init(&mutex_rede_rede_rcv2, NULL);
    pthread_mutex_init(&mutex_rede_rede_receberotas2, NULL);
    /* Inicializar Mutex Rede->Rede Atualizei */
    pthread_mutex_init(&mutex_rede_rede_atualizei1, NULL);
    pthread_mutex_init(&mutex_rede_rede_atualizei2, NULL);
    /* Inicializar Mutex Trans->Rede Enviar */
    pthread_mutex_init(&mutex_trans_rede_env1, NULL);
    pthread_mutex_init(&mutex_trans_rede_env2, NULL);
    /* Inicializar Mutex Trans->Rede Receber */
    pthread_mutex_init(&mutex_trans_rede_rcv1, NULL);
    pthread_mutex_init(&mutex_trans_rede_rcv2, NULL);

    //Inicialização de Mutex Consumidores
    pthread_mutex_lock(&mutex_rede_rede_receberotas2);
    pthread_mutex_lock(&mutex_rede_rede_atualizei2);
    pthread_mutex_lock(&mutex_rede_enlace_rcv2);
    pthread_mutex_lock(&mutex_rede_enlace_env2);
    pthread_mutex_lock(&mutex_rede_rede_rcv2);
    pthread_mutex_lock(&mutex_rede_rede_env2);
    pthread_mutex_lock(&mutex_trans_rede_env2);
    pthread_mutex_lock(&mutex_trans_rede_rcv2);

    /* Inicializacao das estrutura ligacao */
    for (i = 0; i < 18; ++i)
        for (j = 0; j < 3; ++j)
            ligacao.enlaces[i][j] = -1;

    for (i = 0; i < 6; ++i)
        for (j = 0; j < 3; ++j)
            strcpy(ligacao.nos[i][j], "-1");

    /* Inicia a thread iniciarEnlace */
    te = pthread_create(&threadIniciaEnlace, NULL, iniciarEnlace, NULL);

    if (te) {
        printf("ERRO: impossivel criar a thread : iniciarEnlace\n");
        exit(-1);
    }

    usleep(15000);

    /* Inicia a thread iniciarRede */
    tr = pthread_create(&threadIniciaRede, NULL, iniciarRede, NULL);

    if (tr) {
        printf("ERRO: impossivel criar a thread : iniciarRede\n");
        exit(-1);
    }

    usleep(15000);

    /* Inicia a thread iniciarTransporte */
    ttr = pthread_create(&threadIniciaTransporte, NULL, iniciarTransporte, NULL);

    if (ttr) {
        printf("ERRO: impossivel criar a thread : iniciarTransporte\n");
        exit(-1);
    }

    /* Espera as threads terminarem */
    pthread_join(threadIniciaEnlace, NULL);
    pthread_join(threadIniciaRede, NULL);
    pthread_join(threadIniciaTransporte, NULL);

    /* Destroi o Mutex Rede->Enlace env */
    pthread_mutex_destroy(&mutex_rede_enlace_env1);
    pthread_mutex_destroy(&mutex_rede_enlace_env2);
    /* Destroi o Mutex Rede->Enlace rcv */
    pthread_mutex_destroy(&mutex_rede_enlace_rcv1);
    pthread_mutex_destroy(&mutex_rede_enlace_rcv2);
    /* Destroi o Mutex Rede->Rede Atualizei */
    pthread_mutex_destroy(&mutex_rede_rede_atualizei1);
    pthread_mutex_destroy(&mutex_rede_rede_atualizei2);
    /* Destroi o Mutex Rede->Rede env */
    pthread_mutex_destroy(&mutex_rede_rede_env1);
    pthread_mutex_destroy(&mutex_rede_rede_env2);
    /* Destroi o Mutex Rede->Rede rcv */
    pthread_mutex_destroy(&mutex_rede_rede_receberotas2);
    pthread_mutex_destroy(&mutex_rede_rede_rcv1);
    pthread_mutex_destroy(&mutex_rede_rede_rcv2);
    /* Destroi o Mutex Trans->Rede env */
    pthread_mutex_destroy(&mutex_trans_rede_env1);
    pthread_mutex_destroy(&mutex_trans_rede_env2);
    /* Destroi o Mutex Trans->Rede rcv */
    pthread_mutex_destroy(&mutex_trans_rede_rcv1);
    pthread_mutex_destroy(&mutex_trans_rede_rcv2);

    return 0;
}