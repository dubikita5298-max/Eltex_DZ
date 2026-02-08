#ifndef INT_FACE_H     // Условие для исключения повторного присоединения файла
#define INT_FACE_H 
struct abon_dir;                                // Объявление структуры

struct abon_dir* add_ab(struct abon_dir* head); // Прототип ф-ии добавления абонента
struct abon_dir* del_ab(struct abon_dir*head);  // Прототип ф-ии удаления абонента
void search_ab (struct abon_dir* head);         // Прототип ф-ии для поиска абонента
void print_ab(struct abon_dir* head);           // Прототип ф-ии для выовда на экрна всех абонентов 
void free_all (struct abon_dir* head);          // Прототип ф-ии для освобождения памяти при завершении программы
char* s_gets(char* string);                     // Прототип ф-ии для ввода строки

#endif