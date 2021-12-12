//
// Created by dongmu on 21.12.8.
//
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "../redismodule.h"
#ifndef REDIS_PARITY_QUEUE_H
#define REDIS_PARITY_QUEUE_H

#endif //REDIS_PARITY_QUEUE_H

#include <stdio.h>
#include <stdlib.h>

#define ElementType int
#define ERROR -99 //ElementType的特殊值，标志错误

typedef struct Node {
    ElementType data;
    struct Node* next;
}QNode;

typedef struct {
    int size;
    int len;
    QNode* front; //指向对头节点
    QNode* rear; //指向队尾节点
}Queue;

Queue* CreateQueue(int size) ;

void AddQ(Queue* q, ElementType item) ;

int IsEmptyQ(Queue* q);

ElementType DeleteQ(Queue* q);//删除头元素

void ShowQueue(Queue* q) ;
