#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>

#define BUF_SIZE 2048
#define PROC_FILE "/proc/proc_inf"
int main(int args, char **argv) {
	
	if (args != 2) {
		printf("Wrong number of arguments: %d. Provided: 1\n", args);
		printf("Debug: argv:\n");
		for (int i = 0; i < args; i++) {
			printf("%s\n", argv[i]);
		}
		return 1;
	}
	
	char input[BUF_SIZE];
	char output[BUF_SIZE];
	
	sprintf(input, "%s", argv[1]);

	FILE *wfl;
	FILE *rfl;
	wfl = fopen(PROC_FILE, "w");
	if (wfl == NULL) {
		printf("proc open error.");
		return 1;
	}
	fprintf(wfl, "%s", input);
	
	fclose(wfl);
	
	rfl = fopen(PROC_FILE, "r");
	while (fgets(output, 256, rfl) != NULL) {
		printf("%s", output);
	}
	fclose(rfl);
	
	return 0;
}
