/*provide interfaces:
timestart()  to  start timing
timeend()    to  end timing
outputtime() to print the time recorded before
*/

#pragma once

#ifndef _STDIO_H
#include <stdio.h>
# endif

#ifndef _TIME_H
#include <sys/time.h>
#endif

void timestart();
void timeend();
void outputtime();
double timecount1=0;
double timecount2=0;
double timecount3=0;
int tusec1=0;
int tusec2=0;
int tusec3=0;

void timestart(){
	struct timeval tv;
	struct timezone tz;
	gettimeofday(&tv,&tz);
	timecount1=(double)tv.tv_sec+(double)tv.tv_usec/1000000.0;
	tusec1=tv.tv_usec;
}

void timeend(){
	struct timeval tv;
	struct timezone tz;
	gettimeofday(&tv,&tz);
	timecount2=(double)tv.tv_sec+(double)tv.tv_usec/1000000.0;
	tusec2=tv.tv_usec;
}

double returntime(){
	double timecount4;
	timecount4=timecount2-timecount1;
	return timecount4;
}
void outputtime(){
	timecount3=timecount2-timecount1;
	printf("runtime: %.20lfs\n",timecount3);
	tusec3=tusec2-tusec1;
	//printf("usec: %d us\n",tusec3);
	tusec1=0;
	tusec2=0;
	tusec3=0;
	timecount1=0;
	timecount2=0;
	timecount3=0;
	return;
}

