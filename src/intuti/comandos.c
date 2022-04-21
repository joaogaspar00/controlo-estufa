#include "../../lib/mainheader.h"

void cmd_cth (int, char** );
void cmd_cp (int, char** );
void cmd_aj (int, char** );
void cmd_fj (int, char** );
void cmd_lr (int, char** );
void cmd_dr (int, char** );
void cmd_mps (int, char** );
void cmd_mpa (int, char** );
void cmd_cala (int, char** );
void cmd_dala (int, char** );
void cmd_cer (int, char** );
void cmd_aer (int, char** );
void cmd_der (int, char** );
void cmd_tsm (int, char** );
void cmd_lreg (int, char** );
void cmd_trh (int, char** );
void cmd_sos (int, char** ); 
void cmd_sair (int, char** );

const CMD commands[] = {
	{cmd_cth, "cth"," <s>  		consultar temperatura e humidade do setor s (0-todos)"},
	{cmd_cp, "cp"," <s>  		consultar parametros (periodos, estado janelas/rega) setor s (0-todos)"},
	{cmd_aj, "aj"," <s>  		abrir janelas do setor s (0-todos)"},
	{cmd_fj, "fj"," <s>  		fechar janelas do setor s (0-todos)"},
	{cmd_lr, "lr"," <s>  		ligar rega do setor s (0-todos)"},
	{cmd_dr, "dr"," <s>  		desligar a rega do setor s (0-todos)"},
	{cmd_mps, "mps"," <s> <p>  		modificar periodo proc.sensor do setor s (0-todos) (segundos)"},
	{cmd_mpa, "mpa"," <s> <p> 		modificar periodo proc.ambiente do setor s (0-todos) (segundos)"},
	{cmd_cala, "cala","  			consultar limiares alarme temperatura/humidade (minimos e maximos)"},
	{cmd_dala, "dala"," <t> <T> <h> <H>   definir limiares temperatura/humidade, min (t,h) e max (T,H)"},
	{cmd_cer, "cer","  			consultar estado envio de registos para o historico"},
	{cmd_aer, "aer","  			ativar o envio de registos para o histórico"},
	{cmd_der, "der","  			desativar o envio de registos para o histórico"},
	{cmd_tsm, "tsm","  			terminar processo sistema de monitorizacao (sismon)"},
	{cmd_lreg, "lreg","<s> <[t1 [t2]]>  	listar registos setor s (0-todos) entre t1 (dd/mm/aaaa hh:mm:ss) e t2"},
	{cmd_trh,"trh","   			terminar processo de registo historico (reghist)"},
	{cmd_sos,  "sos","          		help"},
	{cmd_sair, "sair"," 			sair"}
};

int NCOMMANDS=(sizeof(commands)/sizeof(struct command_d));

/**** Variável execução processo INTUTI ****/

bool exeIntuti = true;

/**** Variáveis SOCKETS INTUTI ****/

extern int sd_intuti;
extern int sd_intuti2;

extern struct sockaddr_un addr_intuti;
extern socklen_t addrlen_intuti;

extern struct sockaddr_un to_sismon;
extern socklen_t to_sismonlen;

extern struct sockaddr_un to_reghist;
extern socklen_t to_reghistlen;

/***********************************************************************/

OBJECT_COMUN creatComunicationObject(int func_number, int argc, char *argv[]){		// Função que prepara aquilo que queremos enviar para o sismon
	int i;
	OBJECT_COMUN x;

	x.func_number=func_number;
	x.argc = argc-1;			// Retira a primeira posição do argv que veio do intuti-main (normalmente é o nome do comando)
	for(i=0; i<argc-1;i++) {
		x.argv[i]=atoi(argv[i+1]);
	}
	return x;
}

/***********************************************************************/

