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
    key_t key;                       //Ключ
    int mq_id;                       //ID очерди сообщений
    char str_clnt[] = "Hello";       //Строка для отправки   
    struct mq mesg_client;           //Структура
    size_t mesg_size;                //Размер получаемого сообщени
    size_t data_size = sizeof(mesg_client) - sizeof(long);//Размер получаемого сообщения
    
    mesg_client.mtype = 0;

    //Получаем доступ к ключу
    key = ftok(file_name, 'A');
    if (key < 0){
        perror("Ошибка получения ключа");
        exit(EXIT_FAILURE);
    }

    //Получаем доступ к MQ
    mq_id = msgget(key, 0);
    if (mq_id < 0){
        perror("Не удалось создать MQ");
        exit(EXIT_FAILURE);    
    }

    //Читаем сообщение из очереди c типом 1
    mesg_size = msgrcv(mq_id,&mesg_client,data_size,1,0);
    if (mesg_size != data_size){
        perror("Не удалось прочитать сообщение");
        exit(EXIT_FAILURE); 
    }
    printf("Сообщение из очереди: %s\n", mesg_client.message);

    //Заменяем поля структуры для отправки сообщения
    strcpy(mesg_client.message, str_clnt);
    mesg_client.mtype = 2;

    //Отправляем сообщение
    if (msgsnd(mq_id, &mesg_client, data_size, 0) < 0){
        perror("Не удалось передать сообщение");
        exit(EXIT_FAILURE);
    }
    printf("Отправлено сообщение %s\n", str_clnt);

    return 0;
}