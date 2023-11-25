Felipe Leon 202173598-4
Benjamin Lopez 202173533-k


Para ejecutar escribir en una terminal batch
1) make compile
2) make run

para limpiar archivos ejecutables escribir make clean


Instrucciones

- Si es que se mueve de un laberinto a otro, y con la cantidad de movimientos restantes se sale del mapa, se va a pedir al jugador que empieze desde el inicio del siguiente laberinto, se puede ingresar 0 para quedarse en el mismo lugar (inicio del otro laberinto)

- Para realizar la busqueda de otro laberinto, se debe tener una casilla adyacente que sea 'B' y ahi hacer uso de la carta 'Buscar' (fue preguntado al ayudante si se podia realizar asi), luego se crea una puerta 'P' que significa que esos laberintos estan conectados y a traves de esa puerta 'P' solamente se puede pasar. 

- Importante si se quiere interrumpir el programa, ingresar el número 9 (cuando se pide que acción realizar), si se interrumpe así nomas puede causar errores en ejecuciones posteriores 
  debido al uso de memoria compartida

- La máxima cantidad de casillas que puede avanzar el jugador es 9

- Los errores al no estar especificados como se manejan, los controlo a traves de los procesos de los bots en el caso de que sea el turno de un bot o si no en el del jugador 1

- P1, P2, P3 y P4 hace referencia a los portales que pueden ocupar para teletransportarse cada jugador

- En el codigo, se trabajo con letras para identificar jugadores, portales y casillas especiales

- Para llegar a una casilla especial (portal, + 5 turnos o resta de turnos), se debe seleccionar la cantidad exacta de movimientos a realizar, si hace mas movimientos, pasa de largo

Casillas especiales: Camara (R), + 5 turnos (T), - 3 turnos (Y)

J1 (Z) - PORTAL P1 (A)
J2 (X) - PORTAL P2 (S)
J3 (C) - PORTAL P3 (D)
J4 (V) - PORTAL P4 (F)

- El jugador 1 le envia si realizar movimiento o usar la carta a los bots, si realiza el movimiento, ademas tambien le envia la cantidad de celdas a moverse,
  cualquier accion adicional a este movimiento o uso de carta, se realiza en el mismo proceso del bot (fue preguntado al ayudante si se podia realizar asi)

- Ante cualquier error no manejado, se debe solamente de ingresar 9 en la consola para salirse del juego y volver a iniciarlo, aunque se deberian de manejar todos los errores posibles
