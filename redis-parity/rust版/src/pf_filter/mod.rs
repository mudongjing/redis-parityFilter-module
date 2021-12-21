mod pf;
use bit_vec::BitVec;
// 前缀3次，默认最大高度为16
// 所有位置均为停留点
#[derive(Debug)]
pub struct PfFilter{
    width:usize,
    
    original_map:Vec<BitVec>,// 即，这里最多放16个元素
    
    pure_map:Vec<BitVec>,
   
    stop_map:Vec<BitVec>,
}

impl PfFilter{
    pub fn new(width:usize, original_map:Vec<BitVec>, pure_map:Vec<BitVec>, stop_map:Vec<BitVec>) -> PfFilter {
      PfFilter{width:width,original_map:original_map,pure_map:pure_map,stop_map:stop_map}
    }
    pub fn width(self) -> usize{ self.width }
    pub fn set_width(&mut self,width:usize){ self.width=width; }
}

#[derive(Debug, Clone)]
pub struct Request{
    pub succ:bool,
    pub request:String,
}
impl Request{
    pub fn new(succ:bool,request:String) -> Request{
        Request{succ:succ,request:request}
    }
}


pub fn add_element(filter:&mut PfFilter,element:&String) -> Request{
    let width = filter.width;
    let stop_array = pf::stop_array(element,width);
    
    if stop_array.len()==1 && *stop_array.get(0).unwrap() == 0{
        let res = Request{succ:false,request:"exceed the limit".to_string()};
        return res;
    }else{
        let len = stop_array.len();
        let  original = &mut filter.original_map;
        let  stop = &mut filter.stop_map;
        let  pure = &mut filter.pure_map;
        let mut arr = stop.get(len-1-3).unwrap().to_owned();
        let x = *stop_array.get(0).unwrap();
        if arr.get(x).unwrap().eq(&true){// 存在停留点
            let res = Request{succ:false,request:"already has an element".to_string()};
            return res;
        }
        arr.set(x,true);

        let res = Request{succ:true,request:"".to_string()};
        if len-4 <= 0 {
            stop.remove(len-4);
            stop.insert(len-4,arr);
            return res;
        }

        for i in (1+len-4)..=1{ // 添加元素信息
            let mut arr = original.get(len-1-i).unwrap().clone();
            let x = *stop_array.get(i).unwrap();
            if arr.get(x).unwrap().eq(&true){
                let mut arr_p = pure.get(len-1-i).unwrap().clone();
                arr_p.set(x ,true);
                pure.remove(len-1-i);
                pure.insert(len-1-i,arr_p);
            }else{
                arr.set(x,true);
                original.remove(len-1-i);
                original.insert(len-1-i,arr);
            }
        }

        return res;
    }
}

pub fn del_element(filter:&mut PfFilter,element:&String) -> Request{
    let width = filter.width;
    if jug_element(filter,element).eq(&true){
        let stop_array = pf::stop_array(element,width);

        let res = Request{succ:true,request:"".to_string()};

        let len = stop_array.len();
        let  original = &mut filter.original_map;
        let  stop = &mut filter.stop_map;
        let  pure = &mut filter.pure_map;
        let mut arr = stop.get(len-1-3).unwrap().clone();
        let x = *stop_array.get(0).unwrap();
        arr.set(x,false);//删除停留点

        if len-4 <= 0 {
            stop.remove(len-4);
            stop.insert(len-4,arr);
            return res;
        }

        for i in (1+len-4)..=1{
            let arr = pure.get(len-1-i).unwrap();
            let x = *stop_array.get(i).unwrap();

            if arr.get(x).unwrap().eq(&false){
                let mut arr_o = original.get(len-1-i).unwrap().clone();
                arr_o.set(x as usize,false);
                original.remove(len-1-i);
                original.insert(len-1-i,arr_o);
            }
        }

        return res;
    }else{
        let res = Request{succ:false,request:"do not have this element".to_string()};
        return res;
    }
}

pub fn jug_element(filter:&PfFilter,element:&String) -> bool{
    let width = filter.width;
    let stop_array = pf::stop_array(element,width);
    if stop_array.len()==1 && *stop_array.get(0).unwrap() == 0{ return false; }
    else{
        let len = stop_array.len();
        let  original = &filter.original_map;
        let  stop = &filter.stop_map;
        let  pure = &filter.pure_map;
        let  arr = stop.get(len-1-3).unwrap();
        let x = *stop_array.get(0).unwrap();
        if arr.get(x).unwrap().eq(&true){ return false; }

        if len-4 <= 0 { return true; }

        for i in (1+len-4)..=1{
            let arr = original.get(len-1-i).unwrap();
            let x = *stop_array.get(i).unwrap();
            if arr.get(x).unwrap().eq(&false){ return false; }
        }
        return true;
    }
}

pub fn init_pf(width:usize) -> PfFilter{

    let HGIH = 16;
    let mut original_map:Vec<BitVec> = Vec::new();
    for i in 0..=16 {
        original_map.push(BitVec::from_elem(width,false));
    }
    let mut pure_map:Vec<BitVec> = Vec::new();
    for i in 0..=16 {
        let  bit_vec = BitVec::from_elem(width,false);
        pure_map.push(bit_vec);
    }
    let mut stop_map:Vec<BitVec> = Vec::new();
    for i in 0..=16 {
        let  bit_vec = BitVec::from_elem(width,false);
        stop_map.push(bit_vec);
    }

    PfFilter::new(width,original_map,pure_map,stop_map)
}
