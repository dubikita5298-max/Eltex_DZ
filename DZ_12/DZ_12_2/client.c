#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(){
    int fd_pipe;                   //Дескриптор канала
    char pipe_name[] = "pipe_test";//Имя файла канала
    char str_client[124] = {0};    //Строка, которую сохраним текст из канала
     //Открываем канал ч/з дескриптор
    fd_pipe = open(pipe_name, O_RDONLY);
    if (fd_pipe < 0){
        perror("Ошибка открытия канала");
        exit(EXIT_FAILURE);
    }
    //Читаем из канала ч/з дескриптор
    read(fd_pipe, str_client,(sizeof(str_client) - 1));

    printf("Строка из канала: %s\n", str_client);
    //Закрываем канал
    close(fd_pipe);
    //Удаляем канал (файл)
    if (unlink(pipe_name) != 0){
        perror("Не удалось удалить файл");
    }
    else {
        printf("Канал удалён\n");
    }
    
    return 0;
}