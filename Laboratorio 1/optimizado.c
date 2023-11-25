#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
//#include <unistd.h>
#include <dirent.h> 
#include <string.h>
#include <ctype.h>
#include <time.h>

/*
Función que recibe un string y lo convierte todo a mayusculas
Parametros:
            palabra (str): string a convertir a mayusculas
*/

void convertir_mayusculas(char palabra[]) {
    int i = 0;
    while (palabra[i] != '\0') {
        palabra[i] = toupper(palabra[i]);
        i++;
    }
}

/*
Función que recibe un string y le remueve la extension .txt del final.
Parametros:
            nombre_archivo (str): string a remover extension
*/

char* quitar_extension(char *nombre_archivo) {
    char* copia = strdup(nombre_archivo); // Se duplica la cadena para evitar modificar la original
    
    copia[strlen(copia) - 4] = '\0';

    return copia; // Retornar la nueva cadena sin la extensión

}

/*
Función que busca "palabra_buscar" en la matriz que contiene la sopa de letras, esto se realiza recorriendo la matriz y comparando letra por letra de 
la palabra a buscar mediante un contador, cuando el contador es igual a la longitud de la palabra, significa que fue localizada. En el caso de que
la letra en la sopa de la matriz no sea igual se va reiniciando el contador.
Parametros:
            matriz (matriz 2x2): corresponde a la matriz de 2x2 que contiene la sopa de letras del archivo
            filas (int): numero de filas de la matriz
            columnas (int): numero de columnas de la matriz
            palabra_buscar (str): palabra a buscar en la matriz, esta se encuentra toda en mayusculas
            horizontal (int): binario que cuando es 1 significa que se realiza busqueda de acorde a orientacion horizontal y cuando
                              es 0 significa que se realiza la busqueda debido a una orientacion vertical, esto sirve solamente para imprimir
                              por pantalla donde se encuentra la palabra
*/

void buscar_horizontal(char matriz[200][200], int filas, int columnas, char palabra_buscar[256], int horizontal) {
    int longitud = strlen(palabra_buscar);
    int k = 0;
    for (int i = 0; i < filas; i++) {
        for (int j = 0; j < columnas; j++){
            if ( matriz[i][j] == palabra_buscar[k] ){
                //printf("sopa: %c\n",matriz[i][j]);
                //printf("palabra: %c\n",palabra_buscar[k]);
                k++;
            }
            else
                k = 0;                
            if (k == longitud){
                if (horizontal)
                    printf("Palabra encontrada termina en fila: %d, columna: %d, de forma horizontal\n",i+1,j+1);
                else
                    printf("Palabra encontrada termina en fila: %d, columna: %d, de forma vertical\n",j+1,i+1);
                return;
            }
        }
    }
}

/*
Función que realiza la transpuesta de la matriz, para luego realizar la busqueda de la palabra haciendo uso de la funcion "buscar_horizontal"
Parametros:
            matriz (matriz 2x2): corresponde a la matriz de 2x2 que contiene la sopa de letras del archivo
            filas (int): numero de filas de la matriz
            columnas (int): numero de columnas de la matriz
            palabra_buscar (str): palabra a buscar en la matriz, esta se encuentra toda en mayusculas
            horizontal (int): binario que cuando es 1 significa que se realiza busqueda de acorde a orientacion horizontal y cuando
                              es 0 significa que se realiza la busqueda debido a una orientacion vertical, esto sirve solamente para imprimir
                              por pantalla donde se encuentra la palabra
*/


void buscar_vertical(char matriz[200][200], int filas, int columnas, char palabra_buscar[256], int horizontal){
    char transpuesta[200][200];
    // Primero, hacemos la matriz transpuesta
    for (int i = 0; i < filas; i++){
        for (int j = 0; j < columnas; j++){
            transpuesta[i][j] = matriz[j][i];
        }        
    }
    // Para imprimir matriz transpuesta
    /*
    for (int i = 0; i < filas; i++){
        for (int j = 0; j < columnas; j++){
            printf("%3c ",transpuesta[i][j]);
        }
        printf("\n");       
    }
    */
    buscar_horizontal(transpuesta,filas,columnas,palabra_buscar,horizontal);
}


