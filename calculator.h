// Other stuffs
#ifndef CALCULATOR_H
#define CALCULATOR_H

#include "number.h"

#define MAX_ITEMS 32
#define MAX_NUMBER_LENGTH 32

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
  struct Item items[MAX_ITEMS]; // Must be a memory allocated pointer or a valid stack pointer
  int length;
};

// Functions
Number parse_string(char *string);

#endif