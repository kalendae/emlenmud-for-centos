#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emlen.h"
#ifndef WINDOWS
#include <sys/time.h>
#endif

/* Fetch a new character from the free list or memory, clear it, then return it */
CHAR_DATA *
new_char (void)
{
  CHAR_DATA *ch;
  if (char_free == NULL)
    {
      ch = mem_alloc (sizeof (*ch));
    }
  else
    {
      ch = char_free;
      char_free = char_free->next;
      remove_from_fighting_list (ch);

    }
  bzero (ch, sizeof (*ch));
  ch->data_type = K_CHAR;
  clear_data (ch);
  return ch;
}

REALCHAR_DATA *
new_pc (void)
{
  REALCHAR_DATA *pcdata;
  if (pcdata_free == NULL)
    {
      pcdata = mem_alloc (sizeof (*pcdata));
    }
  else
    {
      pcdata = pcdata_free;
      pcdata_free = pcdata_free->next;
    }
  bzero (pcdata, sizeof (*pcdata));
  pcdata->data_type = K_PC;
  clear_data (pcdata);
  return pcdata;
}

void
clear_data (void *thing)
{
  CHAR_DATA *ch;
  REALCHAR_DATA *pcdata;
  SINGLE_OBJECT *obj;
  char *t = thing;
  if (thing == NULL)
    return;
  if (*t == K_CHAR || *t == 50)
    {
      ch = (CHAR_DATA *) thing;
      ch->desc = NULL;
      remove_from_fighting_list (ch);

      ch->ced = NULL;
      ch->in_room = NULL;
      ch->gen_next = NULL;
      ch->data_type = K_CHAR;
      ch->armor = 100;
      ch->position = POSITION_STANDING;
      ch->next_fighting = NULL;
      ch->exp = 0;
      ch->height = -1;
      ch->special = 0;
      ch->pIndexData = NULL;
      ch->affected = NULL;
      ch->timer = -1;
      ch->wait = 0;
      ch->affected_by = 0;
      ch->more_affected_by = 0;
      ch->act = 0;
      ch->copper = 0;
      ch->gold = 0;
      ch->position = POSITION_STANDING;
      ch->carrying = NULL;
      ch->hold1 = NULL;
      ch->hold2 = NULL;
      ch->hitroll = 0;
      ch->damroll = 0;
      ch->hit = 20;
      ch->max_hit = 20;
      ch->move = 70;
      ch->max_move = 70;
    }
  else if (*t == K_PC)
    {
      int k;
      CHANNEL *c;
      pcdata = (REALCHAR_DATA *) thing;
      pcdata->cursor = 0;
      for (k = 0; k < 20; k++)
        pcdata->pet_temp[k] = 0;
      for (k = 0; k < 20; k++)
        pcdata->pet_move[k] = 0;
      for (k = 0; k < 20; k++)
        pcdata->pet_hps[k] = 0;
      pcdata->show_lev = TRUE;
      free_string (pcdata->name);
      pcdata->name = NULL;
      if (pcdata->oldgrep != NULL)
        {
          free_string (pcdata->oldgrep);
          pcdata->oldgrep = NULL;
        }
      free_string (pcdata->short_descr);
      pcdata->short_descr = &str_empty[0];
      free_string (pcdata->long_descr);
      pcdata->long_descr = &str_empty[0];
      free_string (pcdata->description);
      pcdata->description = &str_empty[0];
      pcdata->chan_ignore = 0;
      c = chan_first;
      for (k = 0; k < 30; k++)
        {
          strcpy (pcdata->chan_color[k], c->color);
          if (c->next != NULL)
            c = c->next;
        }
      pcdata->transport_quest = 0;
      pcdata->cool_skills = 0;
      pcdata->security = 9;
      pcdata->learn = 1;
      pcdata->created = current_time;
      pcdata->pray_points = 0;
      pcdata->practice = 1;
      pcdata->logon = current_time;
      pcdata->played = 0;
      pcdata->wizinvis = 0;
      pcdata->colors[COLOR_CHAT] = 14;
      pcdata->colors[COLOR_YELL] = 12;
      pcdata->colors[COLOR_CLANTALK] = 10;
      pcdata->colors[COLOR_IMMTALK] = 11;
      pcdata->colors[COLOR_GODTALK] = 12;
      pcdata->colors[COLOR_SAY] = 15;
      pcdata->colors[COLOR_TELL] = 10;
      pcdata->rprompt = NULL;
      pcdata->pwd = &str_empty[0];
      pcdata->rpwd = &str_empty[0];
      pcdata->beamin = &str_empty[0];
      pcdata->beamout = &str_empty[0];
      pcdata->title = &str_empty[0];
      for (k = 0; k < NUM_STATS; k++)
	{
	  pcdata->stat[k] = 12;
	  pcdata->stat_mod[k] = 0;
	}
      pcdata->temp = NULL;
      pcdata->totalkills = 0;
      pcdata->killpoints = 0;
      pcdata->deaths = 0;
      pcdata->resist_summon = 0;
      pcdata->plus_hide = 0;
      pcdata->plus_sneak = 0;
      pcdata->plus_kick = 0;
      pcdata->armor[0] = 0;
      pcdata->armor[1] = 0;
      pcdata->armor[2] = 0;
      pcdata->armor[3] = 0;
      pcdata->clan = -1;
      pcdata->clan2 = -1;
      pcdata->n_mana = 0;
      pcdata->x = 5;
      pcdata->y = 10;
      pcdata->n_max_mana = 0;
      pcdata->warpoints = 0;
      pcdata->total_wps = 0;
      pcdata->pklevels = 0;
      pcdata->pkhelpers = 0;
      pcdata->hard_pkills = 0;
      pcdata->warmth = 0;
      pcdata->pkills = 0;
      pcdata->pkilled = 0;
      pcdata->pkers = 0;
      pcdata->challenge_on = TRUE;
      pcdata->remort_times = 0;
      pcdata->arena_msg = TRUE;
      pcdata->challenged = FALSE;
      pcdata->challenge_time = 0;
      pcdata->challenged_by = NULL;
      pcdata->tickcounts = 0;
      pcdata->auction_count = 0;
      pcdata->email[0] = '\0';
      pcdata->new_exp = FALSE;
      pcdata->no_spam = 0;
      pcdata->bgf = FALSE;
      pcdata->questflag = 0;
      pcdata->nat_armor = 100;
      pcdata->nat_abilities = 0;
      pcdata->guilds = 0;
      pcdata->no_quit_pk = 0; /* JRAJRA */
      pcdata->number_pets = 0;
      pcdata->pagelen = 24;
      pcdata->condition[COND_THIRST] = 48;
      pcdata->condition[COND_FULL] = 48;
      pcdata->condition[COND_DRUNK] = 0;
      pcdata->bank = 0;
      pcdata->quiet = 0;
      pcdata->online_spot = NULL;
      pcdata->light = 0;
      pcdata->command_objs = 0;
      pcdata->mana_shield_strength = 0;
      pcdata->elemental_alignment = 0;
      pcdata->tmp_con = 0;
      pcdata->tmp_dex = 0;
      pcdata->tmp_str = 0;
      pcdata->tmp_int = 0;
      pcdata->tmp_wis = 0;
      pcdata->perm_con = 0;
      pcdata->perm_dex = 0;
      pcdata->perm_str = 0;
      pcdata->perm_int = 0;
      pcdata->perm_wis = 0;
      for (k = 0; k < 4; k++)
	pcdata->guarded_by[k] = NULL;
      pcdata->guarding = NULL;
      for (k = 0; k < MAXST; k++)
        {
          pcdata->storage[k] = NULL;
        }
      for (k = 0; k < MAX_IGNORE; k++)
        {
          pcdata->ignore[k] = NULL;
        }
      for (k = 0; k < MAX_SCRIPT_FLAGS; k++)
        if (pcdata->script_flags[k])
          {
            free_string (pcdata->script_flags[k]);
            pcdata->script_flags[k] = NULL;
          }
      for (k = 0; k < MAX_TROPHY; k++)
        {
          pcdata->trophy_name[k][0] = '\0';
          pcdata->trophy_times[k] = 0;
          pcdata->trophy_level[k] = 0;
        }
      for (k = 0; k < MAXALIAS; k++)
        {
          pcdata->alias[k] = NULL;
          pcdata->aliasname[k] = NULL;
        }
      for (k = 0; k < SKILL_COUNT; k++)
        pcdata->learned[k] = -100;
      pcdata->stance = 0;
    }
  else if (*t == K_OBJ)
    {
      obj = (SINGLE_OBJECT *) thing;
      obj->in_room = NULL;
      obj->wear_loc = WEAR_NONE;
      obj->boat = NULL;
      obj->contains = NULL;
      obj->in_obj = NULL;
      obj->more = NULL;
      obj->in_room = NULL;
      obj->carried_by = NULL;
      obj->size = 0;
      obj->boat = NULL;
      obj->short_descr = NULL;
      obj->description = NULL;
      obj->timer = -1;
      obj->cost = 0;
    }
  return;
}

