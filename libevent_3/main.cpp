#include "event2/event.h"
#include "event2/bufferevent.h"
#include "event2/listener.h"
#include <WinSock2.h>
#include <windows.h>
#include <iostream>
#include<WS2tcpip.h>
#include <assert.h>
#include <cstring>
using namespace std;

void read_cb(struct bufferevent *bev, void *arg);
void error_cb(struct bufferevent *bev, short event, void *arg);
void listener_cb(evconnlistener *listener, evutil_socket_t fd, struct sockaddr *sock, int socklen, void *arg);

void read_cb(struct bufferevent *bev, void *arg)
{
	evutil_socket_t fd = bufferevent_getfd(bev);

	char buf[1024] = { 0 };
	int iSize = 0;
	while ((iSize = bufferevent_read(bev, buf, 1024)) > 0)
	{
		buf[iSize] = '\0';
		cout << "read message: " << buf << endl;

		char reply_msg[4096] = "I have recvieced the msg: ";
		strcat(reply_msg + strlen(reply_msg), buf);
		bufferevent_write(bev, reply_msg, strlen(reply_msg));
	}
}

void error_cb(struct bufferevent *bev, short event, void *arg)
{
	evutil_socket_t fd = bufferevent_getfd(bev);
	if (event & BEV_EVENT_TIMEOUT)
	{
		cout << "time out" << endl;
	}
	else if (event & BEV_EVENT_EOF)
	{
		cout << "connection close" << endl;
	}
	else if (event & BEV_EVENT_ERROR) {
		cout << "other error" << endl;
	}

	//这将自动close套接字和free读写缓冲区  
	bufferevent_free(bev);
}

//一个新客户端连接上服务器了    
//当此函数被调用时，libevent已经帮我们accept了这个客户端。该客户端的  
//文件描述符为fd
void listener_cb(evconnlistener *listener, evutil_socket_t fd, struct sockaddr *sock, int socklen, void *arg)
{
	sockaddr_in *clientAddr = (sockaddr_in*)sock;

	char cClinetIp[20] = { 0 };
	inet_ntop(AF_INET, (void*)&clientAddr->sin_addr, cClinetIp, 16);
	cout << cClinetIp << ":" << ntohs(clientAddr->sin_port) << endl;

	event_base *base = evconnlistener_get_base(listener);
	//为这个客户端分配一个bufferevent    
	bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
	
	bufferevent_setcb(bev, read_cb, NULL, error_cb, NULL);
	bufferevent_enable(bev, EV_READ | EV_PERSIST);
}

int main(int argc, char *argv[])
{
	WSADATA data;
	WORD w = MAKEWORD(2, 0);//版本号
	WSAStartup(w, &data); //动态链接库初始化

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(10002);
	addr.sin_addr.s_addr = 0;

	//创建一个event_base  
	struct event_base *base = event_base_new();
	assert(base != NULL);

	evconnlistener *listener = evconnlistener_new_bind(base, listener_cb, base,
		LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,
		10, (struct sockaddr*)&addr,
		sizeof(struct sockaddr_in));

	//启动事件循环  
	event_base_dispatch(base);

	evconnlistener_free(listener);
	event_base_free(base);

	cout << "end...." << endl;

	getchar();
	return 0;
}