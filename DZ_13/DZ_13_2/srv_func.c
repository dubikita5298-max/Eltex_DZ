#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <fcntl.h>     // Для open
#include <unistd.h>    // Для read, write, close, lseek
#include <sys/types.h> // Для типов данных
#include <sys/stat.h>  // Для режимов доступа (S_IRUSR и т.д.)
#include "inface.h"

//Функция создания MQ
int make_mq(char*str, key_t* key){
    int mqid = -1; //Возвращаемое значение ID очереди
    int fd;        //Дескриптор файла
    //Создали файл для ключа
    fd = open(str, O_WRONLY|O_CREAT,0644);
    if (fd < 0){
        perror("Не удалось создать файл");
        exit(EXIT_FAILURE);
    }
    //Закрыли дексриптор файла
    if (close(fd) < 0){
        perror("Ошибка закрытия дескриптора файла");
        exit(EXIT_FAILURE); 
    }
    //Создали ключ
    *key = ftok(str, 'A');
    if (*key < 0){
        perror("Не удалось содать ключ");
        exit(EXIT_FAILURE);
    }
    //Создали очередь сообщений
    mqid = msgget(*key, 0666|IPC_CREAT);
    if (mqid < 0){
        perror("Не удалось создать MQ");
        exit(EXIT_FAILURE);    
    }
    return mqid;
}

//Функция удаления MQ
void del_mq(int mqid, char*str){
    if (msgctl(mqid, IPC_RMID, NULL) < 0) {
        perror("Ошибка удаления MQ");
        exit(EXIT_FAILURE);
    }
    if (remove(str) < 0){
        perror("Ошибка удаления файла");
    }
}