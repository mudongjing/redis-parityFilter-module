//
// Created by dongmu on 21.12.6.
//

#include "parityHash.h"
#include "bitOp.h"
// 对给定的字符串计算对应的哈希值,并返回正数
int getHashValue(char data[]){
    const int p = 16777619;
    int hash = (int) 2166136261L;

    for (int i = 0; i < strlen(data); i++) hash = (hash ^ data[i]) * p;
    hash += hash << 13;
    hash ^= hash >>7;
    hash += hash << 3;
    hash ^= hash >> 17;
    hash += hash << 5;
    return abs(hash);
}

char* expanString(char* data){
    int len=sizeof(data)/sizeof(char);
    if (len - strlen(data) < 8){
        char* newdata = RedisModule_Calloc(1,len + 8);
        for (int i = 0; i < len; ++i) {
            newdata[i]=data[i];
        }
        return newdata;
    }else return data;
}

// 在原始的字符串基础上，不断的循环向后添加字符
StrAndIndex * loopChangeString(char original[],char data[],int index){
    StrAndIndex *sat =RedisModule_Calloc(1,sizeof(*sat));
    int len=strlen(original)-1;
    if (data==NULL){
        sat->index=len;
        char* newOriginal=RedisModule_Calloc(1,sizeof(original));
        strcpy(newOriginal,original);
        sat->result=newOriginal;
        return sat;
    }
    if (index>=len) index=0;
    else index++;
    data=expanString(data);
    strcat(data,original[index]);

    sat->index=index;
    sat->result=data;
    return sat;
}

// 发现出现冲突返回true

int calPosQ(Queue *pos,uint64_t weight){
    QNode* node=pos->front;
    int result=0;
    while (node!=NULL){
        result += node->data;
        node=node->next;
    }
    return result % weight;
}

// 给定指定的层数和索引，
// 如果isSet不为NULL，就是按照指定值做操作，否则就值作判断
// 继续的，判断是否是停留点，一样做操作
bool setOrJudgeForPf(int pos,int layer,pfEntry pf,bool isSet,bool isStop,Queue *posQ){
    if (isSet==NULL){//仅作判断
        if (pf.effLen<=layer) return false;
        if (isStop){//是不是一个停留点
            if (getValue(layer,pos,pf.mainBitMap)==true && getValue(layer,pos,pf.stopBitMap)){
                if (posQ!=NULL){
                    uint64_t temp=pf.weight;
                    if (getValue(layer,calPosQ(posQ,temp << 1),pf.lastBitMap)==true) return true;//确实是一个停留点
                }
            }
            return false;//不满足停留点条件
        }else{//是不是一个经历点
            return getValue(layer,pos,pf.mainBitMap);
        }
    }else{
        if (isSet==true){//添加
            if (pf.effLen<=layer) expanPfEntry(layer,pf);
            if (getValue(layer,pos,pf.mainBitMap)==true) setTrueOrFalse(layer,pos,pf.pureBitMap,true);
            setTrueOrFalse(layer,pos,pf.mainBitMap,true);
        }else{//删除
            if (pf.effLen<=layer) return false;
            if (getValue(layer,pos,pf.pureBitMap)!=true) setTrueOrFalse(layer,pos,pf.mainBitMap,false);
        }
        if (isStop==true){
            setTrueOrFalse(layer,pos,pf.stopBitMap,isSet);
            uint64_t  temp=pf.weight;
            setTrueOrFalse(layer,calPosQ(posQ,temp << 1),pf.lastBitMap,isSet);
        }
    }
}

bool judgeSlice(int pos,sliceBitMapArray sliceArray,bool isStop,Queue *posQ){
    int len=sliceArray.len;
    sliceBitMap* slicemap=sliceArray.slice;
    if (isStop==false){
        for (int i = 0; i < len; ++i) {
            if (getValueFromChar(pos,slicemap[i].mainBitMap)==true) return true;//只要有一个存在即可
        }
    }else{
        for (int i = 0; i <len ; ++i) {
            sliceBitMap smap=slicemap[i];
            if (getValueFromChar(pos,smap.mainBitMap)==true && getValueFromChar(pos,smap.stopBitMap)==true){
                if (getValueFromChar(calPosQ(posQ,(uint64_t) sliceArray.lastWeight),smap.lastBitMap)==true){
                    return true;
                }
            }
        }
    }
    return false;
}

// 经历的增删
void setSliceInBestPlace(int pos,sliceBitMapArray sliceArray,bool isSet){
    int len=sliceArray.len;
    sliceBitMap* slicemap=sliceArray.slice;
    if (isSet==true){
        for (int i = 0; i < len; ++i) {
            sliceBitMap bitmap = slicemap[i];
            if (getValueFromChar(pos,bitmap.mainBitMap)==false){//没有过经历
                setTrueOrFalseOnChar(pos,bitmap.mainBitMap,true);
                return;
            }
        }
        // 否则，就找有过两次经历的
        for (int j = 0; j < len; ++j) {
            sliceBitMap bitmap = slicemap[j];
            if (getValueFromChar(pos,bitmap.mainBitMap)==true){
                if (getValueFromChar(pos,bitmap.pureBitMap)==true) return;
            }
        }
        //此时，所有的这个位置的字符串都是有且一次经历
        setTrueOrFalseOnChar(pos,slicemap[0].pureBitMap,true);//直接选择第一个处理
    }else{
        for (int j = 0; j < len; ++j) {
            sliceBitMap bitmap = slicemap[j];
            if (getValueFromChar(pos,bitmap.mainBitMap)==true){
                if (getValueFromChar(pos,bitmap.pureBitMap)==false){
                    setTrueOrFalseOnChar(pos,bitmap.mainBitMap,false);
                    return;
                }
            }
        }
    }

}

