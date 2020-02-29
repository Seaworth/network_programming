// demo: select socket tcp server
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
#include <sys/select.h>
#include <sys/time.h>
#include <stdlib.h>


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

	struct sockaddr_in client_addr;
	int len = sizeof(client_addr);

	// 最大的文件描述符
	int max_fd = socket_fd;
	fd_set rset, tmp; // 读集合，和一个备份
	FD_ZERO(&rset);
	FD_SET(socket_fd, &rset);

	while(1){
		// 委托内核帮助IO检测
		tmp = rset; //保证tmp是最新的，将tmp传给select函数
		int select_ret = select(max_fd+1, &tmp, NULL, NULL, NULL);
		if(select_ret == -1){
			perror("select fail!");
			exit(1);
		}

		// 如果客户端发起了连接
		if(FD_ISSET(socket_fd, &tmp)){
			// 接收连接请求，accept不阻塞
			int client_fd = accept(socket_fd, (struct sockaddr*)&client_addr, &len);
			if(client_fd == -1){
				perror("accept fail!");
				exit(1);
			}

			// print IP and port of client
			printf("accept client IP:%s, port:%d\n", \
					inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

			// 将client_fd 加入到待检测的读集合rset中，下次就能够检测到
			FD_SET(client_fd, &rset);
			// 更新最大的文件描述符，因为下次循环的时候需要传递给select函数，内核检测遍历的时候需要最大值来确认遍历的上界
			max_fd = client_fd > max_fd ? client_fd : max_fd;
		}

		// 已经连接的客户端发送的数据到达
		for(int i=socket_fd+1; i<=max_fd; ++i){
			if(FD_ISSET(i, &tmp)){
				// 通信
				char buf[1024]={0};
				int recv_ret = recv(i, buf, sizeof(buf), 0);
				if(recv_ret == -1){
					perror("recv fail!");
					exit(1);
				}else if(recv_ret == 0){
					printf("client has closed.\n");
					close(i);
					FD_CLR(i, &rset); // 从原集合rset中删除
				}

				printf("recv data:%s\n", buf);
				send(i, buf, strlen(buf)+1, 0); // +1 发送\0
			}

		}

	}

	// print IP and port of client
	printf("accept client IP:%s, port:%d\n", \
			inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

	// 6.close
	close(socket_fd);

	return 0;
}
