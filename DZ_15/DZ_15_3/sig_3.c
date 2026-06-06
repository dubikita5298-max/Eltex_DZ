#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main(void) {
    sigset_t set; 
    int ret;
    int sig_num;

    //Очищаем маску
    sigemptyset(&set);
    //Добавляем в маску сигнал
    sigaddset(&set, SIGUSR1);

    //Блокируем сиганл сигнал
    ret = sigprocmask(SIG_BLOCK, &set, NULL);
    if (ret < 0) {
        perror("Ошибка блокировки сигнала");
        exit(EXIT_FAILURE);
    }

    //Цикл ожидания
    while(1) {
        sigwait(&set, &sig_num);
        printf("Получен сигнал с номером %d\n", sig_num);
    }

    return 0;
}