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
#include <estimation.h>
#include <precision.h>

const int SIZE_N = 6;
const int SIZE_M = 4;
const char METHOD = 'R';
const int RANGE = 5;

void printHelp(char message[])
{
        printf("%s\n\n", message);
        printf(
                "Commands:\n"
                "---------\n\n"
                "qrhh: QR factorization with Householder reduction\n"
                "qrgs: QR factorization with Gram-Schmidt method\n"
                "lu: LU factorization\n"
                "plu: LU factorization with pivoting\n"
                "gj: Gauss Jordan with pivots\n"
                "bs: Back substitution\n"
                "ols: Ordinary least squares\n\n"
                "Options:\n"
                "--------\n\n"
                "-v: verbose\n\n"
                );
}

void qr(char method, int debug)
{
        Matrix A = allocMatrix(SIZE_N, SIZE_M);
        Matrix QR[] = {
                allocMatrix(SIZE_N, SIZE_N),
                allocMatrix(SIZE_N, SIZE_M),
        };
        Matrix _A = allocMatrix(SIZE_N, SIZE_M);

        setMatrixValues(RANGE, METHOD, A);

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

        simpleMultiplyMatrices(QR[0], QR[1], _A);
        printf("QR=\n");
        drawMatrix(_A);

        double stats[2];
        matrixComparison(A, _A, stats);
        printf("Mean Error = %.16lf\n", stats[0]);
        printf("Max Error = %.16lf\n", stats[1]);

        freeMatrix(A);
        freeMatrix(QR[0]);
        freeMatrix(QR[1]);
        freeMatrix(_A);

}

void lu(int pivot, int debug)
{
        MatrixStack stack = allocMatrixStack(SIZE_N,SIZE_N,6);
        Matrix A = popMatrixStack(stack);
        Matrix _A = popMatrixStack(stack);

        setMatrixValues(RANGE, METHOD, A);

        printf("A=\n");
        drawMatrix(A);

        Matrix PA;
        Matrix LU[3];
        LU[0] = popMatrixStack(stack);
        LU[1] = popMatrixStack(stack);

        if (pivot)
        {

                LU[2] = popMatrixStack(stack);
                PA = popMatrixStack(stack);

                PLUDecomposition(A, LU, debug);

                printf("P=\n");
                drawMatrix(LU[0]);

                multiplyMatrices(LU[0], 0, A, 0, PA, 0);

                printf("PA=\n");
                drawMatrix(PA);

        } else
        {
                LUDecomposition(A, LU, debug);

                PA = A;
        }
        pivot = !pivot;

        printf("L=\n");
        drawMatrix(LU[1-pivot]);

        printf("U=\n");
        drawMatrix(LU[2-pivot]);

        multiplyMatrices(LU[1-pivot], 0, LU[2-pivot], 0, _A, 0);

        printf("_A=\n");
        drawMatrix(_A);

        double stats[2];
        matrixComparison(PA, _A, stats);
        printf("Mean Error = %.16lf\n", stats[0]);
        printf("Max Error = %.16lf\n", stats[1]);

        freeMatrixStack(stack);

}

void gj(int debug)
{
        MatrixStack stack = allocMatrixStack(SIZE_N,SIZE_N,5);

        Matrix A = popMatrixStack(stack);
        Matrix B = popMatrixStack(stack);
        Matrix RREF[] = {
                popMatrixStack(stack),
                popMatrixStack(stack)
        };

        setMatrixValues(RANGE, METHOD, A);
        setMatrixValues(1, 'I', B);

        printf("A=\n");
        drawMatrix(A);

        printf("B=\n");
        drawMatrix(B);

        gaussJordanElimination(A, B, RREF, debug);

        printf("A^=\n");
        drawMatrix(RREF[0]);

        printf("B^=\n");
        drawMatrix(RREF[1]);

        Matrix C = popMatrixStack(stack);
        simpleMultiplyMatrices(A, RREF[1], C);

        printf("AA-1=\n");
        drawMatrix(C);

        double stats[2];
        identityPrecision(C, stats);
        printf("Mean Error=%.16lf\n", stats[0]);
        printf("Max Error=%.16lf\n", stats[1]);

        freeMatrixStackAll(stack);
}

void bs()
{
        Matrix A = allocMatrix(10,10);

        MatrixStack stack = allocMatrixStack(10,1,3);
        Matrix b = popMatrixStack(stack);
        Matrix solution = popMatrixStack(stack);
        Matrix _b = popMatrixStack(stack);

        double values[] = {
                1, 0, 5, 0, 9, 0, 3, 0, 9, 9,
                0, 3, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 1, 0, 0, 7, 0, 0, 9, 0,
                0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 1, 0, 4, 0, 0, 0,
                0, 0, 0, 0, 0, 8, 0, 0, 0, 2,
                0, 0, 0, 0, 0, 0, 1, 6, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 2, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 8
        };

        fillMatrix(values, A);
        double _values[] = {
                7.0, 8.0, 5.5, 9.7, 9.1, 0.8, 3.1, 0.2, 9.9, 9.0,
        };
        fillMatrix(_values, b);

        backSubstitution(A, solution, b);

        printf("A=\n");
        drawMatrix(A);
        printf("B=\n");
        drawMatrix(b);
        printf("solution=\n");
        drawMatrix(solution);
        simpleMultiplyMatrices(A, solution, _b);
        printf("A(solution)=\n");
        drawMatrix(_b);

        double stats[2];
        matrixComparison(_b, b, stats);
        printf("Mean Error=%.16lf\n", stats[0]);
        printf("Max Error=%.16lf\n", stats[1]);

        freeMatrix(A);
        freeMatrixStackAll(stack);
}

void ols()
{

        Matrix A = allocMatrix(SIZE_N, SIZE_M);
        Matrix x = allocMatrix(SIZE_M+1, 1);
        Matrix b = allocMatrix(SIZE_N, 1);

        setMatrixValues(RANGE, METHOD, A);
        setMatrixValues(RANGE, METHOD, b);

        printf("A=\n");
        drawMatrix(A);

        printf("b=\n");
        drawMatrix(b);

        linearRegression(A,x,b);

        printf("x=\n");
        drawMatrix(x);

        freeMatrix(A);
        freeMatrix(b);
        freeMatrix(x);
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
        else if (strcmp(argv[1], "lu") == 0)
        {
                lu(0, debug);
        }
        else if (strcmp(argv[1], "plu") == 0)
        {
                lu(1, debug);
        }
        else if (strcmp(argv[1], "gj") == 0)
        {
                gj(debug);
        }
        else if (strcmp(argv[1], "bs") == 0)
        {
                bs();
        }
        else if (strcmp(argv[1], "ols") == 0)
        {
                ols();
        }
        else
        {
                char message[100];
                strcat(message, "command not recognized: ");
                strcat(message, argv[1]);
                printHelp(message);
                return 1;
        }

        return 0;
}
