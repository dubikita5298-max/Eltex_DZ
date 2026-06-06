#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>

int main(void) {
    pid_t target_pid;

    //Запрашиваем PID у пользователя
    printf("Введите PID программы: ");
    if (scanf("%d", &target_pid) != 1) {
        printf("Ошибка ввода PID\n");
        exit(EXIT_FAILURE);
    }

    printf("Отправка сигнала SIGUSR1 процессу %d\n", target_pid);

    //Отправляем сигнал
    if (kill(target_pid, SIGUSR1) == -1) {
        perror("Ошибка отправки сигнала через kill()");
        exit(EXIT_FAILURE);
    }

    printf("Сигнал успешно отправлен.\n");
    return 0;
}