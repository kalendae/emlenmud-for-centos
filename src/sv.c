#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <malloc.h>

#ifndef WINDOWS
#include <sys/errno.h>
#include <signal.h>
#endif


#include "emlen.h"
extern int _filbuf (FILE *);

#define MAX_NESTING 100
static SINGLE_OBJECT *rgObjNest[MAX_NESTING];
void fwrite_char (CHAR_DATA * ch, FILE * fp);
void fwrite_storage_objs (CHAR_DATA * ch, FILE * fp);
void fwrite_obj (CHAR_DATA * ch, SINGLE_OBJECT * obj, FILE * fp, int iNest);
void fread_char (CHAR_DATA * ch, FILE * fp);
void fread_obj (CHAR_DATA * ch, FILE * fp, int storage);

void 
natural_mana (CHAR_DATA * ch)
{
  int base;
  char buffer[500];

  if (IS_MOB (ch))
    return;
  base = get_curr_int(ch) + 2 + get_curr_wis(ch) +
    (((LEVEL (ch) / 3) * (get_curr_int (ch) +
			  get_curr_wis (ch))) / 9);
  if (IS_AUGMENTED(ch, AUG_MANA_BOOST))
    base = (12*base)/10;
  base = (base * (race_info[ch->pcdata->race].mana_percent + align_info[ch->pcdata->alignment].mana_percent))/100;

  ch->pcdata->n_max_mana = base; 

  //sprintf(buffer,"\x1B[1;30m-->\x1B[1;37mYou now have \x1B[1;34m%d max mana\x1B[0;37m\n\r",base);
  //send_to_char(buffer,ch);

  return;
}

void 
write_fixed_values (SINGLE_OBJECT * obj, FILE * fp)
{
  int val[10];
  int i;
  for (i = 0; i < 10; i++)
    val[i] = obj->pIndexData->value[i];
  if (obj->pIndexData->item_type == ITEM_REPELLANT)
    {
      I_SPRAY *sp = (I_SPRAY *) obj->more;
      val[0] = sp->sprays;
    }
  else if (obj->pIndexData->item_type == ITEM_ARMOR)
    {
      I_ARMOR *arm = (I_ARMOR *) obj->more;
      val[7] = arm->condition_now;
      val[6] = arm->max_condition;
      val[0] = arm->protects_legs;
      val[1] = arm->protects_body;
      val[2] = arm->protects_head;
      val[3] = arm->protects_arms;
    }
  else if (obj->pIndexData->item_type == ITEM_WEAPON)
    {
      I_WEAPON *wep = (I_WEAPON *) obj->more;
      val[0] = wep->damage_p;
      val[1] = wep->strength;
      val[7] = wep->firstdice;
      val[2] = wep->seconddice;
      val[4] = wep->ammo_type;
    }
  else if (obj->pIndexData->item_type == ITEM_DRINK_CON)
    {
      I_DRINK *dr = (I_DRINK *) obj->more;
      val[0] = dr->liquid_now;
      val[2] = dr->liquid_type;
      val[3] = dr->not_poison;
    }
  else if (obj->pIndexData->item_type == ITEM_LIGHT)
    {
      I_LIGHT *lg = (I_LIGHT *) obj->more;
      val[3] = lg->light_lit;
      val[0] = lg->light_now;
    }
  else if (obj->pIndexData->item_type == ITEM_TOOL)
    {
      I_TOOL *tl = (I_TOOL *) obj->more;
      val[1] = tl->uses;
    }
  else if (obj->pIndexData->item_type == ITEM_GEM)
    {
      I_GEM *gem = (I_GEM *) obj->more;
      val[0] = gem->gem_type;
      val[1] = gem->mana_now;
      val[2] = gem->max_mana;
      val[3] = gem->max_level;
    }
  else if (obj->pIndexData->item_type == ITEM_WAND)
    {
      I_WAND *wnd = (I_WAND *) obj->more;
      val[2] = wnd->current_charges;
    }
  else if (obj->pIndexData->item_type == ITEM_STAFF)
    {
      I_WAND *stf = (I_WAND *) obj->more;
      val[2] = stf->current_charges;
    }
  else if (obj->pIndexData->item_type == ITEM_CONTAINER)
    {
      I_CONTAINER *cnt = (I_CONTAINER *) obj->more;
      val[1] = cnt->flags;
      val[8] = cnt->money;
    }
  else if (obj->pIndexData->item_type == ITEM_POTION || obj->pIndexData->item_type == ITEM_SCROLL || obj->pIndexData->item_type == ITEM_PILL)
    {
      I_POTION *pot = (I_POTION *) obj->more;
      val[0] = pot->spell_level;
      val[1] = pot->spells[0];
      val[2] = pot->spells[1];
      val[3] = pot->spells[2];
    }
  
  fprintf (fp, "%d %d %d %d %d %d %d %d %d %d ",
	   val[0], val[1], val[2], val[3],
	   val[4], val[5], val[6], val[7],
	   val[8], val[9]);
  return;
}

void 
conv_height (CHAR_DATA * ch)
{
  if (ch->height > 0)
    return;			/*Only find heights for those who have -1 and 0 height */
  if (IS_PLAYER (ch))		/*If the player is a character */
    {
      ch->height = number_range (race_info[ch->pcdata->race].height_min, race_info[ch->pcdata->race].height_max);
      return;
    }
  switch (ch->pIndexData->mobtype)
    {
    case MOB_HUMAN:
      ch->height = number_range (53, 81);
      break;
    case MOB_ELF:
      ch->height = number_range (58, 88);
      break;
    case MOB_DWARF:
      ch->height = number_range (41, 66);
      break;
    case MOB_GNOME:
      ch->height = number_range (38, 58);
      break;
    case MOB_GIANT:
      ch->height = number_range (86, 150);
      break;
    case MOB_DRAGON:
      ch->height = number_range (178, 408);
      break;
    case MOB_CANINE:
      ch->height = number_range (23, 48);
      break;
    case MOB_RODENT:
      ch->height = number_range (12, 30);
      break;
    case MOB_INSECT:
      ch->height = number_range (12, 30);
      break;
    case MOB_UNDEAD:
    case MOB_GNOLL:
    case MOB_BUGBEAR:
    case MOB_TROGLODYTE:
      ch->height = number_range (47, 89);
      break;
    case MOB_GOLEM:
    case MOB_OGRE:
      ch->height = number_range (77, 145);
      break;
    case MOB_DEMON:
      ch->height = number_range (60, 95);
      break;
    case MOB_TROLL:
      ch->height = number_range (65, 120);
      break;
    case MOB_BIRD:
      ch->height = number_range (20, 40);
      break;
    case MOB_FISH:
      ch->height = number_range (12, 38);
      break;
    case MOB_STATUE:
      ch->height = number_range (50, 100);
      break;
    case MOB_FELINE:
      ch->height = number_range (23, 48);
      break;
    case MOB_PLANT:
      ch->height = number_range (60, 150);
      break;
    case MOB_GENERAL_ANIMAL:
      ch->height = number_range (14, 35);
      break;
    case MOB_FAERIE:
      ch->height = number_range (6, 20);
      break;
    case MOB_REPTILE:
      ch->height = number_range (8, 30);
      break;
    case MOB_GHOST:
      ch->height = number_range (53, 80);
      break;
    case MOB_OTHER:
      ch->height = number_range (53, 81);
      break;
    
    
    case MOB_ORC:
      ch->height = number_range (52, 71);
      break;
  
    case MOB_DUMMY:
      ch->height = number_range (51, 70);
      break;
    case MAX_MOB_TYPE:
      ch->height = number_range (51, 75);
      break;
    }
  return;
}


void 
load_corpse (CHAR_DATA * real_ch)
{
  char strsave[SML_LENGTH];
  CHAR_DATA *ch;
  FILE *fp;
  bool found;
  char buffy[500];
  SINGLE_OBJECT *corpsecheck;
/*Scan world if corpse already exists */
  sprintf (buffy, "corpse of %s", NAME (real_ch));
  for (corpsecheck = object_list; corpsecheck && corpsecheck != NULL; corpsecheck = corpsecheck->next)
    {
      if (!str_suffix (buffy, STRR (corpsecheck, short_descr)) ||
	  !str_suffix (STRR (corpsecheck, short_descr), buffy))
	return;
    }
/*Okay, if it existed, function would have returned */
  found = FALSE;
#ifndef WINDOWS
//  fclose (fpReserve);
#endif
  sprintf (strsave, "%s%s.cor", PLAYER_DIR,
	   capitalize (real_ch->pcdata->name));
  if ((fp = fopen (strsave, "r")) == NULL)
    {
      sprintf (strsave, "%s%s.cor", PLAYER_DIR,
	       capitalize (real_ch->pcdata->name));
    }
  else
    {
      fclose (fp);
    }
  if ((fp = fopen (strsave, "r")) == NULL)
    return;
  else
    {
      int iNest;
      ch = new_char ();
      ch->pcdata = new_pc ();
      ch->desc = NULL;
      free_string (ch->pcdata->name);
      ch->pcdata->name = str_dup (NAME (real_ch));
      for (iNest = 0; iNest < MAX_NESTING; iNest++)
	rgObjNest[iNest] = NULL;
      found = TRUE;
      for (;;)
	{
	  char letter;
	  char *word;
	  letter = fread_letter (fp);
	  if (letter == '*')
	    {
	      fread_to_eol (fp);
	      continue;
	    }
	  if (letter != '#')
	    {
	      bug ("Load_char_obj: # not found.", 0);
	      break;
	    }
	  word = fread_word (fp);
	  if (!str_cmp (word, "PLAYER"))
	    fread_char (ch, fp);
	  else if (!str_cmp (word, "OBJECT"))
	    fread_obj (ch, fp, 0);
	  else if (!str_cmp (word, "END"))
	    break;
	  else
	    {
	      bug ("Load_char_obj: bad section.", 0);
	      break;
	    }
	}
      
      fclose (fp);
    }
#ifndef WINDOWS
//  fpReserve = fopen (NULL_FILE, "r");
#endif
  if (ch->pcdata->temp_room == NULL)
    ch->pcdata->temp_room = get_room_index(ch->pcdata->alignment +100);
  char_to_room (ch, ch->pcdata->temp_room);
  make_corpse (ch);
  extract_char (ch, TRUE);
  return;
}

