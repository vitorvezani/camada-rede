//
//  arquivo.h
//
//  Guilherme Sividal - 09054512 
//  Vitor Rodrigo Vezani - 10159861
//
//  Created by Vitor Vezani on 19/03/13.
//  Copyright (c) 2013 Vitor Vezani. All rights reserved.
//

#include <stdio.h>		    /* for printf() */
#include <stdlib.h>			/* for exit() */
#include <string.h>
#include <pthread.h>     	/* para poder manipular threads */

#define MAXNOS			6

#define TRUE 			1
#define FALSE 			0

#define NOS 			1
#define ENLACES 		2

//#define DEBBUG_ARQUIVO

struct ligacoes {
    char nos[6][3][35];
    int enlaces[18][3];
};

extern struct ligacoes ligacao;

void delete_espace(char* input);