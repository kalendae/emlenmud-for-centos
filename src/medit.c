#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emlen.h"

char *check_mob_name(char *nm, CHAR_DATA *ch) {
char *t;
t=nm;
nm[0]=LOWER(nm[0]); 
if (UPPER(*t)=='A' && *(t+1)==' ') {
	send_to_char("WARNING!  Mob name field should only contain KEYWORDS to access the mob.\n\r",ch);
	send_to_char("For instance: dwarf dwarven king stocky\n\r",ch);
	return nm;
	}
for (t=nm; *t!='\0'; t++) {
	if (*t==' ' && *(t+1)!='\0' && *(t+1)=='a' && *(t+2)==' ') {
		send_to_char("WARNING!  Mob name field should only contain KEYWORDS to access the mob.\n\r",ch);
		send_to_char("For instance: dwarf dwarven king stocky\n\r",ch);
		return nm;
		}
	}
return nm;
}

char *check_mob_short(char *nm,CHAR_DATA *ch) {
char *t;
bool warn=FALSE;
if (nm[0]=='\0') return nm;
for (t=nm;*t!='\0';t++) {
	if (!warn && *t<='Z' && *t>='A') {
		send_to_char("WARNING: Capitalized letters found in the mob short description.\n\r",ch);
		send_to_char("A mob's short description should be something like: a dwarven adventurer\n\r",ch);
		send_to_char("If you are specifying a name like 'king Ralysuzki', ignore this warning,\n\r",ch);
		send_to_char("otherwise, please redo the short without using capitals.\n\r",ch);
		warn=TRUE;
		}
	}
t--;
if (*t=='.') *t='\0';
if (strlen(nm)>17) {
	send_to_char("You should try to keep mob 'short's down below 18 or so characters.\n\r",ch);
	send_to_char("Longer shorts may cause wordwraps at bad places and unnecessary spam.\n\r",ch);
	send_to_char("(Please keep this in mind and shorten the mob's short, if possible).\n\r",ch);
	}
return nm;
}

char *check_mob_long(char *nm, CHAR_DATA *ch) {
char *t;
bool warn=FALSE;
if (nm[0]=='\0') return nm;
nm[0]=UPPER(nm[0]);
for (t=nm+1;*t!='\0';t++) {
	if (!warn && *t<='Z' && *t>='A') {
		send_to_char("WARNING: Capitalized letters found in the mob long description.\n\r",ch);
		send_to_char("A mob's long description should be something like:\n\r",ch);
		send_to_char("A massive dwarven gateguard stands here, guarding the gates of the city.\n\r",ch);
		send_to_char("If you are specifying a name like:\n\r",ch);
		send_to_char("King Ralysuzki stands here, talking with Fred the baker. then ignore this\n\r",ch);
		send_to_char("warning.  Otherwise, please redo the long desc without using capitals.\n\r",ch);
		warn=TRUE;
		}
	}
t-=3;
if (*t!='.' && *t!='!') {
	send_to_char("\x1B[1;33mWARNING!!!!!!!!  Your mob long descriptions MUST be a sentence.\n\r",ch);
	send_to_char("This means you should have a noun, verb, etc, and end it with a period.\n\r",ch);
	send_to_char("Here is a proper example:\n\r",ch);
	send_to_char("A timid-looking dwarven explorer forages for food here.\n\r",ch);
	send_to_char("PLEASE redo the long description so that it adheres to these rules.. thanks :)\x1B[37;0m\n\r",ch);
	}
return nm;
}

