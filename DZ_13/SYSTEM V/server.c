#include<stdio.h>
#include<string.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <fcntl.h>    // Для open
#include <unistd.h>   // Для read, write, close, lseek
#include <sys/types.h> // Для типов данных
#include <sys/stat.h>  // Для режимов доступа (S_IRUSR и т.д.)
#include"int_face.h"

int main(){
    char file_name[] = "for_key.txt";//Имя файла для ключа
    char mesg[] = "Hi!";             //Строка для отправки           
    int fd_key_file;                 //Дескриптор файла для ключа
    key_t key;                       //Ключ
    int mq_id;                       //ID очерди сообщений
    struct mq mesg_serv;             //Структура
    size_t mesg_size;                //Размер получаемого сообщения
    size_t data_size = sizeof(mesg_serv) - sizeof(long);//Размер отправляемого сообщения

    //Заполнили поля структуры
    mesg_serv.mtype = 1;         
    strcpy(mesg_serv.message, mesg);
    
    //Создали файл, для ключа
    fd_key_file = open(file_name, O_WRONLY|O_CREAT,0644);
    if (fd_key_file < 0){
        perror("Не удалось создать файл");
        exit(EXIT_FAILURE);
    }

    //Закрыли дексриптор файла
    if (close(fd_key_file) < 0){
        perror("Ошибка закрытия дескриптора файла");
        exit(EXIT_FAILURE); 
    }

    //Создали ключ для
    key = ftok(file_name, 'A');
    if (key < 0){
        perror("Не удалось содать ключ");
        exit(EXIT_FAILURE);
    }

    //Создали очередь сообщений
    mq_id = msgget(key, 0666|IPC_CREAT);
    if (mq_id < 0){
        perror("Не удалось создать MQ");
        exit(EXIT_FAILURE);    
    }

    //Передали сообщение в очередь
    if (msgsnd(mq_id, &mesg_serv, data_size, 0) < 0){
        perror("Не удалось передать сообщение");
        exit(EXIT_FAILURE);
    }
    printf("Отправлено сообщение: %s\n", mesg);

    //Принимаем сообщение с типом 2
    mesg_size = msgrcv(mq_id,&mesg_serv,data_size,2,0);
    if (mesg_size != data_size){
        perror("Не удалось прочитать сообщение");
        exit(EXIT_FAILURE); 
    }
    printf("Сообщение из очереди: %s\n", mesg_serv.message);

    //Удаляем MQ
    if (msgctl(mq_id, IPC_RMID, NULL) < 0) {
        perror("Ошибка удаления MQ");
        exit(EXIT_FAILURE);
    }
    if (remove(file_name) < 0){
        perror("Ошибка удаления файла");
    }
    
    return 0;
}