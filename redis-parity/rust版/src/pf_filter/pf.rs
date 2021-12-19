use bitvec::*;
use std::collections::VecDeque;

pub fn hash_string(vec_string:&mut Vec<u8>) -> u32{
    let p = 16777619;
    let mut hash =  2166136261;
    for ch in &*vec_string  { hash = (hash ^ *ch as u32).wrapping_mul(p); }
    hash.wrapping_add(hash.rotate_left(13));
    hash ^= hash.rotate_right(7);
    hash.wrapping_add(hash.rotate_left(3));
    hash.wrapping_add(hash.rotate_right(17));
    hash ^= hash.rotate_left(5);
    return hash;
}
pub fn string_hash(original_string:&String,original_index:usize,result_string:&mut Vec<u8>,width:u32) -> (u32,usize) {
    let len = original_string.len();
    let mut index = original_index;
    if index >= (len-1) { index = 0; }
    else{ index +=1; }
    result_string.push(original_string.chars().nth(index).unwrap() as u8);
    let hash_value = hash_string(result_string) % width;
    return (hash_value,index);
}

fn is_even(value:u32) -> bool{
    if value % 2 == 1{
        return false;
    }
    return true;
}

pub fn stop_array(element:&String,width:u32) -> VecDeque<u32> {
    let PRE = 2;
    let HIGH = 19;
    let mut string_vec: Vec<u8> = Vec::new();
    let mut result: VecDeque<u32> = VecDeque::new();
    for i in element.chars() {
        string_vec.push(i as u8);
    }

    let (value, index) = string_hash(element, element.len() - 1, &mut string_vec, width);
    result.push_front(value);

    let parity = is_even(value);
    let mut n_index: usize = index;
    while true {
        let (value, index) = string_hash(element, n_index, &mut string_vec, width);
        n_index = index;
        println!("{:?}", result);

        result.push_front(value);
        if result.len() > PRE + 1 && result.len() <= HIGH {
            if !is_even(*result.get(1).unwrap()).eq(&parity) && !is_even(*result.get(2).unwrap()).eq(&parity) {
                if is_even(value).eq(&parity) { return result; }
            }
        } else if result.len() > HIGH {
            break;
        }
    }
    let mut result1: VecDeque<u32> = VecDeque::new();
    result1.push_front(0);
    return result1;
}
