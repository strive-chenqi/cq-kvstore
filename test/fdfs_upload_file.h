#ifndef FDFS_UPLOAD_FILE_H
#define FDFS_UPLOAD_FILE_H

extern void upload_file_pipe(const char *confFile, const char *uploadFile, char *fileID, int size);

extern int upload_file_api(const char *confFile, const char *myFile, char *fileID);

#endif //FDFS_UPLOAD_FILE_H