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
#include <errno.h>

#define SYSFS_FILE "/sys/bus/platform/drivers/test_int/__test_int_driver"
#define DATA_SIZE 1000

int process_all(int sock);

int main(void){
	
	int sock_org;
	struct sockaddr_in addr;
	struct sockaddr_in client;
	size_t len;
	int sock;
	
	sock_org = socket(AF_INET, SOCK_STREAM, 0);

	addr.sin_family = AF_INET;
	addr.sin_port = htons(24);
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

int process_all(int sock){
	FILE *f;
	int sensor_data[DATA_SIZE] = {0};
	int ret;
	int write_result;
	int i;
	
	for(;;){
		f = fopen(SYSFS_FILE, "r");
		if(f == NULL){
printf("f==NULL\n");
			perror("fopen");
			return EXIT_FAILURE;
		}
		ret = fread(sensor_data, sizeof(u_int), DATA_SIZE, f);
		fclose(f);
		if(ret == 0){
printf("data ret == 0\n");
			if(errno == EAGAIN)
				continue;
			return EXIT_FAILURE;
		}
printf("data ret = %d\n",ret);		
		for(i = 0; i < ret; i++){
			printf("sensor_data = %d\n", sensor_data[i]);
		}
		
		write_result = write(sock, sensor_data, DATA_SIZE * sizeof(u_int));
		//write_result = write(sock, sensor_data, sizeof(u_int));
		//write_result = write(sock, sensor_data, sizeof(u_int));
printf("write_result = %d\n", write_result);
		if(write_result == -1){
			printf("socket write failed !!");
		}

	}
	return 0;
}

