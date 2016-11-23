#include "prot.h"

static prot_session_info_t s_sess_info[MAX_SESSION_NUM];
msg_callback_node_t g_msg_prot; 
bool QUIT = false;

static prot_handle_t create_session(void* sock, int ver, const char* acc, const char* pwd)
{
    int i = 0;
    
    if(acc == NULL || pwd == NULL){

        return -1;
    }
    
    for(i=0; i<MAX_SESSION_NUM; i++){
        if(s_sess_info[i].account == NULL){
            
            s_sess_info[i].account = strdup(acc);
            s_sess_info[i].pwd = strdup(pwd);
            s_sess_info[i].sock = sock;
            s_sess_info[i].ver = ver;
            return i;
        }
    }

    return -1;
}


void handle(int sig)
{   
    if (sig == SIGINT)
        QUIT = true;
}

prot_session_info_t* find_session_info(prot_handle_t phandle){
    if(check_prot_handle(phandle)){

        return (prot_session_info_t*)&s_sess_info[phandle];
    }

    return NULL;
}
static void readCallback(Socket *sp)
{
    int n;
    int len = 0;
    int data_len = 0;
    static char* buf = (char*)calloc(1, 1024);
    static int buf_len = 1024;

    n = readSocket(sp, &len, 4);

    len = ntohl(len);
    
    if (n <= 0)
    {
        E_LOG("The peer had shutdown\n");
        closeSocket(sp);
        return;
    }
    else
    {
        I_LOG("recv len [%d]\n", len);
        if(len == 0)
        {
            return;
        }
        else
        {
            data_len = len;
            if(len > buf_len)
            {
                free(buf);
                buf = (char*)calloc(1, len+sizeof(void*));
                buf_len = len+sizeof(void*);
            }
            char* tmp = buf + sizeof(void*);
            while(len>0)
            {
                n = readSocket(sp, tmp, len);
                if(n <= 0)
                {
                    printf("============================\n");
                    closeSocket(sp);
                    break;
                }
                else
                {
                    len -= n;
                    tmp = (char*)tmp + n;
                }

            }
        }
    }
    
    I_LOG("read from %s:%d\n", inet_ntoa(sp->pAddr.sin_addr), ntohs(sp->pAddr.sin_port));
    I_LOG("buf[%s]\n", buf+sizeof(void*));

    memcpy(buf, &sp, sizeof(void*));
    msg_list_push(buf, buf_len, PROT_TASK_ID, PROT_NET_TASK_ID); 
}

static void callbackAccept(Socket *sp)
{
    Socket *sp_c = acceptSocket(sp, readCallback);
    printf("New connection:%s:%d\n", inet_ntoa(sp_c->pAddr.sin_addr), ntohs(sp_c->pAddr.sin_port));
    
}

BU_INT8 send2ctrl(prot_handle_t pHandle, BU_UINT32 type, char* msg, BU_UINT32 msg_len)
{ 
    char* sendBuf = (char*)calloc(1, msg_len + sizeof(type) + sizeof(prot_handle_t));
    if(NULL == sendBuf)
    {
        return BU_ERROR;
    }
    memcpy(sendBuf, &pHandle, sizeof(prot_handle_t));
    memcpy(sendBuf+sizeof(prot_handle_t), &type, sizeof(BU_UINT32));
    memcpy(sendBuf+sizeof(type)+sizeof(prot_handle_t), msg, msg_len);
    msg_list_push(sendBuf, msg_len+sizeof(type)+sizeof(prot_handle_t), CONTRL_TASK_ID, PROT_TASK_ID); 

    return BU_OK;
}

BU_INT8 check_prot_handle(prot_handle_t pHandle)
{
    if(pHandle < 0 || pHandle >= MAX_SESSION_NUM){

        return BU_FALSE;
    } else {

        return BU_TRUE;
    }
}

BU_INT8 notifi_load(prot_handle_t pHandle)
{ 
    pkt_mhead_t mhead;

    if(!check_prot_handle(pHandle)){

        return BU_ERROR;
    }

    mhead.prot_handle = pHandle;
    mhead.type = PKG_PROT_MEDIA_LOAD;

    msg_list_push((char*)&mhead, sizeof(pkt_mhead_t), MEDIA_TASK_ID, PROT_TASK_ID); 
    //msg_list_push((char*)&mhead, sizeof(pkt_mhead_t), CONTRL_TASK_ID); 
    
    return BU_OK;
}

