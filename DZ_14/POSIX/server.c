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
    char name_shr[] = "/shr_seg";       //Имя сегмента
    char mesg_snd[] = "Hi!";            //Строка для отправки   
    
    char name_sem_1[] = "/shr_sem_1";   //Имя 1-го семафора
    char name_sem_2[] = "/shr_sem_2";   //Имя 2-го семафора
    sem_t* sem_1;                       //Указатель на дескриптор 1-го семафора
    sem_t* sem_2;                       //Указатель на дескриптор 2-го семафора

    int shr_fd;                         //ID разделяемого сегмента
    char* segm;                         //Ук-ль на начало разделяемого сегмента 

    //Создали сегмент разделяемой памяти
    if ((shr_fd = shm_open(name_shr, O_RDWR|O_CREAT, 0666)) < 0){
        perror("Ошибка создания сегмента");
        exit(EXIT_FAILURE);
    }
    
    //Устанавливаем размер файла/сегмента
    if (ftruncate(shr_fd, SIZE) < 0){
        perror("Ошибка создания сегмента");
        exit(EXIT_FAILURE);
    }

    //Отобразили сегмент в адресное пространство
    if ((segm = mmap(NULL, SIZE,PROT_READ | PROT_WRITE, MAP_SHARED, shr_fd, 0)) == (void*)-1){
        perror("Ошибка присоединения сегмента");
        exit(EXIT_FAILURE);
    }

    //Создали семафор
    sem_1 = sem_open(name_sem_1, O_CREAT, 0666, 0);
    sem_2 = sem_open(name_sem_2, O_CREAT, 0666, 0);
    if (sem_1 == SEM_FAILED || sem_2 == SEM_FAILED) {
        perror("Не удалось создать семафор");
        exit(EXIT_FAILURE);
    }

    //Сохранили строку по адресу сегмента
    strcpy(segm, mesg_snd);
    printf("Передали в сегмент %s\n", segm);

     //Инкрементируем 2-ой семафор (сигнал для клиента)
    sem_post(sem_2);

    //Блокрируемся на 1 семафоре
    sem_wait(sem_1);

    //Разблокировались после того, как клиент проинкрементировал 1-ый семафор
    printf("Строка из сегмента %s\n", segm);

    //Закрываем семафор и удаляем
    if (sem_close(sem_1) < 0 || sem_close(sem_2) < 0){
        perror("Не удалось закрыть семафор");
        exit(EXIT_FAILURE); 
    }
    if (sem_unlink(name_sem_1) < 0 || sem_unlink(name_sem_2) < 0){
        perror("Не удалось удалить семафор");
        exit(EXIT_FAILURE); 
    }

    //Отключаем сегмент от адресного пространства и удаляем
    if (munmap(segm, SIZE) < 0){
        perror("Не удалось отсоединить сегмент");
        exit(EXIT_FAILURE); 
    }
    if (shm_unlink(name_shr) < 0){
        perror("Не удалось удалить сегмент");
        exit(EXIT_FAILURE); 
    }
    
    return 0;
}