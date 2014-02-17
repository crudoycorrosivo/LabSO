/*
*$./programa_prueba nombre_programa archivo_vectores_prueba
*numero_hilos cantidad_tiempo_a_correr numero_cuentas valor_inicial CANTIDAD_DE_REPETICIONES_DE_ESTE_VECTOR
*/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <wait.h>
#include <string.h>
#include<fcntl.h>

int main (int argc, char*argv[])
{
	char *nombreArchivo = argv[2];
	char *nombrePrograma = argv[1];
	char linea[100];
	int i = 0;
	//abrir archivo
	FILE *archivo = fopen(nombreArchivo, "r"); //Abrimos el archivo
	
	//verificar
	if(archivo == NULL)
	{
		printf("el archivo no se puede abrir");
		return 0;
	}

	//sacar cada linea
	while(fgets(linea, 100, archivo) != NULL){
		
		//guardar parametros
		char *numeroHilos = strtok(linea, " ");
		char *cantidadTiempo = strtok(NULL, " ");
		char *numeroCuentas = strtok(NULL, " ");
		char *valorInicial = strtok(NULL, " ");
		char *strCantRepeticiones = strtok(NULL, " ");
		int repeticiones = atoi(strCantRepeticiones);
	
		//printf("%s %s %s %s %s %d\n", nombrePrograma, numeroHilos, cantidadTiempo, numeroCuentas, valorInicial, repeticiones);
		//ciclo para repeticiones
		for(i = 0; i < repeticiones; i++){
			pid_t pid_hijo = fork();
			if(pid_hijo == 0){
				char *args[] = {nombrePrograma, numeroHilos, cantidadTiempo, numeroCuentas, valorInicial, NULL};
				execv(args[0], args);				
			}
			else{
				wait(NULL);
			}
		}		
	}
	return 0;	
}
