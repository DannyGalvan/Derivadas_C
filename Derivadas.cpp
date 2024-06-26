#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define SUMA '+'
#define RESTA '-'
#define MULTIPLICACION '*'
#define DIVISION '/'
#define POTENCIA '^'
#define SEN 'S'
#define COS 'C'
#define TAN 'T'
#define X 'X'
#define Y 'Y'
#define PARENTESIS_ABRE '('
#define PARENTESIS_CIERRA ')'
#define VERDADERO 1
#define FALSO 0
#define ErrorAlfabeto 0
#define ErrorArchivo 0

struct TipoArbolExpresion
{
    TipoArbolExpresion *Izq;
    TipoArbolExpresion *Der;
    int info;
    float valor;
};

int Token;
char *Lexema;
int TokenAnterior;

void imprimirTextoRojo(const char *texto) {
    printf("\x1b[31m%s\x1b[0m\n", texto);
}

// Funci�n para imprimir texto en verde
void imprimirTextoVerde(const char *texto) {
    printf("\x1b[32m%s\x1b[0m\n", texto);
}

// Funci�n para imprimir texto en amarillo
void imprimirTextoAmarillo(const char *texto) {
    printf("\x1b[33m%s\x1b[0m", texto);
}

TipoArbolExpresion *creaArbol(TipoArbolExpresion *Izq, TipoArbolExpresion *Der, int operador)
{
    TipoArbolExpresion *nuevoNodo = (TipoArbolExpresion *)malloc(sizeof(TipoArbolExpresion));
    if (nuevoNodo == NULL)
        return NULL;
    nuevoNodo->Izq = Izq;
    nuevoNodo->Der = Der;
    nuevoNodo->info = operador;
    return nuevoNodo;
}

void destruyeArbol(TipoArbolExpresion **Arbol)
{
    if (*Arbol != NULL)
    {
        destruyeArbol(&(*Arbol)->Izq);
        destruyeArbol(&(*Arbol)->Der);
        free(*Arbol);
        *Arbol = NULL;
    }
}

void tomaToken()
{
	//si es un espacio lo omite
    while (isspace(*Lexema)) { 
        Lexema++;
    }
	
	//si es nulo lo omite
    if (*Lexema == '\0')
    {
        Token = ErrorAlfabeto;
    }
    else
    {
        switch (*Lexema)
        {
        case SUMA:
        case RESTA:
        case MULTIPLICACION:
        case DIVISION:
        case PARENTESIS_ABRE:
        case PARENTESIS_CIERRA:
        case X:
        case Y:
        case POTENCIA:
            Token = *Lexema++;
            break;
        case 's':
            if (strncmp(Lexema, "sen", 3) == 0)
            {
                Token = SEN;
                Lexema += 3; // Avanza 3 posiciones para "sen"
            }
            else
            {
                Token = ErrorAlfabeto;
            }
            break;
        case 'c':
            if (strncmp(Lexema, "cos", 3) == 0)
            {
                Token = COS;
                Lexema += 3; // Avanza 3 posiciones para "cos"
            }
            else
            {
                Token = ErrorAlfabeto;
            }
            break;
        case 't':
            if (strncmp(Lexema, "tan", 3) == 0)
            {
                Token = TAN;
                Lexema += 3; // Avanza 3 posiciones para "tan"
            }
            else
            {
                Token = ErrorAlfabeto;
            }
            break;
        default:
            if (isdigit(*Lexema))
            {
                // Convert string to float
                Token = strtof(Lexema, &Lexema);
            }
            else
            {
                Token = ErrorAlfabeto;
            }
            break;
        }
    }
}

TipoArbolExpresion *E();
TipoArbolExpresion *T();
TipoArbolExpresion *F();

TipoArbolExpresion *E()
{
    TipoArbolExpresion *Der = NULL;
    TipoArbolExpresion *Izq = NULL;
    int operador;

    Izq = T();
    if (Izq == NULL)
        return NULL;
    else
    {
        while (Token == SUMA || Token == RESTA)
        {
            operador = Token;
            tomaToken();
            Der = T();
            if (Der == NULL)
            {
                destruyeArbol(&Izq);
                return NULL;
            }
            else
                Izq = creaArbol(Izq, Der, operador);
        }
        return Izq;
    }
}