void 
save_corpse (CHAR_DATA * ch)
{
  char strsave[SML_LENGTH];
  FILE *corpsefile;
  if (IN_BATTLE (ch))
    return;
  if (IS_MOB (ch) || LEVEL (ch) < 1)
    return;
  sprintf (strsave, "%s%s.cor", PLAYER_DIR,
	   capitalize (ch->pcdata->name));
  if ((corpsefile = fopen (strsave, "w")) == NULL)
    {
      bug ("Save_char_obj: fopen", 0);
      perror (strsave);
    }
  else
    {
      fwrite_char (ch, corpsefile);
      if (ch->carrying != NULL)
	fwrite_obj (ch, ch->carrying, corpsefile, 0);
      fprintf (corpsefile, "#END\n");
    }
  fclose (corpsefile);
  return;
}




CHAR_DATA *chCorrupt=NULL;
FILE *f=NULL;

void segfault_handler();

static void segfault_save_handler() {
  CHAR_DATA *ch = chCorrupt;
  char strsave[SML_LENGTH];
    char szBuf[512];
    FILE *fp;
  fclose(f);
    sprintf(szBuf,"%s%s.bak", PLAYER_DIR, capitalize(ch->pcdata->name));
#ifdef ENCRYPT_PFILES
  sprintf (strsave, "%s%s.u", PLAYER_DIR, capitalize (ch->pcdata->name));
#else
  sprintf (strsave, "%s%s", PLAYER_DIR, capitalize (ch->pcdata->name));
#endif
  if ( (fp=fopen(szBuf, "r")) != NULL ) {
    FILE *fp2;
    if ( (fp2=fopen(strsave,"w")) != NULL ) {
        int a;
	while (!feof(fp)) {
	  a=fgetc(fp);
	  fputc(a, fp2);
        }
	fclose(fp2);
    }
    fclose(fp);
  } 
  
  fprintf(stderr,"FUCK!  Caught corruption of pfile %s!  Restored from .bak\n", ch->pcdata->name);
  exit(5); 
}

void 
save_char_obj (CHAR_DATA * ch)
{
  char strsave[SML_LENGTH];
  FILE *fp;
  chCorrupt=ch;
#ifndef WINDOWS
  signal(SIGSEGV, segfault_save_handler);
#endif

  if (IN_BATTLE (ch) == TRUE)
    return;
  if (ch->in_room && ch->in_room->vnum > 449 && ch->in_room->vnum < 500)
    return;
  if (IS_MOB (ch) || LEVEL (ch) < 0)
    return;
  if (ch->desc != NULL && ch->desc->original != NULL)
    ch = ch->desc->original;
#ifndef WINDOWS
//  fclose (fpReserve);
#endif
#ifdef ENCRYPT_PFILES
  sprintf (strsave, "%s%s.u", PLAYER_DIR, capitalize (ch->pcdata->name));
#else
  sprintf (strsave, "%s%s", PLAYER_DIR, capitalize (ch->pcdata->name));
#endif
  if ( (fp=fopen(strsave, "r")) != NULL ) {
    FILE *fp2;
    char szBuf[512];
    sprintf(szBuf,"%s%s.bak", PLAYER_DIR, capitalize(ch->pcdata->name));
    if ( (fp2=fopen(szBuf,"w")) != NULL ) {
        int a;
	while (!feof(fp)) {
	  a=fgetc(fp);
	  fputc(a, fp2);
        }
	fclose(fp2);
    }
    fclose(fp);
  }
  if ((fp = fopen (strsave, "w")) == NULL)
    {
      bug ("Save_char_obj: fopen", 0);
      perror (strsave);
    }
  else
    {
      f=fp;
      fwrite_char (ch, fp);
      if (ch->carrying != NULL)
	fwrite_obj (ch, ch->carrying, fp, 0);
      fwrite_storage_objs (ch, fp);
      fprintf (fp, "#END\n");
    }
  f=NULL;
  fclose (fp);
#ifdef ENCRYPT_PFILES
  jumble (capitalize (ch->name));
  sprintf (buft, "rm %s &", strsave);
  system (buft);
#endif
#ifndef WINDOWS
//  fpReserve = fopen (NULL_FILE, "r");
  signal(SIGSEGV, segfault_handler);
#endif
  
  chCorrupt=NULL;
  return;
}


void 
fwrite_char (CHAR_DATA * ch, FILE * fp)
{
  AFFECT_DATA *paf;
  CHANNEL *c;
  int i = 0;
  int sn, k;
  ROOM_DATA *tr;
  char sss[500];
  SPELL_DATA *spell;
  check_ced (ch);
  check_fgt (ch);
  tr = ch->in_room;

  ch->pcdata->played+=(int)(current_time - ch->pcdata->logon);

  ch->pcdata->logon=current_time;
  ch->pcdata->new_exp = TRUE;
  sprintf (sss, "%s%s", PLAYER_DIR, capitalize (NAME (ch)));
  fprintf (fp, "#%s\n", IS_MOB (ch) ? "MOB" : "PLAYER");
  if (ch->pcdata->email[0] != '\0')
    fprintf (fp, "NEmail %s~\n", fix_string (ch->pcdata->email));

  if (ch->pcdata->short_descr && ch->pcdata->short_descr[0] != '\0' &&
strlen(ch->pcdata->short_descr) > 2)
    fprintf (fp, "ShDesc %s~\n", ch->pcdata->short_descr);
  if (ch->pcdata->long_descr && ch->pcdata->long_descr[0] != '\0' &&
strlen(ch->pcdata->long_descr) > 2)
    fprintf (fp, "LoDesc %s~\n", fix_string (ch->pcdata->long_descr));
  fprintf (fp, "Descr %s~\n", fix_string (ch->pcdata->description));
  fprintf (fp, "Sex %d\n", SEX (ch));

  for (c = chan_first; c != NULL; c = c->next)
    {
      fprintf (fp, "LCcol %d %s\n", i, ch->pcdata->chan_color[i]);
      i++;
    }
  fprintf (fp, "LCign %d\n", ch->pcdata->chan_ignore);

  fprintf (fp, "TtlKils .%o\n", ch->pcdata->totalkills);
  fprintf (fp, "NAn %d\n", ch->pcdata->show_lev);
  fprintf (fp, "Killpt .%o\n", ch->pcdata->killpoints);
  fprintf (fp, "Deaths %d\n", ch->pcdata->deaths);
  fprintf (fp, "DPray %d\n", ch->pcdata->pray_points);
  fprintf (fp, "DCoins %d %d %d %d %d\n", 0, 0, ch->gold, 0, ch->copper);

  for (k = 0; k < MAX_SCRIPT_FLAGS; k++)
    {
      if (ch->pcdata->script_flags[k] && ch->pcdata->script_flags[k][0] != '\0')
	{
	  fprintf (fp, "Sf %s~\n", ch->pcdata->script_flags[k]);
	}
    }
  fprintf (fp, "Nexp %d\n", ch->pcdata->new_exp);
  fprintf (fp, "Nospam %d\n", ch->pcdata->no_spam);
  if (ch->height != -1)
    fprintf (fp, "NHeight %d\n", ch->height);
  fprintf (fp, "NDon %d\n", ch->pcdata->donated);
  fprintf (fp, "NQuest %d\n", ch->pcdata->questflag);
  fprintf (fp, "NatArm .%o\n", ch->pcdata->nat_armor);
  fprintf (fp, "NatAbil %d\n", ch->pcdata->nat_abilities);
  fprintf (fp, "NatAbilT %d\n", ch->pcdata->nat_abilities2);
  if (ch->ced && ch->ced->wasroom != 0)
    {
   /*   fprintf (fp, "NWasrm %d\n", ch->ced->wasroom); */
    }
  if (ch->pcdata->challenge_on)
    fprintf (fp, "NChon %d\n", ch->pcdata->challenge_on);
  if (ch->pcdata->remort_times)
    fprintf (fp, "NRemort %d\n", ch->pcdata->remort_times);
  fprintf (fp, "NAMsg %d\n", ch->pcdata->arena_msg);
  if (ch->fgt->challenge)
    fprintf (fp, "NChal %d\n", ch->fgt->challenge);
  if (ch->pcdata->guilds)
    fprintf (fp, "NGldBt %d\n", ch->pcdata->guilds);
  fprintf (fp, "NWPS %ld\n", ch->pcdata->warpoints);
  fprintf (fp, "NMana .%o\n", ch->pcdata->n_mana);

  fprintf (fp, "Race %d\n", ch->pcdata->race);
  fprintf (fp, "Level .%o\n", ch->pcdata->level);
  fprintf (fp, "SCool %d\n", ch->pcdata->cool_skills);
  if (ch->pcdata->level > 100)
    {
      fprintf (fp, "Winv %d\n", ch->pcdata->wizinvis);
      fprintf (fp, "Secrty %d\n", ch->pcdata->security);
    }
  fprintf (fp, "Clan %d\n", ch->pcdata->clan);
  fprintf (fp, "Clan2 %d\n", ch->pcdata->clan2);
  
  fprintf (fp, "Played %ld\n", ch->pcdata->played);
  fprintf (fp, "Note %ld\n", ch->pcdata->last_note);
  fprintf (fp, "Room %d\n", (ch->in_room ? ch->in_room->vnum :100+ch->pcdata->alignment));
  fprintf (fp, "HpMove .%lo .%lo .%o .%o\n", ch->hit, ch->max_hit,
	   ch->move, ch->max_move);
  fprintf (fp, "EFight %d\n", ch->ced->fight_ops);
  fprintf (fp, "Exp %ld\n", ch->exp);
  fprintf (fp, "Act %d\n", ch->act);
  fprintf (fp, "Act2 %d\n", ch->pcdata->act2);
  if (ch->pcdata->act3 != 0)
    fprintf (fp, "Act3 %d\n", ch->pcdata->act3);
  fprintf (fp, "AffTWO %d\n", ch->more_affected_by);
  fprintf (fp, "AffBy %d\n", ch->affected_by);
  fprintf (fp, "Positn %d\n",
      ch->position == POSITION_FIGHTING ? POSITION_STANDING : ch->position);
  fprintf (fp, "Learn .%o\n", ch->pcdata->learn);
  fprintf (fp, "Practc .%o\n", ch->pcdata->practice);

  if (ch->pcdata->saving_throw)
    fprintf (fp, "SvngTh %d\n", ch->pcdata->saving_throw);

  fprintf (fp, "Alignmt %d\n", ch->pcdata->alignment);
  fprintf (fp, "NRat %ld\n", ch->max_hit + ch->max_move + ch->exp - ch->hitroll - ch->damroll);
  fprintf (fp, "Hitrl .%o\n", ch->hitroll);
  fprintf (fp, "Damrl .%o\n", ch->damroll);
  fprintf (fp, "Armor .%o\n", ch->armor);
  if (ch->pcdata->deaf)
    fprintf (fp, "Deaf %d\n", ch->pcdata->deaf);

  fprintf (fp, "Wimpy %d\n", ch->ced->wimpy);
  fprintf (fp, "TotWps %ld\n", ch->pcdata->total_wps);
  fprintf (fp, "PkLevs %ld\n", ch->pcdata->pklevels);
  fprintf (fp, "Pkhelp %ld\n", ch->pcdata->pkhelpers);
  fprintf (fp, "Pkills %d\n", ch->pcdata->pkills);
  fprintf (fp, "Pkilled %d\n", ch->pcdata->pkilled);
  fprintf (fp, "Pkers %ld\n", ch->pcdata->pkers);
  fprintf (fp, "Warmth %ld\n", ch->pcdata->warmth);
  fprintf (fp, "Hardpk %d\n", ch->pcdata->hard_pkills);
  fprintf (fp, "Created %ld\n", ch->pcdata->created);
  fprintf (fp, "ManaShield %d\n", ch->pcdata->mana_shield_strength);
  fprintf (fp, "Elemental %d\n", ch->pcdata->elemental_alignment);
  if (IS_MOB (ch))
    {
      fprintf (fp, "Vnum %d\n", ch->pIndexData->vnum);
    }
  else
    {
      int i; 
      fprintf (fp, "Password %s~\n", ch->pcdata->pwd);
      fprintf (fp, "RealPassword %s~\n", ch->pcdata->rpwd);
      if (ch->pcdata->rprompt)
	fprintf (fp, "Promptr %s~\n", fix_string (ch->pcdata->rprompt));
      fprintf (fp, "Pagelen %d\n", ch->pcdata->pagelen);
      fprintf (fp, "PImplO %d\n", ch->pcdata->augments);
      if (LEVEL (ch) > 99)
	{
	  fprintf (fp, "Bamfin %s~\n", ch->pcdata->beamin);
	  fprintf (fp, "Bamfout %s~\n", ch->pcdata->beamout);
	}
      if (ch->pcdata->bank > 0)
	fprintf (fp, "Bank %ld\n", ch->pcdata->bank);

      fprintf (fp, "Title %s~\n", ch->pcdata->title);
      fprintf (fp, "AttrPerm ");
      for (i = 0; i < NUM_STATS; i++)
	{
	  fprintf(fp, ".%o ", ch->pcdata->stat[i]);
	}
      fprintf(fp, "\n");
      fprintf (fp, "AttrMod ");
      for (i = 0; i < NUM_STATS; i++)
	{
	  fprintf(fp, "%d ", ch->pcdata->stat_mod[i]);
	}
	  fprintf(fp, "\n");
	  fprintf (fp, "StartAttr ");
	  for (i = 0; i < NUM_STATS; i++)
	{
	  fprintf(fp, "%d ", ch->pcdata->start_stat[i]);
	}
      fprintf(fp, "\n");
      fprintf (fp, "Condition %d %d %d\n",
	       ch->pcdata->condition[0],
	       ch->pcdata->condition[1],
	       ch->pcdata->condition[2]);
      fprintf (fp, "Colors %d %d %d %d %d %d %d %d %d %d\n",
	       ch->pcdata->colors[0],
	       ch->pcdata->colors[1],
	       ch->pcdata->colors[2],
	       ch->pcdata->colors[3],
	       ch->pcdata->colors[4],
	       ch->pcdata->colors[5],
	       ch->pcdata->colors[6],
	       ch->pcdata->colors[7],
	       ch->pcdata->colors[8],
	       ch->pcdata->colors[9]);
    fprintf (fp, "Stance %d\n", ch->pcdata->stance);
      for (k = 0; k < MAX_TROPHY; k++)
	{
	  if (ch->pcdata->trophy_name[k][0] != '\0')
	    {
	      fprintf (fp, "NTro %s \n", ch->pcdata->trophy_name[k]);
	      fprintf (fp, "TTro %d\n", ch->pcdata->trophy_times[k]);
	      fprintf (fp, "LTro %d\n", ch->pcdata->trophy_level[k]);
	    }
	}
      fprintf(fp, "Faction ");
      for (k=0; k < MAX_MOB_TYPE; k++)
	fprintf(fp, "%d ", ch->pcdata->faction[k]);
      fprintf(fp, "\n");
      
      for (k = 0; k < MAX_IGNORE; k++)
	{
	  if (ch->pcdata->ignore[k])
	    fprintf (fp, "Ignore %s\n", ch->pcdata->ignore[k]);
	}
      for (k = 0; k < MAXALIAS; k++)
	{
	  if (ch->pcdata->aliasname[k] &&
	      ch->pcdata->aliasname[k][0] != '\0' &&
	      ch->pcdata->alias[k] &&
	      ch->pcdata->alias[k][0] != '\0')
	    {
	      char *msk;
	      for (msk = ch->pcdata->alias[k]; *msk != '\0'; msk++)
		if (*msk == '~')
		  *msk = '*';
	      fprintf (fp, "Alias %s\n", ch->pcdata->aliasname[k]);
	      fprintf (fp, "AlCt %s~\n",
		       fix_string (ch->pcdata->alias[k]));
	      for (msk = ch->pcdata->alias[k]; *msk != '\0'; msk++)
		if (*msk == '*')
		  *msk = '~';
	    }
	}
      
      for (sn = 0; sn < SKILL_COUNT; sn++)
	{
	  if ((spell = skill_lookup (NULL, sn)) == NULL)
	    continue;
	  if (ch->pcdata->learned[sn] > 0)
	    {
	      fprintf (fp, "Skill %d '%s'\n",
		       ch->pcdata->learned[sn], spell->spell_funky_name);
	    }
	}
    }
  for (paf = ch->affected; paf && paf != NULL; paf = paf->next)
    {
      if (paf->type < 0 || paf->type >= SKILL_COUNT)
	continue;
      spell = skill_lookup (NULL, paf->type);
      if (spell == NULL)
	continue;
      fprintf (fp, "AffectData '%s' %3d %3d %3d %10d %10d\n",
	       spell->spell_funky_name,
	       paf->duration,
	       paf->modifier,
	       paf->location,
	       paf->bitvector,
	       paf->bitvector2
	);
    }
  fprintf (fp, "End\n\n");
  return;
}

