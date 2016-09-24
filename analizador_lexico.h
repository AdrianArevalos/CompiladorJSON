#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>

#define L_CORCHETE      256
#define R_CORCHETE      257
#define L_LLAVE		    258
#define R_LLAVE		    259
#define COMA	        260
#define DOS_PUNTOS	    261
#define LITERAL_CADENA	262
#define LITERAL_NUM     263
#define PR_TRUE         264
#define PR_FALSE        265
#define PR_NULL         266

#define TAMBUFF 	5
#define TAMLEX 		50

#define TOPE    256

typedef char * string;

string nombre_componente [] = {"L_CORCHETE","R_CORCHETE", "L_LLAVE", "R_LLAVE", "COMA", "DOS_PUNTOS" , "LITERAL_CADENA", "LITERAL_NUM", "PR_TRUE","PR_FALSE","PR_NULL"};

typedef struct {
	int compLex;
	string lexema;
    string componente;
} token;

void sigLex();
void palabra_reservada(char []);
