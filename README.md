## 网络编程案例
### 介绍
在Linux下的网络编程demo，采用tcp协议，用c语言实现了多进程和多线程的并发服务器案例。
文件说明
- `socket_tcp_client_server`文件夹：简单的client和server通信
- `multi_processed_tcp_client_server`文件夹：多进程的并发服务器案例
- `multi_thread_tcp_client_server`文件夹：多线程的并发服务器案例
### 依赖
编程环境：在VMware虚拟机中的`Ubuntu 16.04`
编程语言：c
编译器：gcc

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

