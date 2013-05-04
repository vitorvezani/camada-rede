//
//  rede.c
//
//  Guilherme Sividal - 09054512
//  Vitor Rodrigo Vezani - 10159861
//
//  Created by Vitor Vezani on 19/03/13.
//  Copyright (c) 2013 Vitor Vezani. All rights reserved.
//

#include "headers/arquivo.h"
#include "headers/rede.h"

void *iniciarRede() {

    int te, tr, tetr, trtr, ted, tes, i;
    pthread_t threadReceberSegmento, threadReceberDatagramas, threadEnviarTabelaRotas, threadReceberTabelaRotas, threadEnviarDatagrama, threadEnviarSegmento;

    /* inicializacao do buffer */
    for (i = 0; i < MAX_BUFFERS_DESFRAG; i++)
        buffers_fragmentacao[i].id = -1;

    /* Inicializa topologia da rede*/
    montarTabelaRotasInicial();

    /* Inicia a thread threadReceberSegmento */
    te = pthread_create(&threadReceberSegmento, NULL, receberSegmento, NULL);

    if (te) {
        printf("ERRO: impossivel criar a thread : receberSegmento\n");
        exit(-1);
    }

    /* Inicia a thread threadReceberDatagramas */
    tr = pthread_create(&threadReceberDatagramas, NULL, receberDatagramas, NULL);

    if (tr) {
        printf("ERRO: impossivel criar a thread : receberDatagramas\n");
        exit(-1);
    }

    /* Inicia a thread threadReceberTabelaRotas */
    trtr = pthread_create(&threadReceberTabelaRotas, NULL, receberTabelaRotas, NULL);

    if (trtr) {
        printf("ERRO: impossivel criar a thread : receberTabelaRotas\n");
        exit(-1);
    }

    /* Inicia a thread threadEnviarDatagrama */
    tes = pthread_create(&threadEnviarSegmento, NULL, enviarSegmento, NULL);

    if (tes) {
        printf("ERRO: impossivel criar a thread : enviarSegmento\n");
        exit(-1);
    }

    /* Inicia a thread threadEnviarDatagrama */
    ted = pthread_create(&threadEnviarDatagrama, NULL, enviarDatagrama, NULL);

    if (ted) {
        printf("ERRO: impossivel criar a thread : enviarDatagrama\n");
        exit(-1);
    }

    /* Inicia a thread EnviarTabelaRotas */
    tetr = pthread_create(&threadEnviarTabelaRotas, NULL, enviarTabelaRotas, NULL);

    if (tetr) {
        printf("ERRO: impossivel criar a thread : enviarTabelaRotas\n");
        exit(-1);
    }

    /* Espera as threads terminarem */
    pthread_join(threadEnviarTabelaRotas, NULL);
    pthread_join(threadReceberTabelaRotas, NULL);  
    pthread_join(threadReceberSegmento, NULL);
    pthread_join(threadReceberDatagramas, NULL);
    pthread_join(threadEnviarDatagrama, NULL);
    pthread_join(threadEnviarSegmento, NULL);
}

void *receberSegmento() {

    while (1) {

        struct datagrama datagrama_env;

        /* Consumir buffer_trans_rede_env */
        pthread_mutex_lock(&mutex_trans_rede_env2);

            retirarDatagramaTransRedeEnv(&datagrama_env);

        /* Consumir buffer_trans_rede_env */
        pthread_mutex_unlock(&mutex_trans_rede_env1);

        /* Produzir buffer_rede_rede_env */
        pthread_mutex_lock(&mutex_rede_rede_env1);

            colocarDatagramaBufferRedeRedeEnv(datagrama_env);

        /* Produzir buffer_rede_rede_env */
        pthread_mutex_unlock(&mutex_rede_rede_env2);
    }
}

