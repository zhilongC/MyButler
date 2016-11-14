#include "file_manage.h"

BU_UINT32 file_sys_cmd (IN const char* pSrcBuf)
{
    BU_UINT32 ulRes = 0;

    if (NULL == pSrcBuf) {
        E_LOG("pSrcBuf NULL!");
        return BU_ERROR;
    }

    ulRes = system (pSrcBuf);

    if (ulRes == -1) {
        E_LOG("system error!");
        return BU_ERROR;
    }
    if (WIFEXITED (ulRes)) {
        if (WEXITSTATUS (ulRes) == 0) {
            return BU_OK;
        } else {
            E_LOG("system error!");
            return BU_ERROR;
        }
    } else {
        E_LOG("cp exit illegal![%d]\n", errno);
        return BU_ERROR;
    }
}


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
BU_UINT32 file_copy(const char* src_path, const char* dst_path)
{
	char format_cmd[1024] = {0};

	if(src_path == NULL || dst_path == NULL || strlen(src_path)+strlen(dst_path)>1000){

		E_LOG("wrong input\n");
		return BU_ERROR;
	}

	sprintf(format_cmd, "cp -fr %s %s", src_path, dst_path);
	I_LOG("format_cmd[%s]\n", format_cmd);

	return file_sys_cmd(format_cmd);
}
BU_UINT32 file_rename(const char* src_path, const char* dst_path)
{
	char format_cmd[1024] = {0};

	if(src_path == NULL || dst_path == NULL || strlen(src_path)+strlen(dst_path)>1000){

		E_LOG("wrong input\n");
		return BU_ERROR;
	}

	sprintf(format_cmd, "mv %s %s", src_path, dst_path);
	I_LOG("format_cmd[%s]\n", format_cmd);

	return file_sys_cmd(format_cmd);
}


BU_UINT32 file_new_file(const char* path)
{
	if(-1 == open(path, (O_CREAT|O_WRONLY|O_TRUNC), 0666)){

		E_LOG("create new file error\n");
		return BU_ERROR;
	}
	
	return BU_OK;
}

BU_UINT32 file_new_dir(const char* path)
{
	if(-1 == mkdir(path, 0777)){

		E_LOG("make dir error\n");
		return BU_ERROR;
	}
	
	return BU_OK;
}

BU_UINT32 file_delete(const char* path)
{
	I_LOG("remove file name [%s]\n", path);

	if(remove(path) != 0){

		E_LOG("remove error\n");
		return BU_ERROR;
	}
	
	return BU_OK;
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
