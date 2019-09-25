 #include <unistd.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <sys/wait.h>
 #include <string.h>
 #include <fcntl.h>

int pid;
int fds[100][2];
int isPipe;
int noOfPipeCommands;

 struct Command
 {
   char* commandName;
   char *arguments[100];
 };

 struct Command command;

void signalHandler(int signal)
{
  int pid;
  pid = wait(NULL);
}

void executeChild(int pipeNumber)
{
  if(command.arguments[1]!=NULL)
  {
    if((strcmp(command.arguments[1], "<")==0 || strcmp(command.arguments[1], ">")==0))
    {
      if(command.arguments[2]==NULL)
      {
        return;
      }
      pid = fork();
      if(pid == 0)
      {

        int fd;

        if((strcmp(command.arguments[1], "<")==0))
        {
          fd = open(command.arguments[2], O_RDONLY);
          if(fd<-1)
          {
            exit(2);
          }
          //close(0);
          dup2(fd,0);
          close(fd);

        }
        else if(strcmp(command.arguments[1], ">")==0)
        {
          fd = open(command.arguments[2], O_WRONLY | O_CREAT, "w");
          if(fd<-1)
          {
            exit(2);
          }
          //close(1);
	  //close(2);
          dup2(fd,1);
          close(fd);
        }

        if(execvp(command.commandName,command.arguments)==-1)
        {
          fprintf(stderr, "Exec failed\n");
        }
        exit(2);
      }
      else if(pid==-1)
      {
        fprintf(stderr, "Forking failed\n");
        return;
      }
      else
      {
        return;
      }
    }
    }

    pid = fork();
    if(pid == 0)
    {
      if(isPipe == 1)
      {
        if(pipeNumber == 0)
        {
          close(1);
          dup2(fds[pipeNumber][1], 1);
          close(fds[pipeNumber][0]);
        }
        else
        {
          close(0);
          dup2(fds[pipeNumber-1][0],0);
          close(fds[pipeNumber-1][1]);

          if((pipeNumber+1) < noOfPipeCommands)
          {
            close(1);
            dup2(fds[pipeNumber][1], 1);
            close(fds[pipeNumber][0]);
          }
        }

      }
      if(execvp(command.commandName,command.arguments)==-1)
      {
        fprintf(stderr, "Exec failed\n");
      }
      exit(2);
    }
    else if(pid==-1)
    {
      fprintf(stderr, "Forking failed\n");
      return;
    }

    if((pipeNumber > 0) && isPipe == 1)
    {
      close(fds[pipeNumber - 1][0]);
      close(fds[pipeNumber - 1][1]);
    }
    return;

}

int executeCommand(int isForeground, int pipeNumber)
{
  int i=0;
  char commandName[100];
  char arguments[100][100];
  if(strcmp(command.commandName,"exit")==0)
   return 1;
  strcpy(commandName, command.commandName);
  executeChild(pipeNumber);
  if(isForeground==1)
  {
    wait(NULL);
    free(command.commandName);
    i=0;
    while(command.arguments[i]!=NULL)
    {
     free(command.arguments[i]);
     i++;
    }
  }
  else if(isForeground==0)
   printf("\n Process %d in background mode\n", pid);

  return 0;
}

int getCommand(char* commandLine)
{
  int isForeground, hasPipe=0;
  int i = 0;
  char* token;

  token = strtok(commandLine, " ");
  if(strcmp(token,"\n") == 0)
  {
    return 2;
  }
  else
  {
    command.commandName=(char *) malloc(100);
    command.arguments[i]=(char *) malloc(100);
    strcpy(command.commandName, token);
    strcpy(command.arguments[i], command.commandName);
    i++;
    token = strtok(NULL, " ");
  }


  while (token != NULL)
  {
    command.arguments[i]=(char *) malloc(100);
    strcpy(command.arguments[i], token);
    token = strtok(NULL, " ");
    i++;
  }

  char* stringWithNewlineChar = (char *) malloc(100);

  if(i>1)
  {
    i--;
    strcpy(stringWithNewlineChar, command.arguments[i]);
    command.arguments[i] = strtok(stringWithNewlineChar, "\n");
    if(strcmp(command.arguments[i], "&") == 0)
     isForeground=0;
    else
     isForeground=1;
    command.arguments[++i]=NULL;
  }
  else
  {
    strcpy(stringWithNewlineChar, command.commandName);
    command.commandName = strtok(stringWithNewlineChar, "\n");
    command.arguments[1]=NULL;
    isForeground=1;
  }
  return isForeground;
}

 void printPromt()
 {
  printf("\nminish> ");
 }

 int main(int argc, char* argv[])
 {
   int isForeground;
   int i=0;
   isPipe = 0;
   noOfPipeCommands = 0;
   signal(SIGCHLD, signalHandler);
   char commandLine[100], fullCommandLine[100];


   while(1==1)
   {
     printPromt();

     fgets(commandLine,100,stdin);
     strcpy(fullCommandLine,commandLine);
     char* filter;
     filter=strtok(commandLine,"|");
     while(filter !=NULL)
     {
       filter = strtok(NULL, "|");
       if(noOfPipeCommands >= 1)
       {
         if (pipe(fds[noOfPipeCommands - 1]) == -1) {
                 perror("pipe");
                 exit(1);
         }
       }
       noOfPipeCommands++;
     }
     if(noOfPipeCommands >= 2)
     {
       isPipe = 1;
     }
     strcpy(commandLine, fullCommandLine);
     strtok(commandLine, "|");

     while(i < noOfPipeCommands)
     {
       if(i==0)
       {
              filter = strtok(fullCommandLine, "|");

       }
       filter = strtok(NULL, "|");
       isForeground=getCommand(commandLine);
       if(executeCommand(isForeground, i) == 0)
       {
         if(filter != NULL)
         {
           strcpy(commandLine, filter);
         }
         i++;
         continue;
       }
      else
      {
        return 0;
      }


     }
     isPipe = 0;
     noOfPipeCommands = 0;
     i=0;

   }
  return 0;
 }
