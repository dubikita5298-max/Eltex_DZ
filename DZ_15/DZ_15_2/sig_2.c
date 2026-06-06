#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main(void) {
    sigset_t set;
    int ret;

    //Очищаем маску (сбрасываем все сигналы)
    sigemptyset(&set);
    
    //Добавляем сигнал SIGINT в маску для блокировки
    sigaddset(&set, SIGINT);
    
    //Устанавливаем блокировку для сигналов из маски set
    ret = sigprocmask(SIG_BLOCK, &set, NULL);
    if (ret < 0) {
        perror("Ошибка блокировки сигнала");
        exit(EXIT_FAILURE);
    }

    printf("Сигнал SIGINT заблокирован\n");

    // Бесконечный цикл
    while (1) {
        sleep(1);
    }

    return 0;
}