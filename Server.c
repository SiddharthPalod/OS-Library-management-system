#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include "headers.h"
// IMT2022002 SIDDHARTH PALOD
// RUN THIS FIRST
// locks-------------------------------------------------------------------------------------

void setLockCust(int fd_custs, struct flock lock_cust){

    lock_cust.l_len = 0;
    lock_cust.l_type = F_RDLCK;
    lock_cust.l_start = 0;
    lock_cust.l_whence = SEEK_SET;
    fcntl(fd_custs, F_SETLKW, &lock_cust);

    return ;
}

void unlock(int fd, struct flock lock){
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLKW, &lock);
}

void bookReadLock(int fd, struct flock lock){
    lock.l_len = 0;
    lock.l_type = F_RDLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    fcntl(fd, F_SETLKW, &lock);
}

void bookWriteLock(int fd, struct flock lock){
    lseek(fd, (-1)*sizeof(struct book), SEEK_CUR);
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_CUR;
    lock.l_start = 0;
    lock.l_len = sizeof(struct book);

    fcntl(fd, F_SETLKW, &lock);
}

// books-------------------------------------------------------------------------------------

void addBooks(int fd, int new_fd){
    char name[50];
    int id, qty;
    struct book p1;
    read(new_fd, &p1, sizeof(struct book));

    strcpy(name, p1.name);
    id = p1.id;
    qty = p1.qty;

    struct flock lock;
    bookReadLock(fd, lock);

    struct book p;

    int flg = 0;
    while (read(fd, &p, sizeof(struct book))){
        if (p.id == id && p.qty > 0){
            write(new_fd, "Duplicate book\n", sizeof("Duplicate book\n"));
            unlock(fd, lock);
            flg = 1;
            return;
        }
    }

    if (!flg){
        lseek(fd, 0, SEEK_END);
        p.id = id;
        strcpy(p.name, name);
        p.qty = qty;

        write(fd, &p, sizeof(struct book));
        write(new_fd, "Added successfully\n", sizeof("Added successfully\n"));
        unlock(fd, lock);   
    }
}

void listBooks(int fd, int new_fd){
    struct flock lock;
    bookReadLock(fd, lock);
    struct book p;
    while (read(fd, &p, sizeof(struct book))){
        if (p.id != -1){
            write(new_fd, &p, sizeof(struct book));
        }
    }
    unlock(fd, lock);
    p.id = -1;
    write(new_fd, &p, sizeof(struct book));
}

void deleteBook(int fd, int new_fd, int id) {
    struct flock lock;
    bookReadLock(fd, lock);
    struct book p;
    int flg = 0;
    while (read(fd, &p, sizeof(struct book))) {
        if (p.id == id) {
            unlock(fd, lock);
            bookWriteLock(fd, lock);
            p.id = -1;
            strcpy(p.name, "");
            p.qty = -1;
            write(fd, &p, sizeof(struct book));
            write(new_fd, "Delete successful", sizeof("Delete successful"));
            unlock(fd, lock);
            flg = 1;
            return;
        }
    }
    if (!flg) {
        write(new_fd, "book id invalid", sizeof("book id invalid"));
        unlock(fd, lock);
    }
}

void updateBook(int fd, int new_fd){
    int id;
    int val = -1;
    struct book p1;
    read(new_fd, &p1, sizeof(struct book));
    id = p1.id;
    val = p1.qty;
    struct flock lock;
    bookReadLock(fd, lock);
    int flg = 0;
    struct book p;
    lseek(fd, 0, SEEK_SET);
    while (read(fd, &p, sizeof(struct book))){
        if (p.id == id){
            unlock(fd, lock);
            bookWriteLock(fd, lock);
            p.qty = val;
            // printf("P.qty: %d , P.id: %d\n", p.qty, p.id);
            write(fd, &p, sizeof(struct book));
            flg = 1;
            unlock(fd, lock);
            break;
        }
    }
    unlock(fd, lock);
    if (!flg){
        write(new_fd, "book id invalid", sizeof("book id invalid"));
    }
    else{
        write(new_fd, "Update successful", sizeof("Update successful"));
    }
}