void* prot_main(void* p)
{
    I_LOG("prot_main start\n");

    signal(SIGINT, handle);
    
    libSocketInit();
    
    Socket *sp = createSocket(SOCK_STREAM, 0, callbackAccept);
    if (sp == NULL)
        return NULL;
    
    listenSocket(sp, 8888);
    while(1)
    {
        msg_list_handle(&g_msg_prot, prot_msg_cb);
        sleep(5);
    }
    libSocketDeinit();

    return NULL;
}

BU_INT8 send_json_str(void* sock, const char* sendbuf, int buf_len)
{
    char* buf = NULL;
    BU_UINT32 ulLen = 0; 
    
    if(buf_len == 0 || sendbuf == NULL){
        E_LOG("input error\n");
        return BU_ERROR;
    }

    buf = (char*)calloc(1, buf_len+4);
    if(NULL == buf){
        E_LOG("calloc error\n");
        return BU_ERROR;        
    }

    ulLen = htonl(buf_len);
    memcpy(buf, &ulLen, 4);
    memcpy(buf+4, sendbuf, buf_len);
    
    writeSocket((Socket*)sock, (void*)buf, buf_len+4, 0);

    free(buf);
    buf = NULL;
    
    return BU_OK;
}

BU_INT8 load_msg_proc(void* sock, json_object* msg_obj)
{
    json_object *my_object = json_object_new_object ();
    prot_handle_t tmp_handle = -1;
    int code = BU_OK;
    const char* tempSend = NULL;

    tmp_handle = create_session(sock, 
       json_object_get_int(json_object_object_get(msg_obj, "VER")),
       json_object_get_string(json_object_object_get(msg_obj, "ACCOUNT")),
       json_object_get_string(json_object_object_get(msg_obj, "PWD")));
    if(check_prot_handle(tmp_handle)){

        notifi_load(tmp_handle);
    } else {
    	E_LOG("create session fail\n");
        code = BU_ERROR;
    }
    
    json_object_object_add (my_object, "DID", json_object_new_int (tmp_handle));
    json_object_object_add (my_object, "TYPE", json_object_new_int (PROT_TYPE_LOAD));
    json_object_object_add (my_object, "CODE", json_object_new_int (code));

    tempSend = json_object_to_json_string (my_object);// if my_object is null , then the function return string "null"
    if(tempSend != NULL && strcmp(tempSend, "null") != 0){
        send_json_str(sock, tempSend, strlen(tempSend));
    }

    return BU_OK;
}

BU_INT8 file_download_msg_proc(void* sock, json_object* msg_obj, prot_handle_t pHandle)
{
	char* sendBuf = NULL;
	BU_UINT32 file_type = 0;
	BU_UINT32 ulOffSet = 0;
	BU_UINT32 file_name_len = 0;
	const char* file_name = NULL;
	prot_session_info_t* psInfo = NULL;
	pkt_mhead_t mhead;

	if((psInfo = find_session_info(pHandle)) == NULL){

		E_LOG("no find session \n");
		return BU_ERROR;
	}

	file_type = json_object_get_int(json_object_object_get(msg_obj, "FTYPE"));
	file_name = json_object_get_string(json_object_object_get(msg_obj, "FILE_NAME"));

	
	mhead.prot_handle = pHandle;
	mhead.media_handle = psInfo->mhandle;
	mhead.type = PKG_PROT_MEDIA_FDOWN;

	file_name_len = strlen(file_name);
	
	/* ****************pkg prot PKG_PROT_MEDIA_FDOWN************** 
		+4B: file type
		+4B: file name length
		+NB: file name
		
	*********************************************************** */
	PKG_INIT_BUFF(sendBuf, sizeof(pkt_mhead_t)+ 4 + 4 + file_name_len);
	PKG_BYTES_MSG(sendBuf, ulOffSet, &mhead, sizeof(pkt_mhead_t));
	PKG_UINT32_MSG(sendBuf,ulOffSet, file_type);
	PKG_UINT32_MSG(sendBuf,ulOffSet, file_name_len);
	PKG_BYTES_MSG(sendBuf, ulOffSet, file_name, file_name_len);	 	 
	msg_list_push(sendBuf, ulOffSet, MEDIA_TASK_ID, PROT_TASK_ID); 
	PKG_FREE_BUFF(sendBuf);

	return BU_OK;
}


