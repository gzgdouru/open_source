#include "event2/event.h"
#include "event2/bufferevent.h"
#include <WinSock2.h>
#include <windows.h>
#include <iostream>
#include<WS2tcpip.h>
#include <assert.h>
#include <cstring>
using namespace std;

void do_accept(evutil_socket_t sock, short event, void *arg);
void read_cb(struct bufferevent *bev, void *arg);
void error_cb(struct bufferevent *bev, short event, void *arg);

void do_accept(evutil_socket_t sock, short event, void *arg)
{
	struct event_base *base = (struct event_base *)arg;
	evutil_socket_t fd;
	struct sockaddr_in clientAddr;
	socklen_t slen = sizeof(clientAddr);
	fd = accept(sock, (struct sockaddr *)&clientAddr, &slen);
	cout << "accept fd = " << fd << endl;

	char cClinetIp[20] = { 0 };
	inet_ntop(AF_INET, (void*)&clientAddr.sin_addr, cClinetIp, 16);
	cout << cClinetIp << ":" << ntohs(clientAddr.sin_port) << endl;

	struct bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
	bufferevent_setcb(bev, read_cb, NULL, error_cb, arg);
	bufferevent_enable(bev, EV_READ | EV_WRITE | EV_PERSIST);
}

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

	//�⽫�Զ�close�׽��ֺ�free��д������  
	bufferevent_free(bev);
}

evutil_socket_t server_init()
{
	WSADATA data;
	WORD w = MAKEWORD(2, 0);//�汾��
	WSAStartup(w, &data); //��̬���ӿ��ʼ��

	evutil_socket_t sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock <= 0)
	{
		int iCode = WSAGetLastError();
		cout << "error code: " << iCode << endl;
		return -1;
	}

	//�����ΰ�ͬһ����ַ��Ҫ����socket��bind֮��
	evutil_make_listen_socket_reuseable(sock);

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(10002);
	addr.sin_addr.s_addr = 0;

	if (bind(sock, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		int iCode = WSAGetLastError();
		cout << "error code: " << iCode << endl;
		return -1;
	}

	if (listen(sock, 32) == SOCKET_ERROR)
	{
		int iCode = WSAGetLastError();
		cout << "error code: " << iCode << endl;
		return -1;
	}

	return sock;
}

int main(int argc, char *argv[])
{

	evutil_socket_t sock = server_init();
	if (sock > 0)
	{
		//��ƽ̨ͳһ�ӿڣ����׽�������Ϊ������״̬  
		evutil_make_socket_nonblocking(sock);

		//����һ��event_base  
		struct event_base *base = event_base_new();
		assert(base != NULL);

		//��������һ��event  
		struct event *listen_event;
		//������event_base, ������fd���¼����ͼ����ԣ��󶨵Ļص����������ص������Ĳ���  
		listen_event = event_new(base, sock, EV_READ | EV_PERSIST, do_accept, (void*)base);
		//������event����ʱʱ��(struct timeval *���͵ģ�NULL��ʾ�޳�ʱ����)  
		timeval tv = { 3, 0 };
		event_add(listen_event, &tv);

		//�����¼�ѭ��  
		event_base_dispatch(base);


		event_base_free(base);

		cout << "end...." << endl;
	}

	getchar();
	return 0;
}