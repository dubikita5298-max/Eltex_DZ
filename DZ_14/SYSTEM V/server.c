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

    int fd;                                 //Дескриптор файла для ключа
    char* segm;                             //Ук-ль на начало разделяемого сегмента 
    char mesg_snd[] = "Hi!";                //Строка для отправки 

    struct sembuf incr = { 0,  1, 0};       //Структура для инкрементирования семафора
    struct sembuf wait = {0,  -1, 0};       //Структура для декрементирования семафора

    //Создали файл, для ключа сегмента 
    fd = open(file_for_shr, O_WRONLY|O_CREAT,0644);
    if (fd < 0){
        perror("Не удалось создать файл");
        exit(EXIT_FAILURE);
    }
    //Закрыли дексриптор файла
    if (close(fd) < 0){
        perror("Ошибка закрытия дескриптора файла");
        exit(EXIT_FAILURE); 
    }
    //Создали ключ для сегмента
    key_shr = ftok(file_for_shr, 'A');
    if (key_shr < 0){
        perror("Не удалось содать ключ");
        exit(EXIT_FAILURE);
    }
    //Создали сегмент разделяемой памяти
    shr_id = shmget(key_shr, SIZE, 0666|IPC_CREAT);
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

    //Создали файл, для ключа 1-го семафора 
    fd = open(file_for_sem_1, O_WRONLY|O_CREAT,0644);
    if (fd < 0){
        perror("Не удалось создать файл");
        exit(EXIT_FAILURE);
    }
    //Закрыли дексриптор файла
    if (close(fd) < 0){
        perror("Ошибка закрытия дескриптора файла");
        exit(EXIT_FAILURE); 
    }
    //Создали файл, для ключа 2-го семафора
    fd = open(file_for_sem_2, O_WRONLY|O_CREAT,0644);
    if (fd < 0){
        perror("Не удалось создать файл");
        exit(EXIT_FAILURE);
    }
    //Закрыли дексриптор файла
    if (close(fd) < 0){
        perror("Ошибка закрытия дескриптора файла");
        exit(EXIT_FAILURE); 
    }

    //Создали ключ для семафора
    key_sem_1 = ftok(file_for_sem_1, 'B');
    key_sem_2 = ftok(file_for_sem_2, 'C');
    if (key_sem_1 < 0 || key_sem_2 < 0){
        perror("Не удалось содать ключ");
        exit(EXIT_FAILURE);
    }
    //Создади семафор
    sem_id_1 = semget(key_sem_1, 1, IPC_CREAT | 0666);
    sem_id_2 = semget(key_sem_2, 1, IPC_CREAT | 0666);
    if (sem_id_1 < 0 || sem_id_2 < 0){
        perror("Не удалось содать семафор");
        exit(EXIT_FAILURE);
    }
    //Инициализировали семафор 0
    if (semctl(sem_id_1, 0, SETVAL, 0) < 0 || semctl(sem_id_2, 0, SETVAL, 0) < 0) {
        perror("Не удалось инициализировать семафор");
        exit(EXIT_FAILURE);
    }

    //Сохранили строку по адресу сегмента
    strcpy(segm, mesg_snd);
    printf("Передали в сегмент %s\n", segm);

    //Инкрементировали 2-ой семафор для клиента
    if (semop(sem_id_2, &incr, 1) < 0){
        perror("Не удалось захватить семафор");
        exit(EXIT_FAILURE);
    } 
    //Заблокировались на 1-ом семафоре
    if (semop(sem_id_1, &wait, 1) < 0){
        perror("Не удалось захватить семафор");
        exit(EXIT_FAILURE);
    }
    //Читаем ответную строку из сегмента после разблокировки
    printf("Строка из сегмента %s\n", segm);

    //Отключаем сегмент от адресного пространства и удаляем
    if (shmdt(segm) < 0){
        perror("Не удалось отсоединить сегмент");
        exit(EXIT_FAILURE); 
    }
    if (shmctl(shr_id, IPC_RMID, NULL) < 0){
        perror("Не удалось удалить сегмент");
        exit(EXIT_FAILURE); 
    }
    //Удаляем семафоры
    if (semctl(sem_id_1, 0, IPC_RMID) < 0 || semctl(sem_id_2, 0, IPC_RMID) < 0){
        perror("Ошибка удаления семафоров");
    }
    //Удаляем файлы
    if (remove(file_for_shr) < 0 || remove(file_for_sem_1) < 0 || remove(file_for_sem_2) < 0){
        perror("Ошибка удаления файла");
    }

    return 0;
}