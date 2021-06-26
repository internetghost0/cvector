#include "vector.h"

void* vector_decimal(const int64_t x) {return (void*) x;}
void* vector_string (const char* s) {return (void*) s;}
void* vector_real(const double x) {
    double* f = (double*)malloc(sizeof(double));
    assert(f != NULL && "Cannot allocate memory");
    *f = x;
    return (void*) f;
}

//
// INIT START
//

vector_t vector_init(vector_type type) {
    vector_t v;
    v.cap = VECTOR_DEFAULT_CAPACITY;
    v.len = 0;
    v.type = type;
    _vector_alloc(&v.value, type, v.cap);
    return v;
}

vector_t vector_init_cap(vector_type type, size_t cap) {
    vector_t v;
    v.cap = cap;
    if (v.cap == 0) {
        v.cap = 1;
    }
    v.len = 0;
    v.type = type;
    _vector_alloc(&v.value, type, v.cap);
    return v;
}


vector_t vector_copy(const vector_t src) {
    return vector_copy_len(src, src.len);
}
vector_t vector_copy_len(const vector_t src, size_t count) { 
    vector_t v = vector_init_cap(src.type, count);
    size_t bytes = _vector_sizeof_by_vectortype(src.type);
    memcpy(v.value, src.value, count*bytes); 
    v.len = count;
    return v;
}


vector_t vector_from_int_arr(int64_t* data, size_t len) {
    vector_t v = vector_init_cap(VECTOR_INT_TYPE, len);
    size_t bytes = sizeof(int64_t);
    memcpy(v.value, data, len*bytes);
    v.len = len;
    return v;
}
vector_t vector_from_double_arr(double* data, size_t len) {
    vector_t v = vector_init_cap(VECTOR_REAL_TYPE, len);
    size_t bytes = sizeof(double);
    memcpy(v.value, data, len*bytes);
    v.len = len;
    return v;
}
vector_t vector_from_str_arr(char** data, size_t len) {
    vector_t v = vector_init_cap(VECTOR_STRING_TYPE, len);
    size_t bytes = sizeof(char*);
    memcpy(v.value, data, len*bytes);
    v.len = len;
    return v;
}


vector_t vector_from_int(size_t count, ...) {
    va_list args;
    vector_t v = vector_init_cap(VECTOR_INT_TYPE, count);
    va_start(args, count);
    for (size_t i = 0; i < count; ++i) {
        vector_push(&v, vector_decimal(va_arg(args, int64_t)));
    }
    va_end(args);
    return v;
}
vector_t vector_from_double(size_t count, ...) {
    va_list args;
    vector_t v = vector_init_cap(VECTOR_REAL_TYPE, count);
    va_start(args, count);
    for (size_t i = 0; i < count; ++i) {
        vector_push(&v, vector_real(va_arg(args, double)));
    }
    va_end(args);
    return v;
}
vector_t vector_from_str(size_t count, ...) {
    va_list args;
    vector_t v = vector_init_cap(VECTOR_STRING_TYPE, count);
    va_start(args, count);
    for (size_t i = 0; i < count; ++i) {
        vector_push(&v, vector_string(va_arg(args, char*)));
    }
    va_end(args);
    return v;
}

vector_t vector_from_range(size_t start, size_t stop) {

    return vector_from_range_step(start, stop, 1);

}
vector_t vector_from_range_step(size_t start, size_t stop, size_t step) {
    vector_t res =  vector_init_cap(VECTOR_INT_TYPE, (int)((stop-start)/step)+1);
    res.type = VECTOR_INT_TYPE; 
    res.len = 0;
    for (size_t i = start; i < stop; i += step) {
        vector_push(&res, vector_decimal(i));
    }
    return res;
}

//
// INIT END
//


//
// HELP FUNC START
//

void _vector_alloc(void** data, vector_type type, size_t len) {
    assert(data != NULL && "SEGFAULT");
    size_t bytes = _vector_sizeof_by_vectortype(type);
    *data = malloc(len*bytes);
    assert(*data != NULL && "Cannot allocate memory");
}

