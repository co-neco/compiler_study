#pragma once

#include "defs.h"

struct ASTnode* compound_statement();
struct ASTnode* funccall();
struct ASTnode* array_value(int symid, struct ASTnode* index);