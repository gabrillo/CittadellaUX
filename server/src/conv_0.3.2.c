/****************************************************************************
*                                                                           *
* File: conv_0.3.1                                                          *
*       Converte la strutture urne da 0.3.1 a 0.3.1                         *
****************************************************************************/
#include <stdio.h>

#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include "cittaserver.h"
#include "extract.h"
#include "memstat.h"
#include "sys_flags.h"
#include "text.h"
#include "conv_urna-old.0.3.2.h"
#include "utente.h"
#include "urna-rw.h"
#include "urna-file.h"
#include "urna-strutture.h"
#include "utility.h"
extern FILE *stderr;

FILE *logfile;
extern void conv_urne();
extern void old_rs_load_data();
extern void conv_stat();
struct urna_stat ustat;
struct old_urna_stat old_ustat;
struct old_urna *old_urna_testa;
struct old_urna *old_urna_ultima;       /*ultima lista creata */
char new[100];
int bk();

#define URNA_NSLOTS 10          /* Numero di slot da allocare per volta */

int main()
{
        printf ("Effettua l'upgrade alla versione 0.3.2 di urne" 
                "Cittadella/UX dalla versione precedente.\nVado?\n");

        logfile=fopen("./urnaconv","a+");
        clogf("----\n\n");
        getchar(); 
        sprintf(new, "%s/...", URNA_DIR);
        printf("from %s to:%s\n", URNA_DIR, new);
        printf("leggo i vecchi...\n");
        old_rs_load_data();
        /*
         * trasferisce i dati
         */
        printf("li converto...\n");
        conv_stat();
        conv_urne();
   
        /*
         * salva il nuovi file
         */
        printf("starting bk...\n");

        if(bk()){
                printf("non posso fare i bk..."
                       "\n\n"
                       "-------------------------------------\n"
                       "CONVERSIONE -----------NON--------- EFFETTUATA\n"
                       "-------------------------------------\n"
                       );
                exit(-1);
	}

        printf("salvo i nuovi\n");


        rs_save_data(1);

        return 0;
};

int bk(){
        DIR *dir;
        char *ffile;
        char *file;
        char *cpfile;
        struct dirent  * dt;
        struct stat *stfile;
        int len,maxlen=0;
        int numfile=0;
        int i=0;
        char **nomifile;

        stfile=(struct stat *)malloc(sizeof(struct stat));
        if(!(dir=opendir(URNA_DIR))){
                printf("errore! %s",strerror(errno));
                return -1;
        };

        while((dt=readdir(dir))){
                numfile++;
                len=strlen(dt->d_name);
                if(maxlen<len)
                        maxlen=len;
        }
        
	nomifile=calloc(numfile,sizeof(char *));
        file=calloc(maxlen+1,sizeof(char));
        ffile=calloc(maxlen+strlen(URNA_DIR)+6,sizeof(char));
        cpfile=calloc(maxlen+strlen(URNA_DIR)+6,sizeof(char));

        rewinddir(dir);
        i=0;
        while((dt=readdir(dir))){
                *(nomifile+i)=calloc(maxlen+1,sizeof(char));
                strncpy(*(nomifile+i),dt->d_name,maxlen);
                i++;
                if(i>numfile){
                        printf("numero di file cambiato in %s!\n", URNA_DIR);
                        return -1;
                }
        }
        closedir(dir);
        for(i=0;i<numfile;i++){
                strncpy(file,*(nomifile+i),maxlen);
                strcpy(ffile,URNA_DIR);
                strcat(ffile,"/");
                strcat(ffile,file);
                if(stat(ffile,stfile)){
                        printf("%s:%s\n",ffile,strerror(errno));
                        free(stfile);
                        return -1;
                };
		if((stfile->st_mode & S_IFMT) != S_IFREG){
                        continue;
                }
                
                strcpy(cpfile,URNA_DIR);
                strcat(cpfile,"/old-");
                strncat(cpfile,file,maxlen);
                printf("%s->%s\n",ffile,cpfile);
                if(rename(ffile,cpfile)){
                        printf("errore! %s",strerror(errno));
                        printf("errore!");
                        return -1;
                }
        };
        free(stfile);
        return 0;
}
