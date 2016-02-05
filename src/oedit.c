
#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emlen.h"

char *check_obj_name(char *nm, CHAR_DATA *ch) {
char *t=nm;
nm[0]=LOWER(nm[0]);
if (UPPER(*t)=='A' && *(t+1)==' ') {
	send_to_char("WARNING!  Obj name field should only contain KEYWORDS to access the mob.\n\r",ch);
	send_to_char("For instance: sword long longsword steel\n\r",ch);
	return nm;
	}
for (t=nm; *t!='\0'; t++) {
	if (*t==' ' && *(t+1)!='\0' && *(t+1)=='a' && *(t+2)==' ') {
		send_to_char("WARNING!  Obj name field should only contain KEYWORDS to access the mob.\n\r",ch);
		send_to_char("For instance: sword long longsword steel\n\r",ch);
		return nm;
		}
	}
return nm;
}

char *check_obj_short(char *nm,CHAR_DATA *ch) {
char *t;
bool warn=FALSE;
if (nm[0]=='\0') return nm;
nm[0]=LOWER(nm[0]);
for (t=nm;*t!='\0';t++) {
	if (!warn && *t<='Z' && *t>='A') {
		send_to_char("WARNING: Capitalized letters found in the obj short description.\n\r",ch);
		send_to_char("An object's short description should be something like: a dwarven longsword\n\r",ch);
		warn=TRUE;
		}
	}
t--;
if (*t=='.') *t='\0';
if (strlen(nm)>17) {
	send_to_char("You should try to keep an object's short description below 18 characters.\n\r",ch);
	send_to_char("Longer shorts may cause wordwraps at bad places and unnecessary spam.\n\r",ch);
	send_to_char("(Please keep this in mind and shorten the object's short, if possible).\n\r",ch);
	}
return nm;
}

char *check_obj_long(char *nm, CHAR_DATA *ch) {
char *t;
bool warn=FALSE;
if (nm[0]=='\0') return nm;
nm[0]=UPPER(nm[0]);
for (t=nm+1;*t!='\0';t++) {
	if (!warn && *t<='Z' && *t>='A') {
		send_to_char("WARNING: Capitalized letters found in the object's long description.\n\r",ch);
		send_to_char("An object's long description should be something like:\n\r",ch);
		send_to_char("\x1B[1;37mA dwarven longsword lies on the ground here.\x1B[37;0m\n\r",ch);
		warn=TRUE;
		}
	}
t-=3;
if (*t!='.' && *t!='!') {
	send_to_char("\x1B[1;33mWARNING!!!!!!!!  Your object's long descriptions MUST be a sentence.\n\r",ch);
	send_to_char("This means you should have a noun, verb, etc, and end it with a period.\n\r",ch);
	send_to_char("Here is a proper example:\n\r",ch);
	send_to_char("\x1B[1;37mA blade made of blue steel lies on the ground here.\x1B[1;33m\n\r",ch);
	send_to_char("PLEASE redo the long description so that it adheres to these rules.. thanks :)\x1B[37;0m\n\r",ch);
	}
return nm;
}


void 
do_rcopy (CHAR_DATA * ch, char *argy)
{
  char arg1[500];
  int orig_v;
  int dest_v;
  ROOM_DATA *orig;
  ROOM_DATA *dest;
  DEFINE_COMMAND ("roomcopy", do_rcopy, POSITION_DEAD, MAX_LEVEL, LOG_ALWAYS, "Copies room vnum <orig> to vnum <dest>.  Use with care!!")
    argy = one_argy (argy, arg1);
  if (!is_number (arg1) || !is_number (argy))
    {
      send_to_char ("Syntax is: <original vnum> <destination vnum>.\n\r", ch);
      return;
    }
  orig_v = atoi (arg1);
  dest_v = atoi (argy);
  if ((orig = get_room_index (orig_v)) == NULL || (dest = get_room_index (dest_v)) == NULL)
    {
      send_to_char ("One of the rooms was not found.\n\r", ch);
      return;
    }
  dest->shade = orig->shade;
  dest->name = str_dup (orig->name);
  dest->description = str_dup (orig->description);
  dest->room_flags = orig->room_flags;
  dest->room_flags2 = orig->room_flags2;
  dest->sector_type = orig->sector_type;
  if (dest->area)
    SET_BIT (dest->area->area_flags, AREA_CHANGED);
  send_to_char ("Room copy completed.\n\r", ch);
  return;
}

