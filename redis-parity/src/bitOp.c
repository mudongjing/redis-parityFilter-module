//
// Created by dongmu on 21.12.6.
//

#include "bitOp.h"


#define CHARSIZE 8





// 给位图增高
static void expanBitMap(bitMap bitmap,int targetLayer){
    int len=strlen(bitmap.oneChar[0]);
    while (bitmap.effcLen<=targetLayer){
        char* p=RedisModule_Calloc(1,sizeof(len*sizeof(char)));
        bitmap.oneChar[bitmap.effcLen++]=p;
    }
}
// 给过滤器中的位图增高
int expanPfEntry(int targetLayer,pfEntry pf){
    if (targetLayer>=pf.high) return REDISMODULE_ERR;
    if (pf.effLen<=targetLayer){
        expanBitMap(pf.mainBitMap,targetLayer);
        expanBitMap(pf.pureBitMap,targetLayer);
        expanBitMap(pf.stopBitMap,targetLayer);
        expanBitMap(pf.lastBitMap,targetLayer);
        pf.effLen=targetLayer+1;
        return REDISMODULE_OK;
    }
}

void setCharBitValue(char* s,int i,int j,bool v){
    char temp;
    if (v==true) temp = 1 << j;
    else temp = ~(1 << j);
    s[i] = s[i] & temp;
}
bool getCharBitValue(char c,int index){
    char temp = 1 << index;
    if ((c & temp) == 0) return false;
    else return true;
}
void setTrueOrFalseOnChar(int pos,char* string,bool v){
    int i=(int) pos/CHARSIZE;
    int j=(int) pos % CHARSIZE;
    setCharBitValue(string,i,j,v);
}

bool getValueFromChar(int pos,char* string){
    int i=(int) pos/CHARSIZE;
    int j=(int) pos % CHARSIZE;
    getCharBitValue(string[i],j);
}

void setTrueOrFalse(int layer,int index,bitMap bitmap,bool v ){
    int i = (int) index / CHARSIZE;
    char* s = bitmap.oneChar[layer];
    int j = index % CHARSIZE;
    setCharBitValue(s,i,j,v);
}

bool getValue(int layer,int index,bitMap bitmap){
    int i = (int) index / CHARSIZE;
    char* s = bitmap.oneChar[layer];
    int j = index % CHARSIZE;
    return getCharBitValue(s[i],j);
}

















//    if (layer>=pf.high){
//        judgeAndRes *res=RedisModule_Calloc(1,sizeof(*res));
//        res->judge=false;
//        res->descrip="The stop point of the element exceeded the limit";
//        return *res;
//    }
//    if (pf.effLen<=layer){expanPfEntry(layer,pf);   }
//    if (layer>=bitmap.effcLen){
//        judgeAndRes *res=RedisModule_Calloc(1,sizeof(*res));
//        res->judge=false;
//        res->descrip="the layer exceeded the limit";
//        return *res;
//    }