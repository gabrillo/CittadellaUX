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
* Cittadella/UX client                   (C) M. Caldarelli and R. Vianello  *
*                                                                           *
* File : utility.h                                                          *
*        routines varie, manipolazione stringhe, prompts,  etc etc..        *
****************************************************************************/
#ifndef _UTILITY_H
#define _UTILITY_H   1
#include <stddef.h>
#include <time.h>

#define DELTA_TIME 1000
#define TEMPO_DEF 60*60*24*7
#define TEMPO_RAND 4*60*60*24

#define NUM_DIGIT 20 /* Max num cifre nei numeri presi da tastiera */
#define SI "S&iacute"
#define NO "No"

#define CLRLINE { printf("\r%79s\r", ""); }

/* Prototipi delle funzioni in utility.c */
int new_str_M(char *prompt, char *str, int max);
int new_str_m(char *prompt, char *str, int max);
int new_str_def_M(char *prompt, char *def, char *str, int max);
int new_str_def_m(char *prompt, char *def, char *str, int max);
void get_number(char *str, char neg, char enter);
int get_int(char enter);
long get_long(char enter);
unsigned long get_ulong(char enter);
int new_int(char *prompt);
long new_long(char *prompt);
unsigned long new_ulong(char *prompt);
int new_int_def(char *prompt, int def);
int new_sint_def(char *prompt, int def);
int new_date(struct tm *data, int pos);
long new_long_def(char *prompt, long def);
void hit_any_key(void);
void print_ok(void);
void print_on_off(int a);
char * print_si_no(int a);
char si_no(void);
int new_si_no_def(char *prompt, int def);
void Beep(void);
void us_sleep(unsigned int n);
void delchar(void);
void delnchar(int n);
void putnspace(int n);
void cleanline(void);
char *space2under(char *stringa);
char * find_filename(char *path, size_t len, char *filename);
char * find_extension(char *path, size_t len, char *extension);
void strdate(char *str, long ora);
int stampa_data(time_t ora);
int stampa_data_breve(time_t ora);
void stampa_datal(long ora);
void stampa_giorno(long ora);
void stampa_datall(long ora);
void stampa_data_smb(long ora);
void stampa_ora(long ora);
int min_lungh(char *str , int min);
char * astrcat(char *str1, char *str2);

/*
inline void Perror(const char *str);
inline void * Calloc(size_t num, unsigned long size, int tipo);
inline void * Realloc(void *ptr, size_t size);
inline void Free(void *ptr);
*/

/* Variabili esterne */
extern int serv_sock;
extern char uflags[8];

#if 0 /* Per ora usiamo le funzioni definite inline... */
/*
 * Wrap Macro per la funzioni perror(), calloc(), realloc() e free().
 */
#define Perror                    perror
#define Calloc(NUM, SIZE, TIPO)   calloc((NUM), (SIZE))
#define Realloc                   realloc
#define Free                      free
#endif

/**************************************************************************/

#endif /* utility.h */
