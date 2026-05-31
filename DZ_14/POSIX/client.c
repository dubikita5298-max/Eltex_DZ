#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>      
#include <unistd.h>     
#include <sys/types.h>
#include <sys/mman.h>   
#include <sys/stat.h>  
#include <semaphore.h>

#define SIZE 24//Размер сегмента

int main(){
    char name_shr[] = "/shr_seg";        //Имя сегмента
    char mesg_snd[] = "Hello!";          //Строка для отправки   
    
    char name_sem_1[] = "/shr_sem_1";   //Имя 1-го семафора
    char name_sem_2[] = "/shr_sem_2";   //Имя 2-го семафора
    sem_t* sem_1;                       //Указатель на дескриптор 1-го семафора
    sem_t* sem_2;                       //Указатель на дескриптор 2-го семафора

    int shr_fd;                         //ID разделяемого сегмента
    char* segm;                         //Ук-ль на начало разделяемого сегмента 

    //Подключились к сегменту разделяемой памяти
    if ((shr_fd = shm_open(name_shr, O_RDWR, 0)) < 0){
        perror("Сегмент не найден");
        exit(EXIT_FAILURE);
    }

    //Отобразили сегмент в адресное пространство
    if ((segm = mmap(NULL, SIZE,PROT_READ | PROT_WRITE, MAP_SHARED, shr_fd, 0)) == (void*)-1){
        perror("Ошибка присоединения сегмента");
        exit(EXIT_FAILURE);
    }

    //Подключили семафор
    sem_1 = sem_open(name_sem_1, 0);
    sem_2 = sem_open(name_sem_2, 0);
    if (sem_1 == SEM_FAILED || sem_2 == SEM_FAILED) {
        perror("Не удалось открыть семафор");
        exit(EXIT_FAILURE);
    }

    //Заходим во второй семафор
    //Читаем и передаём строку
    sem_wait(sem_2);

    printf("Строка из сегмента %s\n", segm);
    strcpy(segm, mesg_snd);
    printf("Передали в сегмент %s\n", segm);

    //Инкрементировали 1-ый семафор, для разблокировки сервера
    sem_post(sem_1);

    //Закрываем семафор
    if (sem_close(sem_1) < 0 || sem_close(sem_2) < 0){
        perror("Не удалось закрыть семафор");
        exit(EXIT_FAILURE); 
    }

    //Отключаем сегмент от адресного пространства
    if (munmap(segm, SIZE) < 0){
        perror("Не удалось отсоединить сегмент");
        exit(EXIT_FAILURE); 
    }
    return 0;
}