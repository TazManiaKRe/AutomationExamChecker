
//Ex1 steps:
//Step 1: compile every student .c file
//step 2: running the student programs .exe files
//step 3: compare the student program output with the expected output
//step 4: writing into cvs file and deleting the buffer files for every student.
#include <stdio.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/wait.h> 

#define SIZE 2
#define SIZEARRAY 120

void copyPathIntoArr(char* arr, char* aOut, char* path, char* dName);
void copyExeIntoArr(char* exe, char* bPath, char* line1);
void copyExeIntoArr1(char* exe, char* bPath, char* dName);
void copyBufferPath(char* bPath, char* dName);
void copyArrToDelete(char* path, char* dName, char* line1);

int main(int argc, char* argv[]) {

	// cheak if the arguments are ok
	if (argc != 2) {
		printf("arguments ERROR\n");
		exit(1);
	}
	// taking the config name // dont forget to delete after
	char* config = argv[1];
	int stat;
	//buffer for copy file into arrey // dont forget to delete  // deleted!!
	char* buff1 = calloc(SIZE, SIZE); //size 1 

	//the 3 elements from the file //dont forget to delete pointers
	char* line1 = calloc(SIZEARRAY, SIZEARRAY);
	char* line2 = calloc(SIZEARRAY, SIZEARRAY);
	char* line3 = calloc(SIZEARRAY, SIZEARRAY);

	//exe path for students buffer path for each student
	char* exePath = calloc(SIZEARRAY, SIZEARRAY);
	char* bufferPath = calloc(SIZEARRAY, SIZEARRAY);

	//deleting array for buffer.txt for each students
	char* arrDelete = calloc(SIZEARRAY, SIZEARRAY);

	//arrays Path For every Student
	char* arr = calloc(SIZEARRAY, SIZEARRAY);
	char* aOut = calloc(SIZEARRAY, SIZEARRAY);

	//opens fileDescriotor // for the 3 lines
	int FD1 = open(config, O_RDONLY);
	if (FD1 == -1) {
		printf("file decriptor not succeed\n");
		exit(1);
	}

	//reading 1 char into the buff
	int fileRead = read(FD1, buff1, SIZE - 1);
	if (fileRead == -1) {
		printf("File Read not succed\n");
		exit(1);
	}

	//reading the file into 3 arrays - 3 lines = 3 arrays
	int flag = 1, counter1 = 0, counter2 = 0, counter3 = 0;
	while (fileRead > 0) {
		if (buff1[0] != '\n') {
			if (flag == 1) {
				line1[counter1] = buff1[0];
				counter1++;
			}
			else if (flag == 2) {
				line2[counter2] = buff1[0];
				counter2++;
			}
			else if (flag == 3) {
				line3[counter3] = buff1[0];
				counter3++;
			}
		}
		else flag++;
		fileRead = read(FD1, buff1, SIZE - 1);
	}


	copyExeIntoArr(exePath, bufferPath, line1);

	struct dirent* de;
	DIR* dr = opendir(line1);
	if (dr == NULL) {
		printf("erorr dir\n");
		exit(1);
	}
	while ((de = readdir(dr)) != NULL) {
		if ((strcmp(de->d_name, ".") != 0) && (strcmp(de->d_name, "..") != 0)) {
			//complies every .c program into main.exe for each student
			pid_t pid = fork();
			if (pid == 0) {
				copyPathIntoArr(arr, aOut, line1, de->d_name);
				int exec = execlp("gcc", "gcc", arr, "-o", aOut, NULL);
				if (exec == -1) {
					printf("exec gcc ERORR\n");
				}
			}

			wait(NULL);
			//exe program and writiing into files for each student
			pid_t pidd = fork();
			if (pidd == 0) {
				copyExeIntoArr1(exePath, bufferPath, de->d_name);

				int expected = open(line2, O_RDONLY | 0666);
				int buffer = open(bufferPath, O_CREAT | O_RDWR | O_TRUNC | 0666);

				//keyboard
				close(0);
				dup(expected);
				close(expected);
				//screen
				close(1);
				dup(buffer);
				close(buffer);

				int exeForExp = execlp(exePath,exePath, NULL);
				if (exeForExp == -1) {
					printf("Wrong arguments\n");
					exit(1);
				}
			}
			wait(NULL);
			pid_t lastPid = fork();
			if (lastPid == 0) {
				copyBufferPath(bufferPath, de->d_name);
				int lastCheak = execlp("./comp.out", "./comp.out", bufferPath, line3, NULL);
				if (lastCheak == -1) {
					printf("The compare is wrong.. \n");
					exit(1);
				}
			}
			else {
				wait(&stat);
				if (WIFEXITED(stat)) {
					int res = open("results.csv", O_CREAT | O_APPEND | O_RDWR, 0777);
					close(1);
					dup(res);
					close(res);
					if (WEXITSTATUS(stat) == 1)
						printf("Student name: %s - grade is: 0\n", de->d_name);
					else printf("Student name: %s - grade is: 100\n", de->d_name);

				}
			}
		}

	}
	struct dirent* dee;
	DIR* drr = opendir(line1);
	if (drr == NULL) {
		perror("erorr dir\n");
		exit(1);
	}
	while ((dee = readdir(drr)) != NULL) {
		if ((strcmp(dee->d_name, ".") != 0) && (strcmp(dee->d_name, "..") != 0)) {
			pid_t pidEnd = fork();
			if (pidEnd == 0) {
				printf("my name");
				copyArrToDelete(arrDelete, dee->d_name, line1);
				int deleteBuff = execlp("rm", "-f", arrDelete, NULL);
				if (deleteBuff == -1) {
					perror("Wrong deleting buff\n");
					exit(1);
				}
			}
			else wait(NULL);
		}
	}
	wait(NULL);
	//closedir(dr); //can not be closed in collage servers..
	//closedir(drr);
	close(FD1);
	free(buff1);
	free(line1);
	free(line2);
	free(line3);
	free(exePath);
	free(bufferPath);
	free(arr);
	free(aOut);
	free(de);
	free(dee);
	free(arrDelete);
	close(fileRead);
	exit(0);
}

void copyPathIntoArr(char* arr, char* aOut, char* path, char* dName) {
	strcpy(arr, path);
	strcat(arr, "/");
	strcat(arr, dName);
	strcat(arr, "/");
	strcpy(aOut, arr);
	strcat(arr, dName);
	strcat(arr, ".c");
	strcat(aOut, "main.exe");
}

void copyExeIntoArr(char* exe, char* bPath, char* line1) {
	strcpy(exe, line1);
	strcpy(bPath, line1);
	strcat(exe, "/");
	strcat(bPath, "/");
}

void copyExeIntoArr1(char* exe, char* bPath, char* dName) {
	strcat(exe, dName);
	strcat(exe, "/main.exe");
	strcat(bPath, dName);
	strcat(bPath, "/buffer.txt");
}
void copyBufferPath(char* bPath, char* dName) {
	strcat(bPath, dName);
	strcat(bPath, "/buffer.txt");
}
void copyArrToDelete(char* path, char* dName, char* line1) {
	strcpy(path, line1);
	strcat(path, "/");
	strcat(path, dName);
	strcat(path, "/buffer.txt");
}
