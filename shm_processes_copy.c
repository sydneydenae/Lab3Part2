#include  <stdio.h>
#include  <stdlib.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>

void  ClientProcess(int []);
void  ParentProcess(int []);


int  main(int  argc, char *argv[])
{
     int    ShmID;
     int    *ShmPTR;
     pid_t  pid;
     int    status;
     srandom(3);

     ShmID = shmget(IPC_PRIVATE, 2*sizeof(int), IPC_CREAT | 0666);
     if (ShmID < 0) {
          printf("*** shmget error (server) ***\n");
          exit(1);
     }
     printf("Server has received a shared memory of two integers...\n");

     // Attach shared memory
    ShmPTR = (int *) shmat(ShmID, NULL, 0);
    if (ShmPTR == (int *) -1) {
        printf("*** shmat error (server) ***\n");
        exit(1);
    }
     printf("Server has attached the shared memory...\n");

     ShmPTR[0] = 0; // Bank Account.
     ShmPTR[1] = 0; // Turn.
     
     printf("Server has filled %d %d in shared memory...\n",
            ShmPTR[0], ShmPTR[1]);

     printf("Server is about to fork a child process...\n");
     pid = fork();
     if (pid < 0) {
          printf("*** fork error (server) ***\n");
          exit(1);
     }
     else if (pid == 0) {
          ClientProcess(ShmPTR);
          exit(0);
     } else {
        ParentProcess(ShmPTR);
        wait(&status);
        exit(0);
     }
}

void  ParentProcess(int  SharedMem[])
{
  int account;
  int balance;
  int randSleep;
  int i = 0;

  for (i = 1; i <= 25; i++){
     randSleep = random() % 5;
      sleep(randSleep);
      while(SharedMem[1] != 0){
        // Do nothing
        usleep(100);
      }
      account = SharedMem[0];
      // Deposit money
      if (account <= 100){
          balance = random() % 100;
          if(balance % 2 == 0){
          account += balance;
          printf("Dear old Dad: Deposits $%d / Balance = $%d\n", balance, account);
          } else {
          printf("Dear old Dad: Doesn't have any money to give\n");
          }
          SharedMem[0] = account;
      }
      SharedMem[1] = 1;
  }
}

void  ClientProcess(int  SharedMem[])
{
  int account;
  int balance;
  int randSleep;
  int i = 0;

  for (i = 1; i <= 25; i++){
      randSleep = random() % 5;
      sleep(randSleep);
      while(SharedMem[1] != 1){
          usleep(100);
      }
      account = SharedMem[0];
      balance = random() % 50;
      printf("Poor Student needs $%d\n", balance);

      // Withdraw
      if(balance <= account){
        account -= balance;
        printf("Poor Student: Withdraws $%d / Balance = $%d\n", balance, account);
      } else {
        printf("Poor Student:Not Enough Cash ($%d)\n", account);
      }

      SharedMem[0] = account;
      SharedMem[1] = 0; // Set turn for Parent

  }
}