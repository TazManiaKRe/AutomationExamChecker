#include <stdio.h>
#include <sys/fcntl.h> 
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#define SIZE 2

int main(int argc, char* argv[]) {
	
	//cheaking if there 3 arguments
	//argv[1] = program name; argv[2] = file1; argv[3] = file 2
	if (argc != 3) {
		printf("Arguments Error\n");
		exit(1);
	}
	//copy 2 names of arguments
	char* FILE1 = argv[1];
	char* FILE2 = argv[2];

	//open buffers with 0's
	char* buff1 = calloc(SIZE, SIZE);
	char* buff2 = calloc(SIZE, SIZE);
	
	//open fileDescriptors
	int FD1 = open(FILE1, O_RDONLY);
	int FD2 = open(FILE2, O_RDONLY);
	if (FD1 == -1 || FD2 == -1) {
		printf("File Descripor is not good\n");
		exit(1);
	}
	 
	//open 2 files for reading only with 1 char 
	int fRead1 = read(FD1, buff1, SIZE - 1);
	int fRead2 = read(FD2, buff2, SIZE - 1);
	//cheak if the fopen for  reading is good
	if (fRead1 == -1 || fRead2 == -1) {
		//printf("File read is not good\n");
		exit(1);
	}
	if ((buff1[0] == '\0') && (buff2[0] == '\0')) {
		//printf("Both files are empty\n");
		exit(2);
	}

	if ((buff1[0] == '\0') || (buff2[0] == '\0')) {
		//printf("Somefiles are empty\n");
		exit(1);
	}
	


	while (fRead1 > 0 && fRead2 > 0) {
		if (buff1[0] != buff2[0]) {
			printf("The chars are not equal\n");
			exit(1);
		}

		//reading the next char
		fRead1 = read(FD1, buff1, SIZE - 1);
		fRead2 = read(FD2, buff2, SIZE - 1);
		//cheaking if the reading is exit with errror
		if (fRead1 == -1 || fRead2 == -1) {
			printf("Error\n");
			exit(1);
		}
		if ((fRead1 == 0 && fRead2 > 0) || (fRead1 > 0 && fRead2 == 0)) {
			printf("They are not equal\n");
			exit(1);
		}
		if (fRead1 == 0 && fRead2 == 0) {
			printf("They are equal\n");
			exit(2);
		}

	}
	
	return 0;
}