BU_INT8 file_copy_msg_proc(void* sock, json_object* msg_obj, prot_handle_t pHandle)
{
	char* sendBuf = NULL;

	BU_UINT32 file_type = 0;
	BU_UINT32 ulOffSet = 0;
	BU_UINT32 file_src_len = 0;
	BU_UINT32 file_dst_len = 0;
	const char* file_src = NULL;
	const char* file_dst = NULL;
	prot_session_info_t* psInfo = NULL;
	pkt_mhead_t mhead;

	if((psInfo = find_session_info(pHandle)) == NULL){

		E_LOG("no find session \n");
		return BU_ERROR;
	}

	file_type = json_object_get_int(json_object_object_get(msg_obj, "FTYPE"));
	file_src = json_object_get_string(json_object_object_get(msg_obj, "FILE_SRC"));
	file_dst = json_object_get_string(json_object_object_get(msg_obj, "FILE_DST"));

	
	mhead.prot_handle = pHandle;
	mhead.media_handle = psInfo->mhandle;
	mhead.type = PKG_PROT_MEDIA_FCOPY;

	file_src_len = strlen(file_src);
	file_dst_len = strlen(file_dst);

	
	/* ****************pkg prot PKG_PROT_MEDIA_FCOPY************** 
		+4B: file type
		+4B: file src length
		+NB: file src
		+4B: file dst length
		+NB: file dst		
	*********************************************************** */
	PKG_INIT_BUFF(sendBuf, sizeof(pkt_mhead_t)+ 4 + 4 + file_src_len + 4 + file_dst_len);
	PKG_BYTES_MSG(sendBuf, ulOffSet, &mhead, sizeof(pkt_mhead_t));
	PKG_UINT32_MSG(sendBuf,ulOffSet, file_src_len);
	PKG_BYTES_MSG(sendBuf, ulOffSet, file_src, file_src_len);	 
	PKG_UINT32_MSG(sendBuf,ulOffSet, file_dst_len);
	PKG_BYTES_MSG(sendBuf, ulOffSet, file_dst, file_dst_len);	 
	msg_list_push(sendBuf, ulOffSet, MEDIA_TASK_ID, PROT_TASK_ID); 
	PKG_FREE_BUFF(sendBuf);

	return BU_OK;
}

BU_INT8 file_rename_msg_proc(void* sock, json_object* msg_obj, prot_handle_t pHandle)
{
	char* sendBuf = NULL;

	BU_UINT32 file_type = 0;
	BU_UINT32 ulOffSet = 0;
	BU_UINT32 file_src_len = 0;
	BU_UINT32 file_dst_len = 0;
	const char* file_src = NULL;
	const char* file_dst = NULL;
	prot_session_info_t* psInfo = NULL;
	pkt_mhead_t mhead;

	if((psInfo = find_session_info(pHandle)) == NULL){

		E_LOG("no find session \n");
		return BU_ERROR;
	}

	file_type = json_object_get_int(json_object_object_get(msg_obj, "FTYPE"));
	file_src = json_object_get_string(json_object_object_get(msg_obj, "FILE_SRC"));
	file_dst = json_object_get_string(json_object_object_get(msg_obj, "FILE_DST"));

	
	mhead.prot_handle = pHandle;
	mhead.media_handle = psInfo->mhandle;
	mhead.type = PKG_PROT_MEDIA_FRENAME;

	file_src_len = strlen(file_src);
	file_dst_len = strlen(file_dst);

	
	/* ****************pkg prot PKG_PROT_MEDIA_FRENAME************** 
			+4B: file type
			+4B: file src length
			+NB: file src
			+4B: file dst length
			+NB: file dst		
	*********************************************************** */

	PKG_INIT_BUFF(sendBuf, sizeof(pkt_mhead_t)+ 4 + 4 + file_src_len + 4 + file_dst_len);
	PKG_BYTES_MSG(sendBuf, ulOffSet, &mhead, sizeof(pkt_mhead_t));
	PKG_UINT32_MSG(sendBuf,ulOffSet, file_src_len);
	PKG_BYTES_MSG(sendBuf, ulOffSet, file_src, file_src_len);    
	PKG_UINT32_MSG(sendBuf,ulOffSet, file_dst_len);
	PKG_BYTES_MSG(sendBuf, ulOffSet, file_dst, file_dst_len);    
	msg_list_push(sendBuf, ulOffSet, MEDIA_TASK_ID, PROT_TASK_ID); 
	PKG_FREE_BUFF(sendBuf);

	return BU_OK;
}


