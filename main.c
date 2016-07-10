/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/07/2016 14:38:51
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dragan Glumac (DG), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define TRUE 1
#define FILE_PATH "nastybob.txt"

int checklocks(int fd) {
  struct flock lock;
  lock.l_type = F_RDLCK;
  int retval;
  retval = fcntl(fd, F_GETLK, &lock);
  if (retval == -1)
    perror("lock: ");
  else
    printf("(l_type=%d,l_whence=%d,l_start=%lld,l_len=%lld,l_pid=%d)\n", lock.l_type, lock.l_whence, lock.l_start, lock.l_len, lock.l_pid);
  return retval;
}
void read_via_fds() {
  int fd = open(FILE_PATH, O_RDONLY);
  if (fd < 0)
    perror("read: ");
  printf("read fileno: %d\n", fd); 

  char buffy[1024];
  long last_read = 0;
  while(TRUE) {
    //    checklocks(fd);
    lseek(fd, last_read, SEEK_SET);
    size_t bytesread = read(fd, buffy, 1024 - 1);
    last_read = lseek(fd, 0L, SEEK_CUR);
    buffy[bytesread] = '\0';
    printf("%s", buffy);
    sleep(1);
  }
}
void read_via_streams() {
  FILE *fp = fopen(FILE_PATH, "r+");
  if (fp == NULL)
    perror("read: ");
  printf("read fileno: %d\n", fileno(fp)); 

  char buffy[1024];
  while(TRUE) {
    //    checklocks(fd);
    size_t bytesread = fread(buffy, sizeof(char), 1024 - 1, fp);
    buffy[bytesread] = '\0';
    printf("%s", buffy);
    fflush(stdout);
    sleep(1);
  }
}
void *read_me(void*args) {
  //  int fd = open(FILE_PATH, O_RDONLY|O_SYNC);
  if (args == NULL)
    read_via_fds();
  else
    read_via_streams();
  return NULL;
}
void write_via_streams() {
  FILE *fp = fopen(FILE_PATH, "w+");
  if (fp == NULL)
    perror("write: ");
  printf("write fileno: %d\n", fileno(fp));
  fcntl(fileno(fp), F_SETFD, O_WRONLY|O_CREAT|O_TRUNC|O_SYNC);
  fprintf(fp, "buffy 1\n");

  char *stream = "stream";
  pthread_t th;
//  pthread_create(&th, NULL, read_me, (void*)stream);
  pthread_create(&th, NULL, read_me, NULL); 
  sleep(1);

  int secs = 59; 
  while (secs > 0) {
    fprintf(fp, "buffy %d\n", 60 - secs + 1);
    fflush(fp);
    secs--;
  }
  sleep(1);
}
void write_via_fds() {
  //  int fd = open(FILE_PATH, O_WRONLY|O_CREAT|O_TRUNC|O_SYNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);
  int fd = open(FILE_PATH, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);
  if (fd < 0)
    perror("write: ");
  printf("write fileno: %d\n", fd);
  write(fd, "buffy 1\n", strlen("buffy 1\n"));

  pthread_t th;
  pthread_create(&th, NULL, read_me, NULL);
  sleep(1);

  int secs = 59; 
  while (secs > 0) {
    char text[11];
    sprintf(text, "buffy %d\n", 60 - secs + 1);
    secs--;
    write(fd, text, strlen(text));
  }
  sleep(1);
}
int main(int argc, char **argv) {
  if (argc < 2)
    write_via_fds();
  else
    write_via_streams();
  return 0;
}
