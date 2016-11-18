#include "analizador_lexico.c"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#define CANTIDAD_COMPONENTES_LEXICOS 10

/*Declaraciones de las funciones*/
void json();
void element();
void array();
void arrayB();
void element_list();
void element_listB();
void object();
void objectB();
void attribute();
void attributes_list();
void attributes_listB();
void attribute_name();
void attribute_value(int []);
void match(int);
void error_sint(int []);
int acepto = 1;


void match(int expectedToken){
    if(tok.compLex == expectedToken){
        sigLex();
    }
}

void error_sint(int sincronizacion []) {
    acepto = 0;
   	printf("\nError sintactico en linea: %d. No se esperaba %s.\n",numLinea,tok.componente);
	int contador = 0;
    while(tok.compLex != sincronizacion[contador] && tok.compLex != EOF) {   
        if (sincronizacion[contador] == '\0') {
            sigLex();
            contador = 0;        
        }
        contador++;
    }
    sigLex();
    return;    	   
}

void check_input(int primero[], int siguiente[]) {
    int syncset[] = {CANTIDAD_COMPONENTES_LEXICOS};
    int contador = 0;
    if(tok.compLex == EOF) {
        return;
    }    
    while(primero[contador] != '\0') {
        if(tok.compLex == primero[contador]) {
            return;
        }
        contador++;
    }
    contador = 0;    
    while(primero[contador] != '\0'){ 
        syncset[contador] = primero[contador];
        contador++;
    }
    int contador2 = 0;
    while(siguiente[contador2] != '\0'){
        syncset[contador] = siguiente[contador2];
        contador++;
        contador2++;
    }
    error_sint(syncset);

}
 
void json(){
    int primero[] = {L_CORCHETE,L_LLAVE,'\0'};
    int siguiente[] = {COMA,R_CORCHETE,R_LLAVE, '\0'};
    element(siguiente);
}

void element(int syncset[]){
    int primero[] = {L_CORCHETE,L_LLAVE,'\0'};
    int siguiente[] = {COMA,R_CORCHETE,R_LLAVE, '\0'};
    check_input(primero,syncset);

    if(tok.compLex == L_LLAVE){
        object(siguiente);
    }
    else if(tok.compLex == L_CORCHETE){
        array(siguiente);
    }
    check_input(siguiente,primero);
}

/*array -> [ arrayB*/
void array(int syncset[]){
    int primero[] = {L_CORCHETE,'\0'};
    int siguiente[] = {COMA,R_CORCHETE,R_LLAVE, '\0'};
    check_input(primero,syncset);
   
    if(tok.compLex == L_CORCHETE){
        match(L_CORCHETE);
        arrayB(siguiente);
    }
    check_input(siguiente,primero);
}

/*array' -> element-list ] | ]*/
void arrayB(int syncset[]){ 
    int primero[] = {L_CORCHETE,R_CORCHETE,L_LLAVE,'\0'};
    int siguiente[] = {L_CORCHETE,COMA,R_CORCHETE,R_LLAVE, '\0'};
    check_input(primero,syncset);
    if(tok.compLex == L_CORCHETE || tok.compLex == L_LLAVE){
        element_list(siguiente);
        match(R_CORCHETE);
    }
    else if(tok.compLex == R_CORCHETE){
        match(R_CORCHETE);
    }
    check_input(siguiente,primero);
}

/*element-list -> element element-list'*/
void element_list(int syncset[]){
    int primero[] = {L_CORCHETE,L_LLAVE,'\0'};
    int siguiente[] = {R_CORCHETE, '\0'};
    check_input(primero,syncset);
    if(tok.compLex == L_CORCHETE || tok.compLex == L_LLAVE){
        element(siguiente);
        element_listB(siguiente);
    }
    check_input(siguiente,primero);
}

/*element-list' ->  ,element element-list' | ε*/
void element_listB(int syncset[]){
    if(tok.compLex == R_CORCHETE){ 
        return;
    }
    int primero[] = {COMA,'\0'};
    int siguiente[] = {R_CORCHETE, '\0'};
    check_input(primero,syncset);
    if(tok.compLex == COMA){
        match(COMA);
        element(siguiente);
        element_listB(siguiente);
    }
    check_input(siguiente,primero);
}

