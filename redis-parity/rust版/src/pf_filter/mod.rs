mod pf;
use bit_vec::BitVec;
// 前缀3次，默认最大高度为16
// 所有位置均为停留点
pub struct PF_filter{
    width:u32,
    original_map:Vec<BitVec>,// 即，这里最多放16个元素
    pure_map:Vec<BitVec>,
    stop_map:Vec<BitVec>,
}
impl PF_filter{
    pub fn new(width:u32) ->PF_filter{
        let HGIH = 16;
        let mut original_map:Vec<BitVec> = Vec::new();
        for i in 0..=16 {
            let mut bit_vec = BitVec::from_elem(width as usize,false);
            original_map.push(bit_vec);
        }
        let mut pure_map:Vec<BitVec> = Vec::new();
        for i in 0..=16 {
            let mut bit_vec = BitVec::from_elem(width as usize,false);
            pure_map.push(bit_vec);
        }
        let mut stop_map:Vec<BitVec> = Vec::new();
        for i in 0..=16 {
            let mut bit_vec = BitVec::from_elem(width as usize,false);
            stop_map.push(bit_vec);
        }
        PF_filter{width:width,original_map:original_map,pure_map:pure_map,stop_map:stop_map}
    }
    pub fn width(self) -> u32{ self.width }
    pub fn original_map(self) -> Vec<BitVec>{ self.original_map }
    pub fn pure_map(self) -> Vec<BitVec>{ self.pure_map }
    pub fn stop_map(self) -> Vec<BitVec>{ self.stop_map }
    pub fn set_width(&mut self,width:u32){ self.width=width; }
    pub fn set_original_map(&mut self,location:usize,res:bool){ self.original_map.set(location,res); }
    pub fn set_pure_map(&mut self,location:usize,res:bool){ self.pure_map.set(location,res); }
    pub fn set_stop_map(&mut self,location:usize,res:bool){ self.stop_map.set(location,res); }
}
pub struct Request{
    succ:bool,
    request:String,
}
pub fn add_element(filter:&PF_filter,element:&String,width:u32) -> Request{
    let stop_array = pf::stop_array(element,width);
    if stop_array.len()==1 && stop_array.get(0) == 0{
        let res = Request{succ:false,request:"exceed the limit".to_string()};
        return res;
    }else{
        let len = stop_array.len();
        let original = filter.original_map();
        let stop = filter.stop_map();
        for i in (len-1)..=2{
            if i == len-1 {
                let arr = stop.get(i-3).unwrap();
                let x = stop_array.get(i).unwrap();
                if arr.get(x as usize).eq(&true){// 亿存在停留点
                    let res = Request{succ:false,request:"already has an element".to_string()};
                    return res;
                }
            }
            if original.get(i).eq(&false) { }
        }

        let res = Request{succ:true,request:"".to_string()};
        return res;
    }
}

pub fn del_element(ilter:&PF_filter,element:&String,width:u32){
    let stop_array = pf::stop_array(element,width);
}

pub fn jug_element(ilter:&PF_filter,element:&String,width:u32){
    let stop_array = pf::stop_array(element,width);
}