void 
medit (CHAR_DATA * ch, char *argy)
{
  AREA_DATA *pArea;
  MOB_PROTOTYPE *pMob;
  char arg[STD_LENGTH];
  char arg1[STD_LENGTH];
  char arg2[STD_LENGTH];
  char buf[STD_LENGTH];
  int value;
  int nextfree;
  int iHash;
  strcpy (arg, argy);
  smash_tilde (argy);
  argy = one_argy (argy, arg1);
  strcpy (arg2, argy);
  value = atoi (arg2);
  pMob = (MOB_PROTOTYPE *) ch->desc->pEdit;
  pArea = pMob->area;
hugebuf_o[0]='\0';
if (!str_cmp (arg1, "show") || arg1[0] == '\0')
  {
    sprintf (buf, "%d", pMob->vnum);
    mindex_window (ch, pMob, -1);
    return;
  }
 if (get_direction_number (arg1) == DIR_MAX)
   {
switch (UPPER(arg1[0]))
  {
  case 'A':
    if (!str_cmp (arg1, "avenger"))
      {
	if (LEVEL(ch) < MAX_LEVEL)
	  return;
	
	if (arg2[0] == '\0')
	  {
	    send_to_char ("Syntax: avenger <vnum of avenger mob>\n\r", ch);
	    return;
	  }
	if (!IS_SET(pMob->act, ACT_AVENGED))
	  {
	    send_to_char ("This mob is not avenged when it dies.\n\r", ch);
	    return;
	  }
	if (!pMob->opt)
	  pMob->opt = new_optional();
	pMob->opt->avenger = atoi (arg2);
	mindex_window (ch, pMob, 6);
	SET_BIT (pArea->area_flags, AREA_CHANGED);
	send_to_char ("Avenger set.\n\r", ch);
	return;
      }
 
    if (!str_cmp (arg1, "armor"))
      {
	if (value != 0)
	  {
	    pMob->armcls = value;
	    mindex_window (ch, pMob, 2);
	    if (value != 101)
	      {
		send_to_char ("Armor Class of Mob Set.\n\r", ch);
		return;
	      }
	    else
	      {
		send_to_char ("Mob's armor class will now be computed automatically.\n\r", ch);
		return;
	      }
	  }
      }
    if (!str_cmp (arg1, "alignment"))
      {
	if (arg2[0] == '\0' || (!is_number (arg2) && atoi (arg2) == 0))
	  {
	    send_to_char ("Syntax: alignment [number]\n\r", ch);
	    return;
	  }
	pMob->alignment = atoi (arg2);
	if (pMob->alignment > NUM_ALIGN) pMob->alignment = 2;
	if (pMob->alignment < 0) pMob->alignment = 1;
	mindex_window (ch, pMob, 2);
	SET_BIT (pArea->area_flags, AREA_CHANGED);
	send_to_char ("Alignment set.\n\r", ch);
	return;
      }
    if (!str_cmp (arg1, "alter"))
      {
	if (arg2[0] == '\0' || (!is_number (arg2) && atoi (arg2) > -1))
	  {
	    send_to_char ("Syntax: alter [vnum]\n\r", ch);
	    return;
	  }
	if (get_mob_index (atoi (arg2)) == NULL && (atoi (arg2) != 0))
	  {
	    send_to_char ("No mob with that vnum.\n\r", ch);
	    return;
	  }
	pMob->alt_vnum = atoi (arg2);
	mindex_window (ch, pMob, 3);
	SET_BIT (pArea->area_flags, AREA_CHANGED);
	send_to_char ("Alter set.\n\r", ch);
	return;
      }
    if (!str_cmp (arg1, "attackname"))
      {
	if (LEVEL (ch) < MAX_LEVEL)
	  {
	    send_to_char ("Please ask a GOD to set this for you. It should be used very rarely.\n\r", ch);
	    return;
	  }
	argy = one_argy (argy, arg1);
	if (arg1[0] == '\0' || arg1[0] == ' ')
	  {
	    if (!str_cmp ("N/A", pMob->attackname) || !pMob->attackname)
	      {
		send_to_char ("Syntax: attackname NAME\n\r", ch);
		return;
	      }
	    if (pMob->attackname)
	      free_string (pMob->attackname);
	    pMob->attackname = str_dup ("N/A");
	    mindex_window (ch, pMob, 6);
	    send_to_char ("Attack name cleared.\n\r", ch);
	    return;
	  }
	if (strlen (arg1) > 15)
	  {
	    send_to_char ("Attacks can't be that LONG. Please ask for help.\n\r", ch);
	    return;
	  }
	if (pMob->attackname)
	  free_string (pMob->attackname);
	pMob->attackname = str_dup (arg1);
	mindex_window (ch, pMob, 6);
	send_to_char ("Attack name set. Please playtest this to see if it is what you like\n\rbefore you continue with building more mobs :)\n\r", ch);
	return;
      }
    /*if ( !str_cmp( arg1, "attack" ) )
      {
      int i;
      int iType;
      if (LEVEL(ch)<MAX_LEVEL) 
      {
      send_to_char("If you REALLY need to set this powerful option, ask a GOD.\n\r",ch); return;
      }
      argy = one_argy( argy, arg1 );
      strcpy( arg2, argy );
      if ( !is_number( arg1 ) )
      {
      send_to_char( "Syntax: attack [num] [weapon type] [min dam] [max dam]\n\r", ch );
      send_to_char( " attack [num] delete\n\r", ch );
      return;
      }
      i = URANGE( 0, atoi( arg1 ), MAX_ATTACK_DATA-1 );
      argy = one_argy( argy, arg1 );
      strcpy( arg2, argy );
      if ( !str_cmp( arg1, "delete" ) )
      {
      if ( pMob->attacks[i] != NULL )
      {
      free_attack( pMob->attacks[i] );
      pMob->attacks[i] = NULL;
      send_to_char( "Attack deleted.\n\r", ch );
      return;
      }
      else
      {
      send_to_char( "Attack does not exist already.\n\r", ch );
      return;
      }
      }
      for ( iType = 0; iType < MAX_ATTACK; iType++ )
      {
      if ( !str_cmp( arg1, attack_table[iType].name ) )
      break;
      }
      if ( iType >= MAX_ATTACK )
      {
      send_to_char( "Invalid weapon type (see IREF WEAPONS).\n\r", ch );
      return;
      }
      if ( pMob->attacks[i] == NULL )
      {
      send_to_char( "Attack added.\n\r", ch );
      pMob->attacks[i] = new_attack( );
      }
      else
      {
      send_to_char( "Attack modified.\n\r", ch );
      }
      pMob->attacks[i]->next = NULL;
      pMob->attacks[i]->idx = iType;
      argy = one_argy( argy, arg1 );
      argy = one_argy( argy, arg2 );
      pMob->attacks[i]->dam1 = atoi( arg1 );
      pMob->attacks[i]->dam2 = atoi( arg2 );
      return;
      } */
    
    break;
  case 'C':
    if (!str_cmp (arg1, "create"))
      {
	value = atoi (argy);
	if (argy[0] == '\0' || value == 0)
	  {
	    send_to_char ("Syntax: medit create [vnum]\n\r", ch);
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
	if (get_mob_index (value) != NULL)
	  {
	    send_to_char ("Mob vnum already exists.\n\r", ch);
	    return;
	  }
	pMob = new_mob_index ();
	pMob->vnum = value;
	pMob->area = pArea;
	if (value > top_vnum_mob)
	  top_vnum_mob = value;
	iHash = value % HASH_MAX;
	pMob->next = mob_index_hash[iHash];
	mob_index_hash[iHash] = pMob;
	ch->desc->pEdit = (void *) pMob;
	SET_BIT (pArea->area_flags, AREA_CHANGED);
	send_to_char ("Mobile Created.\n\r", ch);
	pArea->mob_count++;
	return;
      }
    
    if (!str_cmp (arg1, "casts") && LEVEL (ch) > 109)
      {
	int i = 0;
	int j;
	if (arg2[0] == '\0')
	  return;
	if (!pMob->opt)
	  pMob->opt = new_optional ();
	for (j = 0; j < 10; j++)
	  {
	    if (pMob->opt->cast_spells[j] != NULL && !str_cmp (arg2, pMob->opt->cast_spells[j]))
	      {
		free_string (pMob->opt->cast_spells[j]);
		pMob->opt->cast_spells[j] = NULL;
		send_to_char ("Spell removed.\n\r", ch);
		return;
	      }
	  }
	while (pMob->opt->cast_spells[i] != NULL)
	  i++;
	if (i > 9)
	  {
	    send_to_char ("All full!!\n\r", ch);
	    return;
	  }
	pMob->opt->cast_spells[i] = str_dup (arg2);
	send_to_char ("Spell set.\n\r", ch);
	SET_BIT (pArea->area_flags, AREA_CHANGED);
	return;
      }
    if (!str_prefix ("cast_p", arg1) && LEVEL (ch) > 109)
      {
	if (!pMob->opt)
	  pMob->opt = new_optional ();
	if (value >= 0)
	  pMob->opt->cast_percent = value;
	send_to_char ("Cast Percentage set.\n\r", ch);
	SET_BIT (pArea->area_flags, AREA_CHANGED);
	return;
      }
    
    if (!str_cmp (arg1, "clan_guard"))
      {
	if (value > -1)
	  {
	    pMob->clan_guard_1 = value;
	    send_to_char ("Set clanguard.\n\r", ch);
	  }
	SET_BIT (pArea->area_flags, AREA_CHANGED);
	return;
      }
    if (!str_cmp (arg1, "sect_guard"))
      {
	if (value > -1)
	  {
	    pMob->clan_guard_2 = value;
	    send_to_char ("Set clan2guard.\n\r", ch);
	  }
	SET_BIT (pArea->area_flags, AREA_CHANGED);
	return;
      }
    
    break;
  case 'D':
    if (!str_cmp (arg1, "done"))
      {
	char minibuf[200];
	ch->desc->pEdit = NULL;
	sprintf (minibuf, "\x1B[1;%dr\x1B[2JDone.\n\r", ch->pcdata->pagelen);
	send_to_char (minibuf, ch);
	ch->desc->connected = CON_PLAYING;
	return;
      }
    
    if (!str_cmp (arg1, "damroll"))
      {
	bool neg = FALSE;
	char *a2 = arg2;
	int ii;
	if (arg2[0] == '\0')
	  {
	    send_to_char ("Damroll value cleared.\n\r", ch);
	    pMob->damroll = 0;
	  }
	if (arg2[0] == '-')
	  {
	    neg = TRUE;
	    a2++;
	  }
	ii = atoi (a2);
	if (neg)
	  ii = 0 - ii;
	value = ii;
	if (value != 0)
	  {
	    send_to_char ("Damroll value set.\n\r", ch);
	    pMob->damroll = value;
	  }
	else
	  {
	    send_to_char ("Damroll value cleared.\n\r", ch);
	    pMob->damroll = 0;
	  }
	SET_BIT (pArea->area_flags, AREA_CHANGED);
	return;
      }
    if (!str_cmp (arg1, "desc") || !str_cmp (arg1, "description"))
      {
	char *s;
	if (pMob->description != NULL && pMob->description[0] == 1)
	  {
	    s = str_dup (decompress (pMob->description));
	    free_string (pMob->description);
	    pMob->description = s;
	  }
	if (arg2[0] == '\0')
	  {
	    string_append (ch, &pMob->description);
	    SET_BIT (pArea->area_flags, AREA_CHANGED);
	    return;
	  }
	if (arg2[0] == '+')
	  {
	    string_append (ch, &pMob->description);
	    SET_BIT (pArea->area_flags, AREA_CHANGED);
	    return;
	  }
	send_to_char ("Syntax: desc - line edit\n\r", ch);
	send_to_char (" desc + - line append\n\r", ch);
	return;
      }
  
    if (!str_cmp (arg1, "dodge"))
      {
	if (value > 0)
	  {
	    send_to_char ("Dodge value set.\n\r", ch);
	    pMob->dodge = value;
	  }
	else
	  {
	    send_to_char ("Dodge value cleared.\n\r", ch);
	    pMob->dodge = 0;
	  }
	SET_BIT (pArea->area_flags, AREA_CHANGED);
      return;
      }
    
    break;
  case 'E':
    if (!str_cmp (arg1, "exp")) 
      {				
	if (arg2[0] == '\0' || !is_number (arg2))
	  {
	    send_to_char ("Syntax: exp [multiplier * 10]\n\r", ch);
	    send_to_char ("...so 1.5 times exp would be set by 'exp 15'.\n\r", ch);
	    return;
	  }
	if (atoi (arg2) < 2)
	  {
	    send_to_char ("Multiplier can't be less than two.\n\r", ch);
	    return;
	  }
	pMob->hpmult=pMob->hpmult%1000;
	if (atoi(arg2)!=10) {
	  pMob->hpmult+=(atoi(arg2))*1000;
	}
	mindex_window (ch, pMob, 1);
	send_to_char ("Exp multiplier set.\n\r", ch);
	return;
      }
    
   
    break;
  case 'F':
    if (!str_cmp(arg1, "female"))
      {
	pMob->sex = SEX_FEMALE;
	SET_BIT (pArea->area_flags, AREA_CHANGED);
	mindex_window (ch, pMob, 1);
	send_to_char ("Sex set.\n\r", ch);
	return;
      }
    if (!str_cmp (arg1, "fdesc") || !str_cmp (arg1, "fdescription"))
      {
	char *s;
	if (pMob->description != NULL && pMob->description[0] == 1)
	  {
	    s = str_dup (decompress (pMob->description));
	    free_string (pMob->description);
	    pMob->description = s;
	  }
	fullscreen_editor (ch, &pMob->description);
	SET_BIT (pArea->area_flags, AREA_CHANGED);
	return;
      }
    
    break;
  case 'G':
    if (!str_cmp (arg1, "guarding"))
      {
	if (arg2[0] == '\0')
	  {
	    send_to_char ("Syntax: guarding [vnum/dir]\n\r", ch);
	    return;
	  }
	if (!IS_SET (pMob->act3, ACT3_GUARD_ITEM)
	    && !IS_SET (pMob->act3, ACT3_GUARD_DOOR)
	    && !IS_SET (pMob->act3, ACT3_GUARD_MOB)
	    && pMob->clan_guard_1 == 0 && pMob->clan_guard_2 == 0
	    && !IS_SET (pMob->act3, ACT3_GUARD_DOOR_ALIGN))
	  {
	    send_to_char ("This mob is not set to guard.\n\r", ch);
	    return;
	  }
	if (!IS_SET (pMob->act3, ACT3_GUARD_ITEM) && !IS_SET (pMob->act3, ACT3_GUARD_MOB))
	  {
	    int door;
	    if (str_cmp (arg2, "none"))
	      door = get_direction_number (arg2);
	    else
	      door = -1;
	    if (door < -1 || door > 5)
	      {
		send_to_char ("Door number out of range.\n\r", ch);
		return;
	      }
	    else
	      pMob->guard = door;
	  }
      else
	pMob->guard = atoi (arg2);
      mindex_window (ch, pMob, 6);
      SET_BIT (pArea->area_flags, AREA_CHANGED);
      send_to_char ("Guard set.\n\r", ch);
      return;
    }
     
    break;
  case 'H':
    if (!str_cmp (arg1, "hp"))
      {
	int tmp=0;
	if (arg2[0] == '\0' || !is_number (arg2))
	  {
	    send_to_char ("Syntax: hp [multiplier * 10]\n\r", ch);
	    send_to_char ("...so 1.5 times hp would be set by 'hp 15'.\n\r", ch);
	    return;
	  }
	if (atoi (arg2) < 0)
	  {
	    send_to_char ("Multiplier can't be less than zero.\n\r", ch);
	    return;
	  }
	if (pMob->hpmult>1000 && (atoi(arg2))<1000) {
	  tmp=pMob->hpmult/1000;
	}
	pMob->hpmult = atoi (arg2);
	if (tmp!=0) {
	  pMob->hpmult+=(tmp*1000);
	}
	mindex_window (ch, pMob, 1);
	send_to_char ("Hp multiplier set.\n\r", ch);
	return;
      }
    
    if (!str_cmp (arg1, "height"))
      {
	if (value == -1 || value == 0)
	  {
	    pMob->height = -1;
	    send_to_char ("Height set to default for mobtype.\n\r", ch);
	    SET_BIT (pArea->area_flags, AREA_CHANGED);
	    mindex_window (ch, pMob, 4);
	    return;
	  }
	pMob->height = value;
	send_to_char ("Height set.\n\r", ch);
	mindex_window (ch, pMob, 4);
	SET_BIT (pArea->area_flags, AREA_CHANGED);
	return;
      }
    if (!str_cmp (arg1, "hitroll"))
      {
	if (value > 0)
	  {
	    send_to_char ("Hitroll value set.\n\r", ch);
	    pMob->hitroll = value;
	  }
	else
	  {
	    send_to_char ("Hitroll value cleared.\n\r", ch);
	    pMob->hitroll = 0;
	  }
      SET_BIT (pArea->area_flags, AREA_CHANGED);
      return;
      }
    
    break;

  case 'L':
    if (!str_cmp (arg1, "long"))
      {
	/*
	  int value;
	  */
	if (arg2[0] == '\0')
	  {
	    send_to_char ("Syntax: long [string]\n\r", ch);
	    return;
	  }
	free_string (pMob->long_descr);
	strcat (arg2, "\n\r");
	pMob->long_descr = str_dup (check_mob_long(arg2,ch));
	mindex_window (ch, pMob, 7);
	/*
	  if ( (value=mob_name_type( pMob->long_descr )) >= 0 )
	  pMob->mobtype = value;
	  */
	SET_BIT (pArea->area_flags, AREA_CHANGED);
	send_to_char ("Long description set.\n\r", ch);
	return;
      }
    if (!str_cmp (arg1, "level"))
      {
	if (arg2[0] == '\0' || !is_number (arg2))
	  {
	    send_to_char ("Syntax: level [number]\n\r", ch);
	    return;
	  }
	pMob->level = atoi (arg2);
	mindex_window (ch, pMob, 1);
	SET_BIT (pArea->area_flags, AREA_CHANGED);
	send_to_char ("Level set.\n\r", ch);
	return;
      }
 

 
    break;
  case 'M':
    if (!str_cmp (arg1, "money"))
      {
	if (arg2[0] == '\0' || !is_number (arg2))
	  {
	    send_to_char ("Syntax: money [amount in copper]\n\r", ch);
	    return;
	  }
	pMob->money = atoi (arg2);
	mindex_window (ch, pMob, 1);
	SET_BIT (pArea->area_flags, AREA_CHANGED);
	send_to_char ("Monetary amount set.\n\r", ch);
	return;
      }
    
    if ((!str_cmp (arg1, "miw") || !str_cmp (arg1, "max")) && value > 0)
      {
	pMob->max_in_world = value;
	send_to_char ("Max in world set.\n\r", ch);
	SET_BIT (pArea->area_flags, AREA_CHANGED);
	mindex_window (ch, pMob, 4);
	return;
      }
    if (!str_cmp(arg1, "male"))
      {
	pMob->sex = SEX_MALE;
	SET_BIT (pArea->area_flags, AREA_CHANGED);
	mindex_window (ch, pMob, 1);
	send_to_char ("Sex set.\n\r", ch);
	return;
      }
    break;
  case 'N':
    if (!str_cmp (arg1, "name"))
      {
	bool ffund;
	/*
	  int k;
	  int value;
	  */
	if (arg2[0] == '\0')
	  {
	    send_to_char ("Syntax: name [string]\n\r", ch);
	    return;
	  }
	ffund = FALSE;
	/*         for (k=0;k<MAX_PLAYERS;k++) 
		   {
		   if (!infix(playerbase.player_name[k],argy)) 
		   {
		   ffund=TRUE;
		   break;
		   }
		   }
		   if (ffund) 
		   {
		   send_to_char("Someone has a name with one of those words in it.\n\r",ch);
		   return;
		   }*/
	free_string (pMob->name);
	pMob->name = str_dup (check_mob_name(arg2,ch));
	mindex_window (ch, pMob, 2);
	/*
	  if ( (value=mob_name_type( pMob->name )) >= 0 )
	  pMob->mobtype = value;
	  */
	SET_BIT (pArea->area_flags, AREA_CHANGED);
	send_to_char ("Name set.\n\r", ch);
	return;
      }
    if (!str_cmp(arg1, "neutral") || !str_cmp(arg1, "neuter"))
      {
	pMob->sex = SEX_NEUTRAL;
	SET_BIT (pArea->area_flags, AREA_CHANGED);
	mindex_window (ch, pMob, 1);
	send_to_char ("Sex set.\n\r", ch);
	return;
      } 
    if (!str_cmp (arg, "none"))
      {
	pMob->spec_fun = NULL;
	free_string (pMob->spec_name);
	pMob->spec_name = str_dup ("");
	mindex_window (ch, pMob, 3);
	SET_BIT (pArea->area_flags, AREA_CHANGED);
	send_to_char ("Spec removed.\n\r", ch);
	return;
      }

    break;
  case 'P':
    if (!str_cmp (arg1, "parry"))
      {
	if (value > 0)
	  {
	    send_to_char ("Parry value set.\n\r", ch);
	    pMob->parry = value;
	  }
	else
	  {
	    send_to_char ("Parry value cleared.\n\r", ch);
	    pMob->parry = 0;
	  }
	SET_BIT (pArea->area_flags, AREA_CHANGED);
	return;
      }
     
    if (!str_cmp (arg1, "points"))
      {
	if (value > 0)
	  {
	    send_to_char ("Points value set.\n\r", ch);
	    pMob->points = value;
	  }
	else
	  {
	    send_to_char ("Points value cleared.\n\r", ch);
	    pMob->points = 0;
	  }
	SET_BIT (pArea->area_flags, AREA_CHANGED);
	return;
      }
    
    break;
  case 'S':
    
    if (!str_cmp (arg1, "short"))
      {
	/*
	  int value;
	  */
	if (arg2[0] == '\0')
	  {
	    send_to_char ("Syntax: short [string]\n\r", ch);
	    return;
	  }
	free_string (pMob->short_descr);
	pMob->short_descr = str_dup (check_mob_short(arg2,ch));
	mindex_window (ch, pMob, 3);
	/*
	  if ( (value=mob_name_type( pMob->short_descr )) >= 0 )
	  pMob->mobtype = value;
	  */
	SET_BIT (pArea->area_flags, AREA_CHANGED);
	send_to_char ("Short description set.\n\r", ch);
	return;
      }
    if (!str_cmp (arg1, "summons"))
      {
	if (LEVEL(ch) < MAX_LEVEL)
	  return;
	if (arg2[0] == '\0')
	  {
	    send_to_char ("Syntax: summons <mob vnum>", ch);
	    return;
	  }
	if (!IS_SET(pMob->act4, ACT4_SUMMONER))
	  {
	    send_to_char ("This mob is not set as a summoner, try 4summoner first.\n\r", ch);
	    return;
	  }
	if (!pMob->opt)
	  pMob->opt = new_optional();
	pMob->opt->summons = atoi (arg2);
	mindex_window (ch, pMob, 6);
	SET_BIT (pArea->area_flags, AREA_CHANGED);
	send_to_char ("Summons set.\n\r", ch);
	return;
      }
    
    if (!str_cmp (arg1, "shop"))
      {
	argy = one_argy (argy, arg1);
	strcpy (arg2, argy);
	if (arg1[0] == '\0')
	  {
	    send_to_char ("Syntax: shop hours [#opening] [#closing]\n\r", ch);
	    send_to_char (" shop profit [#buying%] [#selling%]\n\r", ch);
	    send_to_char (" shop decrease [Decrease %]\n\r", ch);
	    send_to_char (" shop items [# until decrease %]\n\r", ch);
	    send_to_char (" shop type [#1-5] [item type]\n\r", ch);
	    send_to_char (" shop creates [#vnum#]\n\r", ch);
	    send_to_char (" shop creates_hours [#hours#]\n\r", ch);
	    send_to_char (" shop message <create_message>\n\r", ch);
	    send_to_char (" shop delete\n\r", ch);
	    return;
	  }
	if (!str_cmp (arg1, "hours"))
	  {
	    argy = one_argy (argy, arg1);
	    strcpy (arg2, argy);
	    if (arg1[0] == '\0' || !is_number (arg1)
		|| arg2[0] == '\0' || !is_number (arg2))
	      {
		send_to_char ("Syntax: shop hours [#opening] [#closing]\n\r", ch);
		return;
	      }
	    if (pMob->pShop == NULL)
	      {
		new_shop (pMob);
		/*pMob->pShop->keeper = pMob->vnum;
		  if (shop_last) shop_last->next = pMob->pShop; else
		  {
		  shop_first=pMob->pShop;
		  shop_last=pMob->pShop;
		  }*/
	      }
	    pMob->pShop->open_hour = atoi (arg1);
	    pMob->pShop->close_hour = atoi (arg2);
	    SET_BIT (pArea->area_flags, AREA_CHANGED);
	    send_to_char ("Shop hours set.\n\r", ch);
	    return;
	  }
	if (!str_cmp (arg1, "decrease"))
	  {
	    int joeman;
	    argy = one_argy (argy, arg1);
	    if (arg1[0] == '\0' || !is_number (arg1))
	      {
		send_to_char ("Syntax: shop decrease [#decrease percent#]\n\r", ch);
		return;
	      }
	    joeman = atoi (arg1);
	    if (joeman < 1 || joeman > 100)
	      {
		send_to_char ("Must be between 1 and 100!\n\r", ch);
		return;
	      }
	    if (pMob->pShop == NULL)
	      {
		new_shop (pMob);
	      }
	    pMob->pShop->decrease = joeman;
	    SET_BIT (pArea->area_flags, AREA_CHANGED);
	    send_to_char ("Shop decrease percent set.\n\r", ch);
	  }
	if (!str_cmp (arg1, "creates"))
	  {
	    int joeman;
	    argy = one_argy (argy, arg1);
	    if (arg1[0] == '\0' || !is_number (arg1))
	      {
		send_to_char ("Syntax: shop creates [#vnum#]\n\r", ch);
		return;
	      }
	    joeman = atoi (arg1);
	    if (joeman != 0 && (joeman < 1 || (get_obj_index (joeman) == NULL)))
	      {
		send_to_char ("Invalid object!\n\r", ch);
		return;
	      }
	    if (pMob->pShop == NULL)
	      {
		new_shop (pMob);
	      }
	    pMob->pShop->creates_vnum = joeman;
	    SET_BIT (pArea->area_flags, AREA_CHANGED);
	    send_to_char ("Shop creates vnum set.\n\r", ch);
	    return;
	  }
	if (!str_cmp (arg1, "createhours") ||
	    !str_cmp (arg1, "create_hour") ||
	    !str_cmp (arg1, "create_hours") ||
	    !str_cmp (arg1, "creates_hours"))
	  {
	    int joeman;
	    argy = one_argy (argy, arg1);
	    if (arg1[0] == '\0' || !is_number (arg1))
	      {
		send_to_char ("Syntax: shop creates_hours [#game_ticks#]\n\r", ch);
		return;
	      }
	    joeman = atoi (arg1);
	    if (joeman < 1 || joeman > 30000)
	      {
		send_to_char ("Invalid creates_hours!\n\r", ch);
		return;
	      }
	    if (pMob->pShop == NULL)
	      {
		new_shop (pMob);
	      }
	    pMob->pShop->creates_hours = joeman;
	    SET_BIT (pArea->area_flags, AREA_CHANGED);
	    send_to_char ("Shop creates_hours set.\n\r", ch);
	    return;
	  }
	if (!str_cmp (arg1, "item") || !str_cmp (arg1, "items"))
	  {
	    int joeman;
	    argy = one_argy (argy, arg1);
	    if (arg1[0] == '\0' || !is_number (arg1))
	      {
		send_to_char ("Syntax: shop items [#items before decrease#]\n\r", ch);
		return;
	      }
	    joeman = atoi (arg1);
	    if (joeman < 1 || joeman > 100)
	      {
		send_to_char ("Must be between 1 and 100!\n\r", ch);
		return;
	      }
	    if (pMob->pShop == NULL)
	      {
		new_shop (pMob);
	    }
	    pMob->pShop->items_until_decrease = joeman;
	    SET_BIT (pArea->area_flags, AREA_CHANGED);
	    send_to_char ("Shop items until decrease set.\n\r", ch);
	  }
	if (!str_cmp (arg1, "profit"))
	  {
	    argy = one_argy (argy, arg1);
	    strcpy (arg2, argy);
	    if (arg1[0] == '\0' || !is_number (arg1)
		|| arg2[0] == '\0' || !is_number (arg2))
	      {
		send_to_char ("Syntax: shop profit [#buying%] [#selling%]\n\r", ch);
		return;
	      }
	    if (pMob->pShop == NULL)
	      {
		new_shop (pMob);
	      }
	    pMob->pShop->profit_buy = atoi (arg1);
	    pMob->pShop->profit_sell = atoi (arg2);
	    SET_BIT (pArea->area_flags, AREA_CHANGED);
	    send_to_char ("Shop profit set.\n\r", ch);
	    return;
	  }
	if (!str_cmp (arg1, "message"))
	  {
	    if (arg2[0] == '\0')
	      {
		send_to_char ("Syntax: shop message [string] or shop message clear.\n\r", ch);
		return;
	      }
	    if (pMob->pShop == NULL)
	    {
	      new_shop (pMob);
	    }
	    if (pMob->pShop->creates_message != NULL)
	      free_string (pMob->pShop->creates_message);
	    if (!str_cmp (arg2, "clear"))
	      {
		pMob->pShop->creates_message = NULL;
		send_to_char ("String cleared.\n\r", ch);
		return;
	      }
	    pMob->pShop->creates_message = str_dup (arg2);
	    SET_BIT (pArea->area_flags, AREA_CHANGED);
	    send_to_char ("Shop create message set.\n\r", ch);
	    return;
	  }
	if (!str_cmp (arg1, "type"))
	  {
	    argy = one_argy (argy, arg1);
	    strcpy (arg2, argy);
	    if (arg1[0] == '\0' || !is_number (arg1)
		|| arg2[0] == '\0')
	      {
		send_to_char ("Syntax: shop type [#] [item type]\n\r", ch);
		return;
	      }
	    if (atoi (arg1) > MAX_TRADE)
	      {
		sprintf (buf, "%d", MAX_TRADE);
		send_to_char ("REdit: Shop keepers may only sell ", ch);
		send_to_char (buf, ch);
		send_to_char (" items max.\n\r", ch);
		return;
	      }
	    if ((value = item_name_type (arg2)) == ITEM_NONE)
	      {
		send_to_char ("REdit: That type of item is not known.\n\r", ch);
		return;
	      }
	    if (pMob->pShop == NULL)
	      {
		new_shop (pMob);
	      }
	    pMob->pShop->buy_type[atoi (arg1)] = item_name_type (arg2);
	    SET_BIT (pArea->area_flags, AREA_CHANGED);
	    send_to_char ("Shop type set.\n\r", ch);
	    return;
	  }
	if (!str_cmp (arg1, "delete"))
	  {
	    if (pMob->pShop == NULL)
	      {
		send_to_char ("REdit: Can't delete a shop that is non-existant.\n\r", ch);
		return;
	      }
	    free_shop (pMob->pShop);
	    pMob->pShop = NULL;
	    SET_BIT (pArea->area_flags, AREA_CHANGED);
	    send_to_char ("Shop deleted.\n\r", ch);
	    return;
	  }
	send_to_char ("Syntax: shop hours [#opening] [#closing]\n\r", ch);
	send_to_char (" shop profit [#buying%] [#selling%]\n\r", ch);
	send_to_char (" shop decrease [Decrease %]\n\r", ch);
	send_to_char (" shop items [# until decrease %]\n\r", ch);
	send_to_char (" shop type [#] [item type]\n\r", ch);
	send_to_char (" shop creates_hours [#hours#]\n\r", ch);
	send_to_char (" shop creates [#vnum#]\n\r", ch);
	send_to_char (" shop message <create_message>\n\r", ch);
	send_to_char (" shop delete\n\r", ch);
	return;
      }
    
    break;
  case 'T':
    if (!str_cmp (arg1, "teach"))
      {
	if (LEVEL (ch) < MAX_LEVEL)
	  {
	    send_to_char ("If you need to use a mob that uses this, get a god to set it.\n\r", ch);
	    return;
	  }
	if (value > 0 && value <= SKILL_COUNT)
	  {
	    nextfree = 0;
	    if (!pMob->opt)
	      pMob->opt = new_optional ();
	    while (nextfree <= 28)
	      {
		if (pMob->opt->skltaught[nextfree] == value)
		  {
		    int tmp;
		    tmp = 0;
		    while (tmp <= 27)
		      {
			if (tmp >= nextfree)
			  {
			    pMob->opt->skltaught[tmp] = pMob->opt->skltaught[tmp + 1];
			  }
			tmp++;
		      }
		    pMob->opt->skltaught[28] = 0;
		    send_to_char ("Skill Removed.\n\r", ch);
		    nextfree = 29;
		    return;
		    /*REMOVE SKILL ABOVE */
		  }
		nextfree++;
	      }
	    nextfree = 0;
	    while (nextfree <= 28)
	      {
		if (pMob->opt->skltaught[nextfree] == 0)
		  {
		    pMob->opt->skltaught[nextfree] = value;
		    send_to_char ("Skill Set.\n\r", ch);
		    nextfree = 29;
		    return;
		  }
		nextfree++;
	      }
	    if (nextfree == 30)
	      send_to_char ("Out of slots.\n\r", ch);
	  }
	SET_BIT (pArea->area_flags, AREA_CHANGED);
	return;
      }
    if (!str_cmp (arg1, "timer"))
      {
	if (arg2[0] == '\0' || !is_number (arg2))
	  {
	    send_to_char ("Syntax: timer [ticks]\n\r", ch);
	    return;
	  }
	pMob->timer = atoi (arg2);
	SET_BIT (pArea->area_flags, AREA_CHANGED);
	send_to_char ("Timer set.\n\r", ch);
	return;
      }

    break;
  case 'W':
    if (!str_cmp (arg1, "willhelp"))
      {
	char bufff[500];
	if (value < 0 || value >= 127)
	  {
	    send_to_char ("Syntax is help <number>, where number is a 1 in <number> chance\n\rthe mob will come assist the yeller.\n\r", ch);
	    return;
	  }
	pMob->will_help = value;
	mindex_window (ch, pMob, 6);
	if (pMob->will_help == 0)
	  {
	    send_to_char ("Mob will no longer hunt out yellers with a similar yeller_num.\n\r", ch);
	    pMob->will_help = 0;
	    return;
	  }
	sprintf (bufff, "Mob will now help out a yeller at 1 in %d chance.\n\r", pMob->will_help);
	send_to_char (bufff, ch);
	SET_BIT (pArea->area_flags, AREA_CHANGED);
	return;
      }
    if (!str_prefix ("warrior_p", arg1) && LEVEL (ch) > 109)
      {
	if (!pMob->opt)
	  pMob->opt = new_optional ();
	if (value >= 0)
	  pMob->opt->warrior_percent = value;
	send_to_char ("Warrior Percentage set.\n\r", ch);
	SET_BIT (pArea->area_flags, AREA_CHANGED);
	return;
      }
    
    break;
  case 'Y':
    if (!str_cmp (arg1, "yeller"))
      {
	if (value >= 0)
	  {
	    pMob->yeller_number = value;
	    mindex_window (ch, pMob, 6);
	    if (value == 0)
	      send_to_char ("Mob is no longer a yeller.\n\r", ch);
	    else
	      send_to_char ("Yeller number set.\n\r", ch);
	  }
	else
	  send_to_char ("Syntax is yeller <yell_number>.\n\r", ch);
	return;
      }
    break;
  }


if ((value = affect_name_bit (arg1)) != AFFECT_NONE)
  {
    pMob->affected_by ^= value;
    SET_BIT (pArea->area_flags, AREA_CHANGED);
    mindex_window (ch, pMob, 5);
    send_to_char ("Affect flag toggled.\n\r", ch);
    return;
  }
if ((value = affect_name_bit_two (arg1)) != AFFECT_NONE)
  {
    pMob->more_affected_by ^= value;
    mindex_window (ch, pMob, 5);
    SET_BIT (pArea->area_flags, AREA_CHANGED);
    send_to_char ("Affect flag toggled.\n\r", ch);
    return;
  }

if ((value = mob_name_type (arg1)) >= 0)
  {
    pMob->mobtype = value;
    mindex_window (ch, pMob, 6);
    SET_BIT (pArea->area_flags, AREA_CHANGED);
    send_to_char ("Mob type set to \x1B[1;31m", ch);
    send_to_char (mob_type_name (value), ch);
    send_to_char ("\x1B[0m.\n\r", ch);
    return;
  }
if ((value = race_name_bit (arg1)) != 0)
  {
    pMob->race_hate ^= value;
    mindex_window (ch, pMob, 4);
    SET_BIT (pArea->area_flags, AREA_CHANGED);
    send_to_char ("Race_hate flag toggled.\n\r", ch);
    return;
  }
if ((value = act_name_bit (arg1)) != ACT_NONE)
  {
    if (arg1[0] == '3')
      pMob->act3 ^= value;
    else if (arg1[0] == '4')
      pMob->act4 ^= value;
    else
      pMob->act ^= value;
    mindex_window (ch, pMob, 4);
    SET_BIT (pArea->area_flags, AREA_CHANGED);
    send_to_char ("Act flag toggled.\n\r", ch);
    return;
  }

if (get_mob_spec (arg) != NULL)
  {
    pMob->spec_fun = get_mob_spec (arg);
    if (pMob->spec_name)
      free_string (pMob->spec_name);
    sprintf (buf, "spec_%s", arg);
    pMob->spec_name = str_dup (buf);
    mindex_window (ch, pMob, 3);
    SET_BIT (pArea->area_flags, AREA_CHANGED);
    send_to_char ("Spec set.\n\r", ch);
    return;
  }
   }


if (!str_suffix ("edit", arg))
    {
      char minibuf[200];
      sprintf (minibuf, "\x1B[1;%dr\x1B[2JDone.\n\r", ch->pcdata->pagelen);
      send_to_char (minibuf, ch);
    }
if (hugebuf_o[0]!='\0') {
	page_to_char(hugebuf_o,ch);
	return;
	}
  interpret (ch, arg);
  return;
}