BU_INT8 file_new_msg_proc(void* sock, json_object* msg_obj, prot_handle_t pHandle)
{
	char* sendBuf = NULL;

	BU_UINT32 file_type = 0;
	BU_UINT32 ulOffSet = 0;
	BU_UINT32 file_name_len = 0;
	const char* file_name = NULL;
    prot_session_info_t* psInfo = NULL;
    pkt_mhead_t mhead;

    if((psInfo = find_session_info(pHandle)) == NULL){

		E_LOG("no find session \n");
		return BU_ERROR;
	}

	file_type = json_object_get_int(json_object_object_get(msg_obj, "FTYPE"));
	file_name = json_object_get_string(json_object_object_get(msg_obj, "FNAME"));
	
    mhead.prot_handle = pHandle;
    mhead.media_handle = psInfo->mhandle;
    mhead.type = PKG_PROT_MEDIA_FDEL;

	file_name_len = strlen(file_name);
	
	/* ****************pkg prot PKG_PROT_MEDIA_FNEW************** 
		+4B: file type
		+4B: file name length
		+NB: file name
		
	*********************************************************** */
    PKG_INIT_BUFF(sendBuf, sizeof(pkt_mhead_t)+ 4 + 4 + file_name_len);
    PKG_BYTES_MSG(sendBuf, ulOffSet, &mhead, sizeof(pkt_mhead_t));
    PKG_UINT32_MSG(sendBuf,ulOffSet, file_name_len);
    PKG_BYTES_MSG(sendBuf, ulOffSet, file_name, file_name_len);    
    msg_list_push(sendBuf, ulOffSet, MEDIA_TASK_ID, PROT_TASK_ID); 
    PKG_FREE_BUFF(sendBuf);

	return BU_OK;
}

BU_INT8 file_del_msg_proc(void* sock, json_object* msg_obj, prot_handle_t pHandle)
{
    char* sendBuf = NULL;
    const char* path = NULL;
    int pathLen = 0;
    int offset = 0;
    pkt_mhead_t mhead;
    prot_session_info_t* psInfo = NULL;

    if((psInfo = find_session_info(pHandle)) == NULL){

		E_LOG("no find session \n");
		return BU_ERROR;
	}
    mhead.prot_handle = pHandle;
    mhead.media_handle = psInfo->mhandle;
    mhead.type = PKG_PROT_MEDIA_FDEL;

    if((path = json_object_get_string(json_object_object_get(msg_obj, "FILE_PATH"))) == NULL){
        E_LOG("FILE_PATH is empty\n");
        return BU_ERROR;
    }
    pathLen = strlen(path);
    /* ****************pkg prot PKG_PROT_MEDIA_FLIST************** 
        +4B: pathLen
        +NB: path string with length pathLen
        
    	*********************************************************** */
    PKG_INIT_BUFF(sendBuf, sizeof(pkt_mhead_t)+ pathLen + 4);
    PKG_BYTES_MSG(sendBuf, offset, &mhead, sizeof(pkt_mhead_t));
    PKG_UINT32_MSG(sendBuf,offset, pathLen);
    PKG_BYTES_MSG(sendBuf, offset, path, pathLen);
    
    msg_list_push(sendBuf, offset, MEDIA_TASK_ID, PROT_TASK_ID); 
    PKG_FREE_BUFF(sendBuf);

	return BU_OK;
}

