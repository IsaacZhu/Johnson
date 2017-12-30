#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include "timecount.h"
#include "FibHeap.h"

#define MAXSIZE 256
#define INF 65536

typedef struct graph{
    int adj[MAXSIZE+2][MAXSIZE+2];  //邻接矩阵
    int vertexnum;              //结点数
}graph;

graph G;
int h[MAXSIZE+2];
int prior[MAXSIZE+2];

void buildgraph(graph *G, int nodenum, int arcnum, int *arcfrom, int *arcto, int *arcweigh){
    int i,j;
    G->vertexnum = nodenum;
    //初始化所有边权值为65536
    for (i=1;i<=nodenum;++i){
        for (j=1;j<=nodenum;++j){
            G->adj[i][j] = INF;
        }
    }
    //将权值赋到邻接矩阵
    for (i=1;i<=arcnum;++i){
        if (arcfrom[i] != arcto[i]){    //去除从自己到自己的边
            G->adj[arcfrom[i]][arcto[i]] = arcweigh[i];
        }
    }
}//buildgraph

//初始化
void InitializeSingleSource(graph *G, int s,int *d,int *prior){
    //memset(d,INF,(MAXSIZE+2)*sizeof(int));      //用65536来代表无穷
    //memset(prior,0,(MAXSIZE+2)*sizeof(int));    //前驱则设为0
    int i;
    for (i=1;i<=G->vertexnum;++i){
        d[i] = INF;
        prior[i] = 0;
    }
    d[s] = 0;
}

//用u松弛v
//返回1表示发生了松弛，0表示未发生松弛
int Relax(graph *G, int u, int v, int *d, int *prior){
    if (d[v] > d[u]+G->adj[u][v]){
        d[v] = d[u]+G->adj[u][v];
        prior[v] = u;
        return d[v];
    }
    else if (d[v] == d[u]+G->adj[u][v]){    //相等，如果还没有前驱，则添加为前驱 否则不需要
        if (prior[v] == 0) prior[v] = u;
    }
    return 0;
}

//Bellman-Ford算法
//输入：图G,源s,距离数组d,前驱数组prior
//输出：0代表失败，说明存在负回路    1表示成功,d即为距离
int BellmanFord(graph *G, int s, int *d, int *prior){
    int i,j,k,nodenum;
    nodenum = G->vertexnum;
    InitializeSingleSource(G,s,d,prior);    //初始化
    //对所有边做N-1次relax
    for (i=1;i<nodenum;++i){
        for (j=1;j<=nodenum;++j){
            for (k=1;k<=nodenum;++k){
                if (G->adj[j][k] != INF){ //边存在
                    Relax(G,j,k,d,prior);
                }//if
            }//for k
        }//for j
    }//for i

    //tc
    /*
    for (i=1;i<=nodenum;++i){
        for (j=1;j<=nodenum;++j){
            printf("%d ",G->adj[i][j]);
        }
        printf("\n");
    }
    exit(0);*/
    //for (i=1;i<=nodenum;++i) printf("%d ",d[i]);//tc
    //printf("\n");
    //for (i=1;i<=nodenum;++i) printf("%d ",prior[i]);//tc
    //printf("\n");

    //检验
    for (j=1;j<=nodenum;++j){
        for (k=1;k<=nodenum;++k){
            if (G->adj[j][k] != INF){ //边存在
                if (d[k] > d[j]+G->adj[j][k]){  //失败
                    return 0;
                }
            }//if
        }//for k
    }//for j

    return 1;
}//BellmanFord

//判断一个元素key是否在集合set中，是则返回1，否则返回0
int inset(int *set, int setsize, int key){
    int i;
    for (i=0;i<setsize;++i){
        if (set[i] == key) return 1;
    }
    return 0;
}//inset

//Dijkstra算法
void Dijkstra(graph *G, int s, int *d, int *prior){
    int i, nodenum, u, set[MAXSIZE+2], setsize, k;
    HeapNode *x;
    setsize = 0;
    nodenum = G->vertexnum;

    memset(set,0,(MAXSIZE+2)*sizeof(int));
    InitializeSingleSource(G,s,d,prior);    //初始化

    FibHeap H;//建一个Fibonacci堆
    MakeHeap(&H);

    //把每个点加入到Fibonacci堆 key值是d[i]
    for (i=1;i<=nodenum;++i){
        FibHeapInsert(&H,d[i],i);
    }

    //循环 贪心地求解s到各点距离
    while (isFibHeapEmpty(&H) == 0){
        x = FibHeapExtractMin(&H);
        if (x == NULL){
            printf("error\n");
            exit(0);
        }
        u = x->info;
        set[setsize++] = u;
        for (i=1;i<=nodenum;++i){
            if (G->adj[u][i] != INF && !inset(set,setsize,i)){   //边存在
                k = Relax(G,u,i,d,prior);//松弛
                if(k != 0){     //如果发生了松弛
                    x = FibHeapLookFor(&H, i);
                    if (x == NULL){ 
                        printf("error\n");
                        exit(0);
                    }
                    FibHeapDecreseKey(&H,x,k);   //修改i的key值
                }   
            }//if
        }//for
    }//while
}//Dijkstra

