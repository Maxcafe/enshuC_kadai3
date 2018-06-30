#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <sys/wait.h>

#define NUMPROCS 4

char filename[]="counter";

int cnt1() {
  FILE *ct;
  int cnt;

  if((ct=fopen(filename, "r"))==NULL) exit(1);
  fscanf(ct, "%d\n", &cnt);
  cnt++;
  fclose(ct);

  if((ct=fopen(filename, "w"))==NULL) exit(1);
  fprintf(ct, "%d\n", cnt);
  fclose(ct);

  return cnt;
}

int main() {
  int i, cnt = 0, pid, status, sid;
  FILE *ct;
  key_t key;
  struct sembuf sb;

  setbuf(stdout, NULL); /* set stdout to be unbufferd */
  cnt = 0;

  /*
   *  鍵を生成
   */
  if ((key = ftok(".", 1)) == -1){
    fprintf(stderr,"ftok path does not exist.\n");
    exit(1);
  }

  /*
   *  セマフォを獲得
   */
  if ((sid=semget(key, 1, 0666 | IPC_CREAT)) == -1) {
    perror("semget error.");
    exit(1);
  }

  /*
   *  counterファイルを開いて，0を入力後，ファイルを閉じる
   */
  if ((ct=fopen(filename, "w"))==NULL) exit(1);
  fprintf(ct, "%d\n", cnt);
  fclose(ct);

  sb.sem_num = 0;
  sb.sem_op = -1;
  sb.sem_flg = 0;
  semctl(sid, 0, SETVAL, 1);

  /*
   *  4つの子プロセスを生成する
   */
  for (i=0; i<NUMPROCS; i++) {
    if ((pid=fork())== -1) {
      perror("fork failed.");
      exit(1);
    }



    if (pid == 0) { /* Child process */

      if(semop(sid, &sb, 1) == -1) {
        perror("Child_process:semop:");
        exit(1);
      }
      sb.sem_op = 1;

      cnt = cnt1();
      printf("count = %d\n", cnt);

      if(semop(sid, &sb, 1) == -1) {
        perror("Child_process:semop:");
        exit(1);
      }

      exit(0);
    }
  }

  for (i=0; i<NUMPROCS; i++) {
    wait(&status);
  }
  semctl(sid, 0, IPC_RMID);

  exit(0);
}
