// Other stuffs
#ifndef CALCULATOR_H
#define CALCULATOR_H

#include "number.h"

// Types
typedef char OPCode;
typedef char ItemType;

// Structures
struct Item
{
  ItemType type; // This determines which value to use
  Number number;
  OPCode opcode;
};
struct Series
{
  struct Item *items; // Must be a memory allocated pointer or a valid stack pointer
  int length;
  int allocated_items;
};

// Functions
Number parse_string(char *string);

#endif