#ifndef VECTOR_H
#define VECTOR_H

//TODO: insert element
//TODO: insert another vec

#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>

#define VECTOR_DEFAULT_CAPACITY 64

typedef enum {
    VECTOR_INT_TYPE,
    VECTOR_REAL_TYPE,
    VECTOR_STRING_TYPE
}vector_type;

typedef struct {
    void *value;
    vector_type type;
    size_t len;
    size_t cap;
} vector_t;


// to convert types to (void*)
void* vector_decimal(const int64_t x);
void* vector_string (const char* s); 
void* vector_real(const double x);


// to init
vector_t vector_init(vector_type type);
vector_t vector_init_cap(vector_type type, size_t cap);
vector_t vector_copy(const vector_t src); 
vector_t vector_copy_len(const vector_t src, size_t count);
vector_t vector_from_int_arr(int64_t* data, size_t len);
vector_t vector_from_double_arr(double* data, size_t len);
vector_t vector_from_str_arr(char** data, size_t len);
vector_t vector_from_int(size_t count, ...); // vector_t v = vector_from_int(10, 0,1,2,3,4,5,6,7,8,9)
vector_t vector_from_double(size_t count, ...);
vector_t vector_from_str(size_t count, ...);
vector_t vector_from_range(size_t start, size_t stop); 
vector_t vector_from_range_step(size_t start, size_t stop, size_t step);

// to manipulate
 
// vector_push and vector_insert return 1 if capacity increased
int vector_push(vector_t *v, void* value);
int vector_insert(vector_t *v, int index, void* value);
void* vector_pop(vector_t *v, int index); 
void vector_change_cap(vector_t *v, size_t cap);
void vector_del(vector_t *v, int index);
void vector_del_range(vector_t *v, int start, int end);
void* vector_pop(vector_t *v, int index);
void vector_delete(vector_t* v); // free memory and set fields to default
vector_t vector_plus(const vector_t v1, const vector_t v2);
void vector_concat(vector_t *dst, const vector_t src);

// to access
void* vector_at(vector_t v, int index); // if type is double, you need to free the return val
void vector_print_el(FILE* stream, vector_t v, size_t index);
void vector_pprint(const vector_t v);


void vector_free(vector_t v); // just free memory

// help functions
void _vector_alloc(void** data, vector_type type, size_t len);
size_t _vector_sizeof_by_vectortype(vector_type type);
 
#endif
