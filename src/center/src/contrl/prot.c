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
        printf("The peer had shutdown\n");
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
    msg_list_push(buf, buf_len, PROT_TASK_ID); 
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
    msg_list_push(sendBuf, msg_len+sizeof(type)+sizeof(prot_handle_t), CONTRL_TASK_ID); 

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

    msg_list_push((char*)&mhead, sizeof(pkt_mhead_t), MEDIA_TASK_ID); 
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

void load_msg_proc(void* sock, json_object* msg_obj)
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
    
        code = BU_ERROR;
    }
    
    json_object_object_add (my_object, "DID", json_object_new_int (tmp_handle));
    json_object_object_add (my_object, "TYPE", json_object_new_int (PROT_TYPE_LOAD));
    json_object_object_add (my_object, "CODE", json_object_new_int (code));

    tempSend = json_object_to_json_string (my_object);// if my_object is null , then the function return string "null"
    if(tempSend != NULL && strcmp(tempSend, "null") != 0){
        send_json_str(sock, tempSend, strlen(tempSend));
    }

    return;
}

void prot_msg_cb(void* msg, BU_UINT32 msg_len)
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

    I_LOG("%s\n", (char*)msg+sizeof(void*));
    memcpy(&sock, msg, sizeof(void*));

    tok = json_tokener_new ();
    new_obj = json_tokener_parse_ex (tok, (const char*)msg+sizeof(void*), msg_len-sizeof(void*));
    if (NULL == new_obj) {
        E_LOG("json parse wrong, illicit msg\n");
        return;
    }

    msg_type = json_object_get_int(json_object_object_get(new_obj, "TYPE"));

    switch ( msg_type )
    {
        case PROT_TYPE_REG :
            
            break;
        case PROT_TYPE_LOAD :
            load_msg_proc(sock, new_obj);
            break;
        case PROT_TYPE_GET_CONFIG :
            break;
        case PROT_TYPE_FILE_ADD :
            break;
        case PROT_TYPE_FILE_LIST :
            break;
        case PROT_TYPE_FILE_DELETE :
            break;
        case PROT_TYPE_FILE_DOWNLOAD :
            break;
        case PROT_TYPE_FILE_COPY :
            break;
        case PROT_TYPE_FILE_RENAME :
            break;
        default:
            E_LOG("wrong msg type\n");
            break;
    }
    
    json_object_put(new_obj);
    json_object_put(my_object);

    return ;
}