void searchBook(int fd, int new_fd){
    struct flock lock;
    bookReadLock(fd, lock);
    lseek(fd, 0, SEEK_SET);
    struct book p;
    int found = 0;
    char name[50];
    read(new_fd, name, sizeof(name));
    while (read(fd, &p, sizeof(struct book))){
        if (strcmp(p.name, name) == 0){
            if (p.id != -1) {
                write(new_fd, &p, sizeof(struct book));
                found = 1;
                break;
            }
        }
    }
    if (!found){
        p.id = -1;
        write(new_fd, &p, sizeof(struct book));
    }
    unlock(fd, lock);
}

void rentBook(int fd_cust_book, int fd, int new_fd) {
    int book_id, cust_id;
    read(new_fd, &book_id, sizeof(int));
    read(new_fd, &cust_id, sizeof(int));
    struct flock lock;
    bookReadLock(fd, lock);
    int found = 0;
    struct book p;
    lseek(fd, 0, SEEK_SET);
    while (read(fd, &p, sizeof(struct book))) {
        if (p.id == book_id && p.qty > 0) {
            unlock(fd, lock);
            found = 1;
            bookWriteLock(fd, lock);
            p.qty--;
            write(fd, &p, sizeof(struct book));
            unlock(fd, lock);
            break;
        }
    }
    unlock(fd, lock);
    if (found) {
        bookWriteLock(fd_cust_book, lock);
        struct cust_book cb;
        cb.cust_id = cust_id;
        cb.book_id = book_id;
        lseek(fd_cust_book, 0, SEEK_END);
        write(fd_cust_book, &cb, sizeof(struct cust_book));
        write(new_fd, "Book rented successfully\n", sizeof("Book rented successfully\n"));

    } else {
        write(new_fd, "Book not available\n", sizeof("Book not available\n"));
    }
}

void returnBook(int fd_cust_book, int fd, int new_fd) {
    struct flock lock_cust_book;
    bookReadLock(fd_cust_book, lock_cust_book);
    int book_id, cust_id;
    read(new_fd, &book_id, sizeof(int));
    read(new_fd, &cust_id, sizeof(int));
    struct cust_book cb;
    lseek(fd_cust_book, 0, SEEK_SET);
    int found = 0;
    while (read(fd_cust_book, &cb, sizeof(struct cust_book))) {
        if (cb.cust_id == cust_id && cb.book_id == book_id) {
            found = 1;
            break;
        }
    }
    unlock(fd_cust_book, lock_cust_book);
    if (found) {
        struct flock lock;
        bookReadLock(fd, lock);
        struct book p;
        found = 0;
        lseek(fd, 0, SEEK_SET);
        while (read(fd, &p, sizeof(struct book))) {
            if (p.id == book_id) {
                found = 1;
                unlock(fd, lock);
                bookWriteLock(fd, lock);
                p.qty++;
                write(fd, &p, sizeof(struct book));
                break;
            }
        }
        unlock(fd, lock);
        if (found) {
            struct flock lock_cust_book;
            bookWriteLock(fd_cust_book, lock_cust_book);
            lseek(fd_cust_book, (-1)*sizeof(struct cust_book), SEEK_CUR);
            cb.cust_id = -1;
            cb.book_id = -1;
            write(fd_cust_book, &cb, sizeof(struct cust_book));
            unlock(fd_cust_book, lock_cust_book);
            write(new_fd, "Book returned successfully\n", sizeof("Book returned successfully\n"));
        } else {
            write(new_fd, "Book not found\n", sizeof("Book not found\n"));

        }
    } else {
        write(new_fd, "Book not rented by customer\n", sizeof("Book not rented by customer\n"));
    }
}

// void readCustBooks(int fd_cust_book) {
//     struct flock lock_cust_book;
//     bookReadLock(fd_cust_book, lock_cust_book);
//     struct cust_book cb;
//     lseek(fd_cust_book, 0, SEEK_SET);
//     while (read(fd_cust_book, &cb, sizeof(struct cust_book))) {
//         printf("Customer id: %d, Book id: %d\n", cb.cust_id, cb.book_id);
//     }
//     unlock(fd_cust_book, lock_cust_book);
// }


// users-------------------------------------------------------------------------------------

