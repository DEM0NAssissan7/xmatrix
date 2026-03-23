#include <stdio.h>
#include <string.h>
#include "matrix.h"
#include "mathlib.h"

Matrix prompt_matrix(char *name)
{
  Matrix m;

  printf("Enter number of rows for matrix %s: ", name);
  scanf("%d", &m.rows);

  printf("Enter number of columns for matrix %s: ", name);
  scanf("%d", &m.columns);

  getchar(); // consume leftover newline after scanf

  for (int i = 0; i < m.rows; i++)
  {
    for (int j = 0; j < m.columns; j++)
    {
      char input[128];

      printf("%s[%d][%d] = ", name, i, j);
      fgets(input, sizeof(input), stdin);

      // remove trailing newline
      input[strcspn(input, "\n")] = '\0';

      Number value = parse_string(input);
      set(&m, i, j, value);
    }
  }

  return m;
}
void print_matrix(Matrix m, char *name)
{
  printf("%s =\n", name);

  for (int i = 0; i < m.rows; i++)
  {
    for (int j = 0; j < m.columns; j++)
    {
      Number n = get(m, i, j);

      if (n.imaginary == 0)
      {
        printf("%8.2f ", n.real);
      }
      else if (n.real == 0)
      {
        printf("%8.2fi ", n.imaginary);
      }
      else
      {
        printf("%8.2f%+.2fi ", n.real, n.imaginary);
      }
    }
    printf("\n");
  }
}

int main()
{
  Matrix A = prompt_matrix("A");
  Matrix B = prompt_matrix("B");

  int choice;

  do
  {
    printf("\nChoose an operation:\n");
    printf("1. Print A and B\n");
    printf("2. Multiply A * B\n");
    printf("3. Determinant of A\n");
    printf("4. Determinant of B\n");
    printf("5. Re-enter A\n");
    printf("6. Re-enter B\n");
    printf("0. Quit\n");
    printf("Choice: ");

    scanf("%d", &choice);
    getchar();

    if (choice == 1)
    {
      print_matrix(A, "A");
      print_matrix(B, "B");
    }
    else if (choice == 2)
    {
      if (A.columns == B.rows)
      {
        Matrix C = matrix_multiply(A, B);
        print_matrix(C, "A * B");
      }
      else
      {
        printf("Cannot multiply A * B.\n");
      }
    }
    else if (choice == 3)
    {
      if (A.rows == A.columns)
      {
        Number d = determinant(A);
        printf("det(A) = %.2f%+.2fi\n", d.real, d.imaginary);
      }
      else
      {
        printf("A is not square.\n");
      }
    }
    else if (choice == 4)
    {
      if (B.rows == B.columns)
      {
        Number d = determinant(B);
        printf("det(B) = %.2f%+.2fi\n", d.real, d.imaginary);
      }
      else
      {
        printf("B is not square.\n");
      }
    }
    else if (choice == 5)
    {
      A = prompt_matrix("A");
    }
    else if (choice == 6)
    {
      B = prompt_matrix("B");
    }

  } while (choice != 0);

  return 0;
}