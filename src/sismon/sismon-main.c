// sismon
#include "../../lib/constantes.h"

int Tmax=TMAX,Tmin=TMIN,Hmax=HMAX,Hmin=HMIN; // Define os limites dos alarmes

bool exeSismon = true;

setorSTATUS setor[3] = {
                        {TINI,HINI,CLOSE,OFF,PSEN,PAMB},
                        {TINI,HINI,CLOSE,OFF,PSEN,PAMB},
                        {TINI,HINI,CLOSE,OFF,PSEN,PAMB}
                        };

reg_t registos[3];
//={{,0,TINI,HINI},{,1,TINI,HINI},{,2,TINI,HINI}};

/**** Variáveis das SOCKETS ****/

extern int sd_sismon;                   
extern struct sockaddr_un from_intuti;
extern socklen_t from_intutilen;

/**** Protótipos ****/

void closeSocketComunication(int sd, char file[]);
void iniSocketServer (int *sd, char file[]);
void warnings(int nSetor);

/**** Semáforos ****/

sem_t actuatorSem[NS];

/**** Variáveis das QUEUES ****/

int mqids, mqidc;

/*****************************/

int actuatorOrder[NS];      // Indica o comando do intuti ao processo atuador

/*****************************/

bool reghistOpen = false;    //indica se a opcao de envio para o reghist esta ativa (inicialmente nao esta ativa)

/**********************************************************
                    PROCESSO AMBIENTE
**********************************************************/

void *processoAmbiente(void * pnSetor){		
    int deltaT=0, deltaH=0;
    int countEnviCycles=1, alt=1;
    
    int nSetor = *(int *)pnSetor; //Identificador do setor
    
    while(exeSismon){
        deltaT = (rand() % 3) - 1; 
        deltaH = (rand() % 3) - 1;
        if(countEnviCycles == NCICL){  
            deltaT+=alt;
            deltaH+=alt;
            countEnviCycles=1;
            alt*=-1;
        }
        else{
            deltaT+=alt;
            deltaH+=alt;
            countEnviCycles++;
        }
        deltaT+=setor[nSetor].windowStatus;
        deltaH+=setor[nSetor].waterStatus;
        setor[nSetor].temperature+=deltaT;
        setor[nSetor].humidity+=deltaH;

        printf("> Atualizacao dos valores de temperatura e humidade\n");

        sleep(setor[nSetor].environmentPeriod);
    }   
    return 0;
}

/**********************************************************
                    PROCESSO ATUADOR
**********************************************************/

void *processoAtuador(void * pnSetor){	
    int nSetor = *(int *)pnSetor;
	
    while(exeSismon){
        sem_wait(&actuatorSem[nSetor]);
        switch (actuatorOrder[nSetor]){
            case AJ:
                setor[nSetor].windowStatus = OPEN;
                actuatorOrder[nSetor]=0;        // Desta forma só entra no switch quando o intuti der outra vez a ordem
                break;
            case FJ:
                setor[nSetor].windowStatus = CLOSE;
                actuatorOrder[nSetor]=0; 
                break;
            case LR:
                setor[nSetor].waterStatus = ON;
                actuatorOrder[nSetor]=0; 
                break;
            case DR:
                setor[nSetor].waterStatus = OFF;
                actuatorOrder[nSetor]=0; 
                break;
            default:
                break;
        }
    }

    return 0;
}

/**********************************************************
                    PROCESSO SENSOR                                   
**********************************************************/           

