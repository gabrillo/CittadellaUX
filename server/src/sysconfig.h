/*
 *  Copyright (C) 1999-2000 by Marco Caldarelli and Riccardo Vianello
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
/*****************************************************************************
* Cittadella/UX server                   (C) M. Caldarelli and R. Vianello   *
*                                                                            *
* File: sysconfig.h                                                          *
*       comandi per visualizzare/modificare la configurazione del sistema.   *
*****************************************************************************/
#ifndef _SYSCONFIG_H
#define _SYSCONFIG_H   1
#include "strutture.h"
#include "room_flags.h"

/**********     Variabili salvate in sysconfig      *************/
/* Configurazione del sistema */
extern int nameserver_lento;
/* Opzioni relative al crashsave */
extern int auto_save;
extern int tempo_auto_save;
/* Opzioni relative all'uso della BBS */
extern int livello_alla_creazione;
extern int express_per_tutti;
extern int min_msgs_per_x;
/* Opzioni di timeout */
extern int butta_fuori_se_idle;
extern int min_idle_warning;
extern int min_idle_logout;
extern int max_min_idle;
extern int login_timeout_min; 
extern struct dati_server dati_server;

/* Prototipi funzioni in sysconfig.c */

int si_no(char *buf);
void legge_configurazione(void);
void sysconfig(char cmd[][256], char val[][256], int n_cmd);
void cmd_rsys(struct sessione *t, char *cmd);
void cmd_esys(struct sessione *t, char *buf);
void cmd_rslg(struct sessione *t);
void cmd_rsst(struct sessione *t);

#endif /* sysconfig.h */
