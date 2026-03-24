#include "matrix.h"

Number get(const Matrix *m, int row, int col)
{
  return m->numbers[row * m->columns + col];
}

void set(Matrix *m, int row, int col, Number value)
{
  m->numbers[row * m->columns + col] = value;
}

int matrix_multiply(const Matrix *a, const Matrix *b, Matrix *result)
{
  if (a->columns != b->rows)
  {
    result->rows = 0;
    result->columns = 0;
    return 0;
  }

  result->rows = a->rows;
  result->columns = b->columns;

  for (int i = 0; i < result->rows; i++)
  {
    for (int j = 0; j < result->columns; j++)
    {
      Number sum = num_from_real_imaginary(0, 0);

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

static void const_multiply(Number scalar, const Matrix *m, Matrix *new_matrix)
{
  new_matrix->rows = m->rows;
  new_matrix->columns = m->columns;

  for (int i = 0; i < m->rows; i++)
  {
    for (int j = 0; j < m->columns; j++)
    {
      Number old_value = get(m, i, j);
      Number new_value = multiply(old_value, scalar);
      set(new_matrix, i, j, new_value);
    }
  }
}

static Number determinant_2x2(const Matrix *m)
{
  Number a = get(m, 0, 0);
  Number b = get(m, 0, 1);
  Number c = get(m, 1, 0);
  Number d = get(m, 1, 1);

  return subtract(multiply(a, d), multiply(b, c));
}

static void matrix_minor(const Matrix *m, int remove_row, int remove_col, Matrix *minor)
{
  minor->rows = m->rows - 1;
  minor->columns = m->columns - 1;

  int dst = 0;
  for (int i = 0; i < m->rows; i++)
  {
    if (i == remove_row)
      continue;

    for (int j = 0; j < m->columns; j++)
    {
      if (j == remove_col)
        continue;

      minor->numbers[dst++] = get(m, i, j);
    }
  }
}

Number determinant(const Matrix *m)
{
  if (m->rows != m->columns)
  {
    return num_from_real_imaginary(0, 0);
  }

  if (m->rows == 1)
  {
    return get(m, 0, 0);
  }

  if (m->rows == 2)
  {
    return determinant_2x2(m);
  }

  Number result = num_from_real_imaginary(0, 0);

  for (int col = 0; col < m->columns; col++)
  {
    Matrix minor;
    matrix_minor(m, 0, col, &minor);

    Number term = multiply(get(m, 0, col), determinant(&minor));

    if (col % 2 == 1)
    {
      term = multiply(term, num_from_real_imaginary(-1, 0));
    }

    result = add(result, term);
  }

  return result;
}

int inverse(const Matrix *m, Matrix *result)
{
  if (m->rows != m->columns)
    return 0;

  Number d = determinant(m);
  if (d.magnitude == 0)
    return 0;

  if (m->rows == 1)
  {
    result->rows = 1;
    result->columns = 1;
    set(result, 0, 0, divide(num_from_real_imaginary(1, 0), get(m, 0, 0)));
    return 1;
  }

  Matrix adj;
  adj.rows = m->rows;
  adj.columns = m->columns;

  for (int i = 0; i < m->rows; i++)
  {
    for (int j = 0; j < m->columns; j++)
    {
      Matrix minor;
      matrix_minor(m, i, j, &minor);

      Number cofactor = determinant(&minor);
      if ((i + j) % 2 == 1)
        cofactor = multiply(cofactor, num_from_real_imaginary(-1, 0));

      set(&adj, j, i, cofactor);
    }
  }

  const_multiply(divide(num_from_real_imaginary(1, 0), d), &adj, result);
  return 1;
}
