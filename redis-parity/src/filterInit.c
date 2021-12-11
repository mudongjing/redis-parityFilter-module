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
bitMap initBitMap(int high,int pre,int count,bool isBig){
    bitMap *bitmap=RedisModule_Calloc(1,sizeof(bitmap));
    bitmap->oneChar=RedisModule_Calloc(high,sizeof(void*));
    if (isBig==true){
        int tpre=1<<pre;
        for (int i = 0; i < tpre; ++i) {
            bitmap->oneChar[i]=NULL;
        }
        bitmap->oneChar[tpre]=initChar(count);
        bitmap->effcLen=tpre+1;
    }else{
        bitmap->oneChar=RedisModule_Calloc(pre+2,sizeof(void*));
        for (int i = 0; i < pre+2; ++i) {
            bitmap->oneChar[i]=initChar(count);
        }
        bitmap->effcLen=pre+2;
    }

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
    array->slice=RedisModule_Calloc(len,sizeof(void*));
    for (int i = 0; i < len; ++i) {
        array->slice[i]=initSliceMap(weight);
    }
}
sliceChain* initSliceChain(int initLayer,int len,int weight){
    sliceChain *chain=RedisModule_Calloc(1,sizeof(chain));
    chain->initLayer=initLayer;
    chain->len=len;
    chain->sliceArray=RedisModule_Calloc(len,sizeof(void*));
    for (int i = 0; i < len; ++i) {
        chain->sliceArray[i]=initSliceArray(initLayer+i,len,weight);
    }
    return chain;
}
pfEntry*  initPfEntry(int pre,int count,int high,bool isBig){
    pfEntry *pf=RedisModule_Calloc(1,sizeof(pf));
    pf->conflictP=pre;
    pf->weight=count;
    pf->high=high;
    pf->isBigData=isBig;

    pf->mainBitMap=initBitMap(high,pre,count,isBig);
    pf->pureBitMap=initBitMap(high,pre,count,isBig);
    pf->stopBitMap=initBitMap(high,pre,count,isBig);
    pf->lastBitMap=initBitMap(high,pre,count<<1,isBig);
    pf->effLen=pre+2;

    if (isBig==true) {
        pf->layerStartStop=0;
        pf->slicechain=initSliceChain(pre+1,1<<pre,count);
    }
    else {
        pf->layerStartStop=pre+1;
    }

    return pf;
}
//
pfEntry* initFilter(int conflict){
    pfEntry* pf = NULL;
    // 太大或较小都不打算支持
    if (conflict>CONFLICT_LIMIT || conflict<13) return pf;

    pair wpb=getPAndW(conflict);
    pf = initPfEntry(wpb.p,wpb.count,wpb.hight,wpb.copy);

    return pf;
}