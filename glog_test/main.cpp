#include <iostream>
#include "glog/logging.h"
using namespace std;

void test1()
{
	google::InitGoogleLogging("test1"); //��ʼ�� glog

	FLAGS_log_dir = "./log";	// ������־Ŀ¼
	FLAGS_logbufsecs = 0;        //������־�����Ĭ��Ϊ30�룬�˴���Ϊ�������
	FLAGS_max_log_size = 100;  //�����־��СΪ 100MB

	google::SetStderrLogging(google::GLOG_INFO); //���ü������ google::INFO ����־ͬʱ�������Ļ

	google::SetLogDestination(google::GLOG_INFO, "./log/INFO_"); //���� google::INFO �������־�洢·�����ļ���ǰ׺
	google::SetLogDestination(google::GLOG_WARNING, "./log/WARNING_");   //���� google::WARNING �������־�洢·�����ļ���ǰ׺
	google::SetLogDestination(google::GLOG_ERROR, "./log/ERROR_");   //���� google::ERROR �������־�洢·�����ļ���ǰ׺

	LOG(INFO) << "Hello,INFO!";
	LOG(WARNING) << "Hello,WARNING!";
	LOG(ERROR) << "Hello,ERROR!";
}

void test2()
{
	FLAGS_log_dir = "./log";
	google::InitGoogleLogging("test2");
	google::SetLogDestination(google::GLOG_INFO, "./Log/INFO_");
	google::SetStderrLogging(google::GLOG_INFO);
	google::SetLogFilenameExtension("log_");
	FLAGS_colorlogtostderr = true;  // Set log color
	FLAGS_logbufsecs = 0;  // Set log output speed(s)
	FLAGS_max_log_size = 1024;  // Set max log file size
	FLAGS_stop_logging_if_full_disk = true;  // If disk is full
	char str[20] = "hello log!";
	LOG(INFO) << str;
	string szStr = "hello google!";
	LOG(INFO) << szStr;
	LOG(INFO) << "info test" << "hello log!";  //���һ��Info��־
	LOG(WARNING) << "warning test";  //���һ��Warning��־
	LOG(ERROR) << "error test";  //���һ��Error��־
	google::ShutdownGoogleLogging();
}

int main(int argc, char *argv[])
{
	test1();
	return 0;
}