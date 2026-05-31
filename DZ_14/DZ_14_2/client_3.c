#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>      
#include <unistd.h> 
#include <pthread.h>    
#include <sys/types.h>
#include <sys/mman.h>   
#include <sys/stat.h>  
#include <semaphore.h>
#include "inface.h"

int main(){
    char my_name[NAME_LRN] = "User_3";  //Имя клиента
    char name_shr[] = "/shr_DZ_14_2";   //Имя сегмента
    struct for_segm* shr_mem;           //Ук-ль на начало разделяемого сегмента 

    //Имена семафоров
    char name_sem_acces_lstus[] = "/shr_sem_1";    
    char name_sem_acces_send[] = "/shr_sem_2";    
    char name_sgnl_srv_nwcl[] = "/shr_sem_3";     
    char name_sgnl_srv_nwmsg[] = "/shr_sem_4";     
    char name_sgnl_cl_cllst[] = "/shr_sem_5";      
    char name_sgnl_cl_mesglst[] = "/shr_sem_6";       

    pthread_t thrd_name_list;           //Поток для обнов списка пользователей
    pthread_t thrd_mesg_list;           //Поток для обнов сообщений
    pthread_t thrd_snd_mesg;            //Поток для отправки сообщений
    struct for_thrd str_thrd;           //Стр-ра для потока форм списка

    //Подключились к РП
    shr_mem = connect_share (name_shr, SIZE_SHR);

    //Если чат заполнен пользователями, закрываем клиента
    if (shr_mem->qty_usr == USRS_QTY){
        close_shr(shr_mem, SIZE_SHR);
        return 0;
    }
    //Иниц-ли поля стр-ры семафорами и разд памятью
    str_thrd.sem_acces_lst = connect_sem(name_sem_acces_lstus); 
    str_thrd.sem_acces_send = connect_sem(name_sem_acces_send); 
    str_thrd.sgnl_srv_nwcl = connect_sem(name_sgnl_srv_nwcl); 
    str_thrd.sgnl_srv_nwmesg = connect_sem(name_sgnl_srv_nwmsg); 
    str_thrd.sgnl_cl_cllst = connect_sem(name_sgnl_cl_cllst);
    str_thrd.sgnl_cl_mesglst = connect_sem(name_sgnl_cl_mesglst);
    str_thrd.segm_shr = shr_mem;
    strcpy(str_thrd.name, my_name);
    str_thrd.flg_cls = 0;

    //Запускаем интерфейс
    make_interface();

    //Сообщаем серверу о подключении
    sem_wait(str_thrd.sem_acces_lst);                                   //Пытаемся захватить семафор
    strcpy(shr_mem->list_usr[shr_mem->qty_usr],my_name);                //Передали имя
    //shr_mem->qty_usr++;                                               //Увеличили кол-во пользователей
    sem_post(str_thrd.sem_acces_lst);                                   //Освободили семафор
    sem_post(str_thrd.sgnl_srv_nwcl);                                   //Пробудили сервер

    //Создание потока для обновления списка пользователей
    if (pthread_create(&thrd_name_list, NULL, make_us_lst, &str_thrd) != 0){
        printf("Ошибка создания потока-спика пользователей\n");
        exit(EXIT_FAILURE);
    }

    //Создание потока для обновления сообщений
    if (pthread_create(&thrd_mesg_list, NULL, make_mesg_lst, &str_thrd) != 0){
        printf("Ошибка создания потока-спика пользователей\n");
        exit(EXIT_FAILURE);
    }

    sleep(1);
    
    //Создание потока для отправки сообщений
    if (pthread_create(&thrd_snd_mesg, NULL, make_mesg, &str_thrd) != 0){
        printf("Ошибка создания потока-списка пользователей\n");
        exit(EXIT_FAILURE);
    }

    //Ожидание завершения потоков
    if (pthread_join(thrd_name_list, NULL) != 0){
        printf("Ошибка завершения потока\n");
        exit(EXIT_FAILURE); 
    }

    if (pthread_join(thrd_mesg_list, NULL) != 0){
        printf("Ошибка завершения потока\n");
        exit(EXIT_FAILURE); 
    }

    if (pthread_join(thrd_snd_mesg, NULL) != 0){
        printf("Ошибка завершения потока\n");
        exit(EXIT_FAILURE); 
    }

    close_interface();
    //Отсоединяем РП
    close_shr(shr_mem, SIZE_SHR);
    return 0;
}