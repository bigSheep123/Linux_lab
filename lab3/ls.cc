#include <cstddef>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <dirent.h>
#include <grp.h>
#include <linux/limits.h>
#include <pwd.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <unistd.h>

char getType(mode_t mode)
{
    if ( S_ISREG(mode) )  return '-';
    if ( S_ISDIR(mode) )  return 'd';
    if ( S_ISCHR(mode) )  return 'c';
    if ( S_ISBLK(mode) )  return 'b';
    if (S_ISFIFO(mode)) return 'p';
    if ( S_ISLNK(mode) )  return 'l';
    if ( S_ISSOCK(mode) )  return 's';

    return '?';
}

void getPermission(mode_t mode, char* perm_str)
{
    // 用户权限
    perm_str[0] = (mode & S_IRUSR) ? 'r' : '-';
    perm_str[1] = (mode & S_IWUSR) ? 'w' : '-';
    perm_str[2] = (mode & S_IXUSR) ? 'x' : '-';
    
    // 组权限
    perm_str[3] = (mode & S_IRGRP) ? 'r' : '-';
    perm_str[4] = (mode & S_IWGRP) ? 'w' : '-';
    perm_str[5] = (mode & S_IXGRP) ? 'x' : '-';
    
    // 其他用户权限
    perm_str[6] = (mode & S_IROTH) ? 'r' : '-';
    perm_str[7] = (mode & S_IWOTH) ? 'w' : '-';
    perm_str[8] = (mode & S_IXOTH) ? 'x' : '-';
    
    perm_str[9] = '\0';
}

const char* getOwner(uid_t uid) {
    struct passwd *pw = getpwuid(uid);
    return pw ? pw->pw_name : "unknown";
}

const char* getGroup(gid_t gid) {
    struct group *gr = getgrgid(gid);
    return gr ? gr->gr_name : "unknown";
}

char *get_formatted_time(const time_t *time) {
    struct tm *tm_info = localtime(time);
    static char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M", tm_info);
    return buffer;
}

void dealDir(const char* path)
{
    DIR* dir;
    struct dirent * entry;
    struct stat statbuf;
    char full_path[PATH_MAX];

    // 打开对应的目录文件
    if( (dir = opendir(path) ) == nullptr) {
        perror("can't open it");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if ( strcmp(entry->d_name,".") == 0 || strcmp(entry->d_name,"..") == 0)
            continue;
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
        // std::cout << full_path << std::endl;

        if ( lstat(full_path,&statbuf) < 0) {
            perror(full_path);
            continue;
        }

        char type = getType(statbuf.st_mode);

        char perm[10];
        getPermission(statbuf.st_mode,perm);
        char permissions[12];
        snprintf(permissions,sizeof(permissions),"%c%s",type,perm);

        // std::cout << permissions << std::endl;

        const char* owner = getOwner(statbuf.st_uid);
        const char* group = getGroup(statbuf.st_gid);

        char filename[NAME_MAX + 10];
               if (S_ISLNK(statbuf.st_mode)) {
            char link_target[PATH_MAX];
            ssize_t len = readlink(full_path, link_target, sizeof(link_target) - 1);
            if (len != -1) {
                link_target[len] = '\0';
                snprintf(filename, sizeof(filename), "%s -> %s", entry->d_name, link_target);
            } else {
                snprintf(filename, sizeof(filename), "%s", entry->d_name);
            }
        } else {
            strncpy(filename, entry->d_name, sizeof(filename));
        }
        
        // 输出文件信息
        printf("%-10s %-3ld %-3s %-3s %-8ld %-19s %s\n", 
               permissions,
               (long)statbuf.st_nlink,
               owner,
               group,
               (long)statbuf.st_size,
               get_formatted_time(&statbuf.st_mtime),
               filename); 
    }  

    closedir(dir);
}

// ls -l  
// ls -l /home/dh/labLinux/lab3/test
int main(int argc,char** argv)
{
    char cwd[PATH_MAX];

    if( argc > 1) {
        for(int i = 1; i < argc; i++) {
          dealDir(argv[i]);
        }
    }
    else { // 获取当前的目录
        if ( getcwd(cwd,sizeof(cwd)) ) {
          dealDir(cwd);
        }
        else {
          perror(" cant't get the working dir");
          return -1;
        }
    }

    return 0;
}