void *processoSensor(void * pnSetor){	
    int nSetor = *(int *)pnSetor;
    reg_t reghist_info;            //guarda o valor anterior da memoria local para saber se envia para o reghist

    /*time_t t;
    struct tm tm;
    char str[26];*/

    reghist_info.t = registos[nSetor].t;
    reghist_info.h = registos[nSetor].h;
    
    while(exeSismon){
        registos[nSetor].t=setor[nSetor].temperature;
        registos[nSetor].h=setor[nSetor].humidity;
        registos[nSetor].s=nSetor+1;

       /* time(&t);
        localtime_r(&t, &tm);
        asctime_r(&tm, &str[0]);
        strftime(&str[0], sizeof(str), "%d/%m/%Y %H:%M:%S\n", &tm);*/
 
        printf("Temperatura=%d e Humidade=%d do setor:%d\n",registos[nSetor].t ,registos[nSetor].h ,registos[nSetor].s);

        if(reghistOpen){
            if((registos[nSetor].t != reghist_info.t) || (registos[nSetor].h != reghist_info.h)){

                if ((mqids=mq_open(REGQ, O_RDWR)) < 0) {
                    perror("SISMON: Erro a associar a queue REGHIST (iniciar Reghist)");
                }
                
                if (mq_send(mqids, (char *)&registos, sizeof(registos), 0) < 0) {
                    perror("SISMON: erro a enviar mensagem");
                }
            }
        }

        warnings(nSetor);
    
        sleep(setor[nSetor].sensorPeriod);
    }
    return 0;
}

/**********************************************************/
void openQueues(){
    struct mq_attr ma;

    ma.mq_flags = 0;
    ma.mq_maxmsg = 2;
    ma.mq_msgsize = sizeof(registos);

     if ((mqidc=mq_open(SISM, O_RDWR|O_CREAT, 0666, &ma)) < 0) {
        perror("SISMON: Erro a criar queue");
    }

}

/**********************************************************/

void warnings(int nSetor){
    if(setor[nSetor].temperature>=Tmax){
        printf("O setor %d atingiu a temperatura maxima (%d)\n",nSetor+1,Tmax);
    }
    else if(setor[nSetor].temperature<=Tmin){
        printf("O setor %d atingiu a temperatura minima (%d)\n",nSetor+1,Tmin);
    }
    else if(setor[nSetor].humidity>=Hmax){
        printf("O setor %d atingiu a humidade maxima (%d)\n",nSetor+1,Hmax);
    }
    else if(setor[nSetor].humidity<=Hmin){
        printf("O setor %d atingiu a humidade minima (%d)\n",nSetor+1,Hmin);
    }
}

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
void temphumRequest(int nSetor){        //cth
    char MSG[MAX_LINE];

    if (nSetor==0){     // Se o argumento for 0 teremos que consultar todos os setores e enviar para o intuti
        sprintf(MSG,">Setor 1 -> Temp=%d e Hum=%d \n>Setor 2 -> Temp=%d e Hum=%d \n>Setor 3 -> Temp=%d e Hum=%d \n",
                registos[0].t,registos[0].h,registos[1].t,registos[2].h,registos[2].t,registos[2].h);
    }
    else{
        sprintf(MSG,">Setor %d -> Temp=%d e Hum=%d \n",nSetor,registos[nSetor-1].t,registos[nSetor-1].h);
    }

    if (sendto(sd_sismon, MSG, sizeof(MSG), 0, (struct sockaddr *)&from_intuti, from_intutilen) < 0){
		perror("Erro ao enviar para intuti");
    }
}

/**********************************************************/

void consultParameters(int nSetor){     //cp
    char MSG[MAX_LINE];

    if (nSetor==0){     // Se o argumento for 0 teremos que consultar todos os setores e enviar para o intuti
        sprintf(MSG,">Setor 1 -> Per.Sensor=%d Per.Ambiente=%d e Janela=%d e Rega=%d\n>Setor 2 -> Per.Sensor=%d Per.Ambiente=%d e Janela=%d e Rega=%d\n>Setor 3 -> Per.Sensor=%d Per.Ambiente=%d e Janela=%d e Rega=%d ",
                setor[0].sensorPeriod,setor[0].environmentPeriod,setor[0].windowStatus,setor[0].waterStatus,
                setor[1].sensorPeriod,setor[1].environmentPeriod,setor[1].windowStatus,setor[1].waterStatus,
                setor[2].sensorPeriod,setor[2].environmentPeriod,setor[2].windowStatus,setor[2].waterStatus);
    }
    else{
        sprintf(MSG,">Setor %d -> Per.Sensor=%d Per.Ambiente=%d e Janela=%d e Rega=%d\n",
                    nSetor,setor[nSetor-1].sensorPeriod,setor[nSetor-1].environmentPeriod,setor[nSetor].windowStatus,setor[nSetor].waterStatus);
    }

    if (sendto(sd_sismon, MSG, sizeof(MSG), 0, (struct sockaddr *)&from_intuti, from_intutilen) < 0){
		perror("Erro ao enviar para intuti");
    }
}

