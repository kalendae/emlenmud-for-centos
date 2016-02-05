#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emlen.h"

extern bool noibm;
extern bool kk;
extern bool mb;
extern bool dbs;
char *const dirrev[] =
{
  "south", "west", "north", "east", "down", "up"
};

const int blood_number[] =
{
1, 2, 4, 8, 16, 32
};

const short revdir[] =
{
  2, 3, 0, 1, 5, 4
};




char *const dir_name[] =
{
  "north", "east", "south", "west", "up", "down"
};

char *const dir_rev[] =
{
  "the south", "the west", "the north", "the east", "below", "above"
};

const short rev_dir[] =
{
  2, 3, 0, 1, 5, 4
};

const struct sector_data sectors[SECT_MAX] =
{
{  1, "Inside", "inside", "Always light unless dark is set.","\x1b[0;34m",1},
{  2, "City", "city", "dark at night, minimal mp loss.", "\x1b[0;37m",7}, 	
{  2,  "Town", "town", "Small inhabited area.", "\x1b[0;37m",7},
{  1,  "Road", "road", "Easy movement between cities", "\x1b[0;36m",3},
{  2,  "Trail", "trail", "Fairly easy movement", "\x1b[0;36m",3},
{  3, "Path", "path", "A way for easier travel in wilderness","\x1b[0;36m",3},
{  4, "Bridge", "bridge", "Some sort of a bridge.", "\x1b[0;36m",3},
{  3, "Field", "field", "can see in the dark on moon-lit nights", "\x1b[1;32m",10},
{  4, "Grasslands", "grasslands", "An open space teeming with life","\x1b[1;32m",10},
{  4, "Savannah", "savannah", "Lush grasslands near rivers","\x1b[1;32m",10},
{ 15,  "Water", "swim", "Swimmable water sector", "\x1b[1;34m",9},
{ 12, "Deep Water", "noswim", "Unswimmable water sector", "\x1b[0;34m",1},
{ 14, "Underwater", "underwater", "requires vehicle underwater, or breath", "\x1b[0;34m",1},
{  9,  "Mountain", "mountain", "extremely hard travelling", "\x1b[1;35m",5},
{  4, "Air", "air", "fly spell required", "\x1b[1;36m",11},
{ 10, "Clouds", "clouds", "Those big puffy things in the sky.","\x1b[1;36m",11},
{  9, "Snow", "snow", "High mp loss without vehicle", "\x1b[1;37m",15},
{ 10, "Ice Field", "ice", "Hard to walk on, slippery", "\x1b[1;37m",15},
{  8, "Arctic", "arctic", "Very very cold area.", "\x1b[1;37m",15},
{ 13, "Tundra", "tundra", "Cold, barren land with few animals","\x1b[1;37m",15},
{  6, "Tropical", "tropical", "Hot and dense", "\x1b[0;32m",2},
{  7, "Marsh", "marsh", "A cooler swamp - no tracks", "\x1b[1;32m",10},
{  7,  "Swamp", "swamp", "Hot, sulfurous area.", "\x1b[1;32m",10},
{  3,"Hollow", "hollow", "A dank, cold swampy area","\x1b[0;34m",1},
{  5, "Steppe", "steppe", "An arid, cool grassland", "\x1b[1;33m",14},
{  3, "Beach", "beach", "Sandy spots near the ocean.", "\x1b[1;33m",14},
{  3,  "Brush", "brush", "Undergrowth...hard to travel through.", "\x1b[0;33m",6},
{  4,  "Forest", "forest", "Is dark if sky is cloudy", "\x1b[0;32m",2},
{  4, "Woods", "woods", "Another type of forest.", "\x1b[0;32m",2},
{  5,  "Canyon", "canyon", "Cool, shaded area", "\x1b[0;31m",4},
{  6, "Hills", "hills", "hard travelling", "\x1b[0;35m",5},
{  6, "Rocky", "rocky", "hard travelling here", "\x1b[0;35m",5},
{  7, "Wasteland", "wasteland", "Dry, hot area of desolation", "\x1b[0;31m",4},
{  8, "Desert", "desert", "can't recall from this room", "\x1b[1;33m",14},
{ 6, "Ruins", "ruins", "This is an ancient, abandoned place.","\x1b[1;30m",8},
{  9,  "Lava", "lava", "blisteringly hot area!", "\x1b[1;31m",4},
{  20,"Astral", "astral", "Need special spell to stay alive.", "\x1b[1;31m",9},
{ 4, "Cave", "cave", "An underground room.","\x1b[0;34m",1},
{ 4, "Hole", "hole", "In or over a hole.","\x1b[1;30m",8},
{ 4, "Cavern", "Cavern", "A large underground area.","\x1b[0;34m",1},
{ 4, "Passage", "passage", "An underground path.","\x1b[1;30m",8},
{ 4, "Alcove", "alcove", "A small side cave.","\x1b[0;34m",1},
{ 4, "Chamber", "chamber", "A hollowed out underground space.","\x1b[0;34m",1},
{ 4, "Vault", "vault","A MASSIVE underground chamber.","\x1b[0;35m",5},
{ 4, "Subterranian", "Subterranian", "Some random underground area.","\x1b[1;30m",8},
{ 4, "Shaft", "shaft", "A naturally formed tunnel.","\x1b[1;30m",8},
{ 4, "Pit", "pit", "In or around a vertical hole.","\x1b[1;30m",8},
{ 4, "Tunnel", "tunnel", "An underground path.","\x1b[1;30m",8},
{ 4, "Tube", "tube", "A round tunnel formed by something...","\x1b[1;30m",8},
{  2, "Throne Room", "throneroom", "dark at night, minimal mp loss.", "\x1b[0;37m",7}, 
};

bool has_key (CHAR_DATA * ch, int key);
int find_door (CHAR_DATA * ch, char *arg);
char *door_cond (int val);

void setnext(CHAR_DATA *ch, char *c) {
if (ch->fgt->next_command) free(ch->fgt->next_command);
ch->fgt->next_command=mem_alloc(strlen(c)+1);
strcpy(ch->fgt->next_command,c);
return;
}



char *
get_direction (char *arg)
{
  switch (LOWER (arg[0]))
    {
    case 'n':
      return "north";
    case 's':
      return "south";
    case 'e':
      return "east";
    case 'w':
      return "west";
    case 'u':
      return "up";
    case 'd':
      return "down";
    }
  return "anywhere";
}

char *
ans_uppercase (const char *txt)
{
  static char buf[STD_LENGTH];
  char *str;
  char *point;
  buf[0] = '\0';
  if (txt == "")
    {
      return buf;
    }
  if (!txt || txt[0] == '\0')
    {
      return buf;
    }
  str = (char *) txt;
  point = buf;
  if (isalpha (txt[0]))
    {
      sprintf (buf, txt);
      buf[0] = UPPER (buf[0]);
      return buf;
    }
  if (strlen (txt) > 5 && isalpha (txt[1]))
    {
      sprintf (buf, txt);
      buf[1] = UPPER (buf[1]);
      return buf;
    }
  if (strlen (txt) > 5 && isalpha (txt[2]) && txt[2] != 'm')
    {
      sprintf (buf, txt);
      buf[2] = UPPER (buf[2]);
      return buf;
    }
  if (strlen (txt) > 5 && isalpha (txt[3]) && txt[3] != 'm')
    {
      sprintf (buf, txt);
      buf[3] = UPPER (buf[3]);
      return buf;
    }
  sprintf (buf, txt);
  while (*point)
    {
      while (*point != 'm' && *point)
	point++;
      if (!*point)
	return buf;
      point++;
      if (isalpha (*point))
	{
	  *point = UPPER (*point);
	  return buf;
	}
    }
  return buf;
}


TRACK_DATA *
new_track (CHAR_DATA * ch, ROOM_DATA * room)
{
  TRACK_DATA *trk;
  trk = (TRACK_DATA *) mem_alloc ((sizeof (*trk)));
  trk->ch = ch;
  trk->dir_came = 10;
  trk->dir_left = 10;
  trk->next_track_in_room = room->tracks;
  room->tracks = trk;
  return trk;
}



/* JRAJRA - Add player tracks to a room */

PTRACK_DATA *
new_ptrack (CHAR_DATA * ch, ROOM_DATA * room)
{
  PTRACK_DATA *ptrk;
  ptrk = (PTRACK_DATA *) mem_alloc ((sizeof (*ptrk)));
  ptrk->online = ch->pcdata->online_spot;
  ptrk->dir_came = 10;
  ptrk->dir_left = 10;
  ptrk->next_track_in_room = room->ptracks;
  room->ptracks = ptrk;
  return ptrk;
}


void
do_rawclear (CHAR_DATA * ch)
{
  char ddd[500];
  if (IS_MOB (ch) || ch->desc == NULL)
    return;
  sprintf (ddd, "\x1B[2J\x1B[0m\x1B[1;%dr", ch->pcdata->pagelen);
  write_to_descriptor2 (ch->desc, ddd, 0);
  return;
}

void
do_capture (CHAR_DATA *ch, char *argy)
{
	DEFINE_COMMAND("capture", do_capture, POSITION_STANDING, 0, LOG_NORMAL,
		       "Captures a Battleground Throne Room. VT100/ANSI")
	if (IS_MOB(ch) || ch->desc == NULL)
		return;
	if (ch->in_room->sector_type == SECT_CAPTURE && ch->in_room->calign != ch->pcdata->alignment) {
		AREA_DATA *pArea = ch->in_room->area;
		ROOM_DATA *pRoom = ch->in_room;
		MOB_PROTOTYPE *pMob;
		int vnum = pRoom->cvnuml;
        	char buf[512];
		int prevalign = pRoom->calign;
		SET_BIT(pArea->area_flags, AREA_CHANGED);
		
		sprintf (buf, "\x1b[1;37m%s\x1b[0;37m has captured \x1b[0;37m'\x1b[1;37m%s\x1b[0;37m' for the forces of %s!", ch->pcdata->name, ch->in_room->name, align_info[ch->pcdata->alignment].name);
		do_echo (ch, buf);
		pRoom->calign = ch->pcdata->alignment;
		if (vnum < 1)
			vnum = 0;
		while (vnum < pRoom->cvnumh) {
			if ((pMob = get_mob_index(vnum)) != NULL) {
				pArea = pMob->area;
				SET_BIT(pArea->area_flags, AREA_CHANGED);
				pMob->alignment = ch->pcdata->alignment;
			}
			vnum ++;
		}
		 /* To drunk to do properly -- Flatline  */
		 if (pRoom->cbonusstr != NULL) {
//			char * tmp = pRoom->cbonusstr;
//			char buf[512];
		send_to_char(pRoom->cbonusstr, ch);
			if (!(strchr(pRoom->cbonusstr, 's') == NULL)) {
                            align_info[prevalign].bonus[0] = 0;
                            align_info[ch->pcdata->alignment].bonus[0] = 1;
			}
			if (!(strchr(pRoom->cbonusstr, 'i') == NULL)) {
                            align_info[prevalign].bonus[1] = 0;
                            align_info[ch->pcdata->alignment].bonus[1] = 1;
			}
			if (!(strchr(pRoom->cbonusstr, 'w') == NULL)) {
                            align_info[prevalign].bonus[2] = 0;
                            align_info[ch->pcdata->alignment].bonus[2] = 1;
			}
			if (!(strchr(pRoom->cbonusstr, 'd') == NULL)) {
                            align_info[prevalign].bonus[3] = 0;
                            align_info[ch->pcdata->alignment].bonus[3] = 1;
			}
/*			if (!(strchr(pRoom->cbonusstr, 'c') == NULL)) {
                            align_info[prevalign].bonus[4] = 0;
                            align_info[ch->pcdata->alignment].bonus[4] = 1;
			} */
		}
 /* */
		do_asave(char_list, "changed");
		fprintf(stderr, "Auto-saving changed areas due to capture.\n");
		return;
	} else if (ch->in_room->sector_type == SECT_CAPTURE) {
		send_to_char ("This area has already been captured.\r\n", ch);
		return;
	} else {
		const char *god;
		char buf[512];
		int n_god;
		n_god = number_range (0, 4);
		god = pow.god_names[n_god];
		sprintf (buf, "\x1b[1;37m%s whispers, \"Moron!  You can't capture outside of a throne room!\"\n\r\x1b[0;37m", god);
		send_to_char(buf, ch);
		return;
	}
}

void
do_clear (CHAR_DATA * ch, char *argy)
{
  char ddd[500];
  DEFINE_COMMAND ("cls", do_clear, POSITION_DEAD, 0, LOG_NORMAL, "Clears the screen; defaults window size to pagelen.  VT100/ANSI")

    if (IS_MOB (ch) || ch->desc == NULL)
    return;
  	sprintf (ddd, "\x1B[2J\x1B[0m\x1B[1;%dr", ch->pcdata->pagelen);
  	write_to_descriptor2 (ch->desc, ddd, 0);
  ch->pcdata->resist_summon = 0;
#ifdef OLD_NOMOVE
  scrn_upd (ch);
#endif
  return;
}

void
do_enter (CHAR_DATA * ch, char *argy)
{
  char arg[SML_LENGTH];
  SINGLE_OBJECT *obj;
  CHAR_DATA *fch;
  ROOM_DATA *dest;
  DEFINE_COMMAND ("enter", do_enter, POSITION_STANDING, 0, LOG_NORMAL,"Allows you to enter an enterable objject.");
  if (!ch || !ch->in_room) return;
  one_argy (argy, arg);
  if (ch == NULL || ch->in_room == NULL) return;
  if ((fch = FIGHTING(ch)) != NULL)
    {
      send_to_char("Not while fighting!\n\r", ch);
      return;
    }
  check_room_more(ch->in_room);
  for(fch = ch->in_room->more->people; fch != NULL;fch=fch->next_in_room)
    {
      if (ch == FIGHTING(fch))
	{
	  send_to_char("Not while fighting!\n\r", ch);
	  return;
	}
    }
  if (RIDING(ch) != NULL)
    {
      send_to_char("How do you expect to enter something when you are riding a mount.\n\r", ch);
      return;
    }
  if (MOUNTED_BY(ch) != NULL)
    {
      send_to_char("How do you expect to enter something when you are being ridden?\n\r", ch);
      return;
    }
  if ((obj = get_obj_here (ch, arg, SEARCH_ROOM_FIRST)) == NULL)
    {
      send_to_char ("There is nothing like that to enter.\n\r", ch);
      return;
    }
  if (!obj || obj->in_room == NULL || obj->carried_by != NULL)
    {
      send_to_char("That object is not in the room.\n\r", ch);
      return;
    }
  if ((obj->pIndexData->item_type != ITEM_FURNITURE))
    {
      send_to_char ("How do you expect to enter that?\n\r", ch);
      return;
    }
  else
    {
      I_FURNITURE *furn = (I_FURNITURE *) obj->more;
      if (!IS_SET (furn->type, FURN_EXIT))
	{
	  send_to_char ("How do you expect to enter that?\n\r", ch);
	  return;
	}
      if ((dest = get_room_index (furn->to_room_vnum)) == NULL)
	{
	  send_to_char ("It doesn't lead anywhere.\n\r", ch);
	  return;
	}
      act ("$n enters $p\e[0m.", ch, obj, NULL, TO_ROOM);
      act ("You enter $p\e[0m.", ch, obj, NULL, TO_CHAR);
      if (furn->strip_possessions == 1)
	{
	  send_to_char ("You feel a strange force ripping at your equipment...\n\r", ch);
	  strip_nonworn (ch);
	}
      char_from_room (ch);
      char_to_room (ch, dest);
      do_look (ch, "auto");
      act ("$n enters from $p\e[0m.", ch, obj, NULL, TO_ROOM);
    }
  return;
}