size_t _vector_sizeof_by_vectortype(vector_type type) {
    switch(type) {
        case VECTOR_INT_TYPE: {
            return sizeof(int64_t);
        }
        case VECTOR_REAL_TYPE: {
            return sizeof(double);
        }
        case VECTOR_STRING_TYPE: {
            return sizeof(char*);
        }
        default: {
            assert(0 && "unreachable");
        }
    }
}


void vector_change_cap(vector_t *v, size_t cap) {
    void *data = NULL;
    _vector_alloc(&data, v->type, cap);
    size_t bytes = _vector_sizeof_by_vectortype(v->type);
    memcpy(data, v->value, v->len*bytes);
    free(v->value);
    v->value = data;
    v->cap = cap;
}


//
// HELP FUNC END
//

//
// MANIPULATE START
//

int vector_push(vector_t *v, void* value) {
    int increased = 0;
    if (v->len == v->cap) {
        vector_change_cap(v, v->cap*2);
        increased = 1;
    }
    assert(v->len < v->cap && "Length > Capacity");

    switch (v->type) {
        case VECTOR_INT_TYPE: {
            ((int64_t*)v->value)[v->len++] = (int64_t)value;
            break;
        }
        case VECTOR_REAL_TYPE: {
            ((double*)v->value)[v->len++] = *(double*)value;
            free(value);
            break;
        }
        case VECTOR_STRING_TYPE: {
            ((char**)v->value)[v->len] = (char*)value;
            v->len++;
            break;
        }
        default: {
            assert(0 && "unreachable");
        }
    }
    return increased;
}

int vector_insert(vector_t *v, int index, void* value) {
    if (index < 0) {
        index += v->len+1;
    }
    assert(index >= 0 && index-1 < (int)v->len && "ERROR: index out of range");
    int increased = 0;

    if (v->len == v->cap) {
        vector_change_cap(v, v->cap*2);
        increased = 1;
    }
    assert(v->len < v->cap && "Length > Capacity");

    size_t bytes = _vector_sizeof_by_vectortype(v->type);
    void* data = NULL;
    data = malloc((v->len+1)*bytes);
    memset(data, 0, (v->len+1)*bytes);
    memcpy(data, v->value, index*bytes);
    //memcpy(data+index*bytes, value, 1*bytes);

    switch (v->type) {
        case VECTOR_INT_TYPE: 
        case VECTOR_STRING_TYPE:
            memcpy((char*)data+index*bytes, &value, 1*bytes);
            break;

        case VECTOR_REAL_TYPE:
            memcpy((char*)data+index*bytes, value, 1*bytes);
            break;
        default: {
            assert(0 && "unreachable");
        }
    }
    memcpy((char*)data+(index+1)*bytes, (char*)v->value+(index*bytes), (v->len-index)*bytes);
    free(v->value);
    v->value = data;
    v->len++;
    return increased;

}





void vector_del(vector_t *v, int index) {
    if (index < 0) {
        index += v->len;
    }
    assert(index >= 0 && index < (int)v->len && "ERROR: index out of range");
    size_t bytes = _vector_sizeof_by_vectortype(v->type);
    void* data = malloc((v->len-1)*bytes);
    memcpy(data, v->value, index*bytes);       // [0,1,2,3,4,5] -> pop(3) -> [0,1,2,4,5]
    memcpy((char*)data+index*bytes, (char*)v->value+(1+index)*bytes, (v->len-1-index)*bytes);
    free(v->value);
    v->value = data;
    v->len-=1;
}
void* vector_pop(vector_t *v, int index) {
    if (index < 0) {
        index += v->len;
    }
    assert(index >= 0 && index < (int)v->len && "ERROR: index out of range");
    void* res = NULL;
    switch (v->type) {
        case VECTOR_INT_TYPE: 
            res = (void*)((int64_t*)v->value)[index];
            break;
        case VECTOR_REAL_TYPE: 
            res = vector_real(((double*)v->value)[index]);
            break;
        case VECTOR_STRING_TYPE: 
            res = (void*)((char**)v->value)[index];
            break;
        default: 
            assert(0 && "unreachable");
    }
    vector_del(v, index);
    return res;
}

