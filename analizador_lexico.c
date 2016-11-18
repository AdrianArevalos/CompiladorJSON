/*
    Analizador Léxico - Práctica de Programación Nro. 1 - 2016

         Implementar un analizador léxico para el lenguaje Json simplificado. Recibe un archivo fuente
    Json, debe realizar un análisis léxico reconociendo tokens, y luego por cada línea del fuente
    imprimir una línea en un archivo de salida con la secuencia de componentes léxicos
    encontrados separados por espacios (se adjuntan ejemplos de un fuente y su respectivo
    archivo de salida). En caso de error léxico deberá imprimir un mensaje de error y continuar con
    la siguiente línea del archivo fuente.

    Integrantes:
        * Adrian Arevalos Bogado    4826367
        * Doris Rios Velazquez      4508991
 */

#include "analizador_lexico.h"

/* Variables globales */

char cadenaError[5*TAMLEX];		    // Mensajes de error
token tok;				            // Token global 

/* Variables para nuestro Analizador Lexico */ 

FILE *archivo;			    // Fuente JSON que se recibe como parametro
char cadena[TAMLEX];		// Utilizado por el analizador lexico
int numLinea = 1;			// Numero de Linea
char caracter = 0;          // Caracter que se lee desde el archivo


/*Funcion utilizada para los mensajes de Error */
void error(string mensaje) {
	printf("\nLin %d: Error Lexico. %s.\n", numLinea, mensaje);
    while(fgetc(archivo) != '\n'){
        fgetc(archivo);    	 
    }
    numLinea++;    	
}

