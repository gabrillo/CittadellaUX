#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "parm.h"

struct sessione *t;
int main(int argv, char ** argc){

		FILE *fp;
		char *file;
		char id[10];
		char val[200];
		char sum[220];
		int a;

		t=(struct sessione*)malloc(sizeof(struct sessione));
        t->utente=(struct  utente*)("1");
		t->parm_com=1;
	   t->parm_num=1;
		if (argv<1){
				return -1;
		};

		file=argc[1];

		if(!(fp=fopen(file,"r"))){
						return -1;
		}
		while(fscanf(fp,"%10s\t%200s",id,val)>0){
		printf("%s->%s\n\n",id,val);
		strcpy(sum,id);
		strcat(sum," | ");
		strcat(sum,val);
		cmd_parm(t, sum);
		};
		fclose(fp);
		printpar(t);
		return 0;
};
