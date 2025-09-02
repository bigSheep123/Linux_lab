#include <stdio.h>
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <iostream>
#include <cassert>
#include <ostream>
#include <unistd.h>
#include<sys/stat.h>

// 实现的这个版本的代码，健壮性太差了
// 健壮性好一点就好了
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

    char buffer[65536];
    ssize_t bytes_read;
    // std::cout << sizeof(ssize_t) << std::endl;
    while((bytes_read = read(srcfd,buffer,sizeof(buffer)))) {
        if(bytes_read == -1) {
            perror("Read error");
            break;
        }
        // std::cout << bytes_read << std::endl;
        if(write(desfd, buffer, bytes_read) != bytes_read) {
            perror("Write error");
            break; 
        }
    }
    close(desfd);
    close(srcfd);
}

void DealDir(char* destPath,char* srcPath)
{
    DIR* srcDir = opendir(srcPath);
    struct dirent* it;
    while( (it = readdir(srcDir)) != nullptr)
    {
        if ( strcmp(it->d_name, ".") == 0 || strcmp(it->d_name, "..") == 0)
            continue;
        if(it->d_type == DT_DIR) {
            char save[128];
            char perm[255];
            snprintf(save,sizeof(save),"%s%s%s",destPath,"/",it->d_name); 
            snprintf(perm,sizeof(perm),"%s/%s",srcPath,it->d_name); 
            struct stat perm_stat;
            // std::cout << save << std::endl;
            // std::cout << perm << std::endl;
            stat(perm,&perm_stat);
            mkdir(save,perm_stat.st_mode);
            DealDir(save,perm);            
        } else {  //普通文件
            char save[128];
            sprintf(save,"%s%s%s",srcPath,"/",it->d_name); 
            DealFile(destPath,save);
        }   
    }
    closedir(srcDir);
}

// cp /home/dh/labLinux/README.md  /home/dh/labLinux/lab4/
// cp -r /home/dh/labLinux/test  /home/dh/labLinux/lab4/
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