bool
find_vehicle (CHAR_DATA * ch, int sector_type)
{
  if (IS_MOB(ch)) return TRUE;
  if ((sector_type == SECT_WATER_SWIM ||
       sector_type == SECT_WATER_NOSWIM) &&
      !IS_SET(ch->act, PLR_HAS_BOAT))
    return FALSE;
  
  else if (sector_type == SECT_MOUNTAIN && 
      !IS_SET(ch->act, PLR_HAS_MTN_BOOTS))
    return FALSE;
  else if (sector_type == SECT_UNDERWATER && 
      !IS_SET(ch->act, PLR_HAS_SCUBA) && 
      !IS_AFFECTED(ch, AFF_BREATH_WATER))
    return FALSE;
  else if (sector_type == SECT_SNOW &&
      !IS_SET(ch->act, PLR_HAS_SNOWSHOES))
    return FALSE;
  return TRUE;
}



bool check_super = TRUE;

bool
move_char (CHAR_DATA * ch, int door)
{
  static int depth = 0;
  int i, n;
  char to_sector;
  CHAR_DATA *fch;
  CHAR_DATA *fch_next;
  ROOM_DATA *in_room;
  ROOM_DATA *curr_room;
  ROOM_DATA *to_room = NULL;
  ROOM_DATA *troom = NULL;
  CHAR_DATA *viewer;
  TRACK_DATA *trk;
  PTRACK_DATA *ptrk; /* NEW Ptrack data for rooms... */
  char buf2[STD_LENGTH];
  char buf[STD_LENGTH];
  EXIT_DATA *pexit;
  bool fndd = FALSE;
  bool flagone = FALSE;
  bool flagtwo = FALSE;
  int followers = 0;
  entlab = 0;
  if (ch->in_room == NULL)
    return FALSE;
  if (door > 18 && ch->in_room->vnum > 99 && ch->in_room->vnum < 109)
    return FALSE;
  if(ch->position < POSITION_SLEEPING)
    return FALSE;
  check_room_more (ch->in_room);

  if (door > 18)
    {
      door -= 20;
    }
  else
    {
      if (FIGHTING(ch))
	return FALSE;
      for (fch = ch->in_room->more->people; fch != NULL; fch = fch->next_in_room)
	{
	  if (FIGHTING (fch) == ch)
	    {
	      send_to_char ("You are in the middle of combat!\n\r", ch);
	      return FALSE;
	    }
	}
      
    }
  entlab  = 1;

  if (ch->desc)
    ch->desc->repeat = 0;
  if (FIGHTING (ch) != NULL && ch->position == POSITION_CASTING)
    return FALSE;
  if (door < 0 || door > 5)
    {
      bug ("Do_move: bad door %d.", door);
      return 0;
    }
  if (MOUNTED_BY (ch) != NULL && MOUNTED_BY (ch)->position != POSITION_STANDING)
    {
      if (LEADER (ch) != NULL && LEADER (ch) != ch)
	{
	  char buffy[100];
	  sprintf (buffy, "%s was not standing and couldn't follow!", NAME (ch));
	  group_notify (buffy, ch);
	}
      return FALSE;
    }
  if (RIDING (ch) != NULL && RIDING (ch)->position < POSITION_STANDING)
    {
      send_to_char ("Your ride doesn't want to move right now.\n\r", ch);
      return 0;
    }
  entlab = 2;
  if (ch->position < POSITION_STANDING)
    {
      if (LEADER (ch) != NULL && LEADER(ch) != ch && ch->position < POSITION_MEDITATING)
	{
	  char buffy[100];
	  sprintf (buffy, "%s couldn't follow!", NAME (ch));
	  group_notify (buffy, ch);
	}
      if (ch->position == POSITION_MEDITATING)
	{
	  act ("You must stop meditating before you try to move!", ch, NULL, NULL, TO_CHAR);
	  return FALSE;
	}
      if (ch->position == POSITION_FALLING && door==DIR_UP) 
	{
	  act("You are heading in quite the opposite direction!",ch,NULL,ch,TO_CHAR);
	  return FALSE;
	}
      if (ch->position == POSITION_FALLING && door==DIR_DOWN) 
	{
	  act("You are heading in that direction already!",ch,NULL,ch,TO_CHAR);
	  return FALSE;
	}
      if (ch->position == POSITION_BASHED)
	{
	  act ("You have just been bashed to the ground! How can you move?!", ch, NULL, NULL, TO_CHAR);
	  return FALSE;
	}
      if (ch->position == POSITION_GROUNDFIGHTING)
	{
	  act ("You are on the ground, fighting! How can you move?!", ch, NULL, NULL, TO_CHAR);
	  return FALSE;
	}
    }
  in_room = ch->in_room;
  if (IS_PLAYER (ch) && (ch->pcdata->condition[COND_DRUNK] > 15 || IS_AFFECTED(ch, AFF_CONFUSE)) && number_range(1,7) == 2)
    door = number_range (0, 5);
  if ((pexit = in_room->exit[door]) == NULL)
    {
      send_to_char("You can't go that way!.\n\r", ch);
      return 0;
    }
  if ((to_room = pexit->to_room) == NULL)
    {
      if (pexit->d_info && 
	  pexit->d_info->description[0] != ' ' &&
          pexit->d_info->description != "" && 
	  pexit->d_info->description[0] != '\0')
	{
	  send_to_char (pexit->d_info->description, ch);
	  return 0;
	}
      send_to_char ("You can't go that way!\n\r", ch);
      return 0;
    }
  to_sector = to_room->sector_type;
  entlab = 3;
  if (pexit->d_info 
      && IS_SET (pexit->d_info->exit_info, EX_CLOSED)
      && !IS_SET(ch->act, PLR_HOLYWALK))
    {
      if (IS_SET (pexit->d_info->exit_info, EX_HIDDEN))
	{
	  send_to_char ("You can't go any further in that direction.\n\r", ch);
	  return 0;
	}
      else if (!IS_AFFECTED(ch, AFF_PASS_DOOR))
	{
	  act ("The $T is closed.", ch, NULL, pexit->d_info->keyword, TO_CHAR);
	  return 0;
	}
    }
  entlab = 31;  
  
  if (IS_PLAYER(ch) && !IS_SET (ch->act, PLR_HOLYWALK))
    {
      SINGLE_OBJECT *ob;
      bool canmove = TRUE;
      check_room_more (ch->in_room);
      if (ch->pcdata->speedwalk > 700  && ch->position != POSITION_RUNNING)
	{
	  send_to_char("You are too tired from moving so much!\n\r", ch);
	  if (ch->desc)
	    ch->desc->inbuf[0] = '\0';
	  ch->pcdata->speedwalk = 2*PULSE_VIOLENCE +1;
	  WAIT_STATE(ch, PULSE_VIOLENCE);
	  return FALSE;
	}
      for (ob = ch->in_room->more->contents; ob != NULL; ob = ob->next_content)
	{
	  if (ob->pIndexData->item_type != ITEM_TRASH)
	    continue;
	  if (ob->pIndexData->value[door] == 0)
	    continue;
	  if (ob->pIndexData->value[6] == 0) continue;
	  switch (ob->pIndexData->value[6])
	    {
	    case 1:
	      if (ob->pIndexData->value[7] < LEVEL (ch))
		canmove = FALSE;
	      break;
	    case 2:
	      if (ob->pIndexData->value[7] != ch->pcdata->race)
		canmove = FALSE;
	      break;
	    case 3:
	      if (ob->pIndexData->value[7] != clan_number (ch))
		canmove = FALSE;
	      break;
	    case 4:
	      if (ob->pIndexData->value[7] != clan_number_2 (ch))
		canmove = FALSE;
	      break;
	    case 5:
	      if ((ch->pcdata->donated == 0 || ob->pIndexData->value[7] > ch->pcdata->donated))
		canmove = FALSE;
	      break;
	    case 6:
	      if (ob->pIndexData->value[7] > 0)
		{
		  SINGLE_OBJECT *obt;
		  canmove = FALSE;
		  for (obt = ch->carrying; obt != NULL; obt = obt->next_content)
		    {
		      if (obt->pIndexData->vnum == ob->pIndexData->value[7])
			{
			  canmove = TRUE;
			  break;
			}
		    } 
		}
	      break;
	    case 7:
	      if (ob->pIndexData->value[7] > LEVEL (ch))
		canmove = FALSE;
	      break;
	    }
	}
      if (!canmove)
	{
	  send_to_char("A strange force prevents you from going in that direction!\n\r", ch);
	  return 0;
	}
     entlab = 32;

      check_room_more(ch->in_room);
      entlab = 321;
      for (fch = ch->in_room->more->people; fch != NULL; fch = fch_next)
	{
	  fch_next = fch->next_in_room;
	  if (IS_PLAYER(fch)) continue;
	  if (fch->pIndexData->guard != door) continue;
	  if (room_is_dark(in_room) && !IS_AFFECTED(fch, AFF_INFRARED))
	    continue;
	  if (IS_AFFECTED(ch, AFF_INVISIBLE) && !IS_AFFECTED(fch, AFF_DETECT_INVIS))
	    continue;
	  if (IS_AFFECTED_EXTRA(ch, AFF_CHAMELEON) && !IS_AFFECTED_EXTRA(fch, AFF_DET_CHAMELEON))
	    continue;
	  if (fch->pIndexData->act3 % 32 != 0)
	    {
	      if (IS_SET (fch->pIndexData->act3, ACT3_GUARD_DOOR))
		{
		  if (IS_SET (fch->act, ACT_ATTITUDE))
		    {
		      do_say (fch, "You'll have to get by me first!");
		      multi_hit (fch, ch, TYPE_UNDEFINED);
		    }
		  else
		    {
		      do_say (fch, "I can't let you by.");
		    }
		  return 0;
		}
	      if (IS_SET (fch->pIndexData->act3, ACT3_GUARD_DOOR_ALIGN) && (DIFF_ALIGN(ch, fch)))
		{
		  char t[80];
		  sprintf (t, "%ss may not pass!", align_info[ch->pcdata->alignment].name);
		  do_say (fch, t);
		  return FALSE;
		}
	      
	      if (IS_SET (fch->pIndexData->act3, ACT3_CLAN_GUARD) && fch->pIndexData->clan_guard_1 != 0 &&
		  clan_number (ch) != fch->pIndexData->clan_guard_1)
		{
		  do_say (fch, "You must be a member of our clan to enter!");
		  return FALSE;
		}
	      if (IS_SET(fch->pIndexData->act3, ACT3_SECT_GUARD) && fch->pIndexData->clan_guard_2 != 0 &&
		  clan_number_2 (ch) != fch->pIndexData->clan_guard_2)
		{
		  do_say (fch, "You must be a member of our sect to enter!");
		  return FALSE;
		}
	    }
	}
    }
  check_room_more (to_room);
  entlab = 4;
  if ((fch = RIDING (ch)) != NULL)
    {
      if (fch->move == 50)
	send_to_char ("Your mount is beginning to get tired.\n\r", ch);
      if (fch->move == 25)
	send_to_char ("Your mount is very tired, and will need to rest soon.\n\r", ch);
      if (fch->move < 15)
	send_to_char ("Your mount is nearly fainting from exhaustion.\n\r", ch);
      if (fch->move < 7)
	{
	  send_to_char ("Your mount is too exhausted.\n\r", ch);
	  return 0;
	}
      if (to_sector == SECT_SNOW)
	if (fch->move > 12)
	  fch->move -= 11;
	else
	  fch->move = 1;
      fch->move -= 3;
    }
  if (IS_MOB (ch))
    {
      if (ch->pIndexData->mobtype != MOB_FISH)
	{
	  if (!IS_AFFECTED(ch, AFF_FLYING) &&
	      (to_sector > SECT_SAVANNAH &&
	       to_sector < SECT_CLOUDS))
	    return FALSE;
	}
      else
	{
	  if (to_sector < SECT_WATER_SWIM ||
	      to_sector > SECT_UNDERWATER)
	    return FALSE;
	}
      if (IS_AFFECTED (ch, AFF_CHARM)
	  && MASTER (ch) != NULL
	  && in_room == MASTER (ch)->in_room)
	{
	  send_to_char ("What? And leave your beloved master?\n\r", ch);
	  return 0;
	}
      if (MOUNTED_BY(ch) == NULL && 
	  ((IS_SET (ch->pIndexData->act3, ACT3_STAYROAD) &&
	    to_sector != SECT_ROAD) ||
	   (IS_SET (ch->act, ACT_STAYOFFROAD) &&
	    to_sector == SECT_ROAD)))
	return FALSE;
    }
  else
    {
      if (to_room->area->open == 0 && !IS_IMMORTAL (ch))
	{
	  send_to_char ("That area has not been opened to mortals yet!\n\r", ch);
	  return 0;
	}
      if (LEVEL (ch) > 100 && LEVEL (ch) < 108 && to_room->area->open == 1)
	{
	  send_to_char ("That is not a region you can explore right now.. sorry.\n\r", ch);
	  return 0;
	}
      if (!IS_SET(ch->act, PLR_HOLYWALK) && (to_sector > SECT_SAVANNAH) && (to_sector < SECT_ICE))
	{
	  if ( !IS_AFFECTED(ch, AFF_FLYING) &&
	       (to_sector == SECT_AIR || to_sector == SECT_CLOUDS))
	    {
	      send_to_char("You're not flying!\n\r", ch);
	      return 0;
	    }
	  if (to_sector == SECT_MOUNTAIN && !IS_SET(ch->act, PLR_HAS_MTN_BOOTS))
	    {
	      send_to_char("It's too steep for you to climb unaided.\n\r", ch);
	      return 0;
	    }
	  if (to_sector == SECT_UNDERWATER && 
	      !IS_SET(ch->act, PLR_HAS_SCUBA) &&
	      !IS_AFFECTED(ch, AFF_BREATH_WATER))
	    {
	      send_to_char("You need to be able to breathe water to go there.\n\r", ch);
	      return 0;
	    }
	  if (to_sector == SECT_WATER_NOSWIM && 
	      !IS_SET(ch->act, PLR_HAS_BOAT))
	    {
	      send_to_char("The water is too rough for you to be out here without a boat!\n\r", ch);
	      return 0;
	    }
	  if (to_sector == SECT_WATER_SWIM &&
	      !IS_SET(ch->act, PLR_HAS_BOAT))
	    {
	      if ((IS_SET(to_room->room_flags, ROOM_INDOORS) || IS_SET(to_room->room_flags, ROOM_UNDERGROUND)) && ch->pcdata->learned[gsn_swim] < 50)
		{
		  send_to_char("You don't know how to swim well enough to go in there.\n\r", ch);
		  return 0;
		}

	      if (!IS_AFFECTED(ch, AFF_FLYING) && number_range (0, 105) > ch->pcdata->learned[gsn_swim])
		{
		  send_to_char ("You try to swim, but your skill is not great enough!\n\r", ch);
		  SUBMOVE(ch,5);
		  return FALSE;
		}
	    }
	}
    }
  entlab = 5;
  {
    int move = (sectors[(int) in_room->sector_type].moves + sectors[to_sector].moves)/2;
    int flagg;
    flagg = 0;
    
    if (to_sector == SECT_SNOW)
      {
	if (RIDING(ch) != NULL)
	  send_to_char("Your mount has an extremely tough time trudging through the snow.\n\r", ch);
	else
	  {
	    if (!IS_SET(ch->act, PLR_HAS_SNOWSHOES))
	      {
		send_to_char ("The going is extremely tough without some type of snowshoes or skis.\n\r", ch);
		move += 11;
	      }
	  }
      }
    if (IS_PLAYER (ch))
      {
	int weightmult = (get_curr_str (ch) + 
			  (IS_AUGMENTED(ch, AUG_BODY_POWER) ? 2 : 0) +
			  (IS_AUGMENTED(ch, AUG_LEG_POWER) ? 2 : 0) +
			  (IS_AUGMENTED(ch, AUG_FOOT_POWER) ? 1 : 0));
	move += (ch->pcdata->carry_weight /(weightmult * WGT_MULT))-1;
	if (ch->pcdata->carry_weight / WGT_MULT > 119)
          move += 30;
	if (ch->pcdata->carry_weight / WGT_MULT > 199)
          move += 100;

	if (IS_AUGMENTED(ch, AUG_FOOT_SPD)) move -=(number_range(0,2));
	if (IS_AUGMENTED(ch, AUG_LEG_SPD)) move -=(number_range(0,2));
	if (IS_AFFECTED(ch, AFF_LETHARGY))
	 //Original value is 5,30  Changed by Sabelis 12-15-2005
          move +=number_range(6,32);
        if (move < 0) move = 0;
	if (IS_AFFECTED(ch, AFF_FLYING)) 
	  move /=2;
	else
	  if (!IS_SET (ch->act, PLR_HOLYWALK) &&
	      !IS_SET(ch->act, PLR_HAS_SHOES) &&
	      RIDING (ch) == NULL && 
	      !IS_AUGMENTED(ch, AUG_FOOT_PLATING))
	    {
	      if (to_sector > SECT_FIELD &&
		  to_sector != SECT_UNDERWATER &&
		  to_sector != SECT_WATER_NOSWIM &&
		  to_sector != SECT_WATER_SWIM && 
		  to_sector < SECT_CAVE) 
		{
		  send_to_char ("Ouch! Every step hurts! Try getting some shoes...\n\r", ch);
		  move +=2;;
		  
		}
	    }
      }
    
    entlab = 6;
    if (move < 1) move = 1;
    if (ch->move < move && RIDING (ch) == NULL)
      {
	send_to_char ("You are too exhausted to move.\n\r", ch);
	if (LEADER (ch) != NULL)
	  {
	    sprintf (buf, "%s was too exhausted to follow!", NAME (ch));
	    group_notify (buf, ch);
	    return 0;
	  }
	return 0;
      }
    
    if (RIDING (ch) == NULL)
      if(pow.easier_moves)
	SUBMOVE(ch,(move/2));
      else
	SUBMOVE(ch,move);
    
    if (ch->move < 20 && number_range(1,7) == 2)
      {
	act ("You start breathing heavily and exhaustion overcomes your tired body.", ch, NULL, NULL, TO_CHAR_SPAM);
	act ("$n is breathing heavily and looks very sweaty.", ch, NULL, NULL, TO_ROOM_SPAM);
      }
    WAIT_STATE (ch, 1);
  }
  if (IS_PLAYER (ch))
    {
      if(to_sector == SECT_UNDERWATER || 
	 (to_sector == SECT_WATER_SWIM && 
	  !IS_AFFECTED(ch, AFF_FLYING) &&
	  !IS_SET(ch->act, PLR_HAS_BOAT)))
	{
	  sprintf (buf, "You swim %s.", dir_name[door]);
	  skill_gain (ch, gsn_swim, FALSE);
	  if (!IS_AFFECTED(ch, AFF_BREATH_WATER) 
	      && number_percent() > ch->pcdata->learned[gsn_swim])
	    {
	      send_to_char ("You try to swim, but your skill is not great enough!\n\r", ch);
	      SUBMOVE(ch, 5);
	    }
	  flagone = TRUE;
	}
      else if (!IS_AFFECTED (ch, AFF_FLYING) &&
	       (to_sector == SECT_WATER_SWIM 
		|| to_sector == SECT_WATER_NOSWIM))
	{
	  sprintf (buf, "You paddle %s.", dir_name[door]);
	  flagtwo = TRUE;
	}
      else if (MOUNTED_BY (ch) != NULL)
	sprintf (buf, "You wander %s, rode by %s.", dir_name[door],
		 PERS (MOUNTED_BY (ch), ch));
      else if (RIDING (ch) != NULL)
	sprintf (buf, "You ride %s %s.", PERS (RIDING (ch), ch),
		 dir_name[door]);
      else if (IS_AFFECTED (ch, AFF_FLYING))
	sprintf (buf, "You fly %s.", dir_name[door]);
      else if (IS_AFFECTED (ch, AFF_SNEAK))
	{
	  sprintf (buf, "You sneak %s.", dir_name[door]);
	  if (number_range (1, 144) == 99)
	    skill_gain (ch, gsn_sneak, FALSE);
	}
      else if (ch->position == POSITION_RUNNING)
	sprintf (buf, "You run %s.", dir_name[door]);
      else
	sprintf (buf, "You walk %s.", dir_name[door]);
    }
  buf[0] = UPPER (buf[0]);
  send_to_char (buf, ch);
  send_to_char ("\n\r", ch);
  entlab = 7;
  for (fch = in_room->more->people; fch != NULL; fch = fch_next)
    {
      fch_next = fch->next_in_room;
      if (fch->position < POSITION_STANDING || 
	  ch == fch || 
	  RIDING (ch) == fch || 
	  MOUNTED_BY (ch) == fch)
	continue;
      if (!can_see (fch, ch) && RIDING(ch) == NULL)
	continue;
      if (IS_MOB(fch) && IS_SET(fch->pIndexData->act4, ACT4_LEAVESCRIPT))
	{
	again_20:
	  {
	    SINGLE_TRIGGER *tr;
	    SCRIPT_INFO *s;
	    for (tr = trigger_list[TLEAVES]; tr != NULL; tr = tr->next)
	      {
		if (IS_MOB (fch) && fch->pIndexData->vnum == tr->attached_to_mob)
		  {
		    if (tr->players_only && IS_MOB (ch))
		      continue;
		    if (tr->running_info && !tr->interrupted)
		      continue;	/* Already running, interrupted, but script says not to allow interruptions. */
		    if (tr->running_info && tr->interrupted != 2)
		      {
			end_script (tr->running_info);
			goto again_20;
		      }
		    /* ----------------- */
		    /* Start the script! */
		    /* ----------------- */
		    tr->running_info = mem_alloc (sizeof (*tr->running_info));
		    s = tr->running_info;
		    bzero (s, sizeof (*s));
		    s->current = ch;
		    s->mob = fch;
		    strcpy (s->code_seg, tr->code_label);
		    s->current_line = 0;
		    s->called_by = tr;
		    s->next = info_list;
		    info_list = s;
		    execute_code (s);
		    /* ----------------- */
		  }
	      }
	  }
	}
      /* End trigger check! */
      
      if (MOUNTED_BY (ch) != NULL)
	break; 
      else if (RIDING (ch) != NULL)
	sprintf (buf, "$N rides %s %s.", PERS (RIDING (ch), fch), dir_name[door]);
      else if (IS_AFFECTED (ch, AFF_FLYING) && !IS_AFFECTED(ch, AFF_SNEAK))
	sprintf (buf, "$N flies %s.", dir_name[door]);
      else if (flagtwo)
	{
	  sprintf (buf, "$N travels %s through the water.", dir_name[door]);
	}
      else if (flagone)
	{
	  sprintf (buf, "$N swims %s.", dir_name[door]);
	}
      else if (IS_AFFECTED (ch, AFF_SNEAK))
	{
	  if (IS_SET (fch->act, PLR_HOLYLIGHT) || (IS_PLAYER (ch) && number_range (1, 75) > ch->pcdata->learned[gsn_sneak]))
	    sprintf (buf, "$N sneaks %s.", dir_name[door]);
	  else
	    buf[0] = '\0';
	}
      else if ((IS_MOB (ch) && IS_SET (ch->act, ACT_SCRAMBLE))
	       || (IS_PLAYER (ch) && ch->position == POSITION_RUNNING))
	sprintf (buf, "$N runs %s.", dir_name[door]);
      else
	sprintf (buf, "$N walks %s.", dir_name[door]);
      buf[0] = UPPER (buf[0]);
      if (buf[0] != '\0')
	{
	  if (!is_same_group (fch, ch) || fch->position != POSITION_STANDING)
	    act (buf, fch, NULL, ch, TO_CHAR);
	}
    }
  if (((ch->hit < ch->max_hit / 3) &&    !IS_AFFECTED_EXTRA(ch, AFF_NOBLOOD)) || IS_AFFECTED(ch, AFF_WOUND))
    if (in_room->sector_type < SECT_WATER_SWIM || in_room->sector_type > SECT_HOLLOW || (in_room->sector_type > SECT_CLOUDS && in_room->sector_type < SECT_TROPICAL))
      if (IS_PLAYER(ch) ||(IS_MOB (ch) && ch->pIndexData->mobtype != MOB_GHOST))
	SET_BIT(in_room->blood, blood_number[door]);           
  
  entlab = 8;
  if (ch->in_room == to_room)
    return FALSE;
  /* Leaving room, check for triggers on room! */
  if (IS_SET(ch->in_room->room_flags, ROOM_LEAVESCRIPT))
    {
    again_21:
      {
	SINGLE_TRIGGER *tr;
	SCRIPT_INFO *s;
	for (tr = trigger_list[TLEAVES]; tr != NULL; tr = tr->next)
	  {
	    if (ch->in_room->vnum == tr->attached_to_room)
	      {
		if (tr->running_info && !tr->interrupted)
		  continue;		/* Already running, interrupted, but script says not to allow interruptions. */
		if (tr->players_only && IS_MOB (ch))
		  continue;
		if (tr->running_info && tr->interrupted != 2)
		  {
		    end_script (tr->running_info);
		    goto again_21;
		  }
		/* ----------------- */
		/* Start the script! */
		/* ----------------- */
		tr->running_info = mem_alloc (sizeof (*tr->running_info));
		s = tr->running_info;
		bzero (s, sizeof (*s));
		s->current = ch;
		s->room = ch->in_room;
		strcpy (s->code_seg, tr->code_label);
		s->current_line = 0;
		s->called_by = tr;
		s->next = info_list;
		info_list = s;
		execute_code (s);
		/* ----------------- */
	      }
	  }
      }
    }
  /* End trigger check! */
  


  
 
  /* Char from room/ char to room  */
  
  if(ch == in_room->more->people)
    {
      in_room->more->people = ch->next_in_room;
    }
  else
    {
      CHAR_DATA *prev;
      for (prev = in_room->more->people; prev; prev = prev->next_in_room)
	{
	  if (prev->next_in_room == ch)
	    {
	      prev->next_in_room = ch->next_in_room;
	      break;
	    }
	}
      if (prev == NULL) return 0;
    }
  entlab = 9;
  if(IS_PLAYER(ch))
    {
      CHAR_DATA *pir;
      in_room->light -= ch->pcdata->light;
      if(in_room->light < 0) in_room->light = 0;
      in_room->area->nplayer--;
      in_room->more->pcs--;
      for (pir = in_room->more->people; pir != NULL; pir = pir->next_in_room)
	{
	  if (in_room->more->pcs == 0)
	    {
	      remove_from_aggro_list (pir);
	    }
	}
      
       
	
	{
	  fndd = FALSE;
	  for (ptrk = in_room->ptracks; ptrk != NULL; ptrk = ptrk->next_track_in_room)
	    {
	      if (ptrk->online== ch->pcdata->online_spot)
		{
		  ptrk->dir_left = door;
		  fndd = TRUE;
		  break;
		}
	    }
	  if (!fndd)
	    {
	      ptrk = new_ptrack (ch, in_room);
	      ptrk->dir_left = door;
	    }
	}
    }
  else
    {
      if(in_room->more->pcs == 0)
	remove_from_aggro_list(ch);
      if (IS_SET(ch->pIndexData->act4, ACT4_COMMANDSCRIPT))
	in_room->command_objs--;
      if (ch->in_room->command_objs < 0)
	in_room->command_objs = 0;
      if(number_range(1,5) == 2)
	{
	  fndd = FALSE;
	  for (trk = in_room->tracks; trk != NULL; trk = trk->next_track_in_room)
	    {
	      if (trk->ch== ch)
		{
		  trk->dir_left = door;
		  fndd = TRUE;
		  break;
		}
	    }
	  if (!fndd)
	    {
	      trk = new_track (ch, in_room);
	      trk->dir_left = door;
	    }
	}
    }
  check_clear_more (in_room);
  entlab = 10;
  if ((IS_SET(to_room->room_flags,ROOM_INDOORS) || 
       IS_SET(to_room->room_flags, ROOM_UNDERGROUND) || 
       to_room->sector_type >= SECT_CAVE)  && 
      IS_AFFECTED(ch,AFF_FLYING) && 
      (ch->height > INDOOR_FLY_HEIGHT)) 
    {
      send_to_char("You cannot fly while indoors.\n\r",ch);
      do_land(ch,"");
    }
  check_room_more(to_room);
  ch->in_room = to_room;
  if (to_room->more->people != ch)
    ch->next_in_room = to_room->more->people;
  to_room->more->people = ch;
  if(IS_PLAYER(ch))
    {
      CHAR_DATA *d;
      for (d = to_room->more->people; d != NULL; d = d->next_in_room)
	{
	  if (IS_MOB (d))
	    add_to_aggro_list (d);
	}
     
      to_room->area->nplayer++;
      to_room->more->pcs++;
      if (to_room->light < 0) to_room->light = 0;
      to_room->light += ch->pcdata->light;
      if(to_room->vnum >= (BATTLEGROUND_START_VNUM + bg_multiplier) && 
	 to_room->vnum <= (BATTLEGROUND_END_VNUM + bg_multiplier))
	{
	  check_fgt (ch);
	  ch->fgt->in_battle = TRUE;
	}
      else
	{
	  if (ch->fgt)
	    {
	      ch->fgt->in_battle = FALSE;
	      ch->fgt->wasroomtwo = to_room->vnum;
	    }
	}
    
	{
	  fndd = FALSE;
	  for (ptrk = to_room->ptracks; ptrk != NULL; ptrk = ptrk->next_track_in_room)
	    {
	      if (ptrk->online== ch->pcdata->online_spot)
		{
		  ptrk->dir_came = rev_dir[door];
		  fndd = TRUE;
		  break;
		}
	    }
	  if (!fndd)
	    {
	      ptrk = new_ptrack (ch, to_room);
	      ptrk->dir_came = rev_dir[door];
	      
	    }
	}
      
    }
  else
    {
      if (IS_SET(ch->pIndexData->act4, ACT4_COMMANDSCRIPT))
	ch->in_room->command_objs++;
      if(ch->in_room->command_objs < 0)
	ch->in_room->command_objs = 0;
      if (IS_SET(ch->pIndexData->act4, ACT4_MOVESCRIPT))
	{
	again_22:	
	  {
	    SINGLE_TRIGGER *tr;
	    SCRIPT_INFO *s;
	    for (tr = trigger_list[TMOVES]; tr != NULL; tr = tr->next)
	      {
		if (ch->pIndexData->vnum == tr->attached_to_mob)
		  {
		    if (tr->running_info && !tr->interrupted)
		      continue;	/* Already running, interrupted, but script says not to allow interruptions. */
		    if (tr->running_info && tr->interrupted != 2)
		      {
			end_script (tr->running_info);
			goto again_22;
		      }
		    /* ----------------- */
		    /* Start the script! */
		    /* ----------------- */
		    tr->running_info = mem_alloc (sizeof (*tr->running_info));
		    s = tr->running_info;
		    bzero (s, sizeof (*s));
		    s->current = ch;
		    s->mob = ch;
		    strcpy (s->code_seg, tr->code_label);
		    s->current_line = 0;
		    s->called_by = tr;
		    s->next = info_list;
		    info_list = s;
		    execute_code (s);
		    /* ----------------- */
		  }
	      }
	  }
	}
      if(number_range(1,5) == 2)
	{
	  fndd = FALSE;
	  for (trk = to_room->tracks; trk != NULL; trk = trk->next_track_in_room)
	    {
	      if (trk->ch == ch)
		{
		  trk->dir_came = rev_dir[door];
		  fndd = TRUE;
		  break;
		}
	    }
	  if (!fndd)
	    {
	      trk = new_track (ch, to_room);
	      trk->dir_came = rev_dir[door];
	    }
	}
    }
  entlab = 11;
  if (RIDING (ch) != NULL && ch->in_room != RIDING (ch)->in_room
      && depth == 0)
    {
      depth++;
      if (!move_char (RIDING (ch), door))
	{
	  check_fgt (ch);
	  check_fgt (ch->fgt->riding);
	  send_to_char ("You just left your mount behind...\n\r", ch);
	  ch->fgt->riding->fgt->mounted_by = NULL;
	  ch->fgt->riding = NULL;
	}
      depth--;
    }
  else if (MOUNTED_BY (ch) != NULL && ch->in_room != MOUNTED_BY (ch)->in_room
	   && depth == 0)
    {
      depth++;
      if (!move_char (MOUNTED_BY (ch), door))
	{
	  send_to_char ("You just left your rider behind...\n\r", ch);
	  check_fgt (ch);
	  check_fgt (ch->fgt->mounted_by);
	  ch->fgt->mounted_by->fgt->riding = NULL;
	  ch->fgt->mounted_by = NULL;
	}
      depth--;
    }
  
  if (IS_PLAYER(ch) && ch->position == POSITION_RUNNING && !IS_SET
      (ch->pcdata->act2, PLR_BRIEF))
    {
      SET_BIT (ch->pcdata->act2, PLR_BRIEF);
      do_look (ch, "auto");
      REMOVE_BIT (ch->pcdata->act2, PLR_BRIEF);
    }
  else
    {
      do_look (ch, "auto");
    }
  entlab = 12;
  /* Entered room, check for triggers on room! */
  if (IS_SET(to_room->room_flags, ROOM_ENTERSCRIPT))
    {
    again_23:
      {
	SINGLE_TRIGGER *tr;
	SCRIPT_INFO *s;
	for (tr = trigger_list[TENTERS]; tr != NULL; tr = tr->next)
	  {
	    if (ch->in_room->vnum == tr->attached_to_room)
	      {
		if (tr->running_info && !tr->interrupted)
		  continue;		/* Already running, interrupted, but script says not to allow interruptions. */
		if (tr->players_only && IS_MOB (ch))
		  continue;
		if (tr->running_info && tr->interrupted != 2)
		  {
		    end_script (tr->running_info);
		    goto again_23;
		  }
		/* ----------------- */
		/* Start the script! */
		/* ----------------- */
		tr->running_info = mem_alloc (sizeof (*tr->running_info));
		s = tr->running_info;
		bzero (s, sizeof (*s));
		s->current = ch;
		s->room = ch->in_room;
		strcpy (s->code_seg, tr->code_label);
		s->current_line = 0;
		s->called_by = tr;
		s->next = info_list;
		info_list = s;
		execute_code (s);
		/* ----------------- */
	      }
	  }
      }
    }
  /* End trigger check! */
  
  
  for (fch = to_room->more->people; fch != NULL; fch = fch_next)
    {
      fch_next = fch->next_in_room;
      if (ch == fch || RIDING (ch) == fch || MOUNTED_BY (ch) == fch)
	continue;
      if (IS_AFFECTED (ch, AFF_AWARENESS) && !IS_AWAKE (ch))
	{
	  sprintf (buf, "You sense a life-form moving in from the %s.", dir_rev[door]);
	  act (buf, ch, NULL, fch, TO_CHAR);
	  continue;
	}
      if (!can_see (fch, ch) && (RIDING (ch) == NULL))
	continue;
      if (MOUNTED_BY (ch) != NULL)
	break;
      else if (RIDING (ch) != NULL)
	sprintf (buf, "$N rides %s in from %s.",
		 PERS (RIDING (ch), fch), dir_rev[door]);
      else if (IS_AFFECTED (ch, AFF_FLYING) && !IS_AFFECTED(ch, AFF_SNEAK))
	sprintf (buf, "$N flies in from %s.",
		 dir_rev[door]);
      else if (IS_AFFECTED (ch, AFF_SNEAK))
	{
	  if (IS_SET (fch->act, PLR_HOLYLIGHT) || (IS_PLAYER (ch) && number_range (1, 90) > ch->pcdata->learned[gsn_sneak]))
	    sprintf (buf, "$N sneaks in from %s.", dir_rev[door]);
	  else
	    buf[0] = '\0';
	}
      else if (flagone)
	{
	  sprintf (buf, "$N swims in from %s.", dir_rev[door]);
	}
      else if (flagtwo)
	{
	  sprintf (buf, "$N paddles in from %s.", dir_rev[door]);
	}
      else if ((IS_MOB (ch) && IS_SET (ch->act, ACT_SCRAMBLE))
	       || (IS_PLAYER (ch) && ch->position == POSITION_RUNNING))
	sprintf (buf, "$N runs in from %s.", dir_rev[door]);
      else
	sprintf (buf, "$N arrives from %s.", dir_rev[door]);
      buf[0] = UPPER (buf[0]);
      if (buf[0] != '\0')
	{
	  /*	 if (!is_same_group(fch,ch)) */
	  act (buf, fch, NULL, ch, TO_CHAR);
	}
      if ((IS_PLAYER(fch) && IS_PLAYER(ch)) &&
	  DIFF_ALIGN(ch, fch) && LEVEL(ch) < 100 && LEVEL(fch) < 100
	  && ALIGN(fch) != 0 && ALIGN (ch) != 0)
	{
	  if (fch->pcdata->no_quit_pk < 4)
	    fch->pcdata->no_quit_pk = 4; /* JRAJRA */
	  if (ch->pcdata->no_quit_pk < 4)
	    ch->pcdata->no_quit_pk = 4; /* JRAJRA */
	} 
      /* Entered room, check for triggers on mobbies! */
      if (IS_MOB(fch) && IS_SET(fch->pIndexData->act4, ACT4_ENTERSCRIPT))
	{
	again_24:
	  {
	    SINGLE_TRIGGER *tr;
	    SCRIPT_INFO *s;
	    for (tr = trigger_list[TENTERS]; tr != NULL; tr = tr->next)
	      {
		if (IS_MOB (fch) && fch->pIndexData->vnum == tr->attached_to_mob)
		  {
		    if (tr->running_info && !tr->interrupted)
		      continue;	/* Already running, interrupted, but script says not to allow interruptions. */
		    if (tr->players_only && IS_MOB (ch))
		      continue;
		    if (tr->running_info && tr->interrupted != 2)
		      {
			end_script (tr->running_info);
			goto again_24;
		      }
		    /* ----------------- */
		    /* Start the script! */
		    /* ----------------- */
		    tr->running_info = mem_alloc (sizeof (*tr->running_info));
		    s = tr->running_info;
		    bzero (s, sizeof (*s));
		    s->current = ch;
		    s->mob = fch;
		    strcpy (s->code_seg, tr->code_label);
		    s->current_line = 0;
		    s->called_by = tr;
		    s->next = info_list;
		    info_list = s;
		    execute_code (s);
		    /* ----------------- */
		  }
	      }
	  }
	}
      /* End trigger check! */
      
   
    } 
entlab = 13;
  if (IS_PLAYER (ch))
    {
      if (ch->pcdata->learned[gsn_search] > 40)
	{
	  bool yesno;
	  int hh;
	  yesno = FALSE;
	  for (hh = 0; hh < 6; hh++)
	    if (ch->in_room->exit[hh] && ch->in_room->exit[hh]->d_info && IS_SET (ch->in_room->exit[hh]->d_info->exit_info, EX_HIDDEN))
	      yesno = TRUE;
	  if (yesno && number_range (1, 4) == 3)
	    send_to_char ("You sense a hidden exit somewhere in this room. Perhaps you should\n\rsearch in all directions...\n\r", ch);
	}
      if (HUNTING (ch) != NULL)
	do_track (ch, "continue");
      if (PHUNTING (ch) !=NULL)
	do_ptrack (ch, "continue");
    }
  check_room_more (in_room);
  for (fch = in_room->more->people; fch != NULL; fch = fch_next)
    {
      fch_next = fch->next_in_room;
      if (MASTER (fch) == ch && fch->position >= POSITION_STANDING && FIGHTING (fch) == NULL && (!MASTER(ch) || MASTER(ch) == fch))
	{
	  act ("You follow $N $t.", fch, dir_name[door], ch, TO_CHAR);
	  if(move_char (fch, door))
	    followers++;
	}
    }
    
  if (IS_PLAYER(ch))
    { 
      for (i=0; i < 6; i++)
	{
	  curr_room = ch->in_room;
	  for (n = 0; n < 4; n++)
	    {
	      if (((pexit = curr_room->exit[i])  == NULL) 
		  || ((troom =(ROOM_DATA *) pexit->to_room) == NULL)
		  || (pexit->d_info && IS_SET(pexit->d_info->exit_info, EX_CLOSED))
		  || !troom->more
		  || troom->more->pcs <= 0)
		break;
	      for (viewer = troom->more->people; viewer != NULL; viewer =viewer->next_in_room)
		{
		  if (IS_MOB(viewer) || !DIFF_ALIGN(ch, viewer)) continue;
		  if (LEVEL(ch) > 100) continue;
		  
		  // ARC: set timer only if  align != 0
		  if ( ALIGN(ch) != 0 && ALIGN(viewer) != 0 && !IN_BATTLE(viewer)) 
		  {
			viewer->pcdata->no_quit_pk = 5;
			ch->pcdata->no_quit_pk = 5;
		  }
		  if (!IS_SET(viewer->pcdata->act2, PLR_AUTOSCAN)
		      || viewer->pcdata->learned[gsn_sixth_sense] < 50
                      || IS_AFFECTED(ch, AFF_BLIND))
		    continue;
      
		  if ((!IS_AFFECTED(viewer, AFF_INFRARED) && 
		      room_is_dark(ch->in_room)) || 
		      (!IS_AFFECTED(viewer, AFF_DETECT_INVIS) &&
		       IS_AFFECTED(ch, AFF_INVISIBLE))
		      || IS_AFFECTED_EXTRA(ch, AFF_CHAMELEON))
		    sprintf(buf, "Someone arrives");
		  else if (ALIGN(ch) != 0)
		  {
			  //ARC: change the sixth sense message depending upon which align is sensed
			  if (!is_first_letter_vowel(align_info[ch->pcdata->alignment].name))
				sprintf(buf, "You see +* A %s %s *+ arrive", align_info[ch->pcdata->alignment].name, race_info[ch->pcdata->race].name);
			  else
				sprintf(buf, "You see +* An %s %s *+ arrive", align_info[ch->pcdata->alignment].name, race_info[ch->pcdata->race].name);
		  }
		  else
		  {
			if (!is_first_letter_vowel(race_info[ch->pcdata->race].name))
				sprintf(buf, "You see a %s arrive", (race_info[ch->pcdata->race].name));
			else
				sprintf(buf, "You see an %s arrive", (race_info[ch->pcdata->race].name));
		  }
		  if (n == 0) strcat(buf, " close by");
		  else if (n > 1) strcat (buf, " far off");
		  switch (i)
		    {
		    case 0:
		      strcat(buf, " to the south");
		      break;
		    case 1:
		      strcat(buf, " to the west");
		      break;
		    case 2:
		      strcat(buf, " to the north");
		      break;
		    case 3:
		      strcat(buf, " to the east");
		      break;
		    case 4:
		      strcat(buf, " down below");
		      break;
		    case 5:
		      strcat(buf,  " up above");
		      break;
		    }
		  if (followers > 0)
		    sprintf(buf2, " with %d followers.\n\r", followers);
		  else
		    sprintf(buf2, ".\n\r");
		  strcat (buf, buf2);
		  send_to_char(buf, viewer);
		  break;
		}
	      curr_room = troom;
	    }
	}
      entlab = 14;    
      if (IS_AFFECTED(ch, AFF_BLIND))
        {
	  REMOVE_BIT(ch->pcdata->act2, PLR_MAPPING);
        }
      else if (ch->desc && IS_SET(ch->pcdata->act2, PLR_MAPPING))
	{
	  int newx = ch->pcdata->x;
	  int newy = ch->pcdata->y;
	  entlab = 1400;   
	  if ((door % 2) == 0)
	    newy += (door-1);
	  else
	    newx += (2-door);
	  if (newy < 2 || newx < 2 || newy > MAP_HEIGHT-2 || newx >
	      MAP_WIDTH-2 || door == 4 || door == 5 || IS_SET(ch->pcdata->act2,
							      PLR_MAPSPAM))
	    {
	       entlab = 140;   
	      small_map(ch);
	    }
	  else 
	    { 
              bool found = FALSE;
	      sprintf(buf, "\x1b[%d;%dr",MAP_HEIGHT,ch->pcdata->pagelen);
	       entlab = 141;   
	      send_to_char(buf, ch);
	      gotoxy(ch, ch->pcdata->x,ch->pcdata->y);
              if (in_room->more && in_room->more->pcs > 0 &&
		  IS_SET(ch->pcdata->act2, PLR_VIEWPLAYERS))
                {
                  if (LEVEL(ch) == MAX_LEVEL)
                    {
                      found = TRUE;
                      send_to_char("\x1b[1;31mP", ch);
                    }
                  else
                    {
		      check_room_more(in_room);
		      entlab = 142;   
		      for(fch = in_room->more->people; fch != NULL; fch=fch->next_in_room)
                        {
                          if (IS_PLAYER(fch) && !DIFF_ALIGN(ch, fch))
                            {
			      found = TRUE;
			      send_to_char("\x1b[1;31mP", ch);
			      break;
                            }
                        }
                    }
		  
		}
	      if (!found && in_room->more && in_room->more->people != NULL && IS_SET(ch->pcdata->act2, PLR_VIEWMOBS))
		{
                   CHAR_DATA *rch;
		   entlab = 143;  
                   for (rch = in_room->more->people; rch != NULL; rch =rch->next_in_room)
                     {
                       if(IS_MOB(rch))
                         {
		           send_to_char("\x1b[1;34mM", ch);
		           found = TRUE;
                         }
                     }
		}
	      else if (!found)
		{
  entlab = 1415;

		  write_to_buffer(ch->desc,color_table[in_room->color].code, 0);
		  if (IS_SET(ch->pcdata->act2, PLR_NOIBM))
		    send_to_char(in_room->noibm, ch);
		  else
		     send_to_char(in_room->ibm, ch);
		}
	      
	      gotoxy(ch, (newx), (newy));
              write_to_buffer(ch->desc, "\x1b[1;31mX", 0);
	      if ((door % 2) == 0)
		ch->pcdata->y += (door-1);
	      else
		ch->pcdata->x += (2-door);
	      sprintf(buf, "\x1B[%d;1f", ch->pcdata->pagelen);
	      send_to_char(buf, ch);
	    }
	}/*
  entlab = 1416;
 
      if (to_room->more->trap)
	{
	  if (number_range(1,130) < ch->pcdata->learned[gsn_locate_trap])
	    {
	      send_to_char("You notice a trap in this room! Be careful.\n\r", ch);
	      skill_gain(ch, gsn_locate_trap, TRUE);
	    }
	  else
	    set_off_trap(ch, to_room, FALSE);
	}*/
    }
  entlab = 1417;
  check_clear_more (in_room);
  entlab = 1418;
  if (IS_PLAYER(ch))
    {
      ch->pcdata->speedwalk += 2;
    }
  return 1;
}