BU_INT8 file_list_msg_proc(void* sock, json_object* msg_obj, prot_handle_t pHandle)
{ 
    char* sendBuf = NULL;
    const char* path = NULL;
    int pathLen = 0;
    int offset = 0;
    pkt_mhead_t mhead;
    prot_session_info_t* psInfo = NULL;

    psInfo = find_session_info(pHandle);
    mhead.prot_handle = pHandle;
    mhead.media_handle = psInfo->mhandle;
    mhead.type = PKG_PROT_MEDIA_FLIST;

    if((path = json_object_get_string(json_object_object_get(msg_obj, "FILE_PATH"))) == NULL){
        E_LOG("FILE_PATH is empty\n");
        return BU_ERROR;
    }
    pathLen = strlen(path);
    /* ****************pkg prot PKG_PROT_MEDIA_FLIST************** 
        +4B: pathLen
        +NB: path string with length pathLen
        
    	*********************************************************** */
    PKG_INIT_BUFF(sendBuf, sizeof(pkt_mhead_t)+ pathLen + 4);
    PKG_BYTES_MSG(sendBuf, offset, &mhead, sizeof(pkt_mhead_t));
    PKG_UINT32_MSG(sendBuf,offset, pathLen);
    PKG_BYTES_MSG(sendBuf, offset, path, pathLen);
    
    msg_list_push(sendBuf, offset, MEDIA_TASK_ID, PROT_TASK_ID); 
    //msg_list_push((char*)&mhead, sizeof(pkt_mhead_t), CONTRL_TASK_ID); 
    PKG_FREE_BUFF(sendBuf);
    return BU_OK;
}

static BU_INT8 fdown_media_msg_proc(media_handle_t mhandle, prot_handle_t phandle, void* msg, BU_UINT32 msg_len)
{
	BU_UINT32 ulResult = BU_ERROR;
	BU_UINT32 ulOffSet = 0;
	json_object *my_object = json_object_new_object (); 
	const char* tempSend = NULL;
	const char* pBuf = (const char*)msg;
	char* download_url = NULL;
	prot_session_info_t* pInfo = NULL;

	/* ****************pkg prot PKG_PROT_MEDIA_FDOWN_ACK************** 
		+4B: result code 
		
	*********************************************************** */
	UNPKG_UINT32_MSG(pBuf,ulOffSet,ulResult);

	json_object_object_add (my_object, "DID", json_object_new_int (phandle));
	json_object_object_add (my_object, "TYPE", json_object_new_int (PROT_TYPE_FILE_DOWNLOAD));
	json_object_object_add (my_object, "CODE", json_object_new_int (ulResult));
	json_object_object_add (my_object, "URL", json_object_new_string(download_url));

	tempSend = json_object_to_json_string (my_object);// if my_object is null , then the function return string "null"
	if(tempSend != NULL && strcmp(tempSend, "null") != 0){
		pInfo = find_session_info(phandle);
		send_json_str(pInfo->sock, tempSend, strlen(tempSend));
	}
	return BU_OK;
}

static BU_INT8 fcopy_media_msg_proc(media_handle_t mhandle, prot_handle_t phandle, void* msg, BU_UINT32 msg_len)
{
	BU_UINT32 ulResult = BU_ERROR;
	BU_UINT32 ulOffSet = 0;
	json_object *my_object = json_object_new_object (); 
	const char* tempSend = NULL;
	const char* pBuf = (const char*)msg;
	prot_session_info_t* pInfo = NULL;

	/* ****************pkg prot PKG_PROT_MEDIA_FDEL_ACK************** 
		+4B: result code 
		
	*********************************************************** */
	UNPKG_UINT32_MSG(pBuf,ulOffSet,ulResult);

	json_object_object_add (my_object, "DID", json_object_new_int (phandle));
	json_object_object_add (my_object, "TYPE", json_object_new_int (PROT_TYPE_FILE_COPY));
	json_object_object_add (my_object, "CODE", json_object_new_int (ulResult));

	tempSend = json_object_to_json_string (my_object);// if my_object is null , then the function return string "null"
	if(tempSend != NULL && strcmp(tempSend, "null") != 0){
		pInfo = find_session_info(phandle);
		send_json_str(pInfo->sock, tempSend, strlen(tempSend));
	}
	return BU_OK;
}

