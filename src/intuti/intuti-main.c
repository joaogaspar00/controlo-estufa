#include "../../lib/constantes.h"

/**** Lista de comandos sismon ****/

extern int NCOMMANDS;
extern const CMD commands[];

/**** Variável execução processo INTUTI ****/

extern bool exeIntuti;

/**** Variáveis das SOCKETS ****/

extern int sd_intuti;	// intuti -> sismon
extern int sd_intuti2; 	// intuti -> reghist
extern struct sockaddr_un to_sismon;
extern socklen_t to_sismonlen;
extern struct sockaddr_un to_reghist;
extern socklen_t to_reghistlen;

/**** Protótipos ****/

void closeSocketComunication(int sd, char file[]);
void iniSocketClient (int *sd, char fileIN[], char fileOUT[], struct sockaddr_un *toAddr, socklen_t *toAddrlen);

/***********************************************************************/

int my_getline (char** argv, int argvsize){			// Função que lê os comandos escritos na linha de comando
	static char line[MAX_LINE];
	char *p;
	int argc;

	fgets(line, MAX_LINE, stdin);
	for (argc=0,p=line; (*line != '\0') && (argc < argvsize); p=NULL,argc++){
		p = strtok(p, " \t\n");
		argv[argc] = p;
		if (p == NULL) 
			return argc;
	}
	argv[argc] = p;
	
	return argc;
}

/**********************************************************************/

int main(void){
	char *argv[ARGVECSIZE+1], *p;
	int argc, i;

	iniSocketClient(&sd_intuti, INTUTIS, SISMON, &to_sismon, &to_sismonlen);		// intuti -> sismon
	iniSocketClient(&sd_intuti2, INTUTIR, REGS, &to_reghist, &to_reghistlen);		// intuti -> reghist

	printf("%s Type sos for help\n", titleMSG);
	while(exeIntuti){
		printf("\nCmd> ");
		if((argc = my_getline(argv, ARGVECSIZE)) > 0){		// Lê o comando e argumentos
			for (p=argv[0]; *p != '\0'; *p=tolower(*p), p++);
			for (i = 0; i < NCOMMANDS; i++){
				if (strcmp(argv[0], commands[i].cmd_name) == 0) 
					break;
			}
			if (i < NCOMMANDS){							 		 
				commands[i].cmd_fnct(argc,argv);	
			}
			else{
				printf("%s", invalidMSG);		
			}	
		}
	}

	closeSocketComunication(sd_intuti, INTUTIS); 
	closeSocketComunication(sd_intuti2, INTUTIR);

 	return 0;
}