void
do_run (CHAR_DATA * ch, char *argy)
{
  char arg1[STD_LENGTH];
  char arg2[STD_LENGTH];
  int door, count;
  DEFINE_COMMAND ("run", do_run, POSITION_STANDING, 0, LOG_NORMAL, "Usage:  Run <direction> <distance>")

    argy = one_argy (argy, arg1);
  argy = one_argy (argy, arg2);
  if (ch->position != POSITION_STANDING)
    {
      send_to_char ("You must be standing to run!\n\r", ch);
      return;
    }
  if (LEVEL (ch) < 100 && ch->in_room && (ch->in_room->sector_type == SECT_WATER_NOSWIM ||
			       ch->in_room->sector_type == SECT_WATER_SWIM))
    {
      send_to_char ("You are in water; you can't run!\n\r", ch);
      return;
    }
  if (IS_AFFECTED (ch, AFF_FLYING))
    {
      send_to_char ("Can't run while flying.\n\r", ch);
      return;
    }
  if (arg1[0] == '\0')
    {
      send_to_char ("Run which way?\n\r", ch);
      return;
    }
  count = 0;
  if (arg2[0] != '\0')
    count = atoi (arg2);
  door = get_direction_number (arg1);
  if (door > 5 || door < 0)
    {
      send_to_char ("Run which way?\n\r", ch);
      return;
    }
  NEW_POSITION(ch, POSITION_RUNNING);
  if (count)
    {
      while (count)
	{
	  if (!move_char (ch, door))
	    break;
	  SUBMOVE(ch,6);
	  if (ch->move < 5)
	    {
	      send_to_char ("You are too exhausted.\n\r", ch);
	      break;
	    }
	  count--;
	}
      NEW_POSITION(ch, POSITION_STANDING);
      return;
    }
  while (move_char (ch, door))
    {
      SUBMOVE(ch,6);
    }
  NEW_POSITION(ch, POSITION_STANDING);
  return;
}


