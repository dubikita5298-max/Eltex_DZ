#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>    // Для fork()
#include <sys/wait.h>  // Для wait()
#include <sys/types.h> // Для pid_t

int main() {
    int status;        //Переменная - статус завершения дочернего процесса

    pid_t pid = fork();//Создаём копию нашего процесса

    //Не удалось создать процесс, завершение программы
    if (pid < 0){      
        perror("Ошибка создания процесса");
        exit(EXIT_FAILURE);
    }

    //Дочерний процесс
    else if (pid == 0){
        //Выводим инф-ию о дочернем процессе
        printf("Я дочерний процесс\nPID = %d\nPPID = %d\n", getpid(), getppid());
        exit(0);//Завершение дочернего процесса
    }


    //Родительский процесс
    else if (pid > 0){
        //Выводим инф-ию о родительском процессе 
        printf("Я родительский процесс\nPID = %d\nPPID = %d\n", getpid(), getppid());
        pid_t finish = wait(&status); //Ожидаем завершения дочернего процесса

        if (finish < 0){
            perror("Ошибка wait");
            exit(EXIT_FAILURE);
        }

        else {
            printf("Дочерний процесс c PID %d завершился со статусом %d\n", finish, WEXITSTATUS(status));//Вывод статуса завершения доч процесса
        }
    }

    return 0;
}