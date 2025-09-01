#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *f = fopen("executor_test.txt", "w");
    if (f) {
        fprintf(f, "Direct executor test\n");
        fclose(f);
        printf("Created executor_test.txt successfully\n");
    } else {
        perror("Failed to create file");
    }
    return 0;
}
