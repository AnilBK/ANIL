// clang-format off
///*///
struct Optional<T>{bool _has_value, T _value};

namespace Optional

function __init__()
  this._has_value = false
endfunction

function has_value() -> bool:
  return this._has_value
endfunction

function get_value() -> T:
  return this._value
endfunction

function set_value(p_value : T)
  this._has_value = true
  this._value = p_value
endfunction

endnamespace
///*///

