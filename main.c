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

void *read_me(void*args) {
  int fd = open(FILE_PATH, O_RDONLY|O_SYNC);
  if (fd < 0)
    perror("read: ");
  printf("read fileno: %d\n", fd); 

  char buffy[1024];
  long last_read = 0;
  while(TRUE) {
    lseek(fd, last_read, SEEK_SET);
    size_t bytesread = read(fd, buffy, 1024 - 1);
    last_read = lseek(fd, 0L, SEEK_CUR);
    buffy[bytesread] = '\0';
    printf("%s", buffy);
    sleep(1);
  }
  return NULL;
}
int main() {
  int fd = open(FILE_PATH, O_WRONLY|O_CREAT|O_TRUNC|O_SYNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);
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
  return 0;
}
