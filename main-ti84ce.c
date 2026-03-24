#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <tice.h>
#include <ti/screen.h>

#include "matrix.h"
#include "calculator.h"

#define PI 3.14159265358979323846
#define MAX_INPUT 31
#define MAX_DIM 8

static void ti_println(const char *text)
{
  os_PutStrFull(text);
  os_NewLine();
}

static uint8_t wait_keypress(void)
{
  uint8_t key;
  do
  {
    key = os_GetCSC();
  } while (key == 0);

  while (os_GetCSC() != 0)
  {
  }

  return key;
}

static void wait_for_clear(void)
{
  ti_println("");
  ti_println("Press CLEAR...");
  while (wait_keypress() != sk_Clear)
  {
  }
}

static void ti_print_float(float value)
{
  real_t real_value = os_FloatToReal(value);
  char out[20];
  os_RealToStr(out, &real_value, 10, 1, -1);
  os_PutStrFull(out);
}

static void ti_print_complex(Number n)
{
  ti_print_float(n.real);
  if (n.imaginary < 0)
  {
    os_PutStrFull(" - ");
    ti_print_float(fabsf(n.imaginary));
  }
  else
  {
    os_PutStrFull(" + ");
    ti_print_float(n.imaginary);
  }
  os_PutStrFull("i");
}

static void render_input(const char *prompt, const char *buffer)
{
  os_ClrHome();
  ti_println(prompt);
  ti_println(buffer);
  ti_println("");
  ti_println("ENT=OK DEL=Back");
  ti_println("CLEAR=Cancel");
  ti_println("VARS=i COMMA=:");
}

static int key_to_char(uint8_t key, char *out)
{
  switch (key)
  {
  case sk_0:
    *out = '0';
    return 1;
  case sk_1:
    *out = '1';
    return 1;
  case sk_2:
    *out = '2';
    return 1;
  case sk_3:
    *out = '3';
    return 1;
  case sk_4:
    *out = '4';
    return 1;
  case sk_5:
    *out = '5';
    return 1;
  case sk_6:
    *out = '6';
    return 1;
  case sk_7:
    *out = '7';
    return 1;
  case sk_8:
    *out = '8';
    return 1;
  case sk_9:
    *out = '9';
    return 1;
  case sk_Add:
    *out = '+';
    return 1;
  case sk_Sub:
  case sk_Chs:
    *out = '-';
    return 1;
  case sk_Mul:
    *out = '*';
    return 1;
  case sk_Div:
    *out = '/';
    return 1;
  case sk_LParen:
    *out = '(';
    return 1;
  case sk_RParen:
    *out = ')';
    return 1;
  case sk_DecPnt:
    *out = '.';
    return 1;
  case sk_Vars:
  case sk_Log:
  case sk_Ln:
    *out = 'i';
    return 1;
  case sk_Comma:
  case sk_Stat:
    *out = ':';
    return 1;
  default:
    return 0;
  }
}

static int input_line(const char *prompt, char *buffer, int max_len)
{
  int len = 0;
  buffer[0] = '\0';

  while (1)
  {
    render_input(prompt, buffer);
    uint8_t key = wait_keypress();

    if (key == sk_Enter)
      return 1;
    if (key == sk_Clear)
      return 0;

    if (key == sk_Del)
    {
      if (len > 0)
      {
        len--;
        buffer[len] = '\0';
      }
      continue;
    }

    char c;
    if (key_to_char(key, &c) && len < max_len - 1)
    {
      buffer[len++] = c;
      buffer[len] = '\0';
    }
  }
}

static int input_int(const char *prompt, int min, int max, int *value)
{
  char input[MAX_INPUT + 1];
  char *endptr;

  while (1)
  {
    if (!input_line(prompt, input, sizeof(input)))
      return 0;

    long parsed = strtol(input, &endptr, 10);
    if (endptr != input && *endptr == '\0' && parsed >= min && parsed <= max)
    {
      *value = (int)parsed;
      return 1;
    }

    os_ClrHome();
    ti_println("Invalid integer.");
    ti_println("Try again.");
    wait_for_clear();
  }
}

static Matrix prompt_matrix(const char *name, int *ok)
{
  Matrix m;
  memset(&m, 0, sizeof(m));

  char prompt[24];
  prompt[0] = 'R';
  prompt[1] = 'o';
  prompt[2] = 'w';
  prompt[3] = 's';
  prompt[4] = ' ';
  prompt[5] = name[0];
  prompt[6] = '?';
  prompt[7] = '\0';

  if (!input_int(prompt, 1, MAX_DIM, &m.rows))
  {
    *ok = 0;
    return m;
  }

  prompt[0] = 'C';
  prompt[1] = 'o';
  prompt[2] = 'l';
  prompt[3] = 's';
  prompt[4] = ' ';
  prompt[5] = name[0];
  prompt[6] = '?';
  prompt[7] = '\0';

  if (!input_int(prompt, 1, MAX_DIM, &m.columns))
  {
    *ok = 0;
    return m;
  }

  char input[MAX_INPUT + 1];
  for (int i = 0; i < m.rows; i++)
  {
    for (int j = 0; j < m.columns; j++)
    {
      prompt[0] = name[0];
      prompt[1] = '[';
      prompt[2] = '0' + i;
      prompt[3] = ']';
      prompt[4] = '[';
      prompt[5] = '0' + j;
      prompt[6] = ']';
      prompt[7] = '=';
      prompt[8] = '\0';

      if (!input_line(prompt, input, sizeof(input)))
      {
        *ok = 0;
        return m;
      }

      set(&m, i, j, parse_string(input));
    }
  }

  *ok = 1;
  return m;
}

