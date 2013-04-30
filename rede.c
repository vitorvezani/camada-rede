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

    int te, tr, i;
    pthread_t threadReceberSegmento, threadReceberDatagramas;

    /*  inicializacao do buffer */
    for(i = 0; i<MAX_BUFFERS_DESFRAG; i++)
        buffers_fragmentacao[i].id = -1;

    /* Inicia a thread receberSegmento */
    te = pthread_create(&threadReceberSegmento, NULL, receberSegmento, NULL);

    if (te) {
        printf("ERRO: impossivel criar a thread : receberSegmento\n");
        exit(-1);
    }

    /* Inicia a thread receberDatagramas */
    tr = pthread_create(&threadReceberDatagramas, NULL, receberDatagramas, NULL);

    if (tr) {
        printf("ERRO: impossivel criar a thread : receberDatagramas\n");
        exit(-1);
    }

    /* Espera as threads terminarem */
    pthread_join(threadReceberSegmento, NULL);
    pthread_join(threadReceberDatagramas, NULL);
}

void *receberSegmento() {

    int MTU = 0;

    while (1) {

        struct datagrama datagrama_env;

        /* Consumir buffer_trans_rede_env */
        pthread_mutex_lock(&mutex_trans_rede_env2);

            montarDatagramaEnv(&datagrama_env);

        /* Consumir buffer_trans_rede_env */
        pthread_mutex_unlock(&mutex_trans_rede_env1);

        /* Produzir buffer_trans_rede_env */
        pthread_mutex_lock(&mutex_rede_enlace_env1);

            enviarDatagrama(datagrama_env);

        /* Produzir buffer_trans_rede_env */
        pthread_mutex_unlock(&mutex_rede_enlace_env2);

        /* Consome resposta da camada de enlace */
        pthread_mutex_lock(&mutex_rede_enlace_env1);

            MTU = retornoEnlace(datagrama_env);

        /* Consome resposta da camada de enlace */
        pthread_mutex_unlock(&mutex_rede_enlace_env1);

        /* É necessario fragmentar o datagrama? */
        if (MTU > 0)
            fragmentarDatagrama(datagrama_env);

    }
}

void *receberDatagramas() {

    int desfrag;
    int i = 0;
    int index; // vetor de index?

    while (TRUE) {

        struct datagrama datagrama_rcv;

        /* Consumir buffer_rede_enlace_rcv */
        pthread_mutex_lock(&mutex_rede_enlace_rcv2);

            /* Retira datagrama do buffer, retona se é necessario desfragmentar */
            desfrag = montarDatagramaRcv(&datagrama_rcv);

        /* Consumir buffer_rede_enlace_rcv */
        pthread_mutex_unlock(&mutex_rede_enlace_rcv1);

        /* É um datagrama IP? */
        if (datagrama_rcv.type == 1)
        {
            /* É necessario desfragmentar o datagrama? */
            if (desfrag)
                desfragmentarDatagrama(datagrama_rcv, &index);

            /* Produzir buffer_trans_rede_rcv */
            pthread_mutex_lock(&mutex_trans_rede_rcv1);

            /* É um datagrama unico? */
            if(datagrama_rcv.mf == -1){

                /* Coloca no buffer_trans_rede_rcv */
                enviarSegmento(datagrama_rcv);

            /* É o ultimo datagrama a ser framentado? */
            }else if(datagrama_rcv.mf == 2){

                /* Coloca no buffer_trans_rede_rcv */
                enviarSegmento(buffers_fragmentacao[index]);
            }

            /* Produzir buffer_trans_rede_rcv */
            pthread_mutex_unlock(&mutex_trans_rede_rcv2);

        /* É um datagrama de roteamento? */
        }else if (datagrama_rcv.type == 2)
        {
            // alimentarTabeladeRotas();
        }
    }
}