void cmd_cth (int argc, char** argv)
{
	int nSetor;
	OBJECT_COMUN objectTo_sismon;

	char MSG[MAX_LINE];

	if(argc == 2){		// Para o cth só é necessário o nome do comando e 1 argumento (nSetor)
		nSetor = atoi(argv[1]);
		if(nSetor <= 3 && nSetor >= 0){
			/* envia para o sismon */
			objectTo_sismon = creatComunicationObject(CTH, argc, argv);
			if (sendto(sd_intuti, &objectTo_sismon, sizeof(objectTo_sismon), 0, (struct sockaddr *)&to_sismon, to_sismonlen) < 0) {
				perror("Erro ao enviar para sismon");
			}
			else{
				printf("> Pedido de temperatura e humidade do setor %d\n", nSetor);
			}

			/* recebe do sismon */
			if (recvfrom(sd_intuti, &MSG, sizeof(MSG), 0, (struct sockaddr *)&to_sismon, &to_sismonlen) < 0){
            	perror("Erro a receber do sismon");
    		}
			else{
				printf("%s\n",MSG);
			}
		}
		else {	// Caso o argumento passado for diferente de 0 a 3
			printf("> Erro no numero do setor [valores válidos 0-3]\n");
		}
	}	
	else{
		printf("> Erro no numero de argumentos\n");
	}
}

/***********************************************************************/

void cmd_cp (int argc, char** argv){	
	int nSetor;
	OBJECT_COMUN objectTo_sismon;

	char MSG[MAX_LINE];

	if(argc == 2){		
		nSetor = atoi(argv[1]);
		if(nSetor <= 3 && nSetor >= 0){
			/* envia para o sismon */
			objectTo_sismon = creatComunicationObject(CP, argc, argv);
			if (sendto(sd_intuti, &objectTo_sismon, sizeof(objectTo_sismon), 0, (struct sockaddr *)&to_sismon, to_sismonlen) < 0) {
				perror("Erro ao enviar para sismon");
			}
			else{
				printf("> Consulta de parametros do setor %d\n", nSetor);
			}

			/* recebe do sismon */
			if (recvfrom(sd_intuti, &MSG, sizeof(MSG), 0, (struct sockaddr *)&to_sismon, &to_sismonlen) < 0){
            	perror("Erro a receber do sismon");
    		}
			else{
				printf("%s \n",MSG);
				printf("Janelas (1-Fechado -1-Aberto) Rega (1-Ligado -1-Desligado)\n");
			}
		}
		else {	// Caso o argumento passado for diferente de 0 a 3
			printf("> Erro no numero do setor [valores válidos 0-3]\n");
		}
	}	
	else{
		printf("> Erro no numero de argumentos\n");
	}
}

/***********************************************************************/

void cmd_aj (int argc, char** argv){
	int nSetor;
	OBJECT_COMUN objectTo_sismon;

	char MSG[MAX_LINE];

	if(argc == 2){		
		nSetor = atoi(argv[1]);
		if(nSetor <= 3 && nSetor >= 0){
			/* envia para o sismon */
			objectTo_sismon = creatComunicationObject(AJ, argc, argv);
			if (sendto(sd_intuti, &objectTo_sismon, sizeof(objectTo_sismon), 0, (struct sockaddr *)&to_sismon, to_sismonlen) < 0) {
				perror("Erro ao enviar para sismon");
			}
			else{
				printf("> Abrir as janelas do setor %d\n", nSetor);
			}

			/* recebe do sismon */
			if (recvfrom(sd_intuti, &MSG, sizeof(MSG), 0, (struct sockaddr *)&to_sismon, &to_sismonlen) < 0){
            	perror("Erro a receber do sismon");
    		}
			else{
				printf("%s\n",MSG);
			}
		}
		else {	// Caso o argumento passado for diferente de 0 a 3
			printf("> Erro no numero do setor [valores válidos 0-3]\n");
		}
	}	
	else{
		printf("> Erro no numero de argumentos\n");
	}
}

/***********************************************************************/

