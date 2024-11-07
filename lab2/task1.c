#include <stdio.h>
#include <stdlib.h>

int main() {
    setenv("NEW_VARIABLE", "Hello, World!", 1);

    char *env_var = getenv("NEW_VARIABLE");
    if (env_var != NULL) {
        printf("NEW_VARIABLE: %s\n", env_var);
    } else {
        printf("NEW_VARIABLE не найдена в среде процесса\n");
    }

    return 0;
}
