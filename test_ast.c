#include <assert.h>
#include "ast.h"
#include "io.h"
#include "serialize.h"

environment_t test_environment()
{
    row_t*row = row_new(4);
    row->inputs[0] = variable_make_continuous(1.0);
    row->inputs[1] = variable_make_continuous(2.0);
    row->inputs[2] = variable_make_continuous(4.0);
    row->inputs[3] = variable_make_categorical(5);

    environment_t e;
    e.row = row;
    return e;
}

node_t*test_serialization(node_t*node)
{
    writer_t *w = growingmemwriter_new();
    node_write(node, w);
    node_destroy(node);
    reader_t*r = growingmemwriter_getreader(w);
    w->finish(w);
    node = node_read(r);
    r->dealloc(r);
    return node;
}

void test_if()
{
    START_CODE(node)
	IF
	    GT
		ADD
		    VAR(0)
		    VAR(1)
		END;
		VAR(2)
	    END;
	THEN
	    RETURN(1)
	ELSE
	    RETURN(2)
	END;
    END_CODE;

    node_print(node);

    node = test_serialization(node);

    environment_t env = test_environment();
    constant_t v = node_eval(node, &env);
    constant_print(&v);puts("\n");

    v = node_eval(node, &env);
    assert(v.c == 2);
    assert(v.type == CONSTANT_CATEGORY);
    env.row->inputs[2].value = 2.5;
    v = node_eval(node, &env);
    assert(v.c == 1);
    assert(v.type == CONSTANT_CATEGORY);

    row_destroy(env.row);
    node_destroy(node);
}

void test_array()
{
    START_CODE(node)
	IF
	    IN
                VAR(3)
		ARRAY_CONSTANT(array_create(3, 1,2,3));
	    END;
	THEN
	    RETURN(1)
	ELSE
	    RETURN(2)
	END;
    END_CODE;

    node_print(node);

    node = test_serialization(node);

    environment_t env = test_environment();

    constant_t v = node_eval(node, &env);
    assert(v.c == 2);
    assert(v.type == CONSTANT_CATEGORY);

    env.row->inputs[3].category = 3;
    v = node_eval(node, &env);
    assert(v.c == 1);
    assert(v.type == CONSTANT_CATEGORY);

    row_destroy(env.row);
    node_destroy(node);
}

int main()
{
    test_if();
    test_array();
    return 0;
}
