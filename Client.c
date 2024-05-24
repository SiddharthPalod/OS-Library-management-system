#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "headers.h"
#include <fcntl.h>
#include <sys/stat.h>
// IMT2022002 SIDDHARTH PALOD
// RUN THIS MULTIPLE TIMES AFTER RUNNING SERVER.C

// display functions
void displayMenuUser(){
    printf("Menu to choose from\n");
    printf("a. See book collection\n");
    printf("b. Search for a book by name\n");
    printf("c. Rent a book\n");
    printf("d. Return a book\n");
    printf("e. Exit the menu\n");
    printf("Please enter your choice\n");
}

void displayMenuAdmin(){
    printf("Menu to choose from\n");
    printf("a. Add a book\n");
    printf("b. Delete a book\n");
    printf("c. Modify quantity of a book\n");
    printf("d. View collection\n");
    printf("e. View customers\n");
    // printf("f. Remove customers\n");
    printf("f. Search for a book by name\n");
    printf("g. Exit the menu\n");
    printf("Please enter your choice\n");
}

// print and list functions
void printBook(struct book book){
    if (book.id != -1 && book.qty > 0){
        printf("%d\t%s\t%d\n", book.id, book.name, book.qty);
    }
}

void getInventory(int sockfd){
    printf("Fetching book inventory\n");
    printf("ID\tName\tQuantity\n");
    while (1){
        struct book book;
        read(sockfd, &book, sizeof(struct book));
        if (book.id != -1){
            printBook(book);
        }else{
            break;
        }
    }
}

void getCustomers(int sockfd){
    printf("Fetching customers\n");
    printf("ID\tName\tPassword\n");
    while (1){
        struct userdata cust;
        int n = read(sockfd, &cust, sizeof(struct userdata));
        if (n == 0) {
            break;
        }
        if (cust.custid != -1){
            printf("%d\t%s\t%s\n", cust.custid, cust.name, cust.password);
        }
        else{
            break;
        }
    }
}

//input functions
int custIdTaker(){
    int custId = -1;
    while (1){
        printf("Enter customer id\n");
        scanf("%d", &custId);

        if (custId < 0){
            printf("Customer id can't be negative, try again\n");
        }else{
            break;
        }
    }
    return custId;
}

int prodIdTaker(){
    int prodId = -1;
    while (1){
        printf("Enter book id\n");
        scanf("%d", &prodId);

        if (prodId < 0){
            printf("book id can't be negative, try again\n");
        }else{
            break;
        }
    }
    return prodId;
}

int quantityTaker(){
    int qty = -1;
    while (1){
        printf("Enter quantity\n");
        scanf("%d", &qty);

        if (qty < 0){
            printf("Quantity can't be negative, try again\n");
        }else{
            break;
        }

    }
    return qty;
}

int main(){
    printf("Waiting for server to connect\n");
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == -1){
        return -1;
    }
    struct sockaddr_in serv;
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = INADDR_ANY;
    serv.sin_port = htons(8080);

    if (connect(sockfd, (struct sockaddr *)&serv, sizeof(serv)) == -1){
        perror("Error: ");
        return -1;
    }

    printf("Server connection established\n");
    printf("Press 1 to login as user or 2 to login as admin\n");
    
    int user;
    scanf("%d", &user);
    write(sockfd, &user, sizeof(user));