TipoArbolExpresion *T()
{
    TipoArbolExpresion *Der = NULL;
    TipoArbolExpresion *Izq = NULL;
    int operador;

    Izq = F();
    if (Izq == NULL)
        return NULL;
    else
    {
        while (Token == MULTIPLICACION || Token == DIVISION)
        {
            operador = Token;
            tomaToken();
            Der = F();
            if (Der == NULL)
            {
                destruyeArbol(&Izq);
                return NULL;
            }
            else
                Izq = creaArbol(Izq, Der, operador);
        }
        return Izq;
    }
}

TipoArbolExpresion *F()
{
    TipoArbolExpresion *Arbol = NULL;
    int operador;
    if (Token == PARENTESIS_ABRE || Token == '[' || Token == '{')
    {
        tomaToken();
        Arbol = E();
        if (Arbol == NULL || (Token != PARENTESIS_CIERRA && Token != ']' && Token != '}'))
        {
            destruyeArbol(&Arbol);
            return NULL;
        }
        tomaToken();
    }
    else if (Token == X || Token == Y)
    {
        operador = Token;
        tomaToken();
        if (Token == POTENCIA)
        {
            tomaToken();
            if (Token != ErrorAlfabeto)
            {
                Arbol = creaArbol(creaArbol(NULL, NULL, operador), F(), POTENCIA);
            }
            else
            {
                return NULL;
            }
        }
        else
        {
            Arbol = creaArbol(NULL, NULL, operador);
        }
    }
    else if (Token == SEN || Token == COS || Token == TAN)
    {
        int operador = Token;
		tomaToken();
		if (Token == PARENTESIS_ABRE)
		{
		    tomaToken();
		    TipoArbolExpresion* arg = E();
		    if (arg == NULL || Token != PARENTESIS_CIERRA)
		    {
		        destruyeArbol(&arg);
		        return NULL;
		    }
		    Arbol = creaArbol(NULL, arg, operador);
		    tomaToken();
		}
		else
		{
		    Token = ErrorAlfabeto;
		}
    }
	else if (Token != ErrorAlfabeto)
    {
        Arbol = creaArbol(NULL, NULL, 0);
        Arbol->valor = Token;
        tomaToken();
    }
    else
    {
        Token = ErrorAlfabeto;
    }
    
    return Arbol;
}

void imprimeArbolInvertido(TipoArbolExpresion *Arbol, int nivel)
{
    if (Arbol == NULL)
        return;

    nivel += 10;

    imprimeArbolInvertido(Arbol->Der, nivel);

    printf("\n");
    for (int i = 10; i < nivel; i++)
        printf(" ");

    switch (Arbol->info)
    {
	    case SUMA:
	        printf("+");
	        break;
	    case RESTA:
	        printf("-");
	        break;
	    case MULTIPLICACION:
	        printf("*");
	        break;
	    case DIVISION:
	        printf("/");
	        break;
	    case POTENCIA:
	        printf("^");
	        break;
	    case SEN:
	        printf("sen");
	        break;
	    case COS:
	        printf("cos");
	        break;
	    case TAN:
	        printf("tan");
	        break;
	    case X:
	        printf("x");
	        break;
	    case Y:
	        printf("y");
	        break;
	    default:
	        printf("%f", Arbol->valor);
	        break;
    }

    imprimeArbolInvertido(Arbol->Izq, nivel);
}

