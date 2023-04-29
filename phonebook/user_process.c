#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

#include "phonebook.h"

int main() {
    int fd = open("/dev/phonebook", O_RDWR);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    struct user_data user;
    strcpy(user.name, "John");
    strcpy(user.surname, "Doe");
    user.age = 30;
    strcpy(user.phone, "1234567890");
    strcpy(user.email, "john.doe@example.com");
    
    
    
    if (ioctl(fd, ADD_USER, &user) < 0) {
        perror("not added");
        return 1;
    }
    
    
    char arg[sizeof(struct user_data)] = "Doe";
    if (ioctl(fd, GET_USER, arg) < 0) {
        perror("not found");
        return 1;
    }
    
    printf("Name: %s\n", ((struct user_data*)arg)->name);
    printf("Surname: %s\n", ((struct user_data*)arg)->surname);
    printf("Age: %d\n", ((struct user_data*)arg)->age);
    printf("Phone: %s\n",((struct user_data*)arg)->phone);
    printf("Email: %s\n", ((struct user_data*)arg)->email);

    char usr_to_del[sizeof(struct user_data)] = "Doe";
    if (ioctl(fd, DEL_USER, usr_to_del) < 0) {
        perror("ioctl");
        return 1;
    }

    if(ioctl(fd, GET_USER, usr_to_del) == 0) {
        perror("not deleted!");
        return 1;
    }



    close(fd);
    return 0;
}
