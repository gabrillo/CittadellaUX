/*
 *  Copyright (C) 1999-2002 by Marco Caldarelli and Riccardo Vianello
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
/****************************************************************************
* Cittadella/UX server                   (C) M. Caldarelli and R. Vianello  *
*                                                                           *
* File: old_urna.c                                                              *
*       Gestione referendum e sondaggi.                                     *
****************************************************************************/
/* #include "config.h"  */

#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "cittaserver.h"
#include "extract.h"
#include "memstat.h"
#include "sys_flags.h"
#include "text.h"
#include "urna-cost.h"
#include "conv_urna-old.0.3.2.h"
#include "utente.h"
#include "utility.h"
#include "urna-file.h"



/* Prototipi funzioni in questo file */
int old_load_file(const char *file, int est, FILE ** fpp);
int old_check_magic_number(FILE * fp, char code);
void old_rs_load_data(void);
int old_load_voci(struct old_voce **testap, long int num_voci, long int num);
int old_ripulisci_quesito(long *testa, int max);
int old_load_stat();
int old_load_dati(struct old_urna_data **udp, long num_quesito);
void azzera_modifica();
void debugga_server(struct old_urna_data *ud, int level);


int    modif_stat = 0;
int modif[MAX_REFERENDUM];
/* 
 * apre un file (r) sotto URNA_DIR con estensione est (se est>=0), e
 * lo assegna a fp esce con 0 se tutto va bene, -1 se ci sono
 * problemi, 1 se il file è lungo 0 (in questo caso lo chiude). Se il
 * file non esiste lo crea, e lo tratta come nel caso di file di
 * lunghezza 0
 */

int old_load_file(const char *file, const int est, FILE ** fpp)
{
    char *nomefile;
    struct stat status;
    int lnomefile;
    int ret = 0;

    lnomefile = strlen(URNA_DIR);
    lnomefile += strlen(file);
    lnomefile += 6;		/*  per '/-est0'   */

    if (abs(est) >= 1000) {
	citta_log("SYSERR: estensione troppo lunga.");
	return -1;
    }

    CREATE(nomefile, char, lnomefile, TYPE_CHAR);

    if (est >= 0)
	sprintf(nomefile, "%s/%s-%d", URNA_DIR, file, est);
    else
	sprintf(nomefile, "%s/%s", URNA_DIR, file);
	/*
	 * fa un backup
	 */

    *fpp = fopen(nomefile, "r");
    if (!*fpp) {
	citta_logf("SYSLOG: errore in apertura:%d,%s", errno, strerror(errno));
	if (errno == ENOENT) {
	    *fpp = fopen(nomefile, "w");
	    fclose(*fpp);
	    Free(nomefile);
	    return (1);
	} else {
	    Free(nomefile);
	    return (-1);
	}
    }

    if (stat(nomefile, &status)) {
	Free(nomefile);
	return (-1);
    }

    if (status.st_size == 0) {
	fclose(*fpp);
	Free(nomefile);
	return (1);
    }

    Free(nomefile);
    return (ret);
}
/* controlla il magic_number - 0 se tutto è OK */
int old_check_magic_number(FILE * fp, char code)
{
    char mnumb[5];
    char mcheck[5];

    sprintf(mcheck, "%2s%1c%1c", MAGIC_CODE, code, VERSIONE);

    if (fread(mnumb, sizeof(char), 5, fp) != 5){
			printf("?\n");
	return (-1);
	}
    /*   */

    return (strcmp(mnumb, mcheck) == 0);
}


void old_rs_load_data()
{
    long i;
    struct old_urna *nuova, *vecchia;
    long num_quesito;

    old_urna_testa = NULL;
    old_urna_ultima = NULL;

    modif_stat = 0;

    if (old_load_stat()) {
	citta_log("SYSERROR: non si possono usare i referendum!");
	return;
    }

    azzera_modifica();

    if (old_ustat.attive == 0)
	return;
    /*   */

    /* 
     * È un po' incasinato. Soprattutto non ha il primo caso e poi
     * i successivi, altrimenti se il primo load fallisce va in
     * vacca tutto (o bisogna fare un while...) 
     */

    nuova = NULL;
    vecchia = NULL;
    for (i = 0; i < old_ustat.attive; i++) {
	num_quesito = old_ustat.quesito_testa[i];


	CREATE(nuova, struct old_urna, 1, TYPE_URNA);
	nuova->num = num_quesito;
	nuova->prev = vecchia;

	if (old_urna_testa == NULL) {
	    old_urna_testa = nuova;
	    vecchia = nuova;	/* se è il primo punta a sé stesso... 
				   ma tanto poi cambia */
	}
	vecchia->next = nuova;

	if (!old_load_dati(&(nuova->data), num_quesito)) {
	    vecchia = nuova;
	} else {
	    if (old_urna_testa == nuova) {
		old_urna_testa = NULL;
	    }
	    Free(nuova);
	    old_ustat.quesito_testa[i] = -1;
	}
        /* debugga_server(vecchia->data, 2);*/
    }
    old_urna_ultima = nuova;
    if (old_urna_testa == nuova) {
	nuova->next = NULL;
    };

    old_ustat.attive = old_ripulisci_quesito(old_ustat.quesito_testa, old_ustat.attive);

    /* rialloca_quesito (&old_ustat); se si formano troppi 0???  */

    return;
}

