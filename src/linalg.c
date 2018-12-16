/*
  @file qr.c
  @author Gerardo Veltri
  QR Decomposition
*/
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <mem.h>
#include <matrix.h>
#include <factorization.h>

void printHelp(char message[])
{
  printf("%s\n\n", message);
  printf(
         "Commands:\n"
         "---------\n\n"
         "qrhh: QR factorization with Householder reduction\n"
         "qrgs: QR factorization with Gram-Schmidt method\n"
         "ge: Gaussian Elimination with Pivots\n\n"
         );
}

void qr(char method, int debug)
{
  MatrixStack stack = allocMatrixStack(5,5,4);

  Matrix A = popMatrixStack(stack);
  Matrix QR[] = {
    popMatrixStack(stack),
    popMatrixStack(stack)
  };
  Matrix _A = popMatrixStack(stack);

  setMatrixValues(2, 'R', A);

  printf("A=\n");
  drawMatrix(A);

  switch (method)
   {
   case 'h':
     hhReflectionsQR(A, QR, debug);
     break;
   case 'g':
     gramSchmidtQR(A, QR, debug);
     break;
   }

  printf("Q=\n");
  drawMatrix(QR[0]);
  printf("R=\n");
  drawMatrix(QR[1]);

  multiplyMatrices(QR[0], QR[1], _A);
  printf("QR=\n");
  drawMatrix(_A);

  subtractMatrix(A, _A);
  absMatrix(A);
  double mean = meanMatrix(A);
  printf("Q - QR=\n");
  drawMatrix(A);

  printf("Mean Error=\n");
  printf("%.6f\n", mean);

  pushMatrixStack(stack, QR[0]);
  pushMatrixStack(stack, QR[1]);
  pushMatrixStack(stack, A);
  pushMatrixStack(stack, _A);
  freeMatrixStack(stack);

}

void ge(int debug)
{
  MatrixStack stack = allocMatrixStack(5,5,4);

  Matrix A = popMatrixStack(stack);
  Matrix B = popMatrixStack(stack);
  Matrix RREF[] = {
    popMatrixStack(stack),
    popMatrixStack(stack)
  };

  setMatrixValues(2, 'R', A);
  setMatrixValues(1, 'I', B);

  printf("A=\n");
  drawMatrix(A);

  printf("B=\n");
  drawMatrix(B);

  copyMatrix(A, RREF[0]);
  printf("copied A to RREF[0]");

  gaussianElimination(A, B, RREF, 0);

  printf("A^=\n");
  drawMatrix(RREF[0]);

  printf("B^=\n");
  drawMatrix(RREF[1]);

  pushMatrixStack(stack, RREF[0]);
  pushMatrixStack(stack, RREF[1]);
  pushMatrixStack(stack, A);
  pushMatrixStack(stack, B);
  freeMatrixStack(stack);

}


int main(int argc, char *argv[])
{

  if (argc < 2)
  {
    printHelp("linalg must be called with a command");
    return 1;
  }

  int debug;
  if (argc < 3)
    debug = 0;
  else if (strcmp(argv[2],"-v") == 0)
    debug = 1;

  if (strcmp(argv[1], "qrhh") == 0)
  {
    qr('h', debug);
  }
  else if (strcmp(argv[1], "qrgs") == 0)
  {
    qr('g', debug);
  }
  else if (strcmp(argv[1], "ge") == 0)
  {
    ge(debug);
  }
  else
  {
    char message[(24*sizeof(char)) + sizeof(argv[1])];
    strcat(message, "command not recognized: ");
    strcat(message, argv[1]);
    printHelp(message);
    return 1;
  }

  return 0;
}