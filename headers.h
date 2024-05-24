#include <stdio.h>

struct book {
    int id;
    char name[50];
    int qty;
};

struct cust_book {
    int cust_id;
    int book_id;
};

struct userdata {
    int custid; 
    char name[50];
    char password[50];
};
