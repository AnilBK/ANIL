// clang-format off
///*///
// Dictionary of <int, list<String>>
// This is just for Symbol Table in preprocess_test as we dont have Dictionary<int, String> as of now.
struct int_str_list{int key, Vector<String> value};
namespace int_str_list
function __init__(p_key : int)
  this.key = p_key
  this.value.__init__(5)
endfunction
    
function __del__()
  this.value.__del__()
endfunction
endnamespace

namespace Vector
function<int_str_list> __contains__(value : T) -> bool:
  for pair in this{
    if pair.key == value.key{
      // FIXME: Incomplete implementation.
      return true
    }
  }
  return false
endfunction
endnamespace

// Ordered Dictionary of key(int) and value(list of string).
struct Dict_int_string{Vector<int_str_list> pairs};
namespace Dict_int_string
function __init__()
  this.pairs.__init__(5)
endfunction

function __contains__(p_key : int) -> bool:
  let found : bool =  false
  for pair in this.pairs{
    if pair.key == p_key{
      found = True
      // no break because the pair should be destructed at the end of the scope.
      // break doesn't do that as of now.
    }
  }
  return found
endfunction

c_function key_exists_quit()
  fprintf(stderr, "Pop from empty Vector.\n");
  exit(EXIT_FAILURE);
endc_function

function add_key(p_key : int)
  if p_key in this{
    this.key_exists_quit()
  }else{
    let s = int_str_list{p_key};
    this.pairs.push(s)
  }
endfunction

function __del__()
  this.pairs.__del__()
endfunction
endnamespace
///*///


