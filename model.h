#ifndef __model_h__
#define __model_h__
#include <stdint.h>
//#include "io.h"

typedef int32_t category_t;
typedef enum {CATEGORICAL,CONTINUOUS,MISSING} columntype_t;

/* input variable (a.k.a. "free" variable) */
typedef struct _variable {
    columntype_t type;
    union {
	category_t category;
	float value;
    };
} variable_t;

variable_t variable_make_categorical(category_t c);
variable_t variable_make_continuous(float v);
variable_t variable_make_missing();

typedef struct _row {
    int num_inputs;
    variable_t inputs[0];
} row_t;

row_t*row_new(int num_inputs);

/* input variable with column info (for sparse rows) */
typedef struct _variable_and_position {
    int index;
    columntype_t type;
    union {
	category_t category;
	float value;
    };
} variable_and_position_t;

typedef struct _sparse_row {
    variable_and_position_t*inputs;
    int num_inputs;
} sparse_row_t;


/* a single "row" in the data, combining a single known output with
   the corresponding inputs */
typedef struct _example {
    row_t row;
    category_t desired_output;
} example_t;

typedef struct _model {
    uint32_t id;
    int num_inputs;

    int (*description_length)();
    
    category_t (*predict)(row_t row);
    
//    void (*write)(writer_t*writer);

    void*internal;
} model_t;

typedef struct _model_trainer {
    model_t*(*train)(example_t*examples, int num_examples);
    void*internal;
} model_trainer_t;



#endif