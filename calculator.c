#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "calculator.h"
#include "number.h"

#define PI 3.14159265358979323846 /* pi */
#define DEG_TO_RAD PI / 180

#define NUMBER 0
#define OPERATION 1

#define INVALID_OPCODE -1
#define ADD_OPCODE 0
#define SUBTRACT_OPCODE 1
#define MULTIPLY_OPCODE 2
#define DIVIDE_OPCODE 3
#define ANGLE_OPCODE 4
#define EXPONENT_OPCODE 5

// Parentheses
#define OPEN_PARENTHESIS 10
#define CLOSED_PARENTHESIS 11

// Parsing
#define DELIMITER ' '

/**
 * All strings must be null-terminated, or else this will not work
 */

OPCode
get_opcode(char c)
{
  switch (c)
  {
  case '+':
    return ADD_OPCODE;
  case '-':
    return SUBTRACT_OPCODE;
  case '*':
    return MULTIPLY_OPCODE;
  case '/':
    return DIVIDE_OPCODE;
  case '@':
    return ANGLE_OPCODE;
  case '^':
    return EXPONENT_OPCODE;
  case '(':
    return OPEN_PARENTHESIS;
  case ')':
    return CLOSED_PARENTHESIS;
  }
  return INVALID_OPCODE;
}

// Order of operations (PEMDAS)
int get_score(OPCode o)
{
  switch (o)
  {
  case OPEN_PARENTHESIS:
  case CLOSED_PARENTHESIS:
    return -1; // We do not use the score to calculate parentheses
  case ADD_OPCODE:
  case SUBTRACT_OPCODE:
    return 0;
  case MULTIPLY_OPCODE:
  case DIVIDE_OPCODE:
    return 1;
  case EXPONENT_OPCODE:
    return 2;
  case ANGLE_OPCODE:
    return 4;
  }
  return -1; // Default
}

Number num_from_token(const char *str)
{
  int is_imaginary = 0;
  char buffer[64];
  int len = 0;

  // Copy and detect 'i'
  for (int i = 0; str[i] != '\0'; i++)
  {
    char c = str[i];

    if (c == 'i')
    {
      is_imaginary = 1;
      continue;
    }

    if (len < 63)
      buffer[len++] = c;
    else
      return num_from_real_imaginary(0, 0);
  }

  buffer[len] = '\0';

  float value;

  // Handle "i"
  if (is_imaginary && len == 0)
    value = 1.0f;
  else
  {
    char *endptr;
    value = strtof(buffer, &endptr);

    if (endptr == buffer || *endptr != '\0')
      return num_from_real_imaginary(0, 0);
  }

  if (is_imaginary)
    return num_from_real_imaginary(0, value);
  else
    return num_from_real_imaginary(value, 0);
}

Number execute(Number num1, Number num2, OPCode opcode)
{
  switch (opcode)
  {
  case ADD_OPCODE:
    return add(num1, num2);
  case SUBTRACT_OPCODE:
    return subtract(num1, num2);
  case MULTIPLY_OPCODE:
    return multiply(num1, num2);
  case DIVIDE_OPCODE:
    return divide(num1, num2);
  case ANGLE_OPCODE:
    return num_from_mag_angle(num1.real, num2.real * DEG_TO_RAD);
  case EXPONENT_OPCODE:
    return exponentiate(num1, num2);
  }
  return invalid_number(); // Unreachable
}

void realloc_series(struct Series *s, int new_length)
{
  if (new_length > s->allocated_items)
  {
    s->allocated_items = new_length * 2;
    s->items = realloc(s->items, s->allocated_items * sizeof(struct Item));
  }
}

void add_item_to_series(struct Series *s, struct Item item)
{
  realloc_series(s, s->length + 1);
  s->items[s->length++] = item;
}

struct Series create_series(char *string)
{
  int length = strlen(string);

  struct Series s;
  s.length = 0;
  s.allocated_items = length;
  s.items = malloc(length * sizeof(struct Item)); // Take a worst-case guess at how much memory to allocate initially (helps w/ fragmentation)

  char c = DELIMITER;
  char previous_c = DELIMITER;
  char str[length]; // Allocate a stack array of the string size to always have a nice buffer in place
  int string_index = 0;

  struct Item item;
  item.type = NUMBER; // Assume it's a number by default
  OPCode previous_opcode = INVALID_OPCODE;
  OPCode opcode = INVALID_OPCODE;
  for (int i = 0; i <= length; i++)
  {
    previous_c = c;
    c = (i < length) ? string[i] : DELIMITER; // force flush at end
    previous_opcode = opcode;
    opcode = get_opcode(c);
    if (c == DELIMITER || ((previous_c != DELIMITER) && (previous_opcode == INVALID_OPCODE && opcode != INVALID_OPCODE)))
    {
      if (string_index == 0)
        continue; // Prevent working with blank strings

      str[string_index] = '\0'; // Null terminate before parsing

      item.type = NUMBER;
      item.number = num_from_token(str);
      s.items[s.length++] = item;

      string_index = 0;

      if (opcode != INVALID_OPCODE) // If we are delimiting because of an operation, we move back to the operation itself
        i--;

      continue;
    }

    if (opcode == INVALID_OPCODE) // If this is not an operation
    {
      str[string_index++] = c;
    }
    else
    {
      item.type = OPERATION;
      item.opcode = opcode;
      add_item_to_series(&s, item);
    }
  }

  // Now allocate the resulting series appropriately to the actual size
  s.items = realloc(s.items, s.length * sizeof(struct Item));
  s.allocated_items = s.length;
  return s;
}

void free_series(struct Series *s)
{
  free(s->items);
}

void splice_series(struct Series *s, int start_index, int end_index, struct Item replacement)
{
  // This function basically just replaces a range of elements in the series with a single element
  // end_index is inclusive

  /**
   * For example, we might have a series that looks like
   * [1, add, 2, multiply, 3, add, 4]
   * We want to replace index 2->4 with the result of 2x3 (6)
   * So the new series would look like
   * [1, add, 6, add 4]
   */

  // We begin by reassigning whatever the start index is to the replacement
  s->items[start_index] = replacement;
  int items_after = s->length - end_index - 1;

  // Now we move the elements that are ahead into the place after
  for (int i = 1; i <= items_after; i++)
  {
    s->items[start_index + i] = s->items[end_index + i];
  }

  // We reduce the length to fit the new one
  s->length -= end_index - start_index;
  // Note: We do not reallocate so that we keep a nice reasonable buffer. The helpers deal with if it ends up expanding in the future
}

void append_to_series(struct Series *s, int index, struct Item *replacements, int count)
{
  const int new_length = s->length + count;
  realloc_series(s, new_length);
  // Shift over elements (to the right)
  for (int i = s->length - 1; i >= index; i--)
  {
    s->items[i + count] = s->items[i];
  }
  s->length = new_length;

  for (int i = 0; i < count; i++)
  {
    s->items[index + i] = replacements[i];
  }
}

void collapse_unary_minuses(struct Series *series)
{
  for (int i = 0; i < series->length - 1; i++)
  {
    if (series->items[i].type == OPERATION &&
        series->items[i].opcode == SUBTRACT_OPCODE &&
        series->items[i + 1].type == NUMBER &&
        (i == 0 || series->items[i - 1].type == OPERATION))
    {
      struct Item replacement = series->items[i + 1];
      Number negative_one = num_from_real_imaginary(-1, 0);
      replacement.number = multiply(replacement.number, negative_one);

      splice_series(series, i, i + 1, replacement);
      i = -1; // restart scan
    }
  }
}

Number parse_series_arithmetic(struct Series *series)
{
  // If the first element has a negative, we just replace the minus operation and number with a negative version of the number
  if (series->length >= 2)
  {
    struct Item first_item = series->items[0];
    struct Item second_item = series->items[1];
    if (first_item.type == OPERATION && first_item.opcode == SUBTRACT_OPCODE && second_item.type == NUMBER)
    {
      Number negative_one = num_from_real_imaginary(-1, 0);
      second_item.number = multiply(second_item.number, negative_one);
      splice_series(series, 0, 1, second_item);
    }
  }

  collapse_unary_minuses(series);

  // Run through each operation one-by-one, find the one with the highest score + earliest index, and run + mutate in-memory
  int highest_score = -1;
  int score;
  int op_index = -1;
  struct Item item;
  while (series->length > 1) // We keep going until we only have a single item left in the series (i.e. the resulting number), then we return it
  {
    for (int i = 0; i < series->length; i++)
    {
      item = series->items[i];
      if (item.type != OPERATION)
        continue;
      score = get_score(item.opcode);
      if (score > highest_score)
      {
        highest_score = score;
        op_index = i;
      }
    }

    // If we have no operations left, we leave the while loop
    if (op_index == -1)
      break;

    // We compute the operation
    OPCode opcode = series->items[op_index].opcode;
    Number num1 = series->items[op_index - 1].number;
    Number num2 = series->items[op_index + 1].number;
    item.type = NUMBER;
    item.number = execute(num1, num2, opcode);
    // We splice the result in place of the expression
    splice_series(series, op_index - 1, op_index + 1, item);

    // State adjustment
    highest_score = -1; // Reset highest score
    op_index = -1;      // Reset operation index
  }

  // Step 3: Return the final element of the series
  return series->items[0].number;
}

void series_preprocessor(struct Series *s)
{
  // Parenthesis proximity multiplication
  struct Item left, right;
  struct Item mul;
  mul.type = OPERATION;
  mul.opcode = MULTIPLY_OPCODE;
  for (int i = 1; i < s->length; i++)
  {
    left = s->items[i - 1];
    right = s->items[i];

    int need_mul = 0;
    // 2(3) -> insert '*' between number and '('
    if (left.type == NUMBER &&
        right.type == OPERATION && right.opcode == OPEN_PARENTHESIS)
    {
      need_mul = 1;
    }
    // (2)3 -> insert '*' between ')' and number
    else if (left.type == OPERATION && left.opcode == CLOSED_PARENTHESIS &&
             right.type == NUMBER)
    {
      need_mul = 1;
    }

    if (need_mul)
    {
      // insert BETWEEN left (i-1) and right (i)
      append_to_series(s, i, &mul, 1);

      // skip past the inserted '*'
      i++; // now at the token after 'right'
      continue;
    }
  }
}

void parse_series_parentheses(struct Series *series)
{
  // Step 1: find all parentheses and create & run their series, and mutate result in-memory
  // We calculate the deepest parentheses first, calc & mutate, and then go from there until there are no more left

  struct Series s;
  s.items = malloc(series->length * sizeof(struct Item)); // Allocate the absolute max number of items that can fit
  s.allocated_items = series->length;
  s.length = 0;
  struct Item item;
  int start_index = -1;
  for (int i = 0; i < series->length; i++)
  {
    item = series->items[i];
    // If we are just working with normal strings
    if (item.type != OPERATION && start_index == -1)
    {
      continue;
    }
    if (item.type == OPERATION)
    {
      if (item.opcode == OPEN_PARENTHESIS)
      {
        start_index = i;
        // Reset series
        s.length = 0;
        continue;
      }
      else if (start_index == -1)
      {
        continue;
      }
      if (item.opcode == CLOSED_PARENTHESIS)
      {
        item.type = NUMBER;
        item.number = parse_series_arithmetic(&s);   // Parse the nested series we created
        splice_series(series, start_index, i, item); // Splice it into the larger series

        start_index = -1;
        i = -1;       // Reset for loop (we use -1 because continue will add 1 to i)
        s.length = 0; // Reset series
        continue;
      }
    }

    // Finally, add the item to the nested series
    add_item_to_series(&s, item);
  }
  free_series(&s);
}

Number parse_series(struct Series *series)
{
  series_preprocessor(series);
  // print_series(series); // For debugging

  parse_series_parentheses(series);

  return parse_series_arithmetic(series);
}

// String must be null-terminated
Number parse_string(char *string)
{
  struct Series s = create_series(string);
  Number result = parse_series(&s);
  free_series(&s);
  return result;
}