void cmd_fj (int argc, char** argv){
	int nSetor;
	OBJECT_COMUN objectTo_sismon;

	char MSG[MAX_LINE];

	if(argc == 2){		
		nSetor = atoi(argv[1]);
		if(nSetor <= 3 && nSetor >= 0){
			/* envia para o sismon */
			objectTo_sismon = creatComunicationObject(FJ, argc, argv);
			if (sendto(sd_intuti, &objectTo_sismon, sizeof(objectTo_sismon), 0, (struct sockaddr *)&to_sismon, to_sismonlen) < 0) {
				perror("Erro ao enviar para sismon");
			}
			else{
				printf("> Fechar as janelas do setor %d\n", nSetor);
			}

			/* recebe do sismon */
			if (recvfrom(sd_intuti, &MSG, sizeof(MSG), 0, (struct sockaddr *)&to_sismon, &to_sismonlen) < 0){
            	perror("Erro a receber do sismon");
    		}
			else{
				printf("%s\n",MSG);
			}
		}
		else {	// Caso o argumento passado for diferente de 0 a 3
			printf("> Erro no numero do setor [valores válidos 0-3]\n");
		}
	}	
	else{
		printf("> Erro no numero de argumentos\n");
	}
}

/***********************************************************************/

void cmd_lr (int argc, char** argv)
{
	int nSetor;
	OBJECT_COMUN objectTo_sismon;

	char MSG[MAX_LINE];

	if(argc == 2){		
		nSetor = atoi(argv[1]);
		if(nSetor <= 3 && nSetor >= 0){
			/* envia para o sismon */
			objectTo_sismon = creatComunicationObject(LR, argc, argv);
			if (sendto(sd_intuti, &objectTo_sismon, sizeof(objectTo_sismon), 0, (struct sockaddr *)&to_sismon, to_sismonlen) < 0) {
				perror("Erro ao enviar para sismon");
			}
			else{
				printf("> Ligar a rega do setor %d\n", nSetor);
			}

			/* recebe do sismon */
			if (recvfrom(sd_intuti, &MSG, sizeof(MSG), 0, (struct sockaddr *)&to_sismon, &to_sismonlen) < 0){
            	perror("Erro a receber do sismon");
    		}
			else{
				printf("%s\n",MSG);
			}
		}
		else {	// Caso o argumento passado for diferente de 0 a 3
			printf("> Erro no numero do setor [valores válidos 0-3]\n");
		}
	}	
	else{
		printf("> Erro no numero de argumentos\n");
	}
}

/***********************************************************************/

void cmd_dr (int argc, char** argv)
{
	int nSetor;
	OBJECT_COMUN objectTo_sismon;

	char MSG[MAX_LINE];

	if(argc == 2){		
		nSetor = atoi(argv[1]);
		if(nSetor <= 3 && nSetor >= 0){
			/* envia para o sismon */
			objectTo_sismon = creatComunicationObject(DR, argc, argv);
			if (sendto(sd_intuti, &objectTo_sismon, sizeof(objectTo_sismon), 0, (struct sockaddr *)&to_sismon, to_sismonlen) < 0) {
				perror("Erro ao enviar para sismon");
			}
			else{
				printf(">Desligar a regar do setor %d\n", nSetor);
			}

			/* recebe do sismon */
			if (recvfrom(sd_intuti, &MSG, sizeof(MSG), 0, (struct sockaddr *)&to_sismon, &to_sismonlen) < 0){
            	perror("Erro a receber do sismon");
    		}
			else{
				printf("%s\n",MSG);
			}
		}
		else {	// Caso o argumento passado for diferente de 0 a 3
			printf("> Erro no numero do setor [valores válidos 0-3]\n");
		}
	}	
	else{
		printf("> Erro no numero de argumentos\n");
	}
}

/***********************************************************************/

void cmd_mps (int argc, char** argv)
{
	int nSetor;
	OBJECT_COMUN objectTo_sismon;

	char MSG[MAX_LINE];

	if(argc == 3){		
		nSetor = atoi(argv[1]);
		if(nSetor <= 3 && nSetor >= 0){
			/* envia para o sismon */
			objectTo_sismon = creatComunicationObject(MPS, argc, argv);
			if (sendto(sd_intuti, &objectTo_sismon, sizeof(objectTo_sismon), 0, (struct sockaddr *)&to_sismon, to_sismonlen) < 0) {
				perror("Erro ao enviar para sismon");
			}
			else{
				printf("> Modifica o periodo sensor do setor %d\n", nSetor);
			}

			/* recebe do sismon */
			if (recvfrom(sd_intuti, &MSG, sizeof(MSG), 0, (struct sockaddr *)&to_sismon, &to_sismonlen) < 0){
            	perror("Erro a receber do sismon");
    		}
			else{
				printf("%s\n",MSG);
			}
		}
		else {	// Caso o argumento passado for diferente de 0 a 3
			printf("> Erro no numero do setor [valores válidos 0-3]\n");
		}
	}	
	else{
		printf("> Erro no numero de argumentos\n");
	}
}

