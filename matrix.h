#ifndef MATRIX_H
#define MATRIX_H

#include "number.h"

typedef struct Matrix
{
  Number *numbers;
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

// Managment
Matrix create_matrix(int rows, int cols);
void free_matrix(Matrix *m);

#endif
