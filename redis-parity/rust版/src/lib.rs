
extern crate libc;
extern crate bit_vec;
extern crate bitflags;
#[macro_use]
extern crate redis_module;

use redis_module::native_types::RedisType;
use redis_module::raw::RedisModuleTypeMethods;


use redis_module::{Status, RedisError, RedisValue};
use redis_module::{Context, RedisResult, RedisString, NextArg};

mod pf_filter;

use libc::c_int;
use pf_filter::*;
const MODULE_NAME: &str = "redis-parityFilter";
const MODULE_VERSION: c_int = 1;

pub const REDIS_FILTER_TYPE_VERSION: i32 = 1;
pub static REDIS_FILTER_TYPE: RedisType = RedisType::new(
    "ReParityF",
    REDIS_FILTER_TYPE_VERSION,
    RedisModuleTypeMethods {
        version: redis_module::TYPE_METHOD_VERSION,
        rdb_load: None,
        rdb_save: None,
        aof_rewrite: None,
        free: None,
        mem_usage: None,
        digest: None,
        aux_load: None,
        aux_save: None,
        aux_save_triggers: 0,
        free_effort: None,
        unlink: None,
        copy: None,
        defrag: None,
    },
);

// pf.add 键名 元素对应的字符串 可能的希望冲突概率
// 输入的概率 p -> 实际的概率 1/2^p , 我们规定 11 =< p <= 64
// 选择 11 是为了26层的高度，正好可以符合一个页容量。
fn pf_add(ctx: &Context, args: Vec<RedisString>) -> RedisResult {
    if args.len() < 3{ return Err(RedisError::String("argument lost".to_string())); }
    let mut flag = false;
    if args.len() == 4{
        flag = true;
    }
    let mut args = args.into_iter().skip(1);
    let key = args.next_arg()?; // 键名
    let element = args.next_string()?; // 元素字符串

    ctx.log_debug(format!("key: {}", key).as_str());

    let key = ctx.open_key_writable(&key);

    match key.get_value::<PfFilter>(&REDIS_FILTER_TYPE)? {
        Some(value) => {
            let res = &add_element(value,&element).to_owned();
            let (succ,request) = (res.succ,res.request.to_string());
            if succ.eq(&false) {
                return Err(RedisError::String(request));
            }
        },
        None => {
             if flag.eq(&true){
                 let p = args.next_u64()?; // 可能的冲突概率
                 let mut value = init_pf(2u64.pow((p-3) as u32) as usize);
                 let res = &add_element(&mut value,&element);
                 if res.succ.eq(&false) {
                     return Err(RedisError::String(res.request.to_string()));
                 }
                 key.set_value(&REDIS_FILTER_TYPE,value)?;
             }else{
                 return Err(RedisError::String("not exist this key".to_string()));
             }
        }
    }

    return Ok((0 as i64).into());
}

fn pf_del(ctx: &Context, args: Vec<RedisString>) -> RedisResult {
    if args.len() < 3 { return Err(RedisError::String("argument lost".to_string())); }
    let mut args = args.into_iter().skip(1);
    let key = args.next_arg()?; // 键名
    let element = args.next_string()?; // 元素字符串
    ctx.log_debug(format!("key: {}", key).as_str());
    let key = ctx.open_key_writable(&key);
    match key.get_value::<PfFilter>(&REDIS_FILTER_TYPE)? {
        Some(value) => {
            let res = &del_element(value,&element);

            if res.succ.eq(&false) {
                return Err(RedisError::String(res.request.to_string()));
            }
        },
        None => { return Err(RedisError::String("not exist this key".to_string())); }
    }

    return Ok(RedisValue::Integer(0));
}

// 返回1 表示存在， 0 表示不存在
fn pf_judge(ctx: &Context, args: Vec<RedisString>) -> RedisResult{
    if args.len() < 3 { return Err(RedisError::String("argument lost".to_string())); }
    let mut args = args.into_iter().skip(1);
    let key = args.next_arg()?; // 键名
    let element = args.next_string()?; // 元素字符串
    ctx.log_debug(format!("key: {}", key).as_str());
    let key = ctx.open_key_writable(&key);
    match key.get_value::<PfFilter>(&REDIS_FILTER_TYPE)? {
        Some(value) => {
            let res = jug_element(value,&element);
            if res.eq(&true) {
                return Ok(RedisValue::Integer(1));
            }else if res.eq(&false){
                return Ok(RedisValue::Integer(0));
            }
        },

        None => { return Err(RedisError::String("not exist this key".to_string())); }
    }

    return Err(RedisError::String("some problem".to_string()));
}

redis_module! {
    name: "ReParityF",
    version: 1,
    data_types: [
        REDIS_FILTER_TYPE,
    ],
    commands: [
        ["pf.add", pf_add, "write", 1, 1, 1],
        ["pf.del", pf_del, "write", 1, 1, 1],
        ["pf.jug", pf_judge, "readonly", 1, 1, 1],
    ],
}