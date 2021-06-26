// do not use it..

#include "vector.h"
#include <assert.h>

void* func(void* num) {
    char* res = malloc(36);
    strcpy(res, num);
    strcpy(res+strlen(num), "_num");
    return res;
}

vec_t take_vec(vec_t v, size_t count);
vec_t drop_vec(vec_t v, size_t count);
vec_t take_while_vec(vec_t v, int(*f)(void*));
vec_t drop_while_vec(vec_t v, int(*f)(void*));
vec_t filter_vec(vec_t v, int(*f)(void*));
void map_vec(vec_t *v, void*(*f)(void*));
vec_t map_vec_new(vec_t *v, void*(*f)(void*));

void* head_vec(vec_t v) {return vec_at(v, 0); }
vec_t tail_vec(vec_t v) {
    vec_t res = vec_copy(v);
    vec_del(&res, 0);
    return res;
}
void* last_vec(vec_t v) {return vec_at(v, -1);}

vec_t take_vec(vec_t v, size_t count) {
    if (count > v.len) {
        count = v.len;
    }
    vec_t res = vec_init_cap(v.type, count);
    res.type = v.type;
    size_t bytes = sizeof_vectype(v.type);
    memcpy(res.value, v.value, bytes*count); 
    res.len = count;
    return res;
}
vec_t drop_vec(vec_t v, size_t count) {
    if (count >= v.len) {
        return vec_init(v.type);
    }
    vec_t res = vec_init_cap(v.type, v.len-count);
    res.type = v.type;
    size_t bytes = sizeof_vectype(v.type);
    memcpy(res.value, v.value + count*bytes, v.len*bytes - count*bytes); 
    res.len = v.len-count;
    return res;
}
vec_t take_while_vec(vec_t v, int(*f)(void*)) {
    vec_t res = vec_init(v.type);
    res.type = v.type;
    size_t c = 0;
    int boolv;
    void* t = NULL;
    do {
        switch(v.type) {
            case VEC_INT_TYPE: {
                t = decimal(((int64_t*)v.value)[c]);
                break;
            }
            case VEC_REAL_TYPE: {
                t = real(((double*)v.value)[c]);
                break;
            }
            case VEC_STRING_TYPE: {
                char* str = ((char**)v.value)[c];
                t = string(str);
                break;
            }
            default: {
                assert(0 && "unreachable");
            }

        }
        boolv = (*f)(t);
        if (boolv != 0) {
            vec_push(&res, t);
        } else {
            return res;
        }
        c++;
        if (v.type == VEC_REAL_TYPE) {
            free(t);
        }
    } while(1);
    return res;
}
vec_t drop_while_vec(vec_t v, int(*f)(void*)) {
    size_t c = 0; // count
    int boolv;
    void* t = NULL;
    do {
        switch(v.type) {
            case VEC_INT_TYPE: {
                t = decimal(((int64_t*)v.value)[c]);
                break;
            }
            case VEC_REAL_TYPE: {
                t = real(((double*)v.value)[c]);
                break;
            }
            case VEC_STRING_TYPE: {
                char* str = ((char**)v.value)[c];
                t = string(str);
                break;
            }
            default: {
                assert(0 && "unreachable");
            }

        }
        boolv = (*f)(t);
        if (v.type == VEC_REAL_TYPE) {
            free(t);
        }
        if (boolv != 0) {
            c++;
        } else {
            break;
        }
    } while(1);
    return drop_vec(v, c);
}
vec_t filter_vec(vec_t v, int(*f)(void*)) {
    vec_t res = vec_init(v.type);
    res.type = v.type;
    size_t c = 0;
    int boolv;
    void* t = NULL;
    do {
        switch(v.type) {
            case VEC_INT_TYPE: {
                t = decimal(((int64_t*)v.value)[c]);
                break;
            }
            case VEC_REAL_TYPE: {
                t = real(((double*)v.value)[c]);
                break;
            }
            case VEC_STRING_TYPE: {
                char* str = ((char**)v.value)[c];
                t = string(str);
                break;
            }
            default: {
                assert(0 && "unreachable");
            }

        }
        boolv = (*f)(t);
        if (boolv != 0) {
            vec_push(&res, t);
        }
        c++;
        if (v.type == VEC_REAL_TYPE) {
            free(t);
        }
        if (c >= v.len) break;
    } while(1);
    return res;
}
void map_vec(vec_t *v, void*(*f)(void*)) {
    void *t = NULL;
    for (size_t i = 0; i < v->len; ++i) {
        switch (v->type) {
            case VEC_INT_TYPE: {
                t = decimal(((int64_t*)v->value)[i]);
                ((int64_t*)v->value)[i] = (int64_t)(*f)(t);
                break;
            }
            case VEC_REAL_TYPE: {
                void* t = real(((double*)v->value)[i]);
                double *r = (double*) (*f)(t);
                ((double*)v->value)[i] = *r; 
                free(r);
                free(t);
                break;
            }
            case VEC_STRING_TYPE: {
                char* str = ((char**)v->value)[i];
                t = string(str);
                strncpy(((char**)v->value)[i], (const char*)(*f)(t), VEC_STRING_LIMIT);
                break;
            }
            default: {
                assert(0 && "unreachable");
            }

        }
    }

}
vec_t map_vec_new(vec_t *v, void*(*f)(void*)) {
    vec_t res = vec_init_cap(v->type, v->len);
    res.type = v->type;
    res.len = 0;
    void* t = NULL;
    for (size_t i = 0; i < v->len; ++i) {
        switch (v->type){
            case VEC_INT_TYPE: {
                t = decimal(((int64_t*)v->value)[i]);
                vec_push(&res, (*f)(t));
                break;
            }
            case VEC_REAL_TYPE: {
                void* t = real(((double*)v->value)[i]);
                void *r =  (*f)(t);
                vec_push(&res, r);
                free(r);
                free(t);
                break;
            }
            case VEC_STRING_TYPE: {
                char* str = ((char**)v->value)[i];
                t = string(str);
                vec_push(&res, (*f)(t));
                break;
            }
            default: {
                assert(0 && "unreachable");
            }

        }
    }
    return res;
}



