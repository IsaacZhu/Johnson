//斐波那契堆
//ZJR 17.12.29

#pragma once

#ifndef _LIMITS_H
#include <limits.h>
#endif

#ifndef _STDLIB_H
#include <stdlib.h>
#endif

#ifndef _STRING_H
#include <string.h>
#endif

#ifndef _FIBHEAP_H
#define _FIBHEAP_H
#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

//斐波那契堆结点
typedef struct HeapNode{
    int degree;
    struct HeapNode *p;
    struct HeapNode *child;
    struct HeapNode *left;
    struct HeapNode *right;
    int mark;
    int key;
    int info;   //额外的信息
}HeapNode;

//斐波那契堆
typedef struct FibHeap{
    int n;
    HeapNode* min;
    HeapNode *rootlist; //根链表
}FibHeap;

void PrintFibHeap(FibHeap *H);

//初始化一个堆
void MakeHeap(FibHeap *H){
    H->n = 0;
    H->min = NULL;
    H->rootlist = NULL;
}//MakeHeap

//将一个新元素插入到H的根链表中去
void FibHeapInsert(FibHeap *H, int key, int info){
    HeapNode *x, *p;
    x = (HeapNode *)malloc(sizeof(HeapNode));
    x->degree = 0;
    x->p = NULL;
    x->child = NULL;
    x->mark = FALSE;
    x->key = key;
    x->left = NULL;
    x->right = NULL;
    x->info = info;
    if (H->min == NULL){ //空堆
        H->rootlist = x;
        x->left = x;
        x->right = x;
        H->min = x;
    }
    else{   //否则，将x插入根链表
        p = H->rootlist;
        //插入根节点右边
        x->right = p->right;
        x->left = p;
        p->right = x;
        x->right->left = x;
        if (x->key < H->min->key){ //新的最小结点
            H->min = x;
        }
    }//else
    H->n++;
}//FibHeapInsert

//把一个结点（及其子树）插入到根链表
void FibHeapInsertToRootList(FibHeap *H, HeapNode *x){
    HeapNode *p;
    if (H->min == NULL){ //空堆
        H->rootlist = x;
        x->left = x;
        x->right = x;
        H->min = x;
    }
    else{   //否则，将x插入根链表
        p = H->rootlist;
        //插入根节点右边
        x->right = p->right;
        x->left = p;
        p->right = x;
        x->right->left = x;
        if (x->key < H->min->key){ //新的最小结点
            H->min = x;
        }
    }//else
    H->n += (x->degree + 1);
}//FibHeapInsertToRootList

//判断斐波那契堆是否为空
int isFibHeapEmpty(FibHeap *H){
    if (H->n == 0) return 1;
    else return 0;
}//isFibHeapEmpty

//把一个结点从链表中移除（其后代也会被一并移除）
void FibHeapRemoveFromList(FibHeap *H, HeapNode *x){
    HeapNode *y, *p;
    if (x == x->right){ //只有一个结点
        x->p->child = NULL;
        if (x->p != NULL) {
           p = x->p;
           while (p != NULL){   //把x的祖先结点degree值都相应地减少
               p->degree -= (x->degree + 1);
               p = p->p;
           }
           x->p = NULL;
       }
       if (H->rootlist == x) H->rootlist = NULL;
       if (H->min == x) H->min = NULL;
    }
    else{   //多个结点
        y = x->right;
        x->left->right = y;
        y->left = x->left;
        if (x->p != NULL && x->p->child == x){  //修改父亲的儿子指针
            x->p->child = y;
        }
        if (x->p != NULL) {
           p = x->p;
           while (p != NULL){   //把x的祖先结点degree值都相应地减少
               p->degree -= (x->degree + 1);
               p = p->p;
           }
           x->p = NULL;
        }
        if (H->rootlist == x) H->rootlist = y;
        if (H->min == x){   //x是最小，则需要在根链表重新查找一个作为min
            H->min = y;
            p = y->right;
            while (p != y){
                if (p->key < H->min->key) H->min = p;
                p = p->right;
            }//while
        }//if
    }//else
    H->n -= x->degree;  //减去x的孩子数
    H->n --;            //减去x
}//FibHeapRemoveFromList

