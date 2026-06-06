#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

void sig_handler(int sig_num, siginfo_t* info, void* args){
    printf("Пришёл сигнал SIGUSR1\n");
}

int main(){
    int ret;
    sigset_t set;
    struct sigaction handler;           

    //Сбросили маску
    sigemptyset(&set);   
    //Добавили в маску сигнал SIGISR1               
    sigaddset(&set, SIGUSR1);           
    
    //Инициализироввли поля структуры
    handler.sa_sigaction = sig_handler; 
    handler.sa_mask = set;              
    handler.sa_flags = SA_SIGINFO;

    //Сделали диспозицию сигнала
    ret = sigaction(SIGUSR1, &handler, NULL);
    if (ret < 0){
        perror("Ошибка диспозиции сигнала");
        exit(EXIT_FAILURE);
    }

    while(1){
        sleep(1);
    }
    return 0;
}