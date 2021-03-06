/******************************************************
* Author       : fengzhimin
* Create       : 2016-12-29 16:32
* Last modified: 2016-12-29 16:32
* Email        : 374648064@qq.com
* Filename     : resource.c
* Description  : 
******************************************************/

#include "running/resource.h"

int getStatusPathByName(char name[], char path[])
{
	char *root_path = "/proc";
	DIR *pdir;
	struct dirent *pdirent;
	pdir = opendir(root_path);
	if(pdir == NULL)
	{
		char error_info[200];
		sprintf(error_info, "%s%s%s%s%s", "打开文件夹: ", root_path, " 失败！ 错误信息： ", strerror(errno), "\n");
		RecordLog(error_info);
		return -1;
	}
	while((pdirent = readdir(pdir)) != NULL)
	{
		if(strcmp(pdirent->d_name, ".") == 0 || strcmp(pdirent->d_name, "..") == 0)
			continue;
		char temp[FILE_PATH_MAX_LENGTH], temp1[LINE_CHAR_MAX_NUM], temp2[FILE_PATH_MAX_LENGTH];
		memset(temp, 0, FILE_PATH_MAX_LENGTH);
		memset(temp2, 0, FILE_PATH_MAX_LENGTH);
		memset(temp1, 0, LINE_CHAR_MAX_NUM);
		sprintf(temp, "%s/%s", root_path, pdirent->d_name);
		if(Is_Dir(temp) == -2)   //判断是否为普通文件
			continue;
		else if(Is_Dir(temp) == -1)
		{
			sprintf(temp2, "%s/%s", temp, "status");
			if(access(temp2, F_OK) != -1)   //判断目录下的status文件是否存在
			{
				FILE *fd = OpenFile(temp2, "r");
				if(fd == NULL)
				{
					char error_info[200];
					sprintf(error_info, "%s%s%s%s%s", "打开文件: ", temp, " 失败！ 错误信息： ", strerror(errno), "\n");
					RecordLog(error_info);
					return -1;
				}
				if(ReadLine(fd, temp1) == -1)
				{
					char subStr[2][MAX_SUBSTR];
					cutStrByLabel(temp1, ':', subStr, 2);
					removeChar(subStr[1], '\t');
					if(strcasecmp(subStr[1], name) == 0)
					{
						strcpy(path, temp);
						CloseFile(fd);
						return 1;
					}
				}
				else
				{
					char error_info[200];
					sprintf(error_info, "%s%s%s%s%s", "读取文件: ", temp, " 失败！ 错误信息： ", strerror(errno), "\n");
					RecordLog(error_info);
				}

				CloseFile(fd);
			}
		}
	}

	return 0;
}

int getProgressInfo(char path[], char info[][MAX_INFOLENGTH])
{
	char status[FILE_PATH_MAX_LENGTH], stat[FILE_PATH_MAX_LENGTH], temp1[LINE_CHAR_MAX_NUM];
	memset(status, 0, FILE_PATH_MAX_LENGTH);
	memset(stat, 0, FILE_PATH_MAX_LENGTH);
	memset(temp1, 0, LINE_CHAR_MAX_NUM);
	sprintf(status, "%s/%s", path, "status");
	sprintf(stat, "%s/%s", path, "stat");
	FILE *fd = OpenFile(status, "r");
	if(fd == NULL)
	{
		char error_info[200];
		sprintf(error_info, "%s%s%s%s%s", "打开文件: ", status, " 失败！ 错误信息： ", strerror(errno), "\n");
		RecordLog(error_info);
		return -1;
	}
	while(ReadLine(fd, temp1) == -1)
	{
		char subStr[2][MAX_SUBSTR];
		cutStrByLabel(temp1, ':', subStr, 2);
		memset(temp1, 0, LINE_CHAR_MAX_NUM);
		removeChar(subStr[1], '\t');
		if(strcasecmp(subStr[0], "Name") == 0)
		{
			strcpy(info[0], subStr[1]);
			continue;
		}
		else if(strcasecmp(subStr[0], "Pid") == 0)
		{
			strcpy(info[1], subStr[1]);
			continue;
		}
		else if(strcasecmp(subStr[0], "PPid") == 0)
		{
			strcpy(info[2], subStr[1]);
			continue;
		}
		else if(strcasecmp(subStr[0], "VmPeak") == 0)
		{
			strcpy(info[5], subStr[1]);
			continue;
		}
		else if(strcasecmp(subStr[0], "VmRSS") == 0)
		{
			strcpy(info[6], subStr[1]);
			break;    //结束读取
		}
		else if(strcasecmp(subStr[0], "State") == 0)
		{
			strcpy(info[7], subStr[1]);
			continue;
		}
	}
	CloseFile(fd);
	char totalMem[30] = {0};
	if(getTotalPM(totalMem) == 1)
	{
		//计算内存使用率
		unsigned int totalMemNum = ExtractNumFromStr(totalMem);
		unsigned int vmrssNum = ExtractNumFromStr(info[6]);
		float pmem = 100.0*vmrssNum/totalMemNum;
		sprintf(info[4], "%.2f", pmem);
	}
	Total_Cpu_Occupy_t total_cpu_occupy1, total_cpu_occupy2;
	Process_Cpu_Occupy_t process_cpu_occupy1, process_cpu_occupy2;
	getTotalCPUTime(&total_cpu_occupy1);
	getProcessCPUTime(stat, &process_cpu_occupy1);
	usleep(500000);
	getTotalCPUTime(&total_cpu_occupy2);
	getProcessCPUTime(stat, &process_cpu_occupy2);
	float total_cpu1 = total_cpu_occupy1.user + total_cpu_occupy1.nice + total_cpu_occupy1.system + total_cpu_occupy1.idle;
	float total_cpu2 = total_cpu_occupy2.user + total_cpu_occupy2.nice + total_cpu_occupy2.system + total_cpu_occupy2.idle;
	float process_cpu1 = process_cpu_occupy1.utime + process_cpu_occupy1.stime + process_cpu_occupy1.cutime + process_cpu_occupy1.cstime;
	float process_cpu2 = process_cpu_occupy2.utime + process_cpu_occupy2.stime + process_cpu_occupy2.cutime + process_cpu_occupy2.cstime;
	float pcpu = 100.0*(process_cpu2-process_cpu1)/(total_cpu2-total_cpu1);
	sprintf(info[3], "%.2f", pcpu);
	return 1;
}

