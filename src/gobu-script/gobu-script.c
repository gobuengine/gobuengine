#include <stdio.h>
#include <string.h>

// Función para procesar líneas de código
void processCodeLines(const char* sourceCode, const char* typedefToken, const char* fnToken, char* structDefinitions, char* functionDefinitions) {
    char* line = strtok(sourceCode, "\n");
    while (line != NULL) {
        if (strstr(line, typedefToken) != NULL) {
            strcat(structDefinitions, line);
            strcat(structDefinitions, "\n");
        } else if (strstr(line, fnToken) == line) {
            strcat(functionDefinitions, line);
            strcat(functionDefinitions, "\n");
        }
        line = strtok(NULL, "\n");
    }
}

int main() {
    // Código fuente de ejemplo en una cadena
    char sourceCode[] = "typedef struct Person { char name[50]; int age; } Person;\n"
                        "typedef struct Student { char rollNumber[10]; float gpa; } Student;\n"
                        "fn void initPerson(struct Person* person) { /* ... */ }\n"
                        "fn int calculateSum(int a, int b) { /* ... */ }\n"
                        "int main() { /* ... */ }";

    // Token que deseamos encontrar
    const char* typedefToken = "typedef struct";
    const char* fnToken = "fn ";

    // Strings para almacenar las definiciones de structs y funciones
    char structDefinitions[4096] = "";
    char functionDefinitions[4096] = "";

    // Procesar el código fuente
    processCodeLines(sourceCode, typedefToken, fnToken, structDefinitions, functionDefinitions);

    // Imprime las definiciones de structs y funciones encontradas
    printf("Definiciones de Structs:\n%s\n", structDefinitions);
    printf("Definiciones de Funciones:\n%s\n", functionDefinitions);

    return 0;
}