void desfragmentarDatagrama(struct datagrama datagram, int *index){
    int i;
    int flag = -1;
    void *aux;

    /* Implementar MUTEX buffers_fragmentacao[i] */

    write(1,&datagram.data,datagram.tam_buffer);

    for (i = 0; i < MAX_BUFFERS_DESFRAG; ++i) {

        /* Há datagramas anteriores com o mesmo id? */
        if (buffers_fragmentacao[i].id == datagram.id)
        {
            flag = i;
	    	break;
        }
    }
    if (flag == -1){
		for(i = 0; i < MAX_BUFFERS_DESFRAG; ++i) {

            /* Não há datagramas anteriores, ache a primeira posicao vazia */
			if(buffers_fragmentacao[i].id == -1) {
				flag = i;
				break;
			}
		}
	}

    /* index = flag */
    *index = flag;
    /* ponteiro recebe posicao de memoria */
    aux = &buffers_fragmentacao[i].data;
    /* pronteiro soma offset */
    aux += datagram.offset;
    /* copia conteudo do datagrama para posicao de memoria apontado por aux, tamanho de tam_buffer */
    memcpy(aux, &datagram.data, datagram.tam_buffer);

}

void fragmentarDatagrama(struct datagrama datagram){

    int i;
    int tam_total_datagrama = sizeof(datagram.data) - (100 - datagram.tam_buffer);    	//Tamanho efetivo para dividir (desconsidera lixo)
    int qtde_divisao = 0;                                                               //Quantidade que será dividido os datagramas
    int tam_parte_final = 0;                                                            //Tamanho do ultimo datagrama
    int env_no = datagram.env_no;                                                       //Env_no
    int retorno = datagram.env_no;                                                      //Retorno
    int offset = 0;                                                                     //Offset para gravar
    int MTU = buffer_rede_enlace_env.retorno;                                           //MTU
    char buffer_interno[tam_total_datagrama];                                         	//Buffer carrega o datagrama

    /* Copia conteudo para o buffer */
    memcpy(buffer_interno,&datagram.data,tam_total_datagrama);

    void *ptr = buffer_interno;

        printf("[REDE - FRAG] MTU excedido dividindo o pacote em max '%d' bytes\n\n", MTU);

        /* Se o ultimo datagrama tiver resto */
        if (tam_total_datagrama % MTU != 0){
            qtde_divisao = (tam_total_datagrama) / MTU + 1;
            tam_parte_final = (tam_total_datagrama) % MTU;
            printf("[REDE - FRAG] Tamanho total : '%d'.Dividirei em '%d' partes\n",(tam_total_datagrama), qtde_divisao);
        }
        /* Se o ultimo datagrama não tiver resto */
        else{
            qtde_divisao = tam_total_datagrama / MTU;
            tam_parte_final = 0;
            printf("[REDE - FRAG] Tamanho total : '%d'.Dividirei o datagrama em '%d' partes\n",(tam_total_datagrama), qtde_divisao);
        }

        /* Loop para mandar os fragmentos dos datagramas*/
        for (i = 0; i < qtde_divisao; i++)
        {
            struct datagrama datagrama_env_aux;

            datagrama_env_aux.type = 1;
            datagrama_env_aux.env_no = env_no;
            datagrama_env_aux.offset = offset;
            datagrama_env_aux.retorno = retorno;
            datagrama_env_aux.id = id;

            printf("[REDE - FRAG] Offset: '%d'\n",datagrama_env_aux.offset);

            /* Primeiro e Demais pacotes (exceto ultimo) */
            if (i + 1 != qtde_divisao) 
            {
                datagrama_env_aux.tam_buffer = MTU; // Atualiza Tam_buffer
                datagrama_env_aux.mf = 1; // More Fragments

                memcpy(&datagrama_env_aux.data, ptr , MTU); // Copia Dados em Datagrama Auxiliar

                #ifdef DEBBUG_REDE_FRAGMENTAR
                    write(1,ptr,MTU); //  Printa o que enviou
                #endif

                offset += MTU;  // Atualiza Offset
                ptr += MTU;     // Move Ponteiro
            }

            /* Se for o ultimo pacote e nao tem sobra */
            else if (i + 1 == qtde_divisao && tam_parte_final == 0) 
            {
                datagrama_env_aux.tam_buffer = MTU; // Atualiza Tam_buffer
                datagrama_env_aux.mf = 2; // More Fragments

                /* Copia Dados em Datagrama Auxiliar */
                memcpy(&datagrama_env_aux.data, ptr , MTU);

                #ifdef DEBBUG_REDE_FRAGMENTAR
                    write(1,ptr,MTU); //Printa o que enviou
                #endif
            }

            /* Se for o ultimo pacote e tem sobra */
            else if(i + 1 == qtde_divisao && tam_parte_final != 0)
            {
                datagrama_env_aux.tam_buffer = tam_parte_final; // Atualiza Tam_buffer
                datagrama_env_aux.mf = 2; // More Fragments

                memcpy(&datagrama_env_aux.data, ptr , tam_parte_final); // Copia Dados em Datagrama Auxiliar

                #ifdef DEBBUG_REDE_FRAGMENTAR
                    write(1,ptr,tam_parte_final); // Printa o que enviou
                #endif
            }

            /*  Qualqr outro >erro fatal<  */
            else
            {
                printf("[REDE - FRAG] Erro Fatal(2)\n\n"); //  Estrouro de Excessão
                exit(2);
            }

            /* Produzir buffer_trans_rede_env */
            pthread_mutex_lock(&mutex_rede_enlace_env1);

                /* Coloca Datagrama no buffer_trans_rede_env */
                enviarDatagrama(datagrama_env_aux);

            /* Produzir buffer_trans_rede_env */
            pthread_mutex_unlock(&mutex_rede_enlace_env2);

            /* Consome resposta da camada de enlace */
            pthread_mutex_lock(&mutex_rede_enlace_env1);

                /* Testa retorno */
                retornoEnlace(datagrama_env_aux);

            /* Consome resposta da camada de enlace */
            pthread_mutex_unlock(&mutex_rede_enlace_env1);
        }
}

