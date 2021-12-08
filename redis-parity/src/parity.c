//
// Created by dongmu on 21.12.6.
//

#include "parity.h"
#include "parityHash.h"
#define EXCEDLIMIT "The number of calculated layers exceeds the limit"

typedef struct judgeAndintArr{
    bool succ;
    Queue * pos;
    char* descrip;
}judgeAndintArr;

bool isEven(int value){//判断数值是否为偶数
    if ((value & 1)==1) return false;
    else return true;
}

bool isAllSameBool(Queue *q,bool v){
    if (q->len < q->size) return false; // 为满足数量
    QNode *node=q->front;
    while (node!=NULL){
        if (isEven(node->data)==v) return false;
        node=node->next;
    }
    return true;
}

// 统一计算一个字符串对应的位置信息，可供其它函数方便处理
// 给出的结果，是按层数指定的对应位置
static judgeAndintArr calEleLocal(char* string,int weight,int limit,int pre){
    judgeAndintArr *judge = RedisModule_Calloc(1,sizeof(*judge));
    Queue * pos=CreateQueue(-1);
    int i=1;
    StrAndIndex* strAndIndex = RedisModule_Calloc(1,sizeof(*strAndIndex));
    int ii=getHashValue(string) % weight;//位图位置
    AddQ(pos,ii);
    bool v=isEven(ii);
    Queue * q=CreateQueue(pre);//当前允许的最大浅前缀计算数量
    while (i<limit){
        strAndIndex=loopChangeString(string,strAndIndex->result,strAndIndex->index);
        int j=getHashValue(strAndIndex->result) % weight;
        AddQ(pos,j);//记录位置
        bool vv=isEven(j);
        if (vv==v){
            if (isAllSameBool(q,v)){ break;  } //达到停留条件
        }
        AddQ(q,vv);
        i++;
    }
    if (i>=limit){
        judge->succ=false;
        judge->descrip=EXCEDLIMIT;
    }else{
        judge->succ=true;
        judge->pos=pos;
    }
    return *judge;
}


/**
 * 先判断是否为大数据情况。
 * 每次计算出各层对应的位置后，先记录，当发现出现冲突就直接返回对应的响应
 * 如果没发现冲突，再逐个位置进行修改，其中对于一些层需要先考虑是否存在对应的副本
 * @param string
 * @param pf
 */
judgeAndRes addString(char* string, pfEntry pf){
    judgeAndRes  *andRes=RedisModule_Calloc(1,sizeof(*andRes));
    bool isBig=pf.isBigData;//如果是大数据情况，实际的位置将从有效层开始
    sliceChain* chain=pf.slicechain; // 这里可以得到大数据情况对应的最低层
    int lowestLayer=chain->initLayer;
    judgeAndintArr judgeArr=calEleLocal(string,pf.weight,pf.high,pf.conflictP);
    if (judgeArr.succ==true){
        Queue *pos=judgeArr.pos;


    }else{
        andRes->judge=false;
        andRes->descrip=judgeArr.descrip;
    }
    return *andRes;
}

void delString(char* string,pfEntry pf){

}

bool judgeString(char* string,pfEntry pf){

}