void *enviarDatagrama(){

int MTU = 0;

    while(TRUE){

        struct datagrama datagrama_env;

        /* Consumir buffer_rede_rede_env */
        pthread_mutex_lock(&mutex_rede_rede_env2);

            retirarDatagramaBufferRedeRedeEnv(&datagrama_env);

        /* Consumir buffer_rede_rede_env */
        pthread_mutex_unlock(&mutex_rede_rede_env1);

        /* Produzir buffer_trans_rede_env */
        pthread_mutex_lock(&mutex_rede_enlace_env1);

            colocarDatagramaBufferTransRedeEnv(datagrama_env);

            printf("Enviei o datagrama\n");

        /* Produzir buffer_trans_rede_env */
        pthread_mutex_unlock(&mutex_rede_enlace_env2);

        /* Consome resposta da camada de enlace */
        pthread_mutex_lock(&mutex_rede_enlace_env1);

            MTU = retornoEnlace(datagrama_env);

        /* Consome resposta da camada de enlace */
        pthread_mutex_unlock(&mutex_rede_enlace_env1);

        /* É necessario fragmentar o datagrama? */
        if (MTU > 0)
            fragmentarDatagramaEnv(datagrama_env);
    
    }

}

void *receberDatagramas() {

    int i = 0;
    int index; // vetor de index?

    while (TRUE) {

        struct datagrama datagrama_rcv;

        /* Consumir buffer_rede_enlace_rcv */
        pthread_mutex_lock(&mutex_rede_enlace_rcv2);

        /* Retira datagrama do buffer */
            retirarDatagramaBufferRedeEnlaceRcv(&datagrama_rcv);

        /* Consumir buffer_rede_enlace_rcv */
        pthread_mutex_unlock(&mutex_rede_enlace_rcv1);

        /* É um datagrama IP? */
        if (datagrama_rcv.type == 1) {

            /* É necessario desfragmentar o datagrama? */
            if (datagrama_rcv.mf >= 0)
                desfragmentarDatagramaRcv(datagrama_rcv, &index);

            if (datagrama_rcv.mf == -1 || datagrama_rcv.mf == 0)
            {
                /* Produzir buffer_rede_rede_rcv */
                pthread_mutex_lock(&mutex_rede_rede_rcv1);

                /* É um datagrama unico? */
                if (datagrama_rcv.mf == -1) {

                    colocarDatagramaBufferRedeRedeRcv(datagrama_rcv);

                /* Foi o ultimo datagrama a ser framentado? */
                } else if (datagrama_rcv.mf == 0) {

                    colocarDatagramaBufferRedeRedeRcv(buffers_fragmentacao[index]);
                    resetarBuffer(&buffers_fragmentacao[index]);
                }

                /* Produzir buffer_rede_rede_rcv */
                pthread_mutex_unlock(&mutex_rede_rede_rcv2);
            }

        /* É um datagrama de roteamento? */
        } else if (datagrama_rcv.type == 2) {

            /* Produzir buffer_rede_rede_rcv */
            pthread_mutex_lock(&mutex_rede_rede_rcv1);

                colocarDatagramaBufferRedeRedeRcv(datagrama_rcv);

            /* Produzir buffer_rede_rede_rcv */
            pthread_mutex_unlock(&mutex_rede_rede_receberotas2);

        }
    }
}

void *enviarSegmento() {

    while(TRUE){

    struct datagrama datagrama_rcv;

        /* Consumir buffer_rede_rede_rcv */
        pthread_mutex_lock(&mutex_rede_rede_rcv2);

            retirarDatagramaBufferRedeRedeRcv(&datagrama_rcv);

        /* Consumir buffer_rede_rede_rcv */
        pthread_mutex_unlock(&mutex_rede_rede_rcv1);

        /* Produzir buffer_trans_rede_rcv */
        pthread_mutex_lock(&mutex_trans_rede_rcv1);

            colocarDatagramaBufferTransRedeRcv(datagrama_rcv);

        /* Produzir buffer_trans_rede_rcv */
        pthread_mutex_unlock(&mutex_trans_rede_rcv2);

    }
}

void *enviarTabelaRotas(){

    int i;

   while(TRUE){

        struct datagrama datagrama_env_inicial, datagrama_env;

        if (iniciei == 1)
        {

            montarDatagramaTabelaRotas(&datagrama_env_inicial);

            enviarTabelaRotasVizinhos(&datagrama_env_inicial);

            iniciei = 0;

        }

        pthread_mutex_lock(&mutex_rede_rede_atualizei2);

            montarDatagramaTabelaRotas(&datagrama_env);

            printf("Montei datagrama para enviar tabela\n");

        pthread_mutex_unlock(&mutex_rede_rede_atualizei1);

        // Produzir buffer_rede_rede_env
        pthread_mutex_lock(&mutex_rede_rede_env1);

            enviarTabelaRotasVizinhos(&datagrama_env);

        // Produzir buffer_rede_rede_env
        pthread_mutex_unlock(&mutex_rede_rede_env2);
   }
}

