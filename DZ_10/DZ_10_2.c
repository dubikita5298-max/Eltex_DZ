#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>    
#include <sys/wait.h>  
#include <sys/types.h> 

int main() {
    int stat;                               //Переменная-статус завершения процессов
    pid_t pid_1, pid_2, pid_3, pid_4, pid_5;//Переменные для fork()
    pid_t finish;                           //Переменная для wait()

    printf("Я процесс 0. PID = %d, PPID = %d\n", getpid(), getppid());

    //Создаём процесс 1
    pid_1 = fork();
    //Не удалось создать процесс 1
    if (pid_1 < 0 ){      
        perror("Ошибка создания процесса 1");
        exit(EXIT_FAILURE);
    }

    //Создан процесс 1, в нём порождаем процессы 3,4
    else if (pid_1 == 0){ 

        printf("Я процесс 1. PID = %d, PPID = %d\n", getpid(), getppid());
        //Создаём Процесс 3
        pid_3 = fork();

        if (pid_3 < 0){
            perror("Ошибка создания процесса 3");
            exit(EXIT_FAILURE);  
        }
        //Вывод инф-ии о процесс 3 и выход из него
        else if (pid_3 == 0) {
            printf("Я процесс 3. PID = %d, PPID = %d\n", getpid(), getppid());
            exit(0);
        }

        else {
            //Создаём Процесс 4
            pid_4 = fork();

            if (pid_4 < 0){
                perror("Ошибка создания процесса 4");
                exit(EXIT_FAILURE);
            }
            //Вывод инф-ии о процесс 4 и выход из него
            else if (pid_4 == 0){
                printf("Я процесс 4. PID = %d, PPID = %d\n", getpid(), getppid());
                exit(0);
            }
        }
        //Цикл для ожидания процессов 3,4, процессом 1
        for (int i = 0; i < 2; i++){
            finish = wait(&stat);
            if (finish < 0){
                perror("Ошибка wait");
                exit(EXIT_FAILURE);  
            }
            else {
                printf("Процесс c PID = %d завершился со статусом %d\n", finish, WEXITSTATUS(stat));
            }
        }
        exit(0);//Выход из процесса 1
    }

    else {
        //Создаём процесс 2
        pid_2 = fork();

        if (pid_2 < 0){
        perror("Ошибка создания процесса 2");
        exit(EXIT_FAILURE);
    }

    else if (pid_2 == 0){
        printf("Я процесс 2. PID = %d, PPID = %d\n", getpid(), getppid());
        
        //Создаём процесс 5
        pid_5 = fork();

        if (pid_5 < 0){
            perror("Ошибка создания процесса 5");
            exit(EXIT_FAILURE);
        }
        //Вывод инф-ии о процесс 5 и выход из него
        else if (pid_5 == 0) {
            printf("Я процесс 5. PID = %d, PPID = %d\n", getpid(), getppid());
            exit(0);
        }

        //Ожидание процесса 5
        finish = wait(&stat);
        printf ("Процесс c PID = %d завершился со статусом %d\n", finish, WEXITSTATUS(stat));
        exit(0); //Выход из процесса 2
    }

    //Цикл для ожидания процессов 1,2, процессом 0
    for (int i = 0; i < 2; i++){
        finish = wait(&stat);

        if (finish < 0){
            perror("Ошибка wait");
            exit(EXIT_FAILURE);
        }
        else {
            printf("Процесс c PID = %d завершился со статусом %d\n", finish, WEXITSTATUS(stat));
        }
      }
    }
    return 0;
}