/**********************************************************/

void openWindows(int order,int nSetor){ //aj
    int i;
    char MSG[MAX_LINE];	
    
    if(nSetor == 0){
        for(i=0; i<NS; i++){
            actuatorOrder[i] = order;
            sem_post(&actuatorSem[i]);
        }
        sprintf(MSG,"> As janelas de todos os setores foram abertas\n");
    }
    else {
        actuatorOrder[nSetor-1] = order;
        sem_post(&actuatorSem[nSetor-1]);
        sprintf(MSG,"> As janelas do setor %d foram abertas\n",nSetor);
    }

    if (sendto(sd_sismon, MSG, sizeof(MSG), 0, (struct sockaddr *)&from_intuti, from_intutilen) < 0){
        perror("Erro ao enviar para intuti");
    }
}

/**********************************************************/

void closeWindows(int order,int nSetor){ //fj
    int i;
    char MSG[MAX_LINE];	
    
    if(nSetor == 0){
        for(i=0; i<NS; i++){
            actuatorOrder[i] = order;
            sem_post(&actuatorSem[i]);
        }
        sprintf(MSG,"> As janelas de todos os setores foram fechadas\n");
    }
    else {
        actuatorOrder[nSetor-1] = order;
        sem_post(&actuatorSem[nSetor-1]);
        sprintf(MSG,"> As janelas do setor %d foram fechadas\n",nSetor);
    }
    
    if (sendto(sd_sismon, MSG, sizeof(MSG), 0, (struct sockaddr *)&from_intuti, from_intutilen) < 0){
        perror("Erro ao enviar para intuti");
    }
}

/**********************************************************/

void turnOnWater(int order,int nSetor){ //lr
    int i;
    char MSG[MAX_LINE];	
    
    if(nSetor == 0){
        for(i=0; i<NS; i++){
            actuatorOrder[i] = order;
            sem_post(&actuatorSem[i]);
        }
        sprintf(MSG,"> A rega de todos os setores foi ligada\n");
    }
    else {
        actuatorOrder[nSetor-1] = order;
        sem_post(&actuatorSem[nSetor-1]);
        sprintf(MSG,"> A rega do setor %d foi ligada\n",nSetor);
    }
    
    if (sendto(sd_sismon, MSG, sizeof(MSG), 0, (struct sockaddr *)&from_intuti, from_intutilen) < 0){
        perror("Erro ao enviar para intuti");
    }
}

/**********************************************************/

void turnOffWater(int order,int nSetor){ //dr
    int i;
    char MSG[MAX_LINE];	
    
    if(nSetor == 0){
        for(i=0; i<NS; i++){
            actuatorOrder[i] = order;
            sem_post(&actuatorSem[i]);
        }
        sprintf(MSG,"> A rega de todos os setores foi desligada\n");
    }
    else {
        actuatorOrder[nSetor-1] = order;
        sem_post(&actuatorSem[nSetor-1]);
        sprintf(MSG,"> A rega do setor %d foi desligada\n",nSetor);
    }
    
    if (sendto(sd_sismon, MSG, sizeof(MSG), 0, (struct sockaddr *)&from_intuti, from_intutilen) < 0){
        perror("Erro ao enviar para intuti");
    }
}

/**********************************************************/