void
do_north (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("north", do_north, POSITION_FIGHTING, 0, LOG_NORMAL, "Moves you north.")

    if (FIGHTING (ch) != NULL || ch->position == POSITION_FIGHTING) {
	send_to_char("You are in the middle of combat!\n",ch);
    	return;
	}
  move_char (ch, DIR_NORTH);

  return;
}

void
do_east (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("east", do_east, POSITION_FIGHTING, 0, LOG_NORMAL, "Moves you east.")

    if (FIGHTING (ch) != NULL || ch->position == POSITION_FIGHTING) {
	send_to_char("You are in the middle of combat!\n",ch);
	return;
	}
  move_char (ch, DIR_EAST);
  return;
}

void
do_south (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("south", do_south, POSITION_FIGHTING, 0, LOG_NORMAL, "Moves you south.")

    if (FIGHTING (ch) != NULL || ch->position == POSITION_FIGHTING) {
	send_to_char("You are in the middle of combat!\n",ch);
	return;
	}
  move_char (ch, DIR_SOUTH);
  return;
}

void
do_west (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("west", do_west, POSITION_FIGHTING, 0, LOG_NORMAL, "Moves you west.")

    if (FIGHTING (ch) != NULL || ch->position == POSITION_FIGHTING) {
	send_to_char("You are in the middle of combat!\n",ch);
	return;
	}
  move_char (ch, DIR_WEST);
  return;
}

