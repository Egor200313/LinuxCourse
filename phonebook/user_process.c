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
        exit(1);
    }

    struct user_data user;
    strcpy(user.name, "John");
    strcpy(user.surname, "Doe");
    user.age = 30;
    strcpy(user.phone, "1234567890");
    strcpy(user.email, "john.doe@example.com");
    
    
    /*
    if (ioctl(fd, ADD_USER, &user) < 0) {
        perror("ioctl");
        exit(1);
    }*/
    
    
    char surname[20] = "Doe";
    if (ioctl(fd, GET_USER, (unsigned long)surname) < 0) {
        perror("ioctl");
        exit(1);
    }
    /*
    printf("Name: %s\n", user.name);
    printf("Surname: %s\n", user.surname);
    printf("Age: %d\n", user.age);
    printf("Phone: %s\n", user.phone);
    printf("Email: %s\n", user.email);

    if (ioctl(fd, DEL_USER, surname) < 0) {
        perror("ioctl");
        exit(1);
    }*/

    close(fd);
    return 0;
}
