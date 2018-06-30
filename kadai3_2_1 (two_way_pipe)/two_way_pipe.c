/*
 * セグメントで制御していないバージョンで完成している．
 * 
 *
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <unistd.h>
#include <sys/wait.h>
#define BUFSIZE 256

int main(int argc, char *argv[]) {
  char pbuf[BUFSIZE], cbuf[BUFSIZE];  //pbufは親プロセスがreadするときのbuf, cbufは子プロセスがreadするときのbuf
  int fd[2], fd2[2];  //[0]はread専用, [1]はwrite専用
  int pid, msg1len, msg2len, status;  //msg1lenは子プロセスから親プロセスに送られたメッセージの長さ, msg2lenは親プロセスから子プロセスに送られたメッセージの長さ

  if (argc != 3) {
    printf("Usage : %s msgtoparent msgtochild\n", argv[0]);
    exit(1);
  }

  if (pipe(fd) == -1) {
    perror("pipe failed.");
    exit(1);
  }

  if (pipe(fd2) == -1) {
    perror("pipe failed.");
    exit(1);
  }

  if ((pid=fork())== -1) {
    perror("fork failed.");
    exit(1);
  }

  if (pid == 0) { /* Child process */
    close(fd[0]);
    close(fd2[1]);
    msg2len = strlen(argv[1]) + 1;
    if (write(fd[1], argv[1], msg2len) == -1) {
      perror("pipe write.");
      exit(1);
    }
    if (read(fd2[0], cbuf, BUFSIZE) == -1) {
      perror("pipe read.");
      exit(1);
    }
    printf("Message from parent process: \n");
    printf("\t%s\n",cbuf);
    exit(0);
  }
  else { /* Parent process */
    close(fd[1]);
    close(fd2[0]);
    msg1len = strlen(argv[2]) + 1;
    if (read(fd[0], pbuf, BUFSIZE) == -1) {
      perror("pipe read.");
      exit(1);
    }
    if (write(fd2[1], argv[2], msg1len) == -1) {
      perror("pipe write.");
      exit(1);
    }
    printf("Message from child process: \n");
    printf("\t%s\n",pbuf);
    wait(&status);
  }
}
