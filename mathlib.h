// Other stuffs
#ifndef CALCULATOR_H
#define CALCULATOR_H

#define MAX_ITEMS 32
#define MAX_NUMBER_LENGTH 32

#define INTRO_MESSAGE                                                                                          \
  "This calculator is fully-featured. All arithmetic operations, including exponents and parenthesis, work.\n" \
  "You can string together multiple operations, and they will execute in accordance with PEMDAS\n\n"           \
  "Syntax:\naddition: +\nsubtraction: -\nmultiplication: *\ndivision: /\nexponent: ^\n\n"                      \
  "Example:\n> (1+3)^2 + 9(2) * 1\n= 34\n\n"

// Types
typedef float Number;
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