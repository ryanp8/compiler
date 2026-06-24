#pragma once

#include <string>
#include <iostream>

enum EOp { ADD, SUB, MUL, AND, L_SHIFT, R_SHIFT, LT, LTEQ, EQ, GTEQ, GT };

std::string op_to_string(EOp op);