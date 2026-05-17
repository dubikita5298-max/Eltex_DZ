#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <errno.h>
#include <unistd.h>    
#include "inface.h"

WINDOW* win_l = NULL;      //Левое окно
WINDOW* win_l_up = NULL;   //Заголовок левого окна
WINDOW* win_inl = NULL;    //Окно внутри левого
WINDOW* win_r = NULL;      //Правое окно
WINDOW* win_r_up = NULL;   //Заголовок правого окна
WINDOW* win_inr = NULL;    //Окно внутри правого
WINDOW* win_down = NULL;   //Окно нижнее 
WINDOW* win_in_down = NULL;//Окно внутри нижнего (под ввод сообщения)

//Ф-ия для создания интерфейса
void make_interface(void){

    initscr();                            //Инициализировали ncurse
    keypad(stdscr, TRUE); 
    start_color();                        //Включили поддержку цветов
    init_pair(1, COLOR_WHITE, COLOR_BLUE);//Создали цветовую пару,как основной цвет
    //Размеры окон и выводимый в них текст
    int sz_y_up = 3;
    int sz_x_up = 3*COLS/4;
    int sz_y_down = 6;
    int sz_x_down = COLS;  
    int sz_y_main = LINES - 9;
    int sz_x_main = 3*COLS/4;
    char headline_l[] = "MESSGES";//Заголовок левого окна
    char headline_r[] = "USERS";   //Заголовок правого окна

    //Заголовок левого окна
    win_l_up = newwin(sz_y_up, sz_x_up, 0, 0);                           //Создание окна
    wbkgd(win_l_up, COLOR_PAIR(1) | ' ');                                //Применяем к окну цвет фона и цвет текста
    wattron(win_l_up, COLOR_PAIR(1) | A_BOLD);                           //Включаем цвет и жирность
    mvwaddstr(win_l_up, 1, (sz_x_up - strlen(headline_l))/2, headline_l);//Перемещаем курсор в опр место и выводим текст
    wattroff(win_l_up, COLOR_PAIR(1) | A_BOLD);                          //Выключаем цвет и жирность
    box(win_l_up,0,0);                                                   //Создаём рамку по периметру окна
    wrefresh(win_l_up);                                                  //Обновление (выгрузка из памяти в окно)

    //Окно левое(рамка)
    win_l = newwin(sz_y_main, sz_x_main, 3, 0);
    wbkgd(win_l, COLOR_PAIR(1) | ' ');       
    box(win_l,0,0);                         
    wrefresh(win_l); 

    //Окно внутри левого, для вывода инфы
    win_inl = newwin(sz_y_main - 2, sz_x_main - 2, 4, 1); 
    wbkgd(win_inl, COLOR_PAIR(1) | ' ');                                        
    wrefresh(win_inl);                                  

    //Заголовок правого окна
    win_r_up = newwin(sz_y_up, (COLS/4), 0, sz_x_main);
    wbkgd(win_r_up, COLOR_PAIR(1) | ' ');
    wattron(win_r_up, COLOR_PAIR(1) | A_BOLD);                           
    mvwaddstr(win_r_up, 1, (COLS/4 - strlen(headline_r))/2, headline_r);
    wattroff(win_r_up, COLOR_PAIR(1) | A_BOLD);   
    box(win_r_up,0,0);
    wrefresh(win_r_up);
    
    //Окно правое (рамка)
    win_r = newwin(sz_y_main, (COLS/4), 3, sz_x_main);
    wbkgd(win_r, COLOR_PAIR(1) | ' ');
    box(win_r,0,0);
    wrefresh(win_r);

    //Окно внутри правого, для вывода инфы
    win_inr = newwin(sz_y_main - 2, (COLS/4 - 2), 4, sz_x_main + 1);
    wbkgd(win_inr, COLOR_PAIR(1) | ' ');
    wrefresh(win_inr);

    //Окно нижнее
    win_down = newwin(sz_y_down, sz_x_down, LINES - 6, 0);
    wbkgd(win_down, COLOR_PAIR(1) | ' ');
    box(win_down, 0,0);
    wrefresh(win_down);

    //Окно внутри нижнего
    win_in_down = newwin(sz_y_down - 2, sz_x_down - 2, LINES - 5, 1);
    wbkgd(win_in_down, COLOR_PAIR(1) | ' ');
    wrefresh(win_in_down);
}