int retornoEnlace(struct datagrama datagrama_env) {

    if (buffer_rede_enlace_env.tam_buffer != -1)
    {
        /* Testa o retorno da camada de enlace */
        if (buffer_rede_enlace_env.retorno == 0) {
        printf("[REDE - ENVIAR] OK\n\n");
        } else if (buffer_rede_enlace_env.retorno == -1) {
        printf("[REDE - ENVIAR] Não há ligacao do nó: '%d'!\n\n", buffer_rede_enlace_env.env_no);
        } else if (buffer_rede_enlace_env.retorno > 0) {
        printf("[REDE - ENVIAR] MTU Excedido MAX '%d'\n", buffer_rede_enlace_env.retorno);
            return buffer_rede_enlace_env.retorno;
        } else{
        printf("[REDE - ENVIAR] Erro Fatal(1)\n\n");
        exit(1);
        }
    }
    return 0;
}

void enviarDatagrama(struct datagrama datagrama_env) {

        buffer_rede_enlace_env.tam_buffer = datagrama_env.tam_buffer;
        buffer_rede_enlace_env.env_no = datagrama_env.env_no;
		buffer_rede_enlace_env.retorno = datagrama_env.retorno;

        memcpy(&buffer_rede_enlace_env.data,&datagrama_env, sizeof(datagrama_env));
}

int montarDatagramaRcv(struct datagrama *datagram){

		if (buffer_rede_enlace_rcv.tam_buffer != -1){

			if(buffer_rede_enlace_rcv.retorno != -1){

				printf("[REDE - RECEBER] Datagrama Recebido com sucesso\n");

				memcpy(datagram, &buffer_rede_enlace_rcv.data, sizeof (buffer_rede_enlace_rcv.data));

				printf("[REDE - RECEBER] Type: '%d', Tam_buffer: '%d' Bytes, ID: '%d', offset: '%d', MF: '%d'\n", datagram->type, datagram->tam_buffer,
				datagram->id,datagram->offset,datagram->mf);

                /* Tem mais fragmentos */
				if (datagram->mf > 0)
				{
					return 1;
				}
			}else
				printf("[REDE - RECEBER] ECC não correspondente - Datagrama Descartado");
        }
    return 0;
}

void enviarSegmento(struct datagrama datagram) {

	buffer_trans_rede_rcv.tam_buffer = -9;
    memcpy(&buffer_trans_rede_rcv.data, &datagram.data, sizeof (datagram.data));

}

void montarDatagramaEnv(struct datagrama *datagram){

    memcpy(&(datagram->data), &buffer_trans_rede_env.data, sizeof (buffer_trans_rede_env.data));

    datagram->type = 1;
    datagram->env_no = buffer_trans_rede_env.env_no;
    datagram->tam_buffer = buffer_trans_rede_env.tam_buffer;
    datagram->id = id;
    datagram->offset = -1;
    datagram->mf = -1;

    id++;
}
