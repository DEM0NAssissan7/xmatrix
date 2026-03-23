#ifndef MATRIX_H
#define MATRIX_H

#include "number.h"

#define MAX_ARRAY_NUMS 64 // or whatever you defined

typedef struct Matrix
{
  Number numbers[MAX_ARRAY_NUMS];
  int rows;
  int columns;
} Matrix;

// Helpers
Number get(Matrix m, int row, int col);
void set(Matrix *m, int row, int col, Number value);

// Basic operations
Matrix matrix_multiply(Matrix a, Matrix b);
Matrix const_multiply(Number a, Matrix m);

// Determinant-related
Number determinant(Matrix m);

// Inverse
Matrix inverse(Matrix m);

#endif