// 停留点的增删
bool setSliceStopInBestPlace(int pos,sliceBitMapArray sliceArray,bool isSet,Queue *posQ){
    int len=sliceArray.len;
    sliceBitMap* slicemap=sliceArray.slice;
    if (isSet==false){
        for (int i = 0; i < len; ++i) {
            sliceBitMap bitmap = slicemap[i];
            if (getValueFromChar(pos,bitmap.stopBitMap)==true){
                if (getValueFromChar(calPosQ(posQ,(uint64_t)sliceArray.lastWeight),bitmap.lastBitMap)==true){
                    setTrueOrFalseOnChar(calPosQ(posQ,(uint64_t)sliceArray.lastWeight),bitmap.lastBitMap,false);
                    setTrueOrFalseOnChar(pos,bitmap.stopBitMap,false);
                    if (getValueFromChar(pos,bitmap.pureBitMap)==false) setTrueOrFalseOnChar(pos,bitmap.mainBitMap,false);
                    return true;
                }
            }
        }
    }else{
        for (int i = 0; i < len; ++i) {
            sliceBitMap bitmap = slicemap[i];
            if (getValueFromChar(pos,bitmap.mainBitMap)==false){
                // 未经历过的点
                if (getValueFromChar(calPosQ(posQ,(uint64_t)sliceArray.lastWeight),bitmap.lastBitMap)==false){
                    setTrueOrFalseOnChar(pos,bitmap.mainBitMap,true);
                    setTrueOrFalseOnChar(pos,bitmap.stopBitMap,true);
                    setTrueOrFalseOnChar(calPosQ(posQ,(uint64_t)sliceArray.lastWeight),bitmap.lastBitMap,true);
                    return true;
                }
            }
        }
        //经历过两次，且不为停留点
        for (int j = 0; j < len; ++j) {
            sliceBitMap  bitmap=slicemap[j];
            if (getValueFromChar(pos,bitmap.pureBitMap)==true && getValueFromChar(pos,bitmap.stopBitMap)==false){
                if (getValueFromChar(calPosQ(posQ,(uint64_t)sliceArray.lastWeight),bitmap.lastBitMap)==false){
                    setTrueOrFalseOnChar(pos,bitmap.stopBitMap,true);
                    setTrueOrFalseOnChar(calPosQ(posQ,(uint64_t)sliceArray.lastWeight),bitmap.lastBitMap,true);
                    return  true;
                }
            }
        }
        //只有一次经历，且不为停留点
        for (int k = 0; k < len; ++k) {
            sliceBitMap  bitmap=slicemap[k];
            if (getValueFromChar(pos,bitmap.mainBitMap)==true && getValueFromChar(pos,bitmap.pureBitMap)==false
                        && getValueFromChar(pos,bitmap.stopBitMap)==false){
                if (getValueFromChar(calPosQ(posQ,(uint64_t)sliceArray.lastWeight),bitmap.lastBitMap)==false){
                    setTrueOrFalseOnChar(pos,bitmap.pureBitMap,true);
                    setTrueOrFalseOnChar(pos,bitmap.stopBitMap,true);
                    setTrueOrFalseOnChar(calPosQ(posQ,(uint64_t)sliceArray.lastWeight),bitmap.lastBitMap,true);
                    return  true;
                }
            }
        }
    }
    return false;
}

bool setSlice(int pos,sliceBitMapArray sliceArray,bool isSet,bool isStop,Queue *posQ){
    if (isSet==NULL) return false;
    if (isStop==false) setSliceInBestPlace(pos,sliceArray,isSet);
    else return setSliceStopInBestPlace(pos,sliceArray,isSet,posQ);
    return true;
}
// 针对副本的操作
bool setOrJudgeForSlice(int pos,sliceBitMapArray sliceArray,bool isSet,bool isStop,Queue *posQ){
    if (isSet==NULL){//仅作判断
        return judgeSlice(pos,sliceArray,isStop,posQ);
    }else{//设置
        return setSlice(pos,sliceArray,isSet,isStop,posQ);
    }
}

bool setOrJudge(Queue *pos,pfEntry pf,bool set){

    bool isBig = pf.isBigData;
    int minEffStopLayer = pf.conflictP+1;// 最低的有效停留层
    sliceChain* chain = pf.slicechain;
    int chainSize;
    if(chain!=NULL){
        chainSize=chain->len; // 由于副本的层数
    }
    int initL=0;// 开始判断的层，大数据就变成最低有效
    if (isBig) initL=minEffStopLayer;
    int maxEffStopLayer=minEffStopLayer+chainSize-1;
    if (set==NULL){//表示仅作判断冲突



    }else{//直接上手根据 set值操作

    }
}



bool trySet(Queue *pos,pfEntry pf,bool b){
    // 先判断是否存在冲突
    // 不存在，再做添加，或存在，再做删除
    bool conflict=judgeConflict(pos,pf);
    if (conflict==true && b==false){//出现冲突，删除
        setOrJudge(pos,pf,b);
    }else if(conflict==false && b==true){// 没有冲突，加入
        setOrJudge(pos,pf,b);
    }else{
        return false;
    }
    return true;
}

bool judgeConflict(Queue *pos,pfEntry pf){
    return setOrJudge(pos,pf,NULL);
}
