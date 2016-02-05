#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emlen.h"

int FORCE_LEVEL;
extern bool noibm;
extern bool kk;
extern bool mb;
extern bool dbs;


ROOM_DATA *find_location (CHAR_DATA * ch, char *arg);
void gotoxy (CHAR_DATA *, int, int);
void strip_all_affects (CHAR_DATA *);





void
flags (CHAR_DATA * ch, char *argy)
{
  CHAR_DATA *vict;
  int col = 0;
  int i;
  DEFINE_COMMAND ("flags", flags, POSITION_DEAD, MAX_LEVEL, LOG_NORMAL, "Shows questflags on a given player.")
    hugebuf_o[0] = '\0';
  if ((vict = get_player_world (ch, argy, FALSE)) == NULL)
    {
      send_to_char ("That player was not found or is not online.\n\r", ch);
      return;
    }
  for (i = 0; i < MAX_SCRIPT_FLAGS; i++)
    {
      if (!vict->pcdata->script_flags[i] || vict->pcdata->script_flags[i][0] == '\0')
        continue;
      col++;
      if (col == 8)
        {
          strcat (hugebuf_o, "\n\r");
          col = 1;
        }
      sprintf (hugebuf_o + strlen (hugebuf_o), "%-8s ", vict->pcdata->script_flags[i]);
    }
  strcat (hugebuf_o, "\n\r");
  page_to_char (hugebuf_o, ch);
  return;
}

void
do_unlearn (CHAR_DATA * ch, char *argy)
{
  int i;
  SPELL_DATA *spell;
  char general_use[2500];
  DEFINE_COMMAND ("unlearn", do_unlearn, POSITION_DEAD, MAX_LEVEL, LOG_ALWAYS, "Allows you to unlearn a specific spell or skill.")

    if (!argy || argy == "" || argy[0] == '\0')
    {
      send_to_char ("Syntax is: Unlearn skill/spellname.\n\r", ch);
      send_to_char ("Please note you do not get your practices or learns back.\n\r", ch);
      send_to_char ("For a full reset (getting pracs and learns back), type preset.\n\r", ch);
      return;
    }
  spell = skill_lookup (argy, -1);
  if (spell == NULL)
    {
      send_to_char ("That isn't a valid skill or spell, sorry.\n\r", ch);
      return;
    }
  if (ch->pcdata->learned[spell->gsn] == -100)
    {
      send_to_char ("You haven't learned that skill/spell anyway!\n\r", ch);
      send_to_char ("If it registers as a skill, you probably are wearing some +bonus equip.\n\r", ch);
      return;
    }
  for (i = 0; i < SKILL_COUNT; i++)
    {
      SPELL_DATA *spl;
      if ((spl = skill_lookup (NULL, i)) == NULL)
        continue;
      if (ch->pcdata->learned[i] < 0)
        continue;
      if (spl->prereq_1 == NULL)
        continue;
      if (skill_lookup (spl->prereq_1, -1) == spell)
        {
          send_to_char ("You can not unlearn a prerequisite to a spell/skill you already know!\n\r", ch);
          return;
        }
      if (spl->prereq_2 == NULL)
        continue;
      if (skill_lookup (spl->prereq_2, -1) == spell)
        {
          send_to_char ("You can not unlearn a prerequisite to a spell/skill you already know!\n\r", ch);
          return;
        }
    }
  ch->pcdata->learned[spell->gsn] = -100;
  sprintf (general_use, "You have successfully unlearned %s.\n\r",
           capitalize (spell->spell_funky_name));
  send_to_char (general_use, ch);
  return;
}

void
player_preset (CHAR_DATA * ch, char *argy)
{
  int i;

  if (str_cmp (argy, "Zlughlkheyn"))
    {
      send_to_char ("This command wipes all of your skills, and gives you learns and\n\r", ch);
      send_to_char ("practices as if you leveled each level from level 1 again.\n\r", ch);
      send_to_char ("To go ahead with the reset, type preset Zlughlkheyn.\n\r", ch);
      return;
    }
  if (IS_MOB (ch))
    {
      send_to_char ("Not on NPC's.\n\r", ch);
      return;
    }
/*clear all skills/spells */
  for (i = 0; i < SKILL_COUNT; i++)
    {
      ch->pcdata->learned[i] = -100;
    }
  ch->pcdata->practice = (1 + (((LEVEL (ch) - 1) *
                              (wis_app[get_curr_wis (ch)-1].practice))));
  if (LEVEL (ch) < 4)
    ch->pcdata->learn = (LEVEL (ch) * 2);
  else
    ch->pcdata->learn = 6 + (LEVEL (ch) - 3);
  send_to_char ("Your practices and learns have been reset.\n\r", ch);
  return;
}

void
do_preset (CHAR_DATA * ch, char *argy)  /*BY Owen Emlen */
{
  CHAR_DATA *victim;
  char arg1[500];
  int i;
  DEFINE_COMMAND ("preset", do_preset, POSITION_DEAD, MAX_LEVEL, LOG_ALWAYS, "This command resets all skills and practices of a target character.")
    
    if (LEVEL(ch) < IMM_LEVEL && str_cmp(argy, "Zlughlkheyn"))
      {
        send_to_char("You must type preset Zlughlkheyn in order to preset.\n\r", ch);
        return;
      }
  
  if (!IS_REAL_GOD (ch))
    {
      player_preset (ch, argy);
      return;
    }
  if (argy == "" || argy[0] == '\0')
    {
      send_to_char ("Format is PRESET <player>\n\r", ch);
      return;
    }
  argy = one_argy (argy, arg1);
  if ((victim = get_char_world (ch, arg1)) == NULL)
    {
      send_to_char ("They aren't here.\n\r", ch);
      return;
    }
  if (IS_MOB(victim)) return;
  /*clear all skills/spells */
  for (i = 0; i < SKILL_COUNT; i++)
    {
      victim->pcdata->learned[i] = -100;
    }
 
  victim->pcdata->practice = (1 + (LEVEL (victim) - 1) *
(wis_app[get_curr_wis (victim)-1].practice));
  if (LEVEL (victim) < 4)
    victim->pcdata->learn = (LEVEL (victim) * 2);
  else
    victim->pcdata->learn = 6 + (LEVEL (victim) - 3);
  send_to_char ("Practices and learns reset.\n\r", ch);
  send_to_char ("Your practices have been reset! :)\n\r", victim);
  return;
}


/* notify stuff partially lifted, so it probalby doens't work 100% - piece o crap */
void
do_notify (CHAR_DATA * ch, char *argy)
{
  char arg1[SML_LENGTH];
  char buf[SML_LENGTH];
  DEFINE_COMMAND ("notify", do_notify, POSITION_DEAD, MAX_LEVEL, LOG_NORMAL, "Notify HELP for help.  This toggles god/immort notification.")

    if (IS_MOB (ch))
    return;
  smash_tilde (argy);
  argy = one_argy (argy, arg1);
  if (arg1[0] == '\0')
    {
      if (IS_SET (ch->act, WIZ_NOTIFY))
        {
          send_to_char ("Godinfo/Notification is now OFF.\n\r", ch);
          REMOVE_BIT (ch->act, WIZ_NOTIFY);
          return;
        }
      else
        {
          send_to_char ("Godinfo/Notification is now ON.\n\r", ch);
          SET_BIT (ch->act, WIZ_NOTIFY);
          return;
        }
    }
  else if (!str_cmp (arg1, "death")
           || !str_cmp (arg1, "deaths"))
    {
      if (IS_SET (ch->act, WIZ_NOTIFY_DEATH))
        {
          send_to_char ("Death notification is now OFF.\n\r", ch);
          REMOVE_BIT (ch->act, WIZ_NOTIFY_DEATH);
          return;
        }
      else
        {
          send_to_char ("Death notification is now ON.\n\r", ch);
          SET_BIT (ch->act, WIZ_NOTIFY_DEATH);
          return;
        }
    }
  else if (!str_cmp (arg1, "bug")
           || !str_cmp (arg1, "bugs"))
    {
      if (IS_SET (ch->act, WIZ_NOTIFY_BUG))
        {
          send_to_char ("Bug notification is now OFF.\n\r", ch);
          REMOVE_BIT (ch->act, WIZ_NOTIFY_BUG);
          return;
        }
      else
        {
          send_to_char ("Bug notification is now ON.\n\r", ch);
          SET_BIT (ch->act, WIZ_NOTIFY_BUG);
          return;
        }
    }
  else if (!str_cmp (arg1, "log")
           || !str_cmp (arg1, "logs"))
    {
      if (IS_SET (ch->act, WIZ_NOTIFY_LOG))
        {
          send_to_char ("Log notification is now OFF.\n\r", ch);
          REMOVE_BIT (ch->act, WIZ_NOTIFY_LOG);
          return;
        }
      else
        {
          send_to_char ("Log notification is now ON.\n\r", ch);
          SET_BIT (ch->act, WIZ_NOTIFY_LOG);
          if (IS_SET (ch->act, WIZ_NOTIFY_BUG))
            do_notify (ch, "bug");
          return;
        }
    }
  else if (!str_cmp (arg1, "login")
           || !str_cmp (arg1, "logins"))
    {
      if (IS_SET (ch->act, WIZ_NOTIFY_LOGIN))
        {
          send_to_char ("Login notify is now OFF.\n\r", ch);
          REMOVE_BIT (ch->act, WIZ_NOTIFY_LOGIN);
          return;
        }
      else
        {
          send_to_char ("Login notify is now ON.\n\r", ch);
          SET_BIT (ch->act, WIZ_NOTIFY_LOGIN);
          return;
        }
    }
  else if (!str_cmp (arg1, "?")
           || !str_cmp (arg1, "help")
           || !str_cmp (arg1, "list"))
    {
      send_to_char ("Typing NOTIFY <option> toggles the notification on and off.\n\r", ch);
      send_to_char ("NOTIFY alone toggles notification off/on.\n\r", ch);
      send_to_char ("NOTIFY LIST and NOTIFY HELP all list this display.\n\r", ch);
      sprintf (buf, "Login notify : %s\n\r",
               IS_SET (ch->act, WIZ_NOTIFY_LOGIN) ? "ON" : "OFF");
      send_to_char (buf, ch);
      sprintf (buf, "Bugs notify : %s\n\r",
               IS_SET (ch->act, WIZ_NOTIFY_BUG) ? "ON" : "OFF");
      send_to_char (buf, ch);
      sprintf (buf, "Logs notify : %s\n\r",
               IS_SET (ch->act, WIZ_NOTIFY_LOG) ? "ON" : "OFF");
      send_to_char (buf, ch);
      sprintf (buf, "Death notify : %s\n\r",
               IS_SET (ch->act, WIZ_NOTIFY_DEATH) ? "ON" : "OFF");
      send_to_char (buf, ch);
      sprintf (buf, "The notify command is currently %s.\n\r",
               IS_SET (ch->act, WIZ_NOTIFY) ? "active" : "inactive");
      send_to_char (buf, ch);
      return;
    }
  else
    {
      send_to_char ("Huh?\n\r", ch);
      return;
    }
  return;
}

void
do_beamin (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("beamin", do_beamin, POSITION_DEAD, IMM_LEVEL, LOG_NORMAL, "This command changes what people see when you trans into the room.")

    if (IS_PLAYER (ch))
    {
      smash_tilde (argy);
      free_string (ch->pcdata->beamin);
      ch->pcdata->beamin = str_dup (argy);
      send_to_char ("Set.\n\r", ch);
    }
  return;
}

void
do_beamout (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("beamout", do_beamout, POSITION_DEAD, IMM_LEVEL, LOG_NORMAL, "This command changes what people see when you trans out of the room.")
    if (IS_PLAYER (ch))
    {
      smash_tilde (argy);
      free_string (ch->pcdata->beamout);
      ch->pcdata->beamout = str_dup (argy);
      send_to_char ("Set.\n\r", ch);
    }
  return;
}


void
do_disconnect (CHAR_DATA * ch, char *argy)
{
  char arg[SML_LENGTH];
  DESCRIPTOR_DATA *d;
  CHAR_DATA *victim;
  DEFINE_COMMAND ("disconnect", do_disconnect, POSITION_DEAD, MAX_LEVEL, LOG_ALWAYS, "This will cause the person to 'drop link'.")

    if (IS_MOB (ch))
    return;
  one_argy (argy, arg);
  if (arg[0] == '\0')
    {
      send_to_char ("Disconnect who?\n\r", ch);
      return;
    }
  if ((victim = get_char_world (ch, arg)) == NULL)
    {
      send_to_char ("They aren't here.\n\r", ch);
      return;
    }
  for (d = descriptor_list; d != NULL; d = d->next)
    {
      if (d == victim->desc)
        {
          close_socket (d);
          send_to_char ("Ok.\n\r", ch);
          return;
        }
    }
  return;
}

void
do_echo (CHAR_DATA * ch, char *argy)
{
  DESCRIPTOR_DATA *d;
  DEFINE_COMMAND ("echo", do_echo, POSITION_DEAD, MAX_LEVEL, LOG_ALWAYS, "This command prints a message to the entire mud.")

    if (argy[0] == '\0')
    {
      send_to_char ("Echo what?\n\r", ch);
      return;
    }
  for (d = descriptor_list; d; d = d->next)
    {
      if ((d->connected == CON_PLAYING || d->connected == CON_MEDITOR ||
           d->connected == CON_OEDITOR || d->connected == CON_REDITOR)
          && d->character)
        {
          send_to_char (anseval (argy, d->character), d->character);
          send_to_char ("\e[0m\n\r", d->character);
        }
    }
  return;
}

void
do_arenaecho (CHAR_DATA * ch, char *argy, bool batspam)
{
  DESCRIPTOR_DATA *d;
  if (argy[0] == '\0')
    {
      send_to_char ("Echo what?\n\r", ch);
      return;
    }
  for (d = descriptor_list; d; d = d->next)
    {
      if (!d->character)
        continue;
      if (IS_MOB (d->character))
        continue;
      if (CHALLENGE (d->character) == 10)
        continue;
      if (d->character && d->character->pcdata->arena_msg == 0)
        continue;
      if (d->character && d->character->pcdata->arena_msg == 2 && batspam == TRUE)
        continue;
      if (d->character->position == POSITION_FIGHTING && number_range (1, 2) == 1)
        continue;
      if (d->connected == CON_PLAYING)
        {
          send_to_char (anseval (argy, d->character), d->character);
          send_to_char ("\e[0m\n\r", d->character);
        }
    }
  return;
}

void
do_recho (CHAR_DATA * ch, char *argy)
{
  DESCRIPTOR_DATA *d;
  DEFINE_COMMAND ("recho", do_recho, POSITION_DEAD, IMM_LEVEL, LOG_NORMAL, "This shows a message to everyone in the room.")

    if (argy[0] == '\0')
    {
      send_to_char ("Recho what?\n\r", ch);
      return;
    }
  for (d = descriptor_list; d; d = d->next)
    {
      if (d->connected == CON_PLAYING
          && d->character->in_room == ch->in_room)
        {
          send_to_char (anseval (argy, d->character), d->character);
          send_to_char ("\e[0m\n\r", d->character);
        }
    }
  return;
}

ROOM_DATA *
find_location (CHAR_DATA * ch, char *arg)
{
  CHAR_DATA *victim;
  SINGLE_OBJECT *obj;
  if (is_number (arg))
    return get_room_index (atoi (arg));
  if ((victim = get_char_world (ch, arg)) != NULL)
    return victim->in_room;
  if ((obj = get_obj_world (ch, arg)) != NULL)
    return obj->in_room;
  return NULL;
}

void
do_transfer (CHAR_DATA * ch, char *argy)
{
  char arg1[SML_LENGTH];
  char arg2[SML_LENGTH];
  CHAR_DATA *t;
  ROOM_DATA *location;
  DESCRIPTOR_DATA *d;
  CHAR_DATA *victim;
  DEFINE_COMMAND ("transfer", do_transfer, POSITION_DEAD, MAX_LEVEL, LOG_NORMAL, "This transfers someone to a room specified, and more...")

    argy = one_argy (argy, arg1);
  one_argy (argy, arg2);
  if (arg1[0] == '\0')
    {
      send_to_char ("Transfer whom (and where)?\n\r", ch);
      return;
    }
  if (!str_cmp (arg1, "all"))
    {
      bool bground;
      bground = FALSE;
      if (UPPER (arg2[0]) == 'B')
        bground = TRUE;
      for (d = descriptor_list; d != NULL; d = d->next)
        {
          if (d->connected == CON_PLAYING
              && d->character->pcdata &&
              IS_PLAYER (d->character) &&
              d->character != ch
              && d->character->in_room != NULL)
            {
              char buf[STD_LENGTH];
              if (bground && d->character->pcdata->level > IMM_LEVEL)
                continue;
              if (!bground)
                {
                  sprintf (buf, "%s %s", NAME (d->character), arg2);
                }
              else
                {
                  char buf2[500];
                  if (!d->character->pcdata->bgf)
                    continue;
                  if (d->character->in_room->vnum < 1000 &&
                      (d->character->in_room->vnum < 100 || 
		       d->character->in_room->vnum > (100+NUM_ALIGN-1)))
                    {
                      send_to_char ("You cannot be in the battleground since you are in an invalid part of the world.\n\r", d->character);
                      continue;
                    }


                  if (LEVEL (d->character) < min_level || LEVEL (d->character) > max_level)
                    continue;
                  d->character->pcdata->bgf = FALSE;
                  if (bg_multiplier && bg_multiplier == 0)
		  sprintf (buf, "%s %d", NAME (d->character),
                           number_range (BATTLEGROUND_START_VNUM, BATTLEGROUND_END_VNUM - 1));
                  else
	          sprintf (buf, "%s %d", NAME (d->character),
			   number_range (BATTLEGROUND_START_VNUM + bg_multiplier, BATTLEGROUND_END_VNUM + bg_multiplier));
		  sprintf (buf2, "%s", NAME (d->character));
                  do_restore (ch, buf2);
                }
              do_transfer (ch, buf);
            }
        }
      return;
    }
  if (!str_cmp (arg1, "all_in_room"))
    {
      for (d = descriptor_list; d != NULL; d = d->next)
        {
          if (d->connected == CON_PLAYING
              && d->character->pcdata &&
              IS_PLAYER (d->character) &&
              d->character != ch
              && d->character->in_room != NULL
              && d->character->in_room == ch->in_room)
            {
              char buf[STD_LENGTH];
              sprintf (buf, "%s %s", NAME (d->character), arg2);
              do_transfer (ch, buf);
            }
        }
      return;
    }
  if (arg2[0] == '\0')
    {
      location = ch->in_room;
    }
  else
    {
      if ((location = find_location (ch, arg2)) == NULL)
        {
          if ((location = find_room_tag (argy)) == NULL)
            {
              send_to_char ("No such location.\n\r", ch);
              return;
            }
        }
    }
  victim = NULL;
  if (allow_anything)
    {
      for (t = char_list; t != NULL; t = t->next)
        {
          if (!str_cmp (RNAME (t), arg1))
            {
              victim = t;
              break;
            }
        }
      if (!victim)
        return;
    }
  else if ((victim = get_char_world_2 (ch, arg1)) == NULL)
    {
      send_to_char ("They aren't here.\n\r", ch);
      return;
    }

  if (FIGHTING (victim) != NULL)
    stop_fighting (FIGHTING (victim), TRUE);
  stop_fighting (victim, TRUE);
 /* act ("$n disappears in a cloud of smoke.", victim, NULL, NULL,TO_ROOM); */
  char_from_room (victim);
  char_to_room (victim, location);
 /* act ("$n arrives in a dazzling white flash.", victim, NULL, NULL,TO_ROOM); */
   if (IS_PLAYER (ch) && ch != victim)
    act ("You have been magically transferred.", ch, NULL, victim, TO_VICT); 
  do_look (victim, "auto");
  if (IS_PLAYER (ch) && LEVEL (ch) > IMM_LEVEL)
    send_to_char ("Ok.\n\r", ch);
  return;
}


