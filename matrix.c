#include "matrix.h"
#include <stdlib.h>

static Number num_zero(void)
{
  return num_from_real_imaginary(0.0f, 0.0f);
}

static Number num_one(void)
{
  return num_from_real_imaginary(1.0f, 0.0f);
}

static Number num_negative_one(void)
{
  return num_from_real_imaginary(-1.0f, 0.0f);
}

static int is_zero(Number n)
{
  return n.magnitude == 0.0f;
}

static int matrix_resize(Matrix *m, int rows, int cols)
{
  Number *new_numbers = (Number *)realloc(m->numbers, rows * cols * sizeof(Number));
  if (new_numbers == NULL && rows * cols > 0)
  {
    return 0;
  }

  m->numbers = new_numbers;
  m->rows = rows;
  m->columns = cols;
  return 1;
}

static void swap_rows(Matrix *m, int row_a, int row_b)
{
  if (row_a == row_b)
    return;

  for (int col = 0; col < m->columns; col++)
  {
    Number temp = get(m, row_a, col);
    set(m, row_a, col, get(m, row_b, col));
    set(m, row_b, col, temp);
  }
}

static int find_pivot_row(const Matrix *m, int start_row, int col)
{
  int pivot_row = -1;
  float best_mag = 0.0f;

  for (int row = start_row; row < m->rows; row++)
  {
    Number value = get(m, row, col);
    if (value.magnitude > best_mag)
    {
      best_mag = value.magnitude;
      pivot_row = row;
    }
  }

  return pivot_row;
}

static void set_identity(Matrix *m)
{
  for (int i = 0; i < m->rows; i++)
  {
    for (int j = 0; j < m->columns; j++)
    {
      set(m, i, j, (i == j) ? num_one() : num_zero());
    }
  }
}

static int copy_matrix(const Matrix *src, Matrix *dst)
{
  if (!matrix_resize(dst, src->rows, src->columns))
    return 0;

  int count = src->rows * src->columns;
  for (int i = 0; i < count; i++)
  {
    dst->numbers[i] = src->numbers[i];
  }

  return 1;
}

Number get(const Matrix *m, int row, int col)
{
  return m->numbers[row * m->columns + col];
}

void set(Matrix *m, int row, int col, Number value)
{
  m->numbers[row * m->columns + col] = value;
}

Matrix create_matrix(int rows, int cols)
{
  Matrix m;
  m.rows = rows;
  m.columns = cols;
  m.numbers = NULL;

  if (rows > 0 && cols > 0)
  {
    m.numbers = (Number *)malloc(rows * cols * sizeof(Number));
  }

  return m;
}

void free_matrix(Matrix *m)
{
  free(m->numbers);
  m->numbers = NULL;
  m->rows = 0;
  m->columns = 0;
}

int matrix_multiply(const Matrix *a, const Matrix *b, Matrix *result)
{
  if (a->columns != b->rows)
  {
    return 0;
  }

  if (!matrix_resize(result, a->rows, b->columns))
  {
    return 0;
  }

  for (int i = 0; i < result->rows; i++)
  {
    for (int j = 0; j < result->columns; j++)
    {
      Number sum = num_zero();

      for (int k = 0; k < a->columns; k++)
      {
        Number product = multiply(get(a, i, k), get(b, k, j));
        sum = add(sum, product);
      }

      set(result, i, j, sum);
    }
  }

  return 1;
}

Number determinant(const Matrix *m)
{
  if (m->rows != m->columns)
  {
    return num_zero();
  }

  if (m->rows == 0)
  {
    return num_zero();
  }

  Matrix temp = create_matrix(m->rows, m->columns);
  if (temp.numbers == NULL)
  {
    return num_zero();
  }

  for (int i = 0; i < m->rows * m->columns; i++)
  {
    temp.numbers[i] = m->numbers[i];
  }

  Number det = num_one();
  int sign = 1;

  for (int col = 0; col < temp.columns; col++)
  {
    int pivot_row = find_pivot_row(&temp, col, col);
    if (pivot_row == -1 || is_zero(get(&temp, pivot_row, col)))
    {
      free_matrix(&temp);
      return num_zero();
    }

    if (pivot_row != col)
    {
      swap_rows(&temp, pivot_row, col);
      sign = -sign;
    }

    Number pivot = get(&temp, col, col);

    for (int row = col + 1; row < temp.rows; row++)
    {
      Number below = get(&temp, row, col);
      if (is_zero(below))
        continue;

      Number factor = divide(below, pivot);

      for (int k = col; k < temp.columns; k++)
      {
        Number value = subtract(
            get(&temp, row, k),
            multiply(factor, get(&temp, col, k)));
        set(&temp, row, k, value);
      }
    }

    det = multiply(det, pivot);
  }

  if (sign < 0)
  {
    det = multiply(det, num_negative_one());
  }

  free_matrix(&temp);
  return det;
}

int inverse(const Matrix *m, Matrix *result)
{
  if (m->rows != m->columns)
  {
    return 0;
  }

  int n = m->rows;
  if (n <= 0)
  {
    return 0;
  }

  Matrix left = create_matrix(n, n);
  Matrix right = create_matrix(n, n);

  if (left.numbers == NULL || right.numbers == NULL)
  {
    free_matrix(&left);
    free_matrix(&right);
    return 0;
  }

  if (!copy_matrix(m, &left))
  {
    free_matrix(&left);
    free_matrix(&right);
    return 0;
  }

  set_identity(&right);

  for (int col = 0; col < n; col++)
  {
    int pivot_row = find_pivot_row(&left, col, col);
    if (pivot_row == -1 || is_zero(get(&left, pivot_row, col)))
    {
      free_matrix(&left);
      free_matrix(&right);
      return 0;
    }

    if (pivot_row != col)
    {
      swap_rows(&left, pivot_row, col);
      swap_rows(&right, pivot_row, col);
    }

    Number pivot = get(&left, col, col);

    for (int j = 0; j < n; j++)
    {
      set(&left, col, j, divide(get(&left, col, j), pivot));
      set(&right, col, j, divide(get(&right, col, j), pivot));
    }

    for (int row = 0; row < n; row++)
    {
      if (row == col)
        continue;

      Number factor = get(&left, row, col);
      if (is_zero(factor))
        continue;

      for (int j = 0; j < n; j++)
      {
        set(&left, row, j,
            subtract(get(&left, row, j),
                     multiply(factor, get(&left, col, j))));

        set(&right, row, j,
            subtract(get(&right, row, j),
                     multiply(factor, get(&right, col, j))));
      }
    }
  }

  if (!matrix_resize(result, n, n))
  {
    free_matrix(&left);
    free_matrix(&right);
    return 0;
  }

  for (int i = 0; i < n * n; i++)
  {
    result->numbers[i] = right.numbers[i];
  }

  free_matrix(&left);
  free_matrix(&right);
  return 1;
}