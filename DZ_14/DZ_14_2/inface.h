#ifndef INFACE_H        //Условие для исключения повторного присоединения файла
#define INFACE_H  
#define USRS_QTY 10     //Макс. кол-во пользователей чата
#define NAME_LRN 10     //Макс. разм. имени пользователя
#define MESG_LRN 24     //Макс. разм. сообщения
#define MESG_QTY 10     //Макс. кол-во сообщений
#define SIZE_SHR 4096   //Размер сегмента

//Структура для разделяемой памяти
struct for_segm {                   
    int qty_usr;                        //Счётчик кол-ва польз
    int qty_mesg;                       //Счётчик кол-ва сообщений          
    char list_usr[USRS_QTY][NAME_LRN];  //Массив списка пользователей
    char list_name[MESG_QTY][MESG_LRN]; //Массив имён истории сообщений
    char list_mesg[MESG_QTY][MESG_LRN]; //Массив сообщений
    int flag_ign;
};

//Структура, передаваемая в потоки клиента/сервера
struct for_thrd {                   
    sem_t* sem_acces_lst;       //Для доступа/добавления нового кл 
    sem_t* sem_acces_send;      //Для доступа/отправки сообщений

    sem_t* sgnl_srv_nwcl;       //Сигнал серверу о новом клиенте
    sem_t* sgnl_srv_nwmesg;     //Сигнал серверу о новом сообщении

    sem_t* sgnl_cl_cllst;       //Сигнал кл-ам обновлять экран списка пользователей
    sem_t* sgnl_cl_mesglst;     //Сигнал кл-ам обновлять экран сообщений
    char name [NAME_LRN];       //Хранит имя пользователя
    struct for_segm* segm_shr;  //Указатель на сегментРП

    int flg_cls;                //Флаг для завершения работы потоков клинета/сервера
};

//Ф-ии клиентов
void make_interface(void);                       //Прототип ф-ии формирования интерфейса 
void close_interface(void);                      //Прототип ф-ии закрытия интерфейса

struct for_segm* connect_share (char*, int);     //Прототип ф-ии подключения к РП
sem_t* connect_sem (char*);                      //Прототип ф-ии подключения к семафорам
void close_shr (struct for_segm* , int);         //Прототип ф-ии отсоединения РП
void close_sem (sem_t*);                         //Прототип ф-ии закрытия семафора

void* make_us_lst(void* arg);                    //Прототип ф-ии потока для фор-ия списка пользователей
void* make_mesg_lst(void* arg);                  //Прототип ф-ии потока для фор-ия списка пользователей
void* make_mesg(void* arg);                      //Прототип ф-ии потока для отправки сообщений

//Ф-ии сервера
struct for_segm* make_share(char*, int);         //Прототип ф-ии создания РП
void close_del_shr(struct for_segm*, char*, int);//Прототип ф-ии закр и удаления РП
sem_t* make_sem(char*, int);                     //Прототип ф-ии оздания семафора
void close_del_sem(char*, sem_t*);               //Прототип ф-ии закр и удаления семафора

void* join_usr(void*);                           //Прототип ф-ии ожидания новых клиентов
void* give_mesg(void*);                          //Прототип ф-ии ожидания новых сообщений
#endif