void
do_mat (CHAR_DATA * ch, char *argy)
{
  int i, j, hash;
  char arg[SML_LENGTH];
  char arg2[SML_LENGTH];
  ROOM_DATA* location;
  ROOM_DATA *original, *roomto;
  DEFINE_COMMAND ("mat", do_mat, POSITION_DEAD, IMM_LEVEL+1, LOG_NORMAL, "This command performs multiple commands at the locations specified.")

    if (IS_MOB (ch))
    return;
  argy = one_argy (argy, arg);
  argy = one_argy (argy, arg2);
  if (arg[0] == '\0' || argy[0] == '\0')
    {
      send_to_char ("At where what?\n\r", ch);
      return;
    }
  if ((location = find_location (ch, arg)) == NULL)
    {
      if ((location = find_room_tag (arg)) == NULL)
        {
          send_to_char ("No such location.\n\r", ch);
          return;
        }
    }

  i = atoi (arg);
  j = atoi (arg2);
  if ((j - i > IMM_LEVEL) || (j - i < 1))
    {
      send_to_char ("Invalid argument range!\n\r", ch);
      return;
    }

  original = ch->in_room;
  for (hash = 0; hash < HASH_MAX; hash++)
    {
      for (roomto = room_hash[hash]; roomto != NULL; roomto = roomto->next)
        {
          if (roomto->vnum <= j && roomto->vnum >= i)
            {
              char_from_room (ch);
              /*u_ */ char_to_room (ch, roomto);
              super_interpret (ch, argy);
            }
        }
    }

  char_from_room (ch);
/*u_ */ char_to_room (ch, original);

  return;
}



void
do_at (CHAR_DATA * ch, char *argy)
{
  char arg[SML_LENGTH];
  ROOM_DATA *location;
  ROOM_DATA *original;
  DEFINE_COMMAND ("at", do_at, POSITION_DEAD, MAX_LEVEL, LOG_NORMAL, "This command performs a command at the location specified.")

    if (IS_MOB (ch))
    return;
  argy = one_argy (argy, arg);
  if (arg[0] == '\0' || argy[0] == '\0')
    {
      send_to_char ("At where what?\n\r", ch);
      return;
    }
  if ((location = find_location (ch, arg)) == NULL)
    {
      if ((location = find_room_tag (arg)) == NULL)
        {
          send_to_char ("No such location.\n\r", ch);
          return;
        }
    }
  original = ch->in_room;
  char_from_room (ch);
/*u_ */ char_to_room (ch, location);
  gotoxy (ch, 1, ch->pcdata->pagelen);
  super_interpret (ch, argy);
  char_from_room (ch);
/*u_ */ char_to_room (ch, original);
  return;
}
 
void
do_goto (CHAR_DATA * ch, char *argy)
{
  char arg[SML_LENGTH];
  DEFINE_COMMAND ("goto", do_goto, POSITION_DEAD, IMM_LEVEL, LOG_NORMAL, "This command beams you to the location specified.")
    if (argy[0] == '\0')
    {
      send_to_char ("Goto where?\n\r", ch);
      return;
    }
  strcpy (arg, "self ");
  strcat (arg, argy);
  do_transfer (ch, arg);
  return;
}

void
do_rstat (CHAR_DATA * ch, char *argy)
{
  extern char *const dir_name[];
  
 
  int sectr = 0;
  char buf[STD_LENGTH];
  char arg[SML_LENGTH];
  TRACK_DATA *tr;
  bool full = FALSE;
  ROOM_DATA *location;
  int door;
  DEFINE_COMMAND ("rstat", do_rstat, POSITION_DEAD, IMM_LEVEL, LOG_NORMAL, "This command shows the 'redit' room stats.")

    if (IS_MOB (ch))
    return;
  if (ch != NULL && ch->in_room != NULL) sectr = ch->in_room->sector_type;
  one_argy (argy, arg);
  location = (arg[0] == '\0') ? ch->in_room : find_location (ch, arg);
  if (!str_prefix ("full", arg))
    {
      location = ch->in_room;
      full = TRUE;
    }
  if (location == NULL)
    {
      send_to_char ("No such location.\n\r", ch);
      return;
    }
  
  sprintf (buf, "Name: '%s'\n\rArea: [%3d] '%s'\n\r",
           location->name,
           location->area->vnum,
           location->area->name);
  send_to_char (buf, ch);
  sprintf (buf,
           "Vnum: %d Sector: (%d) %s - %s\n\rLight:%d. Pcs: %d CmdObj: %d\n\r",
           location->vnum, 
           location->sector_type,
	   sectors[location->sector_type].name_seen, sectors[location->sector_type].desc,
           location->light, (location->more ? location->more->pcs : 0), location->command_objs);
  send_to_char (buf, ch);
  if (location->sector_type == SECT_CAPTURE) {
	  sprintf (buf, "Capture vnum range:        %d - %d\n\r"
			"Current controlling align: %d\n\r"
			"Bonus string for align:    %s\n\r",
			location->cvnuml, location->cvnumh, location->calign, location->cbonusstr);
	  send_to_char (buf, ch);
  }
  if (location->more && IS_SET (location->room_flags2, ROOM2_MOVING))
    {
      sprintf (buf, "Autotransport direction: %s\n\r", dir_name[location->more->move_dir]);
      send_to_char (buf, ch);
      sprintf (buf, "Move message:\n\r%s",
               location->more->move_message);
      send_to_char (buf, ch);
    }
  
  sprintf (buf,
           "Room flags: %s\n\r",
           room_bit_name (location->room_flags));
  send_to_char (buf, ch);
   sprintf (buf,
           "Room flags2: %s\n\r",
           room_bit_name2 (location->room_flags2));
  send_to_char (buf, ch);
  if (location->more && location->more->extra_descr != NULL)
    {
      DESCRIPTION_DATA *ed;
      send_to_char ("Extra description keywords: ", ch);
      for (ed = location->more->extra_descr; ed; ed = ed->next)
        {
          send_to_char ("'", ch);
          send_to_char (ed->keyword, ch);
          send_to_char ("'", ch);
        }
      send_to_char ("\n\r", ch);
    }
  for (door = 0; door <= 5; door++)
    {
      EXIT_DATA *pexit;
      if ((pexit = location->exit[door]) != NULL)
        {
          if (pexit->d_info)
            sprintf (buf,
                     "Door: %5s To: %d Key: %ld Exit flags: %d(%d) '%s'\n\r",
                     dir_name[door],
                     (ROOM_DATA *) pexit->to_room != NULL ? ((ROOM_DATA *) pexit->to_room)->vnum : 0,
                     pexit->d_info->key,
                     pexit->d_info->exit_info, pexit->d_info->rs_flags,
            (pexit->d_info->keyword != NULL) ? pexit->d_info->keyword : "");
          else
            sprintf (buf,
                     "Door: %5s To: %d\n\r",
                     dir_name[door],
                     (ROOM_DATA *) pexit->to_room != NULL ? ((ROOM_DATA *) pexit->to_room)->vnum : 0);
          send_to_char (buf, ch);
          if (pexit->d_info)
            {
              if (pexit->d_info->str != 30 && pexit->d_info->str != 0)
                {
                  sprintf (buf, "\tStr: %d Max Str: %d\n\r",
                           pexit->d_info->str, pexit->d_info->maxstr);
                  send_to_char (buf, ch);
                }
            }
          if (pexit->d_info && pexit->d_info->description != NULL && pexit->d_info->description[0] != '\0')
            send_to_char (pexit->d_info->description, ch);
        }
    }
  if (full)
    {
      for (tr = location->tracks; tr != NULL; tr = tr->next_track_in_room)
        {
          char bugr[500];
          sprintf (bugr, "%s: entered %s, left %s.\n\r",
                   (IS_PLAYER (tr->ch) ? NAME (tr->ch) : tr->ch->pIndexData->short_descr),
                   (tr->dir_came == 10 ? "None" : dir_name[tr->dir_came]),
                   (tr->dir_left == 10 ? "None" : dir_name[tr->dir_left]));
          send_to_char (bugr, ch);
        }
    }
  return;
}

void
do_ostat (CHAR_DATA * ch, char *argy)
{
  char buf[STD_LENGTH];
  char arg[SML_LENGTH];
  AFFECT_DATA *paf;
  SINGLE_OBJECT *obj;
  DEFINE_COMMAND ("ostat", do_ostat, POSITION_DEAD, IMM_LEVEL, LOG_NORMAL, "This command shows 'oedit' info on the object specified.")

    if (IS_MOB (ch))
    return;
  one_argy (argy, arg);
  if (arg[0] == '\0')
    {
      send_to_char ("Ostat what?\n\r", ch);
      return;
    }
  if ((obj = get_obj_world (ch, arg)) == NULL)
    {
      send_to_char ("Nothing like that in hell, earth, or heaven.\n\r", ch);
      return;
    }
  if (LEVEL (ch) > IMM_LEVEL && LEVEL (ch) < MAX_LEVEL && obj->pIndexData->area->open != 0)
    {
      send_to_char ("Due to cheating, builders cannot ostat objects in opened areas.\n\r", ch);
      return;
    }
  sprintf (buf, "Name: '%s'.\n\rArea: [%3d] '%s'.\n\rFile: '%s'.\n\r",
           obj->pIndexData->name,
           obj->pIndexData->area->vnum,
           obj->pIndexData->area->name,
           obj->pIndexData->area->filename);
  send_to_char (buf, ch);
  sprintf (buf, "Vnum: %d. Type: %s.\n\r",
           obj->pIndexData->vnum,
           item_type_name (obj->pIndexData->item_type));
  send_to_char (buf, ch);
  sprintf (buf, "Short description: %s\e[0m\n\rLong description: %s\e[0m",
           OOSTR (obj, short_descr), OOSTR (obj, description));
  send_to_char (buf, ch);
  sprintf (buf, "Wear bits: %s.\n\r Extra bits: %s.\n\r",
           wear_bit_name (obj->pIndexData->wear_flags),
           extra_bit_name (obj->pIndexData->extra_flags));
  send_to_char (buf, ch);
  sprintf (buf, "Number: %d/%d. Weight: %d/%d.\n\r",
           1, get_obj_number (obj),
           obj->pIndexData->weight, get_obj_weight (obj));
  send_to_char (buf, ch);
  sprintf (buf, "Cost: %d. Timer: %d.\n\r",
           obj->cost, obj->timer);
  send_to_char (buf, ch);
  sprintf (buf,
           "In room: %d. In object: %s. Carried by: %s. Wear_loc: %s.\n\r",
           obj->in_room == NULL ? 0 : obj->in_room->vnum,
           obj->in_obj == NULL ? "(none)" : OOSTR (obj->in_obj, short_descr),
           obj->carried_by == NULL ? "(none)" : NAME (obj->carried_by),
           wear_loc_name (obj->wear_loc));
  send_to_char (buf, ch);

  if (obj->pIndexData->extra_descr != NULL)
    {
      DESCRIPTION_DATA *ed;
      send_to_char ("Extra description keywords: '", ch);
      for (ed = obj->pIndexData->extra_descr; ed != NULL; ed = ed->next)
        {
          send_to_char (ed->keyword, ch);
          if (ed->next != NULL)
            send_to_char (" ", ch);
        }
      for (ed = obj->pIndexData->extra_descr; ed != NULL; ed = ed->next)
        {
          send_to_char (ed->keyword, ch);
          if (ed->next != NULL)
            send_to_char (" ", ch);
        }
      send_to_char ("'.\n\r", ch);
    }

  for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next)
    {
      sprintf (buf, "Affects %s by %d.\n\r", affect_loc_name (paf->location),
               paf->modifier);
      send_to_char (buf, ch);
    }
  return;
}

void
do_mstat (CHAR_DATA * ch, char *argy)
{
  char buf[STD_LENGTH];
  char arg[SML_LENGTH];
  AFFECT_DATA *paf;
  CHAR_DATA *victim;
  SPELL_DATA *spell;
  DEFINE_COMMAND ("mstat", do_mstat, POSITION_DEAD, IMM_LEVEL, LOG_NORMAL, "Shows a non-floating mob data summary of the vnum specified.")

    if (IS_MOB (ch))
    return;
  one_argy (argy, arg);
  if (arg[0] == '\0')
    {
      send_to_char ("Mstat whom?\n\r", ch);
      return;
    }
  if ((victim = get_char_world (ch, arg)) == NULL)
    {
      send_to_char ("They aren't here.\n\r", ch);
      return;
    }
  if (LEVEL (ch) > IMM_LEVEL && LEVEL (ch) < MAX_LEVEL && victim->in_room && victim->in_room->area->open != 0)
    {
      send_to_char ("Due to cheating, builders cannot mstat mobs in opened areas.\n\r", ch);
      return;
    }
  if (IS_PLAYER (victim))
    {
      send_to_char ("Not on PC's.\n\r", ch);
      return;
    }
  sprintf (buf, "Name: '%s'.\n\rArea: [%3d] '%s'.\n\r",
           RNAME (victim),
           victim->pIndexData->area->vnum,
           victim->pIndexData->area->name);
  send_to_char (buf, ch);
  sprintf (buf, "Height: \x1B[1;37m%d\x1B[37;0m (%d' %d\")\n\r",
           victim->height,
           victim->height / 12,
           victim->height % 12);
  send_to_char (buf, ch);
  sprintf (buf, "Vnum: %d  Sex: %s  In Room: %d  ",
           victim->pIndexData->vnum,
           SEX (victim) == SEX_MALE ? "male" :
           SEX (victim) == SEX_FEMALE ? "female" : "neutral",
           victim->in_room == NULL ? 0 : victim->in_room->vnum);
  send_to_char (buf, ch);
  sprintf (buf, "Hp: %ld/%ld  Moves: %d/%d\n\r",
           victim->hit, victim->max_hit,
           victim->move, victim->max_move);
  send_to_char (buf, ch);
/*sprintf( buf,
   "Str: %d Int: %d Wis: %d Dex: %d Con: %d\n\r",
   get_curr_str( victim ),
   get_curr_int( victim ),
   get_curr_wis( victim ),
   get_curr_dex( victim ),
   get_curr_con( victim ) );
   send_to_char( buf, ch );
 */
  sprintf (buf,
           "Level: %d  Alignment: %d  AC: %d  Coins: %d  Exp: %ld\n\r",
           LEVEL (victim), ALIGN (victim),
           GET_AC (victim), tally_coins (victim), victim->exp);
  send_to_char (buf, ch);
  sprintf (buf, "Hitroll: %d  Damroll: %d  Position: %d\n\r",
           GET_HITROLL (victim) + victim->pIndexData->hitroll, GET_DAMROLL (victim), victim->position);
  send_to_char (buf, ch);
  if (victim->position == POSITION_FIGHTING)
    {
      sprintf (buf, "Fighting: %s.\n\r",
               FIGHTING (victim) ? NAME (victim->fgt->fighting) : "(none)");
      send_to_char (buf, ch);
    }
  sprintf (buf, "Act: \x1B[1;37m%s\x1B[37;0m  Act3: \x1B[1;37m%s\x1B[37;0m\n\r",
      act_bit_name (victim->act), act3_bit_name (victim->pIndexData->act3));
  send_to_char (buf, ch);
  sprintf (buf, "Mobtype: %s  ",
           mob_type_name (victim->pIndexData->mobtype));
  send_to_char (buf, ch);
  if (HUNTING (victim) != NULL)
    {
      sprintf (buf, "Hunting: %s  ", victim->fgt->hunting);
      send_to_char (buf, ch);
    }
  sprintf (buf, "Master: %s  Leader: %s\n\rAffected by: %s%s.\n\r",
           MASTER (victim) ? NAME (victim->fgt->master) : "(none)",
           LEADER (victim) ? NAME (victim->fgt->leader) : "(none)",
           affect_bit_name (victim->affected_by),
           affect_bit_name_two (victim->more_affected_by));
  send_to_char (buf, ch);
  sprintf (buf, "Short description: %s\n\rLong description: %s\n\r",
           SD (victim), LD (victim));
  send_to_char (buf, ch);
  if (victim->pIndexData->spec_fun != NULL)
    {
      sprintf (buf, "Spec fun: %s.\n\r", victim->pIndexData->spec_name);
      send_to_char (buf, ch);
    }
  if (victim->pIndexData->alt_vnum != 0)
    {
      sprintf (buf, "Alter mob vnum: %d.\n\r", victim->pIndexData->alt_vnum);
      send_to_char (buf, ch);
    }
  for (paf = victim->affected; paf != NULL; paf = paf->next)
    {
      spell = skill_lookup (NULL, (int) paf->type);
      if (spell == NULL)
        continue;
      sprintf (buf,
      "Spell: '%s' modifies %s by %d for %d hours with bits %s and %s.\n\r",
               spell->spell_funky_name,
               affect_loc_name (paf->location),
               paf->modifier,
               paf->duration,
               affect_bit_name (paf->bitvector),
               affect_bit_name_two (paf->bitvector2));
      send_to_char (buf, ch);
    }
  return;
}

