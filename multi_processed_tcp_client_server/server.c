// demo: multi_process tcp server
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <strings.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <signal.h>
#include <sys/wait.h>

// a signal handing function
void recycle(int num){
	pid_t pid;
	while((pid=waitpid(-1, NULL, WNOHANG)) > 0){
		// do sth	
		printf("child process died, pid=%d\n", pid);
	}
}

int main(int argc, char* argv[])
{
	// check user input port
	if(argc<2){
		printf("eg: ./server.o port\n");
		exit(1);
	}
	
	int port_nu = atoi(argv[1]); //	user input port number

	// 1.create a socket
	int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_fd < 0){
		printf("err: create socket fail! caused by %s\n", strerror(errno));
	}

	// 2.bind ip and port
	struct sockaddr_in server_addr;
	bzero((char*)& server_addr, sizeof(server_addr));

	server_addr.sin_family = AF_INET;                // addr family
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // ip addr
	server_addr.sin_port = htons(port_nu);	         // port num

	int bind_ret = bind(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if(bind_ret < 0){
		printf("err: bind fail! caused by %s\n", strerror(errno));
	}

	// 3.listen
	int listen_ret = listen(socket_fd, 128);
	if(listen_ret < 0){
		printf("err: listen fail! caused by %s", strerror(errno));
	}

	printf("the server started...\n");
	printf("listen on port:%d\n", port_nu);
	printf("waiting for client...\n");

	// signal recycle child process
	struct sigaction act;
	act.sa_handler = recycle;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);
	sigaction(SIGCHLD, &act, NULL);


	// 4.wait and connect, fork
	struct sockaddr_in client_addr;
	int len = sizeof(client_addr);
	while(1){
		// father process accept connection
		int client_fd = accept(socket_fd, (struct sockaddr*)&client_addr, &len);
		while(client_fd == -1 && errno == EINTR){
			client_fd = accept(socket_fd, (struct sockaddr*)&client_addr, &len);
		}

		// print IP and port of client
		printf("accept client IP:%s, port:%d\n", \
				inet_ntoa(client_addr.sin_addr), \
				ntohs(client_addr.sin_port));

		// create child process
		pid_t pid = fork();
		if(pid == 0){
			//child process

			close(socket_fd);
			// data transfer
			while(1){
				char buff[1024];
				memset(buff, 0, sizeof(buff));
				int read_ret = read(client_fd, buff, sizeof(buff));	
				if(read_ret == -1){
					printf("err: read fail! caused by %s\n", strerror(errno));	
					exit(1);
				}else if(read_ret == 0){
					printf("client has closed!\n");
					close(client_fd);
					break;
				}
				printf("[IP:%s, port:%d] recv data:%s\n", \
				inet_ntoa(client_addr.sin_addr), \
				ntohs(client_addr.sin_port), buff);
				write(client_fd, buff, sizeof(buff));
			}
			return 0;//exit child process

		}else if(pid > 0){ 
			//father process
			close(client_fd);
		}

	}

	// 6.close
	close(socket_fd);

	return 0;
}
