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