void imprimeArbolEnArchivo(TipoArbolExpresion *Arbol, int nivel, FILE *archivo)
{
	 if (archivo == NULL)
    {
        imprimirTextoRojo("Error al abrir el archivo de arboles\n");
        return;
    }
    
    if (Arbol == NULL)
        return;

    nivel += 10;

    imprimeArbolEnArchivo(Arbol->Der, nivel, archivo);

    fprintf(archivo, "\n");
    for (int i = 10; i < nivel; i++)
        fprintf(archivo, " ");

    switch (Arbol->info)
    {
	    case SUMA:
	        fprintf(archivo,"+");
	        break;
	    case RESTA:
	        fprintf(archivo, "-");
	        break;
	    case MULTIPLICACION:
	        fprintf(archivo, "*");
	        break;
	    case DIVISION:
	        fprintf(archivo, "/");
	        break;
	    case POTENCIA:
	        fprintf(archivo, "^");
	        break;
	    case SEN:
	        fprintf(archivo, "sen");
	        break;
	    case COS:
	        fprintf(archivo, "cos");
	        break;
	    case TAN:
	        fprintf(archivo, "tan");
	        break;
	    case X:
	        fprintf(archivo, "x");
	        break;
	    case Y:
	        fprintf(archivo, "y");
	        break;
	    default:
	        fprintf(archivo, "%f", Arbol->valor);
	        break;
    }

    imprimeArbolEnArchivo(Arbol->Izq, nivel, archivo);
}

void imprimeDerivadaEnArchivo(char* derivada, FILE *archivo){
	if (archivo == NULL)
    {
        imprimirTextoRojo("Error al abrir el archivo de derivadas\n");
        return;
    }
    
    if(derivada == '\0'){
    	imprimirTextoRojo("La cadena viene vacia");
    	return;
	}
	
	fprintf(archivo, "Respuesta: %s", derivada);	
}

void evaluaArbol(TipoArbolExpresion* Arbol, char* derivada)
{	
	char temp[1000];
	temp[0] = '\0';
	char multiTemp[1000];

	if (Arbol != NULL){
		evaluaArbol(Arbol->Izq, derivada);
        evaluaArbol(Arbol->Der, derivada);
        
         switch (Arbol->info)
        {
        	case SUMA:
		        {		        	
					if(Arbol->Der->info != 0){	
						derivada[0] = '\0';		        	
			            evaluaArbol(Arbol->Izq, derivada);
			            strcat(derivada, "+ ");
			            evaluaArbol(Arbol->Der, derivada);
					}					
		            break;
		        }	
			case RESTA:
				{
								
					if(Arbol->Der->info != 0){ 
						derivada[0] = '\0';	           			
						evaluaArbol(Arbol->Izq, derivada);
						strcat(derivada, "- ");
						evaluaArbol(Arbol->Der, derivada);
					}				
					break;
				}        
            case MULTIPLICACION:
                {	
                	if(Arbol->Der->Der != NULL){
                		float deriv_one = Arbol->Izq->valor * Arbol->Der->Der->valor;
	                    float deriv_minus = Arbol->Der->Der->valor - 1;
	                    sprintf(multiTemp, "%.0f*", deriv_one);
	                    strcat(temp, multiTemp);
	                    char tempCharacter = (char)Arbol->Der->Izq->info;
	                    strcat(temp, &tempCharacter);
	                    if(deriv_minus > 1){
	                    	tempCharacter = (char)Arbol->Der->info;
		                    strcat(temp, &tempCharacter);
		                    sprintf(multiTemp, "%.0f", deriv_minus);
		                    strcat(temp, multiTemp);
						}				
						strcat(derivada, temp);
	                    strcat(derivada, " ");
					}else{
						sprintf(multiTemp, "%.0f", Arbol->Izq->valor);
						strcat(temp, multiTemp);
						strcat(derivada, temp);
						strcat(derivada, " ");
					}
                    break;
				}
			case SEN:
				{
					if(Arbol->Der != NULL){
						sprintf(multiTemp, "cos(%c)", Arbol->Der->info);
						strcat(temp, multiTemp);
						strcat(derivada, temp);
						strcat(derivada, " ");
					}
					break;
				}
			case COS:
				{
					if(Arbol->Der != NULL){
						sprintf(multiTemp, "(-sin(%c))", Arbol->Der->info);
						strcat(temp, multiTemp);
						strcat(derivada, temp);
						strcat(derivada, " ");
					}
					break;
				}
			case TAN:
				{
					if(Arbol->Der != NULL){
						sprintf(multiTemp, "sec^2(%c)", Arbol->Der->info);
						strcat(temp, multiTemp);
						strcat(derivada, temp);
						strcat(derivada, " ");
					}
					break;
				}
            default:            	
  				break;
                
        }
	}   
}