int old_load_dati(struct old_urna_data **udp, long num_quesito)
{
    int i, j;
    FILE *fp;
    struct old_urna_data *ud;
    int num_old_urna;

    if (old_load_file(FILE_DATA, num_quesito, &fp)) {
	citta_logf("SYSLOG: problemi con %s", FILE_DATA);
	return (-1);
    }
    /*   */

    if (!old_check_magic_number(fp, MAGIC_DATA)) {
	citta_logf("SYSLOG: old_urna-data %s %ld non corretto", FILE_DATA,
	     num_quesito);
	fclose(fp);
	return (-1);
    }

    if (fread(&num_old_urna, sizeof(long), 1, fp) != 1) {
	citta_logf("SYSLOG: non riesco a leggere il numero del quesito");
	fclose(fp);
	return (-1);
    }

    if (num_old_urna != num_quesito) {
	citta_logf("SYSERR: non tornano i numeri: %d!=%ld!", num_old_urna,
	     num_quesito);
	fclose(fp);
	return (-1);
    }

    *udp = NULL;

    /*CREATE(*udp, struct old_urna_data, 1, TYPE_URNA_DATA);
	/non importa type
	*/
    CREATE(*udp, struct old_urna_data, 1, 1);

    ud = *udp;

    if (fread(ud, sizeof(struct old_urna_data), 1, fp) != 1) {
	citta_logf("SYSLOG: non riesco a leggere i dati");
	Free(*udp);
	fclose(fp);
	return (-1);
    }

    /* LEPPIE: Qui sbombava, queste righe dovrebbero risolvere il
       problema, ma ovvio che non sono una soluzione a lungo
       termine...                                              */
    if (ud->voti_nslots > 100) {
	citta_logf("SYSERR: old_urna_data->voti_nslots %ld sospetto", num_quesito);
	Free(*udp);
	fclose(fp);
	return (-1);
    }
    /* Fine toppa. */
    ud->uvot=(long*)calloc((ud->voti_nslots) * LEN_SLOTS, sizeof(long));

    if (fread(ud->uvot, sizeof(long),
	     (ud->voti_nslots) * LEN_SLOTS, fp) !=
	(ud->voti_nslots) * LEN_SLOTS) {
	citta_logf("SYSLOG: non riesco a leggere i voti");
	Free(ud->uvot);
	Free(*udp);
	fclose(fp);
	return (-1);
    }

    for (i = 0; i < MAXLEN_QUESITO; i++) {
	if (fread(ud->testo[i], sizeof(char), LEN_TESTO, fp) != LEN_TESTO) {
	    /*   */
	    Free(ud->uvot);
	    Free(*udp);
	    citta_logf("SYSLOG: non riesco a leggere il testo");
	    fclose(fp);
	    return (-1);
	}
    }

    if (old_load_voci(&(ud->testa_voci), ud->num_voci, num_quesito)) {
	for (j = 0; j < MAXLEN_QUESITO; j++)
	    Free(ud->uvot);
	Free(*udp);
	citta_logf("SYSLOG: non riesco a leggere le voci");
	fclose(fp);
	return (-1);
    }

    return (0);
};

int
old_load_voci(struct old_voce **testap, long int num_voci, long int num_quesito)
{
    int i;
    FILE *fp;
    struct old_voce *questa;


    if (old_load_file(FILE_VOCI, num_quesito, &fp))
	return (-1);
/*   */

    if (!old_check_magic_number(fp, MAGIC_VOCI)) {
	citta_logf("SYSLOG: old_urna-voci %s %ldnon corretto", FILE_VOCI,
	     num_quesito);
	fclose(fp);
	return (-1);
    }

    /*
	 * CREATE(*testap, struct old_voce, num_voci, TYPE_VOCE);
	non importa type
	*/
	 CREATE(*testap, struct old_voce, num_voci, 1);

    for (i = 0; i < num_voci; i++) {
	questa = *testap + i;
	if (fread(questa, sizeof(struct old_voce), 1, fp) != 1) {
	    Free(*testap);
	    fclose(fp);
	    return (-1);
	}
	/*   */

	if (fread(questa->giudizi, sizeof(long), NUM_GIUDIZI, fp)
	    != NUM_GIUDIZI) {
	    Free(questa->giudizi);
	    citta_log("non posso creare i posti per i voti!");
	    fclose(fp);
	    return (-1);
	}
	/*   */
    }
    fclose(fp);
    return (0);
}

int old_ripulisci_quesito(long *testa, int max)
{
    int i, j;

    j = 0;
    for (i = 0; i < max; i++) {
	if (testa[i] > 0)
	    testa[j++] = testa[i];
/*   */
#ifdef DEBUG
	else {
	    citta_logf("DEB::eliminata pos %d", j);
	}
#endif
    }
    return (j);
}

