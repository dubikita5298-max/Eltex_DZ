#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>      
#include <unistd.h>     
#include <sys/types.h>
#include <sys/mman.h>   
#include <sys/stat.h>
#include <semaphore.h>
#include "inface.h"  

//Ф-ия создания сегмента РП
struct for_segm* make_share (char*name_shr, int size){
    
    struct for_segm* ret_str;
    int shr_id;
    //Создали сегмент РП
    if ((shr_id = shm_open(name_shr, O_RDWR|O_CREAT, 0666)) < 0){
        perror("Ошибка создания сегмента");
        exit(EXIT_FAILURE);
    }
    //Установили размер РП
    if (ftruncate(shr_id, size) < 0){
        perror("Ошибка утсановки размера сегмента");
        exit(EXIT_FAILURE);
    }
    //Отобразили РП в адресное пространсвто программы
    if ((ret_str = mmap(NULL, size,PROT_READ | PROT_WRITE, MAP_SHARED, shr_id, 0)) == (void*)-1){
        perror("Ошибка присоединения сегмента");
        exit(EXIT_FAILURE);
    }
    return ret_str;
}

//Ф-ия отсоединения РП от адресного пространства программы и удаления
void close_del_shr(struct for_segm* shr_mem, char* name_shr, int size){
    //Отсоединили сегмент
    if (munmap(shr_mem, size) < 0){
        perror("Не удалось отсоединить сегмент");
        exit(EXIT_FAILURE); 
    }
    //Удалили
    if (shm_unlink(name_shr) < 0){
        perror("Не удалось удалить сегмент");
        exit(EXIT_FAILURE); 
    }
}

//Ф-ия создания семафора
sem_t* make_sem(char*name_sem, int value){
    
    sem_t* sem; 
    sem = sem_open(name_sem, O_CREAT, 0666, value);
    if (sem == SEM_FAILED) {
        perror("Не удалось создать семафор");
        exit(EXIT_FAILURE);
    }
    return sem;
}

//Ф-ия закрытия и удаления семафора
void close_del_sem(char*name_sem, sem_t* sem){
     //Закрыли семафор
    if (sem_close(sem) < 0){
        perror("Не удалось закрыть семафор");
        exit(EXIT_FAILURE); 
    }
     //Удалили
    if (sem_unlink(name_sem) < 0){
        perror("Не удалось удалить семафор");
        exit(EXIT_FAILURE); 
    }
}

//Ф-ия ожидания новых пользователей
void* join_usr(void* arg){
    struct for_thrd *str = (struct for_thrd *)arg;

    while(1){
        sem_wait(str->sgnl_srv_nwcl);
        if (str->flg_cls == 1){
            break;
        }

        //Захватываем семафор
        sem_wait(str->sem_acces_lst);

        printf("Пришёл пользователь %s\n", str->segm_shr->list_usr[str->segm_shr->qty_usr]);
        str->segm_shr->qty_usr++; 

        //Оповещаем клиентов 
        for(int i = 0; i < str->segm_shr->qty_usr; i++){
            sem_post(str->sgnl_cl_cllst);
        }

        sem_post(str->sem_acces_lst);
    }
   return NULL;
}

//Ф-ия ожидания новых сообщений
void* give_mesg(void* arg){
    struct for_thrd *str = (struct for_thrd *)arg;

    while(1){
        sem_wait(str->sgnl_srv_nwmesg);
        sem_wait(str->sem_acces_send);

        printf("Получено сообщение %s\n", str->segm_shr->list_mesg[str->segm_shr->qty_mesg]);

        // Если клиент прислал EXIT
        if (strcmp(str->segm_shr->list_mesg[str->segm_shr->qty_mesg], "EXIT") == 0){

            // Блокируем список пользователей перед удалением/сдвигом массива
            sem_wait(str->sem_acces_lst);
            
            int targ_ind = -1; //В переменную запишем индекс удаляемого пользователя
            //Поиск индекса удаляемого пользователя
            for(int i = 0; i < str->segm_shr->qty_usr; i++){
                if (strcmp(str->segm_shr->list_usr[i], str->segm_shr->list_name[str->segm_shr->qty_mesg]) == 0){
                    targ_ind = i;
                    break;
                }
            }
            //Удаляем пользователя/сдигаем массив
            if (targ_ind != -1) {
                str->segm_shr->qty_usr--;
                for(int j = targ_ind; j < str->segm_shr->qty_usr; j++){
                    strcpy(str->segm_shr->list_usr[j], str->segm_shr->list_usr[j+1]); 
                }
                printf("Удалил пользователя\n");
            }

            // Если чат пуст — закрываем сервер
            if (str->segm_shr->qty_usr == 0){
                str->flg_cls = 1;
                sem_post(str->sgnl_srv_nwcl);
                
                //Пробуждаем потоки отрисовки окон последего вышедшего клиента
                sem_post(str->sgnl_cl_cllst);
                sem_post(str->sgnl_cl_mesglst);

                sem_post(str->sem_acces_lst);
                sem_post(str->sem_acces_send);
                break;
            }
            //Делаем всем потокам рассылку,для закрытия клинета с флагом 1
            for(int i = 0; i <= str->segm_shr->qty_usr; i++){
                sem_post(str->sgnl_cl_cllst);
                sem_post(str->sgnl_cl_mesglst);
            }

            sem_post(str->sem_acces_lst);
            sem_post(str->sem_acces_send);
            continue; 
        }

        // Если пришло обычное сообщение
        if (str->segm_shr->qty_mesg != MESG_QTY-1){
            str->segm_shr->qty_mesg++;

            for(int i = 0; i < str->segm_shr->qty_usr; i++){
                sem_post(str->sgnl_cl_mesglst);
            }
        }

        sem_post(str->sem_acces_send);
    }
    return NULL;
}