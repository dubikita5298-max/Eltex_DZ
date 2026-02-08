#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "int_face.h"
#define SIZE 20                     //Хранит размер массивов строк

struct abon_dir {                   // Объявление структуры для справочника
    char name [SIZE];               // Массив для имени абонента
    char sec_name [SIZE];           // Массив для фамилии абонента
    char tel_num [SIZE];            // Массив для телефона абонента
  struct abon_dir*  next;           // Указатель на структуру (адрес следующего звена списка)
  struct abon_dir* prev;            // Указатель на структуру (адрес предыдущего звена списка)
};

struct abon_dir* add_ab(struct abon_dir* head){  // (1) Ф-ия добавления абонента (добавление происходит в начало)
   
  struct abon_dir* temp = NULL;            // Объявление и инициализация указателя на структуру
   temp = malloc(sizeof(struct abon_dir)); // Выделение памяти под структуру 
   if (temp != NULL) {                     // Если память успешно выделилась, инициализируются поля структуры
    printf("Введите имя абонента ");
    s_gets(temp->name);
    printf("Введите фамилию абонента ");
    s_gets(temp->sec_name);
    printf("Введите номер телефона абонента ");
    s_gets(temp->tel_num);
    temp->next = head;  //Записали в новое звено в next адрес текущей головы
    temp->prev = NULL;  //Записали в новое звено в prev NULL
    if (head != NULL){  //Условие если список изначально был не пуст
      head->prev = temp;//Записали в текущую голову в prev адрес нового звена
    }
    head = temp;        //Обновили голову на новое созданное звено
    return head;        //Возвращаем новую голову
}
else {                  // Если память не выделилась, сообщение об ошибке
perror("Ошибка выделения памяти\n");
exit(EXIT_FAILURE);
} 
}

struct abon_dir* del_ab(struct abon_dir*head){ // (2) Ф-ия для удаления абонента
  
  struct abon_dir* new_head = head;  // Указатель, который вернёт ф-ия (новый head)
  struct abon_dir* temp = NULL;      // Временная переменная (указатель) для хранения внутри цикла адреса на следующее звено
  char del_name[SIZE];               // Строка для имени удаляемого абонента
  char del_sec_name[SIZE];           // Строка для фамилии удаляемого абонента
  int count = 0;                     // Счётчик для контроля поиска
 
// Ввод имени и фамилии для поиска и удаления
printf ("Введите имя для поиска ");
s_gets(del_name);
printf ("Введите фамилию для поиска ");
s_gets(del_sec_name);

while (head != NULL){ // Цикл повторяется пока не дойдём до последенго звена в поле next, которого хранится NULL
  temp = head->next;  // temp каждую итерацию инициализируется адресом следующего звена
    if (strcmp(del_name, head->name) == 0 && strcmp(del_sec_name, head->sec_name) == 0){ // Если имя и фамилия нашлись
      count++;                                        // Если было обнаруженно совпадение увеличение счётчика
       if (head->prev == NULL && head->next == NULL){ // Если список состоит из одного элемента
        new_head = NULL;                              // В возвращаемый указатель записывается NULL, список будет пуст после высвобождения
        free (head);                                  // Высвобождение памяти
       }
       else if (head->next == NULL){                  // Если найденное звено было последним (хвостом)
        head->prev->next = NULL;                      // В звено до него в next записываем  NULL (теперь это хвост)
        free (head);                                  // Высвобождение памяти
       }
       else if (head->prev == NULL){                  // Если найденное звено было первым (головой) 
        head->next->prev = NULL;                      // В звено после него в prev записываем  NULL (теперь это новая голова)
        new_head = head->next;                        // Обновляем указатель на голову
        free (head);                                  // Высвобождение памяти
       }
       else {                                         // Если найденное звено было в середине списка (не первым и последним)
            head->prev->next = head->next;            // В предыдущее звено в next записываем следующее за текущим
            head->next->prev = head->prev;            // В следующее звено в prev записывается, то, что до текущего
            free (head);                              // Высвобождение памяти
       }
  }
  head = temp; // Записываем в head адрес следующего звена
}
if (count == 0){  // Если счётчик после завершения списка не увеличился, значит совпадений не найдено
  printf ("Абонент не найден\n");
}
else {
  printf("Абонент удалён\n");
}
return new_head;
}

void search_ab (struct abon_dir* head){ // (3) Ф-ия для поиска абонента
 
  int count = 0;            // Счётчик для контроля поиска
  char srch_name[SIZE];     // Строка для имени удаляемого абонента
  char srch_sec_name[SIZE]; // Строка для фамилии удаляемого абонента
  // Ввод имени и фамилии для поиска 
  printf ("Введите имя для поиска ");
  s_gets(srch_name);
  printf ("Введите фамилию для поиска ");
  s_gets(srch_sec_name);
  
  printf("Результат:\n");
  while (head!= NULL){   // Цикл повторяется пока не дойдём до последенго звена в поле next, которого хранится NULL
    if (strcmp (srch_name, head->name) == 0 && strcmp (srch_sec_name, head->sec_name) == 0){// Если нашлось совпадение
      printf ("%s %s %s\n", head->name, head->sec_name, head->tel_num);   // Выводятся данные найденного абонента
      count++;                                // Если было обнаруженно совпадение увеличение счётчика
    }
    head = head->next;
  }
  if (count == 0) { // Если счётчик после завершения списка не увеличился, значит совпадений не найдено
         printf ("Абоненты с таким именем отсутствуют\n");
  }
}

void print_ab(struct abon_dir* head){  // (4) Ф-ия для вывода на экран всех абонентов 
    
  struct abon_dir* temp = head;
    while (temp != NULL) {
    printf ("Имя: %s Фамилия: %s Номер телефона: %s\n", temp->name, temp->sec_name, temp->tel_num);
    temp = temp->next;
    }
}

void free_all (struct abon_dir* head){  // (5) Ф-ия для освобождения памяти при завершении программы
 
  struct abon_dir* temp = NULL; // Временная переменная для хранения адреса следующего звена
  while (head != NULL){
    temp = head->next;
    free(head);
    head = temp;
  }
}

char* s_gets(char* string){ // Ф-ия для ввода строки

  char* str = fgets(string, SIZE, stdin); //Указатель на начало введённой строки
  if (str != NULL) {                      //Проверка условия того, что строка введена без ошибок
    char* zam = strchr(str, '\n');        //Поиск символа новой строки в введённой строке
    if (zam != NULL) {                    //Если символ новой строки найден
      *zam ='\0';                         //на его место записывается символ окончания строки.
    }
    else {  //Если символ новой строки не найден, значит было введено кол-во символов превышающее размер строки.
       while (getchar() != '\n'); //Очистка буфера от лишних символов, пока не встретится символ новой строки.
    }
  }
  return str;
}
