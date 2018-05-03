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
void socket_read_cb(evutil_socket_t fd, short event, void *arg);

void do_accept(evutil_socket_t sock, short event, void *arg)
{
	struct event_base *base = (struct event_base *)arg;
	evutil_socket_t fd;
	struct sockaddr_in clientAddr;
	socklen_t slen = sizeof(clientAddr);
	fd = accept(sock, (struct sockaddr *)&clientAddr, &slen);

	char cClinetIp[20] = { 0 };
	inet_ntop(AF_INET, (void*)&clientAddr.sin_addr, cClinetIp, 16);
	cout << cClinetIp << ":" << ntohs(clientAddr.sin_port) << endl;


	//������Ϊ�˶�̬����һ��event�ṹ��  
	struct event *ev = event_new(NULL, -1, 0, NULL, NULL);
	//����̬�����Ľṹ����Ϊevent�Ļص�����  
	event_assign(ev, base, fd, EV_READ | EV_PERSIST, socket_read_cb, (void*)ev);
	event_add(ev, NULL);

	/*struct bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
	bufferevent_setcb(bev, read_cb, NULL, error_cb, arg);
	bufferevent_enable(bev, EV_READ | EV_WRITE | EV_PERSIST);*/
}

void socket_read_cb(evutil_socket_t fd, short event, void *arg)
{
	char msg[4096];
	struct event *ev = (struct event*)arg;
	int len = recv(fd, msg, sizeof(msg), 0);

	if (len <= 0)
	{
		cout << "connection close!" << endl;
		event_free(ev);
		evutil_closesocket(fd);
		return;
	}

	msg[len] = '\0';
	cout << "read message: " << msg << endl;

	char reply_msg[4096] = "I have recvieced the msg: ";
	strcat(reply_msg + strlen(reply_msg), msg);
	len = send(fd, reply_msg, strlen(reply_msg), 0);
	if (len <= 0)
	{
		cout << "connection close!" << endl;
		event_free(ev);
		evutil_closesocket(fd);
		return;
	}
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

		bufferevent_write(bev, buf, iSize);
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
		event_add(listen_event, NULL);

		//�����¼�ѭ��  
		event_base_dispatch(base);

		cout << "end...." << endl;
		evutil_closesocket(sock);
	}

	getchar();
	return 0;
}