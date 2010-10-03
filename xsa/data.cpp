#include "fortify.h"

// #include <fstream>
// #include <iostream>
// #include <string>

#include <stdio.h>
#include <string.h>

#include "sqlite/sqlite3.h"

#include <sys/stat.h>

// #include <pcre.h>

#define BUF_SIZE 4096

int main(int argc, char* argv[]) {
    sqlite3* db;
    struct stat info;
    int ret = -1;
    ret = stat("file.db",&info);
    if(ret) {
        printf("file doesn't exist\n");
        ret = sqlite3_open("file.db",&db);
        if(ret != SQLITE_OK) { printf("open failed\n"); }
    } else {
        printf("file exists\n");
    }
    Fortify_ListAllMemory();
}
