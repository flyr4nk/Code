/******************************************************
* Author       : fengzhimin
* Create       : 2016-11-06 00:44
* Last modified: 2017-02-14 13:16
* Email        : 374648064@qq.com
* Filename     : config.h
* Descrip:q
* function  : 全局的宏定义
******************************************************/

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdio.h>
#include <unistd.h>

#define OPENLOG 1    //1: 表示打开日志功能　　　0: 表示关闭日志功能
#define SHOWINFO 3    //1：表示冲突信息存在result.out文件中　　２：打印冲突信息　　３：打印并且存储冲突信息

#define CONFIG_FILE_MAX_NUM   100   //一个软件的最大配置文件个数由程序自动寻找配置文件

#define FILE_PATH_MAX_LENGTH   256    //一个文件的路径最大长度

#define LINE_CHAR_MAX_NUM      1024   //一行最大字符个数

#define MAX_FUNCNAME          50    //函数名称的最大字符个数

extern char config_type[][20];    //配置文件的类型

/*****************************
 * function: 获取config_type的个数
 * return: 个数
******************************/
int GetConfig_TypeNum();

extern char note_symbol[][10];    //注释符号

/*****************************
 * function: 获取note_symbol的个数
 * return:　个数
******************************/
int GetNote_SymbolNum();

extern char mem_func[][20];   //关于内存操作的函数

/*****************************
 * function: 获取mem_func的个数
 * return: 个数
******************************/
int GetMemFunc_Num();

extern char proc_func[][20];  //关于进程操作的函数

/*****************************
 * function: 获取proc_func的个数
 * return: 个数
*****************************/
int GetProcFunc_Num();

extern char net_func[][20];   //关于网络操作的函数

/*****************************
 * function: 获取net_func的个数
 * return: 个数
*****************************/
int GetNetFunc_Num();

extern char dev_func[][20];   //关于操作设备的函数

/*****************************
 * function: 获取dev_func的个数
 * return: 个数
*****************************/
int GetDevFunc_Num();

#define CONFIG_KEY_MAX_NUM       50     //配置项key的最大值
#define CONFIG_VALUE_MAX_NUM     30    //配置项value的最大值

#define MAX_SUBSTR               256   //拆分后子字符串的最大长度
#define MAX_INFOLENGTH           50    //获取应用程序占用系统每个资源数据的最大长度

/***************************
 * function: 定义配置项结构体
***************************/
typedef struct ConfigInfo
{
	char key[CONFIG_KEY_MAX_NUM];
	char value[CONFIG_VALUE_MAX_NUM];
} ConfigInfo;

/***************************
 * function: 定义CPU使用时间
 * 详细信息查看 man proc
***************************/
typedef struct TotalCpuTime
{
	unsigned int user;
	unsigned int nice;
	unsigned int system;
	unsigned int idle;
} Total_Cpu_Occupy_t;

/***************************
 * function: 定义程序CPU使用时间
 * 详细信息查看 man proc
****************************/
typedef struct ProcessCpuTime
{
	pid_t pid;
	unsigned int utime;
	unsigned int stime;
	unsigned int cutime;
	unsigned int cstime;
} Process_Cpu_Occupy_t;


/***************************
 * function: 定义函数打分情况
****************************/
typedef struct FuncResourceScore
{
	char funcName[MAX_FUNCNAME];
	int memScore;    //内存的分数
	int procScore;   //进程的分数
	int devScore;    //设备的分数
	int netScore;    //网络的分数
} FuncScore;

#endif

