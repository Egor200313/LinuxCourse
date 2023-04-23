#ifndef PHONEBOOK_H
#define PHONEBOOK_H

#define PHONEBOOK_MAGIC 'p'
#define GET_USER _IOR(PHONEBOOK_MAGIC, 1, char*)
#define ADD_USER _IOW(PHONEBOOK_MAGIC, 2, struct user_data*)
#define DEL_USER _IOW(PHONEBOOK_MAGIC, 3, char*)

struct user_data {
    char name[20];
    char surname[20];
    int age;
    char phone[20];
    char email[50];
};

#endif /* PHONEBOOK_H */
