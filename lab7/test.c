#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#define DEV_NAME "/dev/dhDev"

int main() 
{
  char* name = (char *)malloc(1024);
  char* out = (char *)malloc(1024);
  strcpy(name, "I am dh's Deviece!");
  int fd = open(DEV_NAME, O_RDWR);  // read write

  if (fd < 0) // fd < 0 文件打开失败了 
  {
    printf("%d\n", fd);
    perror("Can Not Open File");
    return 1;
  }

  int num = write(fd, name, strlen(name)); // num 成功代表写入的bytes
  if (num < 0) {
    perror("File is Not Writeable\n");
    return 0;
  }

  printf("Success Open File!\n");
  num = read(fd, out, 1024); // 将读出的数据存放到 out 种

  if (num < 0) {
    perror("File is Not Readable\n");
    return 0;
  }
  
  printf("Success Read the File! Output is: %s\n", out);
  close(fd);
}