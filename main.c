#include <stdlib.h>
#include <stdio.h>
#include "vector.h"


int main(void) 
{
    {
        vector_t v = vector_init(VECTOR_INT_TYPE);
        vector_push(&v, vector_decimal(69));
        vector_pprint(v);
        vector_free(v);

        v = vector_from_range(1, 10+1);
        vector_pprint(v);
        vector_free(v);
    }
    {
        vector_t v = vector_from_double(4, 1.0, 2.3, 3.14, 4.14);
        vector_push(&v, vector_real(5.11));
        vector_pprint(v);
        vector_free(v); 
    }
    {
        vector_t v = vector_init(VECTOR_STRING_TYPE);
        vector_push(&v, "hello");
        vector_push(&v, "world!");
        vector_pprint(v);
        vector_free(v);
    }
    return 0;

}
