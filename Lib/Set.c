// clang-format off
///*///

///////////////////////////////////////////
struct Set{Vector<String> arr};

namespace Set

function len() -> size_t:
  return this.arr.len()
endfunction

function __getitem__(index : int) -> &String:
  return this.arr[index]
endfunction

function __init__(count : int)
  this.arr.__init__(count)
endfunction  

function __del__()
    this.arr.__del__()
endfunction

function __contains__(value : String) -> bool:
  return value in this.arr  
endfunction

function add(value : String)
  if value not in this{
    this.arr.push(value)
  }
endfunction

function print()
  print("[")
  for str in this{
    str.print()
    print(",")
  }
  print("]\n")
endfunction

endnamespace
///*///

