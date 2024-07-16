#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include "fdfs_upload_file.h"

int main() {
    char fildID[1024] = {0};
    upload_file_pipe("../conf/client.conf", "fdfs_test.txt", fildID, sizeof(fildID));
    printf("Multiprocess upload_file1 fildID:%s\n", fildID);
    printf("=================================\n");
    upload_file_api("../conf/client.conf", "fdfs_test.txt", fildID);
    printf("Call API upload_file2 fildID:%s\n", fildID);
}