void
do_up (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("up", do_up, POSITION_STANDING, 0, LOG_NORMAL, "Moves you up.")

    if (FIGHTING (ch) != NULL || ch->position == POSITION_FIGHTING)
    return;
  move_char (ch, DIR_UP);
  return;
}

void
do_down (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("down", do_down, POSITION_STANDING, 0, LOG_NORMAL, "Moves you down.")

    if (FIGHTING (ch) != NULL || ch->position == POSITION_FIGHTING)
    return;
  move_char (ch, DIR_DOWN);
  return;
}

int
find_door (CHAR_DATA * ch, char *arg)
{
  EXIT_DATA *pexit;
  int door;
  bool FLAGDIR;
  FLAGDIR = FALSE;
  if (!str_cmp (arg, "n") || !str_cmp (arg, "north"))
    {
      door = 0;
      FLAGDIR = TRUE;
    }
  else if (!str_cmp (arg, "e") || !str_cmp (arg, "east"))
    {
      door = 1;
      FLAGDIR = TRUE;
    }
  else if (!str_cmp (arg, "s") || !str_cmp (arg, "south"))
    {
      door = 2;
      FLAGDIR = TRUE;
    }
  else if (!str_cmp (arg, "w") || !str_cmp (arg, "west"))
    {
      door = 3;
      FLAGDIR = TRUE;
    }
  else if (!str_cmp (arg, "u") || !str_cmp (arg, "up"))
    {
      door = 4;
      FLAGDIR = TRUE;
    }
  else if (!str_cmp (arg, "d") || !str_cmp (arg, "down"))
    {
      door = 5;
      FLAGDIR = TRUE;
    }
  else
    {
      for (door = 0; door <= 5; door++)
	{
	  if ((pexit = ch->in_room->exit[door]) != NULL
	      && pexit->d_info &&
	      IS_SET (pexit->d_info->exit_info, EX_ISDOOR)
	      && pexit->d_info->keyword != NULL
	      && is_name (arg, pexit->d_info->keyword))
	    return door;
	}
      return -1;
    }
  if ((pexit = ch->in_room->exit[door]) != NULL && pexit->d_info && IS_SET (pexit->d_info->exit_info,
						      EX_HIDDEN) && FLAGDIR)
    {
      act ("I see no door $T here.", ch, NULL, get_direction (arg), TO_CHAR);
      return -1;
    }
  if ((pexit = ch->in_room->exit[door]) == NULL)
    {
      act ("I see no door $T here.", ch, NULL, get_direction (arg), TO_CHAR);
      return -1;
    }
  if (!pexit->d_info || !IS_SET (pexit->d_info->exit_info, EX_ISDOOR))
    {
      send_to_char ("You can't do that.\n\r", ch);
      return -1;
    }
  return door;
}

void
do_open (CHAR_DATA * ch, char *argy)
{
  char arg[SML_LENGTH];
  SINGLE_OBJECT *obj;
  int door;
  DEFINE_COMMAND ("open", do_open, POSITION_RESTING, 0, LOG_NORMAL, "Allows you to open a door or object.")

    argy = one_argy (argy, arg);
  if (!str_cmp ("the", arg))
    argy = one_argy (argy, arg);
  if (arg[0] == '\0')
    {
      send_to_char ("Open what?\n\r", ch);
      return;
    }
  if ((door = find_door (ch, arg)) >= 0)
    {
      ROOM_DATA *to_room;
      EXIT_DATA *pexit;
      EXIT_DATA *pexit_rev;
      pexit = ch->in_room->exit[door];
      if (pexit->d_info)
	{
	  if (!IS_SET (pexit->d_info->exit_info, EX_CLOSED))
	    {
	      send_to_char ("It's already open.\n\r", ch);
	      return;
	    }
	  if (IS_SET (pexit->d_info->exit_info, EX_LOCKED))
	    {
	      send_to_char ("It's locked.\n\r", ch);
	      return;
	    }
	  REMOVE_BIT (pexit->d_info->exit_info, EX_CLOSED);
	  act ("$n opens the $T.", ch, NULL, pexit->d_info->keyword, TO_ROOM);
	  act ("You open the $T.", ch, NULL, pexit->d_info->keyword, TO_CHAR);
	  if ((to_room = pexit->to_room) != NULL
	      && (pexit_rev = to_room->exit[rev_dir[door]]) != NULL
	      && pexit_rev->to_room == ch->in_room && pexit_rev->d_info)
	    {
	      CHAR_DATA *rch;
	      REMOVE_BIT (pexit_rev->d_info->exit_info, EX_CLOSED);
	      if (to_room->more)
		for (rch = to_room->more->people; rch != NULL; rch = rch->next_in_room)
		  act ("The $T is opened from the other side.", rch, NULL, pexit_rev->d_info->keyword, TO_CHAR);
   /*
	  if (pexit_rev->d_info && IS_SET(pexit_rev->d_info->exit_info,EX_HASTRAP))
	    {
	      CHAR_DATA *trapmob;
	      CHAR_DATA *fch;
	      SPELL_DATA *spl;
	      int dam;
	      trapmob = create_mobile(get_mob_index(2));
	      char_to_room(trapmob, ch->in_room);
	      act("Uh oh, that $T that $n opened was trapped!!!", ch, NULL, pexit->d_info->keyword, TO_ROOM);
	      check_room_more(ch->in_room);
	      for(fch = ch->in_room->more->people; fch != NULL; fch = fch->next_in_room)
		{
		  dam = number_range(5,10) * number_range(5,10);
if (fch == trapmob) continue;
		  act("The force of the trap hits $N!", trapmob, NULL, fch, TO_NOTVICT);
		  act("The trap hits You!", trapmob, NULL, fch, TO_VICT);
		  if ((spl = skill_lookup(NULL, (number_range(61,114)))) != NULL && number_range(1,3) ==2)
		    {
		      general_spell(spl, 150, trapmob, fch);
		    }
		  else
		    damage(trapmob, fch, dam, gsn_trapset);
		  stop_fighting(trapmob, TRUE);
		}
	      extract_char(trapmob, TRUE);
	      REMOVE_BIT(pexit->d_info->exit_info, EX_HASTRAP); */
	    }





/*

	    }
	  if (pexit->d_info && IS_SET(pexit->d_info->exit_info, EX_HASTRAP))
	    {
	      CHAR_DATA *trapmob;
	      CHAR_DATA *fch;
	      SPELL_DATA *spl;
	      int dam;
	      trapmob = create_mobile(get_mob_index(2));
	      char_to_room(trapmob, ch->in_room);
	      act("Uh oh, that $T that $n opened was trapped!!!", ch, NULL, pexit->d_info->keyword, TO_ROOM);
	      check_room_more(ch->in_room);
	      for(fch = ch->in_room->more->people; fch != NULL; fch = fch->next_in_room)
		{
		  dam = number_range(5,10) * number_range(5,10);
if (fch == trapmob) continue;
		  act("The force of the trap hits $N!", trapmob, NULL, fch, TO_NOTVICT);
		  act("The trap hits You!", trapmob, NULL, fch, TO_VICT);
		  if ((spl = skill_lookup(NULL, (number_range(61,114)))) != NULL && number_range(1,3) ==2)
		    {
		      general_spell(spl, 150, trapmob, fch);
		    }
		  else
		    damage(trapmob, fch, dam, gsn_trapset);
		  stop_fighting(trapmob, TRUE);
		}
	      extract_char(trapmob, TRUE);
	      REMOVE_BIT(pexit->d_info->exit_info, EX_HASTRAP);
	    }
 	*/	  
	  return;
	}
    }
  if ((obj = get_obj_here (ch, arg, SEARCH_INV_FIRST)) != NULL)
    {
      if (obj->pIndexData->item_type != ITEM_CONTAINER)
	{
	  send_to_char ("That's not a container.\n\r", ch);
	  return;
	}
      else
	{
	  I_CONTAINER *con = (I_CONTAINER *) obj->more;
	  if (!IS_SET (con->flags, CONT_CLOSED))
	    {
	      send_to_char ("It's already open.\n\r", ch);
	      return;
	    }
	  if (!IS_SET (con->flags, CONT_CLOSEABLE))
	    {
	      send_to_char ("You can't do that.\n\r", ch);
	      return;
	    }
	  if (IS_SET (con->flags, CONT_LOCKED))
	    {
	      send_to_char ("It's locked.\n\r", ch);
	      return;
	    }
	  REMOVE_BIT (con->flags, CONT_CLOSED);
	  send_to_char ("Ok.\n\r", ch);
	  act ("$n opens $p.", ch, obj, NULL, TO_ROOM);
	  if (IS_SET(con->flags, CONT_HASTRAP))
	    {
	      CHAR_DATA *trapmob;
	      CHAR_DATA *fch;
	      SPELL_DATA *spl;
	      int dam;
	      trapmob = create_mobile(get_mob_index(2));
	      char_to_room(trapmob, ch->in_room);
	      act("Uh oh, that $p that $n opened was trapped", ch, obj, ch, TO_NOTVICT);
	      act("Uh oh, that $p you just opened was trapped!!!", ch, obj, ch, TO_VICT);
	      
	      check_room_more(ch->in_room);
	      for(fch = ch->in_room->more->people; fch != NULL; fch = fch->next_in_room)
		{
		  dam = number_range(5,10) * number_range(5,10);
		  if (fch == trapmob) continue;
		  act("The force of the trap hits $N!", trapmob, NULL, fch, TO_NOTVICT);
		  act("The trap hits You!", trapmob, NULL, fch, TO_VICT);
		  if ((spl = skill_lookup(NULL, (number_range(61,114)))) != NULL && number_range(1,3) == 2)
		    {
		      general_spell(spl, 150, trapmob, fch);
		    }
		  else
		    damage(trapmob, fch, dam, gsn_trapset);
		  stop_fighting(trapmob, TRUE);
		}
	      extract_char(trapmob, TRUE);
	      REMOVE_BIT(con->flags, CONT_HASTRAP);
	    }
	  
	  return;
	}
    }
  if (door == -1)
    act ("I see no $T here.", ch, NULL, arg, TO_CHAR);
  return;
}