void modifySensorPer(int nSetor,int Periodo){   //mps
    char MSG[MAX_LINE];
    int i;

    if (nSetor==0){     // Se o argumento for 0 teremos que consultar todos os setores e enviar para o intuti
        for(i=0;i<NS;i++){
            setor[i].sensorPeriod=Periodo;
        }
        sprintf(MSG,"O periodo sensor de todos os setores foram modificados\n");
    }
    else{
        setor[nSetor-1].sensorPeriod=Periodo; 
        sprintf(MSG,"O periodo sensor do setor %d foi modificado\n",nSetor);
    }

    if (sendto(sd_sismon, MSG, sizeof(MSG), 0, (struct sockaddr *)&from_intuti, from_intutilen) < 0){
		perror("Erro ao enviar para intuti");
    }
}

/**********************************************************/

void modifySensorAmb(int nSetor,int Periodo){   //mpa
    char MSG[MAX_LINE];
    int i;

    if (nSetor==0){     // Se o argumento for 0 teremos que consultar todos os setores e enviar para o intuti
        for(i=0;i<NS;i++){
            setor[i].environmentPeriod=Periodo;
        }
        sprintf(MSG,"O periodo ambiente de todos os setores foram modificados\n");
    }
    else{
        setor[nSetor-1].environmentPeriod=Periodo; 
        sprintf(MSG,"O periodo ambiente do setor %d foi modificado\n",nSetor);
    }

    if (sendto(sd_sismon, MSG, sizeof(MSG), 0, (struct sockaddr *)&from_intuti, from_intutilen) < 0){
		perror("Erro ao enviar para intuti");
    }
}

/**********************************************************/

void consultLimits(){       //cala
    char MSG[MAX_LINE];
    
    sprintf(MSG,"Temp.Max=%d Temp.Min=%d Hum.max=%d Hum.Min=%d\n",Tmax,Tmin,Hmax,Hmin);

    if (sendto(sd_sismon, MSG, sizeof(MSG), 0, (struct sockaddr *)&from_intuti, from_intutilen) < 0){
		perror("Erro ao enviar para intuti");
    }
}

/**********************************************************/
void defineLimits(int t,int T,int h,int H){  //dala
    char MSG[MAX_LINE];

    Tmin=t;
    Tmax=T;
    Hmin=h;
    Hmax=H;

    sprintf(MSG,"Novos limites -> Temp.Max=%d Temp.Min=%d Hum.max=%d Hum.Min=%d\n",Tmax,Tmin,Hmax,Hmin);

    if (sendto(sd_sismon, MSG, sizeof(MSG), 0, (struct sockaddr *)&from_intuti, from_intutilen) < 0){
		perror("Erro ao enviar para intuti");
    }
}

/**********************************************************/

void consultReghistStatus(){    //cer
    char MSG[MAX_LINE];

    if(reghistOpen){
        sprintf(MSG,"O envio de registos para o historico esta ativado\n");
    }
    else{
        sprintf(MSG,"O envio de registos para o historico esta desativado\n");
    }

    if (sendto(sd_sismon, MSG, sizeof(MSG), 0, (struct sockaddr *)&from_intuti, from_intutilen) < 0){
		perror("Erro ao enviar para intuti");
    }

}

/**********************************************************/

void changeReghistStatus(int order){    //aer e der
    char MSG[MAX_LINE];

    switch (order){
        case AER:
            reghistOpen=true;
            sprintf(MSG,"O envio de registos para o historico foi ativado\n");
            break;
        case DER:
            reghistOpen=false;
            sprintf(MSG,"O envio de registos para o historico foi desativado\n");
            break;
        default:
            break;
    }

    if (sendto(sd_sismon, MSG, sizeof(MSG), 0, (struct sockaddr *)&from_intuti, from_intutilen) < 0){
		perror("Erro ao enviar para intuti");
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
    
    iniSocketServer(&sd_sismon, SISMON);    // inicializa a socket para comunicação entre sismon e intuti
    openQueues();                           // Cria a queue do lado do Sismon

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

    if (mq_unlink(SISM) < 0) {                      // Fecha a queue
        perror("SISMON: Erro a eliminar queue cliente (SISMON)");
    }

    return 0;
}