//
// Created by dongmu on 21.12.12.
//

#include "../redismodule.h"
#include "../assist/base.h"

#ifndef REDIS_PARITY_PERSIST_H
#define REDIS_PARITY_PERSIST_H

#endif //REDIS_PARITY_PERSIST_H

static void *PFRdbLoad(RedisModuleIO *io, int encver);
static void PFRdbSave(RedisModuleIO *io, void *obj);

static void PFAofRewrite(RedisModuleIO *aof, RedisModuleString *key, void *value);

static size_t PFMemUsage(const void *value);

typedef struct __attribute__((packed)){
    uint32_t effcLen;
    char* *oneChar;
}packBitMap;
typedef struct __attribute__((packed)){
    uint32_t weight;//指定的宽度
    char* mainBitMap;
    char* pureBitMap;
    char* stopBitMap;
    char* lastBitMap;
}packSliceBitMap;
typedef struct __attribute__((packed)){
    uint32_t layer;//对应的层数
    uint32_t len; // 副本数量
    uint32_t lastWeight; // last bitMap对应的宽度 ，主要是后面的编程用的频繁，就额外定义
    packSliceBitMap** slice;
}packSliceBitMapArray;
typedef struct __attribute__((packed)){
    uint32_t initLayer;//负责的最低层数
    uint32_t len;//包含的层数
    packSliceBitMapArray** sliceArray;//不同层对应的副本组的指针数组
}packSliceChain;
typedef struct __attribute__((packed)) {
    bool isBigData;// 如果是大数据情况就为true
    uint16_t  conflictP; // 实际的计算前缀数量
    uint32_t layerStartStop;
    uint32_t high;//指定的最大高度
    uint32_t weight;//指定的宽度
    uint32_t effLen;//当前的有效高度
    packBitMap mainBitMap;
    packBitMap pureBitMap;
    packBitMap stopBitMap;
    packBitMap lastBitMap;//宽度是其它位图的两倍
    packSliceChain* slicechain;//副本链
} pfChainHeader;