static void print_matrix(Matrix m, const char *name)
{
  os_ClrHome();
  ti_println(name);

  for (int i = 0; i < m.rows; i++)
  {
    for (int j = 0; j < m.columns; j++)
    {
      Number n = get(&m, i, j);
      ti_print_complex(n);
      os_PutStrFull("<");
      ti_print_float(n.magnitude);
      os_PutStrFull("@");
      ti_print_float(n.angle * (180.0f / (float)PI));
      os_PutStrFull(">");
      os_NewLine();
    }
  }
}

static int wait_menu_choice(void)
{
  while (1)
  {
    os_ClrHome();
    ti_println("Choose operation:");
    ti_println("1 Print A and B");
    ti_println("2 Multiply A*B");
    ti_println("3 Determinant A");
    ti_println("4 Determinant B");
    ti_println("5 Re-enter A");
    ti_println("6 Re-enter B");
    ti_println("7 Compute A^-1*B");
    ti_println("8 Compute A*B^-1");
    ti_println("0 Quit");

    uint8_t key = wait_keypress();
    switch (key)
    {
    case sk_0:
      return 0;
    case sk_1:
      return 1;
    case sk_2:
      return 2;
    case sk_3:
      return 3;
    case sk_4:
      return 4;
    case sk_5:
      return 5;
    case sk_6:
      return 6;
    case sk_7:
      return 7;
    case sk_8:
      return 8;
    default:
      break;
    }
  }
}

int main(void)
{
  int ok = 0;
  Matrix A = prompt_matrix("A", &ok);
  if (!ok)
    return 0;

  Matrix B = prompt_matrix("B", &ok);
  if (!ok)
    return 0;

  int choice;
  do
  {
    choice = wait_menu_choice();

    if (choice == 1)
    {
      print_matrix(A, "A =");
      wait_for_clear();
      print_matrix(B, "B =");
      wait_for_clear();
    }
    else if (choice == 2)
    {
      os_ClrHome();
      if (A.columns == B.rows)
      {
        Matrix C;
        matrix_multiply(&A, &B, &C);
        print_matrix(C, "A * B =");
      }
      else
      {
        ti_println("Cannot multiply A*B.");
      }
      wait_for_clear();
    }
    else if (choice == 3)
    {
      os_ClrHome();
      if (A.rows == A.columns)
      {
        Number d = determinant(&A);
        ti_println("det(A) =");
        ti_print_complex(d);
        os_NewLine();
      }
      else
      {
        ti_println("A is not square.");
      }
      wait_for_clear();
    }
    else if (choice == 4)
    {
      os_ClrHome();
      if (B.rows == B.columns)
      {
        Number d = determinant(&B);
        ti_println("det(B) =");
        ti_print_complex(d);
        os_NewLine();
      }
      else
      {
        ti_println("B is not square.");
      }
      wait_for_clear();
    }
    else if (choice == 5)
    {
      Matrix new_a = prompt_matrix("A", &ok);
      if (ok)
        A = new_a;
    }
    else if (choice == 6)
    {
      Matrix new_b = prompt_matrix("B", &ok);
      if (ok)
        B = new_b;
    }
    else if (choice == 7)
    {
      os_ClrHome();
      Matrix A_inv;
      if (!inverse(&A, &A_inv))
      {
        ti_println("A is not invertible.");
      }
      else if (A_inv.columns != B.rows)
      {
        ti_println("Cannot do A^-1 * B.");
      }
      else
      {
        Matrix C;
        matrix_multiply(&A_inv, &B, &C);
        print_matrix(C, "A^-1 * B =");
      }
      wait_for_clear();
    }
    else if (choice == 8)
    {
      os_ClrHome();
      Matrix B_inv;
      if (!inverse(&B, &B_inv))
      {
        ti_println("B is not invertible.");
      }
      else if (A.columns != B_inv.rows)
      {
        ti_println("Cannot do A * B^-1.");
      }
      else
      {
        Matrix C;
        matrix_multiply(&A, &B_inv, &C);
        print_matrix(C, "A * B^-1 =");
      }
      wait_for_clear();
    }
  } while (choice != 0);

  os_ClrHome();
  ti_println("Bye.");
  return 0;
}