void
do_mfind (CHAR_DATA * ch, char *argy)
{
  char arg1[SML_LENGTH];
  char arg2[SML_LENGTH];
  char arg3[SML_LENGTH];
  int slen = 0;
  MOB_PROTOTYPE *pMobIndex;
  char oneline[500];
  bool fRange = FALSE;
  bool fArea = FALSE;
  bool fName = FALSE;
  int range1 = 0;
  int range2 = 0;
  AREA_DATA *pArea;
  char name[SML_LENGTH];
  int vnum;
  bool found = FALSE;
  char typename[40];
  DEFINE_COMMAND ("mfind", do_mfind, POSITION_DEAD, IMM_LEVEL, LOG_NORMAL, "Used to find mob prototypes (vnums).")

    pArea = ch->in_room->area;
  hugebuf_o[0] = '\0';
  typename[0] = '\0';
  if (IS_MOB (ch))
    return;
  name[0]='\0';
  argy = one_argy (argy, arg1);
  argy = one_argy (argy, arg2);
  argy = one_argy (argy, arg3);
  if (arg1[0] == '\0')
    {
      send_to_char ("Syntax: mfind all\n\r", ch);
      send_to_char (" mfind [name, area] [num1 num2]\n\r", ch);
      return;
    }
  if (is_number (arg1) && is_number (arg2))
    {
      fRange = TRUE;
      range1 = atoi (arg1);
      range2 = atoi (arg2);
      if (!str_cmp (arg3, "area"))
        fArea = TRUE;
      else
        {
          if (!str_cmp (arg3, "all"))
            {
              fRange = FALSE;
              fName = FALSE;
              fArea = FALSE;
            }
          else
            {
              fName = TRUE;
              strcpy (name, arg3);
            }
        }
    }
  else
    {
      if (is_number (arg2) && is_number (arg3))
        {
          fRange = TRUE;
          range1 = atoi (arg2);
          range2 = atoi (arg3);
        }
      if (!str_cmp (arg1, "area"))
        fArea = TRUE;
      else
        {
          if (!str_cmp (arg1, "all"))
            {
              fRange = FALSE;
              fName = FALSE;
              fArea = FALSE;
            }
          else
            {
              fName = TRUE;
              strcpy (name, arg1);
            }
        }
    }
  if (range1 > range2 && fRange)
    {
      vnum = range1;
      range1 = range2;
      range2 = vnum;
    }
  if (range2 > top_vnum_mob || !fRange)
    range2 = top_vnum_mob;
  for (vnum = range1; vnum <= range2; vnum++)
    {
      if ((pMobIndex = get_mob_index (vnum)) != NULL
          && (is_name (name, pMobIndex->name) || !fName)
          && (pMobIndex->area == pArea || !fArea))
        {
          if (pMobIndex->mobtype == -1)
            strcpy (typename, "\e[1mundefined\e[0m");
          else if (pMobIndex->mobtype != MOB_OTHER
                   && mob_type[pMobIndex->mobtype] != NULL)
            strcpy (typename, mob_type[pMobIndex->mobtype]);
          else
            strcpy (typename, "other");
          found = TRUE;
          sprintf (oneline, "[%5d %3d] %-25s%s %-25s %-10s\n\r", vnum, pMobIndex->level,
                   pMobIndex->short_descr,
                   get_ansi_spaces (pMobIndex->short_descr),
                   !fArea ? pMobIndex->name : "", typename);
          sprintf (hugebuf_o + slen, "%s", oneline);
          slen += strlen (oneline);
          if (slen > 69500)
            {
              hugebuf_o[69500] = '\0';
              break;
            }
        }
    }
  if (!found)
    send_to_char ("Nothing like that in hell, earth, or heaven.\n\r", ch);
  if (hugebuf_o[0] != '\0')
    page_to_char (hugebuf_o, ch);
  return;
}

void
do_ofind (CHAR_DATA * ch, char *argy)
{
  char arg1[SML_LENGTH];
  char arg2[SML_LENGTH];
  char arg3[SML_LENGTH];
  OBJ_PROTOTYPE *pObjIndex;
  bool fRange = FALSE;
  bool fArea = FALSE;
  bool fName = FALSE;
  int slen = 0;
  char oneline[500];
  int range1 = 0;
  int range2 = 0;
  AREA_DATA *pArea;
  char name[SML_LENGTH];
  int vnum;
  bool found = FALSE;

  DEFINE_COMMAND ("ofind", do_ofind, POSITION_DEAD, IMM_LEVEL, LOG_NORMAL, "This command is used to find object prototypes (vnums).")
    pArea = ch->in_room->area;
  name[0]='\0';
  if (IS_MOB (ch))
    return;
  hugebuf_o[0] = '\0';
  argy = one_argy (argy, arg1);
  argy = one_argy (argy, arg2);
  argy = one_argy (argy, arg3);
  if (arg1[0] == '\0')
    {
      send_to_char ("Syntax: ofind all\n\r", ch);
      send_to_char (" ofind [name, area] [num1 num2]\n\r", ch);
      return;
    }
  if (!str_cmp(arg1, "sky") || !str_cmp(arg1, "spiked"))
    {
      send_to_char(" For some reason ofind sky and spiked crash it.\n\r",
ch);
      return;
    }

  if (!str_cmp(arg1,"weapons")) {
        OBJ_PROTOTYPE *op;
        int i;
        int ln;
        sprintf(hugebuf_o,"%5s %-25s  %-6s %-10s %5s %s\n\r",
                "Vnum","Weapon Name","Damage","Type","Cost","MoreAff");
        for (i=0; i<HASH_MAX; i++) {
          for (op=obj_index_hash[i];op!=NULL;op=op->next) {
                if (op->item_type!=ITEM_WEAPON) continue;
                ln=strlen(hugebuf_o);
                if (ln>69500) {
                        goto duinnn;
                        }
                sprintf(hugebuf_o+ln,"%-5d %-25s  %2dd%-2d %-10s %5d %2s\n\r",
                        op->vnum,strip_ansi_codes(op->short_descr), op->value[7],
                        op->value[8],
                        (IS_SET(op->extra_flags,ITEM_CONCUSSION)?"Concussion":
                        (IS_SET(op->extra_flags,ITEM_WHIP)?"Whipping":
                        (IS_SET(op->extra_flags,ITEM_HEAVY_SLASH)?"Hvy_slash":
                        (IS_SET(op->extra_flags,ITEM_LIGHT_SLASH)?"Lgt_slash":
                        (IS_SET(op->extra_flags,ITEM_PIERCE)?"Piercing":
                        (IS_SET(op->extra_flags,ITEM_SLASHING)?"Slashing":
                        "unknown")))))),op->cost,
                        (op->affected?"Y":"N"));
                }
          }
        duinnn:
        page_to_char(hugebuf_o,ch);
        return;
        }

  if (!str_cmp(arg1,"armors")) {
        int ln;
        OBJ_PROTOTYPE *op;
        int i;
        sprintf(hugebuf_o,"%5s %-25s  %-17s %5s %s\n\r",
                "Vnum","Armor Name","L B H A Pd Sr Pr","Cost","MoreAff");
        for (i=0; i<HASH_MAX; i++) {
          for (op=obj_index_hash[i];op!=NULL;op=op->next) {

                if (op->item_type!=ITEM_ARMOR) continue;
                ln=strlen(hugebuf_o);
                if (ln>69500) {
                        goto dunnn;
                        }
                sprintf(hugebuf_o+ln,"%-5d %-25s  %-2d%-2d%-2d%-2d%-3d%-3d%-3d %5d %2s\n\r",
                        op->vnum,strip_ansi_codes(op->short_descr),
                        op->value[0],
                        op->value[1],
                        op->value[2],
                        op->value[3],
                        op->value[4],
                        op->value[5],
                        op->value[8],
                        op->cost,
                        (op->affected?"Y":"N"));
                }
          }
        dunnn:
        page_to_char(hugebuf_o,ch);
        return;
        }
  if (is_number (arg1) && is_number (arg2))
    {
      fRange = TRUE;
      range1 = atoi (arg1);
      range2 = atoi (arg2);
      if (!str_cmp (arg3, "area"))
        fArea = TRUE;
      else
        {
          if (!str_cmp (arg3, "all"))
            {
              fRange = FALSE;
              fName = FALSE;
              fArea = FALSE;
            }
          else
            {
              fName = TRUE;
              strcpy (name, arg3);
            }
        }
    }
  else
    {
      if (is_number (arg2) && is_number (arg3))
        {
          fRange = TRUE;
          range1 = atoi (arg2);
          range2 = atoi (arg3);
        }
      if (!str_cmp (arg1, "area"))
        fArea = TRUE;
      else
        {
          if (!str_cmp (arg1, "all"))
            {
              fRange = FALSE;
              fName = FALSE;
              fArea = FALSE;
            }
          else
            {
              fName = TRUE;
              strcpy (name, arg1);
            }
        }
    }
  if (range1 > range2 && fRange)
    {
      vnum = range1;
      range1 = range2;
      range2 = vnum;
    }
  if (range2 > top_vnum_obj || !fRange)
    range2 = top_vnum_obj;
  for (vnum = range1; vnum <= range2; vnum++)
    {
      if ((pObjIndex = get_obj_index (vnum)) != NULL
          && (is_name (name, pObjIndex->name) || !fName)
          && (pObjIndex->area == pArea || !fArea))
        {
          found = TRUE;
          sprintf (oneline, "[%5d %3d] %-32s\e[0m%s %-28s\n\r", vnum,
                   pObjIndex->item_type,
                   pObjIndex->short_descr,
                   get_ansi_spaces (pObjIndex->short_descr),
                   !fArea ? pObjIndex->name : "");
          sprintf (hugebuf_o + slen, "%s", oneline);
          slen += strlen (oneline);
          if (slen > 69500)
            {
              hugebuf_o[69500] = '\0';
              break;
            }
        }
    }
  if (hugebuf_o[0] != '\0')
    page_to_char (hugebuf_o, ch);
  if (!found)
    send_to_char ("Nothing like that in hell, earth, or heaven.\n\r", ch);
  return;
}

void
do_rfind (CHAR_DATA * ch, char *argy)
{
  char arg1[SML_LENGTH];
  char arg2[SML_LENGTH];
  char arg3[SML_LENGTH];
  ROOM_DATA *oneroom;
  bool fRange = FALSE;
  bool fArea = FALSE;
  bool fName = FALSE;
  int slen = 0;
  char oneline[500];
  int range1 = 0;
  int range2 = 0;
  AREA_DATA *pArea;
  char name[SML_LENGTH];
  int vnum;
  bool found = FALSE;
  DEFINE_COMMAND ("rfind", do_rfind, POSITION_DEAD, IMM_LEVEL, LOG_NORMAL, "This command is used to find rooms (vnums).")
    pArea = ch->in_room->area;
  if (IS_MOB (ch))
    return;
  hugebuf_o[0] = '\0';
  argy = one_argy (argy, arg1);
  argy = one_argy (argy, arg2);
  argy = one_argy (argy, arg3);
  if (arg1[0] == '\0')
    {
      send_to_char (" rfind [name, area] [num1 num2]\n\r", ch);
      return;
    }
  if (!str_cmp (arg1, "tags"))
    {
      int hash;
      ROOM_DATA *rid;
      for (hash = 0; hash < HASH_MAX; hash++)
        {
          for (rid = room_hash[hash]; rid != NULL; rid = rid->next)
            {
              char *t;
              for (t = rid->name; *t != '\0'; t++)
                {
                  if (*t == ';')
                    {
                      sprintf (oneline, "\x1B[0m[%5d] %s    SectorType> %s\n\r",
rid->vnum, show_room_name (ch, rid->name),(sectors[rid->sector_type].name_seen));
                      sprintf (hugebuf_o + slen, "%s", oneline);
                      slen += strlen (oneline);
                    }
                }
            }
        }
      if (hugebuf_o[0] != '\0')
        page_to_char (hugebuf_o, ch);
      return;
    }
  
  else if (is_number (arg1) && is_number (arg2))
    {
      fRange = TRUE;
      range1 = atoi (arg1);
      range2 = atoi (arg2);
      if (!str_cmp (arg3, "area") || arg3[0] == '\0')
        fArea = TRUE;
      else
        {
          if (!str_cmp (arg3, "all"))
            {
              fRange = FALSE;
              fName = FALSE;
              fArea = FALSE;
            }
          else
            {
              fName = TRUE;
              strcpy (name, arg3);
            }
        }
    }
  else
    {
      if (is_number (arg2) && is_number (arg3))
        {
          fRange = TRUE;
          range1 = atoi (arg2);
          range2 = atoi (arg3);
        }
      if (!str_cmp (arg1, "area"))
        fArea = TRUE;
     
      else
        {
          if (!str_cmp (arg1, "all"))
            {
              fRange = FALSE;
              fName = FALSE;
              fArea = FALSE;
            }
          else
            {
              fName = TRUE;
              strcpy (name, arg1);
            }
        }
    }
  if (range1 > range2 && fRange)
    {
      vnum = range1;
      range1 = range2;
      range2 = vnum;
    }
  if (range2 > top_vnum_room || !fRange)
    range2 = top_vnum_room;
  for (vnum = range1; vnum <= range2; vnum++)
    {
      if ((oneroom = get_room_index (vnum)) != NULL
          && (!fName || is_name (name, oneroom->name))
          && (oneroom->area == pArea || !fArea))
        {
          found = TRUE;
          sprintf (oneline, "[%5d %s] %-20s---%s\n\r", vnum, DESCRIPTED
(oneroom) ? "D" : "-",  oneroom->name,
sectors[oneroom->sector_type].name_seen );
          sprintf (hugebuf_o + slen, "%s", oneline);
          slen += strlen (oneline);
          if (slen > 69500)
            {
              hugebuf_o[69500] = '\0';
              break;
            }
        }
    }
  if (!found)
    send_to_char ("Nothing like that in hell, earth, or heaven.\n\r", ch);
  if (hugebuf_o[0] != '\0')
    page_to_char (hugebuf_o, ch);
  return;
}

void
do_mwhere (CHAR_DATA * ch, char *argy)
{
  char arg[SML_LENGTH];
  int slen = 0;
  char oneline[500];
  CHAR_DATA *victim;
  bool found;
  DEFINE_COMMAND ("mwhere", do_mwhere, POSITION_DEAD, MAX_LEVEL, LOG_NORMAL, "This command is used to find actual mobs in the world.")

    hugebuf_o[0] = '\0';
  if (IS_MOB (ch))
    return;
  one_argy (argy, arg);
  if (arg[0] == '\0')
    {
      send_to_char ("Mwhere whom?\n\r", ch);
      return;
    }
  found = FALSE;
  for (victim = char_list; victim != NULL; victim = victim->next)
    {
      if (IS_MOB (victim)
          && victim->in_room != NULL
          && is_name (arg, NAME (victim)))
        {
          found = TRUE;
          sprintf (oneline, "[%5d] %-28s%s [%5d] %s\n\r",
                   victim->pIndexData->vnum,
                   SD (victim),
                   get_ansi_spaces (SD (victim)),
                   victim->in_room->vnum,
                   victim->in_room->name);
          sprintf (hugebuf_o + slen, "%s", oneline);
          slen += strlen (oneline);
          if (slen > 69500)
            {
              hugebuf_o[69500] = '\0';
              break;
            }
        }
    }
  if (!found)
    act ("You didn't find any $T.", ch, NULL, arg, TO_CHAR);
  if (hugebuf_o[0] != '\0')
    page_to_char (hugebuf_o, ch);
  return;
}

void
show_guilds (CHAR_DATA * ch, int bit)
{
  if (bit != 0)
    {
      char buf[500];
      int i=0;
      bool prev= FALSE;
      if (IS_MOB(ch)) return;
      sprintf(buf, "Guild(s): ");
      for (i = 0; str_cmp(guild_data[i].what_you_type, "end_of_list"); i++)
	{
	  if (IS_SET(bit,guild_data[i].mob_guildmaster_bit))
	    {
	      if (prev)
		strcat(buf, ",");
	      strcat(buf, guild_data[i].what_you_see);
	      prev = TRUE;
	    }
	}
      
      strcat(buf, ".\n\r");
      send_to_char(buf, ch);
    }
  return;
}

void
do_pstat (CHAR_DATA * ch, char *argy)
{
  char buf[STD_LENGTH];
  char arg[SML_LENGTH];
  AFFECT_DATA *paf;
  CHAR_DATA *victim;
  SPELL_DATA *spell;
  DEFINE_COMMAND ("pstat", do_pstat, POSITION_DEAD, MAX_LEVEL, LOG_NORMAL, "This command can be used to get stats on a player specified.")

    if (IS_MOB (ch))
    return;
  one_argy (argy, arg);
  if (arg[0] == '\0')
    {
      send_to_char ("Pstat whom?\n\r", ch);
      return;
    }
  if ((victim = get_char_world (ch, arg)) == NULL)
    {
      send_to_char ("They aren't here.\n\r", ch);
      return;
    }
  if (IS_MOB (victim))
    {
      send_to_char ("Not on mobs.\n\r", ch);
      return;
    }
  if (LEVEL (ch) < MAX_LEVEL && LEVEL (victim) < IMM_LEVEL)
    {
      send_to_char ("You are not allowed to look at other players' stats!\n\r", ch);
      return;
    }
  sprintf (buf, "Name: \x1B[1;37m%s  \x1B[37;0mRace: \x1B[1;37m%s (%d)\x1B[37;0m Email: \x1B[1;37m%s\x1B[37;0m.\n\r",
           NAME (victim), race_info[victim->pcdata->race].name,
           victim->pcdata->race, victim->pcdata->email);
  send_to_char (buf, ch);
  sprintf (buf, "Remorts: %d  Natural Armor: %d  Sex: %s  Deaths: %d\n\r",
           victim->pcdata->remort_times, victim->pcdata->nat_armor,
           SEX (victim) == SEX_MALE ? "male" : SEX (victim) == SEX_FEMALE ? "female" : "neutral",
           victim->pcdata->deaths);
  send_to_char (buf, ch);

  if (clan_number (victim) > 0)
    {
      sprintf (buf, "Clan: %s.\n\r", get_clan_index (clan_number (victim))->name);
      send_to_char (buf, ch);
    }
  sprintf (buf, "In Room: #%d  Warpoints: %ld Total Warpoints: %ld \n\r",
           victim->in_room == NULL ? 0 : victim->in_room->vnum,
           victim->pcdata->warpoints, 
	   victim->pcdata->total_wps);
  send_to_char (buf, ch);
  send_to_char ("Natural Abilities: ", ch);
  send_to_char (natural_abilities (victim->pcdata->nat_abilities), ch);
  send_to_char (".\n\r", ch);
  send_to_char ("Natural Abilities2: ", ch);
  send_to_char (natural_abilities2 (victim->pcdata->nat_abilities2), ch);
  send_to_char (".\n\r", ch);
  send_to_char ("Augments: ", ch);
  send_to_char (augmentss (victim->pcdata->augments), ch);
  send_to_char (".\n\r", ch);
  show_guilds (ch, victim->pcdata->guilds);
  sprintf (buf, "Str: %d  Int: %d  Wis: %d  Dex: %d  Con: %d  Bank Account: %ld.\n\r",
           get_curr_str (victim), get_curr_int (victim), get_curr_wis (victim), get_curr_dex (victim), get_curr_con (victim), victim->pcdata->bank);
  send_to_char (buf, ch);
  sprintf (buf, "Hp: %ld/%ld  Move: %d/%d  Pracs: %d  Kills: %d  Killpoints: %d\n\r",
           victim->hit, victim->max_hit,
           victim->move, victim->max_move,
           victim->pcdata->practice, victim->pcdata->totalkills,
           victim->pcdata->killpoints);
  send_to_char (buf, ch);
  sprintf (buf, "Level: %d  Alignment: %d  AC: %d  Exp: %ld  Security: %d\n\r",
           LEVEL (victim), victim->pcdata->alignment, GET_AC (victim), victim->exp, victim->pcdata->security);
  send_to_char (buf, ch);
  sprintf (buf, "Hitroll: %d  Damroll: %d  Position: %d  Learns: %d\n\r",
           GET_HITROLL (victim), GET_DAMROLL (victim),
           victim->position, victim->pcdata->learn);
  send_to_char (buf, ch);
  if (victim->position == POSITION_FIGHTING)
    {
      sprintf (buf, "Fighting: %s.\n\r",
               FIGHTING (victim) ? NAME (victim->fgt->fighting) : "(none)");
      send_to_char (buf, ch);
    }
  sprintf (buf, "Thirst: %d/50  Full: %d/50  Drunk: %d/0  Saving throw: %d\n\r",
           victim->pcdata->condition[COND_THIRST],
           victim->pcdata->condition[COND_FULL],
           victim->pcdata->condition[COND_DRUNK],
           victim->pcdata->saving_throw);
  send_to_char (buf, ch);
  sprintf (buf, "Items Carried: %d  Weight Carried: %d Stones, %d Pebbles\n\r",
           victim->pcdata->carry_number, victim->pcdata->carry_weight/WGT_MULT, victim->pcdata->carry_weight % WGT_MULT);
  send_to_char (buf, ch);
  sprintf (buf, "Age: %d  Seconds Played: %d  Act: %s.\n\r",
           get_age (victim), (int) victim->pcdata->played,
           plr_bit_name (victim->act));
  send_to_char (buf, ch);
  sprintf (buf, "Group Info... Master: %s  Leader: %s\n\rAffected by: %s (and) %s.\n\r",
           MASTER (victim) ? NAME (victim->fgt->master) : "(none)",
           LEADER (victim) ? NAME (victim->fgt->leader) : "(none)",
           affect_bit_name (victim->affected_by),
           affect_bit_name_two (victim->more_affected_by));
  send_to_char (buf, ch);
  sprintf (buf, "Short description: %s.\n\rLong description: %s\n\r",
           SD (victim),
           LD (victim));
  send_to_char (buf, ch);
  for (paf = victim->affected; paf != NULL; paf = paf->next)
    {
      spell = skill_lookup (NULL, (int) paf->type);
      if (spell == NULL)
        continue;
      sprintf (buf,
      "Spell: '%s' modifies %s by %d for %d hours with bits %s and %s.\n\r",
               spell->spell_funky_name,
               affect_loc_name (paf->location),
               paf->modifier,
               paf->duration,
               affect_bit_name (paf->bitvector),
               affect_bit_name_two (paf->bitvector2));
      send_to_char (buf, ch);
    }
  return;
}

