#include "../../lib/mainheader.h"

void openQueues(int *queueId,char file[]){
    struct mq_attr ma;

    ma.mq_flags = 0;
    ma.mq_maxmsg = 2;
    ma.mq_msgsize = sizeof(reg_t);

     if ((*queueId=mq_open(file, O_RDWR|O_CREAT, 0666, &ma)) < 0) {
        perror("SISMON: Erro a criar queue");
    }
}