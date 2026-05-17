#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>    
#include <sys/wait.h>  
#include <sys/types.h> 

int main(){
    pid_t pid;                     //Переменная для идентификации ребёнка/родителя
    int pipe_fd[2];                //Массив, в который запишутся десрипторы канала
    char str_per[] = "Hi!";        //Строка, которую родитель передаст в канал
    char str_chld[sizeof(str_per)];//Строка, в которую ребёнок прочитает из канала
    
    //Создаём неименованный канал
    if (pipe(pipe_fd) != 0){
        perror("Ошибка создания канала");
        exit(EXIT_FAILURE);
    }
    //Создаём копию процесса
    pid = fork();
    //Завершение программы, при ошибке создания процесса
    if (pid < 0) {
        perror("Ошибка создания процесса");
        exit(EXIT_FAILURE);
    }
    //Процесс-родитель
    else if (pid > 0){
        //Закрываем дескриптор на чтение
        if (close(pipe_fd[0]) != 0){
            perror("Ошибка закрытия дескриптора на чтение");
            exit(EXIT_FAILURE);
        }
        //Записываем строку в канал
        if (write(pipe_fd[1], str_per, sizeof(str_per)) != sizeof(str_per)){
            perror("Ошибка записи в канал");
            exit(EXIT_FAILURE);
        }
        //Закрываем дескриптор на запись
        close(pipe_fd[1]);
        //Ожидаем ребёнка
        if(wait(NULL) < 0){
            perror("Ошибка завершения процесса");
        }
    }
    //Процесс-ребёнок
    else if (pid == 0){
        //Закрываем дескриптор на запись 
        if (close(pipe_fd[1]) != 0){
            perror("Ошибка закрытия дескриптора на запись");
            exit(EXIT_FAILURE);
        }
        //Читаем строку из канала
        if (read(pipe_fd[0], str_chld, sizeof(str_per)) != sizeof(str_per)){
            perror("Ошибка чтения из канала");
            exit(EXIT_FAILURE);
        }
        //Закрываем дескриптор на чтение
        close(pipe_fd[0]);
        printf("Строка, прочитанная из канала: %s\n", str_chld);
    }
    return 0;
}