void
do_reboo (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("reboo", do_reboo, POSITION_DEAD, MAX_LEVEL, LOG_NORMAL, "Protection against accidentally matching 'reboot'.")

    send_to_char ("If you want to REBOOT, spell it out.\n\r", ch);
  return;
}

void
do_reboot (CHAR_DATA * ch, char *argy)
{
  extern bool aturion_down;
  CHAR_DATA *bah;
  FILE *fpqr;
  AUCTION *auct;
  DEFINE_COMMAND ("reboot", do_reboot, POSITION_DEAD, MAX_LEVEL, LOG_NORMAL, "Always use REBOOT <ticks until reboot>.  Reboot 0 aborts a reboot countdown.")
    if (IS_MOB (ch))
    return;
  if (!IS_REAL_GOD (ch))
    return;

  if (is_number (argy))
    {
      send_to_char ("Ticks to reboot set... set to 0 to abort reboot.\n\r", ch);
      ticks_to_reboot = atoi (argy);
      return;
    }
   for (auct = auction_list; auct != NULL; auct = auct->next)
    {
      if (auct->item)
        {  
          obj_to(auct->item, auct->bychar);
          auct->item = NULL;
        }
    }



  for (bah = char_list; bah != NULL; bah = bah->next)
    {
      if (IS_PLAYER (bah))
        do_save (bah, "xx2xx11");
    };
  fpqr = fopen ("reason.fi", "w+");
  fprintf (fpqr, "Mud-Reboot \n\rEnd~\n\r");
  fclose (fpqr);
  aturion_down = TRUE;
  return;
}

void
do_snoop (CHAR_DATA * ch, char *argy)
{
  char arg[SML_LENGTH];
  DESCRIPTOR_DATA *d;
  CHAR_DATA *victim;
  DEFINE_COMMAND ("snoop", do_snoop, POSITION_DEAD, MAX_LEVEL, LOG_NORMAL, "This command allows you to 'snoop' a player.")

    one_argy (argy, arg);
  if (IS_MOB (ch))
    return;

  if (arg[0] == '\0')
    {
      send_to_char ("Snoop whom?\n\r", ch);
      return;
    }

  if ((victim = get_char_world (ch, arg)) == NULL)
    {
      send_to_char ("They aren't here.\n\r", ch);
      return;
    }
  if (IS_MOB (victim) || victim->desc == NULL)
    {
      send_to_char ("No descriptor to snoop.\n\r", ch);
      return;
    }
  if (victim == ch)
    {
      send_to_char ("Cancelling all snoops.\n\r", ch);
      for (d = descriptor_list; d != NULL; d = d->next)
        {
          if (d->snoop_by == ch->desc)
            d->snoop_by = NULL;
        }
      return;
    }
  if (victim->desc->snoop_by != NULL)
    {
      send_to_char ("Busy already.\n\r", ch);
      return;
    }
  if (LEVEL (victim) > LEVEL (ch))
    {
      send_to_char ("You failed.\n\r", ch);
      return;
    }
  if (ch->desc != NULL)
    {
      for (d = ch->desc->snoop_by; d != NULL; d = d->snoop_by)
        {
          if (d->character == victim || d->original == victim)
            {
              send_to_char ("No snoop loops.\n\r", ch);
              return;
            }
        }
    }
  victim->desc->snoop_by = ch->desc;
  send_to_char ("Ok.\n\r", ch);
  return;
}

void
do_switch (CHAR_DATA * ch, char *argy)
{
  char arg[SML_LENGTH];
  CHAR_DATA *victim;
  DEFINE_COMMAND ("switch", do_switch, POSITION_DEAD, MAX_LEVEL, LOG_ALWAYS, "This command allows you to switch into a mob.")

    if (IS_MOB (ch))
    return;
  one_argy (argy, arg);
  if (arg[0] == '\0')
    {
      send_to_char ("Switch into whom?\n\r", ch);
      return;
    }
  if (IS_MOB (ch) || ch->desc == NULL)
    return;
  if (ch->desc->original != NULL)
    return;

  if ((victim = get_char_world (ch, arg)) == NULL)
    {
      send_to_char ("They aren't here.\n\r", ch);
      return;
    }
  if (victim == ch)
    {
      send_to_char ("Ok.\n\r", ch);
      return;
    }
  ch->desc->character = victim;
  ch->desc->original = ch;
  victim->desc = ch->desc;
  ch->desc = NULL;
  send_to_char ("Ok.\n\r", victim);
  return;
}
/* Make sure switch is okay.. sorta iffy :) */

void
do_return (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("return", do_return, POSITION_DEAD, 0, LOG_NORMAL, "This command allows you to return from a switch command.")

    if (IS_PLAYER (ch))
    return;
  if (ch->desc == NULL)
    return;
  if (ch->desc->original == NULL)
    {
      send_to_char ("You aren't switched.\n\r", ch);
      return;
    }
  send_to_char ("You return to your original body.\n\r", ch);
  ch->desc->character = ch->desc->original;
  ch->desc->original = NULL;
  ch->desc->character->desc = ch->desc;
  ch->desc = NULL;
  return;
}

void
do_mload (CHAR_DATA * ch, char *argy)
{
  char arg[SML_LENGTH];
  MOB_PROTOTYPE *pMobIndex;
  CHAR_DATA *victim;
  char *t;
  DEFINE_COMMAND ("mload", do_mload, POSITION_DEAD, IMM_LEVEL, LOG_ALWAYS, "Syntax: Mload <vnum>, where <vnum> is the mob template vnum of the mob you wish to create.")

    if (IS_MOB (ch))
    return;
  one_argy (argy, arg);
  if (arg[0] == '\0' || !is_number (arg))
    {
      send_to_char ("Syntax: mload <vnum>.\n\r", ch);
      return;
    }
  if ((pMobIndex = get_mob_index (atoi (arg))) == NULL)
    {
      send_to_char ("No mob has that vnum.\n\r", ch);
      return;
    }
  for (t = pMobIndex->name; *t != '\0'; t++)
    if (*t == '%')
      {
        send_to_char ("You may not mload this mob... it is meant for internal game use only.\n\r", ch);
        return;
      }
  for (t = pMobIndex->short_descr; *t != '\0'; t++)
    if (*t == '%')
      {
        send_to_char ("You may not mload this mob... it is meant for internal game use only.\n\r", ch);
        return;
      }
  victim = create_mobile (pMobIndex);
  char_to_room (victim, ch->in_room);
  act ("$n has created $N!", ch, NULL, victim, TO_ROOM);
  act ("You create $N!", ch, NULL, victim, TO_CHAR);
  return;
}

void
do_oload (CHAR_DATA * ch, char *argy)
{
  char arg1[SML_LENGTH];
  OBJ_PROTOTYPE *pObjIndex;
  SINGLE_OBJECT *obj;
  char *t;
  DEFINE_COMMAND ("oload", do_oload, POSITION_DEAD, IMM_LEVEL, LOG_ALWAYS, "Creates an object from the specified object template (vnum).")

    if (IS_MOB (ch))
    return;
  argy = one_argy (argy, arg1);
  if (arg1[0] == '\0' || !is_number (arg1))
    {
      send_to_char ("Syntax: oload <vnum>.\n\r", ch);
      return;
    }
  if ((pObjIndex = get_obj_index (atoi (arg1))) == NULL)
    {
      send_to_char ("No object has that vnum.\n\r", ch);
      return;
    }

  for (t = pObjIndex->name; *t != '\0'; t++)
    if (*t == '%')
      {
        send_to_char ("You may not oload this item... it is meant for internal game use only.\n\r", ch);
        return;
      }
  for (t = pObjIndex->short_descr; *t != '\0'; t++)
    if (*t == '%')
      {
        send_to_char ("You may not oload this item... it is meant for internal game use only.\n\r", ch);
        return;
      }
  obj = create_object (pObjIndex, 1);
  if (CAN_WEAR (obj, ITEM_TAKE))
    {
      obj_to (obj, ch);
    }
  else
    {
      obj_to (obj, ch->in_room);
    }
  act ("$n has created $p!", ch, obj, NULL, TO_ROOM);
  act ("You create $p!", ch, obj, NULL, TO_CHAR);
  return;
}

void
raw_purge (ROOM_DATA * rid)
{
  CHAR_DATA *victim;
  CHAR_DATA *vnext;
  SINGLE_OBJECT *obj;
  SINGLE_OBJECT *obj_next;
  if (!rid->more)
    return;
  for (victim = rid->more->people; victim != NULL; victim = vnext)
    {
      vnext = victim->next_in_room;
      if (IS_MOB (victim) && victim->desc == NULL)
        extract_char (victim, TRUE);
      if (!rid->more)
        return;
    }
  if (!rid->more)
    return;
  rid->more->copper = 0;
  rid->more->gold = 0;
  for (obj = rid->more->contents; obj != NULL; obj = obj_next)
    {
      obj_next = obj->next_content;
      free_it (obj);
      if (!rid->more)
        return;
    }
  return;
}

void
do_purge (CHAR_DATA * ch, char *argy)
{
  CHAR_DATA *v;
  DEFINE_COMMAND ("purge", do_purge, POSITION_DEAD, IMM_LEVEL, LOG_NORMAL, "This command purges the room of all mobs and objects.")
    if (!ch->in_room->more)
    return;
  if (IS_MOB (ch))
    return;
  raw_purge (ch->in_room);
  for (v = ch->in_room->more->people; v != NULL; v = v->next_in_room)
    {
      if (LEVEL (v) > IMM_LEVEL)
        act ("$n cleanses the room with righteous fire!", ch, NULL, v, TO_VICT);
    }
  send_to_char ("You cleanse the room with righteous fire!\n\r", ch);
  return;
}

void
do_advance (CHAR_DATA * ch, char *argy)
{
  char arg1[SML_LENGTH];
  char arg2[SML_LENGTH];
  CHAR_DATA *victim;
  int level;
  int iLevel;
  DEFINE_COMMAND ("advance", do_advance, POSITION_DEAD, MAX_LEVEL, LOG_ALWAYS, "This command advances a char's level.  Their experience is set to 0.  Never use this on players.")

    if (IS_MOB (ch))
    return;
  if (!IS_REAL_GOD (ch))
    return;

  argy = one_argy (argy, arg1);
  argy = one_argy (argy, arg2);
  if (arg1[0] == '\0' || arg2[0] == '\0' || !is_number (arg2))
    {
      send_to_char ("Syntax: advance <char> <level>.\n\r", ch);
      return;
    }
  if ((victim = get_char_room (ch, arg1)) == NULL)
    {
      send_to_char ("That player is not here.\n\r", ch);
      return;
    }

  if ((level = atoi (arg2)) < 1 || level > TOP_LEVEL)
    {
      char buf[STD_LENGTH];
      sprintf (buf, "Level must be 1 to %d.\n\r", TOP_LEVEL);
      send_to_char (buf, ch);
      return;
    }

  if (level > LEVEL (ch) || IS_MOB (victim))
    return;

  if (level < LEVEL (victim))
    {
      int sn;
      send_to_char ("Lowering a player's level!\n\r", ch);
      victim->pcdata->level = 1;
	victim->exp = FIND_EXP (level - 1, 0);
      victim->max_hit = 25;
      victim->max_move = 70;
      for (sn = 0; sn < SKILL_COUNT; sn++)
        victim->pcdata->learned[sn] = -100;
      victim->pcdata->practice = 0;
      victim->hit = victim->max_hit;
      victim->move = victim->max_move;
      advance_level (victim);
    }
  else
    {
      send_to_char ("Raising a player's level!\n\r", ch);
    }
  for (iLevel = LEVEL (victim); iLevel < level; iLevel++)
    {
      send_to_char ("You raise a level! ", victim);
      victim->pcdata->level += 1;
      advance_level (victim);
    }
    victim->exp = FIND_EXP (LEVEL (victim) - 1, 0);
  return;
}

void
do_restore (CHAR_DATA * ch, char *argy)
{
  char arg[SML_LENGTH];
  CHAR_DATA *victim;
  DEFINE_COMMAND ("restore", do_restore, POSITION_DEAD, MAX_LEVEL, LOG_ALWAYS, "This restores hps and mps to a specified character or mob.")

/*if (IS_MOB(ch) && ch!=char_list) return; */
    one_argy (argy, arg);
  if (arg[0] == '\0')
    {
      send_to_char ("Restore whom?\n\r", ch);
      return;
    }
  if ((victim = get_char_world_2 (ch, arg)) == NULL)
    {
      send_to_char ("They aren't here.\n\r", ch);
      return;
    }

  MAXHIT(victim);
  MAXMOVE(victim);
  update_pos (victim);
  send_to_char ("Ok.\n\r", ch);
  return;
}

void
do_freeze (CHAR_DATA * ch, char *argy)
{
  char arg[SML_LENGTH];
  CHAR_DATA *victim;
  DEFINE_COMMAND ("freeze", do_freeze, POSITION_DEAD, MAX_LEVEL, LOG_ALWAYS, "This command freezes the specified player; they can't do *anything*.")

    if (IS_MOB (ch))
    return;
  one_argy (argy, arg);
  if (arg[0] == '\0')
    {
      send_to_char ("Freeze whom?\n\r", ch);
      return;
    }
  if ((victim = get_char_world (ch, arg)) == NULL)
    {
      send_to_char ("They aren't here.\n\r", ch);
      return;
    }
  if (IS_MOB (victim))
    return;

  if (LEVEL (victim) >= LEVEL (ch))
    return;

  if (IS_SET (victim->act, PLR_FREEZE))
    {
      REMOVE_BIT (victim->act, PLR_FREEZE);
      send_to_char ("You have been defrosted.\n\r", victim);
      send_to_char ("FREEZE removed.\n\r", ch);
    }
  else
    {
      SET_BIT (victim->act, PLR_FREEZE);
      send_to_char ("You have been frozen!\n\r", victim);
      send_to_char ("FREEZE set.\n\r", ch);
    }
  save_char_obj (victim);
  return;
}
void
do_noclan (CHAR_DATA * ch, char *argy)
{
  char arg[SML_LENGTH];
  CHAR_DATA *victim;
  DEFINE_COMMAND ("noclan", do_noclan, POSITION_DEAD, MAX_LEVEL, LOG_ALWAYS, "This command noclans the specified player; they can't do *anything*.")

    if (IS_MOB (ch))
    return;
  one_argy (argy, arg);
  if (arg[0] == '\0')
    {
      send_to_char ("Noclan whom?\n\r", ch);
      return;
    }
  if ((victim = get_char_world (ch, arg)) == NULL)
    {
      send_to_char ("They aren't here.\n\r", ch);
      return;
    }
  if (IS_MOB (victim))
    return;

  if (LEVEL (victim) >= LEVEL (ch))
    return;

  if (IS_SET (victim->act, PLR_NOCLAN))
    {
      REMOVE_BIT (victim->act, PLR_NOCLAN);
      send_to_char ("You have been defrosted.\n\r", victim);
      send_to_char ("NOCLAN removed.\n\r", ch);
    }
  else
    {
      SET_BIT (victim->act, PLR_NOCLAN);
      send_to_char ("You have been frozen!\n\r", victim);
      send_to_char ("NOCLAN set.\n\r", ch);
    }
  save_char_obj (victim);
  return;
}
void
do_lamer (CHAR_DATA * ch, char *argy)
{
  char arg[SML_LENGTH];
  CHAR_DATA *victim;
  DEFINE_COMMAND ("lamer", do_lamer, POSITION_DEAD, MAX_LEVEL, LOG_ALWAYS, "This command lamers the specified player; they can't do *anything*.")

    if (IS_MOB (ch))
    return;
  one_argy (argy, arg);
  if (arg[0] == '\0')
    {
      send_to_char ("Lamer whom?\n\r", ch);
      return;
    }
  if ((victim = get_char_world (ch, arg)) == NULL)
    {
      send_to_char ("They aren't here.\n\r", ch);
      return;
    }
  if (IS_MOB (victim))
    return;

  if (LEVEL (victim) >= LEVEL (ch))
    return;

  if (IS_SET (victim->act, PLR_LAMER))
    {
      REMOVE_BIT (victim->act, PLR_LAMER);
      send_to_char ("You have been defrosted.\n\r", victim);
      send_to_char ("LAMER removed.\n\r", ch);
    }
  else
    {
      SET_BIT (victim->act, PLR_LAMER);
      send_to_char ("You have been frozen!\n\r", victim);
      send_to_char ("LAMER set.\n\r", ch);
    }
  save_char_obj (victim);
  return;
}
void
do_shitlist (CHAR_DATA * ch, char *argy)
{
  char arg[SML_LENGTH];
  CHAR_DATA *victim;
  DEFINE_COMMAND ("shitlist", do_shitlist, POSITION_DEAD, MAX_LEVEL, LOG_ALWAYS, "This command shitlists the specified player; they can't do *anything*.")

    if (IS_MOB (ch))
    return;
  one_argy (argy, arg);
  if (arg[0] == '\0')
    {
      send_to_char ("Shitlist whom?\n\r", ch);
      return;
    }
  if ((victim = get_char_world (ch, arg)) == NULL)
    {
      send_to_char ("They aren't here.\n\r", ch);
      return;
    }
  if (IS_MOB (victim))
    return;

  if (LEVEL (victim) >= LEVEL (ch))
    return;

  if (IS_SET (victim->act, PLR_FREEZE))
    {
      REMOVE_BIT (victim->act, PLR_FREEZE);
      send_to_char ("You have been defrosted.\n\r", victim);
      send_to_char ("SHITLIST removed.\n\r", ch);
    }
  else
    {
      SET_BIT (victim->act, PLR_FREEZE);
      send_to_char ("You have been frozen!\n\r", victim);
      send_to_char ("SHITLIST set.\n\r", ch);
    }
  save_char_obj (victim);
  return;
}

