#include "contrl.h"

static task_info_t s_task_info[16] = {
    {"task_contrl", 0, (void*)contrl_main, 0},
    {"task_media", 0, (void*)media_main, 0}

};

int config_tasks()
{
    return BU_OK;
}
int main(int argc, char** argv)
{
    printf("Are you Ready? start my butler!!!!\n");



    return 0;
}