//把y变成x的孩子
void FibHeapLink(FibHeap *H, HeapNode *y, HeapNode *x){
    HeapNode *p;
    FibHeapRemoveFromList(H,y); //移除y
    //把y加入x的孩子链表中去
    if (x->child == NULL){  //x还没有孩子，y将是第一个
        x->child = y;
        y->left = y;
        y->right = y;
        y->p = x;
    }
    else{   //x已经有孩子，插入到x儿子的右边即可
        y->right = x->child->right;
        x->child->right = y;
        y->left = x->child;
        y->right->left = y;
        y->p = x;
    }
    x->degree++; 
    x->degree+=y->degree;
    p = x->p;
    while (p != NULL){  //把x的祖先结点的degree相应地修改
        p->degree += (y->degree + 1);
        p = p->p;
    }//while
    H->n += (y->degree + 1);    //加上y的孩子数及y
    y->mark = FALSE;
}//FibHeapLink

//合并H根链表
void CONSOLIDATE(FibHeap *H){
    HeapNode *w, *x, *start, *y, *tmp, *r;
    int d,i;
    int nodenum = H->n;
    HeapNode* A[nodenum*nodenum]; //这里应该是A[D(n)]，但D(n)只知道是O(lgn)，不容易确定，干脆取大一点
    memset(A,0,nodenum*nodenum*sizeof(HeapNode *));
    
    start = H->rootlist;
    if (start != NULL) w = H->rootlist->right;
    else w = NULL;

    while (w != start){ //检索H根链表中的每个结点
        x = w;
        r = w->right;   //提前保存w->right
        d = x->degree;
        while (A[d] != NULL){   //另有结点与x度数相同
            y = A[d];
            if (x->key > y->key){   //交换x y
                tmp = x;
                x = y;
                y = tmp;
            }
            FibHeapLink(H,y,x); //把y变成x的儿子
            A[d] = NULL;    //重新置A[d]为空
            d = x->degree;  //我觉得书中这个地方是有问题的 d就应该是degree
        }//while
        A[d] = x;
        w = r;
    }//while
    //处理start
    if (start != NULL){
        x = start;
        d = x->degree;
        while (A[d] != NULL){   //另有结点与x度数相同
            y = A[d];
            if (x->key > y->key){   //交换x y 此处交换key即可
                tmp = x;
                x = y;
                y = tmp;
            }
            FibHeapLink(H,y,x); //把y变成x的儿子
            A[d] = NULL;    //重新置A[d]为空
            d = x->degree;
        }//while
        A[d] = x;
    }//if

    H->min = NULL;
    //处理剩余的度数唯一的结点
    for (i=0;i<nodenum*nodenum;++i){
        if (A[i] != NULL){
            if (H->min == NULL){    //H->min仍为空，即现在无最小
                H->rootlist = A[i]; //建新堆
                A[i]->left = A[i];
                A[i]->right = A[i];
                A[i]->p = NULL;
                H->n = A[i]->degree + 1;
                H->min = A[i];
            }//if
            else{   //否则，把A[i]插入根链表
                y = H->rootlist;
                //插入根节点右边
                A[i]->right = y->right;
                A[i]->left = y;
                y->right = A[i];
                A[i]->right->left = A[i];
                H->n += (A[i]->degree + 1);
                if (A[i]->key < H->min->key){ //新的最小结点
                    H->min = A[i];
                }
            }//else
        }//if
    }//for
}//CONSOLIDATE

//把Fibonacci堆中最小的结点删除并将其返回
HeapNode* FibHeapExtractMin(FibHeap *H){
    HeapNode *z, *p, *q, *r;
    z = H->min;
    if (z != NULL){ //堆不空,需要调整
        p = z->child;
        if (p != NULL){
            q = p->right;
            while (q != p){ //把孩子结点全部插入根链表
                r = q->right;
                FibHeapRemoveFromList(H,q);
                FibHeapInsertToRootList(H,q);
                q = r;
            }
            FibHeapRemoveFromList(H,p);
            FibHeapInsertToRootList(H,p);
        }//if p!=NULL
        //删除z
        if (z->right != z){ 
            z->right->left = z->left;
            z->left->right = z->right;
            H->min = z->right;
            if (z->p != NULL && z->p->child == z) z->p->child = z->right;
            if (z->p != NULL){  //把z祖先结点的degree值都相应减少
                q = z->p;
                while (q != NULL){
                    q->degree -= (z->degree + 1);
                    q = q->p;
                }
            }
            if (H->rootlist == z) H->rootlist = z->right;
            CONSOLIDATE(H); //调整
        }
        else{   //该链表只有z
            if (z->p != NULL) z->p->child = NULL;
            if (z->p != NULL){  //把z祖先结点的degree值都相应减少
                q = z->p;
                while (q != NULL){
                    q->degree -= (z->degree + 1);
                    q = q->p;
                }
            }
            H->min = NULL;   
            H->n = 0;
            if (H->rootlist == z) H->rootlist = NULL;
            CONSOLIDATE(H);
        }//else
    }//if
    return z;
}//FibHeapExtractMin

