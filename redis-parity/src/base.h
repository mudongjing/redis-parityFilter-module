//
// Created by dongmu on 21.12.8.
//
#include "stdio.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "redismodule.h"

#ifndef REDIS_PARITY_BASE_H
#define REDIS_PARITY_BASE_H

#endif //REDIS_PARITY_BASE_H


typedef enum
{
    true=1, false=0
}bool;

typedef struct JudgeAndRes{
    bool judge;
    int result;
    char* descrip;
}judgeAndRes;

/**
 * 位图类，结构是二维的字符数组。
 */
typedef struct bitMap{
    uint32_t effcLen;//有效长度，指一维数组，其中的char*还未实际定义，就是实际有效的高度
    // 维度由所属的过滤器决定
    char* *oneChar;//字符数组指针数组
}bitMap;

/**
 * 这里需要特殊指出的一个类，对于一些场景下，我们为了满足最低的冲突概率和较低的空间成本，
 * 希望对过滤器有效的底层，做几份独立的复制件。
 * 通过复制件缓冲一定的冲突，提高总体应对冲突的性能。
 *
 * 结构是一个数组，数组中对应的是多个复制件。
 * 元素是只有一层的位图，但是需要记录完整的4个位图。
 */
typedef struct sliceBitMap{
    uint32_t weight;//指定的宽度
    char* mainBitMap;
    char* pureBitMap;
    char* stopBitMap;
    char* lastBitMap;
}sliceBitMap;

// 存储同一层的多个副本
typedef struct sliceBitMapArray{
    uint32_t layer;//对应的层数
    uint32_t len; // 副本数量
    uint32_t lastWeight; // last bitMap对应的宽度 ，主要是后面的编程用的频繁，就额外定义
    sliceBitMap* *slice;
}sliceBitMapArray;

// 副本链的作用是，存储几个低层对应的sliceBitMapArray
typedef struct sliceChain{
    uint32_t initLayer;//负责的最低层数
    uint32_t len;//包含的层数
    sliceBitMapArray* *sliceArray;//不同层对应的副本组的指针数组
}sliceChain;


/**
 * 记录当前过滤器负责的冲突概率，负责的最大元素数量，以及对应的层高和宽度。
 */
typedef struct pfEntry{
    bool isBigData;// 如果是大数据情况就为true
    uint16_t  conflictP; // 实际的计算前缀数量
    uint32_t layerStartStop;

    sliceChain* slicechain;//副本链

    uint32_t high;//指定的最大高度

    uint32_t weight;//指定的宽度
    uint32_t effLen;//当前的有效高度
    bitMap mainBitMap;
    bitMap pureBitMap;
    bitMap stopBitMap;
    bitMap lastBitMap;//宽度是其它位图的两倍
}pfEntry;


typedef struct StrAndIndex{
    char* result;
    int index;
} StrAndIndex;

typedef struct intListNode{
    int value;
    struct initListNode* next;
}listNodeInt;

typedef struct pair{
    bool copy;
    int p;
    int count;
    int hight;
}pair;
pair getPAndW(int conflict);