#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

#define SYSFS_FILE "/sys/bus/platform/drivers/test_int/__test_int_driver"
#define DATA_SIZE 5

void process_all(int sock);
int move_servo(char *buff);

int main(void){
	
	int sock_org;
	struct sockaddr_in addr;
	struct sockaddr_in client;
	size_t len;
	int sock;
	
	sock_org = socket(AF_INET, SOCK_STREAM, 0);

	addr.sin_family = AF_INET;
	addr.sin_port = htons(23);
	addr.sin_addr.s_addr = INADDR_ANY;

	bind(sock_org, (struct sockaddr *)&addr, sizeof(addr));

	listen(sock_org, 5);

	while(1){
		len = sizeof(client);
		sock = accept(sock_org, (struct sockaddr *)&client, &len);
		process_all(sock);
	}

	close(sock_org);

}

void process_all(int sock){
	int read_size;
	char command[1] = {0};
	char buff[DATA_SIZE] = {0};
	
		while(1){
			read_size = read(sock, command, 1);
			if(read_size <= 0){
				continue;
			}else if(command[0] == 'e'){
printf("end \n");
				close(sock);
				break;
			}else if(command[0] == 'd'){
				read_size = read(sock, buff, sizeof(buff));
//printf("buff = %s \n", buff);
				move_servo(buff);
			}else{
//printf("command = %c \n", command[0]);
				continue;
			}
		}

}

int move_servo(char *buff){

	FILE *f;
	int ret;

//printf("input data = %d size of buff = %d \n", atoi(buff), DATA_SIZE);
	
	f = fopen(SYSFS_FILE, "w");
	if(f == NULL){
		perror("fopen");
		return EXIT_FAILURE;
	}

	ret = fwrite(buff, DATA_SIZE, 1, f);
	fclose(f);
	if(ret < 1){
		return EXIT_FAILURE;
	}

	return ret;
}