void *receberTabelaRotas(){

   while(TRUE){

    struct datagrama datagrama_rcv;

        /* Consumir buffer_rede_rede_rcv */
        pthread_mutex_lock(&mutex_rede_rede_receberotas2);

            retirarDatagramaBufferRedeRedeRcv(&datagrama_rcv);

        /* Consumir buffer_rede_rede_rcv */
        pthread_mutex_unlock(&mutex_rede_rede_rcv1);

        /* Travar e Desbloquear a threadEnviarTabelaRotas */
        pthread_mutex_lock(&mutex_rede_rede_atualizei1);

            //atualizarTabelaRotas(datagrama_rcv);
            printf("Atualizei Tabela de Rotas\n");

        pthread_mutex_unlock(&mutex_rede_rede_atualizei2);

   }

}

void enviarTabelaRotasVizinhos(struct datagrama *datagram){
    int i;

    for (i = 0; i < 7; i++)
    {
        if (tabela_rotas[6][i] != -1)
        {

            // Produzir buffer_rede_rede_env
            pthread_mutex_lock(&mutex_rede_rede_env1);

                datagram->env_no = tabela_rotas[6][i];

                printf("Enviei Tabela de Rotas para o '%d'\n",tabela_rotas[6][i]);

                colocarDatagramaBufferRedeRedeEnv(*datagram);

            // Produzir buffer_rede_rede_env
            pthread_mutex_unlock(&mutex_rede_rede_env2);
        }
    }
}

void retirarDatagramaBufferRedeRedeRcv(struct datagrama *datagram){

    memcpy(datagram, &buffer_rede_rede_rcv, sizeof (buffer_rede_rede_rcv));

}

