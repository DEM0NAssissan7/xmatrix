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
Number get(const Matrix *m, int row, int col);
void set(Matrix *m, int row, int col, Number value);

// Basic operations
int matrix_multiply(const Matrix *a, const Matrix *b, Matrix *result);

// Determinant-related
Number determinant(const Matrix *m);

// Inverse
int inverse(const Matrix *m, Matrix *result);

#endif
