//Alejandro Cifuente Escobar 	cc:1152715413
//John Haider Giraldo		cc:1020436600

#include <stdio.h>      // Biblioteca estándar para manejo de entrada/salida
#include <stdlib.h>     // Biblioteca estándar para funciones de memoria dinámica
#include <string.h>     // Biblioteca para manejo de cadenas
#include <sys/stat.h>   // Biblioteca para manejar la estructura stat y obtener info de archivos

// Función para verificar si dos archivos son el mismo
int verificar_archivos(const char *file1, const char *file2) {
    struct stat stat1, stat2;   // Estructuras para almacenar información de archivos

    // Obtener información del primer archivo
    if (stat(file1, &stat1) != 0) {  // Si falla al obtener la información
        exit(1);   // Termina el programa
    }

    // Obtener información del segundo archivo
    if (stat(file2, &stat2) != 0) {  // Si falla al obtener la información
        exit(1);   // Termina el programa
    }

    // Comparar el número de dispositivo e inodo
    return (stat1.st_dev == stat2.st_dev && stat1.st_ino == stat2.st_ino); // Devuelve si son iguales
}

// Estructura para almacenar una línea
typedef struct LineNode {
    char *line;         // Almacena el contenido de la línea
    struct LineNode *next; // Puntero al siguiente nodo de la lista
} LineNode;

// Función para leer líneas de longitud indefinida
ssize_t leer_linea(char **lineptr, size_t *n, FILE *stream) {

    if (*lineptr == NULL || *n == 0) {  // Si no se ha asignado buffer
        *n = 128;  // Establece un tamaño inicial del buffer
        *lineptr = malloc(*n);  // Asigna memoria para el buffer

        if (*lineptr == NULL) {  // Verifica si falla la asignación
            fprintf(stderr, "malloc failed\n");  // Error si malloc falla
            exit(1);   // Termina si hay error
        }
    }

    ssize_t num_chars = getline(lineptr, n, stream); // Lee una línea del archivo

    if (num_chars == -1) {   // Si hay un error o fin de archivo
        return -1;  // Devuelve -1 indicando el final o error
    }

    return num_chars;  // Devuelve el número de caracteres leídos
}

// Función para agregar una línea a la lista enlazada
LineNode* agregar_linea(LineNode *head, char *line) {
    LineNode *new_node = (LineNode *)malloc(sizeof(LineNode)); // Reservar memoria para un nuevo nodo

    if (new_node == NULL) {     // Verifica si falla la reserva de memoria
        fprintf(stderr, "malloc failed\n");  // Mensaje de error si falla malloc
        exit(1);              // Finaliza el programa si hay error
    }

    new_node->line = strdup(line); // Copia la línea en el nuevo nodo

    if (new_node->line == NULL) {   // Verifica si falla la copia de la línea
        fprintf(stderr, "malloc failed\n");  // Mensaje de error si falla strdup
        exit(1);              // Finaliza el programa si hay error
    }

    new_node->next = head;     // El nuevo nodo apunta al antiguo cabeza de la lista
    return new_node;           // Devuelve el nuevo nodo como cabeza de la lista
}

// Función para liberar la memoria de la lista enlazada
void liberar_memoria(LineNode *head) {
    LineNode *current = head;  // Inicia desde el nodo cabeza

    while (current != NULL) {  // Recorre todos los nodos de la lista
        LineNode *temp = current;  // Almacena el nodo actual
        current = current->next;   // Avanza al siguiente nodo
        free(temp->line);          // Libera la memoria de la línea
        free(temp);                // Libera el nodo actual
    }
}

// Función principal
int main(int argc, char *argv[]) {

    //establecer archivos
    FILE *input_file = stdin;  // Establece el archivo de entrada estándar (teclado)
    FILE *output_file = stdout;  // Establece el archivo de salida estándar (pantalla)

    //establecer lugares de almacenamiento de datos
    LineNode *lines = NULL;    // Inicializa la lista de líneas como vacía
    char *buffer = NULL;       // Inicializa el buffer para las líneas
    size_t buffer_size = 0;    // Tamaño inicial del buffer

    // Manejo de argumentos
    if (argc > 3) {  // Si se pasan más de 3 argumentos
        fprintf(stderr, "usage: reverse <input> <output>\n"); // Mensaje de uso incorrecto
        exit(1);   // Termina el programa
    }

    if (argc == 2) {  // Si se pasa solo un argumento (archivo de entrada)
        input_file = fopen(argv[1], "r");  // Abre el archivo de entrada

        if (input_file == NULL) {  // Si falla al abrir el archivo
            fprintf(stderr, "reverse: cannot open file '%s'\n", argv[1]);  // Mensaje de error
            exit(1);  // Termina el programa
        }
    }

    if (argc == 3) {  // Si se pasan dos argumentos (archivo de entrada y salida)
        input_file = fopen(argv[1], "r");  // Abre el archivo de entrada
        output_file = fopen(argv[2], "w"); // Abre el archivo de salida

        if (input_file == NULL) {  // Si falla al abrir el archivo de entrada
            fprintf(stderr, "reverse: cannot open file '%s'\n", argv[1]);  // Mensaje de error
            exit(1);   // Termina el programa
        }

        if (output_file == NULL) {  // Si falla al abrir el archivo de salida
            fprintf(stderr, "reverse: cannot open file '%s'\n", argv[2]);  // Mensaje de error
            exit(1);   // Termina el programa
        }

        if (strcmp(argv[1], argv[2]) == 0 || verificar_archivos(argv[1], argv[2])) { // Verifica si los archivos son iguales
            fprintf(stderr, "reverse: input and output file must differ\n");  // Mensaje de error si son iguales
            exit(1);   // Termina el programa
        }
    }

    // Leer el archivo de entrada línea por línea y agregar a la lista
    while (leer_linea(&buffer, &buffer_size, input_file) != -1) {  // Mientras haya líneas para leer
        lines = agregar_linea(lines, buffer);  // Agrega cada línea a la lista
    }

    // Imprimir las líneas en orden inverso
    LineNode *current = lines;  // Empieza en la cabeza de la lista

    while (current != NULL) {   // Recorre cada nodo de la lista
        fprintf(output_file, "%s", current->line);  // Imprime la línea actual
        current = current->next;  // Avanza al siguiente nodo
    }

    // Limpiar
    liberar_memoria(lines);  // Libera la memoria de la lista
    free(buffer);       // Libera el buffer

    if (input_file != stdin) fclose(input_file);   // Cierra el archivo de entrada si no es stdin
    if (output_file != stdout) fclose(output_file); // Cierra el archivo de salida si no es stdout

    return 0;  // Fin del programa
}