void vector_del_range(vector_t *v, int start, int end) {
    if (start < 0)
        start += v->len;
    if (end < 0)
        end += v->len;
    // [0,1,2,3,4,5,6]
    // del_range(1,2), lenr = 2
    // [0,3,4,5,6]
    assert(start < end && start >= 0 && end < (int)v->len && "ERROR: index out of range");
    size_t lenr = end-start+1;
    size_t bytes = _vector_sizeof_by_vectortype(v->type);
    void* data = malloc((v->len-lenr)*bytes);
    memcpy(data, v->value, start*bytes);
    memcpy((char*)data+start*bytes, (char*)v->value+(end+1)*bytes, (v->len-lenr)*bytes);
    free(v->value);
    v->value = data;
    v->len-=lenr;

}

void vector_free(vector_t v) {
    free(v.value);
}
void vector_delete(vector_t* v) {
    free(v->value);
    v->value = NULL;
    v->len = 0;
    v->cap = VECTOR_DEFAULT_CAPACITY;
}
vector_t vector_plus(const vector_t v1, const vector_t v2) {
    assert(v1.type == v2.type && "differt types");
    vector_t res = vector_init_cap(v1.type, v1.len + v2.len);
    res.type = v1.type;
    res.len = v1.len+v2.len;
    size_t bytes = _vector_sizeof_by_vectortype(v1.type);
    memcpy(res.value, v1.value, v1.len*bytes);
    memcpy((char*)res.value + v1.len*bytes, v2.value, v2.len*bytes);
    return res;
}
void vector_concat(vector_t *dst, const vector_t src) {
    assert(dst->type == src.type && "differt types");
    if (dst->cap < src.len + dst->len) {
        vector_change_cap(dst, src.len + dst->len);
    }
    size_t bytes = _vector_sizeof_by_vectortype(src.type);
    memcpy((char*)dst->value + dst->len*bytes, src.value, src.len*bytes);
    dst->len += src.len;
}

//
// MANIPULATE END
//


//
// ACCESS START
//

void* vector_at(vector_t v, int index) {
    if (index < 0) {
        index = v.len + index;
    }
    assert(index >= 0 && index < (int)v.len && "index out of range");
    switch(v.type) {
        case VECTOR_INT_TYPE: {
            return (void*)(((int64_t*)v.value)[index]);
        }
        case VECTOR_REAL_TYPE: {
            double *f = (double*)malloc(sizeof(double));
            assert(f != NULL && "Cannot allocate memory");
            *f = ((double*)v.value)[index];
            return (void*) f;
        }
        case VECTOR_STRING_TYPE: {
            return (void*)(((char**)v.value)[index]);
        }
        default: {
            assert(0 && "unreachable");
        }
    }
}

void vector_print_el(FILE* stream, vector_t v, size_t index) {
    assert(index < v.len && "index out of range");
    switch(v.type) {
        case VECTOR_INT_TYPE: {
            fprintf(stream, "%ld", ((int64_t*)v.value)[index]);
            break;
        }
        case VECTOR_REAL_TYPE: {
            fprintf(stream, "%lf", ((double*)v.value)[index]);
            break;
        }
        case VECTOR_STRING_TYPE: {
            fprintf(stream, "\"%s\"", ((char**)v.value)[index]);
            break;
        }
        default: {
            assert(0 && "unreachable");
        }
    }
}
void vector_pprint(const vector_t v) {
    if (v.len == 1) {
        putchar('[');
        vector_print_el(stdout, v, 0);
        puts("]");
    } else if (v.len < 1) {
        puts("[]");
    } else {
        putchar('[');
        for (size_t i=0; i<v.len-1; i++) {
            vector_print_el(stdout, v, i);
            printf(", ");
        }
        vector_print_el(stdout, v, v.len-1);
        puts("]");
    }
}

//
// ACCESS END
//

