#include "../../lib/mainheader.h"


/**** Socket SISMON ****/

int sd_sismon;
struct sockaddr_un from_intuti;
socklen_t from_intutilen;

/**** Socket INTUTI ****/

int sd_intuti;	    // intuti -> sismon
int sd_intuti2; 	// intuti -> reghist
struct sockaddr_un to_sismon;
socklen_t to_sismonlen;
struct sockaddr_un to_reghist;
socklen_t to_reghistlen;

/**** Socket REGHIST ****/

int sd_reghist;
struct sockaddr_un from_intutir;
socklen_t from_intutilenr;

/**********************************************************
                    COMUNICAÇÃO SOCKETS
**********************************************************/

void closeSocketComunication(int sd, char file[]){          // Encerra a socket
    close(sd);             // fecha a socket
    unlink(file);          // Elimina o file do /tmp
}

/****************/

void iniSocketClient (int *sd, char fileIN[], char fileOUT[],           // Inicializa a socket do lado do cliente (intuti)
    struct sockaddr_un *toAddr, socklen_t *toAddrlen){

    struct sockaddr_un addr;
    socklen_t addrlen;

    if ((*sd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0 ) {  // Criação de uma socket do tipo datagram
        perror("Erro a criar socket"); 
        exit(-1);
    }

    addr.sun_family = AF_UNIX;                    // Preenchimento da estrutura da socket com a informação sobre o Intuti
    memset(addr.sun_path, 0, sizeof(addr.sun_path));
    strcpy(addr.sun_path, fileIN);
    addrlen = sizeof(addr.sun_family) + strlen(addr.sun_path);

    if (bind(*sd, (struct sockaddr *)&addr, addrlen) < 0 ) {  // Atribui o nome/endereço à socket 
        perror("Erro no bind"); 
        exit(-1);
    }

    toAddr->sun_family = AF_UNIX;        // Preenchimento da estrutura da socket com a informação sobre o Sismon (para onde queremos comunicar) 
    memset(toAddr->sun_path, 0, sizeof(toAddr->sun_path));
    strcpy(toAddr->sun_path, fileOUT);
    *toAddrlen = sizeof(toAddr->sun_family) + strlen(toAddr->sun_path);
}

/****************/

void iniSocketServer (int *sd, char file[]){    // Inicializa a socket do lado do servidor (sismon e reghist)

    struct sockaddr_un addr;
    socklen_t addrlen;

    if ((*sd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0 ) {   // Criação de uma socket do tipo datagram
        perror("Erro a criar socket"); 
        exit(-1);
    }

    addr.sun_family = AF_UNIX;                       // Preenchimento da estrutura da socket com a informação sobre o SISMON
    memset(addr.sun_path, 0, sizeof(addr.sun_path));
    strcpy(addr.sun_path, file);
    addrlen = sizeof(addr.sun_family) + strlen(addr.sun_path);

    if (bind(*sd, (struct sockaddr *)&addr, addrlen) < 0 ) {  // Atribui o nome/endereço à socket 
        perror("Erro no bind"); 
        exit(-1);
    }
}