void 
fwrite_obj (CHAR_DATA * ch, SINGLE_OBJECT * obj, FILE * fp, int iNest)
{
  AFFECT_DATA *paf;
  if (obj->next_content != NULL)
    fwrite_obj (ch, obj->next_content, fp, iNest);
  if (IS_SET (obj->extra_flags, ITEM_NOSAVE))
    {
      return;
    }
  fprintf (fp, "#OBJECT\n");
  fprintf (fp, "Nest %d\n", iNest);
  if (TRUE || !str_cmp (obj->short_descr, obj->pIndexData->short_descr))
    {
      fprintf (fp, "AOVN %d ", obj->pIndexData->vnum);
      write_fixed_values (obj, fp);
      fprintf (fp, "%d ", obj->wear_loc);
      fprintf (fp, "%d ", obj->size);
      fprintf (fp, "%d\n", obj->timer);
      for ( paf = obj->affected; paf && paf != NULL; paf = paf->next )
	{
	  fprintf( fp, "AffectData %d %d %d %d %d\n",  paf->duration,  paf->modifier,  paf->location, paf->bitvector, paf->bitvector2 
);
	}
    }
  fprintf (fp, "End\n\n");
  if (obj->contains != NULL)
    fwrite_obj (ch, obj->contains, fp, iNest + 1);
  return;
}


void 
fread_clan_storage (int clan_num)
{
  char clan_fname[100];
  FILE *fp;
  int k;
  CLAN_DATA *clan;
  if ((clan = get_clan_index (clan_num)) == NULL)
    {
      fprintf (stderr, "Ack! Bad clan number passed to fread_clan_storage!\n");
      return;
    }
  for (k = 0; k < MAX_CLAN_STORAGE; k++)
    {
      clan->storage[k] = NULL;
    }
  sprintf (clan_fname, "clan%d.sto", clan_num);
  if ((fp = fopen (clan_fname, "r")) == NULL)
    {
      return;
    }
  for (;;)
    {
      char letter;
      char *word;
      letter = fread_letter (fp);
      if (letter == '*')
	{
	  fread_to_eol (fp);
	  continue;
	}
      if (letter != '#')
	{
	  bug ("Load_char_obj: # not found.", 0);
	  break;
	}
      word = fread_word (fp);
      if (!str_cmp (word, "STORAGE"))
	fread_obj (NULL, fp, clan_num + 1);
      else if (!str_cmp (word, "END"))
	break;
      else
	{
	  bug ("Load_clan_storage: bad section.", 0);
	  break;
	}
    }
  fclose (fp);
  return;
}

void 
fwrite_clan_storage (int clan_num)
{
  FILE *fp;
  CLAN_DATA *clan;
  char clan_fname[500];
  bool foundy;
  SINGLE_OBJECT *obj;
  AFFECT_DATA *paf;
  int k;
  if ((clan = get_clan_index (clan_num)) == NULL)
    {
      fprintf (stderr, "Null Clan Index for fwrite_clan_storage.\n");
      return;
    }
  foundy = FALSE;
  for (k = 0; k < MAX_CLAN_STORAGE; k++)
    {
      if (clan->storage[k] != NULL)
	foundy = TRUE;
    }
  if (!foundy)
    return;
  sprintf (clan_fname, "clan%d.sto", clan_num);
  if ((fp = fopen (clan_fname, "w")) == NULL)
    {
      bug ("Save_clan_storage: fopen", 0);
      perror (clan_fname);
    }
  for (k = 0; k < MAX_CLAN_STORAGE; k++)
    {
      if (clan->storage[k] == NULL || IS_SET (clan->storage[k]->extra_flags, ITEM_NOSAVE))
	continue;
      else
	obj = clan->storage[k];
      fprintf (fp, "#STORAGE\n");
      fprintf (fp, "Nest 0\n");
      fprintf (fp, "AOVN %d ", obj->pIndexData->vnum);
      write_fixed_values (obj, fp);
      fprintf (fp, "%d ", obj->wear_loc);
      fprintf (fp, "%d ", obj->size);
      fprintf (fp, "%d\n", obj->timer);
      for ( paf = obj->affected; paf && paf != NULL; paf = paf->next )
	{
	  fprintf( fp, "AffectData %d %d %d %d %d\n",
		   paf->duration,
		   paf->modifier,
		   paf->location,
		   paf->bitvector,
		   paf->bitvector2
		   );
	}
      fprintf (fp, "End\n\n");
    }
  fprintf (fp, "#END\n\n");
  fclose (fp);
  return;
}


