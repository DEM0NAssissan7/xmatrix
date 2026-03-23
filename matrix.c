#include "matrix.h"

// Helpers
Number get(Matrix m, int row, int col)
{
  return m.numbers[row * m.columns + col];
}
void set(Matrix *m, int row, int col, Number value)
{
  m->numbers[row * m->columns + col] = value;
}

Matrix matrix_multiply(Matrix a, Matrix b)
{
  Matrix result;

  if (a.columns != b.rows)
  {
    result.rows = 0;
    result.columns = 0;
    return result;
  }

  result.rows = a.rows;
  result.columns = b.columns;

  for (int i = 0; i < result.rows; i++)
  {
    for (int j = 0; j < result.columns; j++)
    {
      Number sum = num_from_real_imaginary(0, 0);

      for (int k = 0; k < a.columns; k++)
      {
        Number product = multiply(get(a, i, k), get(b, k, j));
        sum = add(sum, product);
      }

      set(&result, i, j, sum);
    }
  }

  return result;
}
Matrix const_multiply(Number a, Matrix m)
{
  Matrix new_matrix;
  new_matrix.rows = m.rows;
  new_matrix.columns = m.columns;
  Number old;
  Number new;
  for (int i = 0; i < m.rows; i++)
  {
    for (int j = 0; j < m.columns; j++)
    {
      old = get(m, i, j);
      new = multiply(old, a);
      set(&new_matrix, i, j, new);
    }
  }
  return new_matrix;
}

// a: left, b: right, 0: top, 1: bottom
Number determinant_2x2(Matrix m)
{
  Number a = get(m, 0, 0);
  Number b = get(m, 0, 1);
  Number c = get(m, 1, 0);
  Number d = get(m, 1, 1);

  return subtract(multiply(a, d), multiply(b, c));
}

Matrix matrix_minor(Matrix m, int remove_row, int remove_col)
{
  Matrix minor;
  minor.rows = m.rows - 1;
  minor.columns = m.columns - 1;

  int dst = 0;
  for (int i = 0; i < m.rows; i++)
  {
    if (i == remove_row)
      continue;

    for (int j = 0; j < m.columns; j++)
    {
      if (j == remove_col)
        continue;

      minor.numbers[dst++] = get(m, i, j);
    }
  }

  return minor;
}

Number determinant(Matrix m)
{
  if (m.rows != m.columns)
  {
    return num_from_real_imaginary(0, 0);
  }

  if (m.rows == 1)
  {
    return get(m, 0, 0);
  }

  if (m.rows == 2)
  {
    return determinant_2x2(m);
  }

  Number result = num_from_real_imaginary(0, 0);

  for (int col = 0; col < m.columns; col++)
  {
    Number term = multiply(get(m, 0, col),
                           determinant(matrix_minor(m, 0, col)));

    if (col % 2 == 1)
    {
      term = multiply(term, num_from_real_imaginary(-1, 0));
    }

    result = add(result, term);
  }

  return result;
}

Matrix inverse(Matrix m)
{
  Matrix invalid;
  invalid.rows = 0;
  invalid.columns = 0;

  if (m.rows != m.columns)
    return invalid;

  Number d = determinant(m);
  if (d.magnitude == 0)
    return invalid;

  if (m.rows == 1)
  {
    Matrix result;
    result.rows = 1;
    result.columns = 1;
    set(&result, 0, 0, divide(num_from_real_imaginary(1, 0), get(m, 0, 0)));
    return result;
  }

  Matrix adj;
  adj.rows = m.rows;
  adj.columns = m.columns;

  for (int i = 0; i < m.rows; i++)
  {
    for (int j = 0; j < m.columns; j++)
    {
      Number cofactor = determinant(matrix_minor(m, i, j));

      if ((i + j) % 2 == 1)
        cofactor = multiply(cofactor, num_from_real_imaginary(-1, 0));

      set(&adj, j, i, cofactor);
    }
  }

  return const_multiply(divide(num_from_real_imaginary(1, 0), d), adj);
}