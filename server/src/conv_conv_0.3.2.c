#include "urna-strutture.h"
#include "conv_urna-old.0.3.2.h"
#include "string.h"
#include "urna_comune.h"


void conv_conf (struct urna_conf *ucf, struct old_urna *old_u);
void conv_voci (struct urna_conf *ucf, struct old_urna *old_u);
void conv_voti (struct urna *u, struct old_urna_data *old_udt);
void conv_dati (struct urna *u, struct old_urna *old_u);
void conv_urne ();
unsigned char lettera_successiva ();

void
conv_stat ()
{
  int i;
  ustat.attive = old_ustat.attive;
  ustat.complete = old_ustat.complete;
  ustat.modificata = 1;
  ustat.voti = old_ustat.voti;
  ustat.totali = ustat.complete + ustat.attive;
  ustat.ultima_urna = old_ustat.num;
  ustat.ultima_urna++;
  ustat.ultima_lettera = 0;
  ustat.urne_slots = ustat.attive / LEN_SLOTS + 1;
  ustat.urna_testa =
    (struct urna **) calloc (ustat.urne_slots * LEN_SLOTS,
			     sizeof (struct urna *));
  for (i = 0; i < ustat.urne_slots * LEN_SLOTS; i++)
    {
      *(ustat.urna_testa + i) = NULL;
    }
  ustat.votanti = (struct votanti *) calloc (1, sizeof (struct votanti));
  ustat.votanti->num = -1;
  ustat.votanti->utente = -1;
  ustat.votanti->inizio = -1;
  ustat.votanti->next = NULL;
}

/*
 * 
 */
void
conv_urne ()
{
  int pos = 0;
  int num;
  struct old_urna *old_urna;
  struct old_urna *testa;
  struct urna *urna;

  for (testa = old_urna_testa; testa; testa = testa->next)
    {

      old_urna = testa;
      num = old_urna->num;
      *(ustat.urna_testa + pos) =
	(struct urna *) calloc (1, sizeof (struct urna));
      urna = *(ustat.urna_testa + pos);
      urna->progressivo = num;
      urna->semaf |= SEM_U_ATTIVA;
      urna->conf = (struct urna_conf *) calloc (1, sizeof (struct urna_conf));
      urna->dati = (struct urna_dati *) calloc (1, sizeof (struct urna_dati));
      urna->prop = NULL;	/* tanto non ci sono ! */
      conv_conf (urna->conf, old_urna);
      conv_dati (urna, old_urna);
      pos++;
    }
}


void
conv_conf (struct urna_conf *ucf, struct old_urna *old_u)
{

  struct old_urna_data *old_udt;
  int i;

  old_udt = old_u->data;
  ucf->progressivo = old_u->num;
  if ((ucf->lettera = lettera_successiva ()) == 255)
    {
      printf ("\n????]\n");
    }
  ucf->proponente = old_udt->proponente;
  ucf->room_num = old_udt->room_num;
  ucf->start = old_udt->start;
  ucf->stop = old_udt->stop;
  strncpy (ucf->titolo, old_udt->titolo, LEN_TITOLO);

		  /***/
  memcpy (ucf->testo, old_udt->testo,
	  MAXLEN_QUESITO * LEN_TESTO * sizeof (char));
  for(i=0;i<MAXLEN_QUESITO;i++){
      printf("%ld:%d:%s ",ucf->progressivo,i,ucf->testo[i]);
      if(strncmp(ucf->testo[i],"@!@!",4)==0){
			  printf("tolto");
	      strcpy(ucf->testo[i],"");
	  }
	  printf("\n");
  }
  ucf->num_prop = 4;
  ucf->num_voci = old_udt->num_voci;
  /* 
   * problemino con
   * ref=si/no
   */
  if (ucf->num_voci == 0)
    ucf->num_voci = 2;
  if (ucf->num_voci == 0)
    ucf->max_voci = old_udt->max_voci;
  if (ucf->max_voci==0){
	  ucf->max_voci=1;
  };
  ucf->maxlen_prop = 20;
  ucf->tipo = old_udt->tipo;
  ucf->modo = old_udt->modo;
  if(ucf->modo==MODO_VOTAZIONE){
		  ucf->max_voci=ucf->num_voci;
  };
  ucf->bianca = old_udt->bianca;
  ucf->astensione = old_udt->astensione_scelta;
  ucf->crit = old_udt->criterio;
  if (ucf->crit == CV_ANZIANITA)
    ucf->val_crit.tempo = (time_t) old_udt->anzianita;
  else
    ucf->val_crit.numerico = old_udt->val_crit;
  conv_voci (ucf, old_u);
}

