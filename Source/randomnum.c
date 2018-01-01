//生成NlogN个点对（可能有重复）
//生成NlogN个点权值，其中有logN个是负数，其余为正   负数取值：-logN~-1 正数取值：1~N
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define N 8
int main(){
	srand(time(0));
	int i,arcnum,weighlimit;
	arcnum = N*(int)log2(N);
	weighlimit = (int)log2(N);
	int arcfrom[arcnum+1],arcto[arcnum+1],arcweigh[arcnum+1];
	//生成NLogN条弧
	for (i=1;i<=arcnum;++i){
		arcfrom[i] = (int)rand()%N+1;
		arcto[i] = (int)rand()%N+1;
	}
	//生成LogN条负权边
	for (i=1;i<=(int)log2(N);++i){
		arcweigh[i] = -((int)rand()%weighlimit+1);
	}

	//生成(N-1)LogN条正权值边
	for (i=(int)log2(N)+1;i<=arcnum;++i){
		arcweigh[i] = (int)rand()%N+1;
	}

	FILE *fp;
	fp=fopen("../Input/size1/input.txt","w+");
	if (fp == NULL){
		printf("Open file error!\n");
		exit(0);
	}
	
	printf("Making some random numbers...\n");
	for (i=1;i<=arcnum;++i){
		fprintf(fp,"%d,%d:%d\n",arcfrom[i],arcto[i],arcweigh[i]);		
	}
	printf("Success!\n");
	fclose(fp);
}
