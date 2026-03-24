#include <stdio.h>
#include <string.h>
#include "matrix.h"
#include "calculator.h"

#define PI 3.14159265358979323846 /* pi */

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
      Number n = get(&m, i, j);

      printf("%8.2f%+.2fi<%.2f@%.2f°>", n.real, n.imaginary, n.magnitude, n.angle * (180 / PI));
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
    printf("7. Compute A^-1 * B\n");
    printf("8. Compute A * B^-1\n");
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
        Matrix C;
        matrix_multiply(&A, &B, &C);
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
        Number d = determinant(&A);
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
        Number d = determinant(&B);
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
    else if (choice == 7)
    {
      Matrix A_inv;
      if (!inverse(&A, &A_inv))
      {
        printf("A is not invertible.\n");
      }
      else if (A_inv.columns != B.rows)
      {
        printf("Cannot multiply A^-1 * B: incompatible dimensions.\n");
      }
      else
      {
        Matrix C;
        matrix_multiply(&A_inv, &B, &C);
        print_matrix(C, "A^-1 * B");
      }
    }
    else if (choice == 8)
    {
      Matrix B_inv;
      if (!inverse(&B, &B_inv))
      {
        printf("B is not invertible.\n");
      }
      else if (A.columns != B_inv.rows)
      {
        printf("Cannot multiply A * B^-1: incompatible dimensions.\n");
      }
      else
      {
        Matrix C;
        matrix_multiply(&A, &B_inv, &C);
        print_matrix(C, "A * B^-1");
      }
    }

  } while (choice != 0);

  return 0;
}
