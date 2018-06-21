/*
 *  Copyright (C) 1999-2000 by Marco Caldarelli and Riccardo Vianello
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
/****************************************************************************
* Cittadella/UX server tools             (C) M. Caldarelli and R. Vianello  *
*                                                                           *
* File: conv_0.3.1                                                          *
*       Converte la struttura dati_ut da 0.3.0 a 0.3.1                      *
****************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <error.h>
#include <string.h>

#include "user_flags.h"
#include "config.h"
#include "memstat.h"
#include "utility.h"

FILE *logfile;

/*
      Vecchia struttura dati dell'utente: versione 0.3.0.
 */
struct dati_ut_old {
  char nome[MAXLEN_UTNAME];       /* Nickname dell'utente            */
  char password[MAXLEN_PASSWORD]; /* Password                        */
  long matricola;                 /* Eternal number                  */
  int livello;                    /* Livello di accesso              */
  char registrato;                /* Se 0 deve ancora registrarsi    */
  char val_key[MAXLEN_VALKEY];    /* Validation Key                  */
                                  /* Se val_key[0]==0 e' validato    */
  /* Dati Personali */
  char nome_reale[MAXLEN_RNAME]; /* Nome 'Real-life'                 */
  char via[MAXLEN_VIA];		 /* Street address                   */
  char citta[MAXLEN_CITTA];	 /* Municipality                     */
  char stato[MAXLEN_STATO];	 /* State or province                */
  char cap[MAXLEN_CAP];		 /* Codice di avviamento postale     */
  char tel[MAXLEN_TEL];		 /* Numero di telefono               */
  char email[MAXLEN_EMAIL];	 /* Indirizzo E-mail                 */
  char url[MAXLEN_URL];          /* Indirizzo Home-Page              */

  /* Dati di accesso */
  int chiamate;                  /* Numero di chiamate               */
  int post;                      /* Numero di post                   */
  int mail;                      /* Numero di mail                   */ 
  int x_msg;                     /* Numero di X-msg                  */ 
  int chat;                      /* Numero messaggi in chat          */
  long firstcall;                /* Prima chiamata                   */
  long lastcall;                 /* Ultima chiamata                  */
  long time_online;              /* Tempo trascorso online           */
  char lasthost[256];            /* Provenienza dell'ultima chiamata */

  /* Flags di configurazione */
  char flags[NUM_UTFLAGS];   /* 64 bits di configurazione        */
  char sflags[NUM_UTSFLAGS]; /* bits conf. non modif. dall'utente*/    
  long friends[NFRIENDS];    /* Matricole utenti nella friendlist*/

  struct dati_ut_old *prossimo;  /* Prossima struttura di utente     */
};

/*
         Nuova struttura dati dell'utente: versione 0.3.1
 */
struct dati_ut {
  char nome[MAXLEN_UTNAME];       /* Nickname dell'utente            */
  char password[MAXLEN_PASSWORD]; /* Password                        */
  long matricola;                 /* Eternal number                  */
  int livello;                    /* Livello di accesso              */
  char registrato;                /* Se 0 deve ancora registrarsi    */
  char val_key[MAXLEN_VALKEY];    /* Validation Key                  */
                                  /* Se val_key[0]==0 e' validato    */
  unsigned char secondi_per_post; /* Numero minimo di secondi tra 2 post */

  /* Dati Personali */
  char nome_reale[MAXLEN_RNAME]; /* Nome 'Real-life'                 */
  char via[MAXLEN_VIA];		 /* Street address                   */
  char citta[MAXLEN_CITTA];	 /* Municipality                     */
  char stato[MAXLEN_STATO];	 /* State or province                */
  char cap[MAXLEN_CAP];		 /* Codice di avviamento postale     */
  char tel[MAXLEN_TEL];		 /* Numero di telefono               */
  char email[MAXLEN_EMAIL];	 /* Indirizzo E-mail                 */
  char url[MAXLEN_URL];          /* Indirizzo Home-Page              */

  /* Dati di accesso */
  int chiamate;                  /* Numero di chiamate               */
  int post;                      /* Numero di post                   */
  int mail;                      /* Numero di mail                   */ 
  int x_msg;                     /* Numero di X-msg                  */ 
  int chat;                      /* Numero messaggi in chat          */
  long firstcall;                /* Prima chiamata                   */
  long lastcall;                 /* Ultima chiamata                  */
  long time_online;              /* Tempo trascorso online           */
  char lasthost[256];            /* Provenienza dell'ultima chiamata */