void
do_close (CHAR_DATA * ch, char *argy)
{
  char arg[SML_LENGTH];
  SINGLE_OBJECT *obj;
  int door;
  DEFINE_COMMAND ("close", do_close, POSITION_RESTING, 0, LOG_NORMAL, "Allows you to close a door or object.")

    argy = one_argy (argy, arg);
  if (!str_cmp ("the", arg))
    argy = one_argy (argy, arg);
  if (arg[0] == '\0')
    {
      send_to_char ("Close what?\n\r", ch);
      return;
    }
  if ((door = find_door (ch, arg)) >= 0)
    {
      ROOM_DATA *to_room;
      EXIT_DATA *pexit;
      EXIT_DATA *pexit_rev;
      pexit = ch->in_room->exit[door];
      if (pexit->d_info)
	{
	  if (IS_SET (pexit->d_info->exit_info, EX_CLOSED))
	    {
	      send_to_char ("It's already closed.\n\r", ch);
	      return;
	    }
	  if (pexit->d_info->str < 1)
	    {
	      send_to_char ("That door is broken...\n\r", ch);
	      return;
	    }
	  SET_BIT (pexit->d_info->exit_info, EX_CLOSED);
	  act ("$n closes the $T.", ch, NULL, pexit->d_info->keyword, TO_ROOM);
	  send_to_char ("Ok.\n\r", ch);
	  if ((to_room = pexit->to_room) != NULL
	      && (pexit_rev = to_room->exit[rev_dir[door]]) != 0
	      && pexit_rev->to_room == ch->in_room && pexit_rev->d_info)
	    {
	      CHAR_DATA *rch;
	      SET_BIT (pexit_rev->d_info->exit_info, EX_CLOSED);
	      if (to_room->more)
		for (rch = to_room->more->people; rch != NULL; rch = rch->next_in_room)
		  {
		    act ("The $T is closed from the other side.", rch, NULL, pexit_rev->d_info->keyword, TO_CHAR);
		  }
	    }
	  if (pexit->d_info && IS_SET(pexit->d_info->exit_info, EX_HASTRAP))
	    {
	      CHAR_DATA *trapmob;
	      CHAR_DATA *fch;
	      SPELL_DATA *spl;
	      int dam;
	      trapmob = create_mobile(get_mob_index(2));
	      char_to_room(trapmob, ch->in_room);
	      act("Uh oh, that $T that $n opened was trapped!!!\n\r", ch, NULL, pexit->d_info->keyword, TO_ROOM);
	      check_room_more(ch->in_room);
	      for(fch = ch->in_room->more->people; fch != NULL; fch = fch->next_in_room)
		{
		  dam = number_range(5,10) * number_range(5,10);
		  if (fch == trapmob) continue;
		  act("The force of the trap hits $N!", trapmob, NULL, fch, TO_NOTVICT);
		  act("The trap hits You!", trapmob, NULL, fch, TO_VICT);
		  if ((spl = skill_lookup(NULL, (number_range(61,114)))) != NULL)
		    {
		      general_spell(spl, 150, trapmob, fch);
		    }
		  else
		    damage(trapmob, fch, dam, gsn_trapset);
		  stop_fighting(trapmob, TRUE);
		}
	      extract_char(trapmob, TRUE);
	      REMOVE_BIT(pexit->d_info->exit_info, EX_HASTRAP);
	    }
	  
	  return;
	}
    }
  if ((obj = get_obj_here (ch, arg, SEARCH_INV_FIRST)) != NULL)
    {
      if (obj->pIndexData->item_type != ITEM_CONTAINER)
	{
	  send_to_char ("That's not a container.\n\r", ch);
	  return;
	}
      else
	{
	  I_CONTAINER *cnt = (I_CONTAINER *) obj->more;
	  if (IS_SET (cnt->flags, CONT_CLOSED))
	    {
	      send_to_char ("It's already closed.\n\r", ch);
	      return;
	    }
	  if (!IS_SET (cnt->flags, CONT_CLOSEABLE))
	    {
	      send_to_char ("You can't do that.\n\r", ch);
	      return;
	    }
	  SET_BIT (cnt->flags, CONT_CLOSED);
	  send_to_char ("Ok.\n\r", ch);
	  act ("$n closes $p.", ch, obj, NULL, TO_ROOM);
	  return;
	}
    }
  if (door == -1)
    act ("I see no $T here.", ch, NULL, arg, TO_CHAR);
  return;
}

bool
has_key (CHAR_DATA * ch, int key)
{
  SINGLE_OBJECT *obj;
  if (get_obj_index (key) == NULL)
    return FALSE;
  if (!ch)
    return FALSE;
  for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
    if (obj->pIndexData->vnum == key)
      return TRUE;
  if (IS_PLAYER (ch) && IS_SET (ch->act, PLR_HOLYWALK))
    return TRUE;
  return FALSE;
}

void
update_weight_num (CHAR_DATA * ch)
{
  SINGLE_OBJECT *obj;
  if (IS_MOB (ch))
    return;
  if (!ch)
    return;
  ch->pcdata->carry_weight = 0;
  ch->pcdata->carry_number = 0;
  ch->pcdata->command_objs = 0;
  for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
    {
      ch->pcdata->carry_weight += get_obj_weight (obj);
      ch->pcdata->carry_number += get_obj_number (obj);
      if (IS_OBJ_STAT(obj, ITEM_COMMANDSCRIPT))
	ch->pcdata->command_objs++;
      
    }
  return;
}

void
do_lock (CHAR_DATA * ch, char *argy)
{
  char arg[SML_LENGTH];
  SINGLE_OBJECT *obj;
  int door;
  DEFINE_COMMAND ("lock", do_lock, POSITION_RESTING, 0, LOG_NORMAL, "Allows you to lock a door or object.")

    one_argy (argy, arg);
  if (arg[0] == '\0')
    {
      send_to_char ("Lock what?\n\r", ch);
      return;
    }
  if ((obj = get_obj_here (ch, arg, SEARCH_INV_FIRST)) != NULL)
    {
      if (obj->pIndexData->item_type != ITEM_CONTAINER)
	{
	  send_to_char ("That's not a container.\n\r", ch);
	  return;
	}
      else
	{
	  I_CONTAINER *cnt = (I_CONTAINER *) obj->more;
	  if (!IS_SET (cnt->flags, CONT_CLOSED))
	    {
	      send_to_char ("It's not closed.\n\r", ch);
	      return;
	    }
	  if (cnt->key_vnum <= 0)
	    {
	      send_to_char ("It can't be locked.\n\r", ch);
	      return;
	    }
	  if (!has_key (ch, cnt->key_vnum))
	    {
	      send_to_char ("You don't have the key.\n\r", ch);
	      return;
	    }
	  if (IS_SET (cnt->flags, CONT_LOCKED))
	    {
	      send_to_char ("It's already locked.\n\r", ch);
	      return;
	    }
	  SET_BIT (cnt->flags, CONT_LOCKED);
	  send_to_char ("*Click*\n\r", ch);
	  act ("$n locks $p.", ch, obj, NULL, TO_ROOM);
	  return;
	}
    }
  if ((door = find_door (ch, arg)) >= 0)
    {
      ROOM_DATA *to_room;
      EXIT_DATA *pexit;
      EXIT_DATA *pexit_rev;
      pexit = ch->in_room->exit[door];
      if (pexit->d_info)
	{
	  if (!IS_SET (pexit->d_info->exit_info, EX_CLOSED))
	    {
	      send_to_char ("It's not closed.\n\r", ch);
	      return;
	    }
	  if (pexit->d_info->key < 0)
	    {
	      send_to_char ("It can't be locked.\n\r", ch);
	      return;
	    }
	  if (!has_key (ch, pexit->d_info->key))
	    {
	      send_to_char ("You don't have the key.\n\r", ch);
	      return;
	    }
	  if (IS_SET (pexit->d_info->exit_info, EX_LOCKED))
	    {
	      send_to_char ("It's already locked.\n\r", ch);
	      return;
	    }
	  SET_BIT (pexit->d_info->exit_info, EX_LOCKED);
	  send_to_char ("*Click*\n\r", ch);
	  act ("$n locks the $T.", ch, NULL, pexit->d_info->keyword, TO_ROOM);
	  if ((to_room = pexit->to_room) != NULL
	      && (pexit_rev = to_room->exit[rev_dir[door]]) != 0
	      && pexit_rev->to_room == ch->in_room && pexit_rev->d_info)
	    {
	      CHAR_DATA *rch;
	      SET_BIT (pexit_rev->d_info->exit_info, EX_LOCKED);
	      if (to_room->more)
		for (rch = to_room->more->people; rch != NULL; rch = rch->next_in_room)
		  act ("*Click* The $T is locked from the other side.", rch, NULL, pexit_rev->d_info->keyword, TO_CHAR);
	    }
	}
    }
  return;
}

void
do_unlock (CHAR_DATA * ch, char *argy)
{
  char arg[SML_LENGTH];
  SINGLE_OBJECT *obj;
  int door;
  DEFINE_COMMAND ("unlock", do_unlock, POSITION_RESTING, 0, LOG_NORMAL, "Allows you to unlock a door or object.")

    one_argy (argy, arg);
  if (arg[0] == '\0')
    {
      send_to_char ("Unlock what?\n\r", ch);
      return;
    }
  if ((obj = get_obj_here (ch, arg, SEARCH_INV_FIRST)) != NULL)
    {
      if (obj->pIndexData->item_type != ITEM_CONTAINER)
	{
	  send_to_char ("That's not a container.\n\r", ch);
	  return;
	}
      else
	{
	  I_CONTAINER *cnt = (I_CONTAINER *) obj->more;
	  if (!IS_SET (cnt->flags, CONT_CLOSED))
	    {
	      send_to_char ("It's not closed.\n\r", ch);
	      return;
	    }
	  if (cnt->key_vnum <= 0)
	    {
	      send_to_char ("It can't be unlocked.\n\r", ch);
	      return;
	    }
	  if (!has_key (ch, cnt->key_vnum))
	    {
	      send_to_char ("You don't have the key.\n\r", ch);
	      return;
	    }
	  if (!IS_SET (cnt->flags, CONT_LOCKED))
	    {
	      send_to_char ("It's already unlocked.\n\r", ch);
	      return;
	    }
	  REMOVE_BIT (cnt->flags, CONT_LOCKED);
	  act ("$n unlocks $p.", ch, obj, NULL, TO_ROOM);
	  act ("You unlock $p.", ch, obj, NULL, TO_CHAR);
	  return;
	}
    }
  if ((door = find_door (ch, arg)) >= 0)
    {
      ROOM_DATA *to_room;
      EXIT_DATA *pexit;
      EXIT_DATA *pexit_rev;
      pexit = ch->in_room->exit[door];
      if (pexit->d_info)
	{
	  if (!IS_SET (pexit->d_info->exit_info, EX_CLOSED))
	    {
	      send_to_char ("It's not closed.\n\r", ch);
	      return;
	    }
	  if (pexit->d_info->key < 1)
	    {
	      send_to_char ("It can't be unlocked.\n\r", ch);
	      return;
	    }
	  if (!has_key (ch, pexit->d_info->key))
	    {
	      send_to_char ("You don't have the key.\n\r", ch);
	      return;
	    }
	  if (!IS_SET (pexit->d_info->exit_info, EX_LOCKED))
	    {
	      send_to_char ("It's already unlocked.\n\r", ch);
	      return;
	    }
	  REMOVE_BIT (pexit->d_info->exit_info, EX_LOCKED);
	  act ("You unlock the $T.", ch, NULL, pexit->d_info->keyword, TO_CHAR);
	  act ("$n unlocks the $T.", ch, NULL, pexit->d_info->keyword, TO_ROOM);
	  if ((to_room = pexit->to_room) != NULL
	      && (pexit_rev = to_room->exit[rev_dir[door]]) != NULL
	      && pexit_rev->to_room == ch->in_room && pexit_rev->d_info)
	    {
	      REMOVE_BIT (pexit_rev->d_info->exit_info, EX_LOCKED);
	    }
	}
    }
  if (door == -1)
    act ("I see no $T here.", ch, NULL, arg, TO_CHAR);
  return;
}

