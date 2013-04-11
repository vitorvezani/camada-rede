//
//  enlace.c
//
//  Guilherme Sividal - 09054512 
//  Vitor Rodrigo Vezani - 10159861
//
//  Created by Vitor Vezani on 19/03/13.
//  Copyright (c) 2013 Vitor Vezani. All rights reserved.
//

#include "headers/arquivo.h"
#include "headers/enlace.h"

void *iniciarEnlace() {

    int te, tr;
    int result;
    int i, j;

    pthread_t threadEnviarFrames, threadReceberFrames;

    struct ligacoes ligacao;

    //Inicializacao das estrutura ligacao
    for (i = 0; i < 18; ++i)
        for (j = 0; j < 3; ++j)
            ligacao.enlaces[i][j] = 0;

    FILE * fp;
    fp = fopen(file_info.file_name, "r");

    if (!fp) {
        perror("Fopen()");
        exit(1);
    }

    //Funcão que coloca o arquivo na estrutura ligacao
    colocarArquivoStruct(fp, &ligacao);

    //Inicia a thread enviarFrames
    te = pthread_create(&threadEnviarFrames, NULL, enviarFrames, (void *) &ligacao);

    if (te) {
        printf("ERRO: impossivel criar a thread : Enviar Pacote\n");
        exit(-1);
    }

    //Inicia a thread ReceberFrames
    tr = pthread_create(&threadReceberFrames, NULL, receberFrames, (void *) &ligacao);

    if (tr) {
        printf("ERRO: impossivel criar a thread : Receber Pacote\n");
        exit(-1);
    }

    //Espera as threads terminarem
    pthread_join(threadEnviarFrames, NULL);
    pthread_join(threadReceberFrames, NULL);
}

void *enviarFrames(void *param) {

    struct ligacoes *ligacaoo = (struct ligacoes *) param;

    struct ligacoes ligacao = *ligacaoo;

    int i, j;

    //Variavel para setar no fim das iteracoes a variavel buffer_rede_enlace_env.retorno
    int flag;

#ifdef DEBBUG_ENLACE
    printf("\n");
#endif

    while (1) {

        struct frame frame_env;
        struct sockaddr_in to;
        int atoi_result = -1;
        int s, mtu;

        //trava mutex de sincronismo
        pthread_mutex_lock(&mutex_env2);

        flag = 0;

        fflush(stdin);

        //Trava acesso exclusivo ao buffer
        pthread_mutex_lock(&mutex_env3);

        //Loop no ligacao enlaces
        for (i = 0; i < 18; ++i) {
            //Verificar se existe ligacao entre seu nó e o nó destino
            if ((ligacao.enlaces[i][0] == file_info.num_no) && (buffer_rede_enlace_env.env_no == ligacao.enlaces[i][1])) {
#ifdef DEBBUG_ENLACE
                printf("Enlace.c = > Existe Ligacao nos [Enlaces]\n");
#endif

                //seta MTU do enlace encontrado
                mtu = ligacao.enlaces[i][2];

                //Loop nos nós para encontrar IP e porta do nó destino
                for (i = 0; i < 6; ++i) {
                    atoi_result = atoi(ligacao.nos[i][0]);

                    //Verificar o IP e Porta do nó destino
                    if (atoi_result == buffer_rede_enlace_env.env_no) {
                        //Cria o socket
                        if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
                            perror("socket()");
                            exit(1);
                        }

#ifdef DEBBUG_ENLACE
                        printf("Enlace.c = > Existe -> [Nó]: '%d',possui IP: '%s' , Porta: '%d'\n", buffer_rede_enlace_env.env_no, ligacao.nos[i][1], atoi(ligacao.nos[i][2]));
#endif

                        //seta o IP e Porta do nó destino no sockaddr_in 'to'
                        to.sin_family = AF_INET;
                        to.sin_port = htons(atoi(ligacao.nos[i][2])); // Porta do nó        
                        to.sin_addr.s_addr = inet_addr(ligacao.nos[i][1]); // Endereço IP do nó  

#ifdef DEBBUG_ENLACE
                        printf("Enlace.c = > Nó Configurado\n");
#endif

                        //Função que monta o frame
                        montarFrame(&frame_env);

#ifdef DEBBUG_ENLACE
                        printf("Enlace.c = > Frame Montado! tam_buffer: '%d', tam_data: '%lu', tam_frame: '%lu'\n", frame_env.tam_buffer,
                                sizeof (frame_env.data), sizeof (frame_env));
#endif

                        //Testa MTU do frame vs MTU do nó
                        if (frame_env.tam_buffer > mtu) {
                            printf("Enlace.c = > Erro de MTU\n");
                            buffer_rede_enlace_env.retorno = mtu;
                            flag = 2;
                            break;
                        }

#ifdef DEBBUG_ENLACE
                        printf("Enlace.c = > sizeof(Frame): '%lu', MTU: '%d'\n", sizeof (frame_env), ligacao.enlaces[i][2]);
#endif	

                        //Funcão do calculo de checksum
                        frame_env.ecc = checkSum(frame_env.data);

#ifdef DEBBUG_ENLACE
                        printf("Enlace.c = > ECC Calculado! ecc: '%d'\n", frame_env.ecc);
#endif	

                        //Setar as variaveis L,C,D do garbler
                        set_garbler(0, 0, 0);

                        //Funcão que envia para o nó destino o Frame
                        if (sendto_garbled(s, &frame_env, sizeof (frame_env), 0, (struct sockaddr *) &to, sizeof (to)) < 0) {
                            perror("sendto()");
                            printf("Enlace.c = > Dados não enviados!\n");
                        } else {
                            printf("Enlace.c = > Dados enviados!\n");
                            flag = 1;
                        }

                    }
                }
            }
        }

        /*Teste da variavel flag para setar a variavel buffer_rede_enlace_env.retorno*/

        if (flag == 0) {
            buffer_rede_enlace_env.retorno = -1;
        }
        else if (flag == 1) {
            buffer_rede_enlace_env.retorno = 0;
        }

        //Destrava acesso exclusivo ao buffer
        pthread_mutex_unlock(&mutex_env3);

        //Libera mutex para sincronismo
        pthread_mutex_unlock(&mutex_env1);
    }
}