/***********************************************************************/

void cmd_mpa (int argc, char** argv)
{
	int nSetor;
	OBJECT_COMUN objectTo_sismon;

	char MSG[MAX_LINE];

	if(argc == 3){		
		nSetor = atoi(argv[1]);
		if(nSetor <= 3 && nSetor >= 0){
			/* envia para o sismon */
			objectTo_sismon = creatComunicationObject(MPA, argc, argv);
			if (sendto(sd_intuti, &objectTo_sismon, sizeof(objectTo_sismon), 0, (struct sockaddr *)&to_sismon, to_sismonlen) < 0) {
				perror("Erro ao enviar para sismon");
			}
			else{
				printf("> Modifica o periodo ambiente do setor %d\n", nSetor);
			}

			/* recebe do sismon */
			if (recvfrom(sd_intuti, &MSG, sizeof(MSG), 0, (struct sockaddr *)&to_sismon, &to_sismonlen) < 0){
            	perror("Erro a receber do sismon");
    		}
			else{
				printf("%s\n",MSG);
			}
		}
		else {	// Caso o argumento passado for diferente de 0 a 3
			printf("> Erro no numero do setor [valores válidos 0-3]\n");
		}
	}	
	else{
		printf("> Erro no numero de argumentos\n");
	}
}

/***********************************************************************/

void cmd_cala (int argc, char** argv)
{
	OBJECT_COMUN objectTo_sismon;

	char MSG[MAX_LINE];

	if(argc == 1){
		/* envia para o sismon */
		objectTo_sismon = creatComunicationObject(CALA, argc, argv);
		if (sendto(sd_intuti, &objectTo_sismon, sizeof(objectTo_sismon), 0, (struct sockaddr *)&to_sismon, to_sismonlen) < 0) {
			perror("Erro ao enviar para sismon");
		}
		else{
			printf("> Pedido para consultar os limites de Temperatura e Humidade\n");
		}

		/* recebe do sismon */
		if (recvfrom(sd_intuti, &MSG, sizeof(MSG), 0, (struct sockaddr *)&to_sismon, &to_sismonlen) < 0){
            	perror("Erro a receber do sismon");
    	}
		else{
			printf("%s\n",MSG);
		}
	}
	else {
		printf("> Muitos argumentos\n");
	}
}

/***********************************************************************/

void cmd_dala (int argc, char** argv)
{
	OBJECT_COMUN objectTo_sismon;

	char MSG[MAX_LINE];

	if(argc == 5){
		/* envia para o sismon */
		objectTo_sismon = creatComunicationObject(DALA, argc, argv);
		if (sendto(sd_intuti, &objectTo_sismon, sizeof(objectTo_sismon), 0, (struct sockaddr *)&to_sismon, to_sismonlen) < 0) {
			perror("Erro ao enviar para sismon");
		}
		else{
			printf("> Definir os limites de Temperatura e Humidade\n");
		}

		/* recebe do sismon */
		if (recvfrom(sd_intuti, &MSG, sizeof(MSG), 0, (struct sockaddr *)&to_sismon, &to_sismonlen) < 0){
            	perror("Erro a receber do sismon");
    	}
		else{
			printf("%s\n",MSG);
		}
	}
	else {
		printf("> Muitos argumentos\n");
	}
}

/***********************************************************************/

