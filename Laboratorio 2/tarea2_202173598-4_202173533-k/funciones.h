#ifndef _funciones_H_
#define _funciones_H_

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>    
#include <math.h>

#define numero_cartas 4
#define numero_tableros 9

#define READ 0
#define WRITE 1
#define ARRIBA -1
#define ABAJO -2
#define IZQUIERDA -3
#define DERECHA -4
#define TERMINO_JUEGO -5
#define PLAYER -6
#define BOT -7
#define CARTA -8

extern char* cartas[5];
extern char* mazo[numero_cartas];
extern char* letra_jugador;
//extern char casilla_anterior[4];

/*
 * struct Punto - struct para representar una coordenada en el tablero
 * int x - coordenada x en el plano
 * int y - coordenada y en el plano
*/

typedef struct{ // Posicion x,y
    int x;
    int y;
} Punto;

/*
 * struct Juego - struct que contiene toda la información del juego
 * Punto casillas_especiales[9]; - posicion en plano donde se encuentra casilla especial
 * Punto posiciones[4];    - posicion cada jugador en tablero que se encuentra
 * int tablero_jugador[4]; - en que tablero esta cada jugador
 * char tablero[numero_tableros][5][5]; - Diferentes laberintos
 * int turno_actual;   - numero del jugador que le toca el turno
 * Punto posicion_tablero[9];    - Ver posiciones x,y de diferentes tableros en el juego para imprimirlos
 * int tesoros[4]; - Booleano para ver si jugadores han recogido tesoros
 * int ronda_actual; - turno actual
 * int rondas_totales; - maximo de turnos
 * int tesoros_instalados[4];  - para ver si el tesoro ha sido instalado o no
 * int laberintos_pordescubrir; - cantidad de laberintos que faltan por descubrir
 * int contador_camaras;  - cantidad de camaras instaladas
 * int tesoros_inst;   - cantidad de tesoros instalados
 * char casilla_anterior[4]  - casilla anterior de cada jugador
*/

typedef struct{
    Punto casillas_especiales[9]; 
    Punto posiciones[4];    
    int tablero_jugador[4];
    char tablero[numero_tableros][5][5]; 
    int turno_actual;  
    Punto posicion_tablero[9];   
    int tesoros[4]; 
    int ronda_actual; 
    int rondas_totales; 
    int tesoros_instalados[4]; 
    int laberintos_pordescubrir; 
    int contador_camaras;   
    int tesoros_inst;
    char casilla_anterior[4];   
} Juego;

/*
 * struct Jugador - el jugador
 * char* carta - carta que posee el jugador
 * int jugador_id - id del jugador
 * pid_t pid - el pid de proceso correspondiente al jugador
 * int hacia_jugador[2] - pipe hacia el jugador
 * int desde_jugador[2] - pipe desde el jugador
*/

typedef struct{
    char* carta;   
    int id_jugador;
    pid_t pid;
    int hacia_jugador[2];
    int desde_jugador[2];
} Jugador;

void enviar_mensaje_a_jugador(Jugador* p, int* mensaje);
// Función para enviar un mensaje a un jugador a través de pipes
// No retorna nada
// Parámetros:
//   Jugador* p: puntero al jugador al que se le enviará el mensaje
//   int* mensaje: puntero al mensaje que se enviará


void leer_mensaje_de_padre(Jugador* p, int* mensaje);
// Función para leer un mensaje enviado por el padre hacia un jugador a través de pipes
// No retorna nada
// Parámetros:
//   Jugador* p: puntero al jugador que leerá el mensaje
//   int* mensaje: puntero donde se almacenará el mensaje leído


void enviar_mensaje_de_jugador(Jugador *p, int* mensaje);
// Función para enviar un mensaje desde un jugador hacia el padre a través de pipes
// No retorna nada
// Parámetros:
//   Jugador* p: puntero al jugador que enviará el mensaje
//   int* mensaje: puntero al mensaje que se enviará


void leer_mensaje_de_jugador(Jugador* p, int* mensaje);
// Función para leer un mensaje enviado desde un jugador hacia el padre a través de pipes
// No retorna nada
// Parámetros:
//   Jugador* p: puntero al jugador que leerá el mensaje
//   int* mensaje: puntero donde se almacenará el mensaje leído


void borrar_jugador(Jugador* jugador);
// Función para cerrar los pipes de los jugadores
// No retorna nada
// Parámetros:
//   Jugador* jugador: struct del jugador a ser cerrados los pipes


void desordenar_cartas(char *array[]);
// Función para desordenar un array de cartas utilizando el algoritmo Fisher-Yates
// No retorna nada
// Parámetros:
//   char* array[]: array de punteros a cartas


void desordenar_laberintos(char laberintos[9][5][5]);
// Función para desordenar un array de laberintos utilizando algoritmo de Fisher-Yates
// No retorna nada
// Parámetros:
//   char laberintos[9][5][5]: array tridimensional de laberintos


int posicion_valida(Punto salida, char tablero[5][5]);
// Función para verificar si una posición de salida es válida en un tablero que se descubre
// Retorna la dirección válida (ARRIBA, IZQUIERDA, ABAJO, DERECHA) o 0 si no es válida
// Parámetros:
//   Punto salida: coordenadas de salida a verificar
//   char tablero[5][5]: tablero descubierto que se verifica si tiene una casilla "B" en la misma dirección de salida