void 
fread_clan_bank (int clan_num)
{
  char clan_fname[100];
  FILE *fp;
  CLAN_DATA *clan;
  if ((clan = get_clan_index (clan_num)) == NULL)
    {
      fprintf (stderr, "Ack! Bad clan number passed to fread_clan_bank!\n");
      return;
    }
  
  sprintf (clan_fname, "clan%d.bnk", clan_num);
  if ((fp = fopen (clan_fname, "r")) == NULL)
    {
      return;
    }
  clan_bank[clan_num].warpoints = fread_number(fp);
  clan_bank[clan_num].gold = fread_number (fp);
  fclose (fp);
  return;
}





void 
fwrite_clan_bank (int clan_num)
{
  FILE *fp;
  CLAN_DATA *clan;
  char clan_fname[500];
  if ((clan = get_clan_index (clan_num)) == NULL)
    {
      fprintf (stderr, "Null Clan Index for fwrite_clan_bank.\n");
      return;
    }
  sprintf (clan_fname, "clan%d.bnk", clan_num);
  if ((fp = fopen (clan_fname, "w")) == NULL)
    {
      bug ("Save_clan_bank: fopen", 0);
      perror (clan_fname);
    }
  fprintf (fp, "%d\n", clan_bank[clan_num].warpoints);
  fprintf (fp, "%d\n", clan_bank[clan_num].gold);
  fclose (fp);
  return;
}

void 
fwrite_align_bank (int align_num)
{
  FILE *fp;
  char align_fname[200];
  if (align_num < 0 || align_num >= NUM_ALIGN)
    {
      fprintf (stderr, "Bad number for fwrite_align_bank.\n");
      return;
    }
  sprintf(align_fname, "align%d.bnk", align_num);
  if ((fp = fopen (align_fname, "w")) == NULL)
    {
      bug ("Save_align_bank: fopen", 0);
      perror (align_fname);
    }
  fprintf (fp, "%d\n", align_bank[align_num].warpoints);
  fprintf (fp, "%d\n", align_bank[align_num].gold);
  fclose (fp);
  return;
}

void 
fwrite_alliance_data (void)
{
  int i; 
  int j;
  char alliance_fname[100];
  FILE *fp;
  sprintf(alliance_fname, "alliance.dat");
  if ((fp = fopen (alliance_fname, "w")) == NULL)
    {
      fprintf(stderr, "BAD WRITE ON ALLIANCE DATA!\n");
      perror(alliance_fname);
    }
  for (i = 0; i < NUM_ALIGN; i++)
    {
      for (j = 0; j < NUM_ALIGN; j++)
	{
	  fprintf(fp, "%d\n", alliance[i][j]);
	}
      alliance[i][i]= TRUE;
    }
  
  
  fclose (fp);
  return;
}


void 
fread_alliance_data (void)
{
  int i; 
  int j;
  char alliance_fname[100];
  FILE *fp;
  sprintf(alliance_fname, "alliance.dat");
  for (i = 0; i < NUM_ALIGN; i++)
    {
      for (j = 0; j < NUM_ALIGN; j++)
	{
	  if (i == j)
	    alliance[i][j] = TRUE;
	  else
	    alliance[i][j] = FALSE;
	  
	}
    }
  if ((fp = fopen (alliance_fname, "r")) == NULL)
    {
      fprintf(stderr, "BAD READ ON ALLIANCE DATA!\n");
      fwrite_alliance_data();
      return;
    }
  for (i = 0; i < NUM_ALIGN; i++)
    {
      for (j = 0; j < NUM_ALIGN; j++)
	{
	  alliance[i][j] = fread_number(fp);
	}
    }
  
  
  fclose (fp);
  fwrite_alliance_data();
  return;
}
void 
fread_align_bank (int align_num)
{
  char align_fname[100];
  FILE *fp;
  if (align_num < 0 || align_num >= NUM_ALIGN)
    {
      fprintf (stderr, "Ack! Bad align number sent to fread_align_bank!\n");
      return;
    }
  sprintf(align_fname, "align%d.bnk", align_num);
  if ((fp = fopen (align_fname, "r")) == NULL)
    {
      return;
    }
  align_bank[align_num].warpoints = fread_number(fp);
  align_bank[align_num].gold = fread_number (fp);
  fclose (fp);
  return;
}






void 
fwrite_storage_objs (CHAR_DATA * ch, FILE * fp)
{
  SINGLE_OBJECT *obj;
  AFFECT_DATA *paf;
  int k;
  if (IS_MOB (ch))
    return;
  for (k = 0; k < MAXST; k++)
    {
      if (ch->pcdata->storage[k] == NULL || IS_SET (ch->pcdata->storage[k]->extra_flags, ITEM_NOSAVE))
	continue;
      else
	obj = ch->pcdata->storage[k];
      fprintf (fp, "#STORAGE\n");
      fprintf (fp, "Nest %d\n", 0);
      if (TRUE || !str_cmp (obj->short_descr, obj->pIndexData->short_descr))
	{
	  fprintf (fp, "AOVN %d ", obj->pIndexData->vnum);
	  write_fixed_values (obj, fp);
	  fprintf (fp, "-1 ");
	  fprintf (fp, "%d ", obj->size);
	  fprintf (fp, "%d\n", obj->timer);
	  for ( paf = obj->affected; paf && paf != NULL; paf = paf->next )
	    {
	      fprintf( fp, "AffectData %d %d %d %d\n",
		       paf->duration,
		       paf->modifier,
		       paf->location,
		       paf->bitvector,
		       paf->bitvector2
		       );
	    } 
	}
      fprintf (fp, "End\n\n");
    }
  return;
}

/*
   * Load a char and inventory into a new ch structure.
 */
bool 
load_char_obj (DESCRIPTOR_DATA * d, char *name, bool ped)
{
  char strsave[SML_LENGTH];
  char strsave2[SML_LENGTH];
  CHAR_DATA *ch;
  CHAR_DATA *tch;
  char *tt;
  FILE *fp;
  bool found;
  char arg1[900];
  char arg2[900];
  arg1[0] = '\0';
  arg2[0] = '\0';
  for (tt = name + 1; *tt != '\0'; tt++)
    {
      if (*tt < 'a')
	*tt = LOWER (*tt);
    }
  
 
  strcpy (arg2, name);
  strcpy (arg2, one_argy (name, arg1));
  if (arg2 == "" || arg2[0] == '\0')
    {
      arg1[0] = '\0';
    }
  else
    {
      char tbuff[900];
      strcpy (tbuff, arg2);
      strcpy (arg2, arg1);
      strcpy (arg1, tbuff);
    }
  if (arg1[0] == '\0')
    strcpy (arg2, name);
  name[0] = UPPER (name[0]);
  if (!ped)
    {
      for (tch = char_list; tch != NULL; tch = tch->next)
	{
	  if (IS_PLAYER (tch) && tch->desc != NULL && !str_cmp (tch->pcdata->name, arg2))
	    {
	      if (!str_cmp(arg1,tch->pcdata->rpwd))
		{
		  tch->desc->connected = CON_PLAYING;
		  close_socket (tch->desc);
		  tch->desc = d;
		  d->character = tch;
		  d->connected = CON_PLAYING;
		  d->repeat = -5;
		  return TRUE;
		} 
	      /* JRAJRA */
	      fprintf (stderr, "%s was online and %s@%s attempted to log in.\n", NAME (tch),   (tch->desc ? tch->desc->username : "nodesc"),
		       (tch->desc ? tch->desc->host : "nodesc"));
	      write_to_descriptor2 (d, "\n\rAlready playing! Type <name> <password> all on one line to break in.\n\r", 0);
	      d->character = tch;
	      tch->pcdata->new_exp = TRUE;
	      return FALSE;
	    }
	  if (IS_PLAYER (tch) && tch->desc == NULL && !strcmp(tch->pcdata->name, arg2))
	    {
	      tch->desc = d;
	      d->character = tch;
	      /*Char used a previously existing char in the game to reconnect. */
	      d->connected = CON_PLAYING;
	      return TRUE;
	    }
	}
    }
  ch = new_char ();
  ch->pcdata = new_pc ();
  if (!ped)
    d->character = ch;
  ch->next = NULL;
  if (!ped)
    ch->desc = d;
  else
    ch->desc = NULL;
  free_string (ch->pcdata->name);
  ch->pcdata->name = str_dup (name);
  ch->pcdata->temp = NULL;
  check_ced (ch);
  check_fgt (ch);
  found = FALSE;
#ifndef WINDOWS
//  fclose (fpReserve);
#endif
#ifdef ENCRYPT_PFILES
  unjumble (capitalize (name), FALSE);
  sprintf (strsave, "%s%s.u", PLAYER_DIR, capitalize (name));
  sprintf (strsave2, "%s%s", PLAYER_DIR, capitalize (name));
#else
  sprintf (strsave, "%s%s", PLAYER_DIR, capitalize (name));
  sprintf (strsave2, "%s%s.u", PLAYER_DIR, capitalize (name));
#endif
  if ((fp = fopen (strsave, "r")) != NULL
      || (fp = fopen (strsave2, "r")) != NULL)
    {
      int iNest;
      for (iNest = 0; iNest < MAX_NESTING; iNest++)
	rgObjNest[iNest] = NULL;
      found = TRUE;
      for (;;)
	{
	  char letter;
	  char *word;
	  letter = fread_letter (fp);
	  if (letter == '*')
	    {
	      fread_to_eol (fp);
	      continue;
	    }
	  if (letter != '#')
	    {
	      bug ("Load_char_obj: # not found.", 0);
	      break;
	    }
	  word = fread_word (fp);
	  if (!str_cmp (word, "PLAYER"))
	    {
	      fread_char (ch, fp);
	    }
	  else if (!str_cmp (word, "OBJECT"))
	    {
	      if (is_equipwipe)
		break;
	      fread_obj (ch, fp, 0);
	    }
	  else if (!str_cmp (word, "STORAGE"))
	    {
	      if (is_equipwipe)
		break;
	      fread_obj (ch, fp, 1);
	    }
	  else if (!str_cmp (word, "END"))
	    break;
	  else
	    {
	      bug ("Load_char_obj: bad section.", 0);
	      break;
	    }
	}
      fclose (fp);
    }
#ifndef WINDOWS
//  fpReserve = fopen (NULL_FILE, "r");
#endif
  if (found)
    {
      pedit_found = ch;
      conv_height (ch);
      fix_char (ch);
    }
#ifdef ENCRYPT_PFILES
  sprintf (buf, "rm %s &", strsave);
  system (buf);
#endif

  return found;
}

