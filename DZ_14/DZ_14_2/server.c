#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>      
#include <unistd.h>     
#include <sys/types.h>
#include <sys/mman.h>   
#include <sys/stat.h>  
#include <pthread.h>  
#include <semaphore.h>
#include "inface.h"

int main(){
    char name_shr[] = "/shr_DZ_14_2";     //Имя сегмента
    struct for_segm* shr_mem;             //Ук-ль на начало разделяемого сегмента 

    char name_sem_acces_lstus[] = "/shr_sem_1";    
    char name_sem_acces_send[] = "/shr_sem_2";    
    char name_sgnl_srv_nwcl[] = "/shr_sem_3";     
    char name_sgnl_srv_nwmsg[] = "/shr_sem_4";     
    char name_sgnl_cl_cllst[] = "/shr_sem_5";      
    char name_sgnl_cl_mesglst[] = "/shr_sem_6";    

    pthread_t thrd_join_usr;             //Поток ожидания нового пользователя
    pthread_t thrd_give_mesg;            //Поток ожидания новых сообщений
    struct for_thrd str_thrd;

    //Создали сегмент РП
    shr_mem = make_share (name_shr, SIZE_SHR);
    //Иниц-ли поля стр-ры потока
    str_thrd.sem_acces_lst = make_sem(name_sem_acces_lstus, 1);    
    str_thrd.sem_acces_send = make_sem(name_sem_acces_send, 1);  

    str_thrd.sgnl_srv_nwcl = make_sem(name_sgnl_srv_nwcl, 0);      
    str_thrd.sgnl_srv_nwmesg = make_sem(name_sgnl_srv_nwmsg, 0);  

    str_thrd.sgnl_cl_cllst = make_sem(name_sgnl_cl_cllst, 0);      
    str_thrd.sgnl_cl_mesglst = make_sem(name_sgnl_cl_mesglst, 0);      
    str_thrd.segm_shr = shr_mem;
    str_thrd.flg_cls = 0;

    //Иниц-ли поля стр-ры РП
    shr_mem->qty_mesg = 0;
    shr_mem->qty_usr = 0;     
    shr_mem->flag_ign = 0;                                                                                                                                                                                                                                                          

    //Создание потока для ожидания новых пользователей
    if (pthread_create(&thrd_join_usr, NULL, join_usr, &str_thrd) != 0){
        printf("Ошибка создания потока\n");
        exit(EXIT_FAILURE);
    }

    //Создание потока для ожидания новых сообщений
    if (pthread_create(&thrd_give_mesg, NULL, give_mesg, &str_thrd) != 0){
        printf("Ошибка создания потока\n");
        exit(EXIT_FAILURE);
    }

    //Ожидание завершения потоков
    if (pthread_join(thrd_join_usr, NULL) != 0){
        printf("Ошибка завершения потока\n");
        exit(EXIT_FAILURE); 
    }
    if (pthread_join(thrd_give_mesg, NULL) != 0){
        printf("Ошибка завершения потока\n");
        exit(EXIT_FAILURE); 
    }

    //Закрываем семафоры и удаляем
    close_del_sem(name_sem_acces_lstus, str_thrd.sem_acces_lst); 
    close_del_sem(name_sem_acces_send, str_thrd.sem_acces_send); 
    close_del_sem(name_sgnl_srv_nwcl, str_thrd.sgnl_srv_nwcl);
    close_del_sem(name_sgnl_srv_nwmsg, str_thrd.sgnl_srv_nwmesg);
    close_del_sem(name_sgnl_cl_cllst, str_thrd.sgnl_cl_cllst);
    close_del_sem(name_sgnl_cl_mesglst, str_thrd.sgnl_cl_mesglst);

    //Отключаем сегмент от адресного пространства и удаляем
    close_del_shr(shr_mem, name_shr, SIZE_SHR);

    return 0;
}