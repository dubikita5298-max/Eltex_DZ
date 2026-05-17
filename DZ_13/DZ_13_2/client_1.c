#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <fcntl.h>     
#include <sys/types.h> 
#include <sys/stat.h>  
#include <unistd.h>
#include "inface.h"
#define SRV_TYPE 666 //Тип сообщений принимаемый сервером
#define USRS_QTY 10  //Макс. кол-во пользователей чата
#define NAME_LRN 10  //Макс. разм. имени пользователя
#define MESG_LRN 24  //Макс. разм. сообщения
#define MESG_QTY 64  //Макс. кол-во сообщений

int main(){
    //Для подключения к MQ
    char file_namelst[] = "key_namelst.txt";     //Имя файла для ключа
    key_t key_namelst;                           //Ключ
    int mqid_namelst;                            //ID очерди сообщений

    //Для создания MQ под передачу истории сообщений
    char file_mesghstr[] = "key_mesg_hstr.txt";   //Имя файла для ключа          
    key_t key_mesghstr;                           //Ключ
    int mqid_mesghstr;                            //ID очерди сообщений

    //Для передачи инф-ии о себе
    struct usr_mesg usr_mesg_str = {SRV_TYPE, 1, "User_1", "-"};  //Стр-ра с инф-ией о себе
    ssize_t size_usr_mesg = sizeof(usr_mesg_str) - sizeof(long);//Размер отправляемого сообщения

    //Для получения/вывода списка пользователей
    pthread_t thrd_name_list;              //Поток для вывода списка пользователей
    struct usr_list name_list_str;         //Стр-ра для вывода списка пользователей 
    struct thrd_list thrd_name_list_str;   //Стр-ра для функции потока (вывод списка пользователей)  

    //Для получения истории сообщений
    struct mesg_hstr hstr_mesg;  

    //Для отправки сообщений
    pthread_t thrd_snd_mesg;               //Поток для отправки сообщений
    struct thrd_msg_snd thrd_send_str;     //Стрк-ра для функции потока (отправка сообщений)  

    //Получаем доступ к MQ для формирования списка пользователей
    mqid_namelst = connect_mq(file_namelst, &key_namelst);
    //Отправили информацию о себе
    if (msgsnd(mqid_namelst, &usr_mesg_str, size_usr_mesg, 0) < 0){
        perror("Не удалось передать сообщение");
        exit(EXIT_FAILURE);
    }
    //Иниц-ем поля стр-ры для передачи в поток-формирования списка пользователей
    thrd_name_list_str.mqid_list = mqid_namelst;
    thrd_name_list_str.mtype = usr_mesg_str.mytype;
    thrd_name_list_str.size_list = sizeof(name_list_str) - sizeof(long);
    thrd_name_list_str.name_list = &name_list_str;

    //Запускаем интерфейс
    make_interface();

    //Создание потока для формирования списка пользователей
    if (pthread_create(&thrd_name_list, NULL, make_list, &thrd_name_list_str) != 0){
        printf("Ошибка создания потока-спика пользователей\n");
        exit(EXIT_FAILURE);
    }

    //Получаем доступ к MQ для отправки сообщений
    mqid_mesghstr = connect_mq(file_mesghstr, &key_mesghstr);
    thrd_send_str.mqid = mqid_mesghstr;
    thrd_send_str.size_data = size_usr_mesg;
    thrd_send_str.usr_mesg_str = &usr_mesg_str;
    //Иниц-ем поля стр-ры для передачи в поток-формирования списка пользователей
    thrd_name_list_str.mqid_hstr = mqid_mesghstr;
    thrd_name_list_str.hstr_mesg = &hstr_mesg;
    thrd_name_list_str.size_hstr = sizeof(hstr_mesg) - sizeof(long);
    
    //Создание потока для отправки сообщения
    if (pthread_create(&thrd_snd_mesg, NULL, send_mesg, &thrd_send_str) != 0){
        printf("Ошибка создания потока-спика пользователей\n");
        exit(EXIT_FAILURE);
    }

    //Ожидание завершения потока, для формирования списка пользователей
    if (pthread_join(thrd_name_list, NULL) != 0){
        printf("Ошибка завершения потока\n");
        exit(EXIT_FAILURE); 
    }
    //Ожидание завершения потока, для формирования списка пользователей
    if (pthread_join(thrd_snd_mesg, NULL) != 0){
        printf("Ошибка завершения потока\n");
        exit(EXIT_FAILURE); 
    }

    close_interface();

    return 0;
}