/*
   * Read in a char.
 */
#if defined(SKEY)
#undef SKEY
#endif
#define SKEY( literal, field, value )  if ( !str_cmp(word,(literal))) {(field)=str_dup((value));fMatch=TRUE;break;}
#if defined(KEY)
#undef KEY
#endif
#define KEY( literal, field, value )  if ( !str_cmp(word,(literal))) {(field)=(value);fMatch=TRUE;break;}


void 
fread_rest (CHAR_DATA * ch)
{
  char *word;
  bool fMatch;
  int dummy;
  int i;
  int k;
  FILE *fpt;
  char bufh[100];
  char sss[500];
  check_ced (ch);
  check_fgt (ch);
  sprintf (sss, "%s%s", PLAYER_DIR, (capitalize (NAME (ch))));
  if ((fpt = fopen (sss, "r")) == NULL)
    {
      return;
    }
  for (;;)
    {
      word = feof (fpt) ? "End" : fread_word (fpt);
      fMatch = FALSE;
      switch (UPPER (word[0]))
	{
	case '*':
	  fMatch = TRUE;
	  fread_to_eol (fpt);
	  break;
	case 'A':
	  if (!str_cmp(word, "Act"))
	    {
	      ch->act = fread_number(fpt);
	      fMatch = TRUE;
	      if (IS_SET(ch->act, ACT_CRASHER))
		{
		  if(ch->desc)
		    SET_BIT(ch->desc->act, DESC_CRASHER);
		}
	    }
	  KEY ("Act2", ch->pcdata->act2, fread_number (fpt));
	  KEY ("Act3", ch->pcdata->act3, fread_number (fpt));
	  KEY ("AffBy", ch->affected_by, fread_number (fpt));
	  KEY ("AffTWO", ch->more_affected_by, fread_number (fpt));
	  if (!str_cmp(word, "Alignmt"))
	    {
	      ch->pcdata->alignment = fread_number(fpt);
	      if (ch->pcdata->alignment < 0) ch->pcdata->alignment = 1;
	      if (ch->pcdata->alignment >= NUM_ALIGN) ch->pcdata->alignment = 2;
	      fMatch = TRUE;
	    }
	  KEY ("Armor", ch->armor, fread_number (fpt));
	  if (!str_cmp (word, "Alias"))
	    {
	      char *msk;
	      for (k = 0; k < MAXALIAS; k++)
		{
		  if (!ch->pcdata->aliasname[k] ||
		      ch->pcdata->aliasname[k][0] == '\0' ||
		      !ch->pcdata->alias[k] ||
		      ch->pcdata->alias[k][0] == '\0')
		    {
		      break;
		    }
		}
	      ch->pcdata->aliasname[k] = str_dup (fread_word (fpt));
	      fread_word (fpt);
	      ch->pcdata->alias[k] = str_dup (fread_string2 (fpt, NULL));
	      for (msk = ch->pcdata->alias[k]; *msk != '\0'; msk++)
		if (*msk == '*')
		  *msk = '~';
	      fMatch = TRUE;
	    }
	  if (!str_cmp (word, "AffectData"))
	    {
	      AFFECT_DATA *paf;
	      if (affect_free == NULL)
		{
		  paf = mem_alloc (sizeof (*paf));
		}
	      else
		{
		  paf = affect_free;
		  affect_free = affect_free->next;
		}
	      if (!str_cmp (word, "Affect"))
		{
		  /* Obsolete 2.0 form. */
		  paf->type = fread_number (fpt);
		}
	      else
		{
		  int sn;
		  sn = lookup_gsn (fread_word (fpt));
		  if (sn < 0)
		    bug ("Fread_char: unknown skill.", 0);
		  else
		    paf->type = sn;
		}
	      paf->duration = fread_number (fpt);
	      paf->modifier = fread_number (fpt);
	      paf->location = fread_number (fpt);
	      paf->bitvector = fread_number (fpt);
	      paf->bitvector2 = fread_number (fpt);
	      paf->next = ch->affected;
	      ch->affected = paf;
	      fMatch = TRUE;
              /* ------------------------------ */
              /*         Restore affect         */
	      /* ------------------------------ */
	      if(paf->type == gsn_manashield)
                renew_affect(ch,paf);
	      else if (paf->type == gsn_fireshield)
                renew_affect(ch,paf);

	      break;
	    }
	  if (!str_cmp (word, "AttrMod"))
	    {
	      int i;
	      for (i = 0; i < NUM_STATS; i++)
		ch->pcdata->stat_mod[i] = fread_number(fpt);
	      fMatch = TRUE;
	      break;
	    }
	  if (!str_cmp (word, "AttrPerm"))
	    {
	      int i;
	      for (i = 0; i < NUM_STATS; i++)
		ch->pcdata->stat[i] = fread_number(fpt);
	      fMatch = TRUE;
	      break;
	    }
	  break;
	case 'B':
	  if (!str_cmp (word, "Bank"))
	    {
	      ch->pcdata->bank = fread_number (fpt);
	      fMatch = TRUE;
	    }
	  SKEY ("Bamfin", ch->pcdata->beamin, fread_string2 (fpt, NULL));
	  SKEY ("Bamfout", ch->pcdata->beamout, fread_string2 (fpt, NULL));
	
	  break;
	case 'C':
	  KEY ("Clan", ch->pcdata->clan, fread_number(fpt));
	   KEY ("Clan2", ch->pcdata->clan2, fread_number(fpt));
	  
	    
	  KEY ("Class", dummy, fread_number (fpt));
	  KEY ("Created", ch->pcdata->created, fread_number(fpt));
	  if (!str_cmp (word, "Condition"))
	    {
	      ch->pcdata->condition[0] = fread_number (fpt);
	      ch->pcdata->condition[1] = fread_number (fpt);
	      ch->pcdata->condition[2] = fread_number (fpt);
	      fMatch = TRUE;
	      break;
	    }
	  if (!str_cmp (word, "Colors"))
	    {
	      ch->pcdata->colors[0] = fread_number (fpt);
	      ch->pcdata->colors[1] = fread_number (fpt);
	      ch->pcdata->colors[2] = fread_number (fpt);
	      ch->pcdata->colors[3] = fread_number (fpt);
	      ch->pcdata->colors[4] = fread_number (fpt);
	      ch->pcdata->colors[5] = fread_number (fpt);
	      ch->pcdata->colors[6] = fread_number (fpt);
	      ch->pcdata->colors[7] = fread_number (fpt);
	      ch->pcdata->colors[8] = fread_number (fpt);
	      ch->pcdata->colors[9] = fread_number (fpt);
	      fMatch = TRUE;
	      break;
	    }
	  break;
	case 'D':
	  KEY ("Damrl", ch->damroll, fread_number (fpt));
	  KEY ("Deaths", ch->pcdata->deaths, fread_number (fpt));
	  KEY ("DPray", ch->pcdata->pray_points, fread_number (fpt));
	  if (!str_cmp (word, "DCoins"))
	    {
	      int cns;
	      ch->gold = 0;
	      ch->copper = 0;
	      cns = fread_number (fpt);
	      ch->gold = cns * 100;
	      cns = fread_number (fpt);
	      ch->gold += (cns * 10);
	      ch->gold += fread_number (fpt);
	      cns = fread_number (fpt);
	      ch->copper = cns * 10;
	      ch->copper += fread_number (fpt);
	      fMatch = TRUE;
	    }
	  KEY ("Deaf", ch->pcdata->deaf, fread_number (fpt));
	  SKEY ("Descr", ch->pcdata->description, fread_string2 (fpt, NULL));
	  break;
	case 'E':
	  KEY ("Elemental", ch->pcdata->elemental_alignment, fread_number (fpt));
	  if (!str_cmp (word, "End"))
	    {
	      if (ch->pcdata->level > MAX_LEVEL)
		ch->pcdata->level = MAX_LEVEL;
	      return;
	    }
	  KEY ("EFight", ch->ced->fight_ops, fread_number (fpt));
	  KEY ("Exp", ch->exp, fread_number (fpt));
	  break;
	case 'F':
	  if (!str_cmp(word, "Faction"))
	    {
	      for (i = 0; i < MAX_MOB_TYPE; i++)
		ch->pcdata->faction[i] = fread_number(fpt);
	      fMatch = TRUE;
	    }
	  break;
	case 'G':
	  if (!str_cmp (word, "Gold"))
	    {
	      fread_to_eol (fpt);
	      fMatch = TRUE;
	    }
	  break;
	case 'H':
	  KEY ("Hitrl", ch->hitroll, fread_number (fpt));
	  if (!str_cmp (word, "HpMove"))
	    {
	      ch->hit = fread_number (fpt);
	      ch->max_hit = fread_number (fpt);
	      ch->move = fread_number (fpt);
	      ch->max_move = fread_number (fpt);
	      fMatch = TRUE;
	      break;
	    }
	  KEY ("Hardpk", ch->pcdata->hard_pkills, fread_number (fpt));
	  break;
	case 'I':
	  if (!str_cmp (word, "Ignore"))
	    {
	      for (i = 0; i < MAX_IGNORE; i++)
		{
		  if (!ch->pcdata->ignore[i])
		    break;
		}
	      ch->pcdata->ignore[i] = str_dup (fread_word (fpt));
	      fMatch = TRUE;
	    }
	case 'K':
	  KEY ("Killpt", ch->pcdata->killpoints, fread_number (fpt));
	  break;
	case 'L':
	 
	  KEY ("Level", ch->pcdata->level, fread_number (fpt));
	  KEY ("Learn", ch->pcdata->learn, fread_number (fpt));
	  SKEY ("LoDesc", ch->pcdata->long_descr, fread_string2 (fpt, NULL));
	  if (!str_cmp (word, "LCcol"))
	    {
	      int tt;
	      tt = fread_number (fpt);
	      strcpy (ch->pcdata->chan_color[tt], fread_word (fpt));
	      fMatch = TRUE;
	    }
	  KEY ("LCign", ch->pcdata->chan_ignore, fread_number (fpt));


	  if (!str_cmp (word, "LTro"))
	    {
	      for (i = 0; i < MAX_TROPHY; i++)
		{
		  if (ch->pcdata->trophy_level[i] == 0)
		    break;
		}
	      ch->pcdata->trophy_level[i] = fread_number (fpt);
	      fMatch = TRUE;
	    }
	  break;
        case 'M':
	  KEY ("ManaShield", ch->pcdata->mana_shield_strength, fread_number (fpt));
	  break;
	case 'N':
	  KEY ("NAn", ch->pcdata->show_lev, fread_number (fpt));
	  KEY ("NPR", dummy, fread_number (fpt));
	  KEY ("NHeight", ch->height, fread_number (fpt));
	  KEY ("Nospam", ch->pcdata->no_spam, fread_number (fpt));
	  KEY ("NDon", ch->pcdata->donated, fread_number (fpt));
	  KEY ("Nexp", ch->pcdata->new_exp, fread_number (fpt));
	  KEY ("NQuest", ch->pcdata->questflag, fread_number (fpt));
	  KEY ("NatArm", ch->pcdata->nat_armor, fread_number (fpt));
	  KEY ("NatAbil", ch->pcdata->nat_abilities, fread_number (fpt));
	  KEY ("NatAbilT", ch->pcdata->nat_abilities2, fread_number (fpt));
/*	  KEY ("NWasrm", ch->ced->wasroom, fread_number (fpt)); */
	  KEY ("NChal", ch->fgt->challenge, fread_number (fpt));
	  KEY ("NChon", ch->pcdata->challenge_on, fread_number (fpt));
	  KEY ("NRemort", ch->pcdata->remort_times, fread_number (fpt));
	  KEY ("NAMsg", ch->pcdata->arena_msg, fread_number (fpt));
	  KEY ("NWPS", ch->pcdata->warpoints, fread_number (fpt));
	  KEY ("NMana", ch->pcdata->n_mana, fread_number (fpt));
	  KEY ("NGldBt", ch->pcdata->guilds, fread_number (fpt));
	 
	  if (!str_cmp (word, "NTro"))
	    {
	      for (i = 0; i < MAX_TROPHY; i++)
		{
		  if (ch->pcdata->trophy_name[i][0] == '\0')
		    break;
		}
	      strcpy (ch->pcdata->trophy_name[i], fread_word (fpt));
	      fMatch = TRUE;
	    }
	  if (!str_cmp (word, "NEmail"))
	    {
	      strcpy (ch->pcdata->email, fread_string2 (fpt, NULL));
	      fMatch = TRUE;
	    }
	  if (!str_cmp (word, "Name"))
	    {
	      /*
	         * Name already set externally.
	       */
	      fread_to_eol (fpt);
	      fMatch = TRUE;
	      break;
	    }
	  KEY ("Note", ch->pcdata->last_note, fread_number (fpt));
	  break;
	case 'P':
	  SKEY ("Promptr", ch->pcdata->rprompt, fread_string2 (fpt, NULL));
	  SKEY ("Password", ch->pcdata->pwd, fread_string2 (fpt, NULL));
         
	  KEY ("Played", ch->pcdata->played, fread_number (fpt));
	  KEY ("Pagelen", ch->pcdata->pagelen, fread_number (fpt));
	  KEY ("PImplO", ch->pcdata->augments, fread_number (fpt));
	  KEY ("Positn", ch->position, fread_number (fpt));
	  KEY ("Practc", ch->pcdata->practice, fread_number (fpt));
	  KEY ("Pklevs", ch->pcdata->pklevels, fread_number (fpt));
	  KEY ("Pkhelp", ch->pcdata->pkhelpers, fread_number (fpt));
	  KEY ("Pkills", ch->pcdata->pkills, fread_number (fpt));
	  KEY ("Pkilled", ch->pcdata->pkilled, fread_number (fpt));
	  KEY ("Pkers", ch->pcdata->pkers, fread_number (fpt));
	  break;
	  if (ch->position == POSITION_FIGHTING)
	    NEW_POSITION(ch, POSITION_STANDING);
	  break;
	case 'R':
	  if (!str_cmp (word, "Race"))
	    {
	      ch->pcdata->race = fread_number (fpt);
	      fMatch = TRUE;
	    }
	  SKEY ("RealPassword", ch->pcdata->rpwd, fread_string2 (fpt,NULL));
	  if (!str_cmp (word, "Room"))
	    {
	      ch->in_room = NULL;
	      ch->pcdata->temp_room = get_room_index (fread_number (fpt));
	      fMatch = TRUE;
	      break;
	    }
	  break;
	case 'S':
	  KEY ("SvngTh", ch->pcdata->saving_throw, fread_number (fpt));
	  KEY ("Sex", ch->pcdata->sex, fread_number (fpt));
	  SKEY ("ShDesc", ch->pcdata->short_descr, fread_string2 (fpt, NULL));
	  KEY ("SCool", ch->pcdata->cool_skills, fread_number (fpt));
	  KEY ("Secrty", ch->pcdata->security, fread_number (fpt));
	  KEY ("Stance", ch->pcdata->stance, fread_number (fpt));
	  if (!str_cmp (word, "Skill"))
	    {
	      int sn;
	      int value;
	      value = fread_number (fpt);
	      sn = lookup_gsn (fread_word (fpt));
	      if (sn > 0)
		ch->pcdata->learned[sn] = value;
	      fMatch = TRUE;
	    }
	  break;
	case 'T':
	  if (!str_cmp (word, "TTro"))
	    {
	      for (i = 0; i < MAX_TROPHY; i++)
		{
		  if (ch->pcdata->trophy_times[i] == 0)
		    break;
		}
	      ch->pcdata->trophy_times[i] = fread_number (fpt);
	      fMatch = TRUE;
	    }
	  KEY ("TtlKils", ch->pcdata->totalkills, fread_number (fpt));
	  if (!str_cmp (word, "Title"))
	    {
	      ch->pcdata->title = str_dup (fread_string2 (fpt, NULL));
	      fMatch = TRUE;
	      break;
	    }
	  KEY ("TotWps", ch->pcdata->total_wps, fread_number(fpt));
	  break;
	case 'V':
	  if (!str_cmp (word, "Vnum"))
	    {
	      ch->pIndexData = get_mob_index (fread_number (fpt));
	      fMatch = TRUE;
	      break;
	    }
	  break;
	case 'W':
	  KEY ("Winv", ch->pcdata->wizinvis, fread_number (fpt));
	  KEY ("Wimpy", ch->ced->wimpy, fread_number (fpt));
	  KEY ("Warmth", ch->pcdata->warmth, fread_number (fpt));
	  break;
	}
      if (!fMatch)
	{
	  sprintf(bufh, "Character Data Load Complete. Loading Player.", NAME(ch), word);
	  bug (bufh, 0);
	  if (!feof (fpt))
	    fread_to_eol (fpt);
	}
    }
  if (ch->pcdata->temp_room == NULL)
    ch->pcdata->temp_room = get_room_index(ch->pcdata->alignment +100);

  fclose (fpt);
  return;
}




