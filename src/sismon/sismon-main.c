#include "../../lib/mainheader.h"

int Tmax=TMAX,Tmin=TMIN,Hmax=HMAX,Hmin=HMIN; // Define os limites dos alarmes

bool exeSismon = true;

setorSTATUS setor[3] = {
                        {TINI,HINI,CLOSE,OFF,PSEN,PAMB},
                        {TINI,HINI,CLOSE,OFF,PSEN,PAMB},
                        {TINI,HINI,CLOSE,OFF,PSEN,PAMB}
                        };

reg_t registos[3];

/**** Variáveis das SOCKETS ****/

extern int sd_sismon;                   
extern struct sockaddr_un from_intuti;
extern socklen_t from_intutilen;

/**** Protótipos ****/

void closeSocketComunication(int sd, char file[]);
void iniSocketServer (int *sd, char file[]);
void openQueues(int *queueId,char file[]);
void closeQueue(char file[]);

void temphumRequest(int nSetor);
void consultParameters(int nSetor);
void openWindows(int order,int nSetor);
void closeWindows(int order,int nSetor);
void turnOnWater(int order,int nSetor);
void turnOffWater(int order,int nSetor);
void modifySensorPer(int nSetor,int Periodo);
void modifySensorAmb(int nSetor,int Periodo);
void consultLimits();
void defineLimits(int t,int T,int h,int H);
void consultReghistStatus();
void changeReghistStatus(int order);

/**** Semáforos ****/

sem_t actuatorSem[NS];

/*****************************/

int actuatorOrder[NS];      // Indica o comando do intuti ao processo atuador

/*****************************/

bool reghistOpen = false;    //indica se a opcao de envio para o reghist esta ativa (inicialmente nao esta ativa)


/**** Threads ****/

void *processoAmbiente(void * pnSetor);
void *processoAtuador(void * pnSetor);
void *processoSensor(void * pnSetor);

/**********************************************************/

void initialSemaphores(){
    int i;
    
    for(i=0;i<NS;i++){
        if (sem_init(&actuatorSem[i],0,0) != 0){
            perror("Erro na inicializacao dos semaforos\n");
        }   
    }
}

/**********************************************************/

void execRequestINTUTI(int execFunction, int vArguments[]){
    switch(execFunction){       // Através do número do comando o sismon sabe qual é o comando a executar
        case CTH:    // cth
            printf("> Consulta de temperatura e humidade do setor: %d\n", vArguments[0]);
            temphumRequest(vArguments[0]);  
            break;
        case CP: //cp
            printf("> Pedido de consulta de parametros do setor: %d\n", vArguments[0]);
            consultParameters(vArguments[0]);  
            break;
        case AJ:   //aj
            printf("> Pedido de abrir a janela do setor: %d\n", vArguments[0]);
            openWindows(execFunction,vArguments[0]);  
            break;
        case FJ:   //fj
            printf("> Pedido de fechar a janela do setor: %d\n", vArguments[0]);
            closeWindows(execFunction,vArguments[0]);  
            break;
        case LR:   //lr
            printf("> Pedido para ligar a rega do setor: %d\n", vArguments[0]);
            turnOnWater(execFunction,vArguments[0]);  
            break;
        case DR:   //dr
            printf("> Pedido para desligar a rega do setor: %d\n", vArguments[0]);
            turnOffWater(execFunction,vArguments[0]);  
            break;
        case MPS:   //mps
            printf("> Modificar o Periodo do sensor: %d\n", vArguments[0]);
            modifySensorPer(vArguments[0],vArguments[1]);  
            break;
        case MPA:   //mpa
            printf("> Modificar o Periodo do sensor: %d\n", vArguments[0]);
            modifySensorAmb(vArguments[0],vArguments[1]);  
            break;
        case CALA:   //cala
            printf("> Consultar os limiares da temperatura e Humidade\n");
            consultLimits();  
            break;
        case DALA:   //dala
            printf("> Definir os limiares da temperatura e Humidade\n");
            defineLimits(vArguments[0],vArguments[1],vArguments[2],vArguments[3]);  
            break;
        case CER:   //cer
            printf("> Consultar estado de envio dos registos para o historico\n");
            consultReghistStatus();
            break;
        case AER:   //aer
            printf("> Pedido para ativar o envio de registos para o historico\n");
            changeReghistStatus(execFunction);
            break;
        case DER:   //der
            printf("> Pedido para desativar o envio de registos para o historico\n");
            changeReghistStatus(execFunction);
            break;    
        case TSM:  //tsm
            exeSismon = !exeSismon;
            break;
        default:
            printf("> Command not valid\n");
            break;
    }
}

/*********************************************************/

void sighand(){
    exeSismon = !exeSismon;
}

/*********************************************************/

int main (void){
    OBJECT_COMUN object_sismon;        // Estutura que serve para a comunicação entre Intuti e Sismon

    int i;
    pthread_t  threadAmb[3], threadAtua[3], threadSens[3];
    int id[3];

    initialSemaphores(); // inicializa os semáforos

    for (i=0; i<3; i++) {       // Criação das 9 threads
        id[i]=i;
        if (pthread_create(&threadAmb[i], NULL, processoAmbiente, (void *)&id[i]) != 0) {
            printf("Erro a criar thread=%d\n", i);
        }
        if (pthread_create(&threadAtua[i], NULL, processoAtuador, (void *)&id[i]) != 0) {
            printf("Erro a criar thread=%d\n", i);
        }
        if (pthread_create(&threadSens[i], NULL, processoSensor, (void *)&id[i]) != 0) {
            printf("Erro a criar thread=%d\n", i);
        }
    }
    
    iniSocketServer(&sd_sismon, SISMON);                // inicializa a socket para comunicação entre sismon e intuti

    signal(SIGTERM, sighand);

    while(exeSismon){
        from_intutilen = sizeof(from_intuti);
        if (recvfrom(sd_sismon, &object_sismon, sizeof(object_sismon), 0, (struct sockaddr *)&from_intuti, &from_intutilen) < 0){
            perror("Erro a receber do intuti");
            exit(0);
        }
        else{
            execRequestINTUTI(object_sismon.func_number, object_sismon.argv);   // Executa aquilo que o intuti comandou através do número do comando e dos argumentos
        } 
    }

    closeSocketComunication(sd_sismon, SISMON);     // Fecha a socket

    return 0;
}