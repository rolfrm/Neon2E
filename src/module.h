void * module_open(const char * name);
void * module_symbol(void * module, const char * sym);
char * module_error();


typedef struct _engine_context{
  u32 context_id;
}engine_context;
extern engine_context current_context;

int load_module(const char * name);
