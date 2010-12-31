/* ast.h

   AST representation of prediction programs.

   Copyright (c) 2010 Matthias Kramm <kramm@quiss.org>
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */

#ifndef __ast_h__
#define __ast_h__

#include <stdio.h>
#include <stdbool.h>
#include "model.h"

typedef struct _node node_t;
typedef struct _nodetype nodetype_t;
typedef struct _value value_t;
typedef struct _environment environment_t;

#define NODE_FLAG_HAS_CHILDREN 1

#define TYPE_FLOAT 1
#define TYPE_CATEGORY 2
#define TYPE_INT 3
#define TYPE_BOOL 4
#define TYPE_MISSING 5

struct _value {
    union {
        float f;
	category_t c;
	int i;
	bool b;
    };
    uint8_t type;
};

void value_print(value_t*v);

struct _environment {
    row_t* row;
};

struct _nodetype {
    char*name;
    int flags;
    int min_args;
    int max_args;
    value_t (*eval)(node_t*n, environment_t* params);
};

struct _node {
    nodetype_t*type;
    node_t*parent;
    union {
        struct {
            node_t**child;
            int num_children;
        };
	value_t value;
    };
};

extern nodetype_t node_root;
extern nodetype_t node_if;
extern nodetype_t node_add;
extern nodetype_t node_lt;
extern nodetype_t node_gt;
extern nodetype_t node_var;
extern nodetype_t node_category;

node_t* node_new(nodetype_t*t, ...);
void node_free(node_t*n);
value_t node_eval(node_t*n,environment_t* e);
void node_print(node_t*n);


/* the following convenience macros allow to write code
   in the following matter:

   START_CODE
    IF 
      GT
	ADD
	  VAR(1)
	  VAR(1)
        VAR(3)
    THEN
      RETURN(1)
    ELSE
      RETURN(1)
   END_CODE

*/
#define NODE_BEGIN(new_type,args...) \
	    do { \
		node_t* new_node = node_new(new_type,##args); \
		if(current_node) { \
		    assert(current_node->type); \
		    assert(current_node->type->name); \
		    if(current_node->num_children >= current_node->type->max_args) { \
			fprintf(stderr, "Too many arguments (%d) to node (max %d args)\n", \
				current_node->num_children, \
				current_node->type->max_args); \
		    } \
		    assert(current_node->num_children < current_node->type->max_args); \
		    current_node->child[current_node->num_children++] = new_node; \
		} \
		if((new_node->type->flags) & NODE_FLAG_HAS_CHILDREN) { \
		    new_node->parent = current_node; \
		    current_node = new_node; \
		} \
	    } while(0);

#define START_CODE(program) \
	node_t* program; \
	{ \
	    node_t*current_node = program = node_new(&node_root); \
	    program = current_node;

#define END_CODE \
	}

#define END do {assert(current_node->num_children >= current_node->type->min_args && \
	               current_node->num_children <= current_node->type->max_args);\
                current_node = current_node->parent; \
	       } while(0)

#define IF NODE_BEGIN(&node_if)
#define THEN assert(current_node && current_node->type == &node_if && current_node->num_children == 1);
#define ELSE assert(current_node && current_node->type == &node_if && current_node->num_children == 2);
#define ADD NODE_BEGIN(&node_add)
#define LT NODE_BEGIN(&node_lt)
#define GT NODE_BEGIN(&node_gt)
#define VAR(i) NODE_BEGIN(&node_var, i)
#define RETURN(n) NODE_BEGIN(&node_category, n)

#endif
