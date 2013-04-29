#
#	makefile.GNUmakefile
#
#  Guilherme Sividal - 09054512 
#  Vitor Rodrigo Vezani - 10159861
#
#  Created by Vitor Vezani on 19/03/13.
#  Copyright (c) 2013 Vitor Vezani. All rights reserved.
#
all: projeto

projeto: main.o enlace.o rede.o arquivo.o transporte.o garbler.o
	gcc -o meuprog main.o enlace.o rede.o arquivo.o transporte.o garbler.o

main.o: main.c "headers/main.h"
	gcc -c main.c

enlace.o: enlace.c "headers/enlace.h"
	gcc -c enlace.c

rede.o: rede.c "headers/rede.h"
	gcc -c rede.c

arquivo.o: arquivo.c "headers/arquivo.h"
	gcc -c arquivo.c

transporte.o: transporte.c "headers/transporte.h"
	gcc -c transporte.c
	
garbler.o: garbler.c "headers/garbler.h"
	gcc -c garbler.c

