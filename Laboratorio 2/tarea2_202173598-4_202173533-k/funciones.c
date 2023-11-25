#include "funciones.h"

char* cartas[5] = {"Arriba", "Derecha", "Izquierda", "Abajo","Carta"};
char* mazo[numero_cartas] = {"Buscar","Buscar","Escalera","Escalera"};
char* letra_jugador = "ZXCV";

void enviar_mensaje_a_jugador(Jugador* p, int* mensaje){ 
    write(p->hacia_jugador[WRITE],mensaje,sizeof(mensaje));
}

void leer_mensaje_de_padre(Jugador* p, int* mensaje){
    read(p->hacia_jugador[READ],mensaje,sizeof(mensaje));
}

void enviar_mensaje_de_jugador(Jugador *p, int* mensaje){
    write(p->desde_jugador[WRITE],mensaje,sizeof(mensaje));
}

void leer_mensaje_de_jugador(Jugador* p, int* mensaje){
    read(p->desde_jugador[READ],mensaje,sizeof(mensaje));
}

void borrar_jugador(Jugador* jugador){
    if(jugador->pid == 0){
        close(jugador->hacia_jugador[READ]);
        close(jugador->desde_jugador[WRITE]);
    }
    else{
        close(jugador->hacia_jugador[WRITE]);
        close(jugador->desde_jugador[READ]);
    }
}