void *receberFrames(void *param) {

    struct ligacoes *ligacaoo = (struct ligacoes *) param;

    struct ligacoes ligacao = *ligacaoo;

    int i, atoi_result, s, from_address_size;
    struct sockaddr_in from, server;

    //Cria socket
    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket()");
        exit(1);
    }

    //Loop nos nós para achar o IP e Porta de seu nó
    for (i = 0; i < 6; ++i) {

        atoi_result = atoi(ligacao.nos[i][0]);

        if (atoi_result == file_info.num_no) {

#ifdef DEBBUG_ENLACE
            //printf("Enlace.c (server) = > Escutando IP: '%s' Porta: '%s'\n",ligacao.nos[i][1],ligacao.nos[i][2]);
#endif

            //seta o IP e Porta de seu nó no sockaddr_in 'server'
            server.sin_family = AF_INET;
            server.sin_port = htons(atoi(ligacao.nos[i][2])); // Porta do servidor
            server.sin_addr.s_addr = inet_addr(ligacao.nos[i][1]); // Endereço IP do servidor
        }
    }

    //Bind no IP e Porta
    if (bind(s, (struct sockaddr *) &server, sizeof (server)) < 0) {
        perror("bind()");
        exit(1);
    }

    while (TRUE) {

        struct frame frame_rcv;
        int sum = 0;

        //Trava o mutex de sincronismo
        pthread_mutex_lock(&mutex_rcv1);

        //Fica esperando receber Frames
        from_address_size = sizeof (from);
        if (recvfrom(s, &frame_rcv, sizeof (frame_rcv), 0, (struct sockaddr *) &from, &from_address_size) < 0) {
            perror("recvfrom()");
            exit(1);
        }

        //Trava acesso exclusivo ao buffer
        pthread_mutex_lock(&mutex_rcv3);

        printf("\nEnlace.c (server)= > Frame Recebido! tam_buffer: '%d', ecc: '%d', tam_datagrama: '%lu', tam_frame: '%lu'\n", frame_rcv.tam_buffer,
                frame_rcv.ecc, sizeof (frame_rcv.data), sizeof (frame_rcv));

        //Monta Datagrama atravez do Frame recebido
        montarBuffer(frame_rcv);

        printf("Enlace.c (server)= > Datagrama Montado!\n");

        //Recalculo do ECC do Datagrama montado
        sum = checkSum(frame_rcv.data);

        printf("Enlace.c (server) = > ECC Recalculado -> frame_rcv.ECC:'%d', ECC recalculado: '%d'\n", frame_rcv.ecc, sum);

        //Teste do ECC e retorno para cadamada de testeenlace(rede)
        if (frame_rcv.ecc == sum)
            buffer_rede_enlace_rcv.retorno = 0;
        else 
            buffer_rede_enlace_rcv.retorno = -1;

        //Libera acesso exclusivo ao buffer
        pthread_mutex_unlock(&mutex_rcv3);

        //Libera mutex de sincronismo
        pthread_mutex_unlock(&mutex_rcv2);

    }
}

void montarBuffer(struct frame frame) {

    //Monta o buffer
    memcpy(&buffer_rede_enlace_rcv.datagrama, &frame.data, sizeof (frame.data));

    buffer_rede_enlace_rcv.tam_buffer = frame.tam_buffer;

}

void montarFrame(struct frame *frame) {

    //Monta o Datagrama

    frame->ecc = 0;

    frame->tam_buffer = buffer_rede_enlace_env.tam_buffer;

    memcpy(&frame->data, &buffer_rede_enlace_env.datagrama, sizeof (buffer_rede_enlace_env.datagrama));

    //tam_
    buffer_rede_enlace_env.env_no = -1;
    buffer_rede_enlace_env.retorno = 0;

}

int checkSum(struct datagrama datagram) {

    int sum = 0;
    int aux = 0;
    int i;
    void *ptr = &datagram;

    //Percorre o datagrama com um ponteiro de void, somando em inteiros
    for (i = 0; i < sizeof (datagram); i += sizeof (int)) {
        memcpy(&aux, ptr, sizeof (int));
        ptr += sizeof (int);
        sum += aux;
    }
    //Retorna a soma total
    return sum;
}