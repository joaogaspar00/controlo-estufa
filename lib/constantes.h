#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <pthread.h>
#include <stdbool.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>


#define NS 3 			/* numero de sectores a controlar */
#define PSEN 5 			/* per´ıodo inicial dos processos sensores (em segundos) */
#define PAMB 10			/* per´ıodo inicial dos processos ambiente (em segundos) */
#define TINI 25 		/* temperatura inicial dos sectores (em graus) */
#define TMIN -100 		/* temperatura minima inicial para alarme (em graus) */
#define TMAX +100 		/* temperatura maxima inicial para alarme (em graus) */
#define HINI 50 		/* humidade inicial dos sectores (em %) */
#define HMIN 0 			/* humidade minima inicial para alarme (em %) */
#define HMAX 100 		/* humidade maxima inicial para alarme (em %) */
#define NCICL 12 		/* numero de ciclos para alternancia ambiente */

#define NREG 300 				/* numero maximo de registos no ficheiro */
#define DADOS "HISTORICO.LOG" 	/* ficheiro com registo historico */

#define SISMON "/tmp/SISMON" 	/* nome do sistema de monitorização (socket) */
#define REGS "/tmp/REG" 		/* nome do registo historico (socket) */
#define INTUTIS "/tmp/INTUTIS" 	/* nome do INTUTI (socket intuti->sismon) */
#define INTUTIR "/tmp/INTUTIR" 	/* nome do INTUTI (socket intuti->reghist) */
#define REGQ "/REGQ" 			/* nome do registo historico (queue) */
#define SISM "/SISM" 			/* nome do sistema de monitorização (queue) */


#define ARGVECSIZE 5		/* Número máximo de argumentos que podem ser lidos no intuti (nome do comando mais 4 argumentos)*/
#define MAX_LINE  200

//defines para as janelas
#define CLOSE 1			// Inicialmente as janelas estão fechadas
#define OPEN -1	

//defines para a rega
#define ON 1							
#define OFF -1			// Inicialmente a rega está desligada

#define invalidMSG "Invalid command!\n"
#define titleMSG "\n Application Control Monitor\n"

/***********************************************************************/

enum CDMLIST{CTH, CP, AJ, FJ, LR, DR, MPS, MPA, CALA, DALA, CER, AER, DER, TSM, LREG, TRH, SOS, SAIR};

/***********************************************************************/

typedef struct command_d {				// Estrutura que contém todos os comandos
	void  (*cmd_fnct)(int, char**);		// Função que irá realizar o que foi comandado
	char*	cmd_name;					// Nome do comando	
	char*	cmd_help;					// Descrição do comando
} CMD;

typedef struct object_comun{		// Estrutura que será utilizada para passar os comandos e argumentos pelos socket (intuti->sismon)
	int func_number;				// Posição de cada comando
	int argc;						// Número de argumentos
	int argv[ARGVECSIZE];			// Os argumentos
}OBJECT_COMUN;

typedef struct status{
	int temperature;				// Temperatura do setor
	int humidity;					// Humidade do setor
	int windowStatus;				// Indicação do estado da janelas (Aberto ou Fechado)
	int	waterStatus;				// Indicação do estado da rega (Ligada ou Desligada)
	int sensorPeriod;				// Periodo do processo sensor
	int environmentPeriod;			// Periodo do processo ambiente
}setorSTATUS;

typedef struct reg_s { 			/* estrutura de um registo */
	struct timespec temp; 		/* estampilha temporal */
	int s; 						/* identificador do sector */
	int t; 						/* valor da temperatura */
	int h; 						/* valor da humidade */
} reg_t;