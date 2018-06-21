/*
 *  Copyright (C) 1999-2005 by Marco Caldarelli and Riccardo Vianello
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
/****************************************************************************
* Cittadella/UX client                   (C) M. Caldarelli and R. Vianello  *
*                                                                           *
* File : pager.h                                                            *
*        Pager per la lettura dei testi.                                    *
****************************************************************************/
#ifndef _PAGER_H
#define _PAGER_H   1
#include "text.h"

extern long prompt_xlog_n;      /* Numero messaggio diario corrente.    */

/* Prototipi delle funzioni in messaggi.c */
void pager(struct text *txt, char send, long max, int start, int cml,
           int spoiler, Metadata_List *mdlist);

/* Macro per i vari utilizzi del pager */
#define txt_pager(TXT)            pager((TXT), 0, 0, 0, FALSE, FALSE, NULL)
#define msg_pager(TXT, LEN, START, CML, SPOILER, MDLIST)  pager((TXT), 1, (LEN)+(START), (START), (CML), (SPOILER), (MDLIST))
#define file_pager(TXT)           pager((TXT), 1, 0, 0, FALSE, FALSE, NULL)

#endif /* pager.h */
