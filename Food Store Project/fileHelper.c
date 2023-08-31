#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MyMacros.h"
#include "fileHelper.h"
#include "General.h"


//write:
int	  writeStringToFile(const char* str, FILE* fp,const char* msg)
{
	int length = (int)strlen(str);
	length++;
	if (!writeIntToFile(length, fp,msg))
		return 0;

	if (!writeCharsToFile(str, length, fp, msg))
		return 0;


	return 1;
}
int	  writeCharsToFile(const char* arr, int size, FILE* fp, const char* msg)
{
	if (fwrite(arr, sizeof(char), size, fp) != size)
	{
		puts(msg);
		return 0;
	}
	return 1;

}
int	  writeIntToFile(int val, FILE* fp, const char* msg)
{
	if (fwrite(&val, sizeof(int), 1, fp) != 1)
	{
		puts(msg);
		return 0;
	}
	return 1;
}


//read:
char* readStringFromFile(FILE* fp, const char* msg)
{
	char* str;
	int length;
	
	//read string's length:
	if (!readIntFromFile(&length, fp,msg))
		return NULL;
	
	//allocate space for string:
	str = (char*)malloc((length) * sizeof(char));
	if (!str)
	{
		puts(msg);
		return NULL;
	}
	
	//read the actual string:
	if (fread(str, sizeof(char), length, fp) != length)
	{
		free(str); // free the space allocated
		puts(msg);
		return NULL;
	}
	return str;
}
char* readDynStringFromTextFile(FILE* fp)
{
	char temp[MAX_STR_LEN];
	myGets(temp, sizeof(temp), fp);
	return getDynStr(temp);
}
int	  readFixSizeStrFromFile(char* arr,FILE* fp, const char* msg)
{
	int len;
	if (!readIntFromFile(&len, fp, msg))
		return 0;
	
	if (!readCharsFromFile(arr, len, fp, msg))
		return 0;

	return 1;
}

int	  readCharsFromFile(char* arr, int length, FILE* fp, const char* msg)
{
	if (fread(arr, sizeof(char), length, fp) != length)
	{
		puts(msg);
		return 0;
	}
	return 1;
}
int	  readIntFromFile(int* pVal, FILE* fp, const char* msg)
{
	if (fread(pVal, sizeof(int), 1, fp) != 1)
	{
		puts(msg);
		return 0;
	}
	return 1;

}

