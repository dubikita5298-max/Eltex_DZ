#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <fcntl.h>     
#include <unistd.h>    
#include <sys/types.h> 
#include <sys/stat.h>  
#include <errno.h>
#include "inface.h"
#define SRV_TYPE 666 //Тип сообщений принимаемый сервером
#define USRS_QTY 10  //Макс. кол-во пользователей чата
#define NAME_LRN 10  //Макс. разм. имени пользователя
#define MESG_LRN 24  //Макс. разм. сообщения
#define MESG_QTY 64  //Макс. кол-во сообщений

int main(){
    //Для создания MQ под передачу списка пользователей
    char file_namelst[] = "key_namelst.txt";//Имя файла для ключа          
    key_t key_namelst;                      //Ключ
    int mqid_namelst;                       //ID очерди сообщений

    //Для создания MQ под передачу истории сообщений
    char file_mesghstr[] = "key_mesg_hstr.txt";//Имя файла для ключа          
    key_t key_mesghstr;                        //Ключ
    int mqid_mesghstr;                         //ID очерди сообщений

    //Для приёма новых пользователей
    struct usr_mesg usr_name_str[USRS_QTY];                         //Стр-ра для приёма
    ssize_t size_name_give = sizeof(usr_name_str[0]) - sizeof(long);//Размер получаемого сообщения
    ssize_t size_name_buf = sizeof(usr_name_str[0]) - sizeof(long); //Полезный размер буфера, в который принимаем сообщение
    
    //Для передачи/хранения списка пользователей
    struct usr_list name_list_str;                                  //Стр-ра для хранения/передачи списка        
    ssize_t size_list = sizeof(name_list_str) - sizeof(long);       //Размер отправляемого сообщения  
    
    //Для приёма сообщений от пользователей 
    struct usr_mesg usr_mesg_str;                                    //Стр-ра для приёма
    ssize_t size_givemesg = sizeof(usr_mesg_str) - sizeof(long);     //Размер получаемого сообщения
    ssize_t size_givemesg_buf = sizeof(usr_mesg_str) - sizeof(long); //Полезный размер буфера, в который принимаем сообщение

    //Для хранения/передачи истории сообщений
    struct mesg_hstr hstr_mesg;                                      //Стр-ра для приёма
    ssize_t size_hstr_mesg = sizeof(hstr_mesg) - sizeof(long);       //Размер получаемого сообщения

    int cnt = 1;  //Для перебора типов 
    int close = 0;//Индикатор для выхода из цикла

    //Создание MQ для обновления списка пользователей
    mqid_namelst = make_mq(file_namelst, &key_namelst);
    //Инициализировали поля структуры списка пользователей
    name_list_str.usr_cnt = 0;        //Кол-во подкл-ых пользователей
    name_list_str.flag = 0;           //ВРЕМЕННАЯ СТРОКА

    //Создание MQ для хранения истории сообщений
    mqid_mesghstr = make_mq(file_mesghstr, &key_mesghstr);
    //Инициализировали поля структуры списка пользователей
    hstr_mesg.mesg_cnt = 0;         //Кол-во сообщений
    hstr_mesg.flag = 0;             //ВРЕМЕННАЯ СТРОКА
    
    printf("Для того, чтобы завершить клиентскую программу напишите в поле ввода EXIT \n");
    while (1){
        //Условие выхода из цикла
        if (close > 0 && name_list_str.usr_cnt == 0 ){
            printf("Все пользователи вышли из чата\n");
            break;
        }
        if (name_list_str.usr_cnt < USRS_QTY){
            //Ждём новых пользователей в 1-ой MQ
            size_name_give = msgrcv(mqid_namelst, &usr_name_str[name_list_str.usr_cnt], size_name_buf, SRV_TYPE, IPC_NOWAIT);
            if (size_name_give == size_name_buf){
                strcpy(name_list_str.names[name_list_str.usr_cnt], usr_name_str[name_list_str.usr_cnt].name);
                printf("Сохранил пользователя %s\n",name_list_str.names[name_list_str.usr_cnt]);
                close++;
                //Передаём список сообщений нововму пользователю
                hstr_mesg.mtype = usr_name_str[name_list_str.usr_cnt].mytype;
                if (msgsnd(mqid_mesghstr, &hstr_mesg, size_hstr_mesg, 0) < 0){
                    perror("Не удалось передать сообщение");
                    exit(EXIT_FAILURE);
                }
                name_list_str.usr_cnt++;
                //Передаём всем пользователям новый список
                cnt = 1;//Для поиска совпадений по типу пользователей
                //Внешний цикл перебирает тип пользователя
                while(cnt <= USRS_QTY){
                    //Внутренний цикл перебирает типы сохраннённых пользователей
                    for (int j = 0; j < name_list_str.usr_cnt; j++){
                        if (cnt == usr_name_str[j].mytype){
                            name_list_str.mtype = cnt;
                            if (msgsnd(mqid_namelst, &name_list_str, size_list, 0) < 0){
                                perror("Не удалось передать сообщение");
                                exit(EXIT_FAILURE);
                            }
                        }
                    }
                    cnt++;
                }
            }

            else if (size_name_give < 0 && errno != ENOMSG){
                perror("Не удалось прочитать сообщение");
                exit(EXIT_FAILURE); 
            }
        }
        else {
            printf("Добавлено максимальное кол-во пользователей\n");
        }

        //Ждём сообщений от пользователей во 2-ой MQ
        if(hstr_mesg.mesg_cnt < MESG_QTY){
            size_givemesg = msgrcv(mqid_mesghstr, &usr_mesg_str, size_givemesg_buf, SRV_TYPE, IPC_NOWAIT);
            if(size_givemesg == size_givemesg_buf){
                //Елси пользователь вышел из чата
                if(strcmp(usr_mesg_str.mesg, "EXIT") == 0){
                    name_list_str.mtype = usr_mesg_str.mytype;
                    name_list_str.flag = 1;
                    //Завершаем поток клиента по формированию списка пользователей
                    if (msgsnd(mqid_namelst, &name_list_str, size_list, 0) < 0){
                        perror("Не удалось передать сообщение");
                        exit(EXIT_FAILURE);
                    }
                    name_list_str.flag = 0;
                    hstr_mesg.mtype = usr_mesg_str.mytype;
                    hstr_mesg.flag = 1; 
                    //Завершаем поток клиента по формированию истории сообщений
                    if (msgsnd(mqid_mesghstr, &hstr_mesg, size_hstr_mesg, 0) < 0){
                        perror("Не удалось передать сообщение");
                        exit(EXIT_FAILURE);
                    }
                    hstr_mesg.flag = 0; 
                    printf("%s покинул чат\n", usr_mesg_str.name);
                    //Удаляем пользователя из списка
                    int indx = -1;
                    for (int j = 0; j < name_list_str.usr_cnt; j++){
                        if (strcmp(name_list_str.names[j], usr_mesg_str.name) == 0){
                            indx = j;
                            break;
                        }
                    }
                    if (indx != - 1){
                        for (int i = indx; i < name_list_str.usr_cnt - 1; i++){
                            strcpy(name_list_str.names[i], name_list_str.names[i + 1]);
                        }
                    }

                    for (int j = 0; j < name_list_str.usr_cnt; j++){
                        if (strcmp(usr_name_str[j].name, usr_mesg_str.name) == 0){
                            indx = j;
                            break;
                        }
                    }
                    if (indx != - 1){
                        for (int i = indx; i < name_list_str.usr_cnt - 1; i++){
                            strcpy(usr_name_str[i].name, usr_name_str[i + 1].name);
                            usr_name_str[i].mytype = usr_name_str[i + 1].mytype;
                        }
                        name_list_str.usr_cnt--;
                    }
                    //Отправляем обновлённый список пользователям
                    cnt = 1;//Для поиска совпадений по типу пользователей
                    //Внешний цикл перебирает тип пользователя
                    while(cnt <= USRS_QTY){
                        //Внутренний цикл перебирает типы сохраннённых пользователей
                        for (int j = 0; j < name_list_str.usr_cnt; j++){
                            if (cnt == usr_name_str[j].mytype){
                                name_list_str.mtype = cnt;
                                if (msgsnd(mqid_namelst, &name_list_str, size_list, 0) < 0){
                                    perror("Не удалось передать сообщение");
                                    exit(EXIT_FAILURE);
                                }
                            }
                        }
                        cnt++;
                    }
                }
                else {
                    //Принимаем сообщение от пользователя
                    strcpy(hstr_mesg.names[hstr_mesg.mesg_cnt], usr_mesg_str.name);
                    strcpy(hstr_mesg.mesg[hstr_mesg.mesg_cnt], usr_mesg_str.mesg);
                    printf("От %s получил сообщение: %s\n", hstr_mesg.names[ hstr_mesg.mesg_cnt], hstr_mesg.mesg[ hstr_mesg.mesg_cnt]);
                    hstr_mesg.mesg_cnt++;
                    //Передаём всем пользователям обновлённую историю сообщений
                    cnt = 1;//Для поиска совпадений по типу пользователей
                    while(cnt <= USRS_QTY){
                        //Внутренний цикл перебирает типы сохраннённых пользователей
                        for (int j = 0; j < name_list_str.usr_cnt; j++){
                            if (cnt == usr_name_str[j].mytype){
                                hstr_mesg.mtype = cnt;
                                if (msgsnd(mqid_mesghstr, &hstr_mesg, size_hstr_mesg, 0) < 0){
                                    perror("Не удалось передать сообщение");
                                    exit(EXIT_FAILURE);
                                }
                            }
                        }
                        cnt++;
                    }
                }

            }
            else if (size_givemesg < 0 && errno != ENOMSG){
                perror("Не удалось прочитать сообщение");
                exit(EXIT_FAILURE); 
            }
        }
        else {
            printf("История сообщений заполнена\n");
        }
    }

    //Удаляем MQ
    del_mq(mqid_namelst, file_namelst);
    del_mq(mqid_mesghstr, file_mesghstr);
    printf("Закончил работу\n");
    
    return 0;
}