#include <cassert>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <ostream>
#include <unistd.h>
#include <cstdio>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>

#define SIZE 100

void DealMode(struct stat *st)
{
    mode_t perm = st->st_mode & 0777;
    mode_t type = st->st_mode;
    if (S_ISREG(type))
        printf("-");
    if (S_ISDIR(type))
        printf("d");
    if (S_ISLNK(type))
        printf("l");
    if (S_ISBLK(type))
        printf("b");
    if (S_ISCHR(type))
        printf("c");
    if (S_ISSOCK(type))
        printf("s");
    if (S_ISFIFO(type))
        printf("p");

    // 提取权限位（低 9 位）
    // 用户权限
    printf("%c%c%c",
           (perm & S_IRUSR) ? 'r' : '-',
           (perm & S_IWUSR) ? 'w' : '-',
           (perm & S_IXUSR) ? 'x' : '-');

    // 组权限
    printf("%c%c%c",
           (perm & S_IRGRP) ? 'r' : '-',
           (perm & S_IWGRP) ? 'w' : '-',
           (perm & S_IXGRP) ? 'x' : '-');

    // 其他用户权限
    printf("%c%c%c",
           (perm & S_IROTH) ? 'r' : '-',
           (perm & S_IWOTH) ? 'w' : '-',
           (perm & S_IXOTH) ? 'x' : '-');
}

void DealuAndg(struct stat *st)
{
    // 将 st_uid 转换为用户名
    struct passwd *pwd = getpwuid(st->st_uid);
    printf(" %s", (pwd != NULL) ? pwd->pw_name : "未知用户");

    // 将 st_gid 转换为组名
    struct group *grp = getgrgid(st->st_gid);
    printf(" %s", (grp != NULL) ? grp->gr_name : "未知组");
}

void DealTime(struct stat *st)
{
    time_t ctime_sec = st->st_ctim.tv_sec;
    // 转换为本地时间
    struct tm *time_info = localtime(&ctime_sec);

    // 格式化输出
    char buffer[64];
    strftime(buffer, sizeof(buffer), "%b %d %H:%M", time_info);
    printf(" %s ", buffer); // 示例输出: Apr 18 11:24
}

void DealAll(struct dirent *Dirent, struct stat *buf)
{
    DealMode(buf);
    // std::cout << buf->st_uid << buf->st_gid << buf->st_size<< std::endl;
    // std::cout << buf->st_mode << std::endl;
    std::cout << " " << buf->st_nlink;
    // std::cout << buf->st_ctim.tv_nsec << std::endl;
    // printf("%s\n",str);
    DealuAndg(buf);
    // 权限，硬连接，用户和所属组，文件字节大小，最后一次修改时间，文件名字
    // std::cout << buf->st_mode << std :: endl;
    std::cout << " " << buf->st_size;

    // std::cout << buf->st_ctim.tv_nsec << std::endl;
    DealTime(buf);
    std::cout << Dirent->d_name << std::endl;
}

int main()
{
    // str 保存当前工作目录的绝对路径
    char path[SIZE];
    getcwd(path, SIZE);
    // open 该文件流
    DIR *dir = opendir(path);
    if (!dir)
        assert("open the dir fialed");

    // 读取目录文件
    struct dirent *Dirent = nullptr;
    while ((Dirent = readdir(dir)) != nullptr)
    {
        Dirent = readdir(dir);
        struct stat *buf = (struct stat *)malloc(sizeof(struct stat));
        char *name = Dirent->d_name;
        char *tmp = path;
        size_t len1 = strlen(tmp);
        size_t len2 = strlen(name);
        size_t total_len = len1 + len2 + 1;

        // 分配内存
        char *result = (char *)malloc(total_len * sizeof(char));
        if (result == NULL)
        {
            fprintf(stderr, "内存分配失败\n");
        }

        // 复制第一个字符串
        strcpy(result, tmp);
        // 追加第二个字符串
        char* sb = "\\";
        strcpy(result,sb);
        strcat(result, name);
        printf("%s\n",result);
        lstat(result, buf);

        // DealAll(Dirent, buf);
    }

    closedir(dir);
    return 0;
}