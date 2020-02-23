// demo: socket tcp server
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <strings.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>

int port_nu = 8080; // port number

int main(int argc, char* argv[])
{

	// 1.create a socket
	int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_fd < 0){
		printf("err: create socket fail! caused by %s\n", strerror(errno));
	}

	// 2.bind ip and port
	struct sockaddr_in server_addr;
	bzero((char*)& server_addr, sizeof(server_addr));

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // ip addr
	server_addr.sin_port = htons(port_nu);	         // port num

	int opt = 1;
	setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

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

	// 4.wait and connect
	struct sockaddr_in client_addr;
	int len = sizeof(client_addr);
	int client_fd = accept(socket_fd, (struct sockaddr*)&client_addr, &len);
	if(client_fd < 0){
		printf("err: accept fail! caused by %s\n", strerror(errno));
	}

	// print IP and port of client
	printf("accept client IP:%s, port:%d\n", \
			inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

	while(1){
		// 5.receive and send data
		char buffer_received[1024];
		memset(buffer_received, '\0', sizeof(buffer_received));

		int len_received = read(client_fd, buffer_received, sizeof(buffer_received));
		if(len_received < 0){
			printf("read failed! caused by %s\n", strerror(errno));
		}

		// print data received
		printf("read bytes length: %d\n", len_received);
		printf("received data:%s\n", buffer_received);

		// write data
		//sprintf(buffer_received, "%s [server recerved]", buffer_received);
		int write_ret = write(client_fd, buffer_received, sizeof(buffer_received));
		if(write_ret < 0){
			printf("err: write fail! caused by %s\n", strerror(errno));
		}
	}

	// 6.close
	close(socket_fd);
	close(client_fd);

	return 0;
}