void
do_log (CHAR_DATA * ch, char *argy)
{
  char arg[SML_LENGTH];
  CHAR_DATA *victim;
  DEFINE_COMMAND ("log", do_log, POSITION_DEAD, MAX_LEVEL, LOG_ALWAYS, "This command can be used to log a player or multiple players.")

    if (IS_MOB (ch))
    return;
  one_argy (argy, arg);
  if (arg[0] == '\0')
    {
      send_to_char ("Log whom?\n\r", ch);
      return;
    }
  if (!str_cmp (arg, "all"))
    {
      if (fLogAll)
        {
          fLogAll = FALSE;
          send_to_char ("Log ALL off.\n\r", ch);
        }
      else
        {
          fLogAll = TRUE;
          send_to_char ("Log ALL on.\n\r", ch);
        }
      return;
    }
  if ((victim = get_char_world (ch, arg)) == NULL)
    {
      send_to_char ("They aren't here.\n\r", ch);
      return;
    }
  if (IS_MOB (victim))
    return;

  if (IS_SET (victim->act, PLR_LOG))
    {
      REMOVE_BIT (victim->act, PLR_LOG);
      send_to_char ("LOG removed.\n\r", ch);
    }
  else
    {
      SET_BIT (victim->act, PLR_LOG);
      send_to_char ("LOG set.\n\r", ch);
    }
  return;
}

void
do_silence (CHAR_DATA * ch, char *argy)
{
  char arg[SML_LENGTH];
  CHAR_DATA *victim;
  DEFINE_COMMAND ("silence", do_silence, POSITION_DEAD, MAX_LEVEL, LOG_ALWAYS, "This command removes all channel privaledges but 'say' from a specified player.")

    if (IS_MOB (ch))
    return;
  one_argy (argy, arg);
  if (arg[0] == '\0')
    {
      send_to_char ("Silence whom?", ch);
      return;
    }
  if ((victim = get_char_world (ch, arg)) == NULL)
    {
      send_to_char ("They aren't here.\n\r", ch);
      return;
    }
  if (IS_MOB (victim))
    return;

  if (LEVEL (victim) >= LEVEL (ch))
    {
      send_to_char ("You failed.\n\r", ch);
      return;
    }
  if (IS_SET (victim->pcdata->act2, PLR_SILENCE))
    {
      REMOVE_BIT (victim->pcdata->act2, PLR_SILENCE);
      send_to_char ("SILENCE removed.\n\r", ch);
    }
  else
    {
      SET_BIT (victim->pcdata->act2, PLR_SILENCE);
      send_to_char ("SILENCE set.\n\r", ch);
    }
  return;
}

void
do_peace (CHAR_DATA * ch, char *argy)
{
  CHAR_DATA *rch;
  DEFINE_COMMAND ("peace", do_peace, POSITION_DEAD, IMM_LEVEL, LOG_NORMAL, "This command stops all the fighting that is going on in the current room.")
    check_room_more (ch->in_room);
  for (rch = ch->in_room->more->people; rch != NULL; rch = rch->next_in_room)
    {
      if (FIGHTING (rch) != NULL)
        {
          rch->fgt->fighting = NULL;
          NEW_POSITION(rch, POSITION_STANDING);
          if (rch->fgt->hunting != NULL)
            {
              free_string (rch->fgt->hunting);
              rch->fgt->hunting = NULL;
            }
	  if (rch->fgt->phunting !=NULL) /* JRAJRA - track stuff */
	    {
	      free_string (rch->fgt->phunting);
	      rch->fgt->phunting = NULL;
	    }
          update_pos (rch);
        }
    }
  return;
}

BAN_DATA *ban_free;
BAN_DATA *ban_list;

void
do_ban (CHAR_DATA * ch, char *argy)
{
  char buf[STD_LENGTH];
  char arg[SML_LENGTH];
  BAN_DATA *pban;
  DEFINE_COMMAND ("siteban", do_ban, POSITION_DEAD, MAX_LEVEL, LOG_ALWAYS, "This command shows and/or adds ban info.  See nban for marking newbie problem sites.")

    if (IS_MOB (ch))
    return;
  one_argy (argy, arg);
  if (arg[0] == '\0')
    {
      strcpy (buf, "Banned sites:\n\r");
      for (pban = ban_list; pban != NULL; pban = pban->next)
        {
          if (pban->newbie)
            strcat (buf, "(\x1B[1;37mN\x1B[37;0m) ");
          else
            strcat (buf, " ");
          strcat (buf, pban->name);
          strcat (buf, "\n\r");
        }
      send_to_char (buf, ch);
      return;
    }
  for (pban = ban_list; pban != NULL; pban = pban->next)
    {
      if (!str_cmp (arg, pban->name))
        {
          send_to_char ("That site is already banned!\n\r", ch);
          return;
        }
    }
  if (ban_free == NULL)
    {
      pban = mem_alloc (sizeof (*pban));
    }
  else
    {
      pban = ban_free;
      ban_free = ban_free->next;
    }
  pban->name = str_dup (arg);
  pban->next = ban_list;
  pban->newbie = FALSE;
  ban_list = pban;
  send_to_char ("Ok.\n\r", ch);
  save_banlist ();
  return;
}

void
bansite (char *argy)
{
  char arg[SML_LENGTH];
  BAN_DATA *pban;
  one_argy (argy, arg);
  for (pban = ban_list; pban != NULL; pban = pban->next)
    {
      if (!str_cmp (arg, pban->name))
        {
          return;
        }
    }
  if (ban_free == NULL)
    {
      pban = mem_alloc (sizeof (*pban));
    }
  else
    {
      pban = ban_free;
      ban_free = ban_free->next;
    }
  pban->name = str_dup (arg);
  pban->next = ban_list;
  pban->newbie = FALSE;
  ban_list = pban;
  return;
}

void
do_nban (CHAR_DATA * ch, char *argy)
{
  char buf[STD_LENGTH];
  char arg[SML_LENGTH];
  BAN_DATA *pban;
  DEFINE_COMMAND ("nban", do_nban, POSITION_DEAD, MAX_LEVEL, LOG_ALWAYS, "This command marks a site as a newbie problem site.  All new chars from this site must do registration.")
    if (IS_MOB (ch))
    return;
  one_argy (argy, arg);
  if (arg[0] == '\0')
    {
      strcpy (buf, "Banned sites:\n\r");
      for (pban = ban_list; pban != NULL; pban = pban->next)
        {
          if (pban->newbie)
            strcat (buf, "(\x1B[1;37mN\x1B[37;0m) ");
          else
            strcat (buf, " ");
          strcat (buf, pban->name);
          strcat (buf, "\n\r");
        }
      send_to_char (buf, ch);
      return;
    }
  for (pban = ban_list; pban != NULL; pban = pban->next)
    {
      if (!str_cmp (arg, pban->name))
        {
          send_to_char ("That site is already banned!\n\r", ch);
          return;
        }
    }
  if (ban_free == NULL)
    {
      pban = mem_alloc (sizeof (*pban));
    }
  else
    {
      pban = ban_free;
      ban_free = ban_free->next;
    }
  pban->name = str_dup (arg);
  pban->next = ban_list;
  pban->newbie = TRUE;
  ban_list = pban;
  send_to_char ("Ok.\n\r", ch);
  save_banlist ();
  return;
}

void
nbansite (char *argy)
{
  char arg[SML_LENGTH];
  BAN_DATA *pban;
  one_argy (argy, arg);
  for (pban = ban_list; pban != NULL; pban = pban->next)
    {
      if (!str_cmp (arg, pban->name))
        {
          return;
        }
    }
  if (ban_free == NULL)
    {
      pban = mem_alloc (sizeof (*pban));
    }
  else
    {
      pban = ban_free;
      ban_free = ban_free->next;
    }
  pban->name = str_dup (arg);
  pban->next = ban_list;
  pban->newbie = TRUE;
  ban_list = pban;
  return;
}

void
do_allow (CHAR_DATA * ch, char *argy)
{
  char arg[SML_LENGTH];
  BAN_DATA *prev;
  BAN_DATA *curr;
  DEFINE_COMMAND ("allow", do_allow, POSITION_DEAD, MAX_LEVEL, LOG_ALWAYS, "This command removes a ban site from the list.")

    if (IS_MOB (ch))
    return;
  one_argy (argy, arg);
  if (arg[0] == '\0')
    {
      send_to_char ("Remove which site from the ban list?\n\r", ch);
      return;
    }
  prev = NULL;
  for (curr = ban_list; curr != NULL; prev = curr, curr = curr->next)
    {
      if (!str_cmp (arg, curr->name))
        {
          if (prev == NULL)
            ban_list = ban_list->next;
          else
            prev->next = curr->next;
          free_string (curr->name);
          curr->next = ban_free;
          ban_free = curr;
          send_to_char ("Ok.\n\r", ch);
          return;
        }
    }
  save_banlist();
  send_to_char ("Site is not banned.\n\r", ch);
  return;
}

void
do_wizlock (CHAR_DATA * ch, char *argy)
{
  extern bool wizlock;
  DEFINE_COMMAND ("wizlock", do_wizlock, POSITION_DEAD, MAX_LEVEL, LOG_ALWAYS, "This makes it so only gods can log in.")

    wizlock = !wizlock;
  if (wizlock)
    send_to_char ("Game wizlocked.\n\r", ch);
  else
    send_to_char ("Game un-wizlocked.\n\r", ch);
  return;
}

void
do_slookup (CHAR_DATA * ch, char *argy)
{
  char buf[STD_LENGTH];
  char arg[SML_LENGTH];
  int sn;
  SPELL_DATA *spell;
  DEFINE_COMMAND ("slookup", do_slookup, POSITION_DEAD, IMM_LEVEL, LOG_NORMAL, "Looks up a skill or spell number, or slookup 'all', or slookup find <snum>.")

    if (IS_MOB (ch))
    return;
  argy = one_argy (argy, arg);
  if (arg[0] == '\0')
    {
      send_to_char ("Slookup what?\n\r", ch);
      return;
    }
  if (is_number (arg))
    {
      sn = atoi (arg);
      if ((spell = skill_lookup (NULL, sn)) == NULL)
        {
          send_to_char ("Invalid Spell/Skill Number.\n\r", ch);
          return;
        }
      sprintf (buf, "Spell/skill number: %4d  Skill/spell: '%s' Aka: '%s'\n\r",
               sn, /*spell->slot,*/ spell->spell_name, spell->spell_funky_name);
      send_to_char (buf, ch);
      return;
    }
  if (!str_cmp (arg, "all"))
    {
      for (sn = 0; sn < SKILL_COUNT; sn++)
        {
          if ((spell = skill_lookup (NULL, sn)) == NULL)
            continue;
          sprintf (buf, "Spell/skill number: %4d  Skill/spell: '%s' Aka: '%s'\n\r",
               sn, /*spell->slot,*/ spell->spell_name, spell->spell_funky_name);
          send_to_char (buf, ch);
        }
    }
  else
    {
      if ((spell = skill_lookup (arg, -1)) == NULL)
        {
          int cntr, subctr;
          char buffy[500];
          SPELL_DATA *spl;
          MOB_PROTOTYPE *mid;
          if (!str_cmp (arg, "find") && is_number (argy))
            {
              if (!IS_REAL_GOD (ch))
                return;
              sn = atoi (argy);
              if ((spl = skill_lookup (NULL, sn)) == NULL)
                {
                  send_to_char ("Invalid!\n\r", ch);
                  return;
                }
              sprintf (buffy, "Mobs that teach sn %d (\x1B[1m%s\x1B[0m)...\n\r", sn, spl->spell_name);
              send_to_char (buffy, ch);
              for (cntr = 1; cntr < top_vnum_mob; cntr++)
                {
                  if ((mid = get_mob_index (cntr)) == NULL || !mid->opt)
                    continue;
                  for (subctr = 0; subctr < 30; subctr++)
                    {
                      if (mid->opt->skltaught[subctr] == sn)
                        {
                          sprintf (buffy, "\x1B[1;37m%-20s\x1B[37;0m vnum \x1B[1;32m%d\x1B[37;0m, teaches this skill/spell.\n\r", mid->short_descr, mid->vnum);
                          send_to_char (buffy, ch);
                        }
                    }
                }
            }
          else
            send_to_char ("No such skill or spell.\n\r", ch);
          return;
        }
      sprintf (buf, "Spell/skill number: %4d  Skill/spell: '%s' Aka: '%s'\n\r",
       spell->gsn, spell->spell_name, spell->spell_funky_name);
      send_to_char (buf, ch);
    }
  return;
}

void
do_award_skill (CHAR_DATA * ch, char *argy)
{
  char arg1[500];
  char arg2[500];
  char arg3[500];
  SPELL_DATA *spl;
  DESCRIPTOR_DATA *dd;
  int amount;
  DEFINE_COMMAND ("awardskill", do_award_skill, POSITION_DEAD, MAX_LEVEL, LOG_ALWAYS, "Awards a skill to a player.  Mostly used for quests.")

    argy = one_argy (argy, arg1);
  argy = one_argy (argy, arg2);
  argy = one_argy (argy, arg3);
  if ((amount = atoi (arg3)) < 1)
    return;
  if ((spl = skill_lookup (arg2, -1)) == NULL)
    return;
  for (dd = descriptor_list; dd != NULL; dd = dd->next)
    {
      if (!dd->character)
        continue;
      if (!str_cmp (NAME (dd->character), arg1))
        goto found;
    }
  return;
found:
  if (dd->character->pcdata->learned[spl->gsn] < 0)
    dd->character->pcdata->learned[spl->gsn] = 1;
  if (dd->character->pcdata->learned[spl->gsn] > 80)
    amount /= 5;
  else if (dd->character->pcdata->learned[spl->gsn] > 65)
    amount /= 4;
  else if (dd->character->pcdata->learned[spl->gsn] > 50)
    amount /= 3;
  else if (dd->character->pcdata->learned[spl->gsn] > 35)
    amount /= 2;
  dd->character->pcdata->learned[spl->gsn] = UMIN (100, (dd->character->pcdata->learned[spl->gsn] + amount));
  return;
}

