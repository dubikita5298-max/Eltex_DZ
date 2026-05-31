#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h> 
#define SIZE 24      //Размер сегмента

int main(){
    char file_for_shr[] = "key_shr.txt";    //Имя файла для ключа сегмента
    key_t key_shr;                          //Ключ для сегмента
    int shr_id;                             //ID сегмента

    char file_for_sem_1[] = "key_sem_1.txt";//Имя файла для ключа семафора 1
    key_t key_sem_1;                        //Ключ для семафора 1
    int sem_id_1;                           //ID семафора 1

    char file_for_sem_2[] = "key_sem_2.txt";//Имя файла для ключа семафора 2
    key_t key_sem_2;                        //Ключ для семафора 2
    int sem_id_2;                           //ID семафора 2

    char* segm;                             //Ук-ль на начало разделяемого сегмента 
    char mesg_snd[] = "Hello!";             //Строка для отправки 

    struct sembuf incr = { 0,  1, 0};       //Структура для инкрементирования семафора
    struct sembuf wait = {0,  -1, 0};       //Структура для декрементирования семафора

    //Получили ключ сегмента
    key_shr = ftok(file_for_shr, 'A');
    if (key_shr < 0){
        perror("Не удалось содать ключ");
        exit(EXIT_FAILURE);
    }
    //Получили id сегмента разделяемой памяти
    shr_id = shmget(key_shr, SIZE, 0);
    if (shr_id < 0){
        perror("Не удалось создать сегмент");
        exit(EXIT_FAILURE);    
    }
    //Отобразили сегмент в адресное пространство
    segm = shmat(shr_id, NULL, 0);
    if (segm == (void*)-1){
        perror("Не удалось присоединить сегмент");
        exit(EXIT_FAILURE);    
    }

    //Получили ключ для семафора
    key_sem_1 = ftok(file_for_sem_1, 'B');
    key_sem_2 = ftok(file_for_sem_2, 'C');
    if (key_sem_1 < 0 || key_sem_2 < 0){
        perror("Не удалось содать ключ");
        exit(EXIT_FAILURE);
    }
    //Получили id семафора
    sem_id_1 = semget(key_sem_1, 1, IPC_CREAT | 0666);
    sem_id_2 = semget(key_sem_2, 1, IPC_CREAT | 0666);
    if (sem_id_1 < 0 || sem_id_2 < 0){
        perror("Не удалось содать семафор");
        exit(EXIT_FAILURE);
    }

    //Заходим во 2-ой семафор
    if (semop(sem_id_2, &wait, 1) < 0){
        perror("Не удалось захватить семафор");
        exit(EXIT_FAILURE);
    }
    //Читаем строку и отправляем ответ
    printf("Строка из сегмента %s\n", segm);
    strcpy(segm, mesg_snd);
    printf("Передали в сегмент %s\n", segm);

    //Инкрементировали 1-ой семафор для сервера
    if (semop(sem_id_1, &incr, 1) < 0){
        perror("Не удалось захватить семафор");
        exit(EXIT_FAILURE);
    } 

    //Отключаем сегмент от адресного пространства
    if (shmdt(segm) < 0){
        perror("Не удалось отсоединить сегмент");
        exit(EXIT_FAILURE); 
    }

    return 0;
}