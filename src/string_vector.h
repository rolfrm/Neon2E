typedef struct{
  u8 data[16];
  
}string_vector_chunk;

typedef struct{
  u32 id;
}string_vector_index;
struct _string_vector;
typedef struct _string_vector string_vector;

string_vector * string_vector_create(const char * name);
string_vector_index string_vector_alloc(string_vector * table, u32 size);
void string_vector_dealloc(string_vector * table, string_vector_index * index);
void string_vector_realloc(string_vector * table, string_vector_index index, u32 size);
char * string_vector_lookup(string_vector * table, string_vector_index index);
size_t string_vector_iterate(string_vector * table,  string_vector_index * ids, u32 id_count, u64 *index);

void string_vector_test();
