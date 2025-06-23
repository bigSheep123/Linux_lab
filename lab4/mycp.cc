#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <iostream>
#include <cassert>
#include <ostream>
#include <unistd.h>
#include<sys/stat.h>

char* getString(char* srcPath)
{
    int num = strlen(srcPath) - 1;
    while (num >= 0 && srcPath[num] != '/') {
        num--;
    }
    if (num < 0) num = -1;
    char* fileName = (char*)malloc(strlen(srcPath) - num);
    int j = 0;
    for (int i = num + 1; i < strlen(srcPath); i++) {
        fileName[j++] = srcPath[i];
    }
    fileName[j] = '\0'; 
    return fileName;
}

void DealFile(char* destPath,char* srcPath)
{
    struct stat src_stat;
    stat(srcPath,&src_stat);

    char* fileName = getString(srcPath);
    char destfile[100];
    sprintf(destfile,"%s%s%s", destPath, "/",fileName);
    
    int srcfd = open(srcPath,O_RDONLY);
    int desfd = open(destfile,O_CREAT| O_RDWR,src_stat.st_mode);
    close(desfd);
}

void DealDir(char* destPath,char* srcPath)
{
    DIR* srcDir = opendir(srcPath);
    struct dirent* it;
    while( (it = readdir(srcDir)) != nullptr)
    {
        if ( strcmp(it->d_name, ".") == 0 || strcmp(it->d_name, "..") == 0)
            continue;
        std::cout << it->d_name << std::endl;
    }
    closedir(srcDir);
}

// cp /home/dh/labLinux/README.md  /home/dh/labLinux/lab4/
int main(int argc,char** argv)
{
    if ( argc < 3 )  {
        assert("mycp must have three argvs at least!");
        return 1; 
    }
    bool flag = false;
    char * srcPath, * destPath;
    if( argc == 3) 
    {
        srcPath = argv[1];
        destPath = argv[2];
    }
    else if ( argc == 4 ) 
    {
        flag = true;
        srcPath = argv[2];
        destPath = argv[3];
    }
    else  {
        assert("must have some args we can't to deal it");
        return 1;
    }

    if ( flag )
    {
        DealDir(destPath,srcPath);
    }
    else {
        DealFile(destPath,srcPath);
    }

    return 0;
}