int seleccionar_laberinto(int laberintoActual, Punto salida, Juego* juego);
// Función para seleccionar un laberinto aleatorio cuando se descubre una casilla 'B', que encaja con esa casilla y que este disponible
// Retorna el número del laberinto seleccionado
// Parámetros:
//   int laberintoActual: número del laberinto actual
//   Punto salida: coordenadas de salida en el laberinto actual (casilla B)
//   Juego* juego: puntero al juego


void identificar_direccion(int dir, Punto posicion_tablero, Juego* juego, int nuevo_laberinto);
// Función para agregar la posicion del nuevo laberinto en el mapa general (en la variable posicion_tablero del nuevo laberinto).
// Si ya hay un laberinto descubierto que encaje con alguno de las casillas B del nuevo laberinto, se conectan
// No retorna nada
// Parámetros:
//   int dir: dirección que se encuentra el nuevo laberinto con respecto al actual que se encuentra el jugador
//   Punto posicion_tablero: coordenadas del tablero actual en el mapa
//   Juego* juego: struct del juego actual con toda la información
//   int nuevo_laboratorio: entero con el número del nuevo laberinto


int teletransporte(Juego* juego, int id_jugador);
// Función para realizar el teletransporte del jugador a traves de las casillas especiales
// No retorna nada
// Parámetros:
//   Juego* juego: struct del juego actual con toda la información
//   int id_jugador: entero con el número del nuevo laberinto


void casilla_final(Juego *j , Jugador *p);
// Función para ver si pasa por una casilla especial, o si recoge un tesoro
// No retorna nada
// Parámetros:
//   Juego* j: struct del juego actual con toda la información
//   Jugador *p: entero con el número del nuevo laberinto


void agregar_evento(Juego* juego, int nuevo_laberinto);
// Función para agregar evento (casilla especial) en nuevo laberinto descubierto
// No retorna nada
// Parámetros:
//   Juego* juego: struct del juego actual con toda la información, en este caso usaremos los tableros para imprimirlos (los descubiertos)
//   int nuevo_laberinto: entero que corresponde al numero del nuevo laberinto descubierto


int ver_termino(Juego* juego);
// Funcion para ver si los 4 jugadores obtuvieron tesoros y volvieron a tablero inicial (si se termina el juego por que ganan)
// No retorna nada
// Parámetros:
//   Juego* juego: struct del juego actual con toda la información


void imprimir_laberintos(Juego* juego);
// Función para imprimir de forma ordenada los laberintos que han sido descubiertos (incluyendo el inicial)
// No retorna nada
// Parámetros:
//   Juego* juego: struct del juego actual con toda la información, en este caso usaremos los tableros para imprimirlos (los descubiertos)


void crear_juego(Juego* juego);
// Función para crear un juego, es decir:
//  1) Carga tablero inicial
//  2) Inicializa tablero inicial en el centro del mapa
//  3) Carga los 8 tableros restantes e inicializa sus coordenadas x e y en el mapa en -20 que significa no han sido descubiertos
//  4) Setea turno actual a jugador 1 
//  5) Inicializa todos los jugadores en tablero inicial
//  6) Inicializa todos los jugadores sin tesoros
// No retorna nada
// Parámetros:
//   Juego* juego: struct del juego actual con toda la información, en este caso usaremos los tableros para imprimirlos (los descubiertos)


void accion(Jugador *p, int opcion_cantidad[2], Juego *j);
// Función que realiza las diferentes acciones que puede elegir el jugador, ya sea para los bots o para el mismo
// Las acciones son:
//  1) Hacer uso de la carta "Buscar"
//      - Se descubre nuevo laberinto, las 'B' entre los laberintos se reemplazan por 'P' que representa que es una puerta abierta
//  2) Hacer uso de la carta "Escalera"
//      - Reemplaza la casilla 'E' por un '0'
//  3) Moverse en las direcciones hacia Arriba, Izquierda, Derecha y Abajo
//      - Realiza el movimiento de acorde a dirección indicada y cantidad de movimientos que desea realizar el jugador
//        lo realiza igualmente si se quiere mover de un laberinto a otro y hay la suficiente cantidad de movimientos
// No retorna nada
// Parámetros:
//   Jugador *p: struct con el jugador que realiza el movimiento
//   int opcion_cantidad[2]: primer entero corresponde al tipo de movimiento que desea realizar
//                           opcion_cantidad[0] == 1 -> Arriba
//                           opcion_cantidad[0] == 2 -> Derecha
//                           opcion_cantidad[0] == 3 -> Izquierda
//                           opcion_cantidad[0] == 4 -> Abajo
//                           opcion_cantidad[0] == 5 -> Carta
//                           opcion_cantidad[1] corresponde a cantidad de movimientos en el caso de realizar uno
//   Juego *j: struct con toda la información del juego


void crear_jugador(Jugador* p, int id_jugador);
// Función para crear un jugador, es decir:
//  1) Crea mediante fork() al proceso hijo
//  2) Crea los pipes hacia el padre y del padre al hijo
//  3) Inicializa los tesoros de los jugadores a falso
//  4) Le asigna su carta correspondiente
//  4) Inicializa los pipes
// No retorna nada
// Parámetros:
//   Jugador* p: struct del jugador que se crea
//   int id_jugador: id del jugador


#endif
