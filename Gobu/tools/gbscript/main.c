#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main() {
    char* input[] = {"let suma = 2", "let name = 'Gobu'", "let average = 2.5"};
    char output[50];

    for(int i = 0; i < 3; i++) {
        char modifiableInput[50];
        strcpy(modifiableInput, input[i]);

        // Split the input into words
        char* keyword = strtok(modifiableInput, " ");
        char* variable = strtok(NULL, " ");
        char* equals = strtok(NULL, " ");
        char* value = strtok(NULL, " ");

        // Determine the data type based on the value
        if (strchr(value, '.') != NULL) {
            keyword = "float";
        } else if (isdigit(value[0])) {
            keyword = "int";
        } else if (value[0] == '\'') {
            keyword = "char*";
        }

        // Ensure the line ends with a semicolon
        strcat(value, ";");

        // Combine the words into the output
        sprintf(output, "%s %s %s %s", keyword, variable, equals, value);

        printf("%s\n", output);
    }

    return 0;
}