void 
fread_char (CHAR_DATA * ch, FILE * fp)
{
  char *word;
  bool fMatch;
  long nRat;
  int i;
  int dummy;
  int lnn;
 
  char bufh[100]; 
  int csf = 0;
  nRat = 0;
  lnn = 0;
  check_ced (ch);
  check_fgt (ch);
  for (;;)
    {
      lnn++;
      word = feof (fp) ? "End" : fread_word (fp);
      fMatch = FALSE;
      switch (UPPER (word[0]))
	{
	case '*':
	  fMatch = TRUE;
	  fread_to_eol (fp);
	  break;
	case 'A':
	  if (!str_cmp (word, "APet"))
	    {
	      int kl;
	      for (kl = 0; kl < 20; kl++)
		{
		  if (ch->pcdata->pet_temp[kl] == 0)
		    {
		      ch->pcdata->pet_temp[kl] = fread_number (fp);
		      ch->pcdata->pet_hps[kl] = fread_number (fp);
		      ch->pcdata->pet_move[kl] = fread_number (fp);
		      break;
		    }
		}
	      fMatch = TRUE;
	    }
	  if (!str_cmp (word, "Act") )
	    {
	      ch->act = fread_number(fp);
	      fMatch = TRUE;
	      if (IS_SET(ch->act, ACT_CRASHER))
		{
		  if (ch->desc)
		    SET_BIT(ch->desc->act, DESC_CRASHER);
		}
	    }
	  KEY ("Act2", ch->pcdata->act2, fread_number (fp));
	  KEY ("Act3", ch->pcdata->act3, fread_number (fp));
	  KEY ("AffBy", ch->affected_by, fread_number (fp));
	  KEY ("AffTWO", ch->more_affected_by, fread_number (fp));
	  if (!str_cmp(word, "Alignmt"))
	    {
	      ch->pcdata->alignment = fread_number(fp);
	      if (ch->pcdata->alignment < 0) ch->pcdata->alignment = 1;
	      if (ch->pcdata->alignment >= NUM_ALIGN) ch->pcdata->alignment = 2;
              fMatch = TRUE;
	    }
	  KEY ("Armor", ch->armor, fread_number (fp));
	  if (!str_cmp (word, "Alias"))
	    {
	      int k;
	      char *msk;
	      for (k = 0; k < MAXALIAS; k++)
		{
		  if (!ch->pcdata->aliasname[k] ||
		      ch->pcdata->aliasname[k][0] == '\0' ||
		      !ch->pcdata->alias[k] ||
		      ch->pcdata->alias[k][0] == '\0')
		    {
		      break;
		    }
		}
	      ch->pcdata->aliasname[k] = str_dup (fread_word (fp));
	      fread_word (fp);
	      ch->pcdata->alias[k] = str_dup (fread_string2 (fp, NULL));
	      for (msk = ch->pcdata->alias[k]; *msk != '\0'; msk++)
		if (*msk == '*')
		  *msk = '~';
	      fMatch = TRUE;
	    }
	  if (!str_cmp (word, "AffectData"))
	    {
	      AFFECT_DATA *paf;
	      if (affect_free == NULL)
		{
		  paf = mem_alloc (sizeof (*paf));
		}
	      else
		{
		  paf = affect_free;
		  affect_free = affect_free->next;
		}
	      if (!str_cmp (word, "Affect"))
		{
		  paf->type = fread_number (fp);
		}
	      else
		{
		  int sn;
		  sn = lookup_gsn (fread_word (fp));
		  if (sn < 0)
		    bug ("Fread_char: unknown skill.", 0);
		  else
		    paf->type = sn;
		}
	      paf->duration = fread_number (fp);
	      paf->modifier = fread_number (fp);
	      paf->location = fread_number (fp);
	      paf->bitvector = fread_number (fp);
              paf->bitvector2 = fread_number (fp);
	      paf->next = ch->affected;
	      ch->affected = paf;
	      fMatch = TRUE;
              /* ------------------------------ */
              /*         Restore affect         */
	      /* ------------------------------ */
	      if(paf->type == gsn_manashield)
                renew_affect(ch,paf);
	      else if (paf->type == gsn_fireshield)
                renew_affect(ch,paf);

	      break;
	    }
	  if (!str_cmp (word, "AttrMod"))
	    {
	      int i;
	      for (i = 0; i < NUM_STATS; i++)
		ch->pcdata->stat_mod[i] = fread_number(fp);
	      fMatch = TRUE;
	      break;
	    }
	  if (!str_cmp (word, "AttrPerm"))
	    {
	      int i;
	      for (i = 0; i < NUM_STATS; i++)
		ch->pcdata->stat[i] = fread_number(fp);
	      fMatch = TRUE;
	      break;
	    }
	  break;
	case 'B':
	  if (!str_cmp (word, "Bank"))
	    {
	      ch->pcdata->bank = fread_number (fp);
	      fMatch = TRUE;
	    }
	  SKEY ("Bamfin", ch->pcdata->beamin, fread_string2 (fp, NULL));
	  SKEY ("Bamfout", ch->pcdata->beamout, fread_string2 (fp, NULL));
	
	  break;
	case 'C':
	  KEY ("Class", dummy, fread_number (fp));
	  KEY ("Created", ch->pcdata->created, fread_number(fp));
	  KEY ("Clan", ch->pcdata->clan, fread_number(fp));
	   KEY ("Clan2", ch->pcdata->clan2, fread_number(fp));
	  
	  if (!str_cmp (word, "Condition"))
	    {
	      ch->pcdata->condition[0] = fread_number (fp);
	      ch->pcdata->condition[1] = fread_number (fp);
	      ch->pcdata->condition[2] = fread_number (fp);
	      fMatch = TRUE;
	      break;
	    }
	  if (!str_cmp (word, "Colors"))
	    {
	      ch->pcdata->colors[0] = fread_number (fp);
	      ch->pcdata->colors[1] = fread_number (fp);
	      ch->pcdata->colors[2] = fread_number (fp);
	      ch->pcdata->colors[3] = fread_number (fp);
	      ch->pcdata->colors[4] = fread_number (fp);
	      ch->pcdata->colors[5] = fread_number (fp);
	      ch->pcdata->colors[6] = fread_number (fp);
	      ch->pcdata->colors[7] = fread_number (fp);
	      ch->pcdata->colors[8] = fread_number (fp);
	      ch->pcdata->colors[9] = fread_number (fp);
	      fMatch = TRUE;
	      break;
	    }
	  break;
	case 'D':
	  KEY ("Damrl", ch->damroll, fread_number (fp));
	  KEY ("Deaths", ch->pcdata->deaths, fread_number (fp));
	  KEY ("DPray", ch->pcdata->pray_points, fread_number (fp));
	  if (!str_cmp (word, "DCoins"))
	    {
	      int cns;
	      ch->gold = 0;
	      ch->copper = 0;
	      cns = fread_number (fp);
	      ch->gold = cns * 100;
	      cns = fread_number (fp);
	      ch->gold += (cns * 10);
	      ch->gold += fread_number (fp);
	      cns = fread_number (fp);
	      ch->copper = cns * 10;
	      ch->copper += fread_number (fp);
	      fMatch = TRUE;
	    }
	  KEY ("Deaf", ch->pcdata->deaf, fread_number (fp));
	  SKEY ("Descr", ch->pcdata->description, fread_string2 (fp, NULL));
	  break;
	case 'E':
	  KEY ("Elemental", ch->pcdata->elemental_alignment, fread_number (fp));

	  if (!str_cmp (word, "End"))
	      return;
	  KEY ("EFight", ch->ced->fight_ops, fread_number (fp));
	  KEY ("Exp", ch->exp, fread_number (fp));
	  break;
	 case 'F':
	   if (!str_cmp(word, "Faction"))
	     {
	       for (i = 0; i < MAX_MOB_TYPE; i++)
		 ch->pcdata->faction[i] = fread_number(fp);
	       fMatch = TRUE;
	     }
	   break; 
	case 'G':
	  if (!str_cmp (word, "Gold"))
	    {
	      fread_to_eol (fp);
	      fMatch = TRUE;
	    }
	  break;
	case 'H':
	  KEY ("Hitrl", ch->hitroll, fread_number (fp));
	  if (!str_cmp (word, "HpMove"))
	    {
	      ch->hit = fread_number (fp);
	      ch->max_hit = fread_number (fp);
	      ch->move = fread_number (fp);
	      ch->max_move = fread_number (fp);
	      fMatch = TRUE;
	      break;
	    }
	  KEY ("Hardpk", ch->pcdata->hard_pkills, fread_number (fp));
	  break;
	case 'I':
	  if (!str_cmp (word, "Ignore"))
	    {
	      for (i = 0; i < MAX_IGNORE; i++)
		{
		  if (!ch->pcdata->ignore[i])
		    break;
		}
	      ch->pcdata->ignore[i] = str_dup (fread_word (fp));
	      fMatch = TRUE;
	    }
	case 'K':
	  KEY ("Killpt", ch->pcdata->killpoints, fread_number (fp));
	  break;
	case 'L':
	 
	  KEY ("Level", ch->pcdata->level, fread_number (fp));
	  KEY ("Learn", ch->pcdata->learn, fread_number (fp));
	  SKEY ("LoDesc", ch->pcdata->long_descr, fread_string2 (fp, NULL));
	  if (!str_cmp (word, "LCcol"))
	    {
	      int tt;
	      tt = fread_number (fp);
	      strcpy (ch->pcdata->chan_color[tt], fread_word (fp));
	      fMatch = TRUE;
	    }
	  KEY ("LCign", ch->pcdata->chan_ignore, fread_number (fp));



	  if (!str_cmp (word, "LTro"))
	    {
	      for (i = 0; i < MAX_TROPHY; i++)
		{
		  if (ch->pcdata->trophy_level[i] == 0)
		    break;
		}
	      ch->pcdata->trophy_level[i] = fread_number (fp);
	      fMatch = TRUE;
	    }
	  break;
        case 'M':
	  KEY ("ManaShield", ch->pcdata->mana_shield_strength, fread_number (fp));
	  break;
	case 'N':
	
	  KEY ("NAn", ch->pcdata->show_lev, fread_number (fp));
	  KEY ("NPR", dummy, fread_number (fp));
	  KEY ("Nospam", ch->pcdata->no_spam, fread_number (fp));
	  KEY ("NHeight", ch->height, fread_number (fp));
	  KEY ("NDon", ch->pcdata->donated, fread_number (fp));
	  KEY ("Nexp", ch->pcdata->new_exp, fread_number (fp));
	  KEY ("NQuest", ch->pcdata->questflag, fread_number (fp));
	  KEY ("NRat", nRat, fread_number (fp));
	  KEY ("NatArm", ch->pcdata->nat_armor, fread_number (fp));
	  KEY ("NatAbil", ch->pcdata->nat_abilities, fread_number (fp));
	  KEY ("NatAbilT", ch->pcdata->nat_abilities2, fread_number (fp));
/*	  KEY ("NWasrm", ch->ced->wasroom, fread_number (fp)); */
	  KEY ("NChal", ch->fgt->challenge, fread_number (fp));
	  KEY ("NChon", ch->pcdata->challenge_on, fread_number (fp));
	  KEY ("NRemort", ch->pcdata->remort_times, fread_number (fp));
	  KEY ("NAMsg", ch->pcdata->arena_msg, fread_number (fp));
	  KEY ("NWPS", ch->pcdata->warpoints, fread_number (fp));
	  KEY ("NMana", ch->pcdata->n_mana, fread_number (fp));
	  KEY ("NGldBt", ch->pcdata->guilds, fread_number (fp));
	
	  if (!str_cmp (word, "NTro"))
	    {
	      for (i = 0; i < MAX_TROPHY; i++)
		{
		  if (ch->pcdata->trophy_name[i][0] == '\0')
		    break;
		}
	      strcpy (ch->pcdata->trophy_name[i], fread_word (fp));
	      fMatch = TRUE;
	    }
	  if (!str_cmp (word, "NEmail"))
	    {
	      strcpy (ch->pcdata->email, fread_string2 (fp, NULL));
	      fMatch = TRUE;
	    }
	  if (!str_cmp (word, "Name"))
	    {
	      /*
	         * Name already set externally.
	       */
	      fread_to_eol (fp);
	      fMatch = TRUE;
	      break;
	    }
	  KEY ("Note", ch->pcdata->last_note, fread_number (fp));
	  break;
	case 'P':
	  SKEY ("Promptr", ch->pcdata->rprompt, fread_string2 (fp, NULL));
	  SKEY ("Password", ch->pcdata->pwd, fread_string2 (fp, NULL));
	  KEY ("Played", ch->pcdata->played, fread_number (fp));
	  KEY ("Pagelen", ch->pcdata->pagelen, fread_number (fp));
	  KEY ("PImplO", ch->pcdata->augments, fread_number (fp));
	  KEY ("Positn", ch->position, fread_number (fp));
	  KEY ("Practc", ch->pcdata->practice, fread_number (fp));
	  KEY ("Pklevs", ch->pcdata->pklevels, fread_number (fp));
	  KEY ("Pkhelp", ch->pcdata->pkhelpers, fread_number (fp));
	  KEY ("Pkills", ch->pcdata->pkills, fread_number (fp));
	  KEY ("Pkilled", ch->pcdata->pkilled, fread_number (fp));
	  KEY ("Pkers", ch->pcdata->pkers, fread_number (fp));
	  break;
	case 'R':
	  if (!str_cmp (word, "Race"))
	    {
	      ch->pcdata->race = fread_number (fp);
	      fMatch = TRUE;
	    }
	  SKEY ("RealPassword", ch->pcdata->rpwd, fread_string2 (fp,NULL));
	  if (!str_cmp (word, "Room"))
	    {
	      ch->pcdata->temp_room = get_room_index (fread_number (fp));
	      fMatch = TRUE;
	      break;
	    }
	  break;
	case 'S':
	  KEY ("SvngTh", ch->pcdata->saving_throw, fread_number (fp));
	  KEY ("Sex", ch->pcdata->sex, fread_number (fp));
	  SKEY ("ShDesc", ch->pcdata->short_descr, fread_string2 (fp, NULL));
	  KEY ("SCool", ch->pcdata->cool_skills, fread_number (fp));
	  KEY ("Secrty", ch->pcdata->security, fread_number (fp));
	  KEY ("Stance", ch->pcdata->stance, fread_number (fp));
	  if (!str_cmp (word, "Sf"))
	    {
	      ch->pcdata->script_flags[csf] = str_dup (fread_string2 (fp, NULL));
	      csf++;
	      fMatch = TRUE;
	    }

	  if (!str_cmp (word, "Skill"))
	    {
	      int sn;
	      int value;
	      value = fread_number (fp);
	      sn = lookup_gsn (fread_word (fp));
	      if (sn >= 0)
		ch->pcdata->learned[sn] = value;
	      fMatch = TRUE;
	    }
	  if (!str_cmp (word, "StartAttr"))
	    {
	      int i;
	      for (i = 0; i < NUM_STATS; i++)
		ch->pcdata->start_stat[i] = fread_number(fp);
	      fMatch = TRUE;
	      break;
	    }
	  break;
	case 'T':
	  if (!str_cmp (word, "TTro"))
	    {
	      for (i = 0; i < MAX_TROPHY; i++)
		{
		  if (ch->pcdata->trophy_times[i] == 0)
		    break;
		}
	      ch->pcdata->trophy_times[i] = fread_number (fp);
	      fMatch = TRUE;
	    }
	  KEY ("TtlKils", ch->pcdata->totalkills, fread_number (fp));
	  if (!str_cmp (word, "Title"))
	    {
	      ch->pcdata->title = str_dup (fread_string2 (fp, NULL));
	      fMatch = TRUE;
	      break;
	    }
	  KEY ("TotWps", ch->pcdata->total_wps, fread_number (fp));
	  break;
	case 'V':
	  if (!str_cmp (word, "Vnum"))
	    {
	      ch->pIndexData = get_mob_index (fread_number (fp));
	      fMatch = TRUE;
	      break;
	    }
	  break;
	case 'W':
	  KEY ("Winv", ch->pcdata->wizinvis, fread_number (fp));
	  KEY ("Wimpy", ch->ced->wimpy, fread_number (fp));
	  KEY ("Warmth", ch->pcdata->warmth, fread_number (fp));  
	  break;
	}
      if (!fMatch)
	{
	  sprintf(bufh, "Character Load Successful! Loading characters current state.", NAME(ch), word);
	  bug(bufh, 0);
	  if (!feof (fp))
	    fread_to_eol (fp);
	}
    }
  return;
}

