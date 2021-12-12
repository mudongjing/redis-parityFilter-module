//
// Created by dongmu on 21.12.8.
//

#include "queue.h"

Queue* CreateQueue(int size) { // 负数，则没有容量限制
    Queue* q = (Queue*)malloc(sizeof(Queue));
    if (!q) {
        printf("空间不足！\n");
        return NULL;
    }
    q->size=size;
    q->len=0;
    q->front = NULL;
    q->rear = NULL;
    return q;
}

void AddQ(Queue* q, ElementType item) {
    QNode* qNode = (QNode*)malloc(sizeof(QNode));
    if (!qNode) {
        printf("空间不足！\n");
        return;
    }
    qNode->data = item;
    qNode->next = NULL;

    if (q->front == NULL) {
        q->front = qNode;
    }
    else if (q->rear == NULL) {
        q->rear = qNode;// 保存了两个元素
        q->front->next=qNode;
    }
    else {
        q->rear->next = qNode;
        q->rear = qNode;// 添加到队尾
        if (q->len==q->size){//队列已满
            DeleteQ(q);//删除头元素
        }
    }
    q->len++;
}

int IsEmptyQ(Queue* q){
    return (q->front == NULL);
}

ElementType DeleteQ(Queue* q) {
    if (IsEmptyQ(q)) {
        printf("空队列\n");
        return ERROR;
    }
    QNode* temp = q->front;
    ElementType item;
    if (q->len==1) { //若队列只有一个元素
        q->front = NULL;
        q->rear = NULL;
    }
    else {
        q->front = q->front->next;
    }
    q->len--;
    item = temp->data;
    free(temp);
    return item;
}

void ShowQueue(Queue* q) {
    if (IsEmptyQ(q)) {
        printf("空队列\n");
        return;
    }
    printf("打印队列数据元素：\n");
    QNode* qNode = q->front;
    while (qNode != NULL) {
        printf("%d " , qNode->data);
        qNode = qNode->next;
    }
    printf("\n");
}