/*  object -> { object'*/
void object(int syncset[]){ 
    int primero[] = {L_LLAVE,'\0'};
    int siguiente[] = {COMA,R_CORCHETE,R_LLAVE, '\0'};
    check_input(primero,syncset);
    if(tok.compLex == L_LLAVE){
        match(L_LLAVE);
        objectB(siguiente);
    }
    check_input(siguiente,primero);
}

/*   object' -> attributes-list} | }*/
void objectB(int syncset[]){
    int primero[] = {LITERAL_CADENA,R_LLAVE,'\0'};
    int siguiente[] = {LITERAL_CADENA,COMA,R_CORCHETE,R_LLAVE, '\0'};
    check_input(primero,syncset);
    if(tok.compLex == LITERAL_CADENA){
        attributes_list(siguiente);
        match(R_LLAVE);
    }
    else if(tok.compLex == R_LLAVE){
        match(R_LLAVE);
    }
    check_input(siguiente,primero);
}

/*   attributes-list -> attribute attributes-list'*/
void attributes_list(int syncset[]){
    int primero[] = {LITERAL_CADENA,'\0'};
    int siguiente[] = {R_LLAVE, '\0'};
    check_input(primero,syncset);

    if(tok.compLex == LITERAL_CADENA){
        attribute(siguiente);
        attributes_listB(siguiente);
    }
    check_input(siguiente,primero);
}

/*   attributes-list' -> ,attribute attributes-list' | ε*/
void attributes_listB(int syncset[]){
    if (tok.compLex == R_LLAVE){
        return;
    }
    int primero[] = {COMA,'\0'};
    int siguiente[] = {R_LLAVE, '\0'};
    check_input(primero,syncset);
    if(tok.compLex == COMA){
        match(COMA);
        attribute(siguiente);
        attributes_listB(siguiente);
    }
    
    check_input(siguiente,primero);
}

/*   attribute -> attribute-name : attribute-value*/
void attribute(int syncset[]){
    int primero[] = {LITERAL_CADENA,'\0'};
    int siguiente[] = {COMA,R_LLAVE, '\0'};
    check_input(primero,siguiente);
    if(tok.compLex == LITERAL_CADENA){
        attribute_name(siguiente);
        match(DOS_PUNTOS);
        attribute_value(siguiente);
    }
    check_input(siguiente,primero);
}

/*   attribute-name -> string*/
void attribute_name(int syncset[]){
    int primero[] = {LITERAL_CADENA,'\0'};
    int siguiente[] = {DOS_PUNTOS, '\0'};
    check_input(primero,syncset);
    if(tok.compLex == LITERAL_CADENA){
       
        match(LITERAL_CADENA);
    }
    check_input(siguiente,primero);
}

/*attribute-value -> element | string | number | true | false | null*/
void attribute_value(int syncset[]){
    int primero[] = {L_CORCHETE,L_LLAVE,LITERAL_CADENA,LITERAL_NUM,PR_TRUE,PR_FALSE,PR_NULL,'\0'};
    int siguiente[] = {COMA,R_LLAVE, '\0'};
    check_input(primero,syncset);
    if(tok.compLex == L_CORCHETE || tok.compLex == L_LLAVE){
        element(siguiente);
    }
    else if(tok.compLex == LITERAL_CADENA){
        match(LITERAL_CADENA);
    }
    else if(tok.compLex == LITERAL_NUM){
        match(LITERAL_NUM);
    }
    else if(tok.compLex == PR_TRUE){
        match(PR_TRUE);
    }
    else if(tok.compLex == PR_FALSE){
        match(PR_FALSE);
    }
    else if(tok.compLex == PR_NULL){
        match(PR_NULL);
    }
    check_input(siguiente,primero);
}

/*Funcion Main*/
int main (int argc,char* args[]){
    if(argc > 1)
    {
        if (!(archivo=fopen(args[1],"rt")))
        {
            printf("Archivo no encontrado.\n");
            exit(1);
        }
        sigLex();
        json();
        if(acepto) printf("El contenido del archivo es correctamente sintactico.\n");
        fclose(archivo);
    }else{
        printf("Debe pasar como parametro el path al archivo fuente.\n");
        exit(1);
    }

    return 0;
}