int getTotalPM(char totalMem[])
{
	char temp[FILE_PATH_MAX_LENGTH], temp1[LINE_CHAR_MAX_NUM];
	memset(temp, 0, FILE_PATH_MAX_LENGTH);
	memset(temp1, 0, LINE_CHAR_MAX_NUM);
	strcpy(temp, "/proc/meminfo");
	FILE *fd = OpenFile(temp, "r");
	if(fd == NULL)
	{
		char error_info[200];
		sprintf(error_info, "%s%s%s%s%s", "打开文件: ", temp, " 失败！ 错误信息： ", strerror(errno), "\n");
		RecordLog(error_info);
		return -1;
	}
	if(ReadLine(fd, temp1) == -1)
	{
		char subStr[2][MAX_SUBSTR];
		cutStrByLabel(temp1, ':', subStr, 2);
		removeChar(subStr[1], '\t');
		strcpy(totalMem, subStr[1]);
		CloseFile(fd);
		return 1;
	}
	else
	{
		char error_info[200];
		sprintf(error_info, "%s%s%s%s%s", "读取文件: ", temp, " 失败！ 错误信息： ", strerror(errno), "\n");
		RecordLog(error_info);
		CloseFile(fd);
		return -1;
	}
}

int getProcessCPUTime(char *stat, Process_Cpu_Occupy_t *processCpuTime)
{
	FILE *fd = OpenFile(stat, "r");
	if(fd == NULL)
	{
		char error_info[200];
		sprintf(error_info, "%s%s%s%s%s", "打开文件: ", stat, " 失败！ 错误信息： ", strerror(errno), "\n");
		RecordLog(error_info);
		return -1;
	}

	char stat_data[1000];
	memset(stat_data, 0, 1000);
	char subStr[18][MAX_SUBSTR];
	if(ReadLine(fd, stat_data) == -1)
	{
		cutStrByLabel(stat_data, ' ', subStr, 18);
		processCpuTime->pid = ExtractNumFromStr(subStr[12]);
		processCpuTime->utime = ExtractNumFromStr(subStr[13]);
		processCpuTime->stime = ExtractNumFromStr(subStr[14]);
		processCpuTime->cutime = ExtractNumFromStr(subStr[15]);
		processCpuTime->cstime = ExtractNumFromStr(subStr[16]);
		CloseFile(fd);
		return 1;
	}
	else
	{
		char error_info[200];
		sprintf(error_info, "%s%s%s%s%s", "读取文件: ", stat, " 失败！ 错误信息： ", strerror(errno), "\n");
		RecordLog(error_info);
		CloseFile(fd);
		return -1;
	}
}

int getTotalCPUTime(Total_Cpu_Occupy_t *totalCpuTime)
{
	char temp[FILE_PATH_MAX_LENGTH], temp1[LINE_CHAR_MAX_NUM];
	memset(temp, 0, FILE_PATH_MAX_LENGTH);
	memset(temp1, 0, LINE_CHAR_MAX_NUM);
	strcpy(temp, "/proc/stat");
	FILE *fd = OpenFile(temp, "r");
	if(fd == NULL)
	{
		char error_info[200];
		sprintf(error_info, "%s%s%s%s%s", "打开文件: ", temp, " 失败！ 错误信息： ", strerror(errno), "\n");
		RecordLog(error_info);
		return -1;
	}
	if(ReadLine(fd, temp1) == -1)
	{
		char name[30];
		sscanf(temp1, "%s %u %u %u %u", name, &totalCpuTime->user, &totalCpuTime->nice, &totalCpuTime->system, &totalCpuTime->idle);
		CloseFile(fd);
		return 1;
	}
	else
	{
		char error_info[200];
		sprintf(error_info, "%s%s%s%s%s", "读取文件: ", temp, " 失败！ 错误信息： ", strerror(errno), "\n");
		RecordLog(error_info);
		CloseFile(fd);
		return -1;
	}
}
