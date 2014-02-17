/*
El objetivo de esta práctica es familiarizarse con el uso de las primitivas de sincronización (semáforos) 
y los problemas de concurrencia que pueden presentarse (condiciones de carrera, deadlocks) al construir 
aplicaciones multihuiladas. También se pretende explorar herramientas de control de versiones y de trabajo en
equipo.

Implemente un programa, en C, considerando

1. El programa se ejecuta así:
$./nombre_programa numeroDeHilos cantidad_tiempo_a_correr numero_cuentas valor_inicial

2. El hilo princial debe crear tantas cuentas como indique el parámetro numero_cuentas. 
El valor inicial de las cuentas estará especificado por valor_inicial.

3. En una cuenta se pueden realizar depósitos, retiros y transferencias.

4. El hilo principal creará la cantidad de hilos definida en numeroDeHilos. 
Cada hilo seleccionará de manera aleatoria dos cuentas y realizará transferencias de una cantidad o monto 
igualmente aleatorio. Cada hilo repetirá esta operación durante la cantidad de tiempo especificada en cantidad_tiempo_a_correr.

5. Al finalizar cantidad_tiempo_a_correr, el hilo principal deberá imprimir el valor de cada una de las cuentas y el balance total
de la sumatoria de todas las cuentas.

6. Realice un programa de prueba que llame automáticamente el programa anterior pero con diferentes vectores de prueba.
Dicho programa deberá leer la salida producida con cada vector de prueba y comprobar que el resultado es correcto.
Los vectores de prueba deberán estar almacenados en un archivo.

7. Corra el programa de prueba de así:
$./programa_prueba nombre_programa archivo_vectores_prueba

8. Imprima el resultado de cada vector indicando si pasó o no pasó.

9. Los vectores de prueba deberán ser almacenados por líneas, así:

numero_hilos cantidad_tiempo_a_correr numero_cuentas valor_inicial CANTIDAD_DE_REPETICIONES_DE_ESTE_VECTOR 

El parámetro CANTIDAD_DE_REPETICIONES_DE_ESTE_VECTOR indica la cantidad de veces que se deberá repetir este vector.

NOTAS:

1. Escriba la estrategia de solución INICIAL, explicando la estructura del programa y las ideas para resolver la práctica.
Explique cómo piensa utilizar las primitivas ofrecidas por el sistemas operativo para evitar condiciones de carrera y posibles interbloqueos.

2. Describa los problemas que encontró durante la solución de la práctica mostrando cómo se manifestaban, cómo los detectó y cómo los solucionó.

3. Describa como fue la solución final, explicando la estructura del programa y las ideas finalmente implementadas y funcionales.

4. Compare la solución inicial y la solución final.

5. EL TRABAJO DEBERÁ SER REALIZADO EN PAREJAS!

6. Suba el trabajo realizado a GitHub, incluyendo el código del programa, el programa de prueba y archivos de prueba.

7. TODA LA DOCUMENTACIÓN SOLICITADA deberá estar también en github, utilizando una página de presentación o la wiki.

8. NO SE RECIBIRÁ NADA POR CORREO ELECTRÓNICO.

./nombre_programa numeroDeHilos cantidad_tiempo_a_correr numero_cuentas valor_inicial
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>
#include <string.h>
#include <fcntl.h>
#include <semaphore.h>

//variables globales
int *arregloCuentas;
sem_t *arregloSemaforos;
int numeroCuentas = 0;
int cantidadTiempo = 0;
int TIEMPOSLEEP = 1;

void* transferir(void* parametros);

int main(int argc, char*argv[])
{
	//archivo
	/*FILE *archivo = fopen("salida.txt", "w"); //Abrimos el archivo
	if(archivo == NULL)
	{
		printf("el archivo no se puede abrir");
		return 0;
	}*/

	int i;	
	int balanceInicial = 0;		
	int balanceFinal = 0;

	//obtener parametros
	int numeroHilos = atoi(argv[1]);
	cantidadTiempo = atoi(argv[2]);	
	numeroCuentas = atoi(argv[3]);
	int valorInicial = atoi(argv[4]);
	int cuentas[numeroCuentas];	//crear el arreglo de cuentas
	sem_t semaforos[numeroCuentas];	//crear arreglo de semaforos
	pthread_t hilos[numeroHilos]; 	//crear arreglo de hilos

	balanceInicial = numeroCuentas*valorInicial;	//balance inicial del banco
 
	arregloCuentas = cuentas;		//inicializar las direcciones
	arregloSemaforos = semaforos;

	//inicializar las cuentas y semaforos
	for(i = 0; i < numeroCuentas; i++){
		cuentas[i] = valorInicial;
		sem_init(&semaforos[i], 0, 1);
	}

	//crear los hilos
	for(i = 0; i < numeroHilos; i++){
		pthread_create(&hilos[i], NULL, &transferir, NULL);
	}

	//esperar los hilos
	for(i = 0; i < numeroHilos; i++){
		pthread_join(hilos[i], NULL);
	}

	//guardar en archivo	
	//imprimir las cuentas
	printf("*******************\n**** Resultado ****\n*******************\n");
	for(i = 0; i < numeroCuentas; i++){
		balanceFinal = balanceFinal + arregloCuentas[i];	//calcular balance final
		//fprintf(archivo, "cuenta[%d] = %d\n", i, arregloCuentas[i]);
		printf("cuenta[%d] = %d\n", i, arregloCuentas[i]);
	}
	
	if(balanceInicial == balanceFinal){
		//fprintf(archivo, "PASO\n");
		printf("BALANCE FINAL = %d\n", balanceFinal);
		printf("PASO\n");
	}
	else{
		//fprintf(archivo, "NO PASO\n");
		printf("NO PASO\n");
	}

	return 0;
	
}

