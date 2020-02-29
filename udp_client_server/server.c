#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>


int main(int argc, char* argv[])
{
	if(argc < 2){
		printf("eg: ./server.o port_n\nu");
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

	// bind()
	int bind_ret = bind(sock_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	if(bind_ret == -1){
		perror("bind err");
		exit(-1);
	}

	struct sockaddr_in clie_addr;
	memset(&clie_addr, 0, sizeof(clie_addr));
	socklen_t clie_len = sizeof(clie_addr);

	char buf[1024] = {0};
	while(1){
		// recvfrom()
		printf("recvfrom...\n");
		int recv_ret = recvfrom(sock_fd, buf, sizeof(buf), 0,  (struct sockaddr*)&clie_addr, &clie_len);
		if( recv_ret==-1){
			perror("recvfrom err");
			exit(-1);
		}

		printf("accept client IP:%s, port:%d\n", inet_ntoa(clie_addr.sin_addr), ntohs(clie_addr.sin_port) );

		buf[recv_ret]='\0';	
		printf("recv data:%s\n", buf);

		// sendto()
		int send_ret = sendto(sock_fd, buf, sizeof(buf), 0, (struct sockaddr*)&clie_addr, clie_len);
		if(send_ret == -1){
			printf("sendto err");
			exit(-1);
		}
	}
	
	close(sock_fd);
	return 0;
}
