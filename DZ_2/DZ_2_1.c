#include <stdio.h>
#define N 3//Константа, которой задан размер квадратной матрицы
int main() {
	int mat[N][N];
	int temp = 1;//Переменная, значение которой примет первый элемент массива
	for (int i = 0; i < N; i++) {
		//Цикл в котором построчно инициализируются и выводятся элементы матрицы
		for (int j = 0; j < N; j++) {
			mat[i][j] = temp;
			temp = mat[i][j] + 1;
			printf("%3d ", mat[i][j]);
		}
		printf("\n");
	}
	return 0;
}