void desordenar_cartas(char *array[]) {
    srand(time(NULL)); 
    for (int i = 4 - 1; i > 0; i--) {
        int j = rand() % (i + 1);  // Generar un índice aleatorio entre 0 e i
        
        // Realizar el swap
        char *temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

void desordenar_laberintos(char laberintos[9][5][5]) {
    srand(time(NULL));
    for (int i = 9 - 1; i > 0; i--) {   // > 1 para no desordenar el inicial
        int j = 1 + rand() % i;
        // Realizar intercambio
        char temp[5][5];
        for (int x = 0; x < 5; x++) {
            for (int y = 0; y < 5; y++) {
                temp[x][y] = laberintos[i][x][y];
                laberintos[i][x][y] = laberintos[j][x][y];
                laberintos[j][x][y] = temp[x][y];
            }
        }
    }
}

int posicion_valida(Punto salida, char tablero[5][5]) {
    if (salida.x == 0 && salida.y == 2 && tablero[4][2] == 'B') return ARRIBA;    // hacia arriba
    if (salida.x == 2 && salida.y == 0 && tablero[2][4] == 'B') return IZQUIERDA;   // izquierda
    if (salida.x == 4 && salida.y == 2 && tablero[0][2] == 'B') return ABAJO; // abajo    
    if (salida.x == 2 && salida.y == 4 && tablero[2][0] == 'B') return DERECHA; // derecha
    return 0;
}

int seleccionar_laberinto(int laberintoActual, Punto salida, Juego* juego) {
    for (int i = 1; i < numero_tableros; i++) {
        if (i != juego->tablero_jugador[juego->turno_actual] && juego->posicion_tablero[i].x == -20) {        
            if (posicion_valida(salida, juego->tablero[i]) == ARRIBA){
            	for (int j = 0; j < numero_tableros; j++){
            		if (juego->posicion_tablero[j].x == juego->posicion_tablero[laberintoActual].x - 1 && 
            		juego->posicion_tablero[j].y == juego->posicion_tablero[laberintoActual].y)
            			return 0;
            	}
            	return i;                              
            }
            else if (posicion_valida(salida, juego->tablero[i]) == IZQUIERDA){
            	for (int j = 0; j < numero_tableros; j++){
            		if (juego->posicion_tablero[j].x == juego->posicion_tablero[laberintoActual].x && 
            		juego->posicion_tablero[j].y == juego->posicion_tablero[laberintoActual].y - 1)
            			return 0;
            	}
            	return i;              
            }
            else if (posicion_valida(salida, juego->tablero[i]) == DERECHA){
            	for (int j = 0; j < numero_tableros; j++){
            		if (juego->posicion_tablero[j].x == juego->posicion_tablero[laberintoActual].x && 
            		juego->posicion_tablero[j].y == juego->posicion_tablero[laberintoActual].y + 1)
            			return 0;
            	}
            	return i;           
            }
            else if (posicion_valida(salida, juego->tablero[i]) == ABAJO){
            	for (int j = 0; j < numero_tableros; j++){
            		if (juego->posicion_tablero[j].x == juego->posicion_tablero[laberintoActual].x + 1 && 
            		juego->posicion_tablero[j].y == juego->posicion_tablero[laberintoActual].y)
            			return 0;
            	}
            	return i;               
            }
        }
    }
    return 0;	// No hay laberinto para esa puerta
}


void identificar_direccion(int dir, Punto posicion_tablero, Juego* juego, int nuevo_laberinto) {

    if(dir == ARRIBA){
        juego->posicion_tablero[nuevo_laberinto].x = posicion_tablero.x - 1;
        juego->posicion_tablero[nuevo_laberinto].y = posicion_tablero.y;        
    }
    else if(dir == IZQUIERDA){
        juego->posicion_tablero[nuevo_laberinto].x = posicion_tablero.x;
        juego->posicion_tablero[nuevo_laberinto].y = posicion_tablero.y - 1;
    }
    else if(dir == DERECHA){
        juego->posicion_tablero[nuevo_laberinto].x = posicion_tablero.x;
        juego->posicion_tablero[nuevo_laberinto].y = posicion_tablero.y + 1;
    }
    else if(dir == ABAJO){
        juego->posicion_tablero[nuevo_laberinto].x = posicion_tablero.x + 1;
        juego->posicion_tablero[nuevo_laberinto].y = posicion_tablero.y;
    }

    // Si ya hay un laberinto descubierto, necesito conectarlo con el
    // Ver primero
    int lab_instalado;
    if(juego->tablero[nuevo_laberinto][0][2] == 'B'){   // hay una B arriba
        lab_instalado = -1;
        // Buscar laberinto que esta arriba
        for(int z = 0; z < numero_tableros; z++){
            if(juego->posicion_tablero[z].x == juego->posicion_tablero[nuevo_laberinto].x - 1
                && juego->posicion_tablero[z].y == juego->posicion_tablero[nuevo_laberinto].y){
                lab_instalado = z;
                break;
            }
        }
        if (lab_instalado != -1 && juego->tablero[lab_instalado][4][2] == 'B'){ // ya hay un nuevo laberinto arriba, necesito cambiar B por P en ambos laberintos
            juego->tablero[nuevo_laberinto][0][2] = 'P'; // lab descubierto
            juego->tablero[lab_instalado][4][2] = 'P'; // lab ya instalado
        }              
    }
    if(juego->tablero[nuevo_laberinto][2][0] == 'B'){   // hay una B en izq
        lab_instalado = -1;
        for(int z = 0; z < numero_tableros; z++){
            if(juego->posicion_tablero[z].x == juego->posicion_tablero[nuevo_laberinto].x
                && juego->posicion_tablero[z].y == juego->posicion_tablero[nuevo_laberinto].y - 1){
                lab_instalado = z;
                break;
            }
        }
        if (lab_instalado != -1 && juego->tablero[lab_instalado][2][4] == 'B'){ // ya hay un nuevo laberinto a la izquierda, necesito cambiar B por P en ambos laberintos
            juego->tablero[nuevo_laberinto][2][0] = 'P'; // lab descubierto
            juego->tablero[lab_instalado][2][4] = 'P'; // lab ya instalado
        } 
    }
    if(juego->tablero[nuevo_laberinto][4][2] == 'B'){ // hay una B abajo
        lab_instalado = -1;
        // Buscar laberinto que esta abajo
        for(int z = 0; z < numero_tableros; z++){
            if(juego->posicion_tablero[z].x == juego->posicion_tablero[nuevo_laberinto].x + 1
                && juego->posicion_tablero[z].y == juego->posicion_tablero[nuevo_laberinto].y){
                lab_instalado = z;
                break;
            }
        }
        if (lab_instalado != -1 && juego->tablero[lab_instalado][0][2] == 'B'){ // ya hay un nuevo laberinto abajo, necesito cambiar B por P en ambos laberintos
            juego->tablero[nuevo_laberinto][4][2] = 'P'; // lab descubierto
            juego->tablero[lab_instalado][0][2] = 'P'; // lab ya instalado
        } 
    }
    if(juego->tablero[nuevo_laberinto][2][4] == 'B'){ // hay una B a la derecha 
        lab_instalado = -1;  
        for(int z = 0; z < numero_tableros; z++){
            if(juego->posicion_tablero[z].x == juego->posicion_tablero[nuevo_laberinto].x
                && juego->posicion_tablero[z].y == juego->posicion_tablero[nuevo_laberinto].y + 1){
                lab_instalado = z;
                break;
            }
        }
        if (lab_instalado != -1 && juego->tablero[lab_instalado][2][0] == 'B'){ // ya hay un nuevo laberinto a la derecha, necesito cambiar B por P en ambos laberintos
            juego->tablero[nuevo_laberinto][2][4] = 'P'; // lab descubierto
            juego->tablero[lab_instalado][2][0] = 'P'; // lab ya instalado
        } 
    }
}

int teletransporte(Juego* juego, int id_jugador){
    int x,y;
    printf("Ingresa coordenadadas del laberinto al cual quieres usar el portal (tablero inicio es x = 0 e y = 0)\n");
    printf("Para arriba disminuye 1 en x (abajo aumenta 1) y para la izquierda disminuye 1 en y (derecha aumenta 1)\n");
    printf("Coordenada x: ");
    scanf("%d",&x);
    printf("Coordenada y: ");
    scanf("%d",&y);
    // Buscar laberinto con esa coordenada
    int laberinto_tp = -1;
    int i;
    for(i = 0; i < numero_tableros; i++){
        if(juego->posicion_tablero[i].x == x && juego->posicion_tablero[i].y == y){
            laberinto_tp = i;
            break;
        }
    }
    if(laberinto_tp != -1 && ( (id_jugador == 0 && juego->tablero[laberinto_tp][juego->casillas_especiales[laberinto_tp].x][juego->casillas_especiales[laberinto_tp].y] == 'A')
        || (id_jugador == 1 && juego->tablero[laberinto_tp][juego->casillas_especiales[laberinto_tp].x][juego->casillas_especiales[laberinto_tp].y] == 'S') 
        || (id_jugador == 2 && juego->tablero[laberinto_tp][juego->casillas_especiales[laberinto_tp].x][juego->casillas_especiales[laberinto_tp].y] == 'D')
        || (id_jugador == 3 && juego->tablero[laberinto_tp][juego->casillas_especiales[laberinto_tp].x][juego->casillas_especiales[laberinto_tp].y] == 'F') )){
        printf("tp -> x: %d, y: %d\n",juego->casillas_especiales[laberinto_tp].x, juego->casillas_especiales[laberinto_tp].y);
        juego->tablero_jugador[id_jugador] = laberinto_tp;
        juego->posiciones[id_jugador].x = juego->casillas_especiales[laberinto_tp].x;
        juego->posiciones[id_jugador].y = juego->casillas_especiales[laberinto_tp].y;
        if(id_jugador == 0)
            juego->casilla_anterior[id_jugador] = 'A';
        else if(id_jugador == 1)
            juego->casilla_anterior[id_jugador] = 'S';
        else if(id_jugador == 2)
            juego->casilla_anterior[id_jugador] = 'D';
        else if(id_jugador == 4)
            juego->casilla_anterior[id_jugador] = 'F';
        return 1;
    }
    else{
    	printf("No te puedes teletransportar a donde quieres, vuelves a tu posicion inicial\n");
    	return 0;
	}
}

void casilla_final(Juego *j , Jugador *p){

     // Ver si es que recoge un tesoro
    if (p->id_jugador == 0 && j->tablero[j->tablero_jugador[j->turno_actual]][j->posiciones[p->id_jugador].x][j->posiciones[p->id_jugador].y] == '1'){
        j->tesoros[p->id_jugador] = 1;
        printf("Jugador 1 ha recogido un tesoro!\n");
    }       
    else if (p->id_jugador == 1 && j->tablero[j->tablero_jugador[j->turno_actual]][j->posiciones[p->id_jugador].x][j->posiciones[p->id_jugador].y] == '2'){
        j->tesoros[p->id_jugador] = 1;
        printf("Jugador 2 ha recogido un tesoro!\n");
    }
    else if (p->id_jugador == 2 && j->tablero[j->tablero_jugador[j->turno_actual]][j->posiciones[p->id_jugador].x][j->posiciones[p->id_jugador].y] == '3'){
        j->tesoros[p->id_jugador] = 1;
        printf("Jugador 3 ha recogido un tesoro!\n");
    }
    else if (p->id_jugador == 3 && j->tablero[j->tablero_jugador[j->turno_actual]][j->posiciones[p->id_jugador].x][j->posiciones[p->id_jugador].y] == '4'){
        j->tesoros[p->id_jugador] = 1;
        printf("Jugador 4 ha recogido un tesoro!\n");
    }

    // Ver si llega a una casilla especial
    if (j->tablero[j->tablero_jugador[j->turno_actual]][j->posiciones[p->id_jugador].x][j->posiciones[p->id_jugador].y] == 'R'){ // Camaras
        j->contador_camaras--; // Resto 1 a camara
        printf("Camara eliminada\n");
    }
    
    if (j->tablero[j->tablero_jugador[j->turno_actual]][j->posiciones[p->id_jugador].x][j->posiciones[p->id_jugador].y] == 'Y'){ // Pierde 3 turnos
        j->rondas_totales = j->rondas_totales - 3; // Pierde 3 turnos
        printf("Jugadores pierden 3 turnos\n");
    }
}


void agregar_evento(Juego* juego, int nuevo_laberinto){

    srand(time(NULL));
    int cant = 0;
    int n_tesoro, aleatorio;

    if ((juego->laberintos_pordescubrir == 4 && juego->tesoros_inst == 0) || (juego->laberintos_pordescubrir == 3 && juego->tesoros_inst == 1)  // tengo que instalar si o si en estos casos
        || (juego->laberintos_pordescubrir == 2 && juego->tesoros_inst == 2) || (juego->laberintos_pordescubrir == 1 && juego->tesoros_inst == 3)){
        aleatorio = 0;
    }
    else {
        aleatorio = rand() % 2;
    }

    // Ver cuantos '0' hay
    for (int fila = 0; fila < 5; fila++){
        for (int col = 0; col < 5; col++){
            if(juego->tablero[nuevo_laberinto][fila][col] == '0')
                cant++;
        }            
    }
    int elegido = (rand() % cant) + 1;  // numero entre 1 y la cantidad maxima de 0s

    if(juego->tesoros_inst == 4 && aleatorio == 0)
        aleatorio = 1;

    if (aleatorio == 0){    // Tesoro

        do {
            n_tesoro = rand() % 4;
        } while (juego->tesoros_instalados[n_tesoro] != 0); // Si no es igual a 0, busco una que sea 0, es decir, no se haya agregado todavia el tesoro de ese jugador

        printf("Se instala tesoro del jugador: %d\n",n_tesoro+1);

        cant = 0;
        // Ahora pongo el numero del tesoro
        for (int fila1 = 0; fila1 < 5; fila1++){
            for (int col1 = 0; col1 < 5; col1++){
                if(juego->tablero[nuevo_laberinto][fila1][col1] == '0'){
                    cant++;
                    if (cant == elegido){
                        juego->tablero[nuevo_laberinto][fila1][col1] = (char)('0' + (n_tesoro + 1));
                        break;
                    }
                }
            }            
        }

        juego->tesoros_instalados[n_tesoro] = 1;
        juego->tesoros_inst++;
    }
    else {  // Casillas especiales
        int tipo = rand() % 4;
        char letra;

        if(tipo == 0){  // Camara LETRA R
            letra = 'R';
            juego->contador_camaras++;
            printf("Casilla especial de camara\n");
        }
        else if (tipo == 1){    // + 5 turnos LETRA T
            letra = 'T';
            printf("Casilla especial para ganar 5 turnos\n");
        }
        else if (tipo == 2){    // - 3 turnos LERTA Y
            letra = 'Y';
            printf("Casilla especial para perder 3 turnos\n");
        }
        else { // tipo == 3 (teletransporte)

            int jugador = rand() % 4;   // de que jugador se crea el tp
            if (jugador == 0){ // jugador 1
                letra = 'A';
            }
            else if (jugador == 1){ // J2
                letra = 'S';
            }
            else if (jugador == 2){ // J3
                letra = 'D';
            }
            else if (jugador == 3){ // J4
                letra = 'F';
            }
            printf("Casilla especial de teletransporte de jugador %d\n", jugador + 1);
        }

        cant = 0;
        // Ahora pongo el numero casilla especial
        for (int fila1 = 0; fila1 < 5; fila1++){
            for (int col1 = 0; col1 < 5; col1++){
                if(juego->tablero[nuevo_laberinto][fila1][col1] == '0'){
                    cant++;
                    if (cant == elegido){
                        juego->casillas_especiales[nuevo_laberinto].x = fila1;
                        juego->casillas_especiales[nuevo_laberinto].y = col1;
                        juego->tablero[nuevo_laberinto][fila1][col1] = letra;
                        break;
                    }
                }
            }            
        }
    }
}

int ver_termino(Juego* juego){
    if (juego->tablero_jugador[0] == 0 && juego->tablero_jugador[1] == 0 && juego->tablero_jugador[2] == 0 && juego->tablero_jugador[3] == 0
        && juego->tesoros[0] == 1 && juego->tesoros[1] == 1 && juego->tesoros[2] == 1 && juego->tesoros[3] == 1 )
        return 1;  
    return 0;
}

void imprimir_laberintos(Juego* juego) {
    
    int minX = 0, maxX = 0, minY = 0, maxY = 0;
    for (int i = 0; i < numero_tableros; i++) {
        if (juego->posicion_tablero[i].x != -20) {
            if (juego->posicion_tablero[i].x < minX) minX = juego->posicion_tablero[i].x;
            if (juego->posicion_tablero[i].x > maxX) maxX = juego->posicion_tablero[i].x;
            if (juego->posicion_tablero[i].y < minY) minY = juego->posicion_tablero[i].y;
            if (juego->posicion_tablero[i].y > maxY) maxY = juego->posicion_tablero[i].y;
        }
    }
    for (int x = minX; x <= maxX; x++) {
        for (int fila = 0; fila < 5; fila++) {
            for (int y = minY; y <= maxY; y++) {
                int laberinto = -1;
                for (int i = 0; i < numero_tableros; i++) {
                    if (juego->posicion_tablero[i].x != -20 && juego->posicion_tablero[i].x == x && juego->posicion_tablero[i].y == y) {
                        laberinto = i;
                        break;
                    }
                }
                printf(" "); // borde izq
                if (laberinto != -1) {
                    for (int col = 0; col < 5; col++) {
                        char caracter = juego->tablero[laberinto][fila][col];
                        switch (caracter){
                            // Jugador 1 y su portal
                            case 'Z':
                                printf("\033[31m%s \033[0m", "J1"); // Rojo
                                break;
                            case 'A':
                                printf("\033[31m%s \033[0m", "P1"); // Rojo
                                break;
                            case '1':
                                printf("\033[31m%c  \033[0m", '1');
                                break;
                            // Jugador 2 y su portal
                            case 'X':
                                printf("\033[32m%s \033[0m", "J2"); // Verde
                                break;
                            case 'S':
                                printf("\033[32m%s \033[0m", "P2"); // Verde
                                break;
                            case '2':
                                printf("\033[32m%c  \033[0m", '2');
                                break;
                            // Jugador 3 y su portal
                            case 'C':
                                printf("\033[33m%s \033[0m", "J3"); // Azul
                                break;
                            case 'D':
                                printf("\033[33m%s \033[0m", "P3"); // Azul
                                break;
                            case '3':
                                printf("\033[33m%c  \033[0m", '3');
                                break;
                            // Jugador 4 y su portal
                            case 'V':
                                printf("\033[34m%s \033[0m", "J4"); // Amarillo
                                break;
                            case 'F':
                                printf("\033[34m%s \033[0m", "P4"); // Amarillo
                                break;
                            case '4':
                                printf("\033[34m%c  \033[0m", '4');
                                break;
                            // Casilla B, P y E
                            case 'E':
                                printf("\033[35m%c  \033[0m", caracter);  // Rosado
                                break;
                            case 'B':
                                printf("\033[37m%c  \033[0m", caracter);  // Morado
                                break;
                            case 'P':
                                printf("\033[36m%c  \033[0m", caracter);  // Morado
                                break;
                            default:
                                printf("%c  ", caracter);
                                break;
                        }
                        printf(" ");
                    }
                } else {
                    for (int col = 0; col < 5; col++) {
                        printf("    "); // Espacio en blanco si no hay laberinto en esa posición
                    }
                }
                printf(" "); //borde derecho
            }
            printf("\n");
        }

        for (int y = minY; y <= maxY; y++) {
            printf("       "); // 5 espacios para el contenido + 2 para el borde
        }
        printf("\n");
    }

    // Imprimir aquellos que tengan tesoros
    for (int i = 0; i < 4; i++){
        if(juego->tesoros[i] == 1){
            printf("Jugador J%d tiene su tesoro!\n",i+1);
        }
    }

    printf("Laberintos restantes: %d, Tesoros instalados: %d, Camaras instaladas: %d\n",juego->laberintos_pordescubrir, juego->tesoros_inst, juego->contador_camaras);
}

void crear_juego(Juego* juego){

    FILE* fp = fopen("Inicio.txt", "r");
    if (fp == NULL) {
        printf("Error al abrir el archivo.\n");
        exit(1);
    }
    
    char linea[20];
    int fila = 0;
    // Cargar tablero de inicio
    while (fgets(linea, sizeof(linea), fp)) {
        char elementos[5][3]; // Para almacenar cada elemento de la línea, maximo seria J1 + caracter nulo
        sscanf(linea, "%s %s %s %s %s", elementos[0], elementos[1], elementos[2], elementos[3], elementos[4]);

        for (int columna = 0; columna < 5; columna++) {
            // elementos[columna] contiene cada %s que es separada por espacio de la linea
            if (elementos[columna][0] == 'J') {
                int jugador = elementos[columna][1] - '1';  // Convertir J1 en 0, J2 en 1 ...
                juego->posiciones[jugador].x = fila;
                juego->posiciones[jugador].y = columna;
                if (jugador == 0){
                    juego->tablero[0][fila][columna] = 'Z';
                }
                else if (jugador == 1){
                    juego->tablero[0][fila][columna] = 'X';
                }
                else if (jugador == 2){
                    juego->tablero[0][fila][columna] = 'C';
                }
                else if (jugador == 3){
                    juego->tablero[0][fila][columna] = 'V';
                }                
            } 
            else {
                juego->tablero[0][fila][columna] = elementos[columna][0];
            }            
        }
        fila++;
    }

    // El tablero inicial esta en el centro
    juego->posicion_tablero[0].x = 0;
    juego->posicion_tablero[0].y = 0;
    
    // Inicializo casillas especiales de laberinto inicial
    juego->casillas_especiales[0].x = -20;
    juego->casillas_especiales[0].y = -20;
   
    // Cargar los demas tableros
    for (int i = 1; i <= 8; i++){
        char tab[20] = "tablero";
        char concatenar[10]; 

        sprintf(concatenar, "%d", i); // Convierte el número a cadena
        strcat(tab, concatenar);
        strcat(tab,".txt");

        fp = fopen(tab,"r");

        fila = 0;
        int cant = 0; 
        while (fgets(linea, sizeof(linea), fp)) {
            char elementos[5][2]; 
            sscanf(linea, "%s %s %s %s %s", elementos[0], elementos[1], elementos[2], elementos[3], elementos[4]);

            for (int columna = 0; columna < 5; columna++){
                juego->tablero[i][fila][columna] = elementos[columna][0];
                if(juego->tablero[i][fila][columna] == '0')
                    cant++;
            }                     
            fila++;
        }
        
        // Posicion de demas tableros
        juego->posicion_tablero[i].x = -20;
        juego->posicion_tablero[i].y = -20;


        juego->casillas_especiales[i].x = -20;
        juego->casillas_especiales[i].y = -20;
    }

    // Turno del jugador
    juego->turno_actual = 0;

    // Ronda actual
    juego->ronda_actual = 1;

    // Maximo de rondas
    juego->rondas_totales = 15;

    // Laberintos por descubrir
    juego->laberintos_pordescubrir = 8;

    // Contador de camaras
    juego->contador_camaras = 0;

    // Cantidad de tesoros instalados
    juego->tesoros_inst = 0;

    // Inicializar todos los jugadores en laberinto 0 y tesoros en 0
    for (int i = 0; i < 4; i++){
        juego->tablero_jugador[i] = 0;
        juego->tesoros[i] = 0;
        juego->casilla_anterior[i] = '0';
    }  
    fclose(fp);
}

void accion(Jugador *p, int opcion_cantidad[2], Juego *j){

    int tab_actual = j->tablero_jugador[p->id_jugador];
    int x_actual = j->posiciones[p->id_jugador].x;
    int y_actual = j->posiciones[p->id_jugador].y;

    // Guardar la posición inicial del jugador para borrarla después
    int x_inicial = x_actual;
    int y_inicial = y_actual;

    if (opcion_cantidad[0] == 5){

        if (strcmp(p->carta, "Buscar") == 0){

            Punto salida;

            // Ver en que casilla adyacente esta la letra B
            
            // Abajo
            if (j->tablero[tab_actual][x_actual + 1][y_actual] == 'B'){
                salida.x = x_actual + 1;
                salida.y = y_actual;
            }
            // Arriba
            else if (j->tablero[tab_actual][x_actual - 1][y_actual] == 'B'){
                salida.x = x_actual - 1;
                salida.y = y_actual;
            }

            // Derecha
            else if (j->tablero[tab_actual][x_actual][y_actual + 1] == 'B'){
                salida.x = x_actual;
                salida.y = y_actual + 1;
            }

            // Izquierda
            else if (j->tablero[tab_actual][x_actual][y_actual - 1] == 'B'){
                salida.x = x_actual;
                salida.y = y_actual - 1;
            }

            else {
                printf("No hay una casilla 'B' adyacente a usted.\n");
                return;
            }

            int nuevo_laberinto = seleccionar_laberinto(tab_actual, salida, j);

            if (nuevo_laberinto != 0){
                // Reemplazar 'B' por 'P' en el laberinto actual
                j->tablero[tab_actual][salida.x][salida.y] = 'P';
                int dir;

                // Reemplazar 'B' por 'P' en el nuevo laberinto
                if(posicion_valida(salida, j->tablero[nuevo_laberinto]) == ARRIBA){
                    j->tablero[nuevo_laberinto][4][2] = 'P';
                    dir = ARRIBA;
                }
                else if(posicion_valida(salida, j->tablero[nuevo_laberinto]) == IZQUIERDA){
                    j->tablero[nuevo_laberinto][2][4] = 'P';
                    dir = IZQUIERDA;
                }
                else if(posicion_valida(salida, j->tablero[nuevo_laberinto]) == DERECHA){
                    j->tablero[nuevo_laberinto][2][0] = 'P';
                    dir = DERECHA;
                }
                else if(posicion_valida(salida, j->tablero[nuevo_laberinto]) == ABAJO){
                    j->tablero[nuevo_laberinto][0][2] = 'P';
                    dir = ABAJO;
                }

                // Agregar laberinto en mapa general
                identificar_direccion(dir, j->posicion_tablero[tab_actual], j, nuevo_laberinto);
                j->laberintos_pordescubrir--; 
                // Agregar evento
                agregar_evento(j,nuevo_laberinto);             
            }
            else{
                printf("No hay laberinto que pueda ser descubierto hacia esa direccion\n");
                return;
            }
        }
        else if (strcmp(p->carta, "Escalera") == 0){

            if (j->tablero[tab_actual][x_actual + 1][y_actual] == 'E')
                j->tablero[tab_actual][x_actual + 1][y_actual] = '0';
            else if (j->tablero[tab_actual][x_actual - 1][y_actual] == 'E')
                j->tablero[tab_actual][x_actual - 1][y_actual] = '0';
            else if (j->tablero[tab_actual][x_actual][y_actual + 1] == 'E')
                j->tablero[tab_actual][x_actual][y_actual + 1] = '0';
            else if (j->tablero[tab_actual][x_actual][y_actual - 1] == 'E')
                j->tablero[tab_actual][x_actual][y_actual - 1] = '0';
            else
                printf("No hay ninguna casilla escalera cerca tuyo\n");
        }
    }

    else {
        int cant = opcion_cantidad[1];
        int casilla_inicio = j->casilla_anterior[j->turno_actual];
        
        if (opcion_cantidad[0] == 1){   // Arriba
            int i;
            // Verificar si hay algo en el camino primero
            for (i = 1; i <= cant; i++) {
                char casilla_actual = j->tablero[tab_actual][x_actual - i][y_actual];

                // Se sale del mapa
                if (x_actual - i < 0 && j->casilla_anterior[j->turno_actual] != 'P'){
                    printf("Te estas saliendo del mapa, por favor ingrese una cantidad a moverse valida: , ingrese 0 para cancelar\n");
                    scanf("%d",&cant);
                    if (cant == 0){
                    	j->casilla_anterior[j->turno_actual] = casilla_inicio;                  	
                    	if(casilla_inicio == 'P'){
                        j->tablero[tab_actual][x_inicial][y_inicial] = letra_jugador[p->id_jugador];
                    	}
                        return;
                    }
                    opcion_cantidad[1] = cant;
                    j->casilla_anterior[j->turno_actual] = casilla_inicio;
                    accion(p,opcion_cantidad,j);
                    return;
                }

                // Caso de pasar a otro laberinto
                if(x_actual - i < 0 && j->casilla_anterior[j->turno_actual] == 'P'){
                    
                    int nuevo_laberinto;
                    int mov_restantes = cant - i;
                    int actual = tab_actual;

                    for(int z = 0; z < numero_tableros; z++){
                        if(j->posicion_tablero[z].x == j->posicion_tablero[actual].x - 1
                            && j->posicion_tablero[z].y == j->posicion_tablero[actual].y){
                            nuevo_laberinto = z;
                            break;
                        }
                    }

                    int nuevo_laberinto_aux = nuevo_laberinto;
                    int mov_restantes_aux = mov_restantes + 1;

                    while(mov_restantes_aux > 0){
                        // Buscar laberinto que esta abajo
                        for(int z = 0; z < numero_tableros; z++){
                            if(j->posicion_tablero[z].x == j->posicion_tablero[actual].x - 1
                                && j->posicion_tablero[z].y == j->posicion_tablero[actual].y){
                                nuevo_laberinto_aux = z;
                                break;
                            }
                        }
                        // ver si topa con el algo en el siguiente laberinto
                        int z;
                        for(z = 0; z <= 4; z++){
                            if(z == mov_restantes_aux){
                                break; 
                            }
                            char a = j->tablero[nuevo_laberinto_aux][4 - z][2];
                            if(a == 'Z' || a == 'X' || a == 'C' || a == 'V' || a == '/' || a == 'B' || a == 'E' 
                                || (p->id_jugador == 0 && (a == '2' || a == '3' || a == '4' || a == 'S' || a == 'D' || a == 'F'))
                                || (p->id_jugador == 1 && (a == '1' || a == '3' || a == '4' || a == 'A' || a == 'D' || a == 'F'))
                                || (p->id_jugador == 2 && (a == '1' || a == '2' || a == '4' || a == 'A' || a == 'S' || a == 'F'))
                                || (p->id_jugador == 3 && (a == '1' || a == '2' || a == '3' || a == 'A' || a == 'S' || a == 'D'))
                                ){
                                printf("Estas topando con una casilla que no puedes avanzar, ingresa una cantidad correcta de movimientos o ingresa 0 para cancelar\n");
                                scanf("%d",&cant);
                                if (cant == 0)
                                    return;
                                opcion_cantidad[1] = cant;
                                j->casilla_anterior[j->turno_actual] = casilla_inicio;
                                accion(p,opcion_cantidad,j);
                                return;
                            } 
                        }
                        mov_restantes_aux = mov_restantes_aux - z;
                        actual = nuevo_laberinto_aux;              
                    }
                    if (casilla_inicio == '0' || casilla_inicio == '1' || casilla_inicio == '2' || casilla_inicio == '3' || casilla_inicio == '4' 
                        || casilla_inicio == 'R' || casilla_inicio == 'T' || casilla_inicio == 'Y')
                        j->tablero[tab_actual][x_inicial][y_inicial] = '0'; // Sacar de donde empezo, si es que no empezo en una P
                    j->tablero_jugador[p->id_jugador] = nuevo_laberinto;  // Cambiar el laberinto que sigue
                    // Coordenadas que aparece en nuevo laberinto
                    j->posiciones[j->turno_actual].x = 4;  
                    j->posiciones[j->turno_actual].y = 2;
                    opcion_cantidad[1] = mov_restantes;                    
                    j->tablero[tab_actual][x_actual - i + 1][y_actual] = 'P';   // Restauro la P por donde sale del laberinto
                    j->casilla_anterior[j->turno_actual] = 'P';

                    accion(p,opcion_cantidad,j);
                    return;
                }

                // Comprobar si la casilla contiene un obstáculo o a otro jugador
                if (casilla_actual == 'Z' || casilla_actual == 'X' || casilla_actual == 'C' || casilla_actual == 'V' || 
                    casilla_actual == '/' || casilla_actual == 'B' || casilla_actual == 'E' 
                    || (p->id_jugador == 0 && (casilla_actual == '2' || casilla_actual == '3' || casilla_actual == '4' 
                    || casilla_actual == 'S' || casilla_actual == 'D' || casilla_actual == 'F'))
                    || (p->id_jugador == 1 && (casilla_actual == '1' || casilla_actual == '3' || casilla_actual == '4'
                    || casilla_actual == 'A' || casilla_actual == 'D' || casilla_actual == 'F'))
                    || (p->id_jugador == 2 && (casilla_actual == '1' || casilla_actual == '2' || casilla_actual == '4'
                    || casilla_actual == 'A' || casilla_actual == 'S' || casilla_actual == 'F'))
                    || (p->id_jugador == 3 && (casilla_actual == '1' || casilla_actual == '2' || casilla_actual == '3'
                    || casilla_actual == 'A' || casilla_actual == 'S' || casilla_actual == 'D'))
                    ){
                    printf("Estas topando con una casilla que no puedes avanzar, ingresa una cantidad correcta de movimientos o ingresa 0 para cancelar\n");
                    scanf("%d",&cant);
                    if (cant == 0)
                        return;
                    opcion_cantidad[1] = cant;
                    j->casilla_anterior[j->turno_actual] = casilla_inicio;
                    accion(p,opcion_cantidad,j);
                    return;
                }

                if (j->casilla_anterior[j->turno_actual] == 'P'){
                    j->tablero[tab_actual][x_actual - i + 1][y_actual] = 'P';
                }
                if (j->casilla_anterior[j->turno_actual] == 'A')
                    j->tablero[tab_actual][x_actual - i + 1][y_actual] = 'A';

                if (j->casilla_anterior[j->turno_actual] == 'S')
                    j->tablero[tab_actual][x_actual - i + 1][y_actual] = 'S';

                if (j->casilla_anterior[j->turno_actual] == 'D')
                    j->tablero[tab_actual][x_actual - i + 1][y_actual] = 'D';

                if (j->casilla_anterior[j->turno_actual] == 'F')
                    j->tablero[tab_actual][x_actual - i + 1][y_actual] = 'F';
                
                j->casilla_anterior[j->turno_actual] = casilla_actual;
            }

            int movido = i - 1;
            j->posiciones[p->id_jugador].x = x_inicial - movido;  
            
            casilla_final(j,p);
			
			// Ver si realiza un teletransporte
			if ((p->id_jugador == 0 && j->tablero[j->tablero_jugador[j->turno_actual]][j->posiciones[p->id_jugador].x][j->posiciones[p->id_jugador].y] == 'A') 
            || (p->id_jugador == 1 && j->tablero[j->tablero_jugador[j->turno_actual]][j->posiciones[p->id_jugador].x][j->posiciones[p->id_jugador].y] == 'S')
            || (p->id_jugador == 2 && j->tablero[j->tablero_jugador[j->turno_actual]][j->posiciones[p->id_jugador].x][j->posiciones[p->id_jugador].y] == 'D')
            || (p->id_jugador == 3 && j->tablero[j->tablero_jugador[j->turno_actual]][j->posiciones[p->id_jugador].x][j->posiciones[p->id_jugador].y] == 'F') ){ // Teletransporte
                printf("Te puedes teletransportar a otra casilla con la misma letra\n");
                // Si es que no se puede tpear, retorna 0 y vuelve al inicio
                if (teletransporte(j,p->id_jugador) == 0){
                    j->posiciones[p->id_jugador].x = x_inicial;
                    j->casilla_anterior[j->turno_actual] = casilla_inicio;
                    if(casilla_inicio == 'P'){
                        j->tablero[tab_actual][x_inicial][y_inicial] = letra_jugador[p->id_jugador];
                    }
                    return;  	
                }
   	        }	

            // Ver si no hay camaras para obtener turnos
            if (j->tablero[j->tablero_jugador[j->turno_actual]][j->posiciones[p->id_jugador].x][j->posiciones[p->id_jugador].y] == 'T'){ // 5 turnos mas
                if (j->contador_camaras >= 2){
                    printf("Debes eliminar las camaras para poder obtener mas turnos\n");
                    // Lo devuelvo a su posicion inicial
                    j->posiciones[p->id_jugador].x = x_inicial;
                    j->casilla_anterior[j->turno_actual] = casilla_inicio;
                    if(casilla_inicio == 'P'){
                        j->tablero[tab_actual][x_inicial][y_inicial] = letra_jugador[p->id_jugador];
                    }
                    return;
                }
                else { 
                    j->rondas_totales = j->rondas_totales + 5;
                    printf("Jugadores ganan 5 turnos\n");
                }
            }

            if (casilla_inicio == '0' || casilla_inicio == '1' || casilla_inicio == '2' || casilla_inicio == '3' || casilla_inicio == '4' 
                || casilla_inicio == 'R' || casilla_inicio == 'T' || casilla_inicio == 'Y')
                j->tablero[tab_actual][x_inicial][y_inicial] = '0'; // Sacar de donde empezo, si es que no empezo en una P
            
            // Actualizar posicion
            j->tablero[j->tablero_jugador[j->turno_actual]][j->posiciones[p->id_jugador].x][j->posiciones[p->id_jugador].y] = letra_jugador[p->id_jugador];  // Actualizar la nueva posición del jugador
        }    

        else if (opcion_cantidad[0] == 4){  // Abajo
            int i;
            // Verificar si hay algo en el camino primero
            for (i = 1; i <= cant; i++) {
                char casilla_actual = j->tablero[tab_actual][x_actual + i][y_actual];

                // Se sale del mapa
                if (x_actual + i > 4 && j->casilla_anterior[j->turno_actual] != 'P'){
                    printf("Te estas saliendo del mapa, por favor ingrese una cantidad a moverse valida: , ingrese 0 para cancelar\n");
                    scanf("%d",&cant);
                    if (cant == 0){
                    	j->casilla_anterior[j->turno_actual] = casilla_inicio;                  	
                    	if(casilla_inicio == 'P'){
                        j->tablero[tab_actual][x_inicial][y_inicial] = letra_jugador[p->id_jugador];
                    	}
                        return;
                    }
                    opcion_cantidad[1] = cant;
                    j->casilla_anterior[j->turno_actual] = casilla_inicio;
                    accion(p,opcion_cantidad,j);
                    return;
                }

                // Caso de pasar a otro laberinto
                if(x_actual + i > 4 && j->casilla_anterior[j->turno_actual] == 'P'){
                    
                    int nuevo_laberinto;
                    int mov_restantes = cant - i;
                    int actual = tab_actual;

                    for(int z = 0; z < numero_tableros; z++){
                        if(j->posicion_tablero[z].x == j->posicion_tablero[actual].x + 1
                            && j->posicion_tablero[z].y == j->posicion_tablero[actual].y){
                            nuevo_laberinto = z;
                            break;
                        }
                    }

                    int nuevo_laberinto_aux = nuevo_laberinto;
                    int mov_restantes_aux = mov_restantes + 1;

                    while(mov_restantes_aux > 0){
                        // Buscar laberinto que esta abajo
                        for(int z = 0; z < numero_tableros; z++){
                            if(j->posicion_tablero[z].x == j->posicion_tablero[actual].x + 1
                                && j->posicion_tablero[z].y == j->posicion_tablero[actual].y){
                                nuevo_laberinto_aux = z;
                                break;
                            }
                        }
                        // ver si topa con el algo en el siguiente laberinto
                        int z;
                        for(z = 0; z <= 4; z++){
                            if(z == mov_restantes_aux){
                                break; 
                            }
                            char a = j->tablero[nuevo_laberinto_aux][0 + z][2];
                            if(a == 'Z' || a == 'X' || a == 'C' || a == 'V' || a == '/' || a == 'B' || a == 'E' 
                                || (p->id_jugador == 0 && (a == '2' || a == '3' || a == '4' || a == 'S' || a == 'D' || a == 'F'))
                                || (p->id_jugador == 1 && (a == '1' || a == '3' || a == '4' || a == 'A' || a == 'D' || a == 'F'))
                                || (p->id_jugador == 2 && (a == '1' || a == '2' || a == '4' || a == 'A' || a == 'S' || a == 'F'))
                                || (p->id_jugador == 3 && (a == '1' || a == '2' || a == '3' || a == 'A' || a == 'S' || a == 'D'))
                                ){
                                printf("Estas topando con una casilla que no puedes avanzar, ingresa una cantidad correcta de movimientos o ingresa 0 para cancelar\n");
                                scanf("%d",&cant);
                                if (cant == 0)
                                    return;
                                opcion_cantidad[1] = cant;
                                j->casilla_anterior[j->turno_actual] = casilla_inicio;
                                accion(p,opcion_cantidad,j);
                                return;
                            } 
                        }
                        mov_restantes_aux = mov_restantes_aux - z;
                        actual = nuevo_laberinto_aux;              
                    }

                    if (casilla_inicio == '0' || casilla_inicio == '1' || casilla_inicio == '2' || casilla_inicio == '3' || casilla_inicio == '4' 
                        || casilla_inicio == 'R' || casilla_inicio == 'T' || casilla_inicio == 'Y')
                        j->tablero[tab_actual][x_inicial][y_inicial] = '0'; // Sacar de donde empezo, si es que no empezo en una P
                    j->tablero_jugador[p->id_jugador] = nuevo_laberinto;  // Cambiar el laberinto que sigue
                    // Coordenadas que aparece en nuevo laberinto
                    j->posiciones[j->turno_actual].x = 0;  
                    j->posiciones[j->turno_actual].y = 2;
                    opcion_cantidad[1] = mov_restantes;                    
                    j->tablero[tab_actual][x_actual + i - 1][y_actual] = 'P';   // Restauro la P por donde sale del laberinto
                    j->casilla_anterior[j->turno_actual] = 'P';
                    accion(p,opcion_cantidad,j);
                    return;
                }

                // Comprobar si la casilla contiene un obstáculo o a otro jugador
                if (casilla_actual == 'Z' || casilla_actual == 'X' || casilla_actual == 'C' || casilla_actual == 'V' || 
                    casilla_actual == '/' || casilla_actual == 'B' || casilla_actual == 'E' 
                    || (p->id_jugador == 0 && (casilla_actual == '2' || casilla_actual == '3' || casilla_actual == '4' 
                    || casilla_actual == 'S' || casilla_actual == 'D' || casilla_actual == 'F'))
                    || (p->id_jugador == 1 && (casilla_actual == '1' || casilla_actual == '3' || casilla_actual == '4'
                    || casilla_actual == 'A' || casilla_actual == 'D' || casilla_actual == 'F'))
                    || (p->id_jugador == 2 && (casilla_actual == '1' || casilla_actual == '2' || casilla_actual == '4'
                    || casilla_actual == 'A' || casilla_actual == 'S' || casilla_actual == 'F'))
                    || (p->id_jugador == 3 && (casilla_actual == '1' || casilla_actual == '2' || casilla_actual == '3'
                    || casilla_actual == 'A' || casilla_actual == 'S' || casilla_actual == 'D'))
                    ){
                    printf("Estas topando con una casilla que no puedes avanzar, ingresa una cantidad correcta de movimientos o ingresa 0 para cancelar\n");
                    scanf("%d",&cant);
                    if (cant == 0)
                        return;
                    opcion_cantidad[1] = cant;
                    j->casilla_anterior[j->turno_actual] = casilla_inicio;
                    accion(p,opcion_cantidad,j);
                    return;
                }

                if (j->casilla_anterior[j->turno_actual] == 'P'){
                    j->tablero[tab_actual][x_actual + i - 1][y_actual] = 'P';
                }

                if (j->casilla_anterior[j->turno_actual] == 'A')
                    j->tablero[tab_actual][x_actual + i - 1][y_actual] = 'A';
                if (j->casilla_anterior[j->turno_actual] == 'S')
                    j->tablero[tab_actual][x_actual + i - 1][y_actual] = 'S';
                if (j->casilla_anterior[j->turno_actual] == 'D')
                    j->tablero[tab_actual][x_actual + i - 1][y_actual] = 'D';
                if (j->casilla_anterior[j->turno_actual] == 'F')
                    j->tablero[tab_actual][x_actual + i - 1][y_actual] = 'F';
                
                j->casilla_anterior[j->turno_actual] = casilla_actual;
            }

            int movido = i - 1;
            j->posiciones[p->id_jugador].x = x_inicial + movido;

            casilla_final(j,p);
            
            // Ver si realiza un teletransporte
			if ((p->id_jugador == 0 && j->tablero[j->tablero_jugador[j->turno_actual]][j->posiciones[p->id_jugador].x][j->posiciones[p->id_jugador].y] == 'A') 
            || (p->id_jugador == 1 && j->tablero[j->tablero_jugador[j->turno_actual]][j->posiciones[p->id_jugador].x][j->posiciones[p->id_jugador].y] == 'S')
            || (p->id_jugador == 2 && j->tablero[j->tablero_jugador[j->turno_actual]][j->posiciones[p->id_jugador].x][j->posiciones[p->id_jugador].y] == 'D')
            || (p->id_jugador == 3 && j->tablero[j->tablero_jugador[j->turno_actual]][j->posiciones[p->id_jugador].x][j->posiciones[p->id_jugador].y] == 'F') ){ // Teletransporte
                printf("Te puedes teletransportar a otra casilla con la misma letra\n");
                // Si es que no se puede tpear, retorna 0 y vuelve al inicio
                if (teletransporte(j,p->id_jugador) == 0){
                    j->posiciones[p->id_jugador].x = x_inicial;
                    j->casilla_anterior[j->turno_actual] = casilla_inicio;
                    if(casilla_inicio == 'P'){
                        j->tablero[tab_actual][x_inicial][y_inicial] = letra_jugador[p->id_jugador];
                    }
                    return;  	
                }
   	        }	
            
            // Ver si no hay camaras para obtener turnos
            if (j->tablero[j->tablero_jugador[j->turno_actual]][j->posiciones[p->id_jugador].x][j->posiciones[p->id_jugador].y] == 'T'){ // 5 turnos mas
                if (j->contador_camaras >= 2){
                    printf("Debes eliminar las camaras para poder obtener mas turnos\n");
                    j->posiciones[p->id_jugador].x = x_inicial;
                    j->casilla_anterior[j->turno_actual] = casilla_inicio;
                    if(casilla_inicio == 'P'){
                        j->tablero[tab_actual][x_inicial][y_inicial] = letra_jugador[p->id_jugador];
                    }
                    return;
                }
                else { 
                    j->rondas_totales = j->rondas_totales + 5;
                    printf("Jugadores ganan 5 turnos\n");
                }
            }

            if (casilla_inicio == '0' || casilla_inicio == '1' || casilla_inicio == '2' || casilla_inicio == '3' || casilla_inicio == '4' 
                || casilla_inicio == 'R' || casilla_inicio == 'T' || casilla_inicio == 'Y')
                j->tablero[tab_actual][x_inicial][y_inicial] = '0'; // Sacar de donde empezo, si es que no empezo en una P
             
            j->tablero[j->tablero_jugador[j->turno_actual]][j->posiciones[p->id_jugador].x][j->posiciones[p->id_jugador].y] = letra_jugador[p->id_jugador];  // Actualizar la nueva posición del jugador
        }
        
        else if (opcion_cantidad[0] == 3){  // Izquierda  

            int i;
            // Verificar si hay algo en el camino primero
            for (i = 1; i <= cant; i++) {
                char casilla_actual = j->tablero[tab_actual][x_actual][y_actual - i];

                // Se sale del mapa
                if (y_actual - i < 0 && j->casilla_anterior[j->turno_actual] != 'P'){
                    printf("Te estas saliendo del mapa, por favor ingrese una cantidad a moverse valida: , ingrese 0 para cancelar\n");
                    scanf("%d",&cant);
                    if (cant == 0){
                    	j->casilla_anterior[j->turno_actual] = casilla_inicio;                  	
                    	if(casilla_inicio == 'P'){
                        j->tablero[tab_actual][x_inicial][y_inicial] = letra_jugador[p->id_jugador];
                    	}
                        return;
                    }
                    opcion_cantidad[1] = cant - 1;
                    j->casilla_anterior[j->turno_actual] = casilla_inicio;
                    accion(p,opcion_cantidad,j);
                    return;
                }

                // Caso de pasar a otro laberinto
                if(y_actual - i < 0 && j->casilla_anterior[j->turno_actual] == 'P'){

                    int nuevo_laberinto;
                    int mov_restantes = cant - i;
                    int actual = tab_actual;

                    for(int z = 0; z < numero_tableros; z++){
                        if(j->posicion_tablero[z].x == j->posicion_tablero[actual].x 
                            && j->posicion_tablero[z].y == j->posicion_tablero[actual].y - 1){
                            nuevo_laberinto = z;
                            break;
                        }
                    }

                    int nuevo_laberinto_aux = nuevo_laberinto;
                    int mov_restantes_aux = mov_restantes + 1;

                    while(mov_restantes_aux > 0){
                        // Buscar laberinto que esta abajo
                        for(int z = 0; z < numero_tableros; z++){
                            if(j->posicion_tablero[z].x == j->posicion_tablero[actual].x 
                                && j->posicion_tablero[z].y == j->posicion_tablero[actual].y - 1){
                                nuevo_laberinto_aux = z;
                                break;
                            }
                        }
                        // ver si topa con el algo en el siguiente laberinto
                        int z;
                        for(z = 0; z <= 4; z++){
                            if(z == mov_restantes_aux){
                                break; 
                            }
                            char a = j->tablero[nuevo_laberinto_aux][2][4 - z];
                            if(a == 'Z' || a == 'X' || a == 'C' || a == 'V' || a == '/' || a == 'B' || a == 'E' 
                                || (p->id_jugador == 0 && (a == '2' || a == '3' || a == '4' || a == 'S' || a == 'D' || a == 'F'))
                                || (p->id_jugador == 1 && (a == '1' || a == '3' || a == '4' || a == 'A' || a == 'D' || a == 'F'))
                                || (p->id_jugador == 2 && (a == '1' || a == '2' || a == '4' || a == 'A' || a == 'S' || a == 'F'))
                                || (p->id_jugador == 3 && (a == '1' || a == '2' || a == '3' || a == 'A' || a == 'S' || a == 'D'))
                                ){
                                printf("Estas topando con una casilla que no puedes avanzar, ingresa una cantidad correcta de movimientos o ingresa 0 para cancelar\n");
                                scanf("%d",&cant);
                                if (cant == 0)
                                    return;
                                opcion_cantidad[1] = cant;
                                j->casilla_anterior[j->turno_actual] = casilla_inicio;
                                accion(p,opcion_cantidad,j);
                                return;
                            } 
                        }
                        mov_restantes_aux = mov_restantes_aux - z;
                        actual = nuevo_laberinto_aux;              
                    }

                    if (casilla_inicio == '0' || casilla_inicio == '1' || casilla_inicio == '2' || casilla_inicio == '3' || casilla_inicio == '4' 
                        || casilla_inicio == 'R' || casilla_inicio == 'T' || casilla_inicio == 'Y')
                        j->tablero[tab_actual][x_inicial][y_inicial] = '0'; // Sacar de donde empezo, si es que no empezo en una P
                    j->tablero_jugador[j->turno_actual] = nuevo_laberinto;  // Cambiar el laberinto que sigue
                    // Coordenadas que aparece en nuevo laberinto
                    j->posiciones[j->turno_actual].x = 2;  
                    j->posiciones[j->turno_actual].y = 4;
                    opcion_cantidad[1] = mov_restantes;                    
                    j->tablero[tab_actual][x_actual][y_actual - i + 1] = 'P';   // Restauro la P por donde sale del laberinto
                    j->casilla_anterior[j->turno_actual] = 'P';
                    accion(p,opcion_cantidad,j);
                    return;
                }

                // Comprobar si la casilla contiene un obstáculo o a otro jugador
                if (casilla_actual == 'Z' || casilla_actual == 'X' || casilla_actual == 'C' || casilla_actual == 'V' || 
                    casilla_actual == '/' || casilla_actual == 'B' || casilla_actual == 'E' 
                    || (p->id_jugador == 0 && (casilla_actual == '2' || casilla_actual == '3' || casilla_actual == '4' 
                    || casilla_actual == 'S' || casilla_actual == 'D' || casilla_actual == 'F'))
                    || (p->id_jugador == 1 && (casilla_actual == '1' || casilla_actual == '3' || casilla_actual == '4'
                    || casilla_actual == 'A' || casilla_actual == 'D' || casilla_actual == 'F'))
                    || (p->id_jugador == 2 && (casilla_actual == '1' || casilla_actual == '2' || casilla_actual == '4'
                    || casilla_actual == 'A' || casilla_actual == 'S' || casilla_actual == 'F'))
                    || (p->id_jugador == 3 && (casilla_actual == '1' || casilla_actual == '2' || casilla_actual == '3'
                    || casilla_actual == 'A' || casilla_actual == 'S' || casilla_actual == 'D'))
                    ){
                    printf("Estas topando con una casilla que no puedes avanzar, ingresa una cantidad correcta de movimientos o ingresa 0 para cancelar\n");
                    scanf("%d",&cant);
                    if (cant == 0)
                        return;
                    opcion_cantidad[1] = cant;
                    j->casilla_anterior[j->turno_actual] = casilla_inicio;
                    accion(p,opcion_cantidad,j);
                    return;
                }

                if (j->casilla_anterior[j->turno_actual] == 'P'){
                    j->tablero[tab_actual][x_actual][y_actual - i + 1] = 'P';
                }
                if (j->casilla_anterior[j->turno_actual] == 'A')
                    j->tablero[tab_actual][x_actual][y_actual - i + 1] = 'A';
                if (j->casilla_anterior[j->turno_actual] == 'S')
                    j->tablero[tab_actual][x_actual][y_actual - i + 1] = 'S';
                if (j->casilla_anterior[j->turno_actual] == 'D')
                    j->tablero[tab_actual][x_actual][y_actual - i + 1] = 'D';
                if (j->casilla_anterior[j->turno_actual] == 'F')
                    j->tablero[tab_actual][x_actual][y_actual - i + 1] = 'F';

                j->casilla_anterior[j->turno_actual] = casilla_actual;
            }

            int movido = i - 1;
            j->posiciones[p->id_jugador].y = y_inicial - movido;
            
            casilla_final(j,p);
            
            // Ver si realiza un teletransporte
			if ((p->id_jugador == 0 && j->tablero[j->tablero_jugador[j->turno_actual]][j->posiciones[p->id_jugador].x][j->posiciones[p->id_jugador].y] == 'A') 
            || (p->id_jugador == 1 && j->tablero[j->tablero_jugador[j->turno_actual]][j->posiciones[p->id_jugador].x][j->posiciones[p->id_jugador].y] == 'S')
            || (p->id_jugador == 2 && j->tablero[j->tablero_jugador[j->turno_actual]][j->posiciones[p->id_jugador].x][j->posiciones[p->id_jugador].y] == 'D')
            || (p->id_jugador == 3 && j->tablero[j->tablero_jugador[j->turno_actual]][j->posiciones[p->id_jugador].x][j->posiciones[p->id_jugador].y] == 'F') ){ // Teletransporte
                printf("Te puedes teletransportar a otra casilla con la misma letra\n");
                // Si es que no se puede tpear, retorna 0 y vuelve al inicio
                if (teletransporte(j,p->id_jugador) == 0){
                    j->posiciones[p->id_jugador].y = y_inicial;
                    j->casilla_anterior[j->turno_actual] = casilla_inicio;
                    if(casilla_inicio == 'P'){
                        j->tablero[tab_actual][x_inicial][y_inicial] = letra_jugador[p->id_jugador];
                    }
                    return;  	
                }
   	        }	

            // Ver si no hay camaras para obtener turnos
            if (j->tablero[j->tablero_jugador[j->turno_actual]][j->posiciones[p->id_jugador].x][j->posiciones[p->id_jugador].y] == 'T'){ // 5 turnos mas
                if (j->contador_camaras >= 2){
                    printf("Debes eliminar las camaras para poder obtener mas turnos\n");
                    j->posiciones[p->id_jugador].y = y_inicial;
                    j->casilla_anterior[j->turno_actual] = casilla_inicio;
                    if(casilla_inicio == 'P'){
                        j->tablero[tab_actual][x_inicial][y_inicial] = letra_jugador[p->id_jugador];
                    }
                    return;
                }
                else { 
                    j->rondas_totales = j->rondas_totales + 5;
                    printf("Jugadores ganan 5 turnos\n");
                }
            }

            if (casilla_inicio == '0' || casilla_inicio == '1' || casilla_inicio == '2' || casilla_inicio == '3' || casilla_inicio == '4' 
                || casilla_inicio == 'R' || casilla_inicio == 'T' || casilla_inicio == 'Y')
                j->tablero[tab_actual][x_inicial][y_inicial] = '0'; // Sacar de donde empezo, si es que no empezo en una P


            j->tablero[j->tablero_jugador[j->turno_actual]][j->posiciones[p->id_jugador].x][j->posiciones[p->id_jugador].y] = letra_jugador[p->id_jugador];  // Actualizar la nueva posición del jugador
        }
        else if (opcion_cantidad[0] == 2){  // Derecha

            int i;
            // Verificar si hay algo en el camino primero
            for (i = 1; i <= cant; i++) {
                char casilla_actual = j->tablero[tab_actual][x_actual][y_actual + i];
                
                // Se sale del mapa
                if (y_actual + i > 4 && j->casilla_anterior[j->turno_actual] != 'P'){
                    printf("Te estas saliendo del mapa, por favor ingrese una cantidad a moverse valida: , ingrese 0 para cancelar\n");
                    scanf("%d",&cant);
                    if (cant == 0){
                    	j->casilla_anterior[j->turno_actual] = casilla_inicio;                  	
                    	if(casilla_inicio == 'P'){
                        j->tablero[tab_actual][x_inicial][y_inicial] = letra_jugador[p->id_jugador];
                    	}
                        return;
                    }
                     opcion_cantidad[1] = cant - 1;
                    j->casilla_anterior[j->turno_actual] = casilla_inicio;
                    accion(p,opcion_cantidad,j);
                    return;
                }

                // Caso de pasar a otro laberinto
                if(y_actual + i > 4 && j->casilla_anterior[j->turno_actual] == 'P'){
                    
                    int nuevo_laberinto;
                    int mov_restantes = cant - i;
                    int actual = tab_actual;

                    for(int z = 0; z < numero_tableros; z++){
                        if(j->posicion_tablero[z].x == j->posicion_tablero[actual].x
                            && j->posicion_tablero[z].y == j->posicion_tablero[actual].y + 1){
                            nuevo_laberinto = z;
                            break;
                        }
                    }

                    int nuevo_laberinto_aux = nuevo_laberinto;
                    int mov_restantes_aux = mov_restantes + 1;

                    while(mov_restantes_aux > 0){
                        // Buscar laberinto que esta abajo
                        for(int z = 0; z < numero_tableros; z++){
                            if(j->posicion_tablero[z].x == j->posicion_tablero[actual].x
                                && j->posicion_tablero[z].y == j->posicion_tablero[actual].y + 1){
                                nuevo_laberinto_aux = z;
                                break;
                            }
                        }
                        // ver si topa con el algo en el siguiente laberinto
                        int z;
                        for(z = 0; z <= 4; z++){
                            if(z == mov_restantes_aux){
                                break; 
                            }
                            char a = j->tablero[nuevo_laberinto_aux][2][0 + z];
                            if(a == 'Z' || a == 'X' || a == 'C' || a == 'V' || a == '/' || a == 'B' || a == 'E' 
                                || (p->id_jugador == 0 && (a == '2' || a == '3' || a == '4' || a == 'S' || a == 'D' || a == 'F'))
                                || (p->id_jugador == 1 && (a == '1' || a == '3' || a == '4' || a == 'A' || a == 'D' || a == 'F'))
                                || (p->id_jugador == 2 && (a == '1' || a == '2' || a == '4' || a == 'A' || a == 'S' || a == 'F'))
                                || (p->id_jugador == 3 && (a == '1' || a == '2' || a == '3' || a == 'A' || a == 'S' || a == 'D'))
                                ){
                                printf("Estas topando con una casilla que no puedes avanzar, ingresa una cantidad correcta de movimientos o ingresa 0 para cancelar\n");
                                scanf("%d",&cant);
                                if (cant == 0)
                                    return;
                                opcion_cantidad[1] = cant;
                                j->casilla_anterior[j->turno_actual] = casilla_inicio;
                                accion(p,opcion_cantidad,j);
                                return;
                            } 
                        }
                        mov_restantes_aux = mov_restantes_aux - z;
                        actual = nuevo_laberinto_aux;              
                    }
         
                    if (casilla_inicio == '0' || casilla_inicio == '1' || casilla_inicio == '2' || casilla_inicio == '3' || casilla_inicio == '4' 
                        || casilla_inicio == 'R' || casilla_inicio == 'T' || casilla_inicio == 'Y')
                        j->tablero[tab_actual][x_inicial][y_inicial] = '0'; // Sacar de donde empezo, si es que no empezo en una P
                    j->tablero_jugador[j->turno_actual] = nuevo_laberinto;  // Cambiar el laberinto que sigue
                    // Coordenadas que aparece en nuevo laberinto
                    j->posiciones[j->turno_actual].x = 2;  
                    j->posiciones[j->turno_actual].y = 0;
                    opcion_cantidad[1] = mov_restantes;                    
                    j->tablero[tab_actual][x_actual][y_actual + i - 1] = 'P';   // Restauro la P por donde sale del laberinto
                    j->casilla_anterior[j->turno_actual] = 'P';
                    accion(p,opcion_cantidad,j);
                    return;
                }

                // Comprobar si la casilla contiene un obstáculo o a otro jugador
                if (casilla_actual == 'Z' || casilla_actual == 'X' || casilla_actual == 'C' || casilla_actual == 'V' || 
                    casilla_actual == '/' || casilla_actual == 'B' || casilla_actual == 'E' 
                    || (p->id_jugador == 0 && (casilla_actual == '2' || casilla_actual == '3' || casilla_actual == '4' 
                    || casilla_actual == 'S' || casilla_actual == 'D' || casilla_actual == 'F'))
                    || (p->id_jugador == 1 && (casilla_actual == '1' || casilla_actual == '3' || casilla_actual == '4'
                    || casilla_actual == 'A' || casilla_actual == 'D' || casilla_actual == 'F'))
                    || (p->id_jugador == 2 && (casilla_actual == '1' || casilla_actual == '2' || casilla_actual == '4'
                    || casilla_actual == 'A' || casilla_actual == 'S' || casilla_actual == 'F'))
                    || (p->id_jugador == 3 && (casilla_actual == '1' || casilla_actual == '2' || casilla_actual == '3'
                    || casilla_actual == 'A' || casilla_actual == 'S' || casilla_actual == 'D'))
                    ){
                    printf("Estas topando con una casilla que no puedes avanzar, ingresa una cantidad correcta de movimientos o ingresa 0 para cancelar\n");
                    scanf("%d",&cant);
                    if (cant == 0)
                        return;
                    opcion_cantidad[1] = cant;
                    j->casilla_anterior[j->turno_actual] = casilla_inicio;
                    accion(p,opcion_cantidad,j);
                    return;
                }

                if (j->casilla_anterior[j->turno_actual] == 'P'){
                    j->tablero[tab_actual][x_actual][y_actual + i - 1] = 'P';
                }

                if (j->casilla_anterior[j->turno_actual] == 'A')
                    j->tablero[tab_actual][x_actual][y_actual - i + 1] = 'A';
                if (j->casilla_anterior[j->turno_actual] == 'S')
                    j->tablero[tab_actual][x_actual][y_actual - i + 1] = 'S';
                if (j->casilla_anterior[j->turno_actual] == 'D')
                    j->tablero[tab_actual][x_actual][y_actual - i + 1] = 'D';
                if (j->casilla_anterior[j->turno_actual] == 'F')
                    j->tablero[tab_actual][x_actual][y_actual - i + 1] = 'F';

        
                j->casilla_anterior[j->turno_actual] = casilla_actual;
            }

            int movido = i - 1;
            j->posiciones[p->id_jugador].y = y_inicial + movido;
            
            casilla_final(j,p);
            
            // Ver si realiza un teletransporte
			if ((p->id_jugador == 0 && j->tablero[j->tablero_jugador[j->turno_actual]][j->posiciones[p->id_jugador].x][j->posiciones[p->id_jugador].y] == 'A') 
            || (p->id_jugador == 1 && j->tablero[j->tablero_jugador[j->turno_actual]][j->posiciones[p->id_jugador].x][j->posiciones[p->id_jugador].y] == 'S')
            || (p->id_jugador == 2 && j->tablero[j->tablero_jugador[j->turno_actual]][j->posiciones[p->id_jugador].x][j->posiciones[p->id_jugador].y] == 'D')
            || (p->id_jugador == 3 && j->tablero[j->tablero_jugador[j->turno_actual]][j->posiciones[p->id_jugador].x][j->posiciones[p->id_jugador].y] == 'F') ){ // Teletransporte
                printf("Te puedes teletransportar a otra casilla con la misma letra\n");
                // Si es que no se puede tpear, retorna 0 y vuelve al inicio
                if (teletransporte(j,p->id_jugador) == 0){
                    j->posiciones[p->id_jugador].y = y_inicial;
                    j->casilla_anterior[j->turno_actual] = casilla_inicio;
                    if(casilla_inicio == 'P'){
                        j->tablero[tab_actual][x_inicial][y_inicial] = letra_jugador[p->id_jugador];
                    }
                    return;  	
                }
   	        }

            // Ver si no hay camaras para obtener turnos
            if (j->tablero[j->tablero_jugador[j->turno_actual]][j->posiciones[p->id_jugador].x][j->posiciones[p->id_jugador].y] == 'T'){ // 5 turnos mas
                if (j->contador_camaras >= 2){
                    printf("Debes eliminar las camaras para poder obtener mas turnos\n");
                    j->posiciones[p->id_jugador].y = y_inicial;
                    j->casilla_anterior[j->turno_actual] = casilla_inicio;
                    if(casilla_inicio == 'P'){
                        j->tablero[tab_actual][x_inicial][y_inicial] = letra_jugador[p->id_jugador];
                    }
                    return;
                }
                else { 
                    j->rondas_totales = j->rondas_totales + 5;
                    printf("Jugadores ganan 5 turnos\n");
                }
            }

            if (casilla_inicio == '0' || casilla_inicio == '1' || casilla_inicio == '2' || casilla_inicio == '3' || casilla_inicio == '4' 
                || casilla_inicio == 'R' || casilla_inicio == 'T' || casilla_inicio == 'Y')
                j->tablero[tab_actual][x_inicial][y_inicial] = '0'; // Sacar de donde empezo, si es que no empezo en una P
                    
            j->tablero[j->tablero_jugador[j->turno_actual]][j->posiciones[p->id_jugador].x][j->posiciones[p->id_jugador].y] = letra_jugador[p->id_jugador];  // Actualizar la nueva posición del jugador
        }
    }    
}

void crear_jugador(Jugador* p, int id_jugador){
    pipe(p->hacia_jugador);
    pipe(p->desde_jugador);

    p->pid = fork();
    p->id_jugador = id_jugador;
    p->carta = mazo[id_jugador];
   
    if(p->pid == -1){
        printf("Error al realizar fork\n");
        exit(1);
    }
    // Padre es quien recibe y envia informacion entre J1 y jugadores bots
    if(p->pid == 0){
        close(p->hacia_jugador[WRITE]); // Hijo quiere enviar hacia padre y leer de padre
        close(p->desde_jugador[READ]);  // 
    }
    else{  // Proceso padre
        close(p->hacia_jugador[READ]);  // Padre quiere enviar hacia hijo y leer de hijo
        close(p->desde_jugador[WRITE]); //  
    }
}