int 
free_clan_storage (CLAN_DATA * clan)
{
  int k;
  for (k = 0; k < MAX_CLAN_STORAGE; k++)
    {
      if (clan->storage[k] == NULL)
	return k;
    }
  return -1;
}

int 
free_storage (CHAR_DATA * ch)
{
  int k;
  if (IS_MOB (ch))
    {
      fprintf (stderr, "FREE STORAGE ERROR CALLED BY NPC!\n");
      exit (1);
    }
  for (k = 0; k < MAXST; k++)
    {
      if (ch->pcdata->storage[k] == NULL)
	return k;
    }
  return -1;
}


void 
fread_obj (CHAR_DATA * ch, FILE * fp, int storage)
{
  SINGLE_OBJECT *obj;
  char *word;
  int iNest;
  bool fMatch;
  bool fNest;
  bool fVnum;
  obj = new_obj ();

  obj->short_descr = NULL;
  obj->description = NULL;
  fNest = FALSE;
  fVnum = TRUE;
  iNest = 0;
  for (;;)
    {
      word = feof (fp) ? "End" : fread_word (fp);
      fMatch = FALSE;
      switch (UPPER (word[0]))
	{
	case '*':
	  fMatch = TRUE;
	  fread_to_eol (fp);
	  break;
	case 'A':
	  if (!str_cmp (word, "AOVN"))
	    {
	      int vnum;
	      int value[10];
	      OBJ_PROTOTYPE *pObjIndex;
	      vnum = fread_number (fp);
	      if ((obj->pIndexData = get_obj_index (vnum)) == NULL)
		obj->pIndexData = get_obj_index (37);
	      fVnum = TRUE;
	      pObjIndex = obj->pIndexData;
	      obj->extra_flags = pObjIndex->extra_flags;
	      obj->timer = pObjIndex->timer;
	      if (pObjIndex->cost != 0)
		obj->cost = pObjIndex->cost;
	      if (pObjIndex->cost != 0 && obj->cost == 0)
		obj->cost = 1;
	      value[0] = fread_number (fp);
	      value[1] = fread_number (fp);
	      value[2] = fread_number (fp);
	      value[3] = fread_number (fp);
	      value[4] = fread_number (fp);
	      value[5] = fread_number (fp);
	      value[6] = fread_number (fp);
	      value[7] = fread_number (fp);
	      value[8] = fread_number (fp);
	      value[9] = fread_number (fp);
	      obj->wear_loc = fread_number (fp);
	      obj->size = fread_number (fp);
	      obj->timer = fread_number (fp);
	      allocate_correct_datatype (obj, value);
	      fMatch = TRUE;
	      if (obj->wear_loc == WEAR_HOLD_1)
		ch->hold1 = obj;
	      if (obj->wear_loc == WEAR_HOLD_2)
		ch->hold2 = obj;
	      if (ch != NULL && IS_PLAYER(ch))
		{
		  if (obj->wear_loc == WEAR_FEET)
		    SET_BIT(ch->act, PLR_HAS_SHOES);
		  if (obj->wear_loc == WEAR_SHIELD)
		    SET_BIT(ch->act, PLR_HAS_SHIELD);
		  if (obj->pIndexData->item_type == ITEM_VEHICLE)
		    {
		      I_VEHICLE *veh = (I_VEHICLE *) obj->more;
		      if (veh->sector_use == SECT_SNOW && obj->wear_loc > WEAR_NONE)
			SET_BIT(ch->act, PLR_HAS_SNOWSHOES);
		      if (veh->sector_use == SECT_UNDERWATER && obj->wear_loc > WEAR_NONE)
			SET_BIT(ch->act, PLR_HAS_SCUBA);
		      if (veh->sector_use == SECT_MOUNTAIN && obj->wear_loc > WEAR_NONE) 
			SET_BIT(ch->act, PLR_HAS_MTN_BOOTS);
		    }  
		  if (obj->pIndexData->item_type == ITEM_LIGHT && IS_LIT(obj) && obj->wear_loc > WEAR_NONE)
		    ch->pcdata->light++;
		  if(IS_OBJ_STAT(obj, ITEM_COMMANDSCRIPT))
		    ch->pcdata->command_objs++;
		}
	    }
	  
	  if (!str_cmp( word, "AffectData" ))
	    {
	      AFFECT_DATA *paf;
	      if ( affect_free == NULL )
		{
		  paf = mem_alloc( sizeof(*paf) );
		}
	      else
		{
		  paf = affect_free;
		  affect_free = affect_free->next;
		}
	      paf->duration = fread_number( fp );
	      paf->modifier = fread_number( fp );
	      paf->location = fread_number( fp );
	      paf->bitvector = fread_number( fp );
	      paf->bitvector2 = fread_number(fp);
	      paf->next = obj->affected;
	      obj->affected = paf;
	      fMatch = TRUE;
	      break;
	    }
	  break;
	case 'C':
	  KEY ("Cost", obj->cost, fread_number (fp));
	  break;
	case 'D':
	  SKEY ("Description", obj->description, fread_string2 (fp, ch));
	  break;
	case 'E':
	  KEY ("ExtraFlags", obj->extra_flags, fread_number (fp));
	  if (!str_cmp (word, "End"))
	    {
	      if (!fNest || !fVnum)
		{
		  bug ("Fread_obj: incomplete object.", 0);
		  if (obj->description)
		    free_string (obj->description);
		  if (obj->short_descr)
		    free_string (obj->short_descr);
		  obj->next = obj_free;
		  obj_free = obj;
		  return;
		}
	      else
		{
		  obj->next = object_list;
		  object_list = obj;
		  obj->pIndexData->count++;
		  if (storage > 1)
		    {
		      CLAN_DATA *clan;
		      if ((clan = get_clan_index (storage - 1)) == NULL)
			{
			  fprintf (stderr, "Bummer. Bad clan attached to storage file.\n");
			  return;
			}
		      clan->storage[free_clan_storage (clan)] = obj;
		    }
		  else if (storage == 1)
		    {
		      ch->pcdata->storage[free_storage (ch)] = obj;
		    }
		  else
		    {
		      got = FALSE;
		      if (iNest == 0 || rgObjNest[iNest] == NULL)
			obj_to (obj, ch);
		      else
			obj_to (obj, rgObjNest[iNest - 1]);
		      got = TRUE;
		    }
		  return;
		}
	    }
	  break;
	case 'N':
	  if (!str_cmp (word, "Nest"))
	    {
	      iNest = fread_number (fp);
	      if (iNest < 0 || iNest >= MAX_NESTING)
		{
		  bug ("Fread_obj: bad nest %d.", iNest);
		}
	      else
		{
		  rgObjNest[iNest] = obj;
		  fNest = TRUE;
		}
	      fMatch = TRUE;
	    }
	  break;
	case 'S':
	  if (!str_cmp (word, "ShortDescr"))
	    {
	      obj->short_descr = NULL;
	      fMatch = TRUE;
	    }
/*          if ( !str_cmp( word, "Spell" ) )
   {
   int iValue;
   int sn;
   iValue = fread_number( fp );
   sn = lookup_gsn( fread_word( fp ) );
   if ( iValue < 0 || iValue > 3 )
   {
   bug( "Fread_obj: bad iValue %d.", iValue );
   }
   else if ( sn < 0 )
   {
   bug( "Fread_obj: unknown skill.", 0 );
   }
   else
   {
   obj->value[iValue] = sn;
   }
   fMatch = TRUE;
   break;
   } */
	  break;
	case 'T':
	  KEY ("Timer", obj->timer, fread_number (fp));
	  break;
	case 'V':
	  if (!str_cmp (word, "Vnum"))
	    {
	      int vnum;
	      vnum = fread_number (fp);
	      if ((obj->pIndexData = get_obj_index (vnum)) == NULL)
		obj->pIndexData = get_obj_index (1002);
	      fVnum = TRUE;
	      fMatch = TRUE;
	      break;
	    }
	  break;
	case 'W':
	  KEY ("WearLoc", obj->wear_loc, fread_number (fp));
	  break;
	}
      if (!fMatch)
	{
	  bug ("Fread_obj: no match.", 0);
	  fread_to_eol (fp);
	}
    }
}
