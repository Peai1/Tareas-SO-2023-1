import java.io.File;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.Scanner;
import java.util.concurrent.ForkJoinPool;
import java.util.concurrent.RecursiveAction;

public class Main {

    public static class busquedaSopa extends RecursiveAction {
        private int tam;
        private String palabra;
        private char[][] matriz;
        private int x;
        private int y;
        private long tiempo_inicio;

        public busquedaSopa(int tam, char[][] matriz, String palabra, int x, int y, long tiempo_inicio) {
            this.tam = tam;
            this.palabra = palabra;
            this.matriz = matriz;
            this.x = x;
            this.y = y;
            this.tiempo_inicio = tiempo_inicio;
        }

        public long getTiempo_inicio(){
            return this.tiempo_inicio;
        }

        @Override
        protected void compute() {

            if (palabra.length() == tam) {

                // buscar horizontal
                if (buscar_horizontal(matriz, palabra.length(), palabra.toUpperCase(), x, y) == 1) {
                    long tiempoFin = System.currentTimeMillis();
                    System.out.println("Tiempo de ejecución: " + (tiempoFin - this.getTiempo_inicio()) + " ms");
                    return;
                } else if (buscar_vertical(matriz, palabra.length(), palabra.toUpperCase(), x, y) == 1) {
                    long tiempoFin = System.currentTimeMillis();
                    System.out.println("Tiempo de ejecución: " + (tiempoFin - this.getTiempo_inicio()) + " ms");
                    return;
                }
                return;

            } else {
                int div = tam / 2;
                char m1[][] = new char[tam][tam];
                char m2[][] = new char[tam][tam];
                char m3[][] = new char[tam][tam];
                char m4[][] = new char[tam][tam];

                for (int i = 0; i < div; i = i + 1) {
                    for (int j = 0; j < div; j = j + 1) {
                        m1[i][j] = matriz[i][j];
                        m2[i][j] = matriz[i + div][j];
                        m3[i][j] = matriz[i][j + div];
                        m4[i][j] = matriz[i + div][j + div];
                    }
                }

                long t = getTiempo_inicio();

                busquedaSopa p1 = new busquedaSopa(div, m1, palabra, x, y, t);
                busquedaSopa p2 = new busquedaSopa(div, m2, palabra, x + div, y, t);
                busquedaSopa p3 = new busquedaSopa(div, m3, palabra, x, y + div, t);
                busquedaSopa p4 = new busquedaSopa(div, m4, palabra, x + div, y + div, t);

                invokeAll(p1, p2, p3, p4);
            }
        }

    }

    // Funcion buscar horizontal
    public static int buscar_horizontal(char[][] matriz, int tam, String palabra_buscar, int x, int y) {
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
                    System.out.printf("fila %d columna %d (horizontal)\n", x + i + 1, y + 1);
                    return 1;
                }
            }
        }
        return 0;
    }

    // Funcion buscar vertical
    public static int buscar_vertical(char[][] matriz, int tam, String palabra_buscar, int x, int y) {
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
                    System.out.printf("fila %d columna %d (vertical)\n", x + 1, y + j + 1);
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

                // Aquí puedes procesar el número y el string según tus necesidades
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
				
				System.out.printf("Palabra: %s ", palabra);
                ForkJoinPool pool = new ForkJoinPool();
                busquedaSopa tarea = new busquedaSopa(tam, sopa, palabra, 0, 0, System.currentTimeMillis());
                pool.invoke(tarea); // Invoca la tarea

            } catch (Exception e) {
                System.out.println("Error al leer el archivo: " + e.getMessage());
            }
        }
    }
}
