#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// IMPORTS //

// STRUCT_DEFINATIONS //

int main() {

  // clang-format off

  // let color2 = Color["red"]
  // This will produce error below, because Color has "Red" and not "red".

  // Currently we Support {"String" : int} dictionary only.
  
  ///*/// main()

  // Compile time dictionaries.
  // These acts as contants, and the individual values of these constants
  // will be placed in code at compile time.
  import Vector
  import String

  constexpr Color = {"Red":255, "Green":200}
  constexpr Age = {"Anil":22, "Universe":10000}

  let color = Color["Red"]
  let color2 = Color["Green"]

  let age1 = Age["Anil"]
  let age2 = Age["Universe"]

  constexpr RGBMap = {"Red" : "225,0,0", "Green" : "0,255,0", default : "255,255,255"}
  
  let red = RGBMap["Red"]
  print("Red Color RGB: {red} \n")

  let green = RGBMap["Green"]
  print("Green Color RGB: {green} \n")

  let default_color = RGBMap["whatever"]  
  print("Default Color RGB: {default_color} \n")

  let g_key = "Green"
  let contexpr_at_runtime_color = RGBMap[g_key]
  print("Green Color RGB fetched from constexpr map at runtime: {contexpr_at_runtime_color} \n")

  // DESTRUCTOR_CODE //
  ///*///

  // clang-format on

  return 0;
}