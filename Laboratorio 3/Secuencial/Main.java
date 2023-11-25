import java.io.File;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.Scanner;

public class Main {

    // Funcion buscar horizontal
    public static int buscar_horizontal(char[][] matriz, int tam, String palabra_buscar) {
        int longitud = palabra_buscar.length();
        int k = 0;

        for (int i = 0; i < tam; i++) {
            for (int j = 0; j < tam; j++) {
                if (matriz[i][j] == palabra_buscar.charAt(k)) {
                    k++;
                } else {
                    k = 0;
                }

                if (k == longitud) {
                    System.out.printf("fila %d columna %d (horizontal)\n", i + 1, j - k + 2);
                    return 1;     
                }
            }
        }
        return 0;
    }

    // Funcion buscar vertical
    public static int buscar_vertical(char[][] matriz, int tam, String palabra_buscar) {
        int longitud = palabra_buscar.length();
        int k = 0;
    
        for (int j = 0; j < tam; j++) {
            for (int i = 0; i < tam; i++) {
                if (matriz[i][j] == palabra_buscar.charAt(k)) {
                    k++;
                } else {
                    k = 0;
                }
    
                if (k == longitud) {
                    System.out.printf("fila %d columna %d (vertical)\n", i - k + 2,  j + 1);
                    return 1;     
                }
            }
        }
        return 0;
    }   

    public static void main(String[] args) {

        Path pathToSopas = Paths.get("../Sopas").toAbsolutePath();
        // Convierte path a un objeto file
        File arch = pathToSopas.toFile();

        // Carpetas
        File[] files = arch.listFiles();
        // Subcarpetas
        for (File file : files) {
            // Si es una subcarpeta, busca el archivo sopa_de_letras.txt
            File sopaDeLetras = new File(file, "sopa_de_letras.txt");
            // Leer sopa_de_letras
            try (Scanner scanner = new Scanner(sopaDeLetras)) {
                int tam = scanner.nextInt(); // Lee el entero de la primera línea
                scanner.nextLine(); // Consumir el resto de la línea
                String palabra = scanner.nextLine(); // Lee la segunda línea

                char[][] sopa = new char[tam][tam];

                // Leer la sopa de letras línea por línea
                for (int i = 0; i < tam; i++) {
                    if (scanner.hasNextLine()) {
                        String line = scanner.nextLine();
                        String[] letras = line.strip().split(" "); // Separa las letras por espacios
                        for (int j = 0; j < tam; j++) {
                            sopa[i][j] = letras[j].charAt(0);
                        }
                    }
                }

                System.out.printf("Palabra: %s ",palabra);

                long t = System.currentTimeMillis();
                if(buscar_horizontal(sopa, tam, palabra.toUpperCase()) == 1){
                    long tiempoFin = System.currentTimeMillis();
                    System.out.println("Tiempo de ejecución: " + (tiempoFin - t) + " ms");
                }
                else {
                    buscar_vertical(sopa, tam, palabra.toUpperCase());
                    long tiempoFin = System.currentTimeMillis();
                    System.out.println("Tiempo de ejecución: " + (tiempoFin - t) + " ms");
                }

            } catch (Exception e) {
                System.out.println("Error al leer el archivo: " + e.getMessage());
            }
        }
    }
}

