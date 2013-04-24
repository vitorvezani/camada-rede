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

        int MTU;

        struct datagrama datagrama_env;

        //Trava o Mutex de sincronismo
        pthread_mutex_lock(&mutex_trans_rede_env2);

            montarDatagramaEnv(&datagrama_env);

        //Destrava mutex de sincronismo
        pthread_mutex_unlock(&mutex_trans_rede_env1);

        //Produz
        pthread_mutex_lock(&mutex_rede_enlace_env1);

            enviarDatagrama(datagrama_env);

        pthread_mutex_unlock(&mutex_rede_enlace_env2);

        //Consome resposta da camada de enlace
        pthread_mutex_lock(&mutex_rede_enlace_env1);

            MTU = retornoEnlace(datagrama_env); 

        pthread_mutex_unlock(&mutex_rede_enlace_env1);

            if (MTU > 0)
                fragmentarDatagrama(datagrama_env);

    }
}

void *receberDatagramas() {

    while (TRUE) {

    int desfrag;

        struct datagrama datagrama_rcv;

        //Trava mutex de sincronismo
        pthread_mutex_lock(&mutex_rede_enlace_rcv2);

            desfrag = montarDatagramaRcv(&datagrama_rcv);

        if (datagrama_rcv.type = 1) // Tipo de Datagrama Normal
        {
            if (desfrag)
            {
                desfragmentarDatagrama(datagrama_rcv);

            }else{
                        //Destrava mutex de sinconismo
            pthread_mutex_unlock(&mutex_rede_enlace_rcv1);

            pthread_mutex_lock(&mutex_trans_rede_rcv1);

                enviarSegmento(datagrama_rcv);

            pthread_mutex_unlock(&mutex_trans_rede_rcv2);
            }

        }else if (datagrama_rcv.type = 2) // tipo de Datagrama de Roteamento
        {
            //alimentarTabeladeRotas();
        }
    }
}

void desfragmentarDatagrama(struct datagrama datagram){
    int i;

    for (i = 0; i < MAX_BUFFERS_DESFRAG; ++i)
    {
        if (buffers_desfrag[i].id = datagram.id)
        {
            //Buffer ja existe para ID
        }else
        {
            //Buffer nao existe para ID
        }
    }
    datagram.tam_buffer;
    datagram.offset;
    datagram.id;
    datagram.tamanho_total;
}

void fragmentarDatagrama(struct datagrama datagram){

    int i;
    int qtde_divisao = 0; // Quantidade que será dividido os datagramas
    int tam_parte_final = 0; // Tamanho do ultimo datagrama
    int offset = 0;
    int MTU = buffer_rede_enlace_env.retorno;
    int tamanho_total = datagram.tam_buffer;
    char buffer_interno[100];

    memcpy(buffer_interno,&datagram.data.buffer,100);

    write(1,buffer_interno,100);

    void *ptr = buffer_interno;

        printf("Rede.c (Enviar - Retorno) = > MTU excedido dividindo o pacote em max '%d' bytes\n\n", MTU);

        if ((datagram.tam_buffer) % MTU != 0){
            qtde_divisao = (datagram.tam_buffer) / MTU + 1;
            tam_parte_final = (datagram.tam_buffer) % MTU;
            printf("Tamanho total : '%d'.Dividirei em '%d' partes\n",(datagram.tam_buffer), qtde_divisao);
        }
        else{
            qtde_divisao = datagram.tam_buffer / MTU;
            tam_parte_final = 0;
            printf("Dividirei o datagrama em '%d' partes\n",qtde_divisao);
        }

                for (i = 0; i < qtde_divisao; i++)
                {
                    struct datagrama datagrama_env_aux;

                    datagrama_env_aux.type = 1;
                    datagrama_env_aux.env_no = datagram.env_no;
                    datagrama_env_aux.offset = offset;
                    datagrama_env_aux.tamanho_total = tamanho_total;
                    datagrama_env_aux.id = id;

                    if (i + 1 != qtde_divisao)
                    {
                        datagrama_env_aux.tam_buffer = MTU;
                        datagrama_env_aux.mf = 1; //More Fragments
                        memcpy(&(datagrama_env_aux.data), ptr , MTU);

                        printf("Vou enviar o '%d' pacote\n", i);
                        write(1,ptr,MTU);

                        offset += MTU;
                        ptr += MTU;
                    }

                    else if (i + 1 == qtde_divisao && tam_parte_final == 0) //se for o ultimo pacote e nao tem sobra
                    {
                        datagrama_env_aux.tam_buffer = MTU;
                        datagrama_env_aux.mf = 0; //More Fragments
                        memcpy(&datagrama_env_aux.data, ptr , MTU);

                        printf("Vou enviar o '%d' pacote\n", i);
                        write(1,ptr,MTU);
                 
                    }
                    else if(i + 1 == qtde_divisao && tam_parte_final != 0)//se for o ultimo pacote e tem sobra
                    {
                        datagrama_env_aux.tam_buffer = tam_parte_final;
                        datagrama_env_aux.mf = 0; //More Fragments
                        memcpy(&datagrama_env_aux.data, ptr , tam_parte_final);

                        printf("Vou enviar o '%d' pacote\n", i);
                        write(1,ptr,tam_parte_final);

                    }
                    else // se nao for o ultimo pacote
                    {
                        printf("Rede.c (Enviar - Retorno) = > Erro Fatal(2)\n\n");
                        exit(2);
                    }

                    pthread_mutex_lock(&mutex_rede_enlace_env1);

                        enviarDatagrama(datagrama_env_aux);

                    pthread_mutex_unlock(&mutex_rede_enlace_env2);

                    //Consome resposta da camada de enlace
                    pthread_mutex_lock(&mutex_rede_enlace_env1);

                        retornoEnlace(datagrama_env_aux); 

                    pthread_mutex_unlock(&mutex_rede_enlace_env1);

                    printf("Enviei o '%d' pacote\n",i);
                }
            id++;
}