void
free_it (void *thing)
{
  CHAR_DATA *this;
  SINGLE_OBJECT *prev;
  SINGLE_OBJECT *obj;
  REALCHAR_DATA *pcdata;
  char *t = thing;
  if (thing == NULL)
    return;
  if (*t == 50)
    return;
  if (*t == K_CHAR)
    {
      CHAR_DATA *dbl;
      SINGLE_OBJECT *eobj;
      SINGLE_OBJECT *obj_next;
      AFFECT_DATA *paf;
      AFFECT_DATA *paf_next;
      DESCRIPTOR_DATA *d;
      this = (CHAR_DATA *) thing;
      MARK_CHAR_DEBUG ("free_it", this);
      if (IS_MOB (this))
        remove_from_aggro_list (this);
      remove_from_fighting_list (this);
     if (this->desc)
        {
          this->desc->character = NULL;
        }
      this->desc = NULL;
      if (this == char_list)
        char_list = this->next;
      else
        {
          for (dbl = char_list; dbl != NULL; dbl = dbl->next)
            {
              if (dbl->next == this)
                {
                  dbl->next = this->next;
                  break;
                }
            }
        }
      for (d = descriptor_list; d != NULL; d = d->next)
        {
          if (d->character && d->connected == CON_PEDITOR &&
              this == (CHAR_DATA *) d->pEdit)
            return;
        }
      if (IS_MOB (this))
        remove_from_aggro_list (this);
      remove_from_fighting_list (this);
      if (this->ced)
        free_ced (this);
      this->ced = NULL;
      if (this->in_room)
        char_from_room (this);
      for (eobj = this->carrying; eobj != NULL; eobj = obj_next)
        {
          obj_next = eobj->next_content;
          free_it (eobj);
        }
      for (paf = this->affected; paf != NULL; paf = paf_next)
        {
          paf_next = paf->next;
          affect_remove (this, paf);
        }
      clear_fgt (this);
/*     if ( IS_MOB(this) && this->npcdata != NULL )
   {
   this->npcdata->next = npcdata_free;
   npcdata_free = this->npcdata;
   }
   this->npcdata=NULL; */
      if (IS_PLAYER (this) && this->pcdata && this->pcdata != &base_pc)
	{
        free_it (this->pcdata);
	}
      this->special|=ISMOB;
      this->desc=NULL;
      this->pcdata = &base_pc;
      this->next = char_free;
      char_free = this;
      this->data_type = 50;
/*free_m(this); */
    }
  else if (*t == K_PC)
    {
      int i;
      pcdata = (REALCHAR_DATA *) thing;
      if (pcdata == &base_pc)
        return;
      MARK_DEBUG ("free_it:pcdata");
      if (pcdata->challenged_by != NULL)
        {
          free_string (pcdata->challenged_by);
          pcdata->challenged_by = NULL;
        }
      for (i = 0; i < MAX_IGNORE; i++)
        {
          if (pcdata->ignore[i])
            {
              free_string (pcdata->ignore[i]);
            }
          pcdata->ignore[i] = NULL;
        }
      pcdata->bank = 0;
      for (i = 0; i < MAXST; i++)
        {
          if (pcdata->storage[i] != NULL)
	    {
            free_it (pcdata->storage[i]);
	    }
	    pcdata->storage[i] = NULL;
        }
      if (pcdata->challenged_by != NULL)
        free_string (pcdata->challenged_by);
      pcdata->challenged_by = NULL;
      if (pcdata->rprompt != NULL)
        free_string (pcdata->rprompt);
      pcdata->rprompt = NULL;
      if (pcdata->rpwd != NULL)
        free_string(pcdata->rpwd);
      pcdata->rpwd = NULL;
      if (pcdata->pwd != NULL)
        free_string (pcdata->pwd);
      pcdata->pwd = NULL;
      if (pcdata->beamin != NULL)
        free_string (pcdata->beamin);
      pcdata->beamin = NULL;
      if (pcdata->beamout != NULL)
        free_string (pcdata->beamout);
      pcdata->beamout = NULL;
      if (pcdata->title != NULL)
        free_string (pcdata->title);
      pcdata->title = NULL;
      pcdata->guarding = NULL;
      for (i = 0; i < 4; i++)
	pcdata->guarded_by[i] = NULL;
       for (i = 0; i < 4; i++)
	pcdata->armor[i] = NULL;

      /* --------------------- */
      /* Init last tells array */
      /* --------------------- */
      for (i = 0; i < 15; i++)
        {
          if (pcdata->last_tells[i])
            {
              free (pcdata->last_tells[i]);
              pcdata->last_tells[i] = NULL;
            }
        }
      /* ----------------------- */
      /* Init last tells counter */
      /* ----------------------- */
      pcdata->tell_counter = 0;

      /* --------------------------- */
      /* Init last group tells array */
      /* --------------------------- */
      for (i = 0; i < 15;i++)
      {
        if(pcdata->last_gtells[i])
        {
          free(pcdata->last_gtells[i]);
          pcdata->last_gtells[i] = NULL;
        }
      }
      /* ---------------------------- */
      /* Init last group tell counter */
      /* ---------------------------- */
      pcdata->gtell_counter = 0;

      /* --------------------------- */
      /* Init last clan  tells array */
      /* --------------------------- */
      for (i = 0; i < 15; i++)
      {
	if(pcdata->last_ctells[i])
	{
	  free(pcdata->last_ctells[i]);
	  pcdata->last_ctells[i] = NULL;
	}
      }

      /* ---------------------------- */
      /* Init last clan  tell counter */
      /* ---------------------------- */
      pcdata->ctell_counter = 0;


      for (i = 0; i < MAXALIAS; i++)
        {
          if (pcdata->alias[i] != NULL
              && pcdata->aliasname[i] != NULL)
            {
              free_string (pcdata->alias[i]);
              free_string (pcdata->aliasname[i]);
              pcdata->alias[i] = NULL;
              pcdata->aliasname[i] = NULL;
            }
        }
      pcdata->next = pcdata_free;
      pcdata_free = pcdata;
    }
  else if (*t == K_OBJ)
    {
      SCRIPT_INFO *inf;
      SCRIPT_INFO *inf_next;
      SINGLE_OBJECT *obj_content;
      SINGLE_OBJECT *obj_next;
      obj = (SINGLE_OBJECT *) thing;
      MARK_DEBUG ("free_it:object");
      for (inf = info_list; inf != NULL; inf = inf_next)
        {
          inf_next = inf->next;
          if (inf->obj == obj)
            {
              end_script (inf);
            }
        }
      if (obj->in_obj != NULL)
        {
          obj_from (obj);
          free_it (obj);
          return;
        }
      else if (obj->in_room != NULL)
        obj_from (obj);
      else if (obj->carried_by != NULL)
        {
          if (obj->wear_loc != -1)
            unequip_char (obj->carried_by, obj);
          obj_from (obj);
        }
      if (obj->contains)
        for (obj_content = obj->contains; obj_content; obj_content = obj_next)
          {
            obj_next = obj_content->next_content;
            free_it (obj->contains);
          }
      if (object_list == obj)
        {
          object_list = obj->next;
        }
      else
        {
          for (prev = object_list; prev && prev != NULL; prev = prev->next)
            if (prev->next == obj)
              {
                prev->next = obj->next;
                break;
              }
        }
      obj->boat = NULL;
      if (obj->short_descr != NULL)
        {
          free_string (obj->short_descr);
          obj->short_descr = NULL;
        }
      if (obj->description != NULL)
        {
          free_string (obj->description);
          obj->description = NULL;
        }
      if (obj->more)
        {
          free_m (obj->more);
          obj->more = NULL;
        }
      --obj->pIndexData->count;
      if (obj->affected != NULL)
	{
	  AFFECT_DATA *paf;
	  AFFECT_DATA *paf_next;
	  for (paf = obj->affected; paf != NULL; paf = paf_next)
	    {
	      paf_next = paf->next;
	      free_affect(paf);
	    }
	  obj->affected = NULL;
	  
	}
      obj->next = obj_free;
      obj_free = obj;
      
/*free_m(obj); */
    }
  return;
}


/*void add_to_mob_in_area_list(CHAR_DATA *mob, AREA_DATA *area) {
   CHAR_DATA *c;
   if (!area || !mob) return;
   for (c=mobs_in_area[area->vnum];c!=NULL;c=c->next_in_area) {
   if (c==mob) return;
   }
   mob->next_in_area = mobs_in_area[area->vnum];
   mobs_in_area[area->vnum]=mob;
   return;
   }

   void remove_from_mob_in_area_list(CHAR_DATA *mob, AREA_DATA *area) {
   CHAR_DATA *c;
   if (!mob || !area) return;
   for (c=mobs_in_area[area->vnum];c!=NULL;c=c->next_in_area) {
   if (c->next_in_area==mob) {
   c->next_in_area=mob->next_in_area;
   mob->next_in_area=NULL;
   return;
   }
   }
   return;
   }

   int count_mob_vnums_in_area(int vnum, AREA_DATA *area)
   {
   int i=0;
   CHAR_DATA *c;
   if (!area) return 0;
   for (c=mobs_in_area[area->vnum];c!=NULL;c=c->next_in_area) {
   if (IS_MOB(c) && c->pIndexData->vnum==vnum) i++;
   }
   return i;
   }
 */

