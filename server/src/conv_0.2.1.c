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
* File: conv_0.2.1                                                          *
*       Converte la struttura dati_server da 0.2.0 a 0.2.1                  *
****************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "user_flags.h"
#include "config.h"

/*
      Vecchia struttura dati dell'utente: versione 0.2.0 o precedente.
 */
struct dati_ut_0_2_0 {
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

  struct dati_ut *prossimo;  /* Prossima struttura di utente     */
};

/*
         Nuova struttura dati dell'utente: versione 0.2.1
 */
struct dati_ut {
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

  struct dati_ut *prossimo;  /* Prossima struttura di utente     */
};

int main(void)
{
	struct dati_ut_0_2_0 lista_utenti_old;
	struct dati_ut_0_2_0 uo, po;
	struct dati_ut lista_utenti;
        struct dati_ut *un, *pn;
	FILE  *fp;
        int hh = 0, i, ch;
	char bak[256];

	printf("\nCopyright (C) 1999-2000 by Marco Caldarelli and Riccardo Vianello\n");
	printf("\nUtility di conversione strutture del server.\n");
	printf("Effettua l'upgrade alla versione 0.2.1 di Cittadella/UX da versioni precedenti.\n\n");

	/* Carica la struttura vecchia */
        fp = fopen(FILE_UTENTI, "r");
        if (!fp) {
                printf("\nErrore: Non posso aprire in lettura il file utenti!\n\n");
		return -1;
        }
        fseek(fp, 0L, 0);
  
        /* Ciclo di lettura dati */  
        CREATE(uo, struct dati_ut_0_2_0, 1, TYPE_DATI_UT);
        hh = fread((struct dati_ut_0_2_0 *)uo,
		   sizeof(struct dati_ut_0_2_0), 1, fp);
  
        if (hh == 1) {
                lista_utenti_old = uo;
                po = NULL;
        } else
                log("SYSTEM: File_utenti vuoto, creato al primo salvataggio.");
  
        while (hh == 1) {
                if (po != NULL)
                        po->prossimo = uo;
                po = uo;
                po->prossimo = NULL;
                CREATE(uo, struct dati_ut_0_2_0, 1, TYPE_DATI_UT);
                hh = fread((struct dati_ut_0_2_0 *)uo,
			   sizeof(struct dati_ut_0_2_0), 1, fp);
        }
        Free(uo);
        fclose(fp);
	
	/* Chiede conferma             */
	printf("Sei sicuro di voler procedere alla conversione (s/n)? ");
	ch = 0;
	do {
		ch = getchar();
	} while (ch != 's' && ch != 'n');
	printf("%c\n\n", ch);
	if (ch != 's') {
		printf("Conversione abortita.\n\n");
		return -1;
	}

	CREATE(pn, struct dati_ut, 1, TYPE_DATI_UT);
	lista_utenti = pn;
	for (po = lista_utenti_old; po; po = po->prossimo) {
		/* Converte la struttura       */
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
		strncpy(pn->->lasthost, po->lasthost, MAXLEN_LASTHOST);
		/* Flags di configurazione */
		for (i=0; i<NUM_UTFLAGS; i++)
			pn->flags[i] = po->flags[i];
		for (i=0; i<NUM_UTSFLAGS; i++)
			pn->sflags[i] = po->sflags[i];
		for (i=0; i<NUM_NFRIENDS; i++)
			pn->friends[i] = po->friends[i];

		/* Inizializzazione nuove variabili */

		CREATE(un, struct dati_ut, 1, TYPE_DATI_UT);
		pn->prossimo = un;

	}

	/* Backup vecchia struttura    */
        sprintf(bak, "%s-0.2.0", FILE_UTENTI);
        rename(FILE_UTENTI, bak);

	/* Salva la struttura          */
        fp = fopen(FILE_UTENTI, "w");
        if (!fp) {
                printf("Errore: Non posso aprire in scrittura il file dati_server!");
                return -1;
        }
        hh = fwrite((struct dati_ut *) &ds, sizeof(struct dati_ut), 1, fp);
        fclose(fp);

	/* The End */
	printf("Ok, la conversione e' stata effettuata con successo!!\n");
	printf("\nIl backup del vecchio file dati_server si trova in:\n");
	printf("%s\n\n", bak);
	return 0;
}
