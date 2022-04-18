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
void closeQueue(char file[]);

/**** Variáveis das QUEUES ****/

int mqids;

int numRegist=0;

/**********************************************************/

void saveToFile(reg_t *pa, reg_t registos){
    /* aceder ao ficheiro através da memória */
    if(numRegist==NREG){
        numRegist=0;
    }
    pa[numRegist] = registos;
    numRegist++;
}

/**********************************************************/

void *comunSismon(){
    reg_t registos;
    int mfd;
    reg_t *pa;

    if ((mfd=open(DADOS, O_RDWR|O_CREAT, 0666 )) < 0) {  /* abrir / criar ficheiro */
        perror("Erro a criar ficheiro");
        exit(-1);
    }
    else {
        if (ftruncate(mfd, NREG*sizeof(reg_t)) < 0) {            /* definir tamanho do ficheiro */
            perror("Erro no ftruncate");
            exit(-1);
        }
    }
    
    /* mapear ficheiro */
    if ((pa=(reg_t *) mmap(NULL,NREG*sizeof(reg_t), PROT_READ|PROT_WRITE, MAP_SHARED, mfd, 0)) < (reg_t *)0) {
        perror("Erro em mmap");
        exit(-1);
    }

    while(exeReghist){
        if (mq_receive(mqids,(char *)&registos, sizeof(reg_t), NULL) < 0) {
            perror("REGHIST: erro a receber mensagem");
        }


        saveToFile(pa, registos);
    }


    munmap(pa,MAX_LINE);
    close(mfd);

    return 0;
}

/**********************************************************/

void readReghists(){
    int mfd;
    reg_t registos;

    struct tm tm;
    char str[26];
    reg_t *pa;
    char MSG[MAX_LINE];
    int i;
        

    if ((mfd=open(DADOS, O_RDWR, 0666 )) < 0) {  /* abrir ficheiro*/
        perror("Erro a criar ficheiro");
        exit(-1);
    }
    else {
        if (ftruncate(mfd, MAX_LINE*NREG) < 0) {          
            perror("Erro no ftruncate");
            exit(-1);
        }
    }

    if ((pa=(reg_t *) mmap(NULL,NREG*sizeof(reg_t), PROT_READ|PROT_WRITE, MAP_SHARED, mfd, 0)) < (reg_t *)0) {
        perror("Erro em mmap");
        exit(-1);
    }

    for(i=0;i<NREG;i++){
        registos=pa[i];
        
        localtime_r(&registos.temp.tv_sec, &tm);  
        strftime(&str[0], sizeof(str), "%d/%m/%Y %H:%M:%S", &tm);
        sprintf(MSG,"%s ->Temperatura=%d e Humidade=%d do setor:%d\n",str,registos.t ,registos.h ,registos.s);
        printf("%s",MSG);
    }

    munmap(pa,MAX_LINE);
    close(mfd);


}

/**********************************************************/

void execRequestINTUTI(int execFunction, int vArguments[]){
    switch(execFunction){
        case LREG:
            printf("Listar registos\n");
            readReghists();
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
           
    closeQueue(REGQ);
    
    return 0;
}