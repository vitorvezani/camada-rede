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

    int te, tite;
    pthread_t threadIniciaEnlace, threadIniciaRede, threadIniciaTesteRede;

    //Testa Parametros
    if (argc != 3) {
        printf("Use: %s 'file_name.ini' 'numero_nó'\n", argv[0]);
        exit(1);
    }

    //Copia para as Variaveis
    strcpy(file_info.file_name, argv[1]);
    file_info.num_no = atoi(argv[2]);

    printf("nome do arquivo: '%s'\n num do nó: '%d'\n", file_info.file_name, file_info.num_no);

    //inicializacao do buffer Rede->Enlace(Enviar)
    shm_env.erro = -9;
    shm_env.env_no = -1;
    strcpy(shm_env.buffer, "");
    shm_env.tam_buffer = 0;

    //inicializacao do buffer Rede->Enlace(Receber)
    shm_rcv.erro = -9;
    shm_rcv.env_no = -1;
    strcpy(shm_rcv.buffer, "");
    shm_rcv.tam_buffer = 0;

    //Inicializar Mutex Enviar
    pthread_mutex_init(&mutex_env1, NULL);
    pthread_mutex_init(&mutex_env2, NULL);
    pthread_mutex_init(&mutex_env3, NULL);
    //Inicializar Mutex Receber
    pthread_mutex_init(&mutex_rcv1, NULL);
    pthread_mutex_init(&mutex_rcv2, NULL);
    pthread_mutex_init(&mutex_rcv3, NULL);

    //Inicia a thread iniciarEnlace
    te = pthread_create(&threadIniciaEnlace, NULL, iniciarEnlace, NULL);

    if (te) {
        printf("ERRO: impossivel criar a thread : iniciarEnlace\n");
        exit(-1);
    }

    usleep(800);

    //Inicia a thread iniciarRede
    tite = pthread_create(&threadIniciaRede, NULL, iniciarRede, NULL);

    if (tite) {
        printf("ERRO: impossivel criar a thread : iniciarRede\n");
        exit(-1);
    }

    usleep(800);

    //Inicia a thread iniciarRede
    tite = pthread_create(&threadIniciaTesteRede, NULL, iniciarTesteRede, NULL);

    if (tite) {
        printf("ERRO: impossivel criar a thread : iniciarTesteRede\n");
        exit(-1);
    }

    //Espera as threads terminarem
    pthread_join(threadIniciaEnlace, NULL);
    pthread_join(threadIniciaRede, NULL);
    pthread_join(threadIniciaTesteRede, NULL);

    //Destroi o Mutex env
    pthread_mutex_destroy(&mutex_env1);
    pthread_mutex_destroy(&mutex_env2);
    pthread_mutex_destroy(&mutex_env3);
    //Destroi o Mutex rcv
    pthread_mutex_destroy(&mutex_rcv1);
    pthread_mutex_destroy(&mutex_rcv2);
    pthread_mutex_destroy(&mutex_rcv3);

    return 0;
}