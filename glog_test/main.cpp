#include <iostream>
#include "glog/logging.h"
using namespace std;

void test1()
{
	google::InitGoogleLogging("test1"); //初始化 glog

	FLAGS_log_dir = "./log";	// 设置日志目录
	FLAGS_logbufsecs = 0;        //缓冲日志输出，默认为30秒，此处改为立即输出
	FLAGS_max_log_size = 100;  //最大日志大小为 100MB

	google::SetStderrLogging(google::GLOG_INFO); //设置级别高于 google::INFO 的日志同时输出到屏幕

	google::SetLogDestination(google::GLOG_INFO, "./log/INFO_"); //设置 google::INFO 级别的日志存储路径和文件名前缀
	google::SetLogDestination(google::GLOG_WARNING, "./log/WARNING_");   //设置 google::WARNING 级别的日志存储路径和文件名前缀
	google::SetLogDestination(google::GLOG_ERROR, "./log/ERROR_");   //设置 google::ERROR 级别的日志存储路径和文件名前缀

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
	LOG(INFO) << "info test" << "hello log!";  //输出一个Info日志
	LOG(WARNING) << "warning test";  //输出一个Warning日志
	LOG(ERROR) << "error test";  //输出一个Error日志
	google::ShutdownGoogleLogging();
}

int main(int argc, char *argv[])
{
	test1();
	return 0;
}