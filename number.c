#include "number.h"
#include <math.h>

Number num_from_mag_angle(float magnitude, float angle)
{
  Number n;
  n.magnitude = magnitude;
  n.angle = angle;

  n.real = magnitude * cos(angle);
  n.imaginary = magnitude * sin(angle);
  n.is_valid = 1;
  return n;
}

Number num_from_real_imaginary(float real, float imaginary)
{
  Number n;
  n.real = real;
  n.imaginary = imaginary;

  n.magnitude = sqrt(real * real + imaginary * imaginary);
  n.angle = atan2(imaginary, real);
  n.is_valid = 1;
  return n;
}

Number invalid_number()
{
  Number n;
  n.real = 0;
  n.imaginary = 0;
  n.magnitude = 0;
  n.angle = 0;
  n.is_valid = 0;
  return n;
}

char has_invalid(Number a, Number b)
{
  if (a.is_valid && b.is_valid)
    return 0;
  return 1;
}

Number add(Number a, Number b)
{
  if (has_invalid(a, b))
    return invalid_number();
  float real = a.real + b.real;
  float imaginary = a.imaginary + b.imaginary;
  return num_from_real_imaginary(real, imaginary);
}

Number subtract(Number a, Number b)
{
  if (has_invalid(a, b))
    return invalid_number();
  Number negative_one = num_from_real_imaginary(-1, 0);
  return add(a, multiply(b, negative_one));
}

Number multiply(Number a, Number b)
{
  if (has_invalid(a, b))
    return invalid_number();
  float magnitude = a.magnitude * b.magnitude;
  float angle = a.angle + b.angle;
  return num_from_mag_angle(magnitude, angle);
}

Number divide(Number numerator, Number denominator)
{
  if (has_invalid(numerator, denominator))
    return invalid_number();
  if (denominator.magnitude == 0)
    return numerator;
  float magnitude = numerator.magnitude / denominator.magnitude;
  float angle = numerator.angle - denominator.angle;
  return num_from_mag_angle(magnitude, angle);
}

Number exponentiate(Number base, Number power)
{
  if (has_invalid(base, power))
    return invalid_number();
  float magnitude = pow(base.magnitude, power.real);
  float angle = power.real * base.angle;
  return num_from_mag_angle(magnitude, angle);
}