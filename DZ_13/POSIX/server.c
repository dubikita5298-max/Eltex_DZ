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
    char send_mesg[] = "Hi!";     //Строка для отправки  
    char get_mesg[SIZE];          //Строка для получения      
    mqd_t mq_snd;                 //Дескриптор MQ на отправку (MQ создаёт сервер)
    mqd_t mq_get;                 //Дескриптор MQ на чтение (MQ создаёт клиент)
    struct mq_attr atr_srv;       //Структура с аттрибутами QM сервера

    //Инициализация элементов структуры
    atr_srv.mq_flags = 0;
    atr_srv.mq_maxmsg = 10;   
    atr_srv.mq_msgsize = SIZE; 

    //Создаём MQ сервера
    mq_snd = mq_open(name_srv, O_WRONLY|O_CREAT, 0644, &atr_srv);
    if (mq_snd < 0){
        perror("Ошибка создания MQ");
        exit(EXIT_FAILURE);
    }

    //Отправляем сообщение
    if (mq_send(mq_snd, send_mesg, sizeof(send_mesg), 1) < 0){
        perror("Ошибка отправки сообщения");
        exit(EXIT_FAILURE);
    }
    printf("Отправлено сообщение: %s\n", send_mesg);

    //Подключаемся к MQ клиента
    //Ждём в цикле, чтобы запустить бинарник клиента и создать там MQ
    while ((mq_get = mq_open(name_clnt, O_RDONLY)) < 0){
        sleep(1);
    }

    //Ждём сообщения
    if (mq_receive(mq_get, get_mesg, SIZE, NULL) < 0){
        perror("Ошибка получения сообщения");
        exit(EXIT_FAILURE);
    }
    printf("Получено сообщение: %s\n", get_mesg);

    //Закрываем дескрипторы MQ
    if (mq_close(mq_get) < 0){
        perror("Ошибка закрытия дескриптора");
    }
    if (mq_close(mq_snd) < 0){
        perror("Ошибка закрытия дескриптора");
    }

    //Удаляем MQ сервера
    if (mq_unlink(name_srv) < 0){
        perror("Не удалось удалить MQ");
        exit(EXIT_FAILURE);
    }

    return 0;
}