int main(){

    DIR *carpeta;   // Carpeta con los txt
    struct dirent *leido;   // Para leer cada txt
    char sopa[200][200];
    char path[1024];    // Para eliminar .txt

    mkdir("Vertical", 0777);
    mkdir("Horizontal", 0777);

    // Intenta leer la carpeta que contiene los juegos
    // Si es nulo, se finaliza la ejecucion
    if ((carpeta = opendir("Archivos")) == NULL)
        perror("No se pudo leer la carpeta\n");
        
    else {
        // Proceso cada archivo
        while ((leido = readdir(carpeta)) != NULL) {
            
            char orientacion[15];

            if (strcmp(leido->d_name, ".") != 0 && strcmp(leido->d_name, "..") != 0){

                char archivo_carpeta[] = "Archivos/";   // Directorio con archivos txt
                strcat(archivo_carpeta,leido->d_name);  // Para leer cada txt
    
                FILE *fp = fopen(archivo_carpeta, "r"); // Se abre cada archivo txt

                fgets(orientacion, sizeof(orientacion), fp); // Lee primera linea
                orientacion[strcspn(orientacion, "\r\n")] = 0; // Remueve \r\n del final de primera linea
                
                int filas = 0;
                int columnas = 0;
                char a;

                while((a = fgetc(fp)) != EOF){ // Contar filas y columnas
                    sopa[filas][columnas] = a;  // Agregar a la sopa las letras
                    if ((a != '\n') && (a != ' '))
                        columnas++;                    
                    else if (a == '\n'){
                        filas++;
                        columnas = 0;   // Se reinicia al inicio de cada fila
                    }
                }
                filas++;

                // Se remueve .txt del final del archivo para obtener la palabra a buscar
                char *nombre_archivo = quitar_extension(leido->d_name);
                convertir_mayusculas(nombre_archivo);

                printf("Nombre Archivo: %s | Orientacion: %s | Filas: %d | Columnas: %d\n",nombre_archivo,orientacion,filas,columnas);
                // Nombre directorio corresponde a donde se guarda la palabra de acorde a orientacion y dimension
                char nombre_directorio[256];

                clock_t tiempo_inicio, tiempo_termino;
                double tiempo_ejecucion;

                // Dependiendo si la orientacion es vertical o horizontal, se busca la palabra en la sopa de letras
                tiempo_inicio = clock();
                if ((strcmp(orientacion, "vertical") == 0)){
                    buscar_vertical(sopa,filas,columnas,nombre_archivo,0);
                    strcpy(nombre_directorio,"Vertical/");
                }
                else {
                    buscar_horizontal(sopa,filas,columnas,nombre_archivo,1);
                    strcpy(nombre_directorio,"Horizontal/");
                }

                tiempo_termino = clock();

                tiempo_ejecucion = ((double) (tiempo_termino - tiempo_inicio)) / CLOCKS_PER_SEC;

                printf("Tiempo de ejecución para el archivo: %s -> %f segundos\n", leido->d_name, tiempo_ejecucion);
                printf("----------------------------------------------------------------------\n");

                free(nombre_archivo);
                char filas_dir[20];
                char columnas_dir[20];
                // Transformar filas y columnas a string
                sprintf(filas_dir, "%d", filas);
                sprintf(columnas_dir, "%d", columnas);

                strcat(filas_dir,"x"); // Filasx
                strcat(filas_dir,columnas_dir); // FilasxColumnas
                strcat(nombre_directorio,filas_dir);    // nombre_directorio: Vertical/FilasxColumnas 

                DIR *obj;
                obj = opendir(nombre_directorio);
                if (obj == NULL){
                    // Carpeta no ha sido creada todaviaS
                    mkdir(nombre_directorio, 0777);
                }
                closedir(obj);

                rewind(fp);
                FILE *copia;

                strcat(nombre_directorio,"/");
                strcat(nombre_directorio,leido->d_name);
                copia = fopen(nombre_directorio, "w"); // nombre_directorio: Vertical/FilasxColumnas/nombre.txt

                char linea[410];
                while (fgets(linea, sizeof(linea), fp) != NULL) {
                    fputs(linea, copia);
                }

                fclose(copia);
                fclose(fp);

                // Se eliminan los .txt de la carpeta Archivos
                snprintf(path, sizeof(path), "Archivos/%s", leido->d_name);
                remove(path);
            }
        }
    }       
    closedir(carpeta);

    return 0;
}