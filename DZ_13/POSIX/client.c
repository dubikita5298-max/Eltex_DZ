#include<stdio.h>
#include<string.h>
#include <stdlib.h>
#include <mqueue.h>
#include <fcntl.h>    
#include <sys/stat.h> 
#include <unistd.h>
#define SIZE 20

int main(){
    char name_srv[] = "/srv_mq";  //Имя для MQ
    char name_clnt[] = "/clnt_mq";//Имя для MQ
    char send_mesg[] = "Hello!";  //Строка для отправки 
    char get_mesg[SIZE];          //Строка для получения         
    mqd_t mq_snd;                 //Дескриптор MQ (MQ создаёт клиент)
    mqd_t mq_get;                 //Дескриптор MQ на чтение (MQ создаёт сервер)
    struct mq_attr atr_clnt;       //Структура с аттрибутами QM для отправки сообщения

    //Инициализация элементов структуры
    atr_clnt.mq_flags = 0;
    atr_clnt.mq_maxmsg = 10;   
    atr_clnt.mq_msgsize = SIZE; 

    //Создаём MQ клиента
    mq_snd = mq_open(name_clnt, O_WRONLY|O_CREAT, 0644, &atr_clnt);
    if (mq_snd < 0){
        perror("Ошибка создания MQ");
        exit(EXIT_FAILURE);
    }

    //Подключаемся к MQ сервера 
    mq_get = mq_open(name_srv, O_RDONLY);
    if (mq_get < 0){
        perror("Ошибка подключения к MQ сервера");
        exit(EXIT_FAILURE);
    }

    //Ждём сообщения
    if (mq_receive(mq_get, get_mesg, SIZE, NULL) < 0){
        perror("Ошибка получения сообщения");
        exit(EXIT_FAILURE);
    }
    printf("Получено сообщение: %s\n", get_mesg);

    //Отправляем сообщение
    if (mq_send(mq_snd, send_mesg, sizeof(send_mesg), 1) < 0){
        perror("Ошибка отправки сообщения");
        exit(EXIT_FAILURE);
    }
    printf("Отправлено сообщение: %s\n", send_mesg);
    sleep(1);
    
    //Закрываем дескрипторы
    if (mq_close(mq_get) < 0){
        perror("Ошибка закрытия дескриптора");
    }
    if (mq_close(mq_snd) < 0){
        perror("Ошибка закрытия дескриптора");
    }

    //Удаляем MQ сервера
    if (mq_unlink(name_clnt) < 0){
        perror("Не удалось удалить MQ");
        exit(EXIT_FAILURE);
    }

    return 0;
}