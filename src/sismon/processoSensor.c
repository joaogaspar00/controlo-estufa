#include "../../lib/mainheader.h"

extern int Tmax,Tmin,Hmax,Hmin;    // Define os limites dos alarmes

extern setorSTATUS setor[3];

extern reg_t registos[3];

extern bool exeSismon;

/*****************************/

extern bool reghistOpen;    //indica se a opcao de envio para o reghist esta ativa (inicialmente nao esta ativa)

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

/**********************************************************
                    PROCESSO SENSOR                                   
**********************************************************/           

void *processoSensor(void * pnSetor){	
    int nSetor = *(int *)pnSetor;
    reg_t reghist_info;            //guarda o valor anterior da memoria local para saber se envia para o reghist
    int queueId;
    pid_t pid;
    
    while(exeSismon){
        reghist_info.t = registos[nSetor].t;  
        reghist_info.h = registos[nSetor].h;

        registos[nSetor].t=setor[nSetor].temperature; 
        registos[nSetor].h=setor[nSetor].humidity;
        registos[nSetor].s=nSetor+1;

        clock_gettime(CLOCK_REALTIME, &registos[nSetor].temp);
 
        printf("Temperatura=%d e Humidade=%d do setor:%d\n",registos[nSetor].t ,registos[nSetor].h ,registos[nSetor].s);    

        if(reghistOpen){
            if((registos[nSetor].t != reghist_info.t) || (registos[nSetor].h != reghist_info.h)){  

                if ((queueId=mq_open(REGQ, O_RDWR)) < 0) {
                    perror("SISMON: Erro a associar a queue REGHIST");
                    printf("A iniciar Reghist\n");

                    if ((pid = fork()) == -1) {			// Criação do processo pai e filho
		                printf("Erro a criar processo\n"); 
                        exit(-1);
	                } 

                    if (pid == 0) {
                        execl("home/controlo-estufa/src/reghist", "controlo-estufa/src/reghist", "./reghist", NULL);
                    }
                    else{
                        printf("Iniciou o Reghist\n");
                    }
                }
                
                if (mq_send(queueId,(char *)&registos[nSetor], sizeof(reg_t), 0) < 0) {
                    perror("SISMON: erro a enviar mensagem");
                }
            }
        }

        warnings(nSetor);
    
        sleep(setor[nSetor].sensorPeriod);
    }
    return 0;
}