void 
do_mcopy (CHAR_DATA * ch, char *argy)
{
  char arg1[500];
  int orig_v;
  int dest_v;
  MOB_PROTOTYPE *orig;
  MOB_PROTOTYPE *dest;
  DEFINE_COMMAND ("mobcopy", do_mcopy, POSITION_DEAD, MAX_LEVEL, LOG_ALWAYS, "Copies mob vnum <orig> to vnum <dest>.  Use with care!!")
    argy = one_argy (argy, arg1);
  if (!is_number (arg1) || !is_number (argy))
    {
      send_to_char ("Syntax is: <original vnum> <destination vnum>.\n\r", ch);
      return;
    }
  orig_v = atoi (arg1);
  dest_v = atoi (argy);
  if ((orig = get_mob_index (orig_v)) == NULL || (dest = get_mob_index (dest_v)) == NULL)
    {
      send_to_char ("One of the mob prototypes was not found.\n\r", ch);
      return;
    }
  if (dest->max_in_world != 5)
    {
      send_to_char ("It looks as if the destination vnum isn't unused!\n\r", ch);
      return;
    }
  if (orig->spec_name)
    dest->spec_name = str_dup (orig->spec_name);
  dest->name = str_dup (orig->name);
  dest->short_descr = str_dup (orig->short_descr);
  dest->long_descr = str_dup (orig->long_descr);
  dest->sex = orig->sex;
  dest->level = orig->level;
  dest->clan_guard_1 = orig->clan_guard_1;
  dest->clan_guard_2 = orig->clan_guard_2;
  dest->act = orig->act;
  dest->act3 = orig->act3;
  dest->act4 = orig->act4;
  dest->affected_by = orig->affected_by;
  dest->more_affected_by = orig->more_affected_by;
  dest->alignment = orig->alignment;
  dest->money = orig->money;
  dest->height = orig->height;
  dest->mobtype = orig->mobtype;
  dest->hpmult = orig->hpmult;
  dest->armcls = orig->armcls;
  dest->race_hate = orig->race_hate;
  dest->guard = orig->guard;
  dest->hitroll = orig->hitroll;
  dest->damroll = orig->damroll;
  dest->dodge = orig->dodge;
  dest->parry = orig->parry;
  if (dest->area)
    SET_BIT (dest->area->area_flags, AREA_CHANGED);
  send_to_char ("Mob copy completed.  Don't forget to type in a NEW description for this mob.\n\r", ch);
  return;
}

