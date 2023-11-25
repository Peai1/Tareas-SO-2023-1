#include "funciones.h"
#include <signal.h>

int main() {

    Jugador jugador[4];
    // Memoria compartida para tablero
    key_t key = ftok("main.c", 4321); // Elige un archivo existente y un entero para generar una clave única.
    int shmid = shmget(key, sizeof(Juego), 0666 | IPC_CREAT); // Crea memoria compartida del tamaño de juego
    Juego* juego = (Juego*) shmat(shmid, (void*)0, 0);
    crear_juego(juego);

    desordenar_cartas(mazo);
    desordenar_laberintos(juego->tablero);

	int i;
    for(i = 0; i < 4; i++){
        crear_jugador(&jugador[i], i);
        if(jugador[i].pid == 0) 
            break;
    }
    int mensaje;

    if(jugador[0].pid == 0 || jugador[1].pid == 0 || jugador[2].pid == 0 || jugador[3].pid == 0){
        
        Jugador p = jugador[i];
        int opcion, cant;

        while(1){
            leer_mensaje_de_padre(&p,&mensaje);

            if (mensaje == PLAYER){
                
                printf("===========================================\n");
                printf("Le toca jugar al jugador J%d | Carta: %s\n", jugador[0].id_jugador + 1, p.carta);
                printf("Recordatorio: Camara (R) | + 5 turnos (T) | - 3 turnos (Y) \n");
                printf("Teletransportes: J1 -> P1, J2 -> P2, J3 -> P3, J4 -> P4\n");
                printf("Tablero:\n");
                imprimir_laberintos(juego);
                printf("Ingrese el numero de la accion que desea realizar:\n");
                printf("1) Arriba, 2) Derecha, 3) Izquierda, 4) Abajo, 5) Carta : (9 si quiere salir o 0 si no quiere hacer nada) ");
                scanf("%d",&opcion);
                printf("\n");
                if (opcion == 1 || opcion == 2 || opcion == 3 || opcion == 4){
                    printf("Ingrese cuantas celdas desea moverse en la direccion escogida: ");
                    scanf("%d",&cant);
                    printf("\n");
                }
                else if(opcion == 9){
                    shmdt(juego);
                    shmctl(shmid, IPC_RMID, NULL);
                    kill(0, SIGTERM);
                    exit(1);
                }
                int opcion_cantidad[2] = {opcion, cant};
                accion(&jugador[0],opcion_cantidad,juego);
                enviar_mensaje_de_jugador(&jugador[0], opcion_cantidad);    // mensaje termino turno                
            }
            
            // Mandar mensaje para que bot se mueve, se envia de jugador a proceso padre, luego del padre al bot
            else if (mensaje == BOT){

                printf("===========================================\n");
                printf("Debes mover al jugador J%d | Carta: %s\n", juego->turno_actual + 1, mazo[juego->turno_actual]);
                printf("Recordatorio: Camara (R) | + 5 turnos (T) | - 3 turnos (Y)\n");
                printf("Teletransportes -> J1 -> P1, J2 -> P2, J3 -> P3, J4 -> P4\n");
                printf("Tablero:\n");
                imprimir_laberintos(juego);
                printf("Ingrese el numero de la accion que desea que el bot realize:\n");
                printf("1) Arriba, 2) Derecha, 3) Izquierda, 4) Abajo, 5) Carta : (9 si quiere salir o 0 si no quiere hacer nada) ");
                scanf("%d",&opcion);
                printf("\n");
                if (opcion == 1 || opcion == 2 || opcion == 3 || opcion ==4){
                    printf("Ingrese cuantas celdas desea moverse en la direccion escogida: ");
                    scanf("%d",&cant);
                    printf("\n");
                }
                else if(opcion == 9){
                    shmdt(juego);
                    shmctl(shmid, IPC_RMID, NULL);
                    kill(0, SIGTERM);
                    exit(1);
                }
                int opcion_cantidad = opcion * 10 + cant;
                enviar_mensaje_de_jugador(&p,&opcion_cantidad);     // mensaje con direccion desde jugador a los bots             
            }

            else if(mensaje == TERMINO_JUEGO)
                break;
            
            else {               
                int opcion = mensaje / 10;  //primer digito
                int cantidad = mensaje % 10;    // segundo digito
                int opcion_cantidad[2] = {opcion, cantidad};
                accion(&p,opcion_cantidad,juego);
                enviar_mensaje_de_jugador(&p, opcion_cantidad);
            }                                                   
        }
    }
       
    else {  // Proceso padre

        while (juego->ronda_actual <= juego->rondas_totales){ // Ciclo para turnos

            printf("Turno numero %d de %d\n", juego->ronda_actual, juego->rondas_totales);            
            juego->turno_actual = 0;
            mensaje = PLAYER;
            enviar_mensaje_a_jugador(&jugador[0],&mensaje);   // Enviar mensaje para que juegue jugador 1
            leer_mensaje_de_jugador(&jugador[0],&mensaje);    // Leer mensaje que jugador 1 termino
            if(ver_termino(juego) == 1){
                printf("Los jugadores consiguieron los tesoros y volvieron al tablero inicial, han ganado!\n");
                break;  // Termino el juego
            }
            juego->turno_actual++;

            for(int z = 1 ; z <= 3; z++){

                mensaje = BOT;
                // Ahora van los bots
                enviar_mensaje_a_jugador(&jugador[0],&mensaje); // Enviar mensaje a jugador 1 de que va el bot (jugador 2)
                leer_mensaje_de_jugador(&jugador[0],&mensaje);  // Se recibe del jugador 1, la acción que realiza el siguiente jugador bot
		    						// Se recibe en mensaje primer digito tipo de movimiento y segundo cuanto se mueve

                enviar_mensaje_a_jugador(&jugador[z], &mensaje); // Enviar a jugador bot, que tipo de accion realiza
                leer_mensaje_de_jugador(&jugador[z],&mensaje);  // Esperar a jugador bot que termine su turno

                if(ver_termino(juego) == 1){
                    printf("Los jugadores consiguieron los tesoros y volvieron al tablero inicial, han ganado!\n");
                    juego->ronda_actual = juego->rondas_totales + 1;  // Para salir del while
                    break;  // Para salir del for
                }

                juego->turno_actual++;
            } 
                            
            printf("-----------------------\n");
            juego->ronda_actual++;
        }

        if(ver_termino(juego) != 1)
            printf("Se acabaron los turnos y no han podido ganar\n");

        printf("Tablero final:\n");
        imprimir_laberintos(juego);
        
        mensaje = TERMINO_JUEGO;  // Mensaje especial para terminar
        enviar_mensaje_a_jugador(&jugador[0],&mensaje);
        enviar_mensaje_a_jugador(&jugador[1],&mensaje);
        enviar_mensaje_a_jugador(&jugador[2],&mensaje);
        enviar_mensaje_a_jugador(&jugador[3],&mensaje); 
        for(int i = 0; i < 4; i++) borrar_jugador(&jugador[i]);       
    }

    shmdt(juego);
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}
