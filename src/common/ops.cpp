#include "ops.h"

std::string op_to_string(EOp op) {
  switch (op) {
  case EOp::ADD:
    return "+";
  case EOp::SUB:
    return "-";
  case EOp::MUL:
    return "*";
  case EOp::AND:
    return "&";
  case EOp::L_SHIFT:
    return "<<";
  case EOp::R_SHIFT:
    return ">>";
  case EOp::LT:
    return "<";
  case EOp::LTEQ:
    return "<=";
  case EOp::GT:
    return ">";
  case EOp::GTEQ:
    return ">=";
  case EOp::EQ:
    return "=";
  default:
    std::cerr << "Invalid EOp in to_string()\n";
    throw;
  }
}