static BU_INT8 frename_media_msg_proc(media_handle_t mhandle, prot_handle_t phandle, void* msg, BU_UINT32 msg_len)
{
	BU_UINT32 ulResult = BU_ERROR;
	BU_UINT32 ulOffSet = 0;
	json_object *my_object = json_object_new_object (); 
	const char* tempSend = NULL;
	const char* pBuf = (const char*)msg;
	prot_session_info_t* pInfo = NULL;

	/* ****************pkg prot PKG_PROT_MEDIA_FDEL_ACK************** 
		+4B: result code 
		
	*********************************************************** */
	UNPKG_UINT32_MSG(pBuf,ulOffSet,ulResult);

	json_object_object_add (my_object, "DID", json_object_new_int (phandle));
	json_object_object_add (my_object, "TYPE", json_object_new_int (PROT_TYPE_FILE_RENAME));
	json_object_object_add (my_object, "CODE", json_object_new_int (ulResult));

	tempSend = json_object_to_json_string (my_object);// if my_object is null , then the function return string "null"
	if(tempSend != NULL && strcmp(tempSend, "null") != 0){
		pInfo = find_session_info(phandle);
		send_json_str(pInfo->sock, tempSend, strlen(tempSend));
	}
	return BU_OK;
}

static BU_INT8 fnew_media_msg_proc(media_handle_t mhandle, prot_handle_t phandle, void* msg, BU_UINT32 msg_len)
{
	BU_UINT32 ulResult = BU_ERROR;
	BU_UINT32 ulOffSet = 0;
	json_object *my_object = json_object_new_object (); 
	const char* tempSend = NULL;
	const char* pBuf = (const char*)msg;
	prot_session_info_t* pInfo = NULL;

	/* ****************pkg prot PKG_PROT_MEDIA_FDEL_ACK************** 
		+4B: result code 
		
	*********************************************************** */
	UNPKG_UINT32_MSG(pBuf,ulOffSet,ulResult);

	json_object_object_add (my_object, "DID", json_object_new_int (phandle));
	json_object_object_add (my_object, "TYPE", json_object_new_int (PROT_TYPE_FILE_NEW));
	json_object_object_add (my_object, "CODE", json_object_new_int (ulResult));

	tempSend = json_object_to_json_string (my_object);// if my_object is null , then the function return string "null"
	if(tempSend != NULL && strcmp(tempSend, "null") != 0){
		pInfo = find_session_info(phandle);
		send_json_str(pInfo->sock, tempSend, strlen(tempSend));
	}
	return BU_OK;
}


static BU_INT8 fdel_media_msg_proc(media_handle_t mhandle, prot_handle_t phandle, void* msg, BU_UINT32 msg_len)
{
	BU_UINT32 ulResult = BU_ERROR;
	BU_UINT32 ulOffSet = 0;
	json_object *my_object = json_object_new_object ();	
	const char* tempSend = NULL;
	const char* pBuf = (const char*)msg;
	prot_session_info_t* pInfo = NULL;

	/* ****************pkg prot PKG_PROT_MEDIA_FDEL_ACK************** 
		+4B: result code 
		
	*********************************************************** */
	UNPKG_UINT32_MSG(pBuf,ulOffSet,ulResult);

	json_object_object_add (my_object, "DID", json_object_new_int (phandle));
	json_object_object_add (my_object, "TYPE", json_object_new_int (PROT_TYPE_FILE_DELETE));
	json_object_object_add (my_object, "CODE", json_object_new_int (ulResult));

	tempSend = json_object_to_json_string (my_object);// if my_object is null , then the function return string "null"
	if(tempSend != NULL && strcmp(tempSend, "null") != 0){
		pInfo = find_session_info(phandle);
		send_json_str(pInfo->sock, tempSend, strlen(tempSend));
	}
	return BU_OK;
}

