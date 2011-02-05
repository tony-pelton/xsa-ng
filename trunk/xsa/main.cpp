#include "fortify.h"

// #include <fstream>
// #include <iostream>
// #include <string>

#include <stdio.h>
#include <string.h>

#include "sqlite/sqlite3.h"
// #include "sqlite/sqlite3ext.h"

#include <sys/stat.h>

// #include <pcre.h>

#define BUF_SIZE 4096

int main(int argc, char* argv[]) {
    sqlite3* db;
    sqlite3_stmt* stmt;
    struct stat info;
    int ret = -1;
    ret = stat("file.db",&info);
    if(ret) {
        printf("file doesn't exist\n");
        ret = sqlite3_open_v2("file.db",&db,SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE,NULL);
        if(ret != SQLITE_OK) { printf("open failed\n"); exit(1); }
        ret = sqlite3_prepare_v2(db,"create table preferences(key text,value)",-1,&stmt,NULL);
        if(ret != SQLITE_OK) { printf("create failed\n"); exit(1); }
    } else {
        printf("file exists\n");
    }
    Fortify_ListAllMemory();
}
