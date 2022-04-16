#include "../../lib/mainheader.h"

extern setorSTATUS setor[3];

extern bool exeSismon;

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