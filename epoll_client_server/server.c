// demo: epoll socket tcp server 
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
#include <stdlib.h>
#include <sys/epoll.h>

#define MAX_EVENTS 10

int main(int argc, char* argv[])
{
	if(argc<2){
		printf("eg: ./server.o port_nu\n");
		exit(1);
	}
	int port_nu = atoi(argv[1]); // port number
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

	// 设置端口复用的属性，为了防止服务器重启时端口还没有释放，因为程序突然退出，要等待msl才会退出
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

	struct epoll_event ev, events[MAX_EVENTS];
	int conn_sock, nfds;

	/* Code to set up listening socket, 'socket_fd',
	   (socket(), bind(), listen()) omitted */
	int epollfd = epoll_create1(0);
	if (epollfd == -1) {
		perror("epoll_create1");
		exit(EXIT_FAILURE);
	}

	ev.events = EPOLLIN;
	ev.data.fd = socket_fd;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, socket_fd, &ev) == -1) {
		perror("epoll_ctl: listen_sock");
		exit(EXIT_FAILURE);
	}


	struct sockaddr_in addr;
	int addrlen = sizeof(addr);
	int running = 1; // 设置停止的标志位
	for (;running;) {
		printf("\npolling for inputing...\n");
		nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
		if (nfds == -1) {
			perror("epoll_wait");
			exit(EXIT_FAILURE);
		}
		printf("%d ready events\n", nfds);

		for (int n = 0; n < nfds; ++n) {
			int fd = events[n].data.fd;
			// 判断是否有新连接
			if (fd == socket_fd) {
				conn_sock = accept(socket_fd, (struct sockaddr *) &addr, &addrlen);
				if (conn_sock == -1) {
					perror("accept");
					exit(EXIT_FAILURE);
				}

				// 将conn_sock挂到epoll树上
				ev.events = EPOLLIN | EPOLLET; // 设置为边沿触发模式
				ev.data.fd = conn_sock;
				if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock, &ev) == -1) {
					perror("epoll_ctl: conn_sock");
					exit(EXIT_FAILURE);
				}
				
				// print IP and port of client
				printf("accept client IP:%s, port:%d\n", \
						inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
			} else {
				// 处理已经连接的客户端
				if(!events[n].events & EPOLLIN) continue; // 该程序只对读事件进行操作，其他的时间不予处理
				char read_buffer[1024]={0};
				printf("Reading file descriptor '%d' -- ", events[n].data.fd);
				size_t bytes_read = read(events[n].data.fd, read_buffer, sizeof(read_buffer));
				if(bytes_read == -1){
					perror("read err");
					exit(-1);
				}else if(bytes_read == 0){
					printf("client has closed...\n");	
					// 将fd从ep树上删除
					if(epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL) == -1){
						perror("epoll_ctl_del err");
						exit(1);
					}
					close(fd);
					continue;
				}
				printf("%zd bytes read.\n", bytes_read);
				read_buffer[bytes_read] = '\0';
				printf("Read data: '%s'\n", read_buffer);

				if(!strncmp(read_buffer, "stop\n", 5))
					running = 0;

				// write data
				int write_ret = write(events[n].data.fd, read_buffer, bytes_read);
				if(write_ret < 0){
					printf("err: write fail! caused by %s\n", strerror(errno));
				}
			}
		}
	}


	// 6.close
	close(socket_fd);
	return 0;
}