int old_load_stat()
{
    FILE *fp;
    int letto;


    letto = old_load_file(FILE_STAT, -1, &fp);

    if (letto == -1) {
	citta_logf("SYSLOG: non posso aprire %s", FILE_STAT);
	return (-1);
    }

    /* se il file è vuoto */

    if (letto == 1) {
	citta_logf("SYSLOG: file %s vuoto", FILE_STAT);
	old_ustat.voti = 0;
	old_ustat.num = 0;
	old_ustat.complete = 0;
	old_ustat.attive = 0;
	old_ustat.q_nslots = 1;	/* comunque ne alloco 1  */
	CREATE(old_ustat.quesito_testa, long, 1, TYPE_LONG);
	return (0);
    }

    if (!(old_check_magic_number(fp, MAGIC_STAT))) {
	citta_logf("SYSLOG: versione %s non corretta", FILE_STAT);
	fclose(fp);
	return (-1);
    }

    if (fread(&old_ustat, sizeof(struct old_urna_stat), 1, fp) != 1) {
	citta_logf("SYSLOG: dati su %s corrotti", FILE_STAT);
	fclose(fp);
	return (-1);
    }


    /* controllino su complete/attive/num??  */

    if (old_ustat.q_nslots < 1) {
	old_ustat.q_nslots = 1;
	citta_logf("SYSLOG:URNA letto q_nslots<1(%ld):riportato a 1",
	     old_ustat.q_nslots);
    }

    if (old_ustat.q_nslots > MAX_SLOT) {
	citta_log("SYSLOG:URNA errori nella lettura, troppi slots per" "old_ustat");
	fclose(fp);
	return (-1);
    }
    old_ustat.quesito_testa= (long*)calloc(old_ustat.q_nslots * LEN_SLOTS,sizeof(long));

    if (fread(old_ustat.quesito_testa, sizeof(long),
	      old_ustat.q_nslots * LEN_SLOTS, fp)
	!= old_ustat.q_nslots * LEN_SLOTS) {
	citta_logf("SYSLOG: dati sui numeri urna in %s corrotti", FILE_STAT);
	fclose(fp);
	return (-1);
    }
    fclose(fp);
    return (0);
}

void azzera_modifica()
{
    long i;
    for (i = 0; i < MAX_SONDAGGI; i++)
	modif[i] = -1;
}

void debugga_server(struct old_urna_data *ud, int level)
{
    int i, j;
    struct old_voce *pvoce;
    char *str_copia, *str_tmp;

    citta_logf("\n\n\n-------------------------------------------------------------\n");
    citta_logf("DEB:proponente:%ld,room:%ld,tipo:%d,modo:%d\n,"
	 " bianca:%d, scelta %d, start %ld, stop %ld", ud->proponente,
	 ud->room_num, ud->tipo, ud->modo, ud->bianca,
	 ud->astensione_scelta, ud->start, ud->stop);

    citta_logf("DEB:titolo:%s", ud->titolo);
    citta_logf("DEB:num_voci:%d,max_voci:%d criterio:%d,val_crit%ld,anzianità:%ld", ud->num_voci, ud->max_voci, ud->criterio, ud->val_crit, ud->anzianita);
    citta_logf("nvoti:%ld", ud->nvoti);
    if (level == 1)
	return;
    /*
    str_copia = Malloc(ud->nvoti * URNA_NSLOTS + 10 * 10 * 4, TYPE_CHAR);
    str_tmp = Malloc(URNA_NSLOTS + 10 * 4, TYPE_CHAR);
    */
    str_copia=(char*)calloc(( ud->nvoti * LEN_SLOTS + 10 * 10 * 4),sizeof(char));
    str_tmp=(char*)calloc( LEN_SLOTS + 10 * 4, sizeof(char));

    citta_log("DEB:quesito");
    for (i = 0; i < MAXLEN_QUESITO; i++) {
	citta_logf("%s", ud->testo[i]);
    }
    sprintf(str_copia, " ");
    for (i = 0; i < ud->nvoti; i++) {
	sprintf(str_tmp, "%ld", (ud->uvot)[i]);
	strcat(str_copia, str_tmp);
    }
    citta_logf("\nDEB:votanti:%s", str_copia);

    citta_logf("\nDEB:bianche:%ld", ud->bianche);
    citta_logf("\nDEB::%d", ud->posticipo);
    pvoce = ud->testa_voci;
    citta_log("\n\nDEB:voci:");
    for (i = 0; i < ud->num_voci; i++) {
	pvoce = ud->testa_voci + i;
	citta_logf("\nvoce (%c):%s", 'a' + i, pvoce->scelta);
	citta_logf("num_voti:%ld, tot_voti %ld, astensioni %ld",
	     pvoce->num_voti, pvoce->tot_voti, pvoce->astensioni);
	sprintf(str_copia, " ");
	for (j = 0; j < 10; j++) {
	    sprintf(str_tmp, "%d:%ld, ", j, (pvoce->giudizi)[j]);
	    strcat(str_copia, str_tmp);
	}
	citta_logf("voto %s", str_copia);
    }
    Free(str_copia);
    Free(str_tmp);
}
