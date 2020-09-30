## 网络编程案例
### 介绍
在Linux下的网络编程demo，采用tcp协议，用c语言实现了**多进程**、**多线程**、**select**和**epoll**的并发服务器案例。启动服务器server后，等待客户端client的连接，再启动客户端client，连接服务器server，在client中输入字符串，通过socket发送到服务器server，在服务器server中打印接收到的字符串、客户端client的IP和port端口，并给客户端client回复相同的字符串。

文件说明:
- `socket_tcp_client_server`文件夹：**简单的client和server通信**
- `multi_processed_tcp_client_server`文件夹：**多进程的并发服务器案例**
- `multi_thread_tcp_client_server`文件夹：**多线程的并发服务器案例**
- `select_client_server`文件夹：**select实现IO多路转接的并发服务器案例**
- `epoll_client_server`文件夹：**epoll实现IO多路转接的并发服务器案例**
### 依赖
- 编程环境：在VMware虚拟机中的`Ubuntu 16.04`
- 编程语言：c
- 编译器：gcc

### 用法
1. 从该主页下载压缩包
2. 将压缩包上传到Linux操作系统，可以通过`FileZilla`软件上传
3. 解压
4. 进入到三个文件夹中的任意一个，在Linux终端输入make，编译成可执行文件
5. 在终端运行服务器的可执行程序`server.o`
```
./server.o port_number
```
6. 在另一个终端运行客户端的可执行文件`client.o`
```
./server.o port_number
```
### 运行结果
1. 简单的client和server通信

**server端**

![server](https://gitee.com/seaworth/document/raw/markdown-picture/2020/20200930091652.png)

**client端**

![client](https://gitee.com/seaworth/document/raw/markdown-picture/2020/20200930091757.png)

2. 多进程的并发服务器案例

![mutil_processed_tcp_client_server](https://gitee.com/seaworth/document/raw/markdown-picture/2020/20200930091820.gif)

欢迎star，fork！:blush:
