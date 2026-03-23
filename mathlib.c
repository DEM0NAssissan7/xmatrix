#include "mathlib.h"
#include "number.h"

#include <math.h>
#include <stdlib.h> // For strtof and strtod
#include <string.h>

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
  case ':':
    return ANGLE_OPCODE;
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
  case ANGLE_OPCODE:
    return 2;
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
    return num_from_mag_angle(num1.real, num2.real);
  }
  return num_from_real_imaginary(0, 0); // Unreachable
}

struct Series create_series(char *string)
{
  struct Series s;
  s.length = 0;

  int length = strlen(string);
  char c;
  char previous_c;
  char str[MAX_NUMBER_LENGTH];
  int string_index = 0;

  char is_end = 0;

  struct Item item;
  item.type = NUMBER; // Assume it's a number by default
  OPCode previous_opcode;
  OPCode opcode;
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
      Number num = num_from_token(str);

      item.type = NUMBER;
      item.number = num;
      s.items[s.length] = item;
      s.length++;

      string_index = 0;

      if (opcode != INVALID_OPCODE) // If we are delimiting because of an operation, we move back to the operation itself
        i--;

      continue;
    }

    if (opcode == INVALID_OPCODE) // If this is not an operation
    {
      str[string_index] = c;
      string_index++;
    }
    else
    {
      item.type = OPERATION;
      item.opcode = opcode;
      s.items[s.length] = item;
      s.length++;
    }

    if (i == length - 1)
    {
      is_end = 1;
      continue;
    }
  }

  return s;
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

  // We modify the length to fit the new one
  s->length = s->length - end_index + start_index;
}

Number parse_series(struct Series series)
{
  // Step 1: find all parentheses and create & run their series, and mutate result in-memory
  // We calculate the deepest parentheses first, calc & mutate, and then go from there until there are no more left
  struct Series s;
  s.length = 0;
  struct Item item;
  int start_index = -1;
  for (int i = 0; i < series.length; i++)
  {
    item = series.items[i];
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
        item.number = parse_series(s);                // Parse the nested series we created
        splice_series(&series, start_index, i, item); // Splice it into the larger series

        start_index = -1;
        i = -1;       // Reset for loop (we use -1 because continue will add 1 to i)
        s.length = 0; // Reset series
        continue;
      }
    }

    // Finally, add the item to the nested series
    s.items[s.length] = item;
    s.length++;
  }

  // Step 2: run through each operation one-by-one, find the one with the highest score + earliest index, and run + mutate in-memory
  int highest_score = -1;
  int score;
  int op_index = -1;
  while (series.length > 1) // We keep going until we only have a single item left in the series (i.e. the resulting number), then we return it
  {
    for (int i = 0; i < series.length; i++)
    {
      item = series.items[i];
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
    OPCode opcode = series.items[op_index].opcode;
    Number num1 = series.items[op_index - 1].number;
    Number num2 = series.items[op_index + 1].number;
    item.type = NUMBER;
    item.number = execute(num1, num2, opcode);
    // We splice the result in place of the expression
    splice_series(&series, op_index - 1, op_index + 1, item);

    // State adjustment
    highest_score = -1; // Reset highest score
    op_index = -1;      // Reset operation index
  }

  // Step 3: Return the final element of the series
  return series.items[0].number;
}

// String must be null-terminated
Number parse_string(char *string)
{
  struct Series s = create_series(string);
  Number result = parse_series(s);
  return result;
}