void addUser(int fd_ads, int new_fd, char* username, char* password, int isAdmin){
    struct flock lock;
    setLockCust(fd_ads, lock);  // Assuming this function sets an appropriate write lock
    int max_id = -1, max_id2 = -1;
    struct userdata id,temp;

    lseek(fd_ads, 0, SEEK_SET);
    // Increment the custid based on the user type
    if(isAdmin != 1){
        while (read(fd_ads, &id, sizeof(struct userdata))){
            if (id.custid > max_id){
            max_id = id.custid;
            }
        }
        max_id++;
        temp.custid = max_id;
    } else {
        while (read(fd_ads, &id, sizeof(struct userdata))){
            if (id.custid > max_id2){
            max_id2 = id.custid;
            }
        }
        max_id2++;
        temp.custid = max_id2;
    }

    // Set the username and password
    strcpy(temp.name, username);
    strcpy(temp.password, password);

    // Move the file pointer to the end to append the new user
    lseek(fd_ads, 0, SEEK_END);
    write(fd_ads, &temp, sizeof(struct userdata));


    printf("User added with id %d \n", temp.custid);
    unlock(fd_ads, lock);

    // Send the new custid back to the client
    if(isAdmin != 1){
        write(new_fd, &max_id, sizeof(int));
    } else {
        write(new_fd, &max_id2, sizeof(int));
    }
}

void listCustomers(int fd_custs, int new_fd){
    struct flock lock_cust;
    setLockCust(fd_custs, lock_cust);
    struct userdata id;
    while (read(fd_custs, &id, sizeof(struct userdata))){
        if (id.custid != -1) {
            write(new_fd, &id, sizeof(struct userdata));
        }
    }
    id.custid = -1;
    write(new_fd, &id, sizeof(struct userdata));
    unlock(fd_custs, lock_cust);
}

int getCustomerId(int fd_custs, char* username, char* password) {
    struct flock lock_cust;
    setLockCust(fd_custs, lock_cust);
    struct userdata id;
    lseek(fd_custs, 0, SEEK_SET);
    while (read(fd_custs, &id, sizeof(struct userdata))) {
        if (strcmp(id.name, username) == 0 && strcmp(id.password, password) == 0) {
            unlock(fd_custs, lock_cust);
            return id.custid;
        }
    }
    unlock(fd_custs, lock_cust);
    return -1;
}

// void deleteCustomer(int fd_custs, int new_fd, int id){
//     struct flock lock;
//     setLockCust(fd_custs, lock);  // Assuming this function sets an appropriate read lock
//     struct userdata customer;
//     customer.custid = -1;
//     strcpy(customer.name, "");
//     strcpy(customer.password, "");
//     int flg= 0;
//     lseek(fd_custs, 0, SEEK_SET);
//     while (read(fd_custs, &customer, sizeof(struct userdata))) {
//         if (customer.custid == id) {
//             lseek(fd_custs, -(sizeof(customer)-sizeof(customer.password)), SEEK_CUR);
//             unlock(fd_custs, lock);
//             bookWriteLock(fd_custs, lock); 
//             struct userdata temp;
//             temp.custid = -1;
//             strcpy(temp.name, "");
//             strcpy(temp.password, ""); 
//             write(fd_custs, &temp, sizeof(struct userdata)); 
//             write(new_fd, "Customer removed successfully", sizeof("Customer removed successfully"));
//             unlock(fd_custs, lock);
//             flg =1;
//             return;
//         }
//     }
//     if(!flg){
//         write(new_fd, "Customer not found", sizeof("Customer not found"));
//         unlock(fd_custs, lock);
//     }
// }

int main(){

// initialse-------------------------------------------------------------------------------------
    printf("Setting up server\n");
    //file containing all the records is called records.txt
    int fd = open("records.txt", O_RDWR | O_CREAT, 0777); //book records
    int fd_custs = open("customers.txt", O_RDWR | O_CREAT, 0777); //customer records
    int fd_ads = open("admins.txt", O_RDWR | O_CREAT, 0777); //admin records
    int fd_cust_book = open("cust_books.txt", O_RDWR | O_CREAT, 0777); 

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1){
        perror("Error: ");
        return -1;
    }

    struct sockaddr_in serv, cli;
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = INADDR_ANY;
    serv.sin_port = htons(8080);

    int opt = 1;
    if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))){
        perror("Error: ");
        return -1;
    }
    if (bind(sockfd, (struct sockaddr *)&serv, sizeof(serv)) == -1){
        perror("Error: ");
        return -1;
    }
    if (listen(sockfd, 5) == -1){
        perror("Error: ");
        return -1;
    }

    int size = sizeof(cli);
    printf("Server set up successfully\n");
