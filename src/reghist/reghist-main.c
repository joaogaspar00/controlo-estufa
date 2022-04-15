#include "../../lib/mainheader.h"

/**** Variável execução processo REGHIST ****/

bool exeReghist = true;

/**** Variáveis das SOCKETS ****/

extern int sd_reghist;                   
extern struct sockaddr_un from_intutir;
extern socklen_t from_intutilenr;

/**** Protótipos ****/

void closeSocketComunication(int sd, char file[]);
void iniSocketServer (int *sd, char file[]);

void openQueues(int *queueId,char file[]);

/**** Variáveis das QUEUES ****/

int mqids;

/**********************************************************/

void *comunSismon(){
    reg_t registos;
    
    char *pa;
    char MSG[MAX_LINE];

    int mfd;

    if ((mfd=open(DADOS, O_RDWR|O_CREAT, 0666 )) < 0) {  /* abrir / criar ficheiro */
        perror("Erro a criar ficheiro");
        exit(-1);
    }
    else {
        if (ftruncate(mfd, MAX_LINE) < 0) {            /* definir tamanho do ficheiro */
            perror("Erro no ftruncate");
            exit(-1);
        }
    }
    
    /* mapear ficheiro */
    if ((pa=mmap(NULL,MAX_LINE, PROT_READ|PROT_WRITE, MAP_SHARED, mfd, 0)) < (char *)0) {
        perror("Erro em mmap");
        exit(-1);
    }

    while(exeReghist){
        if (mq_receive(mqids, (char *)&registos, sizeof(reg_t), NULL) < 0) {
            perror("REGHIST: erro a receber mensagem");
        }
        
        printf("Temperatura=%d e Humidade=%d do setor:%d\n",registos.t ,registos.h ,registos.s);

        sprintf(MSG,"Temperatura=%d e Humidade=%d do setor:%d\n",registos.t ,registos.h ,registos.s);

        
       /* aceder ao ficheiro através da memória */
        strncpy(pa,"MSG de teste\n",strlen("MSG de teste\n"));
        strncpy(pa + strlen("MSG de teste\n"),"MSG de teste777\n",strlen("MSG de teste777\n"));
    }

    munmap(pa,MAX_LINE);
    close(mfd);

    return 0;
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

    openQueues(&mqids,REGQ);

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