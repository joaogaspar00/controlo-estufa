// reghist
#include "../../lib/constantes.h"

/**** Variável execução processo REGHIST ****/

bool exeReghist = true;

/**** Variáveis das SOCKETS ****/

extern int sd_reghist;                   
extern struct sockaddr_un from_intutir;
extern socklen_t from_intutilenr;

/**** Protótipos ****/

void closeSocketComunication(int sd, char file[]);
void iniSocketServer (int *sd, char file[]);

/**** Variáveis das QUEUES ****/

int mqids;

/******************/

reg_t registos[3];

/**********************************************************/

void *comunSismon(){
    int i;

    while(exeReghist){
        if (mq_receive(mqids, (char *)&registos, sizeof(registos), NULL) < 0) {
            perror("REGHIST: erro a receber mensagem");
        }
        
        for(i=0;i<NS;i++){
            printf("Temperatura=%d e Humidade=%d do setor:%d\n",registos[i].t ,registos[i].h ,registos[i].s);
        }
    }

    return 0;
}

/**********************************************************/

void openQueues(){
    struct mq_attr ma;

    ma.mq_flags = 0;
    ma.mq_maxmsg = 2;
    ma.mq_msgsize = sizeof(registos);

    if ((mqids=mq_open(REGQ, O_RDWR|O_CREAT, 0666, &ma)) < 0) {
        perror("REGHIST: Erro a criar queue servidor");
    }
}

/**********************************************************/

void execRequestINTUTI(int execFunction, int vArguments[]){
    switch(execFunction){
        case LREG:
            printf("Listar registos\n");
            break;  
        case TRH:
            exeReghist = !exeReghist;
            break;    
        default:
            printf("> Command not valid\n");
            break;
    }
}

/**********************************************************/

int main (void){
    OBJECT_COMUN object_reghist;        // Estutura que serve para a comunicação entre Intuti e Sismon
    pthread_t  thread;

    iniSocketServer(&sd_reghist, REGS);    // inicializa a socket para comunicação entre sismon e intuti

    if (pthread_create(&thread, NULL, comunSismon,NULL) != 0) {
        printf("Erro a criar thread\n");
        exit(-1);
    }

    openQueues();

    while(exeReghist){
        from_intutilenr = sizeof(from_intutir);
        if (recvfrom(sd_reghist, &object_reghist, sizeof(object_reghist), 0, (struct sockaddr *)&from_intutir, &from_intutilenr) < 0){
            perror("Erro a receber do intuti");
        }
        else{
            execRequestINTUTI(object_reghist.func_number, object_reghist.argv);   // Executa aquilo que o intuti comandou através do número do comando e dos argumentos
        } 
    }

    closeSocketComunication(sd_reghist, REGS);
           
    if (mq_unlink(REGQ) < 0) {
        perror("REGHIST: Erro a eliminar queue servidor (REGHIST)");
    }

    return 0;
}