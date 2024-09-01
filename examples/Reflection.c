#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// IMPORTS //

// STRUCT_DEFINATIONS //

int main() {

  // clang-format off

  ///*/// main()
  import Vector
  import List

  let vector_class_members = [];
  let vector_class_member_functions = [];
  let list_instances = [];

  def reflection():
    forall x in members_of(Vector): vector_class_members.append_str(x)
    forall x in member_functions_of(Vector): vector_class_member_functions.append_str(x) 
    forall x in instances_of_class(List): list_instances.append_str(x)
  enddef  
  reflection

  print("We print the different instances of class List using Reflection as follows: \n")

  // 'instances_of_class' returns name of the instances in "string" form,
  // so, we unquote that using UNQUOTE in macro as belows.
  def reflective_print():
    forall x in instances_of_class(List) UNQUOTE: x.print()
  enddef
  reflective_print

  // DESTRUCTOR_CODE //
  ///*///
                    

  return 0;
}