#include <stdio.h>
int main() {
	int n;//Переменная, которую будем представлять в 2-ом виде
	int size_int =  8 * sizeof(int);//Переменная для хранения размера типа данных int в битах
	printf("Enter number \n");
	scanf("%d", &n);
	//Цикл, в котором за счёт побитового смещения вправо и побитового "И"
   //осуществляется вывод на экран "1" либо "0"
	for (int i = size_int - 1; i >= 0; i--) {
		int result = (n >> i) & 1;
		printf("%d", result);
	}
	return 0;
} 