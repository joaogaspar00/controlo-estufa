#define NS 3 			/* número de sectores a controlar */
#define PSEN 5 			/* período inicial dos processos sensores (em segundos) */
#define PAMB 10			/* período inicial dos processos ambiente (em segundos) */
#define TINI 25 		/* temperatura inicial dos sectores (em graus) */
#define TMIN -100 		/* temperatura minima inicial para alarme (em graus) */
#define TMAX +100 		/* temperatura maxima inicial para alarme (em graus) */
#define HINI 50 		/* humidade inicial dos sectores (em %) */
#define HMIN 0 			/* humidade minima inicial para alarme (em %) */
#define HMAX 100 		/* humidade maxima inicial para alarme (em %) */
#define NCICL 12 		/* numero de ciclos para alternancia ambiente */

#define NREG 300 				/* número máximo de registos no ficheiro */
#define DADOS "HISTORICO.LOG" 	/* ficheiro com registo historico */

#define SISMON "/tmp/SISMON" 	/* nome do sistema de monitorização (socket) */
#define REGS "/tmp/REG" 		/* nome do registo historico (socket) */
#define INTUTIS "/tmp/INTUTIS" 	/* nome do INTUTI (socket intuti->sismon) */
#define INTUTIR "/tmp/INTUTIR" 	/* nome do INTUTI (socket intuti->reghist) */
#define REGQ "/REGQ" 			/* nome do registo historico (queue) */

#define ARGVECSIZE 6	/* Número máximo de argumentos que podem ser lidos no intuti (nome do comando mais 4 argumentos)*/
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

