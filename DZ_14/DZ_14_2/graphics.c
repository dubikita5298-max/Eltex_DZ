#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <errno.h>
#include <unistd.h>    
#include <fcntl.h> 
#include <semaphore.h>
#include <sys/mman.h> 
#include <locale.h> 
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

    setlocale(LC_ALL, "");
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

//Ф-ия для обнов экрана списка пользователей
void* make_us_lst(void* arg){
    struct for_thrd *str = (struct for_thrd *)arg;
    while(1){
        //Ждём сигнал от сервера
        sem_wait(str->sgnl_cl_cllst);
        
        //Захватываем семафор перед проверкой флага закрытия и чтением
        sem_wait(str->sem_acces_lst);
        if(str->flg_cls == 1){
            sem_post(str->sem_acces_lst);
            break;
        }

        werase(win_inr);
        for(int i = 0; i < str->segm_shr->qty_usr; i++){
            wmove(win_inr, i, 0);
            wclrtoeol(win_inr);
            mvwprintw(win_inr, i, 0, "%s", str->segm_shr->list_usr[i]);
        }
        wrefresh(win_inr);

        sem_post(str->sem_acces_lst);
    }
    return NULL;
}


//Ф-ия для обнов экрана сообщений
void* make_mesg_lst(void* arg){
    struct for_thrd *str = (struct for_thrd *)arg;
    while(1){
        //Ждём сигнал от сервера
        sem_wait(str->sgnl_cl_mesglst);
        
        // Захватываем семафор перед проверкой флага закрытия и чтением
        sem_wait(str->sem_acces_send);
        if(str->flg_cls == 1){
            sem_post(str->sem_acces_send);
            break;
        }

        werase(win_inl);
        for(int i = 0; i < str->segm_shr->qty_mesg; i++){
            wmove(win_inl, i, 0);
            wclrtoeol(win_inl);
            mvwprintw(win_inl, i, 0, "%s: %s", str->segm_shr->list_name[i], str->segm_shr->list_mesg[i]);
        }
        wrefresh(win_inl);    

        sem_post(str->sem_acces_send);
    }
    return NULL;
}

//Ф-ия для отправки сообщений
void* make_mesg(void* arg){
    struct for_thrd *str = (struct for_thrd *)arg;
    char buf_mesg[MESG_LRN]; 

    while(1){
        flushinp(); 
        curs_set(1);
        echo(); 

        mvwgetnstr(win_in_down, 0, 0, buf_mesg, MESG_LRN - 1);
        
        noecho(); 
        curs_set(0);
        wmove(win_in_down, 0, 0); 
        wclrtoeol(win_in_down);   
        wrefresh(win_in_down);

        //Захватываем семафор
        sem_wait(str->sem_acces_send);

        //Если история сообщений заполнена завершаем клиента
        if (str->segm_shr->qty_mesg == MESG_QTY - 1){
            strncpy(str->segm_shr->list_mesg[str->segm_shr->qty_mesg], "EXIT", MESG_LRN);
            strncpy(str->segm_shr->list_name[str->segm_shr->qty_mesg], str->name, NAME_LRN); 
        }
        //Если в истории есть место сохраняем строку
        else {
            strncpy(str->segm_shr->list_mesg[str->segm_shr->qty_mesg], buf_mesg, MESG_LRN);
            strncpy(str->segm_shr->list_name[str->segm_shr->qty_mesg], str->name, NAME_LRN); 
        }
        //Ветка для выхода
        if (strcmp(str->segm_shr->list_mesg[str->segm_shr->qty_mesg], "EXIT") == 0){
            str->flg_cls = 1; // Ставим локальный флаг для потоков этого клиента
            
            sem_post(str->sgnl_srv_nwmesg); //Будим сервер
            sem_post(str->sem_acces_send);  
            
            break; 
        } 
        else {
            sem_post(str->sgnl_srv_nwmesg); 
            sem_post(str->sem_acces_send);  
        }
    }
    return NULL;
}


//Ф-ия для подключения к сегменту РП
struct for_segm* connect_share  (char*name_shr, int size){
    
    struct for_segm* ret_str;
    int shr_id;
    //Подключились к сегменту разделяемой памяти
    if ((shr_id = shm_open(name_shr, O_RDWR, 0)) < 0){
        perror("Сегмент не найден");
        exit(EXIT_FAILURE);
    }

    //Отобразили сегмент в адресное пространство
    if ((ret_str = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shr_id, 0)) == (void*)-1){
        perror("Ошибка присоединения сегмента");
        exit(EXIT_FAILURE);
    }
    return ret_str;
}

//Ф-ия закрытия РП
void close_shr (struct for_segm* segm, int size){

    if (munmap(segm, size) < 0){
        perror("Не удалось отсоединить сегмент");
        exit(EXIT_FAILURE); 
    }
}

//Ф-ия подключения семафоров
sem_t* connect_sem (char* name_sem){
    
    sem_t* sem = sem_open(name_sem, 0);
    if (sem == SEM_FAILED) {
        perror("Не удалось открыть семафор");
        exit(EXIT_FAILURE);
    }
    return sem;
}

//Ф-ия закрытия семафоров
void close_sem (sem_t* sem){

    if (sem_close(sem) < 0){
        perror("Не удалось закрыть семафор");
        exit(EXIT_FAILURE); 
    }
}