/* Funcion utilizada para verificar los tokens que tenemos en nuestro fuente, seria nuestro Analizador Lexico. */
void sigLex() {
	int cantidadCaracteres = 0;
	int estado = 0;
    int acepto = 0;
	char msg[5*TAMLEX];
    
	while((caracter = fgetc(archivo)) != EOF) {
		if (caracter == ' ')             
            continue;  
        else if(caracter == '\t')        
            continue;
		else if(caracter == '\n') {
			//Incrementar el numero de linea
			numLinea++;
            continue;
		}
		else if (isalpha(caracter)) {
			//Se espera recibir una palabra reservada
			cantidadCaracteres = 0;
			do{
				cadena[cantidadCaracteres] = caracter;
				cantidadCaracteres++;
				caracter = fgetc(archivo);       
			}while(isalpha(caracter));    

			cadena[cantidadCaracteres] = '\0';

			if (caracter != EOF){
				ungetc(caracter,archivo);
            }else
			    caracter=0;

			palabra_reservada(cadena);
            tok.lexema = cadena;
			break;
		}
        else if (caracter == '"'){
            //Se espera recibir un string (literal_cadena)
            cantidadCaracteres = 0;
            estado = 0;
            acepto = 0;
            cadena[cantidadCaracteres] = caracter;
            while(!acepto){
             	switch(estado){
             		case 0:
             			caracter = fgetc(archivo);
             			if(cantidadCaracteres >= TAMLEX){
             				estado = -1;
                    	}       	
             			else if(caracter == '\t' || caracter == '\n' || caracter == EOF){
             				estado = -1;
             			}
             			else if(caracter == '"'){
             				cadena[++cantidadCaracteres] = caracter;
             				estado = 1;
             			}
             			else{
             				cadena[++cantidadCaracteres] = caracter;
             				estado = 0;
             			}
             			break;
             		case 1://Devolver el caracter correspondiente a otro componente lexico						
						caracter = 0;
						cadena[++cantidadCaracteres] = '\0';
						acepto = 1;
						tok.compLex= LITERAL_CADENA;
		                tok.lexema = cadena;
		                tok.componente = nombre_componente[LITERAL_CADENA - TOPE]; 
						break;
					case -1:						
						if (caracter == EOF)
							error("No se esperaba fin de archivo");
						else{
							ungetc(caracter, archivo);
							if(cantidadCaracteres >= TAMLEX) 
                                error("Longitud de Identificador es mayor al tamaño de buffer");
							else if(caracter == '\n')
                                error("Salto de linea no esperado");
							else if(caracter=='\t')
                                error("Tabulacion no esperada");
							tok.componente = 0;
		                    acepto = 1;
		                    break;
		                }					
             	} 
            }
            break;
        }
		else if (isdigit(caracter))
		{  
			//Se espera recibir un numero
			cantidadCaracteres=0;
			estado = 0;
			acepto = 0;
			cadena[cantidadCaracteres] = caracter;
			while(!acepto)
			{
				switch(estado){
					case 0: //Al recibir un digito, podemos tener . o e
						caracter = fgetc(archivo);
						if (isdigit(caracter))
						{
							cadena[++cantidadCaracteres] = caracter;
							estado = 0;
						}
						else if(caracter=='.'){
							cadena[++cantidadCaracteres] = caracter;
							estado = 1;
						}
						else if(tolower(caracter) == 'e'){
							cadena[++cantidadCaracteres] = caracter;
							estado = 3;
						}
						else{
							estado = 6;
						}
						break;
					
					case 1://Si se recibe un punto debe ir seguido de un digito.
						caracter = fgetc(archivo);						
						if (isdigit(caracter))
						{
							cadena[++cantidadCaracteres] = caracter;
							estado = 2;
						}
						else{
							sprintf(msg,"No se esperaba '%caracter'",caracter);
							estado = -1;
						}
						break;
					case 2://la fraccion decimal, puede ser seguido por digitos o por e
						caracter = fgetc(archivo);
						if (isdigit(caracter))
						{
							cadena[++cantidadCaracteres] = caracter;
							estado = 2;
						}
						else if(tolower(caracter) == 'e')
						{
							cadena[++cantidadCaracteres] = caracter;
		                    printf("%caracter = ",caracter);
							estado = 3;
						}
						else
							estado = 6;
						break;
					case 3://Si recibimos una e, en nuestro numero, puede ir seguido de un + o - o una secuencia de digitos.
						caracter = fgetc(archivo);
						if (caracter == '-' || caracter=='+')
						{
							cadena[++cantidadCaracteres] = caracter;
							estado = 4;
						}
						else if(isdigit(caracter))
						{
							cadena[++cantidadCaracteres] = caracter;
							estado = 5;
						}
						else{
							sprintf(msg,"No se esperaba '%caracter'",caracter);
							estado = -1;
						}
						break;
					case 4://Debe venir por lo menos un digito
						caracter = fgetc(archivo);
						if (isdigit(caracter))
						{
							cadena[++cantidadCaracteres] = caracter;
							estado = 5;
						}
						else{
							sprintf(msg,"No se esperaba '%caracter'",caracter);
							estado = -1;
						}
						break;
					case 5://una secuencia de digitos correspondiente al exponente
						caracter = fgetc(archivo);
						if (isdigit(caracter))
						{
							cadena[++cantidadCaracteres]=caracter;
							estado = 5;
						}
						else{
							estado = 6;
						}break;
					case 6://Devolver el caracter correspondiente a otro componente lexico
						if (caracter != EOF)
							ungetc(caracter,archivo);
						else
							caracter = 0;
							cadena[++cantidadCaracteres] = '\0';
							acepto = 1;
							tok.compLex = LITERAL_NUM;
		                    tok.lexema = cadena;
		                    tok.componente = nombre_componente[LITERAL_NUM - TOPE]; 
						break;
					case -1:
						if (caracter == EOF)
							error("No se esperaba el fin de archivo");
						else{
							tok.componente = 0;
		                    sprintf(msg,"%caracter no esperado",caracter);
			                error(msg);
		                    acepto = 1;
		                }					
		                break;
		        }
			}
        	break;
		}

		else if (caracter == ':')
		{
			tok.compLex = DOS_PUNTOS;
            tok.lexema = ":";			
            tok.componente = nombre_componente[DOS_PUNTOS - TOPE];
			break;
		}
		else if (caracter == ',')
		{
			tok.compLex= COMA;
            tok.lexema = ",";
            tok.componente = nombre_componente[COMA - TOPE];            
			break;
		}		
        else if (caracter == '[')
		{
			tok.compLex=L_CORCHETE;
			tok.lexema = "[";
            tok.componente = nombre_componente[L_CORCHETE - TOPE];
			break;
		}
		else if (caracter == ']')
		{
			tok.compLex = R_CORCHETE;
			tok.lexema = "]";
            tok.componente = nombre_componente[R_CORCHETE - TOPE];
			break;
		}
		else if (caracter == '{')
		{
            tok.compLex= L_LLAVE;
            tok.lexema = "{";            
            tok.componente = nombre_componente[L_LLAVE - TOPE];
			break;		
        }
        else if (caracter == '}')
        {
            tok.compLex= R_LLAVE;
            tok.lexema = "}";            
            tok.componente = nombre_componente[R_LLAVE - TOPE];
			break;		
        }
		else if (caracter != EOF)
		{
			sprintf(msg,"%caracter no esperado",caracter);
			error(msg);
		}
	}
	
    if (caracter == EOF){
		tok.compLex = EOF;
        //exit(1);
    }
        
	
}
/* Funcion para verificar la palabra reservada recibida. */
void palabra_reservada(char cadena []){
   if (strcmp(cadena,"true") == 0 || strcmp(cadena,"TRUE") == 0 ) {
       tok.compLex = PR_TRUE;
       tok.lexema = cadena;
       tok.componente = nombre_componente[PR_TRUE - TOPE]; 
   	}else if(strcmp(cadena,"false") == 0 || strcmp(cadena,"FALSE") == 0 ) {
       tok.compLex = PR_FALSE;
       tok.lexema = cadena;
       tok.componente = nombre_componente[PR_FALSE - TOPE]; 
   	}else if(strcmp(cadena,"null") == 0 || strcmp(cadena,"NULL") == 0 ) {
       tok.compLex = PR_NULL;
       tok.lexema = cadena;
       tok.componente = nombre_componente[PR_NULL - TOPE]; 
   	}
   	else{
   		ungetc(caracter,archivo);
   		sprintf(cadenaError,"No se esperaba '%s'",cadena);
   		tok.componente = 0;
       	error(cadenaError);
    }
}

/*int main(int argc,char* args[]) {
	
	if(argc > 1) {
		if (!(archivo = fopen(args[1],"rt"))) {
			printf("Archivo no encontrado.\n");
			exit(1);
		}
		while (tok.compLex != EOF){
			sigLex();
			if(tok.componente)
       			printf("%s ", tok.componente);
		}
		fclose(archivo);
	}else{
		printf("Se debe recibir como parametro el path al archivo fuente.\n");
		exit(1);
	}

	return 0;
}*/