void cmd_cer (int argc, char** argv)
{
	OBJECT_COMUN objectTo_sismon;

	char MSG[MAX_LINE];

	if(argc == 1){
		objectTo_sismon = creatComunicationObject(CER, argc, argv);
		printf("> Pedido para consultar o estado do envio de registos para o histórico\n");
		if (sendto(sd_intuti, &objectTo_sismon, sizeof(objectTo_sismon), 0, (struct sockaddr *)&to_sismon, to_sismonlen) < 0) {
			perror("Erro ao enviar para sismon");
		}

		if (recvfrom(sd_intuti, &MSG, sizeof(MSG), 0, (struct sockaddr *)&to_sismon, &to_sismonlen) < 0){
            perror("Erro a receber do sismon");
    	}
		else{
			printf("%s\n",MSG);
		}
	}
	else {
		printf("> Muitos argumentos\n");
	}
}

/***********************************************************************/

void cmd_aer (int argc, char** argv)
{
	OBJECT_COMUN objectTo_sismon;

	char MSG[MAX_LINE];

	if(argc == 1){
		objectTo_sismon = creatComunicationObject(AER, argc, argv);
		printf("> Pedido para ativar o envio de registos para o histórico\n");
		if (sendto(sd_intuti, &objectTo_sismon, sizeof(objectTo_sismon), 0, (struct sockaddr *)&to_sismon, to_sismonlen) < 0) {
			perror("Erro ao enviar para sismon");
		}

		if (recvfrom(sd_intuti, &MSG, sizeof(MSG), 0, (struct sockaddr *)&to_sismon, &to_sismonlen) < 0){
            perror("Erro a receber do sismon");
    	}
		else{
			printf("%s\n",MSG);
		}
	}
	else {
		printf("> Muitos argumentos\n");
	}
}

/***********************************************************************/

void cmd_der (int argc, char** argv)
{
	OBJECT_COMUN objectTo_sismon;

	char MSG[MAX_LINE];

	if(argc == 1){
		objectTo_sismon = creatComunicationObject(DER, argc, argv);
		printf("> Pedido para desativar o envio de registos para o histórico\n");
		if (sendto(sd_intuti, &objectTo_sismon, sizeof(objectTo_sismon), 0, (struct sockaddr *)&to_sismon, to_sismonlen) < 0) {
			perror("Erro ao enviar para sismon");
		}

		if (recvfrom(sd_intuti, &MSG, sizeof(MSG), 0, (struct sockaddr *)&to_sismon, &to_sismonlen) < 0){
            perror("Erro a receber do sismon");
    	}
		else{
			printf("%s\n",MSG);
		}
	}
	else {
		printf("> Muitos argumentos\n");
	}
}

/***********************************************************************/

void cmd_tsm (int argc, char** argv)
{
	OBJECT_COMUN objectTo_sismon;

	if(argc == 1){
		objectTo_sismon = creatComunicationObject(TSM, argc, argv);
		printf("> Encerramento do processo SISMON\n");
		if (sendto(sd_intuti, &objectTo_sismon, sizeof(objectTo_sismon), 0, (struct sockaddr *)&to_sismon, to_sismonlen) < 0) {
			perror("Erro ao enviar para sismon");
		}
	}
	else {
		printf("> Muitos argumentos\n");
	}
	
}

/***********************************************************************/

