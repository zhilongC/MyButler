#include "file_manage.h"

BU_UINT32 file_num(const char* path)
{
    DIR *dp = NULL;
    struct dirent *dirp = NULL;
    BU_UINT32 count = 0;
    
    if ((dp = opendir (path)) == NULL) {

        E_LOG("opendir file\n");
        return 0;
    }

    while ((dirp = readdir(dp)) != NULL) {
        if ((strcmp (dirp->d_name, ".") == 0 || strcmp (dirp->d_name, "..") == 0)){

			continue;
    	}

        count++;
    }
    rewinddir (dp);

    I_LOG("file_num[%d]\n", count);
    
    return count;
}

BU_UINT32 file_get_size (const char* path)
{
    FILE* fp = NULL;    
    int length = 0;

    if((fp = fopen (path, "rb")) == NULL){

        E_LOG("open failed!\n");
        return 0;
    }
    fseek (fp, 0, SEEK_END);
    length = ftell (fp);
    fclose (fp);
    
    return length;
}

BU_UINT8 file_get_list(const char* path, file_info_t* info_arr, BU_UINT32 arrNum)
{
    DIR *dp = NULL;
    struct dirent *dirp = NULL;
    struct stat statbuf;
    char filePath[FILE_MAX_NAME_LEN] = {0};

    BU_UINT32 i = 0;
    
    if ((dp = opendir (path)) == NULL) {

        E_LOG("opendir file\n");
        return 0;
    }

    while ((dirp = readdir(dp)) != NULL) {
        /*ignore hidden file*/
        if ((strcmp (dirp->d_name, ".") == 0 || strcmp (dirp->d_name, "..") == 0)){

			continue;
    	}
		
        bzero(filePath, sizeof (filePath));
        sprintf (filePath, "%s/%s", path, dirp->d_name);
        
        /* get file profile */
        memcpy(info_arr[i].name, dirp->d_name, strlen(dirp->d_name));
        if (dirp->d_type == 4) {

            info_arr[i].type = FTYPE_DIR;
            info_arr[i].size = 1;
        } else if (dirp->d_type == 8) {
        
            info_arr[i].type = FTYPE_NORMAL;
            info_arr[i].size = file_get_size(filePath);
        } else {
        
            info_arr[i].type = FTYPE_UNKNOW;
            info_arr[i].size = 0;
        }

        if (lstat (filePath, &statbuf) != 0) {
        
            E_LOG("lstat2 file error: %s\n", strerror (errno));
            memset(&info_arr[i], 0, sizeof(file_info_t));
            continue;
        }
        info_arr[i].modifi_time = statbuf.st_mtime;
          
        i++;
    }
    return BU_OK;
}