void
do_pick (CHAR_DATA * ch, char *argy)
{
  SINGLE_OBJECT *lockpicks;
  char arg[SML_LENGTH];
  CHAR_DATA *gch;
  SINGLE_OBJECT *obj;
  int door;
  DEFINE_COMMAND ("pick", do_pick, POSITION_STANDING, 0, LOG_NORMAL, "Attempts to pick a door or object's lock.  Based on Pick skill.")
    if (IS_MOB(ch))
      return;
    one_argy (argy, arg);
  if (arg[0] == '\0')
    {
      send_to_char ("Pick what?\n\r", ch);
      return;
    }
  WAIT_STATE (ch, 28);
  lockpicks = get_item_held(ch, ITEM_TOOL);
  if (lockpicks == NULL || ((I_TOOL *) lockpicks->more)->tool_type != TOOL_LOCKPICK)
    {
      send_to_char("You must be using LOCKPICKS to pick a lock!\n\r", ch);
      return;
    }
  for (gch = ch->in_room->more->people; gch; gch = gch->next_in_room)
    {
      if (IS_MOB (gch) && IS_AWAKE (gch) && IS_SET (gch->act, ACT_SENTINEL))
	{
	  act ("$N is standing too close to the lock; you can't pick that door now!",
	       ch, NULL, gch, TO_CHAR);
	  return;
	}
    }
  if (!use_tool(lockpicks, TOOL_LOCKPICK))
    {
      send_to_char("Your lockpicks seem to have broken!\n\r", ch);
    }
  if (!is_member(ch, GUILD_THIEFG) && number_range(1,110) > ch->pcdata->learned[gsn_pick_lock])
    {
      send_to_char ("You failed.\n\r", ch);
      return;
    }
  if ((door = find_door (ch, arg)) >= 0)
    {
      ROOM_DATA *to_room;
      EXIT_DATA *pexit;
      EXIT_DATA *pexit_rev;
      pexit = ch->in_room->exit[door];
      if (pexit->d_info)
	{
	  if (!IS_SET (pexit->d_info->exit_info, EX_CLOSED))
	    {
	      send_to_char ("It's not closed.\n\r", ch);
	      return;
	    }
	  if (pexit->d_info->key < 0 && !is_member(ch, GUILD_THIEFG))
	    {
	      send_to_char ("It can't be picked.\n\r", ch);
	      return;
	    }
	  if (!IS_SET (pexit->d_info->exit_info, EX_LOCKED))
	    {
	      send_to_char ("It's already unlocked.\n\r", ch);
	      return;
	    }
	  if (IS_SET (pexit->d_info->exit_info, EX_PICKPROOF))
	    if (!(is_member(ch, GUILD_THIEFG) && number_range(1,50) ==1))
	      {
		if (number_range (1, 4) == 2)
		  skill_gain (ch, gsn_pick_lock, FALSE);
		send_to_char ("You failed.\n\r", ch);
		return;
	      }
	  REMOVE_BIT (pexit->d_info->exit_info, EX_LOCKED);
	  act ("You pick the $T.", ch, NULL, pexit->d_info->keyword, TO_CHAR);
	  act ("$n picks the $T.", ch, NULL, pexit->d_info->keyword, TO_ROOM);
	  skill_gain (ch, gsn_pick_lock, FALSE);
	  if ((to_room = pexit->to_room) != NULL
	      && (pexit_rev = to_room->exit[rev_dir[door]]) != NULL
	      && pexit_rev->to_room == ch->in_room && pexit_rev->d_info)
	    {
	      REMOVE_BIT (pexit_rev->d_info->exit_info, EX_LOCKED);
	    }
	}
    }
  if ((obj = get_obj_here (ch, arg, SEARCH_INV_FIRST)) != NULL)
    {
      if (obj->pIndexData->item_type != ITEM_CONTAINER)
	{
	  send_to_char ("That's not a container.\n\r", ch);
	  return;
	}
      else
	{
	  I_CONTAINER *cnt = (I_CONTAINER *) obj->more;
	  if (!IS_SET (cnt->flags, CONT_CLOSED))
	    {
	      send_to_char ("It's not closed.\n\r", ch);
	      return;
	    }
	  if (cnt->key_vnum <= 0)
	    {
	      send_to_char ("It can't be unlocked.\n\r", ch);
	      return;
	    }
	  if (!IS_SET (cnt->flags, CONT_LOCKED))
	    {
	      send_to_char ("It's already unlocked.\n\r", ch);
	      return;
	    }
	  if (IS_SET (cnt->flags, CONT_PICKPROOF))
	    {
	      send_to_char ("You failed.\n\r", ch);
	      return;
	    }
	  REMOVE_BIT (cnt->flags, CONT_LOCKED);
	  act ("You pick $p.", ch, obj, NULL, TO_CHAR);
	  act ("$n picks $p.", ch, obj, NULL, TO_ROOM);
	  return;
	}
    }
  if (door == -1)
    act ("I see no $T here.", ch, NULL, arg, TO_CHAR);
  return;
}

void
do_stand (CHAR_DATA * ch, char *argy)
{
  AFFECT_DATA *paf;
  DEFINE_COMMAND ("stand", do_stand, POSITION_SLEEPING, 0, LOG_NORMAL, "Makes you stand up, from sleeping, resting, etc...")

    if (RIDING (ch) != NULL)
    {
      send_to_char ("If you wanted to stand on your own two feet you would not be riding!\n\r", ch);
      return;
    }
  if (ch->fgt && ch->fgt->ears > 9)
    {
      send_to_char ("You are bashed, and can't stand yet...\n\r", ch);
      return;
    }
  switch (ch->position)
    {
    case POSITION_SLEEPING:
      if (IS_AFFECTED (ch, AFF_SLEEP))
	{
	  send_to_char ("You can't wake up!\n\r", ch);
	  return;
	}
      send_to_char ("You wake and stand up.\n\r", ch);
      act ("$n wakes and stands up.", ch, NULL, NULL, TO_ROOM);
      NEW_POSITION(ch, POSITION_STANDING);
      break;
    case POSITION_BASHED:
      check_fgt (ch);
      act ("You quickly stand, trying to avoid getting hit!", ch, NULL, NULL, TO_CHAR);
      act ("$n quickly stands, trying not to get hit in the process!", ch, NULL, NULL, TO_ROOM);
      if (FIGHTING (ch) == NULL)
	NEW_POSITION(ch, POSITION_STANDING);
      else
	NEW_POSITION(ch, POSITION_FIGHTING);
      ch->wait = 0;
      ch->fgt->combat_delay_count = -5;
      break;
    case POSITION_GROUNDFIGHTING:
      if ((number_range (1, (pow.groundfight_stand + ((ch->ced->ground_pos
== FP_CHOKE_1 || ch->ced->ground_pos == FP_CHOKE_2 || ch->ced->ground_pos == FP_CHOKE_3) ? 1: 0))) != 1) && 
	  (ch->fgt) &&
	  (ch->fgt->fighting) &&
	  (IS_MOB(FIGHTING(ch)) || 
	   (IS_PLAYER(FIGHTING(ch)) &&   
	    ((FIGHTING(ch)->pcdata->learned[gsn_grapple] == -100 ? FALSE :
	      (number_range(1,180) <
	       ch->fgt->fighting->pcdata->learned[gsn_grapple]))))))
	{
	  send_to_char ("You try to stand, but your opponent won't let you!\n\r", ch);
	  WAIT_STATE (ch, PULSE_VIOLENCE);
	  return;
	}
      else
	{
	  send_to_char ("You manage to wiggle free and stand up!\n\r", ch);
	  NEW_POSITION(ch, POSITION_FIGHTING);
	  if (FIGHTING (ch) != NULL)
	    NEW_POSITION(FIGHTING (ch), POSITION_FIGHTING);
	}
      break;
    case POSITION_RESTING:
      send_to_char ("You stand up.\n\r", ch);
      act ("$n stands up.", ch, NULL, NULL, TO_ROOM);
      if (!IS_AFFECTED (ch, AFF_FLYING))
	{
	  if (ch->in_room && ((!IS_SET(ch->in_room->room_flags,ROOM_INDOORS) && !IS_SET(ch->in_room->room_flags, ROOM_UNDERGROUND)) || ch->height<= INDOOR_FLY_HEIGHT) && IS_SET (ch->pcdata->nat_abilities, AFF_FLYING))
	    {
	      if (IS_AFFECTED (ch, AFF_SNEAK) && (!is_member (ch, GUILD_THIEFG) || !is_member(ch, GUILD_ROGUE)))
		{
		  send_to_char ("You stop sneaking.\n\r", ch);
		  affect_strip (ch, gsn_sneak);
		} 
	      SET_BIT (ch->affected_by, AFF_FLYING);
	      send_to_char ("You begin to fly again!\n\r", ch);
	      act ("$n floats gently up in the air.", ch, NULL, NULL, TO_ROOM);
	    }
	  if (ch->in_room && ((!IS_SET(ch->in_room->room_flags,ROOM_INDOORS) && !IS_SET(ch->in_room->room_flags, ROOM_UNDERGROUND)) || ch->height<= INDOOR_FLY_HEIGHT)) { 
	    for (paf = ch->affected; paf != NULL; paf = paf->next)
	      {
		if (paf->bitvector != AFF_FLYING)
		  continue;
		else if (paf->duration > 0)
		  {
		    if (IS_AFFECTED (ch, AFF_SNEAK) && (!is_member (ch, GUILD_THIEFG) || !is_member(ch, GUILD_ROGUE)))
		      {
			send_to_char ("You stop sneaking.\n\r", ch);
			affect_strip (ch, gsn_sneak);
		      } 
		    SET_BIT (ch->affected_by, AFF_FLYING);
		    send_to_char ("You begin to fly again!\n\r", ch);
		    act ("$n floats gently up in the air.", ch, NULL, NULL, TO_ROOM);
		  }
	      }
	  }
	}
      NEW_POSITION(ch, POSITION_STANDING);
      break;
    case POSITION_STANDING:
      send_to_char ("You are already standing.\n\r", ch);
      break;
    case POSITION_MEDITATING:
      send_to_char ("You stand up, and stop meditating.\n\r", ch);
      act ("$n stands up and stops meditating.", ch, NULL, NULL, TO_ROOM);
      NEW_POSITION(ch, POSITION_STANDING);
      break;
    case POSITION_FIGHTING:
      send_to_char ("You are already fighting!\n\r", ch);
      break;
    case POSITION_FALLING:
      send_to_char ("You can't! You're falling!\n\r", ch);
      break;
    }
  return;
}

void
do_rest (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("rest", do_rest, POSITION_RESTING, 0, LOG_NORMAL, "Rest makes your character sit and rest.")

    if (RIDING (ch) != NULL)
    {
      send_to_char ("You can't rest while you are riding! Dismount first.\n\r", ch);
      return;
    }
  if (IS_AFFECTED (ch, AFF_FLYING))
    {
      do_land (ch, "");
    }

  switch (ch->position)
    {
    case POSITION_FALLING:
      send_to_char ("You are falling downward.\n\r", ch);
      break;
    case POSITION_SLEEPING:
      send_to_char ("You are already sleeping.\n\r", ch);
      break;
    case POSITION_RESTING:
      send_to_char ("You are already resting.\n\r", ch);
      break;
    case POSITION_MEDITATING:
      send_to_char ("You stop meditating and begin to rest.\n\r", ch);
      act ("$n stops meditating and rests.", ch, NULL, NULL, TO_ROOM);
      NEW_POSITION(ch, POSITION_RESTING);
      break;
    case POSITION_STANDING:
      send_to_char ("You rest.\n\r", ch);
      act ("$n rests.", ch, NULL, NULL, TO_ROOM);
      NEW_POSITION(ch, POSITION_RESTING);
      break;
    case POSITION_FIGHTING:
      send_to_char ("You are already fighting!\n\r", ch);
      break;
    }
  return;
}
void
do_sleep (CHAR_DATA * ch, char *argy)
{
  int warmth;
  DEFINE_COMMAND ("sleep", do_sleep, POSITION_SLEEPING, 0, LOG_NORMAL, "Sleep allows you to sleep and recover moves and hps.")

    if (RIDING (ch) != NULL)
    {
      send_to_char ("It might be hazardous while riding. Dismount first.\n\r", ch);
      return;
    }
  if (IS_AFFECTED (ch, AFF_FLYING))
    {
      do_land (ch, "");
    }
  if (FIGHTING (ch) && FIGHTING (ch) != NULL)
    {
      send_to_char ("But you're in the middle of combat.. it wouldn't be to your benefit!\n\r", ch);
      return;
    }
  if (pow.weather && IS_PLAYER (ch) && ch->in_room &&
      (ch->in_room->sector_type !=SECT_INSIDE) && (IS_OUTSIDE(ch) ||
ch->in_room->sector_type < SECT_CAVE))
    {
      if ((!is_member(ch, GUILD_RANGER) && 
	  (cur_temp(ch) + ch->pcdata->warmth < 40)) || 
	 (is_member(ch, GUILD_RANGER) && 
	(cur_temp(ch) + ch->pcdata->warmth < 30)))
	{
	  send_to_char ("You try to lie down to sleep, but you are just too cold! brrr...\n\r", ch);
	  return;
	}
    }
  switch (ch->position)
    {
    case POSITION_FALLING:
      send_to_char ("You are falling downward.\n\r", ch);
      break;
    case POSITION_SLEEPING:
      send_to_char ("You are already sleeping.\n\r", ch);
      break;
    case POSITION_MEDITATING:
      send_to_char ("You stop meditating and go to sleep.\n\r", ch);
      act ("$n stops meditating and falls asleep.", ch, NULL, NULL, TO_ROOM);
      NEW_POSITION(ch, POSITION_SLEEPING);
      break;
    case POSITION_RESTING:
    case POSITION_STANDING:
      send_to_char ("You sleep.\n\r", ch);
      act ("$n sleeps.", ch, NULL, NULL, TO_ROOM);
      NEW_POSITION(ch, POSITION_SLEEPING);
      break;
    case POSITION_FIGHTING:
      send_to_char ("You are fighting!\n\r", ch);
      break;
    }
  return;
}