void 
do_ocopy (CHAR_DATA * ch, char *argy)
{
  char arg1[500];
  int orig_v;
  int i;
  int dest_v;
  OBJ_PROTOTYPE *orig;
  OBJ_PROTOTYPE *dest;
  DEFINE_COMMAND ("objcopy", do_ocopy, POSITION_DEAD, MAX_LEVEL, LOG_ALWAYS, "Copies object vnum <orig> to vnum <dest>.  Use with care!!")
    argy = one_argy (argy, arg1);
  if (!is_number (arg1) || !is_number (argy))
    {
      send_to_char ("Syntax is: <original vnum> <destination vnum>.\n\r", ch);
      return;
    }
  orig_v = atoi (arg1);
  dest_v = atoi (argy);
  if ((orig = get_obj_index (orig_v)) == NULL || (dest = get_obj_index (dest_v)) == NULL)
    {
      send_to_char ("One of the object prototypes was not found.\n\r", ch);
      return;
    }
  if (dest->item_type != ITEM_TRASH || dest->value[3] != 0)
    {
      send_to_char ("There appears to be an object in the destination already!!\n\r", ch);
      return;
    }
  dest->name = str_dup (orig->name);
  dest->short_descr = str_dup (orig->short_descr);
  dest->description = str_dup (orig->description);
  dest->item_type = orig->item_type;
  dest->timer = orig->timer;
  dest->extra_flags = orig->extra_flags;
  dest->wear_flags = orig->wear_flags;
  dest->weight = orig->weight;
  for (i = 0; i < 11; i++)
    {
      dest->value[i] = orig->value[i];
    }
  dest->cost = orig->cost;
  if (dest->area)
    SET_BIT (dest->area->area_flags, AREA_CHANGED);
  send_to_char ("Object copy completed.\n\r", ch);
  return;
}

