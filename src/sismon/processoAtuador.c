#include "../../lib/mainheader.h"

extern setorSTATUS setor[3];

extern bool exeSismon;

/**** Semáforos ****/

extern sem_t actuatorSem[NS];

/*****************************/

extern int actuatorOrder[NS];      // Indica o comando do intuti ao processo atuador

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