#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <iron/types.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iron/log.h>
#include <iron/mem.h> 
#include <iron/fileio.h>
#include <iron/time.h>
#include <iron/utils.h>
#include <iron/linmath.h>
#include <iron/math.h>
#include <iron/datastream.h>
#include<dlfcn.h>
#include <sys/mman.h>
#include <icydb.h>
#include "string_vector.h"
#include "module.h"
#include "neon.h"
void * module_open(const char * name){
  void * module = dlopen(name, RTLD_NOW | RTLD_GLOBAL);
  return module;
}

void * module_symbol(void * module, const char * sym){
  return dlsym(module, sym);
}

char * module_error(){
  return dlerror();
}

engine_context current_context;

// get/set module data gets static data related to the module.
void * get_module_data(module_data * md){
  engine_context e = current_context;
  var vec = (icy_vector *) md->internal;
  if(vec == NULL || icy_vector_count(vec) <= e.context_id)
    return NULL;
  
  return ((void **) icy_vector_lookup(vec, (icy_index){e.context_id}))[0];
}

void set_module_data(module_data * md, void * data){
  var vec = (icy_vector *) md->internal;
  if(vec == NULL)
    md->internal = vec = icy_vector_create(NULL, sizeof(void *));
  
  engine_context e = current_context;
  while(icy_vector_count(vec) <= e.context_id)
    icy_vector_alloc(vec);
  ((void **)icy_vector_lookup(vec, (icy_index){e.context_id}))[0] = data;
}

string_vector * get_loaded_modules(){
  static module_data _loaded_module;
  string_vector * vec = NULL;
  if(!(vec = get_module_data(&_loaded_module))){
    char buf[100];
    sprintf(buf, "imported_modules.%i", current_context.context_id);
    vec = string_vector_create(buf);
    set_module_data(&_loaded_module, vec);
  }
  return vec;
}

int load_module(const char * name){
  string_vector * loaded_modules = get_loaded_modules();
  static module_data _loaded_vec;
  string_vector * loaded_vec = NULL;
  if(!(loaded_vec = get_module_data(&_loaded_vec))){
    loaded_vec = string_vector_create(NULL);
    set_module_data(&_loaded_vec, loaded_vec);
  }
  
  size_t idx = 0;
  string_vector_index id;
  while(string_vector_iterate(loaded_vec, &id, 1, &idx)){
    char * _name = string_vector_lookup(loaded_vec, id);
    if(_name == NULL)continue;
    if(strcmp(_name, name) == 0){
      dmsg(neon_main, "Module '%s' is already loaded..\n", name);
      return -1;
    }
  }

  dmsg(neon_main,"Loading module '%s'\n", name);
  void * module = module_open(name);;
  if(module == NULL){
    dmsg(neon_main, "ERROR: unable to load module '%s' %s\n", name, module_error());
    ASSERT(false);
    return -1;
  }

  void (* init_module)() = module_symbol(module, "init_module");
  if(init_module == NULL){
    dmsg(neon_main, "ERROR: Unable to get 'init_module' from module '%s'\n", name);
    return -1;
  }

  var sidx = string_vector_alloc(loaded_vec, strlen(name) + 1);
  char * _name = string_vector_lookup(loaded_vec, sidx);
  strcpy(_name, name);
  
  _name = string_vector_lookup(loaded_modules, sidx);
  strcpy(_name, name);

  init_module();
  
  return 0;
}
