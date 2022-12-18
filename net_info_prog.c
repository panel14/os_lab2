#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>

#include "mod.h"

#define PROC_FILE "/proc/net_info"
int main(int args, char **argv) {
	
	if (args != 2) {
		printf("Wrong number of arguments: %d. Provided: 1\n", args);
		printf("Debug: argv:\n");
		for (int i = 0; i < args; i++) {
			printf("%s\n", argv[i]);
		}
		return 1;
	}
	
	char *dev_name = argv[1];
	int n_len = strlen(argv[1]);
	
	struct net_device_info info;
	
	printf("Input net device name: %s\n\n", dev_name);
	
	int fd = open(PROC_FILE, O_WRONLY);
	if (fd == -1) {
		printf("proc opening failed.");
	}
	
	write(fd, dev_name, n_len);
	close(fd);
	
	fd = open(PROC_FILE, O_RDONLY);
	read(fd, &info, sizeof(struct net_device_info));
	
	printf("Input net device name: %s\n", dev_name);
	
	printf("NET DEVICE INFO:\n");
	printf("device memory start: %ld\n", info.mem_start);
	printf("device memory end: %ld\n", info.mem_end);
	printf("device I/O address: %ld\n", info.base_addr);
	printf("device state: %ld\n", info.state);
	
	close(fd);
	
	return 0;
}
