#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <iron/types.h>
#include <iron/log.h>
#include <iron/mem.h>
#include <iron/utils.h>

#include "string_vector.h"

#include "string_vector_internal.h"
#include "string_vector_defer.h"
#include <string_vector_internal.c>
#include <string_vector_defer.c>
struct _string_vector{
  string_vector_internal * strings;
  string_vector_defer * defered;
};

string_vector * string_vector_create(const char * name){
  string_vector * n = alloc(sizeof(string_vector));
  {
    char buffer[100];
    char * internal_name = NULL;
    if(name != NULL){
      sprintf(buffer, "%s.__internal__", name);
      internal_name = buffer;
    }

    n->strings = string_vector_internal_create(internal_name);
  }
  {
    char buffer[100];
    char * defer_name = NULL;
    if(name != NULL){
      sprintf(buffer, "%s.__defer__", name);
      defer_name = buffer;
    }

    n->defered = string_vector_defer_create(defer_name);
  }
  if(*n->strings->count == 0){
    // Reserve the first index.
    logd("Reserving first index\n");
    string_vector_alloc(n, 1);
  }
  
  return n;
}

string_vector_index string_vector_alloc(string_vector * table, u32 size){
  string_vector_internal_indexes indexes = string_vector_internal_alloc_sequence(table->strings, 1 + size / sizeof(string_vector_chunk));
  string_vector_defer_index defer_index = string_vector_defer_alloc(table->defered);
  table->defered->int_id[defer_index.index]  = indexes;
  string_vector_index idx = {(u32)defer_index.index};
  return idx;
}

void string_vector_dealloc(string_vector * table, string_vector_index * _index){
  let index = *_index;
  string_vector_internal_indexes i = table->defered->int_id[index.id];
  string_vector_internal_remove_sequence(table->strings, &i);
  string_vector_defer_remove(table->defered, (string_vector_defer_index){index.id});
  memset(_index, 0, sizeof(*_index));
}

void string_vector_realloc(string_vector * table, string_vector_index index, u32 size){
  string_vector_internal_indexes i = table->defered->int_id[index.id];
  i = string_vector_internal_alloc_sequence(table->strings, size);
  table->defered->int_id[index.id] = i;
}

char * string_vector_lookup(string_vector * table, string_vector_index index){
  if(index.id == 0)
    return NULL;
  string_vector_internal_indexes i = table->defered->int_id[index.id];
  char * ptr = (char *) (&table->strings->data[i.index]);
  return ptr;
}

size_t string_vector_iterate(string_vector * table,  string_vector_index * ids, u32 id_count, u64 *index){
  if(*index == 0)
    *index = 1;
  size_t read = 0;
  for(u32 i = 0; i < id_count; i++){

    if(*index >= *table->defered->count)
      break;
    ids[i] = (string_vector_index){*index};
    *index += 1;
    read += 1;
  }
  return read;
}

void string_vector_test(){
  string_vector * sv = string_vector_create("test_string_vector");

  string_vector_index idx = string_vector_alloc(sv, 10);
  string_vector_index idx2 = string_vector_alloc(sv, 20);
  string_vector_index idx3 = string_vector_alloc(sv, 30);

  const char * s1 = "dwadjss";
  const char * s2 = "more than 10";
  const char * s3 = "_more_ than 10 and 20";
  ASSERT(strlen(s1) < 10 && strlen(s2) < 20 && strlen(s3) < 30);
  
  char * str = string_vector_lookup(sv, idx);
  char * str2 = string_vector_lookup(sv, idx2);
  char * str3 = string_vector_lookup(sv, idx3);
  sprintf(str2, s2);
  sprintf(str3, s3);
  sprintf(str, s1);

  // testing no overlap.
  ASSERT(strcmp(str, s1) == 0);
  ASSERT(strcmp(str2, s2) == 0);
  ASSERT(strcmp(str3, s3) == 0);
  
  logd("%s %s %s\n", str, str2, str3);
  logd("%i %i %i", idx, idx2, idx3);
  string_vector_index current_id;
  u64 index = 0;
  u64 total_count = 0;
  while(string_vector_iterate(sv, &current_id, 1, &index)){
    total_count += 1;
    logd("%i %i\n", current_id, total_count);
  }
  
  string_vector_dealloc(sv, &idx);
  string_vector_dealloc(sv, &idx2);
  string_vector_dealloc(sv, &idx3);
}
