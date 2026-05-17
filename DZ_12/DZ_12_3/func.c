#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>    
#include "int_face.h"

//Функция для ввода строки и удаления символа новой строки \n
//Принимает указатель на строку и размер строки
char* s_gets(char* string, int size){ 

  char* str = fgets(string, size, stdin); 
  if (str != NULL) {                      
    char* zam = strchr(str, '\n');        
    if (zam != NULL) {                    
      *zam ='\0';                         
    }
    else {  
       while (getchar() != '\n'); 
    }
    return str;
  }
  else {
    perror("Ошибка записи строки");
    exit(EXIT_FAILURE);
  }
}


//Функция для дробления введённой строки, на отдельные строки за счёт замены пробелов на \0
//Принимает ук-ль на строку, ук-ль на массивы ук-ей на строки
int make_arg(char* string, char** mas_1, char** mas_2, int qty){
    int count = 0;            //Переменная для индекса массива указателей на строку
    int j = 0;                //Переменная-индекс начала новой строки
    int size = strlen(string);//Кол-во символов в строке
    int flag = 1;             //Возвращаемое значение (кол-во потоков)

    for (int i = 0; i < size; i++){
      //Если встретился символ '|'
      //Начинаем сохранять строки во второй массив
      if (string[i] == '|'){
        flag = 2; //Обновляем значение флага
        count = 0;//Обнуляем значение счётчика-индекса для 2го массива строк
        j++;      //Увеличиваем индекс текущего символа в обрабатываемой строке
        continue;
      }
      //Для первого массива-строк
      if (flag == 1){
        if (string[i] == ' '){
          string[i] = '\0';
          if (strlen(&string[j]) > 0){
            mas_1[count] = &string[j];
            count++;
          }
          j = i + 1;
        }
        else if (i == size - 1){
          mas_1[count] = &string[j];
          count++;
        }
      }
      //Для второго массива-строк
      if (flag == 2){
        if (string[i] == ' '){
          string[i] = '\0';
          if (strlen(&string[j]) > 0){
            mas_2[count] = &string[j];
            count++;
          }
          j = i + 1;
        }
        else if (i == size - 1){
          mas_2[count] = &string[j];
          count++;
        }
      }
      //Если вышли за границу массива-строк
      //прерываем цикл, возвращаем -1
      if (count >= (qty - 1)){
        flag = -1;
        break;
      }
    }
    return flag;
}