void Johnson(graph *G){
    graph *Gp = (graph *)malloc(sizeof(graph));
    int i,j,s,d[MAXSIZE+2];
    int nodenum = G->vertexnum;
    memcpy(Gp->adj,G->adj,(MAXSIZE+2)*(MAXSIZE+2)*sizeof(int));
    //计算加入s的G'
    s = nodenum + 1;
    Gp->vertexnum = nodenum + 1;
    for (i=1;i<=s;++i){   //w(s,u) = 0
        Gp->adj[s][i] = 0;
        Gp->adj[i][s] = INF;    //w(u,s) = 无穷 
    }
    Gp->adj[s][s] = 0;

    //对G'及s 计算Bellman-Ford 并设置h(v)
    if (BellmanFord(Gp,s,h,prior) == 0){
        printf("图中有负权回路\n");
        exit(0);
    }

    //更新w'(u,v)=w(u,v)+h(u)-h(v)
    for (i=1;i<=nodenum;++i){
        for (j=1;j<=nodenum;++j){
            if (G->adj[i][j] != INF){   //边存在,更新权值
                G->adj[i][j] += (h[i] - h[j]);
            }
        }//for j
    }//for i
    
    
    //对每个点，执行dijkstra
    for (i=1;i<=nodenum;++i){
        Dijkstra(G,i,d,prior);
    }//for

}//Johnson

int main(){
	int i,j,arcnum;
    arcnum = MAXSIZE*log2(MAXSIZE);
    int arcfrom[arcnum],arcto[arcnum],arcweigh[arcnum];
    int scale[7] = {0,8,16,32,64,128,256};
    char inputpath[100];
    char numstr[5];
    char inputfilename[100] = "/input.txt";
    char outputpath[100];
    char timefilename[100] = "/time2.txt";
    char resultpath[100];
    char resultfilename[100] = "/output2.txt";

	//read arcs
	FILE *fp,*fpt,*fpr;

	fpt=fopen("time2.txt","w+");
	fprintf(fpt,"********result*********\n");

	//calculate
	printf("**************CALCULATION START...****************\n");
	for (i=1;i<=6;++i){
        strcpy(inputpath,"../Input/size");
		numstr[0]='0'+i;
        numstr[1]='\0';
        strcat(inputpath,numstr);
        strcat(inputpath,inputfilename);
        fp = fopen(inputpath,"r+");
        
        arcnum = scale[i]*log2(scale[i]);
        //读入弧信息
        for (j=1;j<=arcnum;++j){
            fscanf(fp,"%d,%d:%d",&arcfrom[j],&arcto[j],&arcweigh[j]);
        }

        
        //初始化
        memset(h,0,(MAXSIZE+2)*sizeof(int));
        memset(prior,0,(MAXSIZE+2)*sizeof(int));
        buildgraph(&G,scale[i],arcnum,arcfrom,arcto,arcweigh);

        /*unit test
        int k;
        for (j=1;j<=scale[i];++j){
            for (k=1;k<=scale[i];++k){
                printf("%d ",G.adj[j][k]);
            }
            printf("\n");
        }
        exit(0);*/

        timestart();
        Johnson(&G);
        
        timeend();

		//output result

        strcpy(outputpath,"../Output/size");
		numstr[0]='0'+i;
        numstr[1]='\0';
        strcat(outputpath,numstr);
        strcpy(resultpath,outputpath);
        strcat(outputpath,timefilename);
        fpt = fopen(outputpath,"w+");
        
		//output time to time.txt
		fprintf(fpt,"\nN:%d time is %.8lfs\n",scale[i],returntime());

        //output connect component
        strcat(resultpath,resultfilename);
        fpr = fopen(resultpath,"w+");
       

		//output time to screen
		outputtime();
	}//for
	printf("\n**************CALCULATION END**********************\n");
}