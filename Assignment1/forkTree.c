#include <stdio.h>
#include <unistd.h> 
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
void delay(int number_of_seconds) 
{ 
    // Converting time into milli_seconds 
    int milli_seconds = 1000 * number_of_seconds; 
  
    // Stroing start time 
    clock_t start_time = clock(); 
  
    // looping till required time is not acheived 
    while (clock() < start_time + milli_seconds) 
        ; 
}  

int main(int argc, char* argv[]){
int height, children, pid, ppid;
int i;
height=atoi(argv[1]);
children=atoi(argv[2]);
pid=getpid();
ppid=getppid();

delay(10000);

 printf("(%d) : Process starting\n", pid);
printf("(%d) : Parent's ID is %d \n", pid, ppid);
printf("(%d) : Height in the tree %d \n", pid, height);

  if(height > 1 && children>0)
  {
 printf("(%d) : Creating %d child/children at height %d \n \n \n \n", pid, children, height-1);
     for(i=0;i<children;i++)
   {
     pid=fork();

     if(pid==-1)
     {
	fprintf(stderr, "Fork Failed\n");
 	exit(1);
     }

     if(pid==0)
     {
	sprintf(argv[1], "%d", --height);
	if(execlp(argv[0], argv[0], argv[1], argv[2], (char *) NULL) == -1)
	    fprintf(stderr, "Exec failed\n");
        exit(2);
     }
   }
  }
  else
  {
     printf("\n \n \n \n");
     exit(0);
  }

while(children>0 && height>0)
{
   wait(NULL);
   children--;
}
pid=getpid();

 printf("(%d) : Terminating at height %d \n \n \n", pid, height);
 exit(0);

return 0;
}


