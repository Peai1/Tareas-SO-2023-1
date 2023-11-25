import java.io.File;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.Scanner;

public class Main {

    public static class Hebra extends Thread {
        private int tam;
        private String palabra;
        private char[][] matriz;
        private int x, y;
        private long tiempo_inicio;

        public Hebra(int tam, String palabra, char[][] matriz, int x, int y, long tiempo_inicio) {
            this.tam = tam;
            this.palabra = palabra;
            this.matriz = matriz;
            this.x = x;
            this.y = y;
            this.tiempo_inicio = tiempo_inicio;
        }

        public long getTiempo_inicio() {
            return this.tiempo_inicio;
        }

        @Override
        public void run() {
            
            if (palabra.length() == tam) {

                if (buscar_horizontal(matriz, palabra.length(), palabra.toUpperCase(), x, y) == 1) {
                    long tiempoFin = System.currentTimeMillis();
                    System.out.println("Tiempo de ejecución: " + (tiempoFin - this.getTiempo_inicio()) + " ms");
                    return;
                } else if (buscar_vertical(matriz, palabra.length(), palabra.toUpperCase(), x, y) == 1) {
                    long tiempoFin = System.currentTimeMillis();
                    System.out.println("Tiempo de ejecución: " + (tiempoFin - this.getTiempo_inicio()) + " ms");
                    return;
                }

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

                Hebra thread1 = new Hebra(div, palabra, m1, x, y, t);
                Hebra thread2 = new Hebra(div, palabra, m2, x + div, y, t);
                Hebra thread3 = new Hebra(div, palabra, m3, x, y + div, t);
                Hebra thread4 = new Hebra(div, palabra, m4, x + div, y + div, t);

                thread1.start();
                thread2.start();
                thread3.start();
                thread4.start();

                try {
                    thread1.join();
                    thread2.join();
                    thread3.join();
                    thread4.join();
                } catch (InterruptedException e) {
                }
            }
        }

        public int buscar_horizontal(char[][] matriz, int tam, String palabra_buscar, int x, int y) {
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
        public int buscar_vertical(char[][] matriz, int tam, String palabra_buscar, int x, int y) {
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
				
				System.out.printf("Palabra: %s ", palabra);
                Hebra thread1 = new Hebra(tam, palabra, sopa, 0, 0,System.currentTimeMillis());
                thread1.start();

                try {
                    thread1.join();
                } catch (InterruptedException e) {
                }

            } catch (Exception e) {
                System.out.println("Error al leer el archivo: " + e.getMessage());
            }
        }
    }
}