//将一个结点x（及其子树）从y的孩子中删除并挂到根链表中
void FibHeapCut(FibHeap *H, HeapNode *x, HeapNode *y){
    HeapNode *p;
    FibHeapRemoveFromList(H,x); //将x移除
    
    //将x 挂到根链表
    if (H->min == NULL){ //空堆
        H->rootlist = x;
        x->left = x;
        x->right = x;
        H->min = x;
    }
    else{   //否则，将x插入根链表
        p = H->rootlist;
        //插入根节点右边
        x->right = p->right;
        x->left = p;
        p->right = x;
        x->right->left = x;
        if (x->key < H->min->key){ //新的最小结点
            H->min = x;
        }
    }//else
    H->n += (x->degree + 1);    //加上x及其孩子的数目
}//FibHeapCut

//级联切断
void FibHeapCascadingCut(FibHeap *H, HeapNode *y){
    HeapNode *z;
    z = y->p;
    if (z != NULL){
        if (y->mark == FALSE){  //y还没被cut过 
            y->mark = TRUE;
        }
        else{
            FibHeapCut(H,y,z);  //将y从z的儿子中移除
            FibHeapCascadingCut(H,z);//递归地切断
        }
    }//if
}//FibHeapCascadingCut

//将一个结点x的值降到k
//目标是调整后仍为小根堆
void FibHeapDecreseKey(FibHeap *H, HeapNode *x, int k){
    if (k > x->key) return; //有误
    HeapNode *y;
    x->key = k;
    y = x->p;
    if (y != NULL && x->key < y->key){  //x降得太多，不应该出现在子结点
        FibHeapCut(H,x,y);
        FibHeapCascadingCut(H,y);
    }
    if (x->key < H->min->key){  //调整后的key比最小的还要小
        H->min = x;
    }
}//FibHeapDecreaseKey

//根据信息匹配链表中结点 这是个递归的过程
HeapNode* HeapListLookFor(HeapNode *list, int info){
    if (list == NULL) return NULL;
    if (list->info == info) return list;    //匹配成功
    HeapNode *x, *y;
    x = list->right;
    while (x != list){
        if (x->info == info) return x;  //检查x
        if (x->child != NULL){  //DFS
            y = HeapListLookFor(x->child,info);
            if (y != NULL) return y;
        }
        x = x->right;
    }
    //遍历list的子堆
    y = HeapListLookFor(list->child,info);
    if (y != NULL) return y;
    return NULL;
}//HeapListLookFor

//根据信息(info)来匹配堆中结点
HeapNode* FibHeapLookFor(FibHeap *H, int info){
    HeapNode *x;
    x = H->rootlist;
    return HeapListLookFor(x, info);
}//FibHeapLookFor

void PrintFibHeapOneNode(HeapNode *x){
    printf("key:%d info:%d degree:%d leftkey:%d leftinfo:%d rightkey:%d rightinfo:%d\n",
    x->key,x->info,x->degree,x->left->key,x->left->info,x->right->key,x->right->info);
    printf("pkey:%d pinfo:%d childkey:%d childinfo:%d\n\n",
    x->p==NULL?0:x->p->key,x->p==NULL?0:x->p->info,
    x->child==NULL?0:x->child->key,x->child==NULL?0:x->child->info);
}//PrintFibHeapOneNode

//递归地打印信息(DFS)
void PrintFibHeapNode(HeapNode *x){
    if (x == NULL) return;
    HeapNode *y;
    PrintFibHeapOneNode(x);
    if (x->child != NULL) PrintFibHeapNode(x->child);
    y = x->right;
    while (y != x){ //DFS 过程
        PrintFibHeapOneNode(y);
        if (y->child != NULL) PrintFibHeapNode(y->child);
        y = y->right;
    }
}//PrintFibHeapNode

//打印Fibonacci堆
void PrintFibHeap(FibHeap *H){
    HeapNode *x;
    x = H->rootlist;
    printf("start print FibHeap info...*****************\n");
    printf("H.min: info:%d key:%d\n",H->min==NULL?0:H->min->info,
    H->min==NULL?0:H->min->key);
    printf("H.n: %d\n",H->n);
    PrintFibHeapNode(x);
    printf("End print FibHeap info...********************\n\n");
}//PrintFibHeap