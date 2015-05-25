/*
 * create_rawramdump.c
 * Sometimes ramdumps received are split accross many files.
 * And there can be gaps in dumps. But crash tool need a raw data
 * file. This application creates raw file.
 */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

struct ram_partition_info {
	char *name;		/* Partition file name */
	unsigned int addr;	/* This is the physcial address of ram on device */
	int size;		/* only used by to fill zeros */
};

struct ram_partition_info info[] = {
	{ "DDRCS0_0.BIN", 	0x0 },
	{ "DDRCS1_0.BIN", 	0x30000000 },
	{ "zero", 		0x60000000, 0x20000000 },
	{ "DDRCS0_1.BIN", 	0x80000000 },
	{ "DDRCS1_1.BIN", 	0xb0000000 },
	{ NULL, 		NULL},
};

#define BUFFER_SIZE 1024

int main(void)
{
	int fd_in, fd_out;
	int file_size, i = 0;
	struct ram_partition_info *ri;
	int ret_in, ret_out;
	unsigned char buffer[BUFFER_SIZE];

	fd_out = open("RAM.BIN", O_WRONLY | O_CREAT, 0644);
	if (fd_out == -1) {
		perror ("open output file\n");
		return -1;
	}

	for (ri = &info[i]; ri->name != NULL; i++, ri = &info[i])
	{
		printf("file name = %s addr = %x\n", ri->name, ri->addr);

		if(!strncmp("zero", ri->name, 4)) {
			unsigned char junk[BUFFER_SIZE];
			int size = ri->size;
			printf("filling zeros...\n");
			while(size) {
				write(fd_out, &junk, BUFFER_SIZE);
				size -= BUFFER_SIZE;
			}
			printf("done.\n");
		} else {
			printf("non zero\n");
			fd_in = open(ri->name, O_RDONLY);
			if (fd_in == -1){
				perror ("failed to open input file\n");
				return -1;
			}
			printf("copy in progress...\n");
			/* Copy process */
			while((ret_in = read (fd_in, &buffer, BUFFER_SIZE)) > 0){
				ret_out = write (fd_out, &buffer, (ssize_t) ret_in);
				if(ret_out != ret_in){
					/* Write error */
					perror("write");
					return 4;
				}
			}
			close(fd_in);
			printf("done.\n");
		}
	}
	return 0;
}
