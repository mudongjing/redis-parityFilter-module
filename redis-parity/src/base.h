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
    char* oneChar[];//字符数组指针数组
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
    uint32_t lastWeight; // last bitMap对应的特定宽度
    sliceBitMap* slice[];
}sliceBitMapArray;

// 副本链的作用是，存储几个低层对应的sliceBitMapArray
typedef struct sliceChain{
    uint32_t initLayer;//负责的最低层数
    uint32_t len;//包含的层数
    sliceBitMapArray* sliceArray[];//不同层对应的副本组的指针数组
}sliceChain;


/**
 * 记录当前过滤器负责的冲突概率，负责的最大元素数量，以及对应的层高和宽度。
 */
typedef struct pfEntry{
    bool isBigData;// 如果是大数据情况就为true
    //最大元素数量，是对应2的次幂，从7开始，加maxEle，最多支持2^70
    uint32_t maxEle;
    uint16_t  conflictP; // 实际的计算前缀数量
    uint32_t layerStartStop;
    // 是相对于设置的最大元素数量，再提升的2的次幂。
    // 比如，元素数量设置了0，就是2^7,128个元素，那么，冲突概率设置为1，就是冲突概率为1/2^8，即1/256.
    // 这样的设置，方便用户判断自己想要的结果。
    // 比如用户设置了 最大的元素数量为1亿，那么最基础的条件就是冲突概率起码药师1/一亿，在此基础上，为了减小冲突概率，
    // 用户可以选择提升多少倍，所以，这里设置的冲突概率是在最基础的条件下，让用户确定安全的倍数，而不是去自己计算需要的冲突概率

    sliceChain* slicechain;//负责在需要压低成本时使用

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