  /* Flags di configurazione */
  char flags[NUM_UTFLAGS];   /* 64 bits di configurazione        */
  char sflags[NUM_UTSFLAGS]; /* bits conf. non modif. dall'utente*/    
  long friends[NFRIENDS];    /* Matricole utenti nella friendlist*/

  struct dati_ut *prossimo;  /* Prossima struttura di utente     */
};

int main(void)
{
	struct dati_ut_old  *po;
        struct dati_ut  *pn;
	FILE  *fpold, *fpnew;
        int hh = 0, i, kk=0;
	char bak[256];
	char new[256];

	logfile=stderr;
	printf("\nCopyright (C) 1999-2000 by Marco Caldarelli and Riccardo Vianello\n");
	printf("\nUtility di conversione strutture del server.\n");
	printf("Effettua l'upgrade alla versione 0.3.1 di Cittadella/UX dalla versione precedente.\n\n");
	printf("size old:%d\n",sizeof(struct dati_ut_old));
	printf("size new:%d\n",sizeof(struct dati_ut));

        sprintf(new, "%s-new", FILE_UTENTI);
		printf("from %s to:%s\n", FILE_UTENTI, new);
		printf("starting...\n");
	/* Carica la struttura vecchia */
        fpold = fopen(FILE_UTENTI, "r");
        fpnew = fopen(new, "w");
        if (!fpold) {
                printf("\nErrore: Non posso aprire in lettura il file utenti!\n\n");
		return -1;
        }

        fseek(fpold, 0L, 0);
        if (!fpnew) {
                printf("\nErrore: Non posso aprire in scrittura il file utenti!\n\n");
		return -1;
        }

        fseek(fpnew, 0L, 0);
  
        /* Ciclo di lettura dati */  

        CREATE(po, struct dati_ut_old, 1, TYPE_DATI_UT);
        CREATE(pn, struct dati_ut, 1, TYPE_DATI_UT);

		while((hh = fread((struct dati_ut_old *)po,
		   sizeof(struct dati_ut_old), 1, fpold))==1){

                if (po == NULL)
						continue;
				kk++;

		strncpy(pn->nome, po->nome, MAXLEN_UTNAME);
		strncpy(pn->password, po->password, MAXLEN_PASSWORD);
		pn->matricola = po->matricola;
		pn->livello = po->livello;
		pn->registrato = po->registrato;
		strncpy(pn->val_key, po->val_key, MAXLEN_VALKEY);
		/* Dati Personali */
		strncpy(pn->nome_reale, po->nome_reale, MAXLEN_RNAME);
		strncpy(pn->via, po->via, MAXLEN_VIA);
		strncpy(pn->citta, po->citta, MAXLEN_CITTA);
		strncpy(pn->stato, po->stato, MAXLEN_STATO);
		strncpy(pn->cap, po->cap, MAXLEN_CAP);
		strncpy(pn->tel, po->tel, MAXLEN_TEL);
		strncpy(pn->email, po->email, MAXLEN_EMAIL);
		strncpy(pn->url, po->url, MAXLEN_URL);
		/* Dati di accesso */
		pn->chiamate = po->chiamate;
		pn->post = po->post;
		pn->mail = po->mail;
		pn->x_msg = po->x_msg;
		pn->chat = po->chat;
		pn->firstcall = po->firstcall;
		pn->lastcall = po->lastcall;
		pn->time_online = po->time_online;
		strncpy(pn->lasthost, po->lasthost, MAXLEN_LASTHOST);
		/* Flags di configurazione */
		for (i=0; i<NUM_UTFLAGS; i++)
			pn->flags[i] = po->flags[i];
		for (i=0; i<NUM_UTSFLAGS; i++)
			pn->sflags[i] = po->sflags[i];
		for (i=0; i<NFRIENDS; i++)
			pn->friends[i] = po->friends[i];

		/* Inizializzazione nuove variabili */
		pn->secondi_per_post=0;

        if (fwrite((struct dati_ut_old *)pn,
		          sizeof(struct dati_ut), 1, fpnew)!=1) {
				printf("ERRORE!");
				fclose(fpnew);
				fclose(fpold);
				Free(po);
				Free(pn);
				return -100;
	}

		}

	/* Backup vecchia struttura    */
        sprintf(bak, "%s-old", FILE_UTENTI);
        rename(FILE_UTENTI, bak);
		rename(new,FILE_UTENTI);

	/* Salva la struttura          */
        fclose(fpnew);
        fclose(fpold);

	/* The End */
	printf("Ok, la conversione e' stata effettuata con successo!!%d\n",kk);
	printf("\nIl backup del vecchio file dati_server si trova in:\n");
	printf("%s\n\n", bak);
	return 0;
}
