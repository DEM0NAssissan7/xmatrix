#include <stdlib.h>
#include <tice.h>
#include <ti/screen.h>
#include "number.h"

void clear_screen()
{
  os_ClrHome();
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
    *out = '∠';
    return 1;
  default:
    return 0;
  }
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

static void printfloat(float value)
{
  real_t real_value = os_FloatToReal(value);
  char out[20];
  os_RealToStr(out, &real_value, 10, 1, -1);
  os_PutStrFull(out);
}
static void printnum(Number n)
{
  ti_print_float(n.real);
  if (n.imaginary)
  {
    if (n.imaginary < 0)
    {
      os_PutStrFull("-");
      ti_print_float(fabsf(n.imaginary));
    }
    else
    {
      os_PutStrFull("+");
      ti_print_float(n.imaginary);
    }
    os_PutStrFull("i\n<");

    ti_print_float(n.magnitude);
    os_PutStrFull("∠");
    ti_print_float(n.angle);
    os_PutStrFull("°>");
  }
}
