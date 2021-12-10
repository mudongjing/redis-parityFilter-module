//
// Created by dongmu on 21.12.9.
//

#include "filterInit.h"

// 如果概率大于这个，就认为当前的结构无法有效保证
#define CONFLICT_LIMIT 42


char* initChar(int weight){
    char* s=RedisModule_Calloc(1,weight);
    memset(s,'\0',weight);
    return s;
}
bitMap initBitMap(int pre,int count){
    bitMap *bitmap=RedisModule_Calloc(1,sizeof(bitmap));
    for (int i = 0; i < pre+2; ++i) {
        char* content=RedisModule_Calloc(1,count);
        memset(content,'\0',count);
        bitmap->oneChar[i]=content;
    }
    bitmap->effcLen=pre+2;
    return *bitmap;
}
sliceBitMap * initSliceMap(int weight){
    sliceBitMap  *map=RedisModule_Calloc(1,sizeof(map));
    map->weight=weight;
    map->mainBitMap=initChar(weight);
    map->pureBitMap=initChar(weight);
    map->stopBitMap=initChar(weight);
    map->lastBitMap=initChar(weight<<1);
    return map;
}
sliceBitMapArray * initSliceArray(int layer,int len,int weight){
    sliceBitMapArray * array=RedisModule_Calloc(1,sizeof(array));
    array->layer=layer;
    array->len=len;
    array->lastWeight=weight<<1;
    for (int i = 0; i < len; ++i) {
        array->slice[i]=initSliceMap(weight);
    }
}
sliceChain initSliceChain(int initLayer,int len,int weight){
    sliceChain *chain=RedisModule_Calloc(1,sizeof(chain));
    chain->initLayer=initLayer;
    chain->len=len;
    for (int i = 0; i < len; ++i) {
        chain->sliceArray[i]=initSliceArray(initLayer+i,len,weight);
    }
    return *chain;
}
pfEntry  initPfEntry(int pre,int count,int high,bool isBig){
    pfEntry *pf=RedisModule_Calloc(1,sizeof(pf));
    pf->conflictP=pre;
    pf->weight=count;
    pf->high=high;
    pf->isBigData=isBig;
    if (isBig==true) pf->layerStartStop=0;
    else pf->layerStartStop=pre+1;

    pf->mainBitMap=initBitMap(pre,count);
    pf->pureBitMap=initBitMap(pre,count);
    pf->stopBitMap=initBitMap(pre,count);
    pf->lastBitMap=initBitMap(pre,count<<1);
    pf->effLen=pre+2;


    return *pf;
}
//
pfEntry initFilter(int conflict){
    pfEntry *pf = NULL;
    // 太大或较小都不打算支持
    if (conflict>CONFLICT_LIMIT || conflict<13) return *pf;
    pair wpb=getPAndW(conflict);
    pf = RedisModule_Calloc(1,sizeof(pf));
    pf->isBigData=false;


    bitMap *main=RedisModule_Calloc(1,sizeof(main));
    bitMap *pure=RedisModule_Calloc(1,sizeof(pure));
    bitMap *stop=RedisModule_Calloc(1,sizeof(stop));
    bitMap *last=RedisModule_Calloc(1,sizeof(last));

    if (conflict>=BIGDATA_SIZE){
        pf->isBigData=true;
    }else{

    }
}