void
do_sset (CHAR_DATA * ch, char *argy)
{
  char arg1[SML_LENGTH];
  char arg2[SML_LENGTH];
  char arg3[SML_LENGTH];
  CHAR_DATA *victim;
  int value;
  int sn;
  bool fAll;
  SPELL_DATA *spell;
  DEFINE_COMMAND ("sset", do_sset, POSITION_DEAD, MAX_LEVEL, LOG_ALWAYS, "This sets a person's skills/spells (all or specific skill, and amount).")

    if (IS_PLAYER (ch) && LEVEL (ch) < MAX_LEVEL)
    return;
  argy = one_argy (argy, arg1);
  argy = one_argy (argy, arg2);
  argy = one_argy (argy, arg3);
  if (arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0')
    {
      send_to_char ("Syntax: sset <victim> <skill> <value>\n\r", ch);
      send_to_char ("or: sset <victim> all <value>\n\r", ch);
      return;
    }
  if ((victim = get_char_world (ch, arg1)) == NULL)
    {
      send_to_char ("They aren't here.\n\r", ch);
      return;
    }
  if (IS_MOB (victim))
    {
      send_to_char ("Not on mobs.\n\r", ch);
      return;
    }
  if (LEVEL (victim) < IMM_LEVEL && LEVEL (ch) < MAX_LEVEL)
    {
      send_to_char ("You may not set players' skills!\n\r", ch);
      send_to_char ("Cheat attempt logged!\n\r", ch);
      return;
    }
  fAll = !str_cmp (arg2, "all");
  spell = skill_lookup (arg2, -1);
  sn = 0;
  if (!fAll && spell == NULL)
    {
      send_to_char ("No such skill or spell.\n\r", ch);
      return;
    }
  if (!is_number (arg3))
    {
      send_to_char ("Value must be numeric.\n\r", ch);
      return;
    }
  value = atoi (arg3);
  if (value < 0 || value > 100)
    {
      send_to_char ("Value range is 0 (unlearned) to 100.\n\r", ch);
      return;
    }
  if (fAll)
    {
      for (sn = 0; sn < SKILL_COUNT; sn++)
        {
          if (skill_lookup (NULL, sn) != NULL)
            victim->pcdata->learned[sn] = (value == 0 ? -100 : value);
        }
      return;
    }
  else
    {
      victim->pcdata->learned[spell->gsn] = (value == 0 ? -100 : value);
    }
  return;
}

void
do_mset (CHAR_DATA * ch, char *argy)
{
  char arg1[SML_LENGTH];
  char arg2[SML_LENGTH];
  char arg3[SML_LENGTH];
  CHAR_DATA *victim;
  int value;
  DEFINE_COMMAND ("mset", do_mset, POSITION_DEAD, MAX_LEVEL, LOG_NORMAL, "Sets a specific mob's stats and stuff...")

    if (IS_MOB (ch))
    return;
  if (!IS_REAL_GOD (ch))
    {
      send_to_char ("Command has been removed.\n\r", ch);
      return;
    }
  smash_tilde (argy);
  argy = one_argy (argy, arg1);
  argy = one_argy (argy, arg2);
  strcpy (arg3, argy);
  if (arg1[0] == '\0' || arg2[0] == '\0')
    {
      send_to_char ("Syntax: mset <victim> <field> <value>\n\r", ch);
      send_to_char ("or: mset <victim> <string> <value>\n\r", ch);
      send_to_char ("\n\r", ch);
      send_to_char ("Field being one of:\n\r", ch);
      send_to_char (" sex level\n\r", ch);
      send_to_char (" hp move timer\n\r", ch);
      send_to_char (" align master leader practice\n\r", ch);
      send_to_char (" act affect hunting\n\r", ch);
      send_to_char ("\n\r", ch);
      send_to_char ("String being one of:\n\r", ch);
      send_to_char (" name short long description spec\n\r", ch);
      return;
    }
  if ((victim = get_char_world (ch, arg1)) == NULL)
    {
      send_to_char ("They aren't here.\n\r", ch);
      return;
    }
  if (IS_PLAYER (victim))
    {
      send_to_char ("Not on players.. only mobs.\n\r", ch);
      return;
    }
  if (LEVEL (ch) < MAX_LEVEL && victim->in_room && victim->in_room->area->open != 0)
    {
      send_to_char ("You cannot set mobs that are in open areas!\n\r", ch);
      send_to_char ("Possible cheat attempt logged.\n\r", ch);
      return;
    }
  value = is_number (arg3) ? atoi (arg3) : -1;
  if (!str_cmp (arg2, "hp") && arg3[0] != '\0')
    {
      if (value < -10 || value > 2000000000)
        {
          send_to_char ("Hp range is -10 to 2,000,000,000 hit points.\n\r", ch);
          return;
        }
      victim->max_hit = value;
      return;
    }
  if (!str_cmp (arg2, "move") && arg3[0] != '\0')
    {
      if (value < 0 || value > 30000)
        {
          send_to_char ("Move range is 0 to 30,000 move points.\n\r", ch);
          return;
        }
      victim->max_move = value;
      return;
    }
  if (!str_cmp (arg2, "timer") && arg3[0] != '\0')
    {
      if (value < 0 || value > 30000)
        {
          send_to_char ("Timer range is 0 to 30,000.\n\r", ch);
          return;
        }
      victim->timer = value;
      return;
    }
  if (!str_cmp (arg2, "practice") && arg3[0] != '\0')
    {
      if (value < 0 || value > 100)
        {
          send_to_char ("Practice range is 0 to 100 sessions.\n\r", ch);
          return;
        }
      victim->pcdata->practice = value;
      return;
    }
  if (!str_cmp (arg2, "hunting") && arg3[0] != '\0' && strlen (arg3) < 16)
    {
      check_fgt (victim);
      if (victim->fgt->hunting != NULL)
        {
          free_string (victim->fgt->hunting);
          victim->fgt->hunting = NULL;
        }
      victim->fgt->hunting = str_dup (arg3);
      send_to_char ("Done.\n\r", ch);
      return;
    }
  if (!str_cmp (arg2, "master") && arg3[0] != '\0')
    {
      check_fgt (victim);
      if (!str_cmp (arg3, "none"))
        {
          victim->fgt->master = NULL;
          return;
        }
      if ((get_char_world (ch, arg3)) == NULL)
        {
          send_to_char ("They aren't here.\n\r", ch);
          return;
        }
      victim->fgt->master = get_char_world (ch, arg3);
      return;
    }
  if (!str_cmp (arg2, "leader") && arg3[0] != '\0')
    {
      check_fgt (victim);
      if (!str_cmp (arg3, "none"))
        {
          victim->fgt->leader = NULL;
          return;
        }
      if ((get_char_world (ch, arg3)) == NULL)
        {
          send_to_char ("They aren't here.\n\r", ch);
          return;
        }
      victim->fgt->leader = get_char_world (ch, arg3);
      return;
    }
  if (!str_cmp (arg2, "affect") && arg3[0] != '\0')
    {
      victim->affected_by = value;
      return;
    }
  if (!str_cmp (arg2, "spec") && arg3[0] != '\0')
    {
      if (spec_lookup (arg3) == NULL)
        {
          send_to_char ("No such spec fun.\n\r", ch);
          return;
        }
      victim->pIndexData->spec_fun = spec_lookup (arg3);
      victim->pIndexData->spec_name = str_dup (arg3);
      return;
    }
  do_mset (ch, "");
  return;
}

void
do_oset (CHAR_DATA * ch, char *argy)
{
  char arg1[SML_LENGTH];
  char arg2[SML_LENGTH];
  char arg3[SML_LENGTH];
  SINGLE_OBJECT *obj;
  int value;
  DEFINE_COMMAND ("oset", do_oset, POSITION_DEAD, MAX_LEVEL, LOG_NORMAL, "Sets a specific object's stats and stuff...")

    if (IS_MOB (ch))
    return;
  if (LEVEL (ch) < MAX_LEVEL)
    {
      send_to_char ("Command has been removed due to abuse!\n\r", ch);
      return;
    }
  smash_tilde (argy);
  argy = one_argy (argy, arg1);
  argy = one_argy (argy, arg2);
  strcpy (arg3, argy);
  if (arg1[0] == '\0' || arg2[0] == '\0')
    {
      send_to_char ("Syntax: oset <object> <field> <value>\n\r", ch);
      send_to_char ("or: oset <object> <string> <value>\n\r", ch);
      send_to_char ("\n\r", ch);
      send_to_char ("Field being one of:\n\r", ch);
      send_to_char (" extra wear level weight\n\r", ch);
      send_to_char (" cost type timer\n\r", ch);
      send_to_char ("\n\r", ch);
      send_to_char ("String being one of:\n\r", ch);
      send_to_char (" name short long ed\n\r", ch);
      return;
    }
  if ((obj = get_obj_world (ch, arg1)) == NULL)
    {
      send_to_char ("Nothing like that in hell, earth, or heaven.\n\r", ch);
      return;
    }
  value = atoi (arg3);

  if (!str_cmp (arg2, "short") && arg3[0] != '\0')
    {
      if (obj->short_descr != NULL)
        free_string (obj->short_descr);
      obj->short_descr = str_dup (arg3);
      return;
    }
  if (!str_cmp (arg2, "long"))
    {
      send_to_char ("Entering line editing mode.\n\r", ch);
      send_to_char ("Terminate with a ~ on a blank line.\n\r", ch);
      ch->desc->pString = &obj->description;
      return;
    }
  if (!str_cmp (arg2, "cost") && arg3[0] != '\0')
    {
      obj->cost = value;
      return;
    }
  if (!str_cmp (arg2, "timer") && arg3[0] != '\0')
    {
      obj->timer = value;
      return;
    }

  do_oset (ch, "");
  return;
}

void
do_pset (CHAR_DATA * ch, char *argy)
{
  bool ad=FALSE;
  bool sb=FALSE;
  char arg1[SML_LENGTH];
  char arg2[SML_LENGTH];
  char arg3[SML_LENGTH];
  CHAR_DATA *victim;
  int value;
  DEFINE_COMMAND ("pset", do_pset, POSITION_DEAD, MAX_LEVEL, LOG_ALWAYS, "Changes a player's info...")

    smash_tilde (argy);
  argy = one_argy (argy, arg1);
  argy = one_argy (argy, arg2);
  strcpy (arg3, argy);
  if (arg1[0] == '\0' || arg2[0] == '\0')
    {
      send_to_char ("Syntax: pset <victim> <field> <value>\n\r", ch);
      send_to_char ("\n\r", ch);
      send_to_char ("Options:\n\r", ch);
      send_to_char (" (Use pedit to set attribs) sex security killpts AC compflag\n\r", ch);
      send_to_char (" hp move practice align race height removexp removetp\n\r", ch);
      send_to_char (" thirst guild drunk full master leader learn addxp addtp\n\r", ch);
      send_to_char (" ipflag tflag warpoints deaths (fly/breath/d_hid/d_invis/infra) tp\n\r", ch);
      send_to_char (" short long description title remort str con dex int wis\n\r", ch);
      send_to_char (" align_fire align_water align_air hardcore\n\r", ch);
      return;
    }
  if ((victim = get_char_world_2 (ch, arg1)) == NULL)
    {
      send_to_char ("They aren't here.\n\r", ch);
      return;
    }
  if (IS_MOB (victim))
    {
      send_to_char ("Not on mobs.\n\r", ch);
      return;
    }
  if (!str_cmp ("scriptflag", arg2))
    {
      int ik;
      argy = one_argy (argy, arg3);
      if (!str_cmp (argy, "off"))
        {
          for (ik = 0; ik < MAX_SCRIPT_FLAGS; ik++)
            {
              if (victim->pcdata->script_flags[ik] && victim->pcdata->script_flags[ik][0] != '\0' &&
                  !str_cmp (victim->pcdata->script_flags[ik], arg3))
                {
                  free_string (victim->pcdata->script_flags[ik]);
                  victim->pcdata->script_flags[ik] = NULL;
                  return;
                }
            }
        }
      if (!str_cmp (argy, "on"))
        {
          for (ik = 0; ik < MAX_SCRIPT_FLAGS; ik++)
            {
              if (!victim->pcdata->script_flags[ik])
                continue;
              if (!str_cmp (victim->pcdata->script_flags[ik], arg3))
                return;
            }
          for (ik = 0; ik < MAX_SCRIPT_FLAGS; ik++)
            {
              if (victim->pcdata->script_flags[ik] && victim->pcdata->script_flags[ik][0] != '\0')
                continue;
              victim->pcdata->script_flags[ik] = str_dup (arg3);
              return;
            }
        }
      return;
    }
  if (arg3[0] == '-') {
    char *rr=arg3;
    value=(0-(atoi((rr+1))));
    sb=TRUE;
    value = atoi (arg3);
    }
  else
  if (arg3[0] == '+') {
        char *rr=arg3;
        ad=TRUE;
        value=atoi((rr+1));
        } else
    value = is_number (arg3) ? atoi (arg3) : -1;

  if (!str_cmp (arg2, "hardcore")) {
    if (IS_MOB (ch))
    return;
    if (IS_SET (victim->pcdata->act2, PLR_HARDCORE))
    {
      REMOVE_BIT (victim->pcdata->act2, PLR_HARDCORE);
      send_to_char ("Hardcore mode off.\n\r", ch);
    }
    else
    {
      SET_BIT (victim->pcdata->act2, PLR_HARDCORE);
      send_to_char ("Hardcore mode on.\n\r", ch);
    }
    return;
  }
  // Set elemental alignment
  if (!str_cmp (arg2, "align_fire")) {

    victim->pcdata->elemental_alignment ^= ELEMENTAL_ALIGN_FIRE;
    send_to_char ("Fire Alignment Toggled\n\r",ch);
    return;
  }
  if (!str_cmp (arg2, "align_water")) {

    victim->pcdata->elemental_alignment ^= ELEMENTAL_ALIGN_WATER;
    send_to_char ("Water Alignment Toggled\n\r",ch);
    return;
  }
  if (!str_cmp (arg2, "align_air")) {

    victim->pcdata->elemental_alignment ^= ELEMENTAL_ALIGN_AIR;
    send_to_char ("Air Alignment Toggled\n\r",ch);
    return;
  }
  if (!str_cmp (arg2, "align_earth")) {

    victim->pcdata->elemental_alignment ^= ELEMENTAL_ALIGN_EARTH;
    send_to_char ("Earth Alignment Toggled\n\r",ch);
    return;
  }
  if (!str_cmp (arg2, "align_good")) {

    victim->pcdata->elemental_alignment ^= ELEMENTAL_ALIGN_GOOD;
    send_to_char ("Good Alignment Toggled\n\r",ch);
    return;
  }
  if (!str_cmp (arg2, "align_evil")) {

    victim->pcdata->elemental_alignment ^= ELEMENTAL_ALIGN_EVIL;
    send_to_char ("Evil Alignment Toggled\n\r",ch);
    return;
  }
 

  if (!str_cmp (arg2, "level"))
    {
      if (value < 1 || value > IMM_LEVEL-1)
        return;
      
      victim->exp = (FIND_EXP ((value - 1), 1));
      send_to_char ("Level set.\n\r", ch);
      send_to_char ("The next mob you hit, you will raise in level.\n\r", victim);
      return;
    }
  

  if (!str_cmp (arg2, "remort"))
    {
      if (is_number(arg3))
	{
	  victim->pcdata->remort_times = UMIN(200, UMAX(0, atoi(arg3)));
	  send_to_char("Ok remort times changed.\n\r", ch);
	  return;
	}
      else
	{
	  send_to_char("pset <name> remort <number>.\n\r", ch);
	  return;
	}
    }
  


  if (!str_cmp (arg2, "str"))
    {
      if (is_number(arg3))
	{
	  victim->pcdata->stat[STR] = UMIN(32, UMAX(3, atoi(arg3)));
	  send_to_char("Ok str changed.\n\r", ch);
	  return;
	}
      else
	{
	  send_to_char("pset <name> str <number>.\n\r", ch);
	  return;
	}
      
    }
  if (!str_cmp (arg2, "int"))
    {
      if (is_number(arg3))
	{
	  victim->pcdata->stat[INT] = UMIN(32, UMAX(3, atoi(arg3)));
	  send_to_char("Ok int changed.\n\r", ch);
	  return;
	}
      else
	{
	  send_to_char("pset <name> int <number>.\n\r", ch);
	  return;
	}
      
    }
if (!str_cmp (arg2, "wis"))
    {
      if (is_number(arg3))
	{
	  victim->pcdata->stat[WIS]= UMIN(32, UMAX(3, atoi(arg3)));
	  send_to_char("Ok wis changed.\n\r", ch);
	  return;
	}
      else
	{
	  send_to_char("pset <name> wis <number>.\n\r", ch);
	  return;
	}
      
    }
if (!str_cmp (arg2, "dex"))
    {
      if (is_number(arg3))
	{
	  victim->pcdata->stat[DEX] = UMIN(32, UMAX(3, atoi(arg3)));
	  send_to_char("Ok dex changed.\n\r", ch);
	  return;
	}
      else
	{
	  send_to_char("pset <name> dex <number>.\n\r", ch);
	  return;
	}
      
    }

if (!str_cmp (arg2, "con"))
    {
      if (is_number(arg3))
	{
	  victim->pcdata->stat[CON] = UMIN(32, UMAX(3, atoi(arg3)));
	  send_to_char("Ok con changed.\n\r", ch);
	  return;
	}
      else
	{
	  send_to_char("pset <name> con <number>.\n\r", ch);
	  return;
	}
      
    }
  if (!str_cmp (arg2, "hitroll") && arg3[0] != '\0')
    {
      if (value < 0 || value > 140)
        {
          send_to_char ("Valid range for hitroll is 0 - 140.\n\r", ch);
          return;
        }
      victim->hitroll = value;
      send_to_char ("Victim base hitroll set.\n\r", ch);
      return;
    }
  if (!str_cmp (arg2, "height") && arg3[0] != '\0')
    {
      if (value < 1 || value > 32000)
        {
          send_to_char ("Valid range for height is 1 - 32000.\n\r", ch);
          return;
        }
      victim->height = value;
      send_to_char ("Victim height set.\n\r", ch);
      return;
    }
  
  if (!str_cmp (arg2, "remove_hp") && arg3[0] != '\0')
    {
      if (value <= 0)
        {
          return;
        }
      victim->hit -= value;
      if (victim->hit < 1)
        victim->hit = 1;
      return;
    }
  if (!str_cmp (arg2, "same_side_pkill"))
    {
      if (IS_SET (victim->act, PLR_SAME_SIDE_PK))
        {
          act ("$N should not same side pk.", ch, NULL, victim, TO_CHAR);
          victim->act -= PLR_SAME_SIDE_PK;
        }
      else
        {
          act ("$N can now same side pk if they want.", ch, NULL, victim, TO_CHAR);
          victim->act += PLR_SAME_SIDE_PK;
        }
      return;
    }
  if (!str_cmp (arg2, "pkilled_this_mort"))
    {
      if (IS_SET (victim->act, PLR_PKED_THIS_MORT))
        {
          act ("$N can not advance without pking.", ch, NULL, victim, TO_CHAR);
          victim->act -= PLR_PKED_THIS_MORT;
        }
      else
        {
          act ("$N can advance this mort without pking now.", ch, NULL, victim, TO_CHAR);
          victim->act += PLR_PKED_THIS_MORT;
        }
      return;
    }
  
  if (!str_cmp (arg2, "removexp") && arg3[0] != '\0')
    {
      char buf[300];
      if (value < 0)
        {
          return;
        }
      victim->exp -= value;
      send_to_char ("Victim xps removed.\n\r", ch);
      sprintf (buf, "%d exp points were removed!\n\r", value);
      send_to_char ("You have been demoted.  ", victim);
      send_to_char (buf, victim);
      return;
    }
  if (!str_cmp (arg2, "addxp") && arg3[0] != '\0')
    {
      if (value < 0)
        {
          return;
        }
      send_to_char ("You feel a bit more experienced!\n\r", victim);
      gain_exp (victim, value);
      send_to_char ("Victim xps awarded.\n\r", ch);
      return;
    }
  
  if (!str_cmp (arg2, "bank") && arg3[0] != '\0')
    {
      if (value < 0 || value > 1000000)
        {
          send_to_char ("Valid range for bank coinage is 0 - 1,000,000.\n\r", ch);
          return;
        }
      victim->pcdata->bank = value;
      send_to_char ("Victim bank coinage set.\n\r", ch);
      return;
    }
  if (!str_cmp (arg2, "damroll") && arg3[0] != '\0')
    {
      if (value < 0 || value > 140)
        {
          send_to_char ("Valid range for damroll is 0 - 140.\n\r", ch);
          return;
        }
      victim->damroll = value;
      send_to_char ("Victim base damroll set.\n\r", ch);
      return;
    }
  if (!str_cmp (arg2, "ac") && arg3[0] != '\0')
    {
      if (value < 0 || value > 140)
        {
          send_to_char ("Valid range for natural armor class is 0 to 140.\n\r", ch);
          return;
        }
      victim->pcdata->nat_armor = value;
      send_to_char ("Natural Armor Class set.\n\r", ch);
      return;
    }

  if (!str_cmp (arg2, "warpoints") && (ad || sb || value!=-1))
    {
      int wp;
      if (ad) wp=victim->pcdata->warpoints+value; else
      if (sb) wp=victim->pcdata->warpoints+value;
      else wp = value;
      victim->pcdata->warpoints = wp;
      send_to_char ("Warpoints set.\n\r", ch);
      return;
    }
  if (!str_cmp (arg2, "total_wps") && (ad || sb || value!=-1))
    {
      int twp;
      if (ad) twp=victim->pcdata->total_wps+value; else
      if (sb) twp=victim->pcdata->total_wps+value;
      else twp = value;
      victim->pcdata->total_wps = twp;
      send_to_char ("Total Warpoints set.\n\r", ch);
      return;
    }
  if (!str_cmp (arg2, "deaths") && is_number (arg3))
    {
      int wp;
      wp = atoi (arg3);
      if (wp < 0)
        return;
      victim->pcdata->deaths = wp;
      send_to_char ("Deaths set.\n\r", ch);
      return;
    }
  if (!str_cmp (arg2, "guild") && arg3[0] != '\0')
    {
      
      int i;
      for (i = 0; str_cmp(guild_data[i].what_you_type, "end_of_list"); i++)
	{
	  if (!str_cmp(guild_data[i].what_you_type, arg3))
	    {
	      char buf[100];
	      sprintf(buf, guild_data[i].what_you_see);
	      if (IS_SET(victim->pcdata->guilds,guild_data[i].mob_guildmaster_bit))
		strcat(buf, " Removed.\n\r");
	      else
		strcat(buf, " Added.\n\r");
	      send_to_char(buf, ch);
	      victim->pcdata->guilds ^= guild_data[i].mob_guildmaster_bit;
	      fix_char(victim);
	      return;
	    }
	}
 
      send_to_char ("Unknown guild.\n\r", ch);
      return;
    }
  if (!str_cmp (arg2, "int") && arg3[0] != '\0')
    {
      if (value < 3 || value > 25)
        {
          send_to_char ("Intelligence range is 3 to 25.\n\r", ch);
          return;
        }
      victim->pcdata->stat[INT] = value;
      return;
    }
  if (!str_cmp (arg2, "killpts") && arg3[0] != '\0')
    {
      if (value < 0 || value > 30000)
        {
          send_to_char ("Killpoint range is 0 to 30,000 killpoints.\n\r", ch);
          return;
        }
      victim->pcdata->killpoints = value;
      return;
    }
  if (!str_cmp (arg2, "sex") && arg3[0] != '\0')
    {
      if (value < 0 || value > 2)
        {
          send_to_char ("Sex range is 0 to 2.\n\r", ch);
          return;
        }
      victim->pcdata->sex = value;
      return;
    }
  if (!str_cmp (arg2, "race") && arg3[0] != '\0')
    {
      if (value < 0 || value >= RACE_COUNT)
        {
          char buf[STD_LENGTH];
          sprintf (buf, "Race range is 0 to %d.\n", RACE_COUNT - 1);
          send_to_char (buf, ch);
          return;
        }
      raceswitch(victim, value);
      return;
    }
  if (!str_cmp (arg2, "hp") && arg3[0] != '\0')
    {
      if (value < 1 || value > 2000000000)
        {
          send_to_char ("Hp range is 1 to 2000000000 hit points.\n\r", ch);
          return;
        }
      victim->max_hit = value;
      return;
    }
  if (!str_cmp (arg2, "move") && arg3[0] != '\0')
    {
      if (value < 1 || value > 30000)
        {
          send_to_char ("Move range is 1 to 30,000 move points.\n\r", ch);
          return;
        }
      victim->max_move = value;
      return;
    }
  if (!str_cmp (arg2, "practice") && arg3[0] != '\0')
    {
      if (value < 0 || value > 100)
        {
          send_to_char ("Practice range is 0 to 100 sessions.\n\r", ch);
          return;
        }
      victim->pcdata->practice = value;
      return;
    }
  if (!str_cmp (arg2, "learn") && arg3[0] != '\0')
    {
      if (value < 0 || value > SKILL_COUNT)
        {
          send_to_char ("Practice range is 0 to 100 sessions.\n\r", ch);
          return;
        }
      victim->pcdata->learn = value;
      return;
    }
  
  if ((!str_cmp (arg2, "align") || !str_cmp (arg2, "alignment")) && arg3[0] != '\0')
    {
      if (value < 0 || value > NUM_ALIGN)
        {
          send_to_char ("Alignment range is 0 to 9. (0 = newbie, 1 = evil, 2 = good atm\n\r", ch);
          return;
        }
      alignswitch(victim, value);
      return;
    }
  if (!str_cmp (arg2, "thirst") && arg3[0] != '\0')
    {
      if (value < 0 || value > 100)
        {
          send_to_char ("Thirst range is 0 to 100.\n\r", ch);
          return;
        }
      victim->pcdata->condition[COND_THIRST] = value;
      return;
    }
  if (!str_cmp (arg2, "drunk") && arg3[0] != '\0')
    {
      if (value < 0 || value > 100)
        {
          send_to_char ("Drunk range is 0 to 100.\n\r", ch);
          return;
        }
      victim->pcdata->condition[COND_DRUNK] = value;
      return;
    }
  if (!str_cmp (arg2, "full") && arg3[0] != '\0')
    {
      if (value < 0 || value > 100)
        {
          send_to_char ("Full range is 0 to 100.\n\r", ch);
          return;
        }
      victim->pcdata->condition[COND_FULL] = value;
      return;
    }
  if (!str_cmp (arg2, "master") && arg3[0] != '\0')
    {
      check_fgt (victim);
      if (!str_cmp (arg3, "none"))
        {
          victim->fgt->master = NULL;
          return;
        }
      if ((get_char_world (ch, arg3)) == NULL)
        {
          send_to_char ("They aren't here.\n\r", ch);
          return;
        }
      victim->fgt->master = get_char_world (ch, arg3);
      return;
    }
  if (!str_cmp (arg2, "leader") && arg3[0] != '\0')
    {
      check_fgt (victim);
      if (!str_cmp (arg3, "none"))
        {
          victim->fgt->leader = NULL;
          return;
        }
      if ((get_char_world (ch, arg3)) == NULL)
        {
          send_to_char ("They aren't here.\n\r", ch);
          return;
        }
      victim->fgt->leader = get_char_world (ch, arg3);
      return;
    }
  if (!str_cmp (arg2, "short") && arg3[0] != '\0')
    {
      if (victim->pcdata->short_descr && victim->pcdata->short_descr != NULL)
        free_string (victim->pcdata->short_descr);
      victim->pcdata->short_descr = str_dup (arg3);
      return;
    }
  if (!str_cmp (arg2, "long"))
    {
      send_to_char ("Entering line editing mode.\n\r", ch);
      send_to_char ("Terminate with a ~ on a blank line.\n\r", ch);
      ch->desc->pString = &victim->pcdata->long_descr;
      return;
    }
  if (!str_cmp (arg2, "title") && arg3[0] != '\0')
    {
      set_title (victim, arg3);
      return;
    }
  if (!str_cmp (arg2, "augment"))
    {
      char bufff[200];
      bool isoff = FALSE;
      int num = 0;
      bool found = FALSE;
      int augment = victim->pcdata->augments;
      for (num = 0; str_cmp (augment_flagss[num].what_you_type, "end_of_list") && (!found); num++)
	{
	  if (!str_cmp(arg3, augment_flagss[num].what_you_type ))
	    {
	      found = TRUE;
	      isoff =((IS_SET(augment,augment_flagss[num].flagname))? FALSE : TRUE);
	      sprintf(bufff, " %s bit toggled %s\n\r",augment_flagss[num].what_you_type ,(isoff ? "On" : "Off") );
	      send_to_char(bufff,ch);
	      TOGGLE_BIT(victim->pcdata->augments, augment_flagss[num].flagname);
	    }
	}
      if (!found)
	{
	  sprintf(bufff, "Which augment????\n\r.");
	  send_to_char(bufff, ch);
	}
      else
	return;
    }

  if (!str_cmp (arg2, "affect"))
    {
      char bufff[200];
      bool isoff = FALSE;
      int num = 0;
      bool found = FALSE;
      int affect = victim->pcdata->nat_abilities;
      for (num = 0; str_cmp(affect_flagss[num].what_you_type, "end_of_list") && (!found) ; num++)
	{
	  if (!str_cmp(arg3, affect_flagss[num].what_you_type ))
	    {
	      found = TRUE;
	      isoff =((IS_SET(affect,affect_flagss[num].flagname))? FALSE : TRUE);
	      sprintf(bufff, " %s bit toggled %s\n\r",affect_flagss[num].what_you_type ,(isoff ? "On" : "Off") );
	      TOGGLE_BIT(victim->pcdata->nat_abilities, affect_flagss[num].flagname);
	    }
	}
      if (!found)
	{
	  sprintf(bufff, "Which affect????\n\r.");
	  send_to_char(bufff, ch);
	}
      else
	{
	  send_to_char(bufff, ch);
	  return;
	}
    }
  if (!str_cmp (arg2, "affect2"))
    {
      char bufff[200];
      bool isoff = FALSE;
      int num = 0;
      bool found = FALSE;
      int affect = victim->pcdata->nat_abilities2;
      for (num = 0; str_cmp(affect_flags_2[num].what_you_type, "end_of_list") && (!found); num++)
	{
	  if (!str_cmp(arg3, affect_flags_2[num].what_you_type ))
	    {
	      found = TRUE;
	      isoff =((IS_SET(affect,affect_flags_2[num].flagname))? FALSE : TRUE);
	      sprintf(bufff, " %s bit toggled %s\n\r",affect_flags_2[num].what_you_type ,(isoff ? "On" : "Off") );
	      TOGGLE_BIT(victim->pcdata->nat_abilities2, affect_flags_2[num].flagname);
	    }
	}
      if (!found)
	{
	  sprintf(bufff, "Which affect????\n\r.");
	  send_to_char(bufff, ch);
	}
      else
	{
	  send_to_char(bufff, ch);
	  return;
	}
    }
  do_pset (ch, "");
  return;
}

char *
con_state (DESCRIPTOR_DATA * d)
{
  static char st[30];
  st[0] = '\0';
  if (d->connected == CON_PLAYING)
    strcpy (st, "Playing");
  else if (d->connected == CON_AEDITOR && (AREA_DATA *) d->pEdit != NULL)
    {
      sprintf (st, "Aedit #%d", ((AREA_DATA *) d->pEdit)->vnum);
    }
  else if (d->connected == CON_REDITOR)
    {
      sprintf (st, "Redit #%d", d->character->in_room->vnum);
    }
  else if (d->connected == CON_MEDITOR && (MOB_PROTOTYPE *) d->pEdit != NULL)
    {
      sprintf (st, "Medit #%d", ((MOB_PROTOTYPE *) d->pEdit)->vnum);
    }
  else if (d->connected == CON_OEDITOR && (OBJ_PROTOTYPE *) d->pEdit != NULL)
    {
      sprintf (st, "Oedit #%d", ((OBJ_PROTOTYPE *) d->pEdit)->vnum);
    }
  else if (d->connected == CON_CEDITOR && (CLAN_DATA *) d->pEdit != NULL)
    {
      sprintf (st, "Cedit #%d", ((CLAN_DATA *) d->pEdit)->vnum);
    }
  else if (d->connected == CON_FSEDITOR)
    strcpy (st, "FullScreen");
  else if (d->connected == CON_PEDITOR && (CHAR_DATA *) d->pEdit != NULL)
    {
      sprintf (st, "Pedit %s", ((CHAR_DATA *) d->pEdit)->pcdata->name);
    }
  else if (d->connected == CON_SEDITOR && (SPELL_DATA *) d->pEdit != NULL)
    {
      sprintf (st, "Sedit #%d", ((SPELL_DATA *) d->pEdit)->gsn);
    }
  else if (d->connected == CON_SOCEDIT)
    strcpy (st, "SocEdit");
  else if (d->connected == CON_GET_OLD_PASSWORD)
    strcpy (st, "Asking Passwd");
  else if (d->connected == CON_READ_MOTD)
    strcpy (st, "Welcome Messages");
  else if (d->connected == CON_SHELL)
    strcpy (st, "<SHELL>");
  else
    strcpy (st, "Newbie/Login");
  return st;
}

void
do_users (CHAR_DATA * ch, char *argy)
{
  DESCRIPTOR_DATA *d;
  CHAR_DATA *tch;
  int cc;
  int count;
  int tcount;
  DEFINE_COMMAND ("users", do_users, POSITION_DEAD, MAX_LEVEL, LOG_NORMAL, "Shows a list of all connections and sites.")

    if (IS_MOB (ch))
    return;
  if (ch->pcdata->level > MAX_LEVEL)
    ch->pcdata->level = MAX_LEVEL;
  hugebuf_o[0] = '\0';
  tcount = 0;
  count = 0;
  cc = 0;
  for (tch = char_list; tch != NULL; tch = tch->next)
    {
      if (IS_PLAYER (tch) && tch->desc == NULL)
        cc++;
    }
  for (d = descriptor_list; d != NULL; d = d->next)
    {
      tcount++;
      if (d->character != NULL && can_see (ch, d->character))
        {
          if (d->connected <= 0)
            count++;
          sprintf (hugebuf_o + strlen (hugebuf_o),
                   "%s%-3d \x1B[1;37m%-12s\x1B[37;0m %-17s %-18s \x1B[1;37m%s\x1B[37;0m\n\r",
                   (d->host[29] == 1 ? "*" : "#"), d->descriptor, d->character ? NAME (d->character) : "(none)",
		   d->host, con_state (d), d->username);
	}
    }
  sprintf (hugebuf_o + strlen (hugebuf_o), "-----------------------------------------------------------------------\n\r");
  sprintf (hugebuf_o + strlen (hugebuf_o), "%d Active Player%s, %d Connection%s, and %d Linkless Player%s.\n\r",count, count == 1 ? "" : "s", tcount, tcount == 1 ? "" : "s", cc, cc == 1 ? "" : "s");
  page_to_char (hugebuf_o, ch);
  sprintf (hugebuf_o, "Max # of connections since reboot   [\x1B[34;1m%5d\x1B[37;0m]\n\r", max_connected); send_to_char (hugebuf_o, ch);
  return;
}

void
do_force (CHAR_DATA * ch, char *argy)
{
  char arg[SML_LENGTH];
  DEFINE_COMMAND ("force", do_force, POSITION_DEAD, MAX_LEVEL, LOG_ALWAYS, "Forces a player/mob to do something.")

    argy = one_argy (argy, arg);
  if (IS_PLAYER (ch) && LEVEL (ch) > MAX_LEVEL)
    ch->pcdata->level = MAX_LEVEL;
  if (arg[0] == '\0' || argy[0] == '\0')
    {
      send_to_char ("Force whom to do what?\n\r", ch);
      return;
    }
  if (!str_cmp (arg, "all") && IS_PLAYER (ch))
    {
      CHAR_DATA *vch;
      CHAR_DATA *vch_next;
      if (LEVEL (ch) < MAX_LEVEL)
        return;
      for (vch = char_list; vch != NULL; vch = vch_next)
        {
          vch_next = vch->next;
          if (IS_PLAYER (vch) && LEVEL (vch) < LEVEL (ch))
            {
              act ("$n forces everyone to '$t'.", ch, argy, vch, TO_VICT);
              if (IS_PLAYER (ch))
                FORCE_LEVEL = LEVEL (ch);
              else
                FORCE_LEVEL = TOP_LEVEL;
              interpret (vch, argy);
            }
        }
    }
  else
    {
      CHAR_DATA *victim;
      if ((victim = get_char_world_2 (ch, arg)) == NULL)
        {
          send_to_char ("They aren't here.\n\r", ch);
          return;
        }
      if (victim == ch)
        {
          send_to_char ("Aye aye, right away!\n\r", ch);
          return;
        }
      if (IS_PLAYER (ch) && victim->in_room->area->open == 1 && !IS_REAL_GOD (ch))
        return;
      if (IS_PLAYER (ch) && LEVEL (victim) >= LEVEL (ch))
        {
          send_to_char ("Do it yourself!\n\r", ch);
          return;
        }
      if (LEVEL (ch) < MAX_LEVEL && IS_PLAYER (victim))
        {
          send_to_char ("You are not allowed to force Players at all!\n\r", ch);
          return;
        }
      act ("$n forces you to '$t'.", ch, argy, victim, TO_VICT);
      if (IS_PLAYER (ch))
        FORCE_LEVEL = LEVEL (ch);
      else
        FORCE_LEVEL = TOP_LEVEL;
      interpret (victim, argy);
    }
  send_to_char ("Ok.\n\r", ch);
  return;
}

void
do_invis (CHAR_DATA * ch, char *argy)
{
  char arg[SML_LENGTH];
  int oldinvis;
  DEFINE_COMMAND ("invis", do_invis, POSITION_DEAD, IMM_LEVEL, LOG_NORMAL, "This command toggles whether an immort is vis to morts or not.")

    one_argy (argy, arg);
  if (IS_MOB (ch))
    return;
  oldinvis = ch->pcdata->wizinvis;
  if (arg[0] == '\0')
    {
      ch->pcdata->wizinvis = ch->pcdata->wizinvis == 0 ? LEVEL (ch) : 0;
    }
  else
    ch->pcdata->wizinvis = atoi (arg);
  ch->pcdata->wizinvis = ch->pcdata->wizinvis <= LEVEL (ch) ? ch->pcdata->wizinvis : LEVEL (ch);
  if (ch->pcdata->wizinvis <= LEVEL_IMMORTAL && ch->pcdata->wizinvis != 0)
    ch->pcdata->wizinvis = 0;
  if (ch->pcdata->wizinvis == oldinvis ||
      (ch->pcdata->wizinvis >= LEVEL_IMMORTAL && oldinvis >= LEVEL_IMMORTAL))
    return;
  if (ch->pcdata->wizinvis < oldinvis && ch->pcdata->wizinvis == 0)
    {
      act ("$n slowly fades into existence.", ch, NULL, NULL, TO_ROOM);
      send_to_char ("You slowly fade back into existence.\n\r", ch);
    }
  else if (ch->pcdata->wizinvis >= LEVEL_IMMORTAL && ch->pcdata->wizinvis > oldinvis)
    {
      act ("$n slowly fades into normal air.", ch, NULL, NULL, TO_ROOM);
      send_to_char ("You slowly vanish into normal air.\n\r", ch);
    }
  return;
}

void
do_holywalk (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("holywalk", do_holywalk, POSITION_DEAD, IMM_LEVEL, LOG_NORMAL, "Toggles holywalk on/off.")

    if (IS_MOB (ch))
    return;
  if (IS_SET (ch->act, PLR_HOLYWALK))
    {
      REMOVE_BIT (ch->act, PLR_HOLYWALK);
      send_to_char ("Holy walk mode off.\n\r", ch);
    }
  else
    {
      SET_BIT (ch->act, PLR_HOLYWALK);
      send_to_char ("Holy walk mode on.\n\r", ch);
    }
  return;
}

void 
do_mapping (CHAR_DATA *ch, char *argy)
{
  DEFINE_COMMAND ("mapping", do_mapping, POSITION_DEAD, 1, LOG_NORMAL, "Toggles mapping on/off.")
    
    if (IS_MOB (ch))
      return;
 /*  if (!is_member(ch, GUILD_RANGER))
    {
	send_to_char ("Only rangers of unsurpassed ability may use mapping.\n\r", ch);
    	return;
    } */
  if (IS_SET (ch->pcdata->act2, PLR_MAPPING) || IS_AFFECTED(ch,AFF_BLIND))
    {
      REMOVE_BIT (ch->pcdata->act2, PLR_MAPPING);
      do_rawclear(ch);
      send_to_char ("Mapping mode off.\n\r", ch);
      
    }
  else
    {
      SET_BIT (ch->pcdata->act2, PLR_MAPPING);
      do_rawclear(ch);
      small_map(ch);
      send_to_char ("Mapping mode on.\n\r", ch);
    }
  return;
}

void 
do_noibm (CHAR_DATA *ch, char *argy)
{
  DEFINE_COMMAND ("noibm", do_noibm, POSITION_DEAD, 1, LOG_NORMAL, "Toggles noibm on/off.")
    
    if (IS_MOB (ch))
      return;
  if (IS_SET (ch->pcdata->act2, PLR_NOIBM))
    {
      REMOVE_BIT (ch->pcdata->act2, PLR_NOIBM);
      send_to_char ("Noibm mode off.\n\r", ch);
    }
  else
    {
      SET_BIT (ch->pcdata->act2, PLR_NOIBM);
      send_to_char ("Noibm mode on.\n\r", ch);
    }
  return;
}
void 
do_mapspam (CHAR_DATA *ch, char *argy)
{
  DEFINE_COMMAND ("mapspam", do_mapspam, POSITION_DEAD, 1, LOG_NORMAL,
"Toggles mapspam on/off.")
    
    if (IS_MOB (ch))
      return;
  if (IS_SET (ch->pcdata->act2, PLR_MAPSPAM))
    {
      REMOVE_BIT (ch->pcdata->act2, PLR_MAPSPAM);
      send_to_char ("Mapspam mode off.\n\r", ch);
    }
  else
    {
      SET_BIT (ch->pcdata->act2, PLR_MAPSPAM);
      send_to_char ("Mapspam mode on.\n\r", ch);
    }
  return;
}
void 
 do_viewmobs (CHAR_DATA *ch, char *argy)
{
  DEFINE_COMMAND ("viewmobs", do_viewmobs, POSITION_DEAD, 1, LOG_NORMAL,
"Toggles viewmobs on/off.")
    
    if (IS_MOB (ch))
      return;
  if (IS_SET (ch->pcdata->act2, PLR_VIEWMOBS))
    {
      REMOVE_BIT (ch->pcdata->act2, PLR_VIEWMOBS);
      send_to_char ("Viewmobs mode off.\n\r", ch);
    }
  else
    {
      SET_BIT (ch->pcdata->act2, PLR_VIEWMOBS);
      send_to_char ("Viewmobs mode on.\n\r", ch);
    }
  return;
} 
void 
do_viewplayers (CHAR_DATA *ch, char *argy)
{
  DEFINE_COMMAND ("viewplayers", do_viewplayers, POSITION_DEAD, 1, LOG_NORMAL, "Toggles viewplayers on/off.")
    
    if (IS_MOB (ch))
      return;
  if (IS_SET (ch->pcdata->act2, PLR_VIEWPLAYERS))
    {
      REMOVE_BIT (ch->pcdata->act2, PLR_VIEWPLAYERS);
      send_to_char ("Viewplayers mode off.\n\r", ch);
    }
  else
    {
      SET_BIT (ch->pcdata->act2, PLR_VIEWPLAYERS);
      send_to_char ("Viewplayers mode on.\n\r", ch);
    }
  return;
}
void
do_holylag (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("holylag", do_holylag, POSITION_DEAD, IMM_LEVEL, LOG_NORMAL, "Toggles holylag on/off.")

    if (IS_MOB (ch))
    return;
  if (IS_SET (ch->pcdata->act2, PLR_HOLYLAG))
    {
      REMOVE_BIT (ch->pcdata->act2, PLR_HOLYLAG);
      send_to_char ("Holy lag mode off.\n\r", ch);
    }
  else
    {
      SET_BIT (ch->pcdata->act2, PLR_HOLYLAG);
      send_to_char ("Holy lag mode on.\n\r", ch);
    }
  return;
}

void
do_holypeace (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("holypeace", do_holypeace, POSITION_DEAD, IMM_LEVEL, LOG_NORMAL, "Toggles holypeace on/off.")

    if (IS_MOB (ch))
    return;
  if (IS_SET (ch->act, PLR_HOLYPEACE))
    {
      REMOVE_BIT (ch->act, PLR_HOLYPEACE);
      send_to_char ("Holy peace mode off.\n\r", ch);
    }
  else
    {
      SET_BIT (ch->act, PLR_HOLYPEACE);
      send_to_char ("Holy peace mode on.\n\r", ch);
    }
  return;
}

void
do_holylight (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("holylight", do_holylight, POSITION_DEAD, IMM_LEVEL, LOG_NORMAL, "Toggles holylight on/off.")

    if (IS_MOB (ch))
    return;
  if (IS_SET (ch->act, PLR_HOLYLIGHT))
    {
      REMOVE_BIT (ch->act, PLR_HOLYLIGHT);
      send_to_char ("Holy light mode off.\n\r", ch);
    }
  else
    {
      SET_BIT (ch->act, PLR_HOLYLIGHT);
      send_to_char ("Holy light mode on.\n\r", ch);
    }
  return;
}


void
do_owhere (CHAR_DATA * ch, char *argy)
{
  char arg[SML_LENGTH];
  char oneline[500];
  int slen = 0;
  bool found = FALSE;
  SINGLE_OBJECT *obj;
  SINGLE_OBJECT *in_obj;
  int obj_counter = 1;
  DEFINE_COMMAND ("owhere", do_owhere, POSITION_DEAD, MAX_LEVEL, LOG_NORMAL, "Allows you to find actual objects in the world.")

    if (IS_MOB (ch))
    return;
  hugebuf_o[0] = '\0';
  one_argy (argy, arg);
  if (!IS_REAL_GOD (ch))
    {
      send_to_char ("Huh?\n\r", ch);
      return;
    }
  if (arg[0] == '\0')
    {
      send_to_char ("Syntax: owhere <object>.\n\r", ch);
      return;
    }
  else
    {
      for (obj = object_list; obj != NULL; obj = obj->next)
        {
          if (!can_see_obj (ch, obj) || !is_name (arg, obj->pIndexData->name))
            continue;
          found = TRUE;
          for (in_obj = obj; in_obj->in_obj != NULL;
               in_obj = in_obj->in_obj)
            ;
          if (in_obj->carried_by != NULL)
            {
              sprintf (oneline, "[%2d] %s carried by %s.\n\r", obj_counter,
                   OOSTR (obj, short_descr), PERS (in_obj->carried_by, ch));
            }
          else
            {
              sprintf (oneline, "[%2d] %s in %s [%d].\n\r", obj_counter,
                       OOSTR (obj, short_descr), (in_obj->in_room == NULL) ?
                       "somewhere" : in_obj->in_room->name, (in_obj->in_room == NULL) ?
                       0 : in_obj->in_room->vnum);
            }
          obj_counter++;
          oneline[0] = UPPER (oneline[0]);
          sprintf (hugebuf_o + slen, "%s", oneline);
          slen += strlen (oneline);
          if (slen > 69500)
            {
              hugebuf_o[69500] = '\0';
              break;
            }
        }
    }
  if (!found)
    send_to_char (
                   "Nothing like that in hell, earth, or heaven.\n\r", ch);
  if (hugebuf_o[0] != '\0')
    page_to_char (hugebuf_o, ch);
  return;
}

void
do_areas (CHAR_DATA * ch, char *argy)
{
  char buf[2048];
  int slen = 0;
  AREA_DATA *pAreat;
  DEFINE_COMMAND ("areas", do_areas, POSITION_DEAD, 0, LOG_NORMAL, "This command shows the current areas that make up the world.")

    if (IS_MOB (ch))
    return;
  hugebuf_o[0] = '\0';
  buf[0] = '\0';
  if (LEVEL (ch) >= IMM_LEVEL)
    {
      sprintf (hugebuf_o, "[%3s] %1s %-24s (%-5s-%5s) %10s %3s [%s]\n\r",
               "\x1b[1;37m-\x1b[1;30m= \x1b[1;37mThe Lands of Loreia \x1b[1;30m=\x1b[1;37m- \x1b[1;32mNum\x1b[0;37m", "W", "\x1b[1;36mArea Name\x1b[0;37m", "\x1b[1;34mlvnum\x1b[0;37m", "\x1b[1;34muvnum\x1b[0;37m", "\x1b[1;31mFilename\x1b[0;37m", "\x1b[0;30mSec", "\x1b[1;34mBuilders\x1b[0;37m");
      buf[0] = '\0';
      slen = strlen (hugebuf_o);
      for (pAreat = area_first; pAreat != NULL; pAreat = pAreat->next)
        {
          sprintf (buf, "[\x1b[1;32m%3d\x1b[0;37m ] %1s \x1b[1;36m%-24s\x1b[0;37m (\x1b[1;34m%-5d-%5d\x1b[0;37m) \x1b[1;33m%13s %2d \x1b[1;34m[%s]\x1b[0;37m\n\r",
                   pAreat->vnum,
                   (ch->in_room->area == pAreat ? "*" : " "),
                   pAreat->name,
                   pAreat->lvnum,
                   pAreat->uvnum,
                   pAreat->filename,
                   pAreat->security,
                   pAreat->builders);
          sprintf (hugebuf_o + slen, "%s", buf);
          slen += strlen (buf);
          if (slen > 69500)
            {
              hugebuf_o[69500] = '\0';
              break;
            }
        }
    }
  else
    {
      sprintf (hugebuf_o, "[%3s] %1s %-30s [%s]\n\r",
               "\x1b[1;32mNum", "\x1b[1;37mWhere", "\x1b[1;36mArea Name", "\x1b[1;34mBuilders");
      slen = strlen (hugebuf_o);
      buf[0] = '\0';
      for (pAreat = area_first; pAreat != NULL; pAreat = pAreat->next)
        {
          if (!pAreat->open)
            continue;
          sprintf (buf, "\x1b[0;37m[\x1b[1;32m%3d\x1b[0;37m] \x1b[1;37m%1s \x1b[1;36m%-30s \x1b[0;37m[\x1b[1;34m%s\x1b[0;37m]\n\r",
                   pAreat->vnum,
                   (ch->in_room->area == pAreat ? "*" : " "),
                   pAreat->name, pAreat->builders);
          sprintf (hugebuf_o + slen, "%s", buf);
          slen += strlen (buf);
          if (slen > 69500)
            {
              hugebuf_o[69500] = '\0';
              break;
            }
        }
    }
  page_to_char (hugebuf_o, ch);
  return;
}

void
do_count (CHAR_DATA * ch, char *argy)
{
  char arg[STD_LENGTH];
  char arg2[STD_LENGTH];
  char buf[2048];
  AREA_DATA *pArea;
  int tot_room = 0, tot_mob = 0, tot_obj = 0;
  int open_area = 0, open_room = 0, open_mob = 0, open_obj = 0;
  DEFINE_COMMAND ("count", do_count, POSITION_DEAD, IMM_LEVEL, LOG_NORMAL, "Shows area statistics.")

    if (IS_MOB (ch))
    return;
  hugebuf_o[0] = '\0';
  argy = one_argy (argy, arg);
  if (argy[0] != '\0')
    argy = one_argy (argy, arg2);
  if (!str_cmp (arg, "open"))
    {
      sprintf (buf, "[%3s] %-10s (%-5s-%5s) %4s %4s %4s [%12s] %-3s %-4s\n\r",
               "Num", "FileN", "lvnum", "uvnum", "Room", "Mob", "Obj",
               "Builders", "Opn", "Plrs");
      strcpy (hugebuf_o, buf);
      for (pArea = area_first; pArea != NULL; pArea = pArea->next)
        {
          if (pArea->open)
            {
              sprintf (buf, "[%3d] %-10s (%-5d-%5d) %3d/%3d %4d %4d [%12s] %-3s %-4d\n\r",
                       pArea->vnum, pArea->filename,
                       pArea->lvnum, pArea->uvnum, pArea->desc_rooms,
                       pArea->room_count, pArea->mob_count,
                       pArea->obj_count, pArea->builders, pArea->open ? "Yes" : "No", pArea->nplayer);
              strcat (hugebuf_o, buf);
              open_area++;
              open_room += pArea->room_count;
              open_mob += pArea->mob_count;
              open_obj += pArea->obj_count;
            }
          if (strlen (hugebuf_o) > 69500)
            {
              hugebuf_o[69500] = '\0';
              break;
            }
        }
      sprintf (buf, "Open areas: %4d Rooms: %4d Mobs: %4d Objs: %4d\n\r",
               open_area, open_room, open_mob, open_obj);
      strcat (hugebuf_o, buf);
      page_to_char (hugebuf_o, ch);
      return;
    }
  if (arg[0] != '\0')
    {
      sprintf (buf, "[%3s] %-10s (%-5s-%5s) %4s %4s %4s [%12s] %-3s %-4s\n\r",
               "Num", "FileN", "lvnum", "uvnum", "Room", "Mob", "Obj",
               "Builders", "Opn", "Plrs");
      strcpy (hugebuf_o, buf);
      if (is_number (arg))
        {
          int vnum, vnum2 = 0;
          pArea = area_first;
          vnum = atoi (arg);
          if (arg2[0] != '\0' && is_number (arg2))
            vnum2 = atoi (arg2);
          if (vnum2 != 0 && vnum2 < vnum)
            {
              int tmp;
              tmp = vnum;
              vnum = vnum2;
              vnum2 = tmp;
            }
          while (pArea && pArea->vnum != vnum)
            pArea = pArea->next;
          if (!pArea)
            {
              send_to_char ("That area doesn't exist.\n\r", ch);
              return;
            }
          if (vnum2)
            {
              while (pArea && pArea->vnum <= vnum2)
                {
                  if (strlen (hugebuf_o) > 69500)
                    {
                      hugebuf_o[69500] = '\0';
                      break;
                    }
                  sprintf (buf, "[%3d] %-10s (%-5d-%5d) %3d/%3d %4d %4d [%12s] %-3s %-4d\n\r",
                           pArea->vnum, pArea->filename,
                           pArea->lvnum, pArea->uvnum, pArea->desc_rooms,
                           pArea->room_count, pArea->mob_count,
                           pArea->obj_count, pArea->builders, pArea->open ? "Yes" : "No",
                           pArea->nplayer);
                  strcat (hugebuf_o, buf);
                  tot_room += pArea->room_count;
                  tot_mob += pArea->mob_count;
                  tot_obj += pArea->obj_count;
                  if (pArea->open)
                    {
                      open_area++;
                      open_room += pArea->room_count;
                      open_mob += pArea->mob_count;
                      open_obj += pArea->obj_count;
                    }
                  pArea = pArea->next;
                }
            }
          else
            {
              sprintf (buf, "[%3d] %-10s (%-5d-%5d) %3d/%3d %4d %4d [%12s] %-3s %-4d\n\r",
                       pArea->vnum, pArea->filename,
                       pArea->lvnum, pArea->uvnum, pArea->desc_rooms,
                       pArea->room_count, pArea->mob_count,
              pArea->obj_count, pArea->builders, pArea->open ? "Yes" : "No",
                       pArea->nplayer);
              strcat (hugebuf_o, buf);
              tot_room += pArea->room_count;
              tot_mob += pArea->mob_count;
              tot_obj += pArea->obj_count;
              if (pArea->open)
                {
                  open_area++;
                  open_room += pArea->room_count;
                  open_mob += pArea->mob_count;
                  open_obj += pArea->obj_count;
                }
            }
        }
      else
        {
          pArea = area_first;
          while (pArea)
            {
              if (strlen (hugebuf_o) > 69500)
                {
                  hugebuf_o[69500] = '\0';
                  break;
                }
              if (is_name (arg, pArea->builders))
                {
                  sprintf (buf, "[%3d] %-10s (%-5d-%5d) %3d/%3d %4d %4d [%12s] %-3s %-4d\n\r",
                           pArea->vnum, pArea->filename,
                           pArea->lvnum, pArea->uvnum, pArea->desc_rooms,
                           pArea->room_count, pArea->mob_count,
                           pArea->obj_count, pArea->builders, pArea->open ? "Yes" : "No",
                           pArea->nplayer);
                  strcat (hugebuf_o, buf);
                  tot_room += pArea->room_count;
                  tot_mob += pArea->mob_count;
                  tot_obj += pArea->obj_count;
                  if (pArea->open)
                    {
                      open_area++;
                      open_room += pArea->room_count;
                      open_mob += pArea->mob_count;
                      open_obj += pArea->obj_count;
                    }
                }
              pArea = pArea->next;
            }
        }
      sprintf (buf, "Total rooms: %4d Total mobs: %4d Total objs: %4d\n\r",
               tot_room, tot_mob, tot_obj);
      strcat (hugebuf_o, buf);
      sprintf (buf, "Open areas: %4d Rooms: %4d Mobs: %4d Objs: %4d\n\r",
               open_area, open_room, open_mob, open_obj);
      strcat (hugebuf_o, buf);
      page_to_char (hugebuf_o, ch);
      return;
    }
  sprintf (buf, "[%3s] %10s (%-5s-%5s) %4s %4s %4s [%12s] %-3s %-4s\n\r",
           "Num", "Filename", "lvnum", "uvnum", "Room", "Mob", "Obj",
           "Builders", "Opn", "Plrs");
  strcat (hugebuf_o, buf);
  for (pArea = area_first; pArea != NULL; pArea = pArea->next)
    {
      sprintf (buf, "[%3d] %10s (%-5d-%5d) %3d/%3d %4d %4d [%12s] %-3s %-4d\n\r",
               pArea->vnum,
               pArea->filename,
               pArea->lvnum,
               pArea->uvnum,
               pArea->desc_rooms, pArea->room_count,
               pArea->mob_count,
               pArea->obj_count,
               pArea->builders,
               pArea->open ? "Yes" : "No",
               pArea->nplayer);
      strcat (hugebuf_o, buf);
      tot_room += pArea->room_count;
      tot_mob += pArea->mob_count;
      tot_obj += pArea->obj_count;
      if (pArea->open)
        {
          open_area++;
          open_room += pArea->room_count;
          open_mob += pArea->mob_count;
          open_obj += pArea->obj_count;
        }
      if (strlen (hugebuf_o) > 69500)
        {
          hugebuf_o[69500] = '\0';
          break;
        }
    }
  sprintf (buf, "Total rooms: %4d Total mobs: %4d Total objs: %4d\n\r",
           tot_room, tot_mob, tot_obj);
  strcat (hugebuf_o, buf);
  sprintf (buf, "Open areas: %4d Rooms: %4d Mobs: %4d Objs: %4d\n\r",
           open_area, open_room, open_mob, open_obj);
  strcat (hugebuf_o, buf);
  page_to_char (hugebuf_o, ch);
  return;
}

char *
get_ansi_spaces (char *txt)
{
  static char mem[40000];
  char *spaces = mem;
  int ansi = 0, i;
  while (*txt != '\0')
    {
      if (*txt == '\e')
        {
          do
            {
              ansi++;
            }
          while (*txt++ != 'm' && *txt!='\0');
        }
      else
        txt++;
    }
  for (i = 0; i < ansi; i++)
    {
      *spaces = ' ';
      spaces++;
    }
  *spaces = '\0';

  return mem;
}

void
do_noaffect (CHAR_DATA * ch, char *argy)
{
  CHAR_DATA *victim;
  DEFINE_COMMAND ("noaffect", do_noaffect, POSITION_DEAD, MAX_LEVEL, LOG_NORMAL, "This command will remove all affects of the character specified.")

    if (IS_MOB (ch))
    return;
  if (argy == "")
    {
      send_to_char ("Syntax is: noaffect <target_char>.\n\r", ch);
      return;
    }
  if ((victim = get_char_world (ch, argy)) == NULL)
    {
      send_to_char ("Could not find that person in the world...\n\r", ch);
      return;
    }
  strip_all_affects (victim);
  send_to_char ("Affects removed.\n\r", ch);
  return;
}