// Customer-----------------------------------------------
    if (user == 1){

        char username[50];
        char password[50];
        printf("Enter username: ");
        scanf("%s", username);
        printf("Enter password: ");
        scanf("%s", password);
        write(sockfd, &username, sizeof(username));
        write(sockfd, &password, sizeof(password));
        printf("Login request sent\n");
        char loginStatus = 'y';
        read(sockfd, &loginStatus, sizeof(char));
        if (loginStatus == 'y') {
            printf("Login successful\n");
        } else {
            char signStatus = 'y';
            printf("Login failed\n");
            printf("Do you want to sign up? y/n\n");
            scanf("%c", &signStatus);
            scanf("%c", &signStatus);
            write(sockfd, &signStatus, sizeof(char));
            if(signStatus != 'y'){
                printf("Signup not requested\n");
            }
            else{
                printf("Customer signed up please login again to confirm\n");
            }
            return 0; //After signing up you cant stay loginned in this is to make sure admin is always password protected and not everyone can access it
        }
        int cust_id;
        read(sockfd, &cust_id, sizeof(int));
        printf("Customer id: %d\n", cust_id);
        while (1){
            displayMenuUser();
            char ch;
            scanf("%c",&ch);
            scanf("%c",&ch);

            write(sockfd, &ch, sizeof(char));
// Customer menu ----------------------------------------------------------------------------------------
            if (ch == 'a'){
                getInventory(sockfd);
            }
            else if (ch == 'b'){
                char name[50];
                printf("Enter book name\n");
                scanf("%s", name);
                write(sockfd, &name, sizeof(name));
                struct book p;
                read(sockfd, &p, sizeof(struct book));
                printf("ID\tName\tQuantity\n");
                printBook(p);
            }
            else if (ch == 'c'){
                int book_id = prodIdTaker();
                printf("Book id: %d\n", book_id);
                printf("Customer id: %d\n", cust_id);
                write(sockfd,&book_id, sizeof(int));
                write(sockfd,&cust_id, sizeof(int));
                char response[80];
                read(sockfd, &response, sizeof(response));
                printf("%s\n", response);
            }
            else if (ch == 'd'){
                int book_id = prodIdTaker();
                printf("Book id: %d\n", book_id);
                printf("Customer id: %d\n", cust_id);
                write(sockfd,&book_id, sizeof(int));
                write(sockfd,&cust_id, sizeof(int));
                char response[80];
                read(sockfd, &response, sizeof(response));
                printf("%s\n", response);
            }
            else if (ch == 'e'){
                break;
            }
            else{
                printf("Invalid choice, try again\n");
            }            
        }
    }
//  admin----------------------------------------------- 
    else if (user == 2){
        char username[50];
        char password[50];
        printf("Enter username: ");
        scanf("%s", username);
        printf("Enter password: ");
        scanf("%s", password);
        write(sockfd, &username, sizeof(username));
        write(sockfd, &password, sizeof(password));
        printf("Login request sent\n");
        char loginStatus = 'y';
        read(sockfd, &loginStatus, sizeof(char));
        if (loginStatus == 'y') {
            printf("Login successful\n");
        } else {
            char signStatus = 'y';
            printf("Login failed\n");
            printf("Do you want to sign up? y/n\n");
            scanf("%c", &signStatus);
            scanf("%c", &signStatus);
            write(sockfd, &signStatus, sizeof(char));
            if(signStatus != 'y'){
                printf("Signup not requested\n");
            }
            else{
                printf("Admin signed up please login again to confirm\n");
            }
            return 0; //After signing up you cant stay loginned in this is to make sure admin is always password protected and not everyone can access it
        }
        
        while (1){
            displayMenuAdmin();
            char ch;
            scanf("%c",&ch);
            scanf("%c",&ch);
            write(sockfd, &ch, sizeof(ch));
// Admin menu ----------------------------------------------------------------------------------------
            if (ch == 'a'){
                //add a book
                int id, qty;
                char name[50];

                printf("Enter book name\n");
                scanf("%s", name);
                id = prodIdTaker();
                qty = quantityTaker();
                
                struct book p;
                p.id = id;
                strcpy(p.name, name);
                p.qty = qty;

                int n1 = write(sockfd, &p, sizeof(struct book));
                char response[80];
                read(sockfd, response, sizeof(response));
                printf("%s\n", response);
            }

            else if (ch == 'b'){
                // printf("Enter book id to be deleted\n");
                int bookid = prodIdTaker();
                write(sockfd, &bookid, sizeof(int));
                //deleting is equivalent to setting everything as -1
                char response[80];
                read(sockfd, response, sizeof(response));
                printf("%s\n", response);
            }

            else if (ch == 'c'){
                int id = prodIdTaker();
                int qty = quantityTaker();
                struct book p;
                p.id = id;
                p.qty = qty;
                write(sockfd, &p, sizeof(struct book));
                char response[80];
                read(sockfd, response, sizeof(response));
                printf("%s\n", response);
            }

            else if (ch == 'd'){
                getInventory(sockfd);
            }
            else if (ch == 'e'){
                getCustomers(sockfd);
            }
            // else if(ch == 'f'){
            //     //remove customers
            //     int id = custIdTaker();
            //     write(sockfd, &id, sizeof(int));
            //     char response[80];
            //     read(sockfd, response, sizeof(response));
            //     printf("%s\n", response);
            // }
            else if (ch == 'f'){
                char name[50];
                printf("Enter book name\n");
                scanf("%s", name);
                write(sockfd, &name, sizeof(name));
                struct book p;
                read(sockfd, &p, sizeof(struct book));
                printf("ID\tName\tQuantity\n");
                printBook(p);
            }
            else if (ch == 'g'){
                break;
            }
            else{
                printf("Invalid choice, try again\n");
            }
        }
    }

    printf("Exiting program\n");
    close(sockfd);
    return 0;
}