// -------------------------------------------------------------------------------------
    while (1){

        int new_fd = accept(sockfd, (struct sockaddr *)&cli, &size);
        if (new_fd == -1){
            perror("Error: ");
            return -1;
        }

        if (!fork()){
            printf("Connection with client successful\n");
            close(sockfd);

            int user;
            read(new_fd, &user, sizeof(int));
// Customer----------------------------------------------------------------------------------------------
            if (user == 1){
                char username[50];
                char password[50];
                read(new_fd, &username, sizeof(username));
                read(new_fd, &password, sizeof(password));
                int cust_id = getCustomerId(fd_custs, username, password);

                char loginStatus = 'y';
                if (cust_id == -1) {
                    loginStatus = 'n';
                    write(new_fd, &loginStatus, sizeof(char));
                    char signStatus;
                    read(new_fd, &signStatus, sizeof(char));
                    printf("Sign Status: %c\n", signStatus);
                    if(signStatus == 'y'){
                        printf("Customer added\n");
                        addUser(fd_custs,new_fd, username, password,0);
                    }
                    else
                        return 0;
                }
                else {
                    write(new_fd, &loginStatus, sizeof(char));
                }
                write(new_fd, &cust_id, sizeof(int));

                char ch;
                while (1){
                    read(new_fd, &ch, sizeof(char));

                    lseek(fd, 0, SEEK_SET);
                    lseek(fd_custs, 0, SEEK_SET);
// Customer options-------------------------------------------------------------------------------------
                    if (ch == 'a'){
                        listBooks(fd, new_fd);
                    }
                    else if (ch == 'b'){
                        searchBook(fd, new_fd);
                    }
                    else if (ch == 'c'){
                        rentBook(fd_cust_book, fd, new_fd);
                    }
                    else if (ch == 'd'){
                        returnBook(fd_cust_book, fd, new_fd);
                    }
                    else if (ch == 'e'){
                        close(new_fd);
                        break;
                    }
                }
                printf("Connection terminated\n");

            }
// Admin----------------------------------------------------------------------------------------------
            else if (user == 2){
                char username[50];
                char password[50];
                read(new_fd, &username, sizeof(username));
                read(new_fd, &password, sizeof(password));
                int admin_id = getCustomerId(fd_ads, username, password);
                char loginStatus = 'y';
                if (admin_id == -1) {
                    loginStatus = 'n';
                    write(new_fd, &loginStatus, sizeof(char));
                    char signStatus;
                    read(new_fd, &signStatus, sizeof(char));
                    printf("Sign Status: %c\n", signStatus);
                    if(signStatus == 'y'){
                        addUser(fd_ads, new_fd, username, password,1);
                        printf("Admin added\n");
                    }
                    else
                        return 0;
                }
                else {
                    write(new_fd, &loginStatus, sizeof(char));
                }
                char ch;
                while (1){
                    read(new_fd, &ch, sizeof(ch));

                    lseek(fd, 0, SEEK_SET);
                    lseek(fd_ads, 0, SEEK_SET);
                    lseek(fd_custs, 0, SEEK_SET);
// admin options-------------------------------------------------------------------------------------
                    if (ch == 'a'){
                        addBooks(fd, new_fd);
                    } 
                    else if (ch == 'b'){
                        int bookid;
                        read(new_fd, &bookid, sizeof(int));
                        deleteBook(fd, new_fd, bookid);
                    }
                    else if (ch == 'c'){
                        updateBook(fd, new_fd);
                    }

                    else if (ch == 'd'){
                        listBooks(fd, new_fd);
                    }
                    else if (ch == 'e'){
                        listCustomers(fd_custs, new_fd);
                    }
                    // else if (ch == 'f'){
                    //     int custid;
                    //     read(new_fd, &custid, sizeof(int));
                    //     deleteCustomer(fd_custs, new_fd, custid);
                    //     printf("-------------------------\n");
                    //     readFD_CUSTSfile(fd_custs);
                    // }
                    else if (ch == 'f'){
                        searchBook(fd, new_fd);
                    }
                    else if (ch == 'g'){
                        close(new_fd);
                        break;
                    }
                    else{
                        continue;
                    }
                }
            }
            printf("Connection terminated\n");
        }else{
            close(new_fd);
        }
    }
}