int retornoEnlace(struct datagrama datagrama_env) {

    if (buffer_rede_enlace_env.tam_buffer != 0)
    {
        //Testa o retorno da camada de enlace
        if (buffer_rede_enlace_env.retorno == 0) {
        printf("Rede.c (Enviar - Retorno) = > OK\n\n");
        } else if (buffer_rede_enlace_env.retorno == -1) {
        printf("Rede.c (Enviar - Retorno) = > Não há ligacao do nó: '%d'!\n\n", buffer_rede_enlace_env.env_no);
        } else if (buffer_rede_enlace_env.retorno > 0) {
        printf("MTU Excedido MAX '%d'\n", buffer_rede_enlace_env.retorno);
            return buffer_rede_enlace_env.retorno;
        } else{
        printf("Rede.c (Enviar - Retorno) = > Erro Fatal(1)\n\n");
        exit(1);
        }
        return 0;
    }
}

void enviarDatagrama(struct datagrama datagrama_env) {

        buffer_rede_enlace_env.tam_buffer = datagrama_env.tam_buffer;
        buffer_rede_enlace_env.env_no = datagrama_env.env_no;
        buffer_rede_enlace_env.retorno = -1;

        memcpy(&buffer_rede_enlace_env.data,&datagrama_env, sizeof(datagrama_env));
}

int montarDatagramaRcv(struct datagrama *datagram){

        if (buffer_rede_enlace_rcv.tam_buffer != 0){

            if (buffer_rede_enlace_rcv.retorno == 0)
            {
                printf("Rede.c (Receber) = > Datagrama Recebido com sucesso\n");

                memcpy(datagram, &buffer_rede_enlace_rcv.data, sizeof (buffer_rede_enlace_rcv.data));
                
                printf("Rede.c (Receber) = > Type: '%d', Tam_buffer: '%d' Bytes, Tam Total: '%d', ID: '%d', offset: '%d', MF: '%d'\n", datagram->type, datagram->tam_buffer,
                datagram->tamanho_total,datagram->id,datagram->offset,datagram->mf);

                if (datagram->mf != -1) // tem mais fragmentos
                {
                    return 1;
                }

            }
            else if (buffer_rede_enlace_rcv.retorno == -1){
                printf("Rede.c (Receber) = > ERRO: Datagrama corrompido e descartado!\n");
            }
        }
    return 0;    
}

void enviarSegmento(struct datagrama datagram) {

    buffer_trans_rede_rcv.tam_buffer = datagram.tam_buffer;
    memcpy(&buffer_trans_rede_rcv.data, &datagram.data, sizeof (datagram.data));

}

void montarDatagramaEnv(struct datagrama *datagram){

        datagram->type = 1;
        datagram->env_no = buffer_trans_rede_env.env_no;
        datagram->tam_buffer = buffer_trans_rede_env.tam_buffer;
        datagram->offset = -1;
        datagram->id = -1;
        datagram->tamanho_total = -1;
        datagram->mf = -1; //More Fragments

        memcpy(&(datagram->data), &buffer_trans_rede_env.data, sizeof (buffer_trans_rede_env.data));

}