#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <iron/types.h>
#include <iron/datastream.h>
#include <iron/log.h>
#include <iron/mem.h> 
#include <iron/fileio.h>
#include <iron/time.h>
#include <iron/utils.h>
#include <iron/math.h>
#include <dlfcn.h>
#include <sys/mman.h>
#include <icydb.h>
#include "string_vector.h"
#include "module.h"
#include "event_table.h"
#include "event_table_weight.h"

#include "intern_string_vector_1.h"
#include "intern_string_vector_2.h"

typedef struct{
  u128 X;
  u128 Y;
}u256;

#include "intern_string_vector_3.h"

#include "neon.h"

void test_register_event(){

  register_event(0,0,false);
  register_event(1,0,false);
  register_event(2,0,false);
  register_event(3,0, true);
  register_event(4,1, true);
  register_event(5,1, false);
  register_event(6,0, true);
  register_event(7,0, true);
  register_event(8,0, true);
  bool m1_called = false, m2_called = false, m3_called = false;
  void m1(){
    logd("M1 Called\n");
    m1_called = true;
  }

  void m2(){
    logd("M2 Called\n");
    m2_called = true;
  }

  void m3(){
    logd("M3 Called\n");
    m3_called = true;
  }
  
  register_method(3, m1);
  register_method(5, m2);
  register_method(6, m3);
 
  ASSERT(m1_called && m2_called && m3_called);
}

void intern_string_test(){
  u32 x1 = intern_string("X");
  u32 x2 = intern_string("X");
  u32 x3 = intern_string("hello world");
  u32 x4 = intern_string("hello world");
  u32 x5 = intern_string("hello world2 Hello?");
  u32 x6 = intern_string("hello world2");
  ASSERT(x1 == x2);
  ASSERT(x3 == x4);
  ASSERT(x1 != x3);
  ASSERT(x5 != x1);
  ASSERT(x5 != x6);
  ASSERT(x5 != x1);
  ASSERT(x5 != x3); 
}

void neon_engine_test(){
  static module_data test_mod;
  engine_context ctx = {3};
  current_context = ctx;
  
  string_vector_test();
  intern_string_test();


  ASSERT(!get_module_data(&test_mod));
  void *test_data = alloc0(10);
  set_module_data(&test_mod, test_data);
  var newv = get_module_data(&test_mod);
  ASSERT(newv == test_data);

  current_context = (engine_context){2};
  {
    ASSERT(!get_module_data(&test_mod));
    void *test_data = alloc0(10);
    set_module_data(&test_mod, test_data);
    var newv = get_module_data(&test_mod);
    ASSERT(newv == test_data);
  }
  
  current_context = ctx;

  ASSERT(get_module_data(&test_mod) == test_data);
  //ASSERT(0 == load_module("./game_module1.so"));
  //ASSERT(-1 == load_module("./game_module1.so"));

  test_register_event();
}
