#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>

#include "mod.h"

#define PROC_FILE "/proc/inode_info"
int main(int args, char **argv) {
	
	if (args != 2) {
		printf("Wrong number of arguments: %d. Provided: 1\n", args);
		printf("Debug: argv:\n");
		for (int i = 0; i < args; i++) {
			printf("%s\n", argv[i]);
		}
		return 1;
	}
	
	char *file_path = argv[1];
	int path_len = strlen(argv[1]);
	
	struct inode_info info;
	
	printf("Input path: %s\n\n", file_path);
	
	int fd = open(PROC_FILE, O_WRONLY);
	if (fd == -1) {
		printf("proc opening failed.");
		return 1;
	}
	
	write(fd, file_path, path_len);
	close(fd);
	
	fd = open(PROC_FILE, O_RDONLY);
	read(fd, &info, sizeof(struct inode_info));
	
	printf("INODE INFO:\n");
	printf("inode number: %lu\n", info.i_no);
	printf("inode references count: %d\n", info.i_count);
	printf("inode access permissions: %llu\n", info.i_mode);
	printf("inode file size: %lld\n", info.i_size);
	printf("inode last modify time: %s", asctime(gmtime(&info.m_time)));
	
	close(fd);
	
	return 0;
}
