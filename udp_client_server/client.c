#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>


int main(int argc, char* argv[])
{
	if(argc < 2){
		printf("eg: ./client.o port_n\nu");
		exit(-1);
	}
	
	int port = atoi(argv[1]);

	// socket()
	int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if( sock_fd == -1){
		perror("socket err");
		exit(-1);
	}

	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	socklen_t serv_len = sizeof(serv_addr);

	char buf[1024] = {0};
	while(1){
		// sendto()
		printf("input data:");
		fgets(buf, sizeof(buf), stdin);
		int send_ret = sendto(sock_fd, buf, sizeof(buf), 0, (struct sockaddr*)&serv_addr, serv_len);
		if(send_ret == -1){
			printf("sendto err");
			exit(-1);
		}

		// recvfrom()
		int recv_ret = recvfrom(sock_fd, buf, sizeof(buf), 0,  NULL, NULL);
		if( recv_ret==-1){
			perror("recvfrom err");
			exit(-1);
		}
		buf[recv_ret]='\0';	
		printf("recv data:%s\n", buf);
	}
	
	close(sock_fd);
	return 0;
}