void
conv_voci (struct urna_conf *ucf, struct old_urna *old_u)
{
  int i = 0;
  struct old_urna_data *old_udt;
  struct old_voce *t_v;
  old_udt = old_u->data;

  ucf->voci = (char **) calloc (ucf->num_voci, sizeof (char *));
  for (i = 0; i < ucf->num_voci; i++)
    {
      t_v = old_udt->testa_voci + i;
      *(ucf->voci + i) = (char *) calloc (MAXLEN_VOCE + 1, sizeof (char));
      strncpy (*(ucf->voci + i), t_v->scelta, MAXLEN_VOCE);
    }
}

void
conv_dati (struct urna *u, struct old_urna *old_u)
{

  struct urna_dati *udt;
  struct old_urna_data *old_udt;
  old_udt = old_u->data;
  udt = u->dati;

  udt->stop = old_udt->stop;
  udt->posticipo = old_udt->posticipo;
  udt->bianche = old_udt->bianche;
  udt->nvoti = old_udt->nvoti;
  udt->num_voci = old_udt->num_voci;
  udt->voti_nslots = udt->nvoti / LEN_SLOTS + 1;
  udt->uvot = (long *) calloc (udt->voti_nslots * LEN_SLOTS, sizeof (long));
  memcpy (udt->uvot, old_udt->uvot, udt->nvoti * sizeof (long));
/* qui nvoti e` giusto... prche non so quanti sono */
  udt->coll_nslots = udt->nvoti / LEN_SLOTS + 1;
  udt->ucoll = (long *) calloc (udt->voti_nslots * LEN_SLOTS, sizeof (long));
  memcpy (udt->ucoll, old_udt->uvot, udt->nvoti * sizeof (long));
  conv_voti (u, old_udt);
}

void
conv_voti (struct urna *u, struct old_urna_data *old_udt)
{
  int i = 0;
  struct old_voce *voce;
  struct urna_voti *uvt;
  struct urna_dati *udt;
  struct urna_conf *ucf;
  udt = u->dati;
  ucf = u->conf;

  udt->voti =
    (struct urna_voti *) calloc (udt->num_voci, sizeof (struct urna_voti));


  for (i = 0; i < ucf->num_voci; i++)
    {
      voce = old_udt->testa_voci + i;
      uvt = (udt->voti) + i;
      uvt->num_voti = voce->num_voti;
      uvt->tot_voti = voce->tot_voti;
      uvt->astensioni = voce->astensioni;
    }
}
unsigned char
lettera_successiva ()
{
  /* */
  struct urna *testa;
  unsigned char lettera;
  int i;
  unsigned char usate[32] = "";	/* 
				 * * * elenco delle lettere usate       v9...a1 d c b a
				 * * * lo 0 e il 255 non sono usati
				 */

  for (i = 0; i < ustat.urne_slots * LEN_SLOTS; i++)
    {
      testa = *(ustat.urna_testa + i);
      if (testa != NULL)
	{
	  lettera = testa->conf->lettera;
	  usate[lettera / 8] |= 1 << (lettera % 8);
	}
    }


  /* 
   * * lettera vale 'a', ma se l'ultima lettera
   * * utilizzata e` una lettera singola <z, allora
   * * e` la successiva.
   */

  lettera = 1;

  if (ustat.ultima_lettera <= 25)
    {
      lettera = ustat.ultima_lettera + 1;
    };

  /* 
   * * ciclo su tutto l'alfabeto, partendo da ultima_lettera+1
   */

  for (; lettera <= 26; lettera++)
    if ((usate[lettera / 8] & (1 << (lettera % 8))) == 0)
      {
	ustat.ultima_lettera = lettera;
	return lettera;
      }

  /* 
   * *superata la z si torna da capo (da a) e si attraversa
   * * tutto da a...z a1...z1.... a9  u9
   */

  for (lettera = 1; lettera < 255; lettera++)
    if ((usate[lettera / 8] & (1 << (lettera % 8))) == 0)
      {
	ustat.ultima_lettera = lettera;
	return lettera;
      }

  return 255;
};