//Ф-ия закрытия всех окон
void close_interface(void){
    delwin(win_l_up);
    delwin(win_r_up);
    delwin(win_inl);
    delwin(win_inr);
    delwin(win_down);
    delwin(win_l);
    delwin(win_r);

    endwin(); 
}

//Ф-ия для подключения пользователя к MQ
int connect_mq(char* str, key_t* key){
    int mqid = -1; //Возвращаемое значение ID очереди
    //Получили ключ
    *key = ftok(str, 'A');
    if (*key < 0){
        perror("Ошибка получения ключа");
        exit(EXIT_FAILURE);
    }
    //Получили ID MQ
    mqid = msgget(*key, 0);
    if (mqid < 0){
        perror("Не удалось создать MQ");
        exit(EXIT_FAILURE);    
    }
    return mqid;
}

//Ф-ия вывода на экран списка пользователей
void* make_list(void* arg){
    struct thrd_list *list = (struct thrd_list *)arg;
    ssize_t ret_size_lst;
    ssize_t ret_size_hstr;

    while(1){

        ret_size_lst = msgrcv(list->mqid_list, list->name_list, list->size_list, list->mtype, IPC_NOWAIT);
        if (ret_size_lst == list->size_list){
            //Если сервер передал флаг для завершения
            if (list->name_list->flag == 1){
                break;
            }
            //Очищаем окно и выводим список
            wclear(win_inr);
            for(int j = 0; j < list->name_list->usr_cnt; j++){
                mvwprintw(win_inr , j, 0, "%s", list->name_list->names[j]);
            }
            wrefresh(win_inr);    
        }
        else if (ret_size_lst < 0 && errno != ENOMSG){
            close_interface();
            perror("Не удалось получить список пользователей");
            exit(EXIT_FAILURE);
        }

        ret_size_hstr = msgrcv(list->mqid_hstr, list->hstr_mesg, list->size_hstr, list->mtype, IPC_NOWAIT);
        if (ret_size_hstr == list->size_hstr){
            if (list->hstr_mesg->flag == 1){
                break;
            }
            wclear(win_inl);
            for(int j = 0; j < list->hstr_mesg->mesg_cnt; j++){
                mvwprintw(win_inl, j, 0, "%s: %s", list->hstr_mesg->names[j], list->hstr_mesg->mesg[j]);
            }
            wrefresh(win_inl);    //Обновление окна после всех изменений
        }
        else if (ret_size_hstr < 0 && errno != ENOMSG){
            close_interface();
            perror("Не удалось получить историю сообщений");
            exit(EXIT_FAILURE);
        }
        sleep(1);
    }
    
    return NULL;
}

//Ф-ия для отправки сообщений
void* send_mesg(void* arg){
    struct thrd_msg_snd* msg =(struct thrd_msg_snd *)arg;

    while(1){
        curs_set(1);
        echo(); 
        //Вводим сообщение
        mvwgetnstr(win_in_down, 0, 0, msg->usr_mesg_str->mesg, MESG_LRN);
        noecho(); 
        curs_set(0);
        wmove(win_in_down, 0, 0); 
        wclrtoeol(win_in_down);   
        wrefresh(win_in_down); 
        //Передаём сообщение сервису
        if (msgsnd(msg->mqid, msg->usr_mesg_str, msg->size_data, 0) < 0){
            close_interface();
            perror("Не удалось передать сообщение");
            exit(EXIT_FAILURE);
        }
        //Для закрытия интерфейса вводим EXIT
        if (strcmp(msg->usr_mesg_str->mesg, "EXIT") == 0){
            break;
        }
    }

    return NULL;
}