int main()
{
	imprimirTextoVerde("BIENVENIDO AL SISTEMA DE DERIVADAS BASICAS \n");
	
	imprimirTextoRojo("-Instrucciones");
	imprimirTextoAmarillo("-Ingrese sus derivadas en el archivo entrada.txt \n");
	imprimirTextoAmarillo("-Solo es una derivada por linea \n");
	imprimirTextoAmarillo("-El resultado de los arboles de cada derivada estaran en el archivo arboles.dat \n");
	imprimirTextoAmarillo("-El resultado de las derivadas estaran en el archivo salida.dat \n\n");
	imprimirTextoAmarillo("-Aprecie el resultado de sus derivadas: \n\n");
	imprimirTextoVerde("-------------------------------------------- INICIO -------------------------------------------- \n");
	
	FILE *archivoArboles = fopen("arboles.dat", "w");
	FILE *archivoSalida  = fopen("salida.dat", "w");
	FILE *archivoEntrada = fopen("entrada.txt", "r");
	
	if (archivoEntrada == NULL)
    {
        imprimirTextoRojo("No hay archivo de entrada \n");
        imprimirTextoRojo("Debe Crear el archivo de datos de tipo texto (.txt) con el nombre 'entrada' para poder seguir con la ejecucion del programa.");
        imprimirTextoRojo("El archivo debe estar en la misma ruta del archivo ejecutable");
        return ErrorArchivo;
    }
	
    TipoArbolExpresion *Arbol;
    char Expresion[500];
    
    while(fgets(Expresion, 500, archivoEntrada) != NULL){
    	Lexema = Expresion;
	    tomaToken();
	    Arbol = E();
	    if (Arbol == NULL)
	    {
	    	char errorExpresion [500] = "-Error en la expresion ";
	    	strcat(errorExpresion, Expresion);
	        imprimirTextoRojo(errorExpresion);
	        imprimirTextoRojo("-Verifique que si esta usando x o y estas sean mayusculas (X) o (Y)");
	        imprimirTextoRojo("-Verifique que no existan signos incorrectos");
	        imprimirTextoRojo("-Verifique que la estructura de la expresion sea correcta \n");
	        imprimirTextoRojo("-------------------------------------------- Fin expresion -------------------------------------------- \n");
	    }
	    else
	    {
	       //imprimir derivada y arbol en consola
	       char derivada[1000] = ""; // Buffer para almacenar la cadena concatenada
	       evaluaArbol(Arbol,derivada);
		   char expresionColor[200] = "La expresion es: ";
		   strcat(expresionColor, Expresion); 
	       imprimirTextoAmarillo(expresionColor);
	       expresionColor[0] = '\0';
		   strcat(expresionColor, "La derivada es: "); 
	       strcat(expresionColor, derivada);
	       imprimirTextoVerde(expresionColor);       
	       imprimeArbolInvertido(Arbol, 0);
	       imprimirTextoVerde("\n\n-------------------------------------------- Fin expresion -------------------------------------------- \n");
	       
	       //imprimir arbol en archivo
	       fprintf(archivoArboles, "-------------------------------------------- Inicio del arbol -------------------------------------------- \n\n");
	       fprintf(archivoArboles, "Arbol para la funcion: %s \n", Expresion);
	       imprimeArbolEnArchivo(Arbol, 10, archivoArboles);
	       fprintf(archivoArboles, "\n\n-------------------------------------------- Fin del arbol -------------------------------------------- \n");
	       
	       //imprimir derivada en archivo
	       fprintf(archivoSalida, "-------------------------------------------- Inicio de la derivada -------------------------------------------- \n\n");
	       fprintf(archivoSalida, "Derivada para la funcion: %s \n", Expresion);
	       imprimeDerivadaEnArchivo(derivada, archivoSalida);
	       fprintf(archivoSalida, "\n\n-------------------------------------------- Fin de la derivada -------------------------------------------- \n");
	    }
	    destruyeArbol(&Arbol);
	}
    
    fclose(archivoArboles);
    fclose(archivoSalida);
    fclose(archivoEntrada);
    
    imprimirTextoAmarillo("Presione [Enter] para terminar con la ejecucion...");
    getchar();
    return 0;
}
