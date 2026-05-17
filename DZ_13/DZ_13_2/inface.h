#ifndef INFACE_H     //Условие для исключения повторного присоединения файла
#define INFACE_H  
#define USRS_QTY 10  //Макс. кол-во пользователей чата
#define NAME_LRN 10  //Макс. разм. имени пользователя
#define MESG_LRN 24  //Макс. разм. сообщения
#define MESG_QTY 64  //Макс. кол-во сообщений

struct usr_mesg {                      //Стр-ра, передающая инф-ию о клиенте серверу  
    long serv_type;
    long mytype;
    char name[NAME_LRN];
    char mesg[MESG_LRN];
};

struct usr_list {                      //Стр-ра для хранения списка пользователей
    long mtype;
    int flag;
    int usr_cnt;
    char names[USRS_QTY][NAME_LRN];
};

struct mesg_hstr {                      //Стр-ра для хранения истории сообщений
    long mtype;
    int flag;
    int mesg_cnt;
    char names[MESG_QTY][NAME_LRN];
    char mesg[MESG_QTY][MESG_LRN];
};


struct thrd_list {                      //Стр-ра для потока под вывод списка пользоватлей+истории сообщений
    long mtype;
    int mqid_list;
    int mqid_hstr;
    struct usr_list* name_list;
    struct mesg_hstr* hstr_mesg;
    ssize_t size_list;
    ssize_t size_hstr;
};

struct thrd_msg_snd {                      //Стр-ра для потока под отправку сообщений
    int mqid;
    struct usr_mesg* usr_mesg_str;
    ssize_t size_data;
};

//Ф-ии клиентов
void make_interface(void);     //Прототип ф-ии формирования интерфейса 
void close_interface(void);    //Прототип ф-ии закрытия интерфейса
void* make_list(void*);        //Прототип ф-ии формирования списка пользователей
void* send_mesg(void* arg);    //Прототип ф-ии отправка сообщений в чат
int connect_mq(char*, key_t*); //Прототип ф-ии создания MQ

//Ф-ии сервера
int make_mq(char*, key_t*);   //Прототип ф-ии создания MQ
void del_mq(int, char*);      //Прототип ф-ии удаления MQ

#endif