void desfragmentarDatagramaRcv(struct datagrama datagram, int *index) {
    int i;
    int flag = -1;
    void *aux;

    /* Implementar MUTEX buffers_fragmentacao[i] */

    for (i = 0; i < MAX_BUFFERS_DESFRAG; ++i) {

        /* Há datagramas anteriores com o mesmo id? */
        if (buffers_fragmentacao[i].id == datagram.id) {
            flag = i;
            break;
        }
    }
    if (flag == -1) {
        for (i = 0; i < MAX_BUFFERS_DESFRAG; ++i) {

            /* Não há datagramas anteriores, ache a primeira posicao vazia */
            if (buffers_fragmentacao[i].id == -1) {
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

void fragmentarDatagramaEnv(struct datagrama datagram) {

    int i;
    int tam_total_datagrama = sizeof (datagram.data) - (TAM_MAX_BUFFER - datagram.tam_buffer); //Tamanho efetivo para dividir (desconsidera lixo)
    int qtde_divisao = 0; //Quantidade que será dividido os datagramas
    int tam_parte_final = 0; //Tamanho do ultimo datagrama
    int env_no = datagram.env_no; //Env_no
    int retorno = datagram.env_no; //Retorno
    int offset = 0; //Offset para gravar
    int MTU = buffer_rede_enlace_env.retorno; //MTU
    char buffer_interno[tam_total_datagrama]; //Buffer carrega o datagrama

    /* Copia conteudo para o buffer */
    memcpy(buffer_interno, &datagram.data, tam_total_datagrama);

    void *ptr = buffer_interno;

    printf("[REDE - FRAG] MTU excedido dividindo o pacote em max '%d' bytes\n\n", MTU);

    /* Se o ultimo datagrama tiver resto */
    if (tam_total_datagrama % MTU != 0) {
        qtde_divisao = (tam_total_datagrama) / MTU + 1;
        tam_parte_final = (tam_total_datagrama) % MTU;
        printf("[REDE - FRAG] Tamanho total : '%d'.Dividirei em '%d' partes\n", (tam_total_datagrama), qtde_divisao);
    }        /* Se o ultimo datagrama não tiver resto */
    else {
        qtde_divisao = tam_total_datagrama / MTU;
        tam_parte_final = 0;
        printf("[REDE - FRAG] Tamanho total : '%d'.Dividirei o datagrama em '%d' partes\n", (tam_total_datagrama), qtde_divisao);
    }

    /* Loop para mandar os fragmentos dos datagramas*/
    for (i = 0; i < qtde_divisao; i++) {
        struct datagrama datagrama_env_aux;

        datagrama_env_aux.type = 1;
        datagrama_env_aux.env_no = env_no;
        datagrama_env_aux.offset = offset;
        datagrama_env_aux.retorno = retorno;
        datagrama_env_aux.id = id;

        printf("[REDE - FRAG] Offset: '%d'\n", datagrama_env_aux.offset);

        /* Primeiro e Demais pacotes (exceto ultimo) */
        if (i + 1 != qtde_divisao) {
            datagrama_env_aux.tam_buffer = MTU; // Atualiza Tam_buffer
            datagrama_env_aux.mf = 1; // More Fragments

            memcpy(&datagrama_env_aux.data, ptr, MTU); // Copia Dados em Datagrama Auxiliar

    #ifdef DEBBUG_REDE_FRAGMENTAR
            //write(1, ptr, MTU); //  Printa o que enviou
    #endif

            offset += MTU; // Atualiza Offset
            ptr += MTU; // Move Ponteiro
        }
            /* Se for o ultimo pacote e nao tem sobra */
        else if (i + 1 == qtde_divisao && tam_parte_final == 0) {
            datagrama_env_aux.tam_buffer = MTU; // Atualiza Tam_buffer
            datagrama_env_aux.mf = 0; // More Fragments

            /* Copia Dados em Datagrama Auxiliar */
            memcpy(&datagrama_env_aux.data, ptr, MTU);

    #ifdef DEBBUG_REDE_FRAGMENTAR
            //write(1, ptr, MTU); //Printa o que enviou
    #endif
        }
            /* Se for o ultimo pacote e tem sobra */
        else if (i + 1 == qtde_divisao && tam_parte_final != 0) {
            datagrama_env_aux.tam_buffer = tam_parte_final; // Atualiza Tam_buffer
            datagrama_env_aux.mf = 0; // More Fragments

            memcpy(&datagrama_env_aux.data, ptr, tam_parte_final); // Copia Dados em Datagrama Auxiliar

    #ifdef DEBBUG_REDE_FRAGMENTAR
            //write(1, ptr, tam_parte_final); // Printa o que enviou
    #endif
        }
            /* Qualqr outro >erro fatal< */
        else {
            printf("[REDE - FRAG] Erro Fatal(2)\n\n"); //  Estrouro de Excessão
            exit(2);
        }

        /* Produzir buffer_trans_rede_env */
        pthread_mutex_lock(&mutex_rede_enlace_env1);

        /* Coloca Datagrama no buffer_trans_rede_env */
        colocarDatagramaBufferTransRedeEnv(datagrama_env_aux);

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

void colocarDatagramaBufferRedeRedeEnv(struct datagrama datagrama_env) {

memcpy(&buffer_rede_rede_env, &datagrama_env, sizeof (datagrama_env));

}

void colocarDatagramaBufferTransRedeEnv(struct datagrama datagrama_env) {

    buffer_rede_enlace_env.tam_buffer = datagrama_env.tam_buffer;
    buffer_rede_enlace_env.env_no = datagrama_env.env_no;
    buffer_rede_enlace_env.retorno = datagrama_env.retorno;

    memcpy(&buffer_rede_enlace_env.data, &datagrama_env, sizeof (datagrama_env));
}

void colocarDatagramaBufferRedeRedeRcv(struct datagrama datagram) {

    memcpy(&buffer_rede_rede_rcv, &datagram, sizeof (datagram));

}

void colocarDatagramaBufferTransRedeRcv(struct datagrama datagram) {

    int tam_buffer = datagram.tam_buffer;
    int env_no = datagram.env_no;
    int retorno = 1;

    memcpy(&buffer_trans_rede_rcv.data, &datagram.data.segmento, sizeof (datagram.data.segmento));

}

void retirarDatagramaBufferRedeEnlaceRcv(struct datagrama *datagram) {

    if (buffer_rede_enlace_rcv.retorno != -1) {

        printf("[REDE - RECEBER] Datagrama Recebido com sucesso\n");

        memcpy(datagram, &buffer_rede_enlace_rcv.data, sizeof (buffer_rede_enlace_rcv.data));

        printf("[REDE - RECEBER] Type: '%d', Tam_buffer: '%d' Bytes, ID: '%d', offset: '%d', MF: '%d'\n", datagram->type, datagram->tam_buffer,
                datagram->id, datagram->offset, datagram->mf);
    } else
        printf("[REDE - RECEBER] ECC não correspondente - Datagrama Descartado");

}

void retirarDatagramaTransRedeEnv(struct datagrama *datagram) {

    memcpy(&(datagram->data), &buffer_trans_rede_env.data, sizeof (buffer_trans_rede_env.data));

    datagram->type = 1;
    datagram->env_no = buffer_trans_rede_env.env_no;
    datagram->tam_buffer = buffer_trans_rede_env.tam_buffer;
    datagram->id = id;
    datagram->offset = 0;
    datagram->mf = -1;

}

void retirarDatagramaBufferRedeRedeEnv(struct datagrama *datagram) {

    memcpy(datagram, &buffer_rede_rede_env, sizeof (buffer_rede_rede_env));
    
    id++;

}

int retornoEnlace(struct datagrama datagrama_env) {

    if (buffer_rede_enlace_env.tam_buffer != -1) {
        /* Testa o retorno da camada de enlace */
        if (buffer_rede_enlace_env.retorno == 0) {
            printf("[REDE - ENVIAR] OK\n\n");
        } else if (buffer_rede_enlace_env.retorno == -1) {
            printf("[REDE - ENVIAR] Não há ligacao do nó: '%d'!\n\n", buffer_rede_enlace_env.env_no);
        } else if (buffer_rede_enlace_env.retorno > 0) {
            printf("[REDE - ENVIAR] MTU Excedido MAX '%d'\n", buffer_rede_enlace_env.retorno);
            return buffer_rede_enlace_env.retorno;
        } else {
            printf("[REDE - ENVIAR] Erro Fatal(1)\n\n");
            exit(1);
        }
    }
    return 0;
}

void resetarBuffer(struct datagrama *datagram){

    char aux[sizeof(datagram->data)] = "";

    datagram->tam_buffer = -1;
    datagram->offset = -1;
    datagram->id = -1;
    datagram->mf = -1;
    datagram->type = -1;
    datagram->env_no = -1;
    datagram->retorno = -1;
    memcpy(&(datagram->data), aux, sizeof(datagram->data));

}

void montarDatagramaTabelaRotas(struct datagrama *datagram){

    datagram->tam_buffer = 10;// MUDAR PARA SIZEOF(tabela_rotas)
    datagram->offset = 0;
    datagram->id = id;
    datagram->mf = -1;
    datagram->type = 2;
    datagram->retorno = -1;
    memcpy(&(datagram->data), &tabela_rotas, sizeof(tabela_rotas));

}

void montarTabelaRotasInicial(){
  int i,j;
  int atoiresult;

  for (i = 0; i < 7; i++){
      for (j = 0; j < 7; j++)
          if (i == j)
              tabela_rotas[i][j] = 0;
          else
              tabela_rotas[i][j] = INFINITO;
  }

  for (i = 0; i < 7; i++)
    tabela_rotas[i][6] = -1;

  for (j = 0; j < 7; j++)
    tabela_rotas[6][j] = -1;

  tabela_rotas[file_info.num_no - 1][6] = file_info.num_no;

  for (i = 0; i < 18; i++)
  {
      if(file_info.num_no == ligacao.enlaces[i][0]){
         tabela_rotas[file_info.num_no - 1][ligacao.enlaces[i][1] - 1] = 1;
         tabela_rotas[6][ligacao.enlaces[i][1] - 1] = ligacao.enlaces[i][1];
      }
      else if(file_info.num_no == ligacao.enlaces[i][1]){
         tabela_rotas[file_info.num_no - 1][ligacao.enlaces[i][0] - 1] = 1;
         tabela_rotas[6][ligacao.enlaces[i][0] - 1] = ligacao.enlaces[i][0];
      }
  }

#ifdef DEBBUG_MONTAR_TABELA
  for (i = 0; i < 7; i++)
  {
    for (j = 0; j < 7; j++)
    {
      printf("Tabela_rotas[%d][%d] = '%d'\n", i, j, tabela_rotas[i][j]);
    }
  }
#endif
}