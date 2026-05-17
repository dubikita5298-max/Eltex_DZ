#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>    
#include <sys/wait.h> 
#include <sys/types.h> 
#include "int_face.h"

#define SIZE_S 100     //Размер строки, вводимой пользователем 
#define STR_QTY 10     //Размер массива указателей на строки

int main() {

    pid_t pid_1, pid_2;   //Для возвращаемого pid в fork()
    pid_t finish;         //Для возвращаемого pid в wait()
    char vvod[SIZE_S];    //Строка, вводимая пользователем
    int pipe_fd[2];       //Массив дескрипторов для pipe
    int flag_arg;         //Переменная по которой будет определяться кол-во создаваемых процессов

    while(1){
        char* arg_1[STR_QTY] = {NULL}; //Массивы для строк-аргументов exec()
        char* arg_2[STR_QTY] = {NULL}; //зануляем в начале цикла
        flag_arg = 0;                  //Зануляем значение флага в начале цикла

        printf("Введите название программы и опции её запуска. Для выхода введите exit\n");
        s_gets(vvod, SIZE_S);          //Вызов функции для воода строки
        if (strcmp(vvod, "exit") == 0){//Елси введён exit -выход из программы
            break;
        }
        //Елси введена пустая строка, переходим на новую итерацию для повторной попытки ввода строки
        else if (strlen(vvod) == 0){
            continue;
        }
        //Вызов ф-ии создания аргументов
        //Ф-ия вернёт число потоков, которое будем создавать 1 или 2
        flag_arg = make_arg(vvod, arg_1, arg_2, STR_QTY);
        //Если был переполнен один из массивов строк
        //Прерываем цикл, повторяем ввод
        if (flag_arg < 0){
            printf("Ошибка ввода\n");
            continue;
        } 
        //Для двух процессов
        else if (flag_arg == 2){
            //Создаём неименованный канал
            if (pipe(pipe_fd) != 0){
                perror("Ошибка создания канала");
                exit(EXIT_FAILURE);
            }
        }
        //Созадём процесс(1)
        pid_1 = fork();
        if (pid_1 < 0) {
            perror("Ошибка создания процесса");
            exit(EXIT_FAILURE);
        }
        //Процесс(1)
        else if (pid_1 == 0){
            //printf("Создан процесс_1 %d\n", getpid());
            //Если процесса два
            if (flag_arg == 2){
                close(pipe_fd[0]);     //Закрываем канал на чтение
                //Подменяем поток вывода
                if (dup2(pipe_fd[1], 1) < 0){
                    perror("Ошибка подмены потока вывода");
                    close(pipe_fd[1]);
                    exit (EXIT_FAILURE);
                }
                close(pipe_fd[1]);  //Закрываем старый дескриптор
            }
            //Запускаем бинарник
            if (execvp(arg_1[0], arg_1) == -1){
                perror("Ошибка открытия бинарника");
            }
            exit(EXIT_FAILURE);
        }
        //Процесс - родитель
        else {
            //Если процесса два, создаём второй
            if (flag_arg == 2){
                pid_2 = fork();
                if (pid_2 < 0){
                    perror("Ошибка создания процесса");
                    exit(EXIT_FAILURE);
                }
            //Процесс(2)
                else if (pid_2 == 0){
                    //printf("Создан процесс_2 %d\n", getpid());
                    close(pipe_fd[1]);  //Закрываем канал на запись
                    //Подменяем поток ввода
                    if (dup2(pipe_fd[0], 0) < 0){
                        perror("Ошибка подмены потока ввода");
                        exit(EXIT_FAILURE);
                    }
                    close(pipe_fd[0]);  //Закрываем старый дескриптор
                    //Запускаем бинарник
                    if (execvp(arg_2[0], arg_2) == -1){
                        perror("Ошибка открытия бинарника");
                    }
                        exit(EXIT_FAILURE);
            }
        }
        //Для двух потоков
        if (flag_arg == 2){
            close(pipe_fd[0]);//Закрываем дескрипторы, открытие в родителе
            close(pipe_fd[1]); 
        }
        //Ожидание детей
        for (int i = 0; i < flag_arg; i++){
            finish = wait(NULL);
            if (finish < 0){
                perror("Ошибка wait");
                exit(EXIT_FAILURE);
            }
        }
    }
}
    return 0;
}