void cmd_lreg (int argc, char** argv)
{
	OBJECT_COMUN_LREG objectTo_reghist;
	
	char MSG[MAX_LINE];
	int totalReghists=0,i,nSetor;
	struct tm tm;
	char str[26];

	if(argc == 2){			// Nome do comando + Número do setor
		nSetor = atoi(argv[1]);
		if(nSetor <= 3 && nSetor >= 0){
			objectTo_reghist.func_number = LREG;
			objectTo_reghist.nSetor=nSetor;
			objectTo_reghist.count_times = 0;		// Não foi indicada nenhuma data

			printf("> Listar registos\n");
			if (sendto(sd_intuti2, &objectTo_reghist, sizeof(objectTo_reghist), 0, (struct sockaddr *)&to_reghist, to_reghistlen) < 0) {
				perror("Erro ao enviar para reghist");
			}
			
			if (recvfrom(sd_intuti2,&totalReghists, sizeof(totalReghists), 0, (struct sockaddr *)&to_reghist, &to_reghistlen) < 0){
				perror("Erro a receber do sismon");
			}
			else{
				printf("%d\n",totalReghists);
			}

			for(i=0;i<totalReghists;i++){
				if (recvfrom(sd_intuti2, &MSG, sizeof(MSG), 0, (struct sockaddr *)&to_reghist, &to_reghistlen) < 0){
					perror("Erro a receber do sismon");
				}
				else{
					printf("%s\n",MSG);
				}
			}
		}
		else {	// Caso o argumento passado for diferente de 0 a 3
			printf("> Erro no numero do setor [valores válidos 0-3]\n");
		}
	}

	else if(argc == 4){			// Nome do comando + Número do setor + t1
		nSetor = atoi(argv[1]);
		if(nSetor <= 3 && nSetor >= 0){
			objectTo_reghist.func_number = LREG;
			objectTo_reghist.nSetor=nSetor;
			objectTo_reghist.count_times = 1;			// Foi indicada a data t1

			sprintf(str,"%s %s",argv[2],argv[3]);		// Junção da data e hora em apenas uma string
			
			strptime(str,"%d/%m/%Y %H:%M:%S\n", &tm);	// Conversão do tempo
			objectTo_reghist.t[0] = mktime(&tm);

			printf("%ld\n",objectTo_reghist.t[0]);

			printf("> Listar registos\n");
			if (sendto(sd_intuti2, &objectTo_reghist, sizeof(objectTo_reghist), 0, (struct sockaddr *)&to_reghist, to_reghistlen) < 0) {
				perror("Erro ao enviar para reghist");
			}
		}
		else {	// Caso o argumento passado for diferente de 0 a 3
			printf("> Erro no numero do setor [valores válidos 0-3]\n");
		}
	}

	else if(argc == 6){			// Nome do comando + Número do setor + t1 + t2
		nSetor = atoi(argv[1]);
		if(nSetor <= 3 && nSetor >= 0){
			objectTo_reghist.func_number = LREG;
			objectTo_reghist.nSetor=nSetor;
			objectTo_reghist.count_times = 2;			// Foi indicada a data t1 e t2

			sprintf(str,"%s %s",argv[2],argv[3]);		// Junção da data e hora em apenas uma string
			
			strptime(str,"%d/%m/%Y %H:%M:%S\n", &tm);	// Conversão do tempo t1
			objectTo_reghist.t[0] = mktime(&tm);

			sprintf(str,"%s %s",argv[4],argv[5]);		// Junção da data e hora em apenas uma string
			
			strptime(str,"%d/%m/%Y %H:%M:%S\n", &tm);	// Conversão do tempo t2
			objectTo_reghist.t[1] = mktime(&tm);

			printf("> Listar registos\n");
			if (sendto(sd_intuti2, &objectTo_reghist, sizeof(objectTo_reghist), 0, (struct sockaddr *)&to_reghist, to_reghistlen) < 0) {
				perror("Erro ao enviar para reghist");
			}
		}
		else {	// Caso o argumento passado for diferente de 0 a 3
			printf("> Erro no numero do setor [valores válidos 0-3]\n");
		}
	}

	else {
		printf("> Erro no numero de argumentos\n");
	}
}

/***********************************************************************/

void cmd_trh (int argc, char** argv)
{
	OBJECT_COMUN_LREG objectTo_reghist;

	if(argc == 1){
		objectTo_reghist.func_number = TRH;
		printf("> Encerramento do processo REGHIST\n");
		if (sendto(sd_intuti2, &objectTo_reghist, sizeof(objectTo_reghist), 0, (struct sockaddr *)&to_reghist, to_reghistlen) < 0){
			perror("Erro ao enviar para reghist");
		}
	}
	else {
		printf("> Muitos argumentos\n");
	}
}

/***********************************************************************/

void cmd_sos (int argc, char **argv)
{
  int i;

	if(argc == 1){
		printf("%s\n", titleMSG);
		for (i=0; i<NCOMMANDS; i++)
			printf("%s %s\n", commands[i].cmd_name, commands[i].cmd_help);
	}
	else {
		printf("> Muitos argumentos\n");
	}
}

/***********************************************************************/

void cmd_sair (int argc, char **argv)
{
	if(argc == 1){
    	exeIntuti = !exeIntuti;
	}
	else {
		printf("> Muitos argumentos\n");
	}
}