void
do_wake (CHAR_DATA * ch, char *argy)
{
  char arg[SML_LENGTH];
  CHAR_DATA *victim;
  DEFINE_COMMAND ("wake", do_wake, POSITION_SLEEPING, 0, LOG_NORMAL, "This command allows you to wake up from sleep.")

    one_argy (argy, arg);
  if (arg[0] == '\0')
    {
      do_stand (ch, argy);
      return;
    }
  if (!IS_AWAKE (ch))
    {
      send_to_char ("You are asleep yourself!\n\r", ch);
      return;
    }
  if ((victim = get_char_room (ch, arg)) == NULL)
    {
      send_to_char ("They aren't here.\n\r", ch);
      return;
    }
  if (IS_AWAKE (victim))
    {
      act ("$N is already awake.", ch, NULL, victim, TO_CHAR);
      return;
    }
  if (IS_AFFECTED (victim, AFF_SLEEP))
    {
      act ("You can't wake $M!", ch, NULL, victim, TO_CHAR);
      return;
    }
  if (victim->hit < 1)
    {
      send_to_char ("That character is way too injured!\n\r", ch);
      return;
    }
  NEW_POSITION(victim, POSITION_STANDING);
  act ("You wake $M.", ch, NULL, victim, TO_CHAR);
  act ("$n wakes you.", ch, NULL, victim, TO_VICT);
  return;
}
void
do_sneak (CHAR_DATA * ch, char *argy)
{
  AFFECT_DATA af;
  bool sneakal;
  DEFINE_COMMAND ("sneak", do_sneak, POSITION_STANDING, 0, LOG_NORMAL, "Allows you to sneak.  This is based on Sneak skill.  Skill is checked each step; if you fail, people will see you try to 'sneak' into the room.  If you succeed, your entry will not be noticed.")
    bzero (&af, sizeof (af));
  sneakal = TRUE;
  if (RIDING (ch) != NULL)
    {
      send_to_char ("It's tough to sneak while riding, and won't do any good.\n\r", ch);
      return;
    }
  if (!IS_AFFECTED (ch, AFF_SNEAK))
    sneakal = FALSE;
  if (IS_AFFECTED (ch, AFF_FLYING))
      {
	if(is_member(ch, GUILD_ROGUE) && is_member (ch, GUILD_THIEFG)) /* hf ASN little extra bonus - JRAJRA */
	  {
	    send_to_char("Entering whisper flight mode...\n\r", ch);
	  }
	else
	  {
	    send_to_char ("You're too obvious, being in the air.\n\r", ch);
	    return;
	  }
      }
  send_to_char ("You attempt to move silently.\n\r", ch);
  affect_strip (ch, gsn_sneak);
  if (IS_MOB (ch) || number_percent () < ch->pcdata->learned[gsn_sneak] + ch->pcdata->plus_sneak)
    {
      af.type = gsn_sneak;
      af.duration = LEVEL (ch);
      af.location = APPLY_NONE;
      af.modifier = 0;
      af.bitvector = AFF_SNEAK;
      affect_to_char (ch, &af);
    }
  return;
}

void
do_hide (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("hide", do_hide, POSITION_STANDING, 0, LOG_NORMAL, "Allows you to hide, based on skill.  People with the search skill can search for you, with some luck.")

    if (RIDING (ch) != NULL)
    {
      send_to_char ("Hide while riding! I don't think so...\n\r", ch);
      return;
    }
  if (ch->position != POSITION_STANDING && ch->position != POSITION_RESTING)
    {
      send_to_char ("You must be standing or resting to hide.\n\r", ch);
      return;
    }
  if (argy[0] == '\0')
    {
      send_to_char ("You attempt to hide.\n\r", ch);
      if (IS_AFFECTED (ch, AFF_HIDE))
	REMOVE_BIT (ch->affected_by, AFF_HIDE);
      if (!IN_BATTLE (ch) && (IS_MOB (ch) ||
			      number_range (1, 104) <= ch->pcdata->learned[gsn_hide] + ch->pcdata->plus_hide))
	SET_BIT (ch->affected_by, AFF_HIDE);
      WAIT_STATE (ch, 2 * PULSE_VIOLENCE);
      return;
    }
  check_fgt (ch);
  do_drop (ch, argy);
  return;
}


void
do_visible (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("visible", do_visible, POSITION_SLEEPING, 0, LOG_NORMAL, "This command cancels all effects of invisibility, sneaking, etc...")

    affect_strip_bits (ch, AFF_INVISIBLE);
  affect_strip_bits (ch, AFF_HIDE);
  affect_strip_bits (ch, AFF_SNEAK);
  send_to_char ("Ok.\n\r", ch);
  return;
}



void
do_fly (CHAR_DATA * ch, char *argy)
{
  AFFECT_DATA *paf;
  DEFINE_COMMAND ("fly", do_fly, POSITION_DEAD, 0, LOG_NORMAL, "If you can, this makes you fly into the air.")
    if (ch->in_room && (IS_SET(ch->in_room->room_flags , ROOM_INDOORS) || IS_SET (ch->in_room->room_flags,ROOM_UNDERGROUND)) && (ch->height > INDOOR_FLY_HEIGHT)) 
      {
	send_to_char("You are indoors; you can't fly in here!\n\r",ch);
	return;
      }
  if (ch->position!=POSITION_STANDING) 
    {
       
      send_to_char("You must be in a standing position to begin flying.\n\r",ch);
      return;
    }
  if (IS_AFFECTED (ch, AFF_FLYING))
    {
      send_to_char ("You're already flying!\n\r", ch);
      return;
    }
  if (IS_PLAYER (ch) && (IS_SET (ch->pcdata->nat_abilities, AFF_FLYING)))
    {
      if (RIDING (ch) != NULL)
	{
	  send_to_char ("Not while you are riding!\n\r", ch);
	  return;
	}
      if (IS_AFFECTED (ch, AFF_SNEAK))
	{ 
	  if(is_member (ch, GUILD_ROGUE) && is_member(ch, GUILD_THIEFG)) /* JRAJRA - Let Thf/ Asn sneak while flying :) */
	    {
	      send_to_char("Entering whisper flight mode...\n\r", ch);
	    }
	  else
	    {
	      send_to_char ("You cannot sneak while flying. You stop sneaking.\n\r", ch);
	      affect_strip (ch, gsn_sneak);
	    }
	}
      SET_BIT (ch->affected_by, AFF_FLYING);
      send_to_char ("You begin to fly again!\n\r", ch);
      act ("$n floats gently up in the air.", ch, NULL, NULL, TO_ROOM);
      return;
    }
  for (paf = ch->affected; paf != NULL; paf = paf->next)
    {
      if (paf->bitvector != AFF_FLYING)
	continue;
      if (RIDING (ch) != NULL)
	{
	  send_to_char ("Not while you are riding!\n\r", ch);
	  return;
	}
      if (IS_AFFECTED (ch, AFF_SNEAK))
	{
	  if(is_member (ch, GUILD_ROGUE) && is_member(ch, GUILD_THIEFG)) /* JRAJRA - Let Thf/ Asn sneak while flying :) */
	    {
	      send_to_char("Entering whisper flight mode...\n\r", ch);
	    }
	  else
	    {
	      send_to_char ("You cannot sneak while flying. You stop sneaking.\n\r", ch);
	      affect_strip (ch, gsn_sneak);
	    }
	}
      SET_BIT (ch->affected_by, AFF_FLYING);
      send_to_char ("You begin to fly again!\n\r", ch);
      act ("$n floats gently up in the air.", ch, NULL, NULL, TO_ROOM);
      return;
    }
  
  
  send_to_char ("You flap your arms, but never leave the ground.\n\r", ch);
  act ("$n flaps $s arms trying to fly, but can't.\n\r",
       ch, NULL, NULL, TO_ROOM);
  return;
}

void
do_land (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("land", do_land, POSITION_DEAD, 0, LOG_NORMAL, "Makes you land on the ground if you are flying.")

    if (!IS_AFFECTED (ch, AFF_FLYING))
    {
      send_to_char ("But you're not flying!\n\r", ch);
      return;
    }
  send_to_char ("You gently float downward to the ground.\n\r", ch);
  REMOVE_BIT (ch->affected_by, AFF_FLYING);
  if (!ch->in_room) return;

  while (VALID_EDGE (ch->in_room, DIR_DOWN)
	 && ch->in_room->sector_type == SECT_AIR)
    {
      ROOM_DATA *to_room;
      to_room = ch->in_room->exit[DIR_DOWN]->to_room;
      act ("$n descends downward.", ch, NULL, NULL, TO_ROOM);
      char_from_room (ch);
      char_to_room (ch, to_room);
      do_look (ch, "auto");
      act ("$n descends from above.", ch, NULL, NULL, TO_ROOM);
    }
  return;
}


void
do_break (CHAR_DATA * ch, char *argy)
{
  char arg[SML_LENGTH];
  int door;
  DEFINE_COMMAND ("break", do_break, POSITION_STANDING, 0, LOG_NORMAL, "Break <direction> or Break <doorname> allows you to attempt to charge into a door, breaking it.  Based on strength.")

    one_argy (argy, arg);
  if (arg[0] == '\0')
    {
      send_to_char ("Which door do you want to break?\n\r", ch);
      return;
    }
  if (ch->position != POSITION_STANDING)
    {
      send_to_char ("You have to be standing to charge a door!\n\r", ch);
      return;
    }
  if ((door = find_door (ch, arg)) >= 0)
    {
      /* 'open door' */
      ROOM_DATA *to_room;
      EXIT_DATA *pexit;
      EXIT_DATA *pexit_rev;
      int dam, broke = 0;
      char buf[STD_LENGTH];
      pexit = ch->in_room->exit[door];
      if (pexit->d_info)
	{
	  if (!IS_SET (pexit->d_info->exit_info, EX_CLOSED))
	    {
	      send_to_char ("It's already open.\n\r", ch);
	      return;
	    }
	  if (pexit->d_info->str < 0)
	    {
	      send_to_char ("It's already broken.\n\r", ch);
	      return;
	    }
	  if (door >= 0 && door <= 4)
	    {
	      act ("You charge towards the $T...", ch, NULL, pexit->d_info->keyword, TO_CHAR);
	      act ("$n charges towards the $T...", ch, NULL, pexit->d_info->keyword, TO_ROOM);
	    }
	  else if (door == 5)
	    {
	      act ("You stomp on the $T...", ch, NULL, pexit->d_info->keyword, TO_CHAR);
	      act ("$n stomp on the $T...", ch, NULL, pexit->d_info->keyword, TO_ROOM);
	    }
	  else if (door == 6)
	    {
	      act ("You pound on the $T...", ch, NULL, pexit->d_info->keyword, TO_CHAR);
	      act ("$n pound on the $T...", ch, NULL, pexit->d_info->keyword, TO_ROOM);
	    }
	  dam = (get_curr_str (ch) * 2 - pexit->d_info->str) / 2;
          if (IS_AUGMENTED(ch, AUG_ARM_POWER)) dam +=3;
          if (IS_AUGMENTED(ch, AUG_BODY_POWER)) dam +=1;
          if (IS_AUGMENTED(ch, AUG_ARM_POWER)) dam +=1;
          if (IS_AUGMENTED(ch, AUG_HAND_POWER)) dam +=1;
	  if (IS_SET (pexit->d_info->exit_info, EX_BASHPROOF))
	    {
	      pexit->d_info->str = 100;
	      dam = 0;
	    }
	  if (dice (3, 6) < get_curr_str (ch) && dam > 0)
	    {
	      pexit->d_info->str -= number_range (1, get_curr_str (ch)/5);
	      if ((to_room = pexit->to_room) != NULL
		  && (pexit_rev = to_room->exit[rev_dir[door]]) != NULL
		  && pexit_rev->to_room == ch->in_room && pexit_rev->d_info)
		{
		  pexit_rev->d_info->str = pexit->d_info->str;
		}
	      if (pexit->d_info->str > 0)
		{
		  sprintf (buf, "SLAM! The $T %s",
			   door_cond (pexit->d_info->str));
		}
	      else
		sprintf (buf, "SLAM!");
	      act (buf, ch, NULL, pexit->d_info->keyword, TO_CHAR);
	      act (buf, ch, NULL, pexit->d_info->keyword, TO_ROOM);
	    }
	  else
	    {
	      send_to_char ("Slam! Ouch! That hurt!\n\r", ch);
	      sprintf (buf, "Slam! $n cringes in pain!");
	      act (buf, ch, NULL, NULL, TO_ROOM);
	      if (dam <= 0)
		dam = dice (1, pexit->d_info->str / 3);
	      if (dam >= ch->hit)
		ch->hit = 1;
	      else
		SUBHIT(ch, dam);
	    }
	  if (pexit->d_info->str < 0)
	    {
	      REMOVE_BIT (pexit->d_info->exit_info, EX_CLOSED);
	      REMOVE_BIT (pexit->d_info->exit_info, EX_ISDOOR);
	      act ("The $T is reduced to rubble!", ch, NULL, pexit->d_info->keyword, TO_ROOM);
	      act ("You reduced the $T to rubble!", ch, NULL, pexit->d_info->keyword, TO_CHAR);
	      broke = 1;
	    }
	  if (pexit->d_info && IS_SET(pexit->d_info->exit_info, EX_HASTRAP))
	    {
	      CHAR_DATA *trapmob;
	      CHAR_DATA *fch;
	      SPELL_DATA *spl;
	      int dam;
	      trapmob = create_mobile(get_mob_index(2));
	      char_to_room(trapmob, ch->in_room);
	      act("Uh oh, that $T that $n opened was trapped!!!", ch, NULL, pexit->d_info->keyword, TO_ROOM);
	      check_room_more(ch->in_room);
	      for(fch = ch->in_room->more->people; fch != NULL; fch = fch->next_in_room)
		{
		  dam = number_range(5,10) * number_range(5,10);
		  if (fch == trapmob) continue;
		  act("The force of the trap hits $N!", trapmob, NULL, fch, TO_NOTVICT);
		  act("The trap hits You!", trapmob, NULL, fch, TO_VICT);
		  if ((spl = skill_lookup(NULL, (number_range(61,114)))) != NULL && number_range(1,3) ==2)
		    {
		      general_spell(spl, 150, trapmob, fch);
		    }
		  else
		    damage(trapmob, fch, dam, gsn_trapset);
		  stop_fighting(trapmob, TRUE);
		}
	      extract_char(trapmob, TRUE);
	    }
	  WAIT_STATE(ch, 8);
	  if (pexit->d_info->str > 0)
	    return;
	  /* open the other side */
	  if ((to_room = pexit->to_room) != NULL
	      && (pexit_rev = to_room->exit[rev_dir[door]]) != NULL
	      && pexit_rev->to_room == ch->in_room && pexit_rev->d_info)
	    {
	      REMOVE_BIT (pexit_rev->d_info->exit_info, EX_CLOSED);
	      if (broke)
		REMOVE_BIT (pexit_rev->d_info->exit_info, EX_ISDOOR);
	    }
	}
      else
	{
	  send_to_char ("Which door was that?\n\r", ch);
	  return;
	}
    }
  return;
}






char *
door_cond (int val)
{
  static char door_condit[100];
  if (val > 80)
    strcpy (door_condit, "looks the same as before.");
  else if (val > 70)
    strcpy (door_condit, "seems to be weakening a little.");
  else if (val > 55)
    strcpy (door_condit, "has a visible crack now.");
  else if (val > 39)
    strcpy (door_condit, "is concave from the impacts!");
  else if (val > 25)
    strcpy (door_condit, "won't stay standing much longer!");
  else if (val > 14)
    strcpy (door_condit, "looks like it could break any second!");
  else
    strcpy (door_condit, "What door?");
  return door_condit;
}
