**CS342- Operating Systems Lab Mini Project Report  ![](Aspose.Words.aa01c61e-10e0-44fa-b4c0-f470667d394e.001.png)**

**Title: Design and Implementation of an Online Library Management System** 

**IMT2022002 – SiddharthPalod**

**Table of Contents** 

1. Introduction ![](Aspose.Words.aa01c61e-10e0-44fa-b4c0-f470667d394e.002.png)
1. Project Overview 
1. How to run 
1. Server Implementation 
   1. File Handling and Locking Mechanisms ![](Aspose.Words.aa01c61e-10e0-44fa-b4c0-f470667d394e.003.png)
   1. Functional Modules 
1. Client Implementation ![](Aspose.Words.aa01c61e-10e0-44fa-b4c0-f470667d394e.004.png)
   1. User Interaction and Commands ![](Aspose.Words.aa01c61e-10e0-44fa-b4c0-f470667d394e.005.png)
1. Conclusion ![](Aspose.Words.aa01c61e-10e0-44fa-b4c0-f470667d394e.006.png)
1. Future Enhancements 



|**1. Introduction** |
| - |
|This project report documents the development of a client-server library management system using socket |
|programming in C. The system allows users to view, search, rent, and return books, while administrators can |
|manage the inventory and customer database. The server handles multiple clients concurrently, ensuring data |
|consistency using file locking mechanisms. |
|**2. Project Overview** |
|The project is divided into two main components: the server and the client. The server manages the library's |
|inventory and customer records, while the client provides an interface for users and administrators to interact |
|with the system. Communication between the server and clients is handled via TCP sockets. |
|**3. How to run** |
|Compiling the codes:  |
|gcc -o c Client.c  gcc -o s Server.c |
|Executing the codes:  ./s starts the server   ./c starts the client interface |
|Some helpful data included in the zip: premade Admin : username: admin , password; admin |
|Premade Customer: username: user1 password: user1 |
|Preinstalled books: to test renting and returning quickly |
|Ofcourse you can make your own admin,customer,books and try it out it would work perfectly |
|Note: As admin is made password protected after creating a new admin restart the server so that server records |
|and gives admin all the authority  |
|Note: All password protected signups need to be again log in to showcase person is original of course this |
|feature can be removed from the code based on how the code is converted into a application for future purposes |

4. **Server Implementation ![](Aspose.Words.aa01c61e-10e0-44fa-b4c0-f470667d394e.007.png)**

**File Handling and Locking Mechanisms** 

The server uses multiple files to store data: 

- **records.txt**: Stores book information. ![](Aspose.Words.aa01c61e-10e0-44fa-b4c0-f470667d394e.008.png)
- **customers.txt**: Stores customer information. 
- **cust\_books.txt**: Stores records of rented books. 
- **admins.txt**: Stores administrator information. 

To ensure data consistency and prevent race conditions, the server employs file locking mechanisms using **fcntl**. ![](Aspose.Words.aa01c61e-10e0-44fa-b4c0-f470667d394e.009.png)The locking strategies used include: 

- **Read Locks**: For read-only access to ensure data consistency. ![](Aspose.Words.aa01c61e-10e0-44fa-b4c0-f470667d394e.010.png)
- **Write Locks**: For modifying data to prevent concurrent writes. 



|**Functional Modules** |||||||||||||||||||||||||||||||||||||||||
| - | :- | :- | :- | :- | :- | :- | :- | :- | :- | :- | :- | :- | :- | :- | :- | :- | :- | :- | :- | :- | :- | :- | :- | :- | :- | :- | :- | :- | :- | :- | :- | :- | :- | :- | :- | :- | :- | :- | :- | :- |
|**Lock Management Functions** |||||||||||||||||||||||||||||||||||||||||
|void|` `setLockCust int  struct  | |void|` `unlock int  struct  | |void|` `bookReadLock int  struct  | |void||||||||||||||||||||||||||||||||
|bookWriteLock int  struct  | ||||||||||||||||||||||||||||||||||||||||
|These functions manage file locks, ensuring data integrity during concurrent access. |||||||||||||||||||||||||||||||||||||||||
|**Book Management Functions** |||||||||||||||||||||||||||||||||||||||||
|void|` `addBooks int  int | |void|` `listBooks int  int | |void|` `deleteBook int  int  int | |void|` `updateBook int  int|||||||||||||||||||||||||||||||
|| |void|` `searchBook int  int | |||||||||||||||||||||||||||||||||||||
|These functions handle the addition, listing, deletion, updating, and searching of books in the inventory. |||||||||||||||||||||||||||||||||||||||||
|**Customer Management Functions** |||||||||||||||||||||||||||||||||||||||||
|void|` `listCustomers int  int | |void|` `addUser int  int  char  char  int | |int|||||||||||||||||||||||||||||||||||
|getCustomerId int  char  char | |void|` `removeCustomers int  int | |||||||||||||||||||||||||||||||||||||
|These functions manage customer records, including adding new users, listing existing users, and removing users. |||||||||||||||||||||||||||||||||||||||||
|**Transaction Management Functions** |||||||||||||||||||||||||||||||||||||||||
|void|` `rentBook int  int  int | |void|` `returnBook int  int  int | ||||||||||||||||||||||||||||||||||||
|These functions handle book rental and return transactions, ensuring proper updates to both book and customer |||||||||||||||||||||||||||||||||||||||||
|records. |||||||||||||||||||||||||||||||||||||||||
|**Main Server Loop** |||||||||||||||||||||||||||||||||||||||||

The main server loop waits for client connections, forks a new process for each connection, and handles client ![](Aspose.Words.aa01c61e-10e0-44fa-b4c0-f470667d394e.011.png)requests based on user type (customer or administrator). 

int  main          

5. **Client Implementation** 

The client provides a user interface for both regular users and administrators to interact with the system. It supports various commands and sends requests to the server over a TCP connection. 

**User Interaction and Commands** 

**User Menu** 

void  displayMenuUser 

Displays options for users to view the book collection, search for books, rent books, and return books. **Admin Menu** 

void  displayMenuAdmin 

Displays options for administrators to add, delete, and update books, as well as manage customer records. **Helper Functions** 

void  printBook struct    void  getInventory int   void  getCustomers int   int  custIdTaker  int  prodIdTaker  int quantityTaker 

These functions assist in reading user input, displaying book information, and fetching inventory and customer data from the server. 

**Main Client Logic** 

The client connects to the server, prompts the user for login details, and provides the appropriate menu based on the user type. 

int  main         

6. **Conclusion** 

The library management system successfully implements a multi-client server architecture using socket programming and file locking mechanisms. It provides robust functionalities for managing book inventory and customer records, ensuring data consistency and integrity. 
