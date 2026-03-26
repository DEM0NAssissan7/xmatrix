#ifndef NUMBER_H
#define NUMBER_H

typedef struct Number
{
  float real;
  float imaginary;
  float magnitude;
  float angle;
  char is_valid;
} Number;

Number num_from_mag_angle(float magnitude, float angle);

Number num_from_real_imaginary(float real, float imaginary);

Number invalid_number();

Number add(Number a, Number b);

Number subtract(Number a, Number b);

Number multiply(Number a, Number b);

Number divide(Number numerator, Number denominator);

#endif