#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAXN 1024+10
char url [MAXN] = "http://www.google.com:80/wiki/Search?search=train&go=Go#steammachine"; 

int main()
{
	const char *parseptr1;
	const char *parseptr2;
	int len;
	int i;
	parseptr2 = url;
	parseptr1 = strchr(parseptr2, ':');
	if ( NULL == parseptr1 ) {
		printf("URL����!\n");
		return 0;
	}
	len = parseptr1 - parseptr2;
	for ( i = 0; i < len; i++ ) {
		if ( !isalpha(parseptr2[i]) ) {
			printf("URL����!\n");
			return 0;
		}
	}
	printf("protocol: ");
	for(i=0;i<len;i++)
		printf("%c",parseptr2[i]);
	printf("\n");//����Э��
	parseptr1++;
	parseptr2 = parseptr1;
	for ( i = 0; i < 2; i++ ) {
		if ( '/' != *parseptr2 ) {
			printf("URL����!\n");
			return 0;
		}
		parseptr2++;
	}
	parseptr1 = strchr(parseptr2, ':');
	if ( NULL == parseptr1 )//�ж����޶˿ں�
	{
		parseptr1 = strchr(parseptr2, '/');
		if ( NULL == parseptr1 ) {
			printf("URL����!\n");
			return 0;}
		len = parseptr1 - parseptr2;
		printf("host: ");
		for(i=0;i<len;i++)
			printf("%c",parseptr2[i]);
		printf("\n");//��������
	}
	else{
		len = parseptr1 - parseptr2;
		printf("host: ");
		for(i=0;i<len;i++)
			printf("%c",parseptr2[i]);
		printf("\n");
		parseptr1++;
		parseptr2 = parseptr1;
		parseptr1 = strchr(parseptr2, '/');
		if ( NULL == parseptr1 ) {
			printf("URL����!\n");
			return 0;
		}
		len = parseptr1 - parseptr2;
		printf("port: ");
		for(i=0;i<len;i++)
			printf("%d",(parseptr2[i]-48));
		printf("\n");//�����˿�
	}
	parseptr1++;
	parseptr2 = parseptr1;
	while ( '\0' != *parseptr1 && '?' != *parseptr1  && '#' != *parseptr1 ) {
		parseptr1++;
	}
	len = parseptr1 - parseptr2;
	printf("path: ");
	for(i=0;i<len;i++)
		printf("%c",parseptr2[i]);
	printf("\n");//����·��
	parseptr2=parseptr1;
	if ( '?' == *parseptr1 ) {
		parseptr2++;
		parseptr1 = parseptr2;
		while ( '\0' != *parseptr1 && '#' != *parseptr1 ) {
			parseptr1++;
		}
		len = parseptr1 - parseptr2;
		printf("query: ");
		for(i=0;i<len;i++)
			printf("%c",parseptr2[i]);//�ж�����ѯ�ʲ�����
		printf("\n");
	}
	parseptr2=parseptr1;
	if ( '#' == *parseptr1 ) {
		parseptr2++;
		parseptr1 = parseptr2;
		while ( '\0' != *parseptr1 ) {
			parseptr1++;
		}
		len = parseptr1 - parseptr2;
		printf("fragment: ");
		for(i=0;i<len;i++)
			printf("%c",parseptr2[i]);
		printf("\n");//�ж�����Ƭ�β�����

	}
	getchar();
}