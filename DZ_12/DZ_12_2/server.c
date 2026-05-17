#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(){
    int fd_pipe;                   //Дескриптор канала
    char pipe_name[] = "pipe_test";//Имя файла канала
    char str_srvr[] = "Hi!";       //Строка, которую передадим в канал
    //Создаём канал
    mkfifo(pipe_name,0666);
    //Открываем канал ч/з дескриптор
    printf("Ждём клиента и переадём в канал строку: %s\n", str_srvr);
    fd_pipe = open("pipe_test", O_WRONLY);
    if (fd_pipe < 0){
        perror("Ошибка открытия канала");
        exit(EXIT_FAILURE);
    }
    //Пишем в канал строку
    if (write(fd_pipe,str_srvr,sizeof(str_srvr)) < 0){
        perror("Ошибка записи в канал");
        exit(EXIT_FAILURE);
    }
    //Закрываем канал
    close(fd_pipe);
    return 0;
}