void 
oedit (CHAR_DATA * ch, char *argy)
{
  AREA_DATA *pArea;
  OBJ_PROTOTYPE *pObj;
  DESCRIPTION_DATA *ed;
  AFFECT_DATA *pAf;
  char arg[STD_LENGTH];
  char arg1[STD_LENGTH];
  char arg2[STD_LENGTH];
  char buf[STD_LENGTH];
  long long value;
  int iHash;
  char str[STD_LENGTH + 5];
  strcpy (arg, argy);
  smash_tilde (argy);
  argy = one_argy (argy, arg1);
  strcpy (arg2, argy);
  pObj = (OBJ_PROTOTYPE *) ch->desc->pEdit;
  pArea = pObj->area;
  hugebuf_o[0]='\0';

  if (!str_cmp (arg1, "show") || arg1[0] == '\0') {
    sprintf (buf, "%d", pObj->vnum);
    do_oindex (ch, buf);
    return;
  }
  if (!str_cmp (arg1, "?")) {
    do_help (ch, "oedit");
    return;
  }

  if (get_direction_number (arg1) == DIR_MAX)  {
    switch (UPPER(arg1[0]))
      {
      case 'A':
	if (!str_cmp (arg1, "addaffect"))
	  {
	    char arg3[STD_LENGTH];
	    argy = one_argy (argy, arg1);
	    argy = one_argy (argy, arg2);
	    strcpy (arg3, argy);
	    if (arg1[0] == '\0' || arg2[0] == '\0' || (!is_number (arg2) && atoi (arg2) * -1 == 0)
		|| arg3[0] == '\0' || !is_number (arg3))
	      {
		send_to_char ("Syntax: addaffect|remaffect [what-affect-added] [modifier #] [1]\n\r", ch);
		return;
	      }
	    pAf = new_affect ();
	    pAf->location = affect_name_loc (arg1);
	    pAf->modifier = atoi (arg2);
	    pAf->type = atoi (arg3);
	    pAf->duration = 0;
	    pAf->bitvector = 0;
	    pAf->next = pObj->affected;
	    pObj->affected = pAf;
	    SET_BIT (pArea->area_flags, AREA_CHANGED);
	    send_to_char ("Affect added.\n\r", ch);
	    return;
	  }
	
	break;
      case 'C':
	if (!str_cmp (arg1, "cost"))
	  {
	    if (arg2[0] == '\0' || !is_number (arg2))
	      {
		send_to_char ("Syntax: cost [number]\n\r", ch);
		return;
	      }
	    value = atoi (arg2);
	    pObj->cost = value;
	    SET_BIT (pArea->area_flags, AREA_CHANGED);
	    send_to_char ("Cost set.\n\r", ch);
	    return;
	  }
	
	if (!str_cmp (arg1, "create"))
	  {
	    value = atoi (arg2);
	    if (arg2[0] == '\0' || value == 0)
	      {
		send_to_char ("Syntax: oedit create [vnum]\n\r", ch);
		return;
	      }
	    pArea = get_vnum_area (value);
	    if (pArea == NULL)
	      {
		send_to_char ("That vnum is not assigned an area.\n\r", ch);
		return;
	      }
	    if (!IS_BUILDER (ch, pArea))
	      {
		send_to_char ("Vnum in an area you can't build in.\n\r", ch);
		return;
	      }
	    if (get_obj_index (value) != NULL)
	      {
		send_to_char ("Object vnum already exists.\n\r", ch);
		return;
	      }
	    pObj = new_obj_index ();
	    pObj->vnum = value;
	    pObj->area = pArea;
	    if (value > top_vnum_obj)
	      top_vnum_obj = value;
	    iHash = value % HASH_MAX;
	    pObj->next = obj_index_hash[iHash];
	    obj_index_hash[iHash] = pObj;
	    ch->desc->pEdit = (void *) pObj;
	    SET_BIT (pArea->area_flags, AREA_CHANGED);
	    send_to_char ("Object Created.\n\r", ch);
	    pArea->obj_count++;
	    return;
	  }
	
	break;
      case 'D':
	if (!str_cmp (arg1, "done"))
	  {
	    ch->desc->pEdit = NULL;
	    ch->desc->connected = CON_PLAYING;
	    return;
	  }
	if (!str_cmp (arg1, "delaffect") || !str_cmp (arg1, "remaffect"))
	  {
	    pAf = pObj->affected;
	    if (pAf == NULL)
	      {
		send_to_char ("No affects on this object!\n\r", ch);
		return;
	      }
	    pObj->affected = pObj->affected->next;
	    free_affect (pAf);
	    SET_BIT (pArea->area_flags, AREA_CHANGED);
	    send_to_char ("Affect removed.\n\r", ch);
	    return;
	  }
	
	break;
      case 'E':
	
	if (!str_cmp (arg1, "ed"))
	  {
	    if (arg2[0] == '\0')
	      {
		send_to_char ("Syntax: ed add [keyword]\n\r", ch);
		send_to_char (" ed delete [keyword]\n\r", ch);
		send_to_char (" ed edit [keyword]\n\r", ch);
		send_to_char (" ed format [keyword]\n\r", ch);
		return;
	      }
	    argy = one_argy (argy, arg1);
	    strcpy (arg2, argy);
	    if (!str_cmp (arg1, "add"))
	      {
		if (arg2[0] == '\0')
		  {
		    send_to_char ("Syntax: ed add [keyword]\n\r", ch);
		    return;
		  }
		ed = new_extra_descr ();
		ed->keyword = str_dup (arg2);
		ed->next = pObj->extra_descr;
		pObj->extra_descr = ed;
		string_append (ch, &ed->description);
		SET_BIT (pArea->area_flags, AREA_CHANGED);
		return;
	      }
	    if (!str_cmp (arg1, "edit"))
	      {
		if (arg2[0] == '\0')
		  {
		    send_to_char ("Syntax: ed edit [keyword]\n\r", ch);
		    return;
		  }
		for (ed = pObj->extra_descr; ed != NULL; ed = ed->next)
		  {
		    if (is_name (arg2, ed->keyword))
		      break;
		  }
		if (ed == NULL)
		  {
		    send_to_char ("Extra description keyword not found.\n\r", ch);
		    return;
		  }
		string_append (ch, &ed->description);
		SET_BIT (pArea->area_flags, AREA_CHANGED);
		return;
	      }
	    if (!str_cmp (arg1, "append"))
	      {
		if (arg2[0] == '\0')
		  {
		    send_to_char ("Syntax: ed edit [keyword]\n\r", ch);
		    return;
		  }
		for (ed = pObj->extra_descr; ed != NULL; ed = ed->next)
		  {
		    if (is_name (arg2, ed->keyword))
		      break;
		  }
		if (ed == NULL)
		  {
		    send_to_char ("Extra description keyword not found.\n\r", ch);
		    return;
		  }
		string_append (ch, &ed->description);
		SET_BIT (pArea->area_flags, AREA_CHANGED);
		return;
	      }
	    if (!str_cmp (arg1, "delete"))
	      {
		DESCRIPTION_DATA *ped;
		if (arg2[0] == '\0')
		  {
		    send_to_char ("Syntax: ed delete [keyword]\n\r", ch);
		    return;
		  }
		ped = NULL;
		for (ed = pObj->extra_descr; ed != NULL; ed = ed->next)
		  {
		    if (is_name (arg2, ed->keyword))
		      break;
		    ped = ed;
		  }
		if (ed == NULL)
		  {
		    send_to_char ("Extra description keyword not found.\n\r", ch);
		    return;
		  }
		if (ped == NULL)
		  {
		    pObj->extra_descr = ed->next;
		  }
		else
		  {
		    ped->next = ed->next;
		  }
		free_extra_descr (ed);
		send_to_char ("Extra description deleted.\n\r", ch);
		SET_BIT (pArea->area_flags, AREA_CHANGED);
		return;
	      }
	    if (!str_cmp (arg1, "format"))
	      {
		DESCRIPTION_DATA *ped;
		if (arg2[0] == '\0')
		  {
		    send_to_char ("Syntax: ed format [keyword]\n\r", ch);
		    return;
		  }
		ped = NULL;
		for (ed = pObj->extra_descr; ed != NULL; ed = ed->next)
		  {
		    if (is_name (arg2, ed->keyword))
		      break;
		    ped = ed;
		  }
		if (ed == NULL)
		  {
		    send_to_char ("Extra description keyword not found.\n\r", ch);
		    return;
		  }
		ed->description = format_string (ed->description);
		send_to_char ("Extra description formatted.\n\r", ch);
		SET_BIT (pArea->area_flags, AREA_CHANGED);
		return;
	      }
	  } 
	break;
      case 'L':
	
	if (!str_cmp (arg1, "long"))
	  {
	    if (arg2[0] == '\0')
	      {
		send_to_char ("Syntax: long [string]\n\r", ch);
		return;
	      }
	    strcat (arg2, "\n\r");
	    free_string (pObj->description);
	    sprintf (str, "%s\x1B[0;36m", anseval (str_dup (check_obj_long(arg2,ch)), ch));
	    pObj->description = str_dup (str);
	    SET_BIT (pArea->area_flags, AREA_CHANGED);
	    send_to_char ("Long description set.\n\r", ch);
	    return;
	  }
	break;
      case 'M':
	if (!str_cmp (arg1, "max"))
	  {
	    argy = one_argy (argy, arg1);
	    if (!is_number (arg1))
	      {
		send_to_char ("Usage: max <number>\n\r", ch);
		return;
	      }
	    if (atoi (arg1) < 1 || atoi (arg1) > 50)
	      {
		send_to_char ("Must be a range between 1 and 50.\n\r", ch);
		return;
	      }
	    pObj->max_in_room = atoi (arg1);
	    send_to_char ("Max items concurrently in 1 room value Set :)\n\r", ch);
	    SET_BIT (pArea->area_flags, AREA_CHANGED);
	    return;
	  }
	if (!str_cmp (arg1, "material"))
	  {
	    int ii=0;
	    char bf[500];
	    argy = one_argy (argy, arg1);
	    if (!is_number (arg1))
	      {
		send_to_char ("Usage: material <number>\n\r", ch);
		while (str_cmp(materials[ii].name,"END")) {
		  sprintf(bf,"%d) %s.\n\r",ii+1,materials[ii].name);
		  send_to_char(bf,ch);
		}
		return;
	      }
	    if (atoi (arg1) < 0 )
	      {
		pObj->made_of=0;
		SET_BIT (pArea->area_flags, AREA_CHANGED);
		return;
	      }
	    pObj->made_of = atoi (arg1);
	    ii=0;
	    while (str_cmp(materials[ii].name,"END")) ii++;
	    if (pObj->made_of > ii) {
	      pObj->made_of=0;
	      send_to_char("Material number too high.  Not set.\n\r",ch);
	      return;
	    }
	    send_to_char ("Material set.\n\r", ch);
	    SET_BIT (pArea->area_flags, AREA_CHANGED);
	    return;
	  }
	
	break;
      case 'N':
	if (!str_cmp (arg1, "name"))
	  {
	    if (arg2[0] == '\0')
	{
	  send_to_char ("Syntax: name [string]\n\r", ch);
	  return;
	}
	    free_string (pObj->name);
	    pObj->name = str_dup (check_obj_name(arg2,ch));
	    SET_BIT (pArea->area_flags, AREA_CHANGED);
	    send_to_char ("Name set.\n\r", ch);
	    return;
	  }
	
	break;
	case 'R':
	  if (!str_cmp (arg1, "delaffect") || !str_cmp (arg1, "remaffect"))
	    {
	      pAf = pObj->affected;
	      if (pAf == NULL)
		{
		  send_to_char ("No affects on this object!\n\r", ch);
		  return;
		}
	      pObj->affected = pObj->affected->next;
	      free_affect (pAf);
	      SET_BIT (pArea->area_flags, AREA_CHANGED);
	      send_to_char ("Affect removed.\n\r", ch);
	      return;
	    }
	  
	  break;
      case 'S':
	if (!str_cmp (arg1, "short"))
	  {
	    if (arg2[0] == '\0')
	      {
		send_to_char ("Syntax: short [string]\n\r", ch);
		return;
	      }
	    free_string (pObj->short_descr);
	    strcpy (str, anseval (str_dup (arg2), ch));
	    pObj->short_descr = str_dup (check_obj_short(str,ch));
	    SET_BIT (pArea->area_flags, AREA_CHANGED);
	    send_to_char ("Short description set.\n\r", ch);
	    return;
	  }
	
	break;
      case 'T':
	if (!str_cmp (arg1, "timer"))
	  {
	    if (arg2[0] == '\0' || !is_number (arg2))
	      {
		send_to_char ("Syntax: timer [number]\n\r", ch);
		return;
	      }
	    pObj->timer = atoi (arg2);
	    SET_BIT (pArea->area_flags, AREA_CHANGED);
	    send_to_char ("Timer set.\n\r", ch);
	    return;
	  }
	
	break;
      case 'G':
	if (!str_cmp (arg1, "guild"))
	  {
	    /*Set Guild Specific Stats Here*/
	    int i;
	    if (arg2[0] == '\0')
	    {
		send_to_char ("Syntax is: guild <guildname>\n\r", ch);
		return;
	    }
	    for (i = 0; str_cmp(guild_data[i].what_you_type, "end_of_list");i++)
	    {
		if (!str_cmp(guild_data[i].what_you_type, arg2))
		{
			char buf[100];
			sprintf (buf, guild_data[i].what_you_see);
			if (IS_SET(pObj->value[10], guild_data[i].mob_guildmaster_bit))
	  		  send_to_char("Guild Removed\n\r", ch);
			else
			  send_to_char("Guild Added\n\r", ch);
			pObj->value[10] ^= guild_data[i].mob_guildmaster_bit;
			SET_BIT (pArea->area_flags, AREA_CHANGED);
			return;
		}
	    }
	    send_to_char ("Syntax is: guild <guildname>\n\r", ch);
	    return;
	  }
	break;
      case 'V':
	if (!str_cmp (arg1, "value0") || !str_cmp (arg1, "v0"))
	  {
	    if (arg2[0] == '\0' || (!is_number (arg2) && atoi (arg2) == 0))
	      {
		send_to_char ("Syntax: value0 [number]\n\r", ch);
		send_to_char ("or v0 [number]\n\r", ch);
		return;
	      }
	    value = atoi (arg2);
	    pObj->value[0] = value;
	    SET_BIT (pArea->area_flags, AREA_CHANGED);
	    send_to_char ("Value 0 set.\n\r", ch);
	    return;
	  }
	if (!str_cmp (arg1, "value1") || !str_cmp (arg1, "v1"))
	  {
	    if (arg2[0] == '\0' || (!is_number (arg2) && atoi (arg2) == 0))
	      {
		send_to_char ("Syntax: value1 [number]\n\r", ch);
		send_to_char ("or v1 [number]\n\r", ch);
		return;
	      }
	    value = atoi (arg2);
	    pObj->value[1] = value;
	    SET_BIT (pArea->area_flags, AREA_CHANGED);
	    send_to_char ("Value 1 set.\n\r", ch);
	    return;
	  }
	if (!str_cmp (arg1, "value2") || !str_cmp (arg1, "v2"))
	  {
	    if (arg2[0] != '\0' && !str_cmp (arg2, "-1"))
	      {
		pObj->value[2] = -1;
		SET_BIT (pArea->area_flags, AREA_CHANGED);
		send_to_char ("Value 2 set (lampoil).\n\r", ch);
		return;
	      }
	    if (arg2[0] == '\0' || !is_number (arg2))
	      {
		send_to_char ("Syntax: value2 [number]\n\r", ch);
		send_to_char ("or v2 [number]\n\r", ch);
		return;
	      }
	    value = atoi (arg2);
	    pObj->value[2] = value;
	    SET_BIT (pArea->area_flags, AREA_CHANGED);
	    send_to_char ("Value 2 set.\n\r", ch);
	    return;
	  }
	if (!str_cmp (arg1, "value3") || !str_cmp (arg1, "v3"))
	  {
	    if (arg2[0] == '\0' || !is_number (arg2))
	      {
		send_to_char ("Syntax: value3 [number]\n\r", ch);
		send_to_char ("or v3 [number]\n\r", ch);
		return;
	      }
	    value = atoi (arg2);
	    pObj->value[3] = value;
	    SET_BIT (pArea->area_flags, AREA_CHANGED);
	    send_to_char ("Value 3 set.\n\r", ch);
	    return;
	  }
	if (!str_cmp (arg1, "value4") || !str_cmp (arg1, "v4"))
	  {
	    if (arg2[0] == '\0' || !is_number (arg2))
	      {
		send_to_char ("Syntax: value4 [number]\n\r", ch);
		send_to_char ("or v4 [number]\n\r", ch);
		return;
	      }
	    value = atoi (arg2);
	    pObj->value[4] = value;
	    SET_BIT (pArea->area_flags, AREA_CHANGED);
	    send_to_char ("Value 4 set.\n\r", ch);
	    return;
	  }
	if (!str_cmp (arg1, "value5") || !str_cmp (arg1, "v5"))
	  {
	    if (arg2[0] == '\0' || !is_number (arg2))
	      {
		send_to_char ("Syntax: value5 [number]\n\r", ch);
		send_to_char ("or v5 [number]\n\r", ch);
		return;
	      }
	    value = atoi (arg2);
	    pObj->value[5] = value;
	    SET_BIT (pArea->area_flags, AREA_CHANGED);
	    send_to_char ("Value 5 set.\n\r", ch);
	    return;
	  }
	if (!str_cmp (arg1, "value6") || !str_cmp (arg1, "v6"))
	  {
	    if (arg2[0] == '\0' || !is_number (arg2))
	      {
		send_to_char ("Syntax: value6 [number]\n\r", ch);
		send_to_char ("or v6 [number]\n\r", ch);
		return;
	      }
	    value = atoi (arg2);
	    pObj->value[6] = value;
	    SET_BIT (pArea->area_flags, AREA_CHANGED);
	    send_to_char ("Value 6 set.\n\r", ch);
	    return;
	  }
	if (!str_cmp (arg1, "value7") || !str_cmp (arg1, "v7"))
	  {
	    if (arg2[0] == '\0' || !is_number (arg2))
	      {
		send_to_char ("Syntax: value7 [number]\n\r", ch);
		send_to_char ("or v7 [number]\n\r", ch);
		return;
	      }
	    value = atoi (arg2);
	    pObj->value[7] = value;
	    SET_BIT (pArea->area_flags, AREA_CHANGED);
	    send_to_char ("Value 7 set.\n\r", ch);
	    return;
	  }
	if (!str_cmp (arg1, "value8") || !str_cmp (arg1, "v8"))
	  {
	    if (arg2[0] == '\0' || !is_number (arg2))
	      {
		send_to_char ("Syntax: value8 [number]\n\r", ch);
		send_to_char ("or v8 [number]\n\r", ch);
		return;
	      }
	    value = atoi (arg2);
	    pObj->value[8] = value;
	    SET_BIT (pArea->area_flags, AREA_CHANGED);
	    send_to_char ("Value 8 set.\n\r", ch);
	    return;
	  }
	if (!str_cmp (arg1, "value9") || !str_cmp (arg1, "v9"))
	  {
	    if (arg2[0] == '\0' || !is_number (arg2))
	      {
		send_to_char ("Syntax: value9 [number]\n\r", ch);
		send_to_char ("or v9 [number]\n\r", ch);
		return;
	      }
	    value = atoi (arg2);
	    pObj->value[9] = value;
	    SET_BIT (pArea->area_flags, AREA_CHANGED);
	    send_to_char ("Value 9 set.\n\r", ch);
	    return;
	  }
	
	break;
	case 'W':
	  if (!str_cmp (arg1, "weight"))
	    {
	      if (arg2[0] == '\0' || !is_number (arg2))
		{
		  send_to_char ("Syntax: weight [number]\n\r", ch);
		  return;
		}
	      value = atoi (arg2);
	      pObj->weight = value;
	      SET_BIT (pArea->area_flags, AREA_CHANGED);
	      send_to_char ("Weight set.\n\r", ch);
	      return;
	    }
	  break;
      }
 
  if ((value = item_name_type (arg1)) != ITEM_NONE)
    {
      pObj->item_type = value;
      SET_BIT (pArea->area_flags, AREA_CHANGED);
      send_to_char ("Type set.\n\r", ch);
      return;
    }
 
  if ((value = extra_name_bit (arg1)) != EXTRA_NONE)
    {
      if (!str_cmp(arg1, "wounding") || !str_cmp(arg1, "blinding")||!str_cmp(arg1, "poisoning")  ||!str_cmp(arg1, "elemental") || !str_cmp(arg1, "power"))
      if (LEVEL (ch) > MAX_LEVEL)
	{
	  send_to_char ("Due to overuse of this flag, gods only may now set it.\n\r", ch);
	  return;
	}
      pObj->extra_flags ^= value;
      SET_BIT (pArea->area_flags, AREA_CHANGED);
      send_to_char ("Extra flag toggled.\n\r", ch);
      return;
    }
  if ((value = item_guild_bit (arg1)) != EXTRA_NONE)
    {
      pObj->guild_flags ^= value;
      SET_BIT (pArea->area_flags, AREA_CHANGED);
      send_to_char ("Guild flag toggled.\n\r", ch);
      return;
    }
  if ((value = wear_name_bit (arg1)) != ITEM_WEAR_NONE)
    {
      pObj->wear_flags ^= value;
      SET_BIT (pArea->area_flags, AREA_CHANGED);
      send_to_char ("Wear flag toggled.\n\r", ch);
      return;
    }
  }
if (hugebuf_o[0]!='\0') {
	page_to_char(hugebuf_o,ch);
	return;
	}
  interpret (ch, arg);
  return;
}