static BU_INT8 flist_media_msg_proc(media_handle_t mhandle, prot_handle_t phandle, void* msg, BU_UINT32 msg_len)
{
	BU_UINT32 ulOffSet = 0;
	BU_UINT32 ulFileNum = 0;
	BU_UINT32 i = 0;
	BU_UINT32 ulNameLen = 0;
	BU_UINT32 ulType = 0;
	BU_UINT32 ulSize = 0;
	BU_UINT64 ulTime = 0;
	prot_session_info_t* pInfo = NULL;
	const char* pBuf = (const char*)msg;

	char* name = NULL;
	json_object *my_object = json_object_new_object ();	
    json_object *my_array  = json_object_new_array ();	
    json_object *my_obj_tmp = NULL;
	prot_handle_t tmp_handle = -1;
	int code = BU_OK;
	const char* tempSend = NULL;
	
    /* ****************pkg prot PKG_PROT_MEDIA_FLIST_ACK************** 
        +4B: num
        ---array 1st
        +2B: nameLen
        +NB: name
        +1B: file type
        +4B: size
        +8B: time
        ---
        +NB: path string with length pathLen
        
    	*********************************************************** */
	
	json_object_object_add (my_object, "DID", json_object_new_int (phandle));
	json_object_object_add (my_object, "TYPE", json_object_new_int (PROT_TYPE_FILE_LIST));
	json_object_object_add (my_object, "CODE", json_object_new_int (code));
	
	UNPKG_UINT32_MSG(pBuf, ulOffSet, ulFileNum);
	json_object_object_add (my_object, "NUM", json_object_new_int (ulFileNum));

	for(i=0; i<ulFileNum; i++){
		my_obj_tmp = json_object_new_object ();

		UNPKG_UINT16_MSG(pBuf, ulOffSet, ulNameLen);
		if((name = (char*)calloc(1, ulNameLen+1)) == NULL){
			E_LOG("calloc error\n");
			return BU_ERROR;
		}
		UNPKG_BYTES_MSG(pBuf, ulOffSet, ulNameLen, name);
		json_object_object_add (my_obj_tmp, "NAME", json_object_new_string(name));
		free(name);
		name = NULL;
		
		UNPKG_UINT8_MSG(pBuf, ulOffSet, ulType);
		json_object_object_add (my_obj_tmp, "FILE_TYPE", json_object_new_int(ulType));
		UNPKG_UINT32_MSG(pBuf, ulOffSet, ulSize);
		json_object_object_add (my_obj_tmp, "FILE_SIZE", json_object_new_int(ulSize));
		UNPKG_UINT64_MSG(pBuf, ulOffSet, ulTime);
		json_object_object_add (my_obj_tmp, "FILE_MOD_TIME", json_object_new_int(ulTime));

		json_object_array_add (my_array, my_obj_tmp);
	}
	json_object_object_add (my_object, "DATA", my_array);

	tempSend = json_object_to_json_string (my_object);// if my_object is null , then the function return string "null"
	if(tempSend != NULL && strcmp(tempSend, "null") != 0){
		pInfo = find_session_info(phandle);
		send_json_str(pInfo->sock, tempSend, strlen(tempSend));
	}

	return BU_OK;
}


