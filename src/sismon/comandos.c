#include "../../lib/mainheader.h"

extern int Tmax,Tmin,Hmax,Hmin;    // Define os limites dos alarmes

extern setorSTATUS setor[3];

extern reg_t registos[3];

/**** Variáveis das SOCKETS ****/

extern int sd_sismon;                   
extern struct sockaddr_un from_intuti;
extern socklen_t from_intutilen;

/**** Semáforos ****/

extern sem_t actuatorSem[NS];

/*****************************/

extern int actuatorOrder[NS];      // Indica o comando do intuti ao processo atuador

/*****************************/

extern bool reghistOpen;    //indica se a opcao de envio para o reghist esta ativa (inicialmente nao esta ativa)

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

void openWindows(int order,int nSetor){     //aj
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

void closeWindows(int order,int nSetor){    //fj
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

void turnOnWater(int order,int nSetor){     //lr
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

void turnOffWater(int order,int nSetor){        //dr
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
        sprintf(MSG,"O envio de registos para o historico esta ativo\n");
    }
    else{
        sprintf(MSG,"O envio de registos para o historico esta desativo\n");
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