void* transferir(void* parametros){
	
	//generar valores de las cuentas
	int sonIguales;		//para cuentaOrigen = cuentaDestino
	int estanDisponibles;	//tratar de acceder a las cuentas
	int cuentaDestino = 0;
	int cuentaOrigen = 0;	
	int i = 0;
	int montoTrans = 0;		//cantidad a transferir
	unsigned int semilla = rand();	//semilla para el rand
	while(i < cantidadTiempo){		
		srand(semilla);
		estanDisponibles = 1;
		sonIguales = 1;		
		//generar las cuentas
		//verificar que las cuentas son distintas	
		while(sonIguales == 1){		
			cuentaOrigen = rand()%numeroCuentas;
			cuentaDestino = rand()%numeroCuentas;
			if(cuentaOrigen != cuentaDestino){
					sonIguales = 0;
			}		
		}
		
		//tratar de acceder a las cuentas
		while(estanDisponibles == 1){
			int semOrigen = sem_trywait(&arregloSemaforos[cuentaOrigen]);
			int semDestino = sem_trywait(&arregloSemaforos[cuentaDestino]);
			if((semOrigen == 0) && (semDestino == 0)){
				estanDisponibles = 0;
				//hacer la transferencia
				montoTrans = rand()%arregloCuentas[cuentaOrigen];
				arregloCuentas[cuentaOrigen] = arregloCuentas[cuentaOrigen] - montoTrans;
				arregloCuentas[cuentaDestino] = arregloCuentas[cuentaDestino] + montoTrans;
				i++;				
				sleep(TIEMPOSLEEP);		
			}
			else{
				if(semOrigen == 0){
					sem_post(&arregloSemaforos[cuentaOrigen]);	//liberar semaforo
				}
				if(semDestino == 0){
					sem_post(&arregloSemaforos[cuentaDestino]);	//liberar semaforo
				}	
			}
		}

		//nueva semilla
		semilla = semilla + montoTrans;

		//liberar semaforos
		sem_post(&arregloSemaforos[cuentaOrigen]);	
		sem_post(&arregloSemaforos[cuentaDestino]);
	}
}