static BU_INT8 media_msg_proc(void* msg, BU_UINT32 msg_len)
{
    pkt_mhead_t mhead;
    prot_session_info_t* psInfo = NULL;
    if(msg_len < sizeof(pkt_mhead_t) || msg == NULL){
        E_LOG("error input\n");
        return BU_ERROR;
    }

    memcpy(&mhead, msg, sizeof(pkt_mhead_t)); 
    I_LOG("media msg type [%d]\n", mhead.type);

    switch ( mhead.type )
    {
        case PKG_PROT_MEDIA_LOAD_ACK :
            psInfo = find_session_info(mhead.prot_handle);
            if(NULL != psInfo){
                psInfo->mhandle = mhead.media_handle;
            }
            break;
        case PKG_PROT_MEDIA_FLIST_ACK :
			flist_media_msg_proc(mhead.media_handle, mhead.prot_handle, msg + sizeof(pkt_mhead_t), msg_len - sizeof(pkt_mhead_t));
            break;	
		
		case PKG_PROT_MEDIA_FDEL_ACK :
			fdel_media_msg_proc(mhead.media_handle, mhead.prot_handle, msg + sizeof(pkt_mhead_t), msg_len - sizeof(pkt_mhead_t));
			break;
		case PKG_PROT_MEDIA_FNEW_ACK :
			fnew_media_msg_proc(mhead.media_handle, mhead.prot_handle, msg + sizeof(pkt_mhead_t), msg_len - sizeof(pkt_mhead_t));
			break;
		case PKG_PROT_MEDIA_FDOWN_ACK :
			fdown_media_msg_proc(mhead.media_handle, mhead.prot_handle, msg + sizeof(pkt_mhead_t), msg_len - sizeof(pkt_mhead_t));
			break;
		case PKG_PROT_MEDIA_FCOPY_ACK :
			fcopy_media_msg_proc(mhead.media_handle, mhead.prot_handle, msg + sizeof(pkt_mhead_t), msg_len - sizeof(pkt_mhead_t));
			break;
		case PKG_PROT_MEDIA_FRENAME_ACK :
			frename_media_msg_proc(mhead.media_handle, mhead.prot_handle, msg + sizeof(pkt_mhead_t), msg_len - sizeof(pkt_mhead_t));
			break;

        default:
            E_LOG("wrong media msg type\n");
            return BU_ERROR;
    }
        
    return BU_OK;
}
static BU_INT8 net_msg_proc(void* msg, BU_UINT32 msg_len)
{
    json_object *new_obj = NULL;
    json_object *my_object = NULL;
    json_tokener *tok = NULL;
    const char* temp = NULL;
    const char* tempSend = NULL;
    const char* tempName = NULL;
    int tempMode = 0;
    int tempNum = 0;
    void* sock = NULL;
    int msg_type = 0;
    int i = 0;
    int subType = 0;
    int status = 0;
    prot_handle_t pHandle = -1;

    I_LOG("%s\n", (char*)msg+sizeof(void*));
    memcpy(&sock, msg, sizeof(void*));

    tok = json_tokener_new ();
    new_obj = json_tokener_parse_ex (tok, (const char*)msg+sizeof(void*), msg_len-sizeof(void*));
    if (NULL == new_obj) {
        E_LOG("json parse wrong, illicit msg\n");
        return BU_ERROR;
    }

    msg_type = json_object_get_int(json_object_object_get(new_obj, "TYPE"));
    pHandle = json_object_get_int(json_object_object_get(new_obj, "DID"));
    switch ( msg_type )
    {
        case PROT_TYPE_REG :
            
            break;
        case PROT_TYPE_LOAD :
            load_msg_proc(sock, new_obj);
            break;
        case PROT_TYPE_GET_CONFIG :
            break;
        case PROT_TYPE_FILE_NEW :
            file_new_msg_proc(sock, new_obj, pHandle);
            break;
        case PROT_TYPE_FILE_LIST :
            file_list_msg_proc(sock, new_obj, pHandle);
            break;
        case PROT_TYPE_FILE_DELETE :
			file_del_msg_proc(sock, new_obj, pHandle);
            break;
        case PROT_TYPE_FILE_DOWNLOAD :
			file_download_msg_proc(sock, new_obj, pHandle);
            break;
        case PROT_TYPE_FILE_COPY :
			file_copy_msg_proc(sock, new_obj, pHandle);
            break;
        case PROT_TYPE_FILE_RENAME :
			file_rename_msg_proc(sock, new_obj, pHandle);
            break;
        default:
            E_LOG("wrong msg type\n");
            break;
    }
    
    json_object_put(new_obj);
    json_object_put(my_object);

    return BU_OK;
}

void prot_msg_cb(void* msg, BU_UINT32 msg_len, task_id src_id)
{
    switch ( src_id )
    {
        case PROT_NET_TASK_ID:
            net_msg_proc(msg, msg_len);
            break;
        case MEDIA_TASK_ID :
            media_msg_proc(msg, msg_len);
            break;
        case CONTRL_TASK_ID :
            break;
        case TIMER_TASK_ID :
            break;
        case PROT_TASK_ID :
            break;
        default:
            E_LOG("wrong task id\n");
            break;
    }
    return ;
}

