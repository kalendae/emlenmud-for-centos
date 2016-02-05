#include <sys/types.h>
// ARC: commenting this out since its not used
//#include <crypt.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emlen.h"

bool inventory=FALSE;

void 
do_showloot (CHAR_DATA * ch, char *argy)
{
  CHAR_DATA *fch;
  DEFINE_COMMAND ("showloot", do_showloot, POSITION_DEAD, 0, LOG_NORMAL, "This command shows what is currently in your inventory.")
  send_to_char("You show off your inventory to the room.\n\r", ch);
  WAIT_STATE(ch, 60);
  for (fch = ch->in_room->more->people; fch != NULL; fch = fch->next_in_room)
  {
    if (ch->pcdata->alignment == fch->pcdata->alignment)
      show_list_to_char(ch->carrying, fch, TRUE, TRUE);
  }
  return;
} // END SHOWLOOT

void
do_where (CHAR_DATA * ch, char *argy)
{
  char buf[STD_LENGTH];
  char arg[SML_LENGTH];
  CHAR_DATA *victim;
  DESCRIPTOR_DATA *d;
  bool found;
  DEFINE_COMMAND ("where", do_where, POSITION_RESTING, 0, LOG_NORMAL, "This command shows you characters nearby, and what area you are in.")


  one_argy (argy, arg);
    if (!pow.allow_where)
    {
      send_to_char ("Huh?\n\r", ch);
      return;
    }  if (IS_MOB (ch))
    return;
  sprintf (buf, "You are in \x1B[1m%s\x1B[0m, built by \x1B[1;31m",
           ch->in_room->area->name);
  send_to_char (buf, ch);
  if (LEVEL (ch) > 100 && LEVEL (ch) < 110)
    return;
  if (words (ch->in_room->area->builders) == 0)
    {
      send_to_char ("\x1B[0man anonymous builder.\n\r", ch);
    }
  else if (words (ch->in_room->area->builders) == 1)
    {
      sprintf (buf, "%s\x1B[0m.\n\r", ch->in_room->area->builders);
      send_to_char (buf, ch);
    }
  else if (words (ch->in_room->area->builders) == 2)
    {
      sprintf (buf, "%s\x1B[0m and \x1B[1;31m%s\x1B[0m.\n\r",
               first (ch->in_room->area->builders),
               ch->in_room->area->builders +
               strlen (first (ch->in_room->area->builders)) + 1);
      send_to_char (buf, ch);
    }
  else if (words (ch->in_room->area->builders) == 3)
    {
      sprintf (buf, "%s\x1B[0m, \x1B[1;31m%s\x1B[0m, and \x1B[1;31m%s\x1B[0m.\n\r",
               first (ch->in_room->area->builders),
               first (ch->in_room->area->builders + strlen (first (ch->in_room->area->builders)) + 1),
	       first (ch->in_room->area->builders +
		      strlen (first (ch->in_room->area->builders)) +
		      1 +
		      strlen (ch->in_room->area->builders + strlen (first (ch->in_room->area->builders)) + 1) + 1));
      send_to_char (buf, ch);
    }

  send_to_char ("Players near you:\n\r", ch);
  found = FALSE;
  for (d = descriptor_list; d; d = d->next)
    {
      if ((victim = d->character) != NULL
          && IS_PLAYER (victim)
          && victim->in_room != NULL
          && can_see (ch, victim) &&
          (!DIFF_ALIGN(ch, victim)|| LEVEL(ch) > 100))
        {
          if (LEVEL (ch) >= 100)
            {
              sprintf (buf, "%-28s %s [%d]\n\r", NAME (victim), victim->in_room->name, victim->in_room->vnum);
              send_to_char (buf, ch);
            }
          else if (d->connected == CON_PLAYING)
            {
              if ((victim->in_room->area == ch->in_room->area) &&
                  victim != ch && !DIFF_ALIGN(ch, victim))
                {
                  found = TRUE;
                  sprintf (buf, "%-28s %s\n\r", NAME (victim), victim->in_room->name);
                }
              else
                buf[0] = '\0';
              send_to_char (buf, ch);
            }
        }
    }
  if (!found)
    send_to_char ("No one else can be found.\n\r", ch);

  return;
}

/*
   Scan_direction scans up to three rooms away, or until a mob is found
   in the direction passed.
 */
void
scan_direction (CHAR_DATA * ch, int dir)
{
  EXIT_DATA *pexit;
  ROOM_DATA *troom;
  CHAR_DATA *mob;
  char final[STD_LENGTH];
  char buf[STD_LENGTH];
  bool fMultiple;
  int n = 0, max_iteration;
  hugebuf_o[0] = '\0';
  final[0] = '\0';
  max_iteration = 3;
  pexit = ch->in_room->exit[dir];
  if (pexit == NULL || (ROOM_DATA *) pexit->to_room == NULL)
    return;
  troom = (ROOM_DATA *) pexit->to_room;

  if (troom != NULL && (!pexit->d_info || (!IS_SET (pexit->d_info->exit_info, EX_CLOSED))) && room_is_dark (troom) && !IS_AFFECTED (ch, AFF_INFRARED))
    {
      if (troom->more && troom->more->people != NULL)
        {
          sprintf (buf, "You can make out some motion to the %s.\n\r", dir_name[URANGE (0, dir, MAX_DIR - 1)]);
          send_to_char (buf, ch);
        }
      return;
    }
#ifndef NEW_WORLD
  if (troom != NULL && !(room_is_dark (troom) && !IS_AFFECTED (ch, AFF_INFRARED)))
    {
      if (troom->more && troom->more->people != NULL && troom->more->people->ced && troom->more->people->ced->in_boat != NULL)
        {
          sprintf (buf, "You can make out some sort of boat to the %s.\n\r", dir_name[URANGE (0, dir, MAX_DIR - 1)]);
          send_to_char (buf, ch);
          return;
        }
    }
#endif
  n = 0;
  while ((pexit != NULL) && (n < max_iteration) &&
         ((ROOM_DATA *) pexit->to_room != NULL) && (!pexit->d_info || (!IS_SET (pexit->d_info->exit_info, EX_CLOSED)
                                         && pexit->d_info->exit_info != 1)))
    {
      fMultiple = FALSE;
      troom = (ROOM_DATA *) pexit->to_room;
      if (troom != NULL && troom->more && troom->more->people != NULL)
        {
          int cnnt;
          cnnt = 0;
          for (mob = troom->more->people; mob != NULL; mob = mob->next_in_room)
            {
              if (!can_see_nextdoor (ch, mob) || !can_see (ch, mob))
                continue;
              cnnt++;
              if (cnnt > 1 && mob->next_in_room == NULL && ((
                                            mob != troom->more->people) || (
                                                                             troom->more->people->next_in_room != NULL && mob != troom->more->people->next_in_room)))
                {
                  fMultiple = TRUE;
                  sprintf (buf, " and %s", rNAME (mob, ch));
                }
              else if (mob->next_in_room != NULL && can_see_nextdoor (ch, mob)
                       && mob->next_in_room->next_in_room != NULL)
                sprintf (buf, "%s, ", rNAME (mob, ch));
              else
                sprintf (buf, "%s", rNAME (mob, ch));
              strcat (final, buf);
            }
          if (cnnt > 0)
            {
              sprintf (buf, " %s %s \x1B[1;37m%s\x1B[0m from here.\n\r", fMultiple ? "are" : "is",
                       dist_name[URANGE (0, n, 7)],
                       dir_name[URANGE (0, dir, MAX_DIR - 1)]);
              strcat (final, buf);
            }
          if (final[0] != '\0')
            {
              final[0] = UPPER (final[0]);
              strcpy (hugebuf_o, final);
              send_to_char (hugebuf_o, ch);
            }
          final[0] = '\0';
          return;
        }
      pexit = troom->exit[dir];
      n++;
    }
  return;
}
void
scan_direction2 (CHAR_DATA * ch, int dir, int followers)
{
  EXIT_DATA *pexit;
  ROOM_DATA *troom;
  ROOM_DATA *curr_room;
  CHAR_DATA *viewer;
  char buff[100];
  int max_iteration = 2;
  int n = 0;
  bool seen = FALSE;
  if (ch->in_room)
    curr_room = ch->in_room;
  else
    return;
  for (n = 0; n < max_iteration; n++)
    {
      pexit = curr_room->exit[dir];
      if (pexit == NULL || (troom =(ROOM_DATA *) pexit->to_room) == NULL)
        return;
      if (!pexit->d_info || (!IS_SET (pexit->d_info->exit_info, EX_CLOSED)))
        {
          if (troom->more && troom->more->people != NULL)
            {
              for (viewer = troom->more->people; viewer != NULL; viewer =viewer->next_in_room)
                {
                  buff[0] = '\0';
		  if (IS_MOB(viewer))  continue;
		  if (!DIFF_ALIGN(ch, viewer)) continue;
		  // ARC: no timers associated with neuts
		  if (ALIGN(ch) == 0 || ALIGN(viewer) == 0 && !IN_BATTLE(ch)) continue;
		  if (viewer->pcdata->no_quit_pk < 2)
		    viewer->pcdata->no_quit_pk = 2;
		  if (ch->pcdata->no_quit_pk < 2)
		    ch->pcdata->no_quit_pk = 2;
		  if (!IS_SET(viewer->pcdata->act2, PLR_AUTOSCAN))
		    continue;
		  if(viewer->pcdata->learned[gsn_sixth_sense] < 50)
		    continue;
		  if ((ch->pcdata->wizinvis > LEVEL(viewer))
		      || IS_AFFECTED(viewer, AFF_BLIND))
		    continue;
		  /* Similar to can_see */
		  if ((room_is_dark(ch->in_room) && !IS_AFFECTED(viewer, AFF_INFRARED))
		      || (IS_AFFECTED(ch, AFF_INVISIBLE) && !IS_AFFECTED(viewer, AFF_DETECT_INVIS))
		      || (IS_AFFECTED(ch, AFF_HIDE) && !IS_AFFECTED(viewer, AFF_DETECT_HIDDEN))
		      || (weather_info.sky == SKY_FOGGY)
		      || (IS_AFFECTED_EXTRA(ch, AFF_CHAMELEON) && !IS_AFFECTED_EXTRA(ch, AFF_DET_CHAMELEON)))
		    sprintf(buff, "Something arrives ");
		  else
		    sprintf(buff,"%s arrives ", rNAME(ch, viewer));
		  switch (n)
		    {
		    default:
		      strcat(buff, "very far ");
		      break;
		    case 0:
		      strcat(buff, "close by ");
		      break;
		    case 1:
		      strcat(buff, "off ");
		      break;
		    case 2:
		      strcat(buff, "far ");
		      break;
		    }
		  switch (dir)
		    {
		    case 0:
		      strcat(buff, "to the south");
		      break;
		    case 1:
		      strcat(buff, "to the west");
		      break;
		    case 2:
		      strcat(buff, "to the north");
		      break;
		    case 3:
		      strcat(buff, "to the east");
		      break;
		    case 4:
		      strcat(buff, "down below");
		      break;
		    case 5:
		      strcat(buff, "up above");
		      break;
		    }
		  if (followers > 0)
		    {
		      char buf[30];
		      sprintf(buf, " with %d followers", followers);
		      strcat(buff, buf);
		    }
		  strcat(buff, "./n/r");
                  seen = TRUE;
		  send_to_char(buff, viewer);
		}
            }
        }
      if (seen) return;
      curr_room = troom;
    }
}

void
show_list_to_char (SINGLE_OBJECT * list, CHAR_DATA * ch, bool fShort, bool fShowNothing)
{
#ifdef NEW_WORLD
  char buf[STD_LENGTH];
  char prgpstrShow[200][150];
  int prgnShow[200];
  char pstrShow[150];
#else
  char buf[STD_LENGTH];
  char prgpstrShow[200][STD_LENGTH]; //ARC: fixed array size
  int prgnShow[200];
  char pstrShow[STD_LENGTH]; //ARC: fixed array size
#endif
  SINGLE_OBJECT *obj;
  int nShow;
  int iShow;
  int count;
  bool fCombine;
  count = 0;
  bzero (&prgpstrShow, sizeof (prgpstrShow));
  bzero (&prgnShow, sizeof (prgnShow));
  bzero (&pstrShow, sizeof (pstrShow));

  if (list == NULL)
    return;
  nShow = 0;
  for (obj = list; obj && obj != NULL; obj = obj->next_content)
    {
#ifndef NEW_WORLD
      if (ch->ced && ch->ced->in_boat == obj)
        continue;
#endif
      if (obj->wear_loc == WEAR_NONE && can_see_obj (ch, obj)
          && ((!IS_SET (obj->extra_flags, ITEM_UNSEEN) &&
               !IS_SET (obj->pIndexData->extra_flags, ITEM_UNSEEN))
              || IS_SET (ch->act, PLR_HOLYLIGHT)))
        {

          strcpy (pstrShow, format_obj_to (obj, ch, fShort));
          fCombine = FALSE;
          if (IS_MOB (ch) || IS_SET (ch->pcdata->act2, PLR_COMBINE))
            {
              for (iShow = nShow - 1; iShow >= 0; iShow--)
                {
                  if (!strcmp (prgpstrShow[iShow], pstrShow))
                    {
                      prgnShow[iShow]++;
                      fCombine = TRUE;
                      break;
                    }
                }
            }
          if (!fCombine)
            {
              strcpy (prgpstrShow[nShow], pstrShow);
              prgnShow[nShow] = 1;
              nShow++;
              if (nShow >= 200)
                {
                  fprintf (stderr, "Error.  nShow>=200.  Ch: %s.\n", NAME (ch));
                  return;
                }
            }
        }
    }
  for (iShow = 0; iShow < nShow; iShow++)
    {
      if (IS_MOB (ch) || IS_SET (ch->pcdata->act2, PLR_COMBINE))
        {
          if (prgnShow[iShow] != 1)
            {
              sprintf (buf, "[x%d] ", prgnShow[iShow]);
              send_to_char (buf, ch);
            }
        }
      send_to_char (prgpstrShow[iShow], ch);
      if (fShort)
        send_to_char ("\x1B[0m", ch);
    }
  if (fShowNothing && nShow == 0)
    {
      if (IS_MOB (ch) || IS_SET (ch->pcdata->act2, PLR_COMBINE))
        send_to_char ("Nothing.\n\r", ch);
    }
  return;
}


void show_char_to_char (CHAR_DATA * list, CHAR_DATA * ch)
{

  CHAR_DATA *rch;
  for (rch = list; rch != NULL; rch = rch->next_in_room)
    {
      if (rch == ch)
        continue;
      show_char_to_char_0 (rch, ch);
    }
  return;
}



void
show_char_to_char_0 (CHAR_DATA * victim, CHAR_DATA * ch)
{
  char buf[STD_LENGTH * 5];
  char to_show[STD_LENGTH * 5];
  bool tflag;
  bool fll = FALSE;
  bool rcselect;
  buf[0] = '\0';
  rcselect = FALSE;
  tflag = FALSE;
  to_show[0]='\0';
  buf[0]='\0';
  if (!can_see (ch, victim))
    return;
  if (ch == victim) return;
  if (MOUNTED_BY (victim) != NULL
      && victim->fgt->mounted_by->in_room == ch->in_room
      && MOUNTED_BY (victim) == ch)
    {
      act ("\x1B[1;37mYou are riding $N.\x1B[0m", ch, NULL, victim, TO_CHAR_SPAM);
      return;
    }
  if (IS_PLAYER (victim) && victim->desc == NULL)
    strcat (buf, "\x1B[1;34m>LINKLESS< \x1B[0m");
  if (IS_PLAYER (victim))
    send_to_char ("\x1B[1;35m", ch);
  else
    send_to_char ("\x1B[1;36m", ch);
  to_show[0] = '\0';
  if (IS_PLAYER (ch) && IS_PLAYER (victim) && DIFF_ALIGN(ch, victim))
    {
      strcpy (to_show, rNAME (victim, ch));
      rcselect = TRUE;
    }
  if (MOUNTED_BY (victim) != NULL)
    return;
  if (!rcselect)
    {
      if (victim->position == POSITION_STANDING && RIDING (victim) == NULL
          && LD (victim) && LD (victim)[0] != '\0' && IS_MOB(victim))
        {
          strcpy (to_show, LD (victim));
          tflag = FALSE;
        }
      else
        {
          strcpy (to_show, PERS (victim, ch));
          tflag = TRUE;
        }
    }
  if (IS_PLAYER (ch) && (ch->pcdata->no_spam == 2 || ch->pcdata->no_spam == 1) && !tflag)
    {
      if (to_show[strlen (to_show) - 1] == '\n')
        to_show[strlen (to_show) - 1] = '\0';
      if (to_show[strlen (to_show) - 2] == '\r')
        to_show[strlen (to_show) - 3] = '\0';
      if (to_show[strlen (to_show) - 2] == '\n')
        to_show[strlen (to_show) - 2] = '\0';
      if (to_show[strlen (to_show) - 3] == '\r')
        to_show[strlen (to_show) - 4] = '\0';
      if (to_show[strlen (to_show) - 3] == '\n')
        to_show[strlen (to_show) - 3] = '\0';
      if (to_show[strlen (to_show) - 1] == '\r')
        to_show[strlen (to_show) - 2] = '\0';
//      if (IS_AFFECTED (victim, AFF_CHARM))
//        strcat (to_show, " \x1B[1;32m(>> Undead <<)\x1B[0m");
      if (victim->is_undead == TRUE)
        strcat (to_show, " \x1B[1;32m(>> Undead <<)\x1B[0m");
      if (IS_AFFECTED (victim, AFF_INVISIBLE))
        strcat (to_show, "\x1B[1;36m (\x1B[0;36mInvis\x1B[1;36m)\x1B[0m");
      if (IS_AFFECTED (victim, AFF_HIDE))
        strcat (to_show, "\x1B[1;30m (Hidden)\x1B[0m");
      if (IS_AFFECTED (victim, AFF_SANCTUARY))
          strcat (to_show, "\x1B[0;37m (\x1B[1;37mSanctuary\x1B[0;37m)");
      if (IS_AFFECTED_EXTRA (victim, AFF_FIRESHIELD))
	strcat (to_show, "\x1B[0;31m (\x1b[1;31mFireshield\x1b[0;31m)\x1b[0;37m");
      if (IS_AFFECTED (victim, AFF_FAERIE_FIRE))
        strcat (to_show, "\x1B[1;35m (Outlined)\x1B[0m");
      if (IS_AFFECTED_EXTRA (victim, AFF_MANASHIELD))
	strcat (to_show, " \x1b[0;36m(\x1b[1;36mMana Shield\x1b[0;36m)\x1B[0m");
      if (IS_AFFECTED (victim, AFF_FLYING))
	{
	  if ((IS_MOB(victim) && (victim->pIndexData->mobtype == MOB_BIRD || victim->pIndexData->mobtype == MOB_DRAGON)) || (IS_PLAYER(victim) && IS_SET(race_info[victim->pcdata->race].affect_bits, AFF_FLYING)))
	    strcat (to_show, "\x1B[0;1;35m (Flying)\x1B[0m");
	  else
	    strcat (to_show, "\x1B[0;1;35m (Flying)\x1B[0m");
	}
      strcat (buf, to_show);
      strcat (buf, "\n\r");
      send_to_char (buf, ch);
      return;
    }
  strcat (buf, to_show);
  if (tflag || rcselect)
    {
      buf[0] = UPPER (buf[0]);

      /* ARC: removing this showing of titles in room...
        looks ugly, useless and gives room for visual abuse
      if (IS_PLAYER (victim) && !IS_SET (ch->pcdata->act2, PLR_BRIEF)
&& !rcselect &&
          strlen (victim->pcdata->title) < 39)
        {
          strcat (buf, " ");
          strcat (buf, victim->pcdata->title);
        }

       */
      if (RIDING (victim) != NULL && victim->fgt->riding->in_room == victim->in_room)
        {
          char buf2[STD_LENGTH];
          sprintf (buf2, " is here, riding %s.", PERS (RIDING (victim), ch));
          strcat (buf, buf2);
        }
      else
        {
          strcat (buf, " is");
          switch (victim->position)
            {
            case POSITION_DEAD:
              strcat (buf, " DEAD!!");
              break;
            case POSITION_MORTAL:
              strcat (buf, " mortally wounded.");
              break;
            case POSITION_INCAP:
              strcat (buf, " incapacitated.");
              break;
            case POSITION_STUNNED:
              strcat (buf, " lying here, stunned.");
              break;
            case POSITION_SLEEPING:
              strcat (buf, " asleep here.");
              break;
            case POSITION_BASHED:
               strcat (buf, " on the ground, bashed.\n\r");
	       break;
            case POSITION_MEDITATING:
              strcat (buf, " meditating here.");
              break;
            case POSITION_SEARCHING:
              strcat (buf, " searching for something here.");
              break;
            case POSITION_CASTING:
              strcat(buf, " casting a spell.");
              break;
            case POSITION_FALLING:
              strcat (buf, " falling to the ground!");
              break;
            case POSITION_RESTING:
              strcat (buf, " resting here.");
              break;
            case POSITION_STANDING:
              strcat (buf, " here.");
              break;
            case POSITION_FIGHTING:
              strcat (buf, " here, fighting ");
              if (FIGHTING (victim) == NULL)
                strcat (buf, "thin air??\n\r");
              else if (FIGHTING (victim) == ch)
                strcat (buf, "you!\n\r");
              else if (FIGHTING (victim) && victim->in_room == victim->fgt->fighting->in_room)
                {
                  strcat (buf, rNAME (FIGHTING (victim), ch));
                  strcat (buf, ".\n\r");
                }
              else
                strcat (buf, "someone who left??\n\r");
              break;
            case POSITION_GROUNDFIGHTING:
	      strcat (buf, " fighting on the ground with ");
              if (FIGHTING (victim) == NULL)
                strcat (buf, "thin air??\n\r");
              else if (FIGHTING (victim) == ch)
                strcat (buf, "YOU!\n\r");
              else if (FIGHTING (victim) && victim->in_room == victim->fgt->fighting->in_room)
                {
                  strcat (buf, rNAME (FIGHTING (victim), ch));
                  strcat (buf, ".\n\r");
                }
              else
                strcat (buf, "someone who left??\n\r");

              break;
            }
        }
    }
  if (victim->position != POSITION_FIGHTING &&
      victim->position != POSITION_GROUNDFIGHTING && victim->position != POSITION_BASHED)
    {
      if (IS_PLAYER (ch) && (ch->pcdata->no_spam == 2 || ch->pcdata->no_spam == 1))
        {
          strcpy (to_show, buf);
          if (IS_AFFECTED (victim, AFF_CHARM))
            strcat (to_show, " \x1B[1;32m(>> Undead <<)\x1B[0m");
          if (IS_AFFECTED (victim, AFF_INVISIBLE))
            strcat (to_show, "\x1B[1;36m (\x1B[0;36mInvis\x1B[1;36m)\x1B[0m");
          if (IS_AFFECTED (victim, AFF_HIDE))
            strcat (to_show, "\x1B[1;30m (Hidden)\x1B[0m");
          if (IS_AFFECTED (victim, AFF_SANCTUARY))
	    strcat (to_show, "\x1B[0;37m (\x1B[1;37mSanctuary\x1B[0;37m)");
	  if (IS_AFFECTED_EXTRA (victim, AFF_FIRESHIELD))
	    strcat (to_show, "\x1B[0;31m (\x1b[1;31mFireshield\x1b[0;31m)\x1b[0;37m");
          if (IS_AFFECTED (victim, AFF_FAERIE_FIRE))
            strcat (to_show, "\x1B[1;35m (Outlined)\x1B[0m");
	  if (IS_AFFECTED (victim, AFF_FLYING))
	    {
	      if ((IS_MOB(victim) && (victim->pIndexData->mobtype == MOB_BIRD || victim->pIndexData->mobtype == MOB_DRAGON)) || (IS_PLAYER(victim) && IS_SET(race_info[victim->pcdata->race].affect_bits, AFF_FLYING)))
		strcat (to_show, "\x1B[0;1;35m (Flying)\x1B[0m");
	      else
		strcat (to_show, "\x1B[0;1;35m (Flying)\x1B[0m");
	    }
          strcat (to_show, "\n\r");
          send_to_char (to_show, ch);
          return;
        }
    }
  send_to_char (buf, ch);
  if (IS_PLAYER (ch) && ch->pcdata->no_spam == 0)
    {
      if (victim->position != POSITION_FIGHTING && victim->position != POSITION_BASHED &&
          victim->position != POSITION_GROUNDFIGHTING &&
          (tflag || rcselect) && (!fll))
        send_to_char ("\n\r", ch);
      if (IS_AFFECTED (victim, AFF_CHARM))
        act ("\x1B[1;32m...$N is >> Undead <<.\x1B[0m", ch, NULL, victim, TO_CHAR);
      if (IS_AFFECTED (victim, AFF_INVISIBLE) && IS_AFFECTED (victim, AFF_HIDE))
        act ("\x1B[1;36m...$N is \x1B[0;36minvisible\x1B[1;36m and \x1B[1;30mhidden\x1B[1;36m.\x1B[0m", ch, NULL, victim, TO_CHAR);
      else
        {
          if (IS_AFFECTED (victim, AFF_INVISIBLE))
            act ("\x1B[1;36m...$N is \x1B[0;36minvisible\x1B[1;36m.\x1B[0m", ch, NULL, victim, TO_CHAR);
          if (IS_AFFECTED (victim, AFF_HIDE))
            act ("\x1B[1;30m...$N is hidden.\x1B[0m", ch, NULL, victim, TO_CHAR);
        }
      if (IS_AFFECTED (victim, AFF_SANCTUARY))
          act ("\x1B[0;1m...$N is protected by a \x1B[1mbright\x1B[0;1m aura!\x1B[0m", ch, NULL, victim, TO_CHAR);
      if (IS_AFFECTED_EXTRA(victim, AFF_FIRESHIELD))
	act ("$4...$N is protected by a shroud of $B$4Fire$R$4!$R$7", ch, NULL, victim, TO_CHAR);
      if (IS_AFFECTED (victim, AFF_FAERIE_FIRE))
	act ("\x1B[1;35m...$N's body is outlined for easy targeting!\x1B[0m", ch, NULL, victim, TO_CHAR);
      if (IS_AFFECTED_EXTRA(victim, AFF_MANASHIELD))
	act ("$3...$N is $1p$Br$R$3o$Bt$7e$3c$R$3t$B$1e$R$1d$R by a $B$0m$R$7a$Bn$0a$R $1s$B$0h$R$7i$Be$R$7l$1d$R!", ch, NULL, victim, TO_CHAR);
      if (IS_AFFECTED (victim, AFF_FLYING))
	{
	  if (IS_MOB(victim))
	    {
	      if(victim->pIndexData->mobtype == MOB_BIRD || victim->pIndexData->mobtype == MOB_DRAGON)
		act ("\x1B[0;1;35m...$N is flying around here.\x1B[0m", ch, NULL,   victim, TO_CHAR);
	      else
		act ("\x1B[0;1;35m...$N is flying around here.\x1B[0m", ch, NULL,   victim, TO_CHAR);
	    }
	  else
	    {
	      if (IS_SET(race_info[victim->pcdata->race].affect_bits, AFF_FLYING))
		act ("\x1B[0;1;35m...$N is flying in the air.\x1B[0m", ch, NULL, victim, TO_CHAR);
	      else
		act ("\x1B[0;1;35m...$N is flying in the air.\x1B[0m", ch,
NULL, victim, TO_CHAR);
	    }
	}
      return;
    }
  return;
}







void
show_char_to_char_1 (CHAR_DATA * victim, CHAR_DATA * ch)
{
  char buf[STD_LENGTH];
  SINGLE_OBJECT *obj;
  int iWear;
  int percent, percent2;
  bool found;

  if (IS_PLAYER (ch) && IS_PLAYER (victim))
    {
      send_to_char (victim->pcdata->name, ch);
      if (LEVEL (victim) < 10)
        send_to_char (" is a experienced of the \x1B[1m", ch);
      else if (LEVEL (victim) < 25)
        send_to_char (" is a member of the \x1B[1m", ch);
      else if (LEVEL (victim) < 45)
        send_to_char (" is well-respected amongst the \x1B[1m", ch);
      else if (LEVEL (victim) < 70)
        send_to_char (" is a hero amongst the \x1B[1m", ch);
      else if (LEVEL (victim) < 75)
        send_to_char (" is a legend amongst the \x1B[1m", ch);
      else if (LEVEL (victim) < 80)
	send_to_char (" is a avatar amongst the \x1B[1m", ch);
      else if (LEVEL (victim) < 90)
	send_to_char (" is a warlord amongst the \x1b[1m", ch);
      else if (LEVEL (victim) < 91)
        send_to_char (" is a warlord amongst the \x1b[1m", ch);
      else
        send_to_char (" is an Immortal who belongs to the \x1B[1m", ch);
      send_to_char (race_info[victim->pcdata->race].name, ch);
      send_to_char ("\x1B[0m race.\n\r", ch);
    }
  if (victim == ch)
    {
      sprintf (buf, "You are %d\' %d\" tall.", ch->height / 12, ch->height % 12);
    }
  else
    {
      sprintf (buf, "$E is");
      if (ch->height > victim->height)
        strcat (buf, " shorter than you.");
      else if (ch->height < victim->height)
        strcat (buf, " taller than you.");
      else
        strcat (buf, " the same height as you.");
    }
  act (buf, ch, NULL, victim, TO_CHAR);
  if (DSC(victim) && DSC (victim)[0] != '\0' && !FIGHTING(ch))
    {
      send_to_char (DSC (victim), ch);
    }
  else
    {
      act ("You don't see anything special about $M.", ch, NULL, victim, TO_CHAR);
    }
  if (victim->max_hit > 0)
    percent = (100 * victim->hit) / victim->max_hit;
  else
    percent = 0;
  if (victim->max_move > 0)
    percent2 = (100 * victim->move) / victim->max_move;
  else
    percent2 = 0;
  send_to_char ("\n\r", ch);
  if (percent > 100)
    percent = 100;
  if (percent2 > 100)
    percent2 = 100;
  if (percent2 < 1) percent2=1;
  strcpy (buf, PERS (victim, ch));
  if (percent > 0)
    sprintf (buf, " %s %s and looks %s.\n\r", capitalize (PERS (victim, ch)),
             percent_ded[percent/10],
             percent_tired[percent2 / 10]);
  else
    sprintf (buf, " %s is \x1B[34mmortally wounded\x1B[0m.\n\r",
             capitalize (PERS (victim, ch)));
  send_to_char (buf, ch);
  {
    found = FALSE;
    for (iWear = 0; iWear < MAX_WEAR; iWear++)
      {
        if ((obj = get_eq_char (victim, iWear)) != NULL
            && can_see_obj (ch, obj))
          {
            if (!found)
              {
                send_to_char ("\n\r", ch);
                act ("$N is using:", ch, NULL, victim, TO_CHAR);
                found = TRUE;
              }
            send_to_char (where_name[iWear], ch);
            send_to_char (format_obj_to (obj, ch, TRUE), ch);
            send_to_char ("\x1B[0m", ch);
          }
      }
  }
  if (victim != ch
      && ((IS_PLAYER (ch) && number_percent () < ch->pcdata->learned[gsn_peek]) || IS_IMMORTAL (ch)))
    {
      send_to_char ("\n\rYou peek at the inventory:\n\r", ch);
      if (tally_coins (victim) > 0)
        {
          char tttt[500];
          sprintf (tttt, "%s coins.\n\r", name_amount (tally_coins (victim)));
          send_to_char (tttt, ch);
        }
      show_list_to_char (victim->carrying, ch, TRUE, FALSE);
      if (number_range (1, 500) == 23)
        skill_gain (ch, gsn_peek, FALSE);
    }
  return;
}

bool
check_blind (CHAR_DATA * ch)
{
  if (IS_PLAYER (ch) && IS_SET (ch->act, PLR_HOLYLIGHT))
    return TRUE;
  if (IS_AFFECTED (ch, AFF_BLIND))
    {
      send_to_char ("\x1B[33mYou can't see a thing!\x1B[0m\n\r", ch);
      return FALSE;
    }
  return TRUE;
}

void
do_glance (CHAR_DATA * ch, char *argy)
{
  char buf[STD_LENGTH];
  CHAR_DATA *victim;
  int percent, percent2;
  DEFINE_COMMAND ("glance", do_glance, POSITION_RESTING, 0, LOG_NORMAL, "This command allows you to glance at the condition of a player or mob.")

    if (ch->position < POSITION_SLEEPING)
    {
      send_to_char ("\x1B[1mYou can't see anything but stars!\x1B[0m\n\r", ch);
      return;
    }
  if (ch->position == POSITION_SLEEPING)
    {
      send_to_char ("\x1B[1;30mYou can't see anything, you're sleeping!\x1B[0m\n\r", ch);
      return;
    }
  if (!check_blind (ch))
    return;
  if (IS_PLAYER (ch)
      && !IS_SET (ch->act, PLR_HOLYLIGHT)
      && (room_is_dark (ch->in_room) && !IS_AFFECTED (ch, AFF_INFRARED)))
    {
      send_to_char ("\x1B[1;30mIt is pitch black ... \x1B[0m\n\r", ch);
      if (ch->in_room && ch->in_room->more) show_char_to_char (ch->in_room->more->people, ch);
      return;
    }
  if ((victim = get_char_room (ch, argy)) != NULL)
    {
      if (victim->max_hit > 0)
        percent = (100 * victim->hit) / victim->max_hit;
      else
        percent = -1;
      if (victim->max_move > 0)
        percent2 = (100 * victim->move) / victim->max_move;
      else
        percent2 = 0;
      if (percent > 100)
        percent = 100;
      if (percent2 > 100)
        percent2 = 100;
      if (percent2 < 1) percent2=1;
      strcpy (buf, PERS (victim, ch));
      if (percent > 0)
        sprintf (buf, "%s %s and looks %s.\n\r", capitalize (PERS (victim, ch)),
                 percent_ded[percent / 10],
                 percent_tired[percent2 / 10]);
      else
        sprintf (buf, " %s is \x1B[34mmortally wounded\x1B[0m.\n\r",
                 capitalize (PERS (victim, ch)));
      send_to_char (buf, ch);
    }
  return;
}

/*
   do_scan looks in all directions for mobs/players
   This uses a char boolean ch->scanning
 */
void
do_scan (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("scan", do_scan, POSITION_RESTING, 0, LOG_NEVER, "This command allows you to scan your surroundings and find players/creatures that are nearby.")

    check_ced (ch);
  if (IS_PLAYER(ch) && !str_cmp(argy, "refresh") && !IS_AFFECTED(ch,
AFF_BLIND) && IS_AFFECTED(ch, AFF_INFRARED) && IS_SET(ch->pcdata->act2,
PLR_MAPPING))
    {
      char buf[SML_LENGTH];
      sprintf(buf, "\x1b[%d;%dr", MAP_HEIGHT, ch->pcdata->pagelen);
      send_to_char(buf, ch);
      ch->pcdata->x = MAP_WIDTH/2;
      ch->pcdata->y = MAP_HEIGHT/2;
      small_map(ch);
      sprintf(buf, "\x1B[%d;1f", ch->pcdata->pagelen);
      send_to_char(buf, ch);
      WAIT_STATE(ch, PULSE_VIOLENCE);
      return;
    }
  ch->ced->scanning = TRUE;

  send_to_char ("You scan all four directions around you:\n\r\n\r", ch);
  do_look (ch, "n");
  do_look (ch, "e");
  do_look (ch, "s");
  do_look (ch, "w");
  do_look (ch, "u");
  do_look (ch, "d");
  ch->ced->scanning = FALSE;
  return;
}


void
do_silentscan (CHAR_DATA * ch, int followers)
{
  return;
  if (IS_MOB(ch)) return;
  check_ced(ch);
  ch->ced->scanning = TRUE;
  scan_direction2 (ch, 0, followers);
  scan_direction2 (ch, 1, followers);
  scan_direction2 (ch, 2, followers);
  scan_direction2 (ch, 3, followers);
/*  scan_direction2 (ch, 4, followers);
  scan_direction2 (ch, 5, followers);*/
  ch->ced->scanning = FALSE;
  return;
}




int
GET_DAMROLL (CHAR_DATA * ch)
{
  return (ch->damroll + add_damroll (ch) + str_app[get_curr_str(ch)].dam);
}

int
GET_HITROLL (CHAR_DATA * ch)
{
  return (ch->hitroll + add_hitroll (ch) + (get_curr_dex(ch)*3/2));
}

int
add_damroll (CHAR_DATA * ch)
{
  return 0;
    return 0;
  if (ch->in_room->sector_type >= SECT_CAVE || IS_SET(ch->in_room->room_flags, ROOM_UNDERGROUND))
    return 4;
  if (IS_SET (ch->in_room->room_flags, ROOM_INDOORS))
    return 2;
  if (weather_info.sunlight > SUN_DARK)
    {
      if (weather_info.sky > SKY_CLOUDLESS)
        return -2;
      else
        return -7;
    }
  return 4;
}

int
add_hitroll (CHAR_DATA * ch)
{
    return 0;
  if (ch->in_room->sector_type >= SECT_CAVE || IS_SET(ch->in_room->room_flags, ROOM_UNDERGROUND))
    return 8;
  if (IS_SET (ch->in_room->room_flags, ROOM_INDOORS))
    return 4;
  if (weather_info.sunlight > SUN_DARK)
    {
      if (weather_info.sky > SKY_CLOUDLESS)
        return -5;
      else
        return -13;
    }
  return 8;
}



void
do_look (CHAR_DATA * ch, char *argy)
{
  char buf[STD_LENGTH];
  char arg1[SML_LENGTH];
  char arg2[SML_LENGTH];
  char buff[SML_LENGTH];
  EXIT_DATA *pexit;
  CHAR_DATA *victim;
  SINGLE_OBJECT *obj;
  char *pdesc;
  int door;
  int count, number;
  DEFINE_COMMAND ("look", do_look, POSITION_RESTING, 0, LOG_NORMAL, "This command enables you to look at your surroundings, at or in an object, or in a direction.")
/*Handles message boards if the character likes READ instead of NOTE */
if (!ch->in_room) {
        fprintf(stderr,"%s null room: look.\n",NAME(ch));
        return;
        }
    if (argy != '\0' && is_number (argy))
    {
      char buffie[25];
      sprintf (buffie, "read %d", atoi (argy));
      do_note (ch, buffie);
      return;
    }
  if (IS_PLAYER (ch) && (!str_cmp (argy, "at board") || !str_cmp (argy, "board")))
    {
      do_note (ch, "list");
      return;
    }

  buf[0] = '\0';
  if (ch->position < POSITION_SLEEPING)
    {
      send_to_char ("\x1B[1mYou can't see anything but stars!\x1B[0m\n\r", ch);
      return;
    }
  if (ch->position == POSITION_SLEEPING)
    {
      send_to_char ("\x1B[1;30mYou can't see anything, you're sleeping!\x1B[0m\n\r", ch);
      return;
    }
  if (!check_blind (ch))
    return;
  if (weather_info.sky == SKY_FOGGY
      && number_bits (2) != 0
      && IS_OUTSIDE (ch)
      && !IS_SET (ch->act, PLR_HOLYLIGHT) && !IS_AFFECTED_EXTRA(ch, AFF_DET_FOG))
    {
      send_to_char ("It's hard to see through this fog.\n\r", ch);
      return;
    }
  argy = one_argy (argy, arg1);

  if (str_cmp (arg1, "in") && str_cmp (arg1, "i") && str_cmp (arg1, "inside"))
    while (str_cmp (arg1, "in") && argy != "" && argy[0] != '\0')
      argy = one_argy (argy, arg1);

  argy = one_argy (argy, arg2);
  while (argy != "" && argy[0] != '\0')
    argy = one_argy (argy, arg2);

  if (!str_cmp (arg1, "n") ||
      !str_cmp (arg1, "w") ||
      !str_cmp (arg1, "e") ||
      !str_cmp (arg1, "s") ||
      !str_cmp (arg1, "north") ||
      !str_cmp (arg1, "south") ||
      !str_cmp (arg1, "east") ||
      !str_cmp (arg1, "west") ||
      !str_cmp (arg1, "up") ||
      !str_cmp (arg1, "down") ||
      !str_cmp (arg1, "u") ||
      !str_cmp (arg1, "d"))
    goto lookdirection;
  if (IS_PLAYER (ch)
      && !IS_SET (ch->act, PLR_HOLYLIGHT)
      && room_is_dark (ch->in_room) && !IS_AFFECTED (ch, AFF_INFRARED))
    {
      send_to_char ("\x1B[1;30mIt is pitch black ... \x1B[0m\n\r", ch);
      return;
    }
  if (arg1[0] == '\0' || !str_cmp (arg1, "auto"))
    {
      sprintf(buff, "\x1b[1;37m%s \x1b[1;34m[%s%s\x1b[1;34m/", show_room_name (ch, ch->in_room->name), sectors[ch->in_room->sector_type].color, sectors[ch->in_room->sector_type].name_seen);
      if (ch->in_room->sector_type >= SECT_CAVE||IS_SET(ch->in_room->room_flags, ROOM_UNDERGROUND))
        strcat (buff, "\x1b[1;30mUnderground\x1b[34m]\x1b[0;37m\n\r");
      else  if (IS_SET (ch->in_room->room_flags, ROOM_INDOORS))
        strcat (buff, "\x1b[0;33mIndoors\x1b[34m]\x1b[0;37m\n\r");
      else if (weather_info.sunlight > SUN_DARK)
        {
	  if (weather_info.sky > SKY_CLOUDLESS)
	    {
	      switch (weather_info.sky)
		{
		case SKY_RAINING:
		  strcat (buff, "Raining]\x1b[0;37m\n\r");
		  break;
		case SKY_FOGGY:
		  strcat (buff, "\x1b[0;36mFoggy\x1b[34m]\x1b[0;37m\n\r");
		  break;
		case SKY_LIGHTNING:
		  strcat (buff, "\x1b[1;33mLi\x1b[0;33mghtni\x1b[1;33mng\x1b[34m]\x1b[0;37m\n\r");
		  break;
		case SKY_CLOUDY:
		  strcat (buff, "\x1b[0;37mOvercast\x1b[34m]\x1b[0;37m\n\r");
		  break;
		}
	    }
          else if (ch->in_room->shade)
            strcat (buff, "\x1b[30mShaded\x1b[34m]\x1b[0;37m\n\r");
          else
            {
	      switch (weather_info.sunlight)
		{
		case SUN_RISE:
		  strcat (buff, "\x1b[31mSunrise\x1b[34m]\x1b[0;37m\n\r");
		  break;
		case SUN_MORNING:
		  strcat (buff, "\x1b[33mMorning\x1b[34m]\x1b[0;37m\n\r");
		  break;
		case SUN_MIDDAY:
		  strcat (buff, "\x1b[33mMidday\x1b[34m]\x1b[0;37m\n\r");
		  break;
		case SUN_AFTERNOON:
		  strcat (buff, "\x1b[33mAfternoon\x1b[34m]\x1b[0;37m\n\r");
		  break;
		case SUN_EVENING:
		  strcat (buff, "\x1b[33mEvening\x1b[34m]\x1b[0;37m\n\r");
		  break;
		case SUN_SET:
		  strcat (buff, "\x1b[31mSunset\x1b[34m]\x1b[0;37m\n\r");
		  break;
		}
	    }
	}
      else
	{
	  if (weather_info.sky == SKY_LIGHTNING)
	    strcat(buff, "\x1b[33mLightning\x1b[34m/\x1b[30mNight\x1b[34m]\x1b[0;37m\n\r");
          else if (weather_info.sky == SKY_RAINING)
            strcat (buff, "Raining/\x1b[30mNight\x1b[34m]\x1b[0;37m\n\r");
          else
            strcat (buff, "\x1b[30mNight\x1b[34m]\x1b[0;37m\n\r");
        }
      send_to_char(buff, ch);
        if (arg1[0] == '\0' || (IS_PLAYER (ch) &&
				!IS_SET (ch->pcdata->act2, PLR_BRIEF)))
	  send_to_char (ch->in_room->description, ch);
	if (IS_PLAYER (ch) && IS_SET (ch->pcdata->act2, PLR_AUTOEXIT))
	  {
	    ansi_color (BOLD, ch);
	    ansi_color (BLUE, ch);
	    do_exits (ch, "auto");
	    ansi_color (NTEXT, ch);
	  }
	if(ch->in_room->blood > 0)
	  {
	  int i;
	  char bufg[1000];
	  char buff[100];
	  sprintf(bufg, "\x1b[0;31m");
	  for (i = 0; i < 6; i++)
	    {
	      if (IS_SET(ch->in_room->blood, blood_number[i]))
		{
		  sprintf(buff, "There is a trail of blood leading \x1b[1;31m%s\x1b[0;31m.\n\r", dir_name[i]);
		  strcat(bufg, buff);
		}
	    }
	  strcat(bufg, "\x1b[0;37m");
	  send_to_char(bufg, ch);
	}
      if (ch->in_room->more->contents)
        {
          ansi_color (NTEXT, ch);
          ansi_color (CYAN, ch);
        }
      if (ch->in_room->more->gold > 0 || ch->in_room->more->copper > 0)
        {
          char bufff[500];
          if (ch->in_room->more->gold > 0 && ch->in_room->more->copper == 0)
            {
              sprintf (bufff, "\x1B[37;0mThere %s %d \x1B[1;33mgold\x1B[37;0m coin%s here.\x1B[37;0m\n\r",
                       (ch->in_room->more->gold > 1 ? "are" : "is"), ch->in_room->more->gold,
                       (ch->in_room->more->gold > 1 ? "s" : ""));
            }
          else if (ch->in_room->more->gold == 0 && ch->in_room->more->copper > 0)
            {
              sprintf (bufff, "\x1B[37;0mThere %s %d \x1B[33;0mcopper\x1B[37;0m coin%s here.\x1B[37;0m\n\r",
                       (ch->in_room->more->copper > 1 ? "are" : "is"), ch->in_room->more->copper,
                       (ch->in_room->more->copper > 1 ? "s" : ""));
            }
          else if (ch->in_room->more->gold > 0 && ch->in_room->more->copper > 0)
            {
              sprintf (bufff, "\x1B[37;0mThere %s %d \x1B[1;33mgold\x1B[37;0m coin%s and %d \x1B[33;0mcopper\x1B[37;0m coin%s here.\x1B[37;0m\n\r",
                       (ch->in_room->more->gold > 1 ? "are" : "is"), ch->in_room->more->gold,
                       (ch->in_room->more->gold > 1 ? "s" : ""), ch->in_room->more->copper,
                       (ch->in_room->more->copper > 1 ? "s" : ""));
            }
          send_to_char (bufff, ch);
        }

      show_list_to_char (ch->in_room->more->contents, ch, FALSE, FALSE);
      show_char_to_char (ch->in_room->more->people, ch);
if (arg1[0]=='\0' || (ch->in_room && ch->in_room->more &&
        (ch->in_room->more->contents || ch->in_room->more->people!=ch ||
        ch->in_room->more->people->next_in_room))) {
     send_to_char ("\x1B[37;0m", ch);
        }
      return;
    }
  if (!str_cmp (arg1, "i") || !str_cmp (arg1, "in") || !str_cmp (arg1, "inside"))
    {
      /* 'look in' */
      if (arg2[0] == '\0')
        {
          send_to_char ("Look in what?\n\r", ch);
          return;
        }
      if ((obj = get_obj_here (ch, arg2, SEARCH_INV_FIRST)) == NULL)
        {
          send_to_char ("You do not see that here.\n\r", ch);
          return;
        }
      switch (obj->pIndexData->item_type)
        {
        default:
          send_to_char ("That is not a container.\n\r", ch);
          break;
        case ITEM_LIGHT:
          {
            I_LIGHT *light = (I_LIGHT *) obj->more;
            if (obj->carried_by && obj->wear_loc == WEAR_NONE)
              send_to_char ("(In Inventory) ", ch);
            if (obj->carried_by && obj->wear_loc != WEAR_NONE)
              send_to_char ("(Equipped) ", ch);
            if (!obj->carried_by)
              send_to_char ("(On the Ground) ", ch);
            if (light->max_light != 0)
              {
                int percent = PERCENTAGE (light->light_now, light->max_light);
                if (light->light_now <= 0)
                  {
                    send_to_char ("It is empty.\n\r", ch);
                    break;
                  }
                else
                  {
                    sprintf (buf, "It's %s of fuel.\n\r",
                             percent < 10 ? "almost empty" :
                             percent < 40 ? "less than half full" :
                             percent < 60 ? "half full" :
                             percent < 90 ? "more than half" : "full");
                    send_to_char (buf, ch);
                  }
              }
            else
              send_to_char ("It contains nothing.\n\r", ch);
            break;
          }
        case ITEM_DRINK_CON:
          {
            I_DRINK *dr = (I_DRINK *) obj->more;
            if (obj->carried_by && obj->wear_loc == WEAR_NONE)
              send_to_char ("(In Inventory) ", ch);
            if (obj->carried_by && obj->wear_loc != WEAR_NONE)
              send_to_char ("(Equipped) ", ch);
            if (!obj->carried_by)
              send_to_char ("(On the Ground) ", ch);
            if (dr->liquid_now <= 0)
              {
                send_to_char ("It is empty.\n\r", ch);
                break;
              }
            else
              {
                int percent = PERCENTAGE (dr->liquid_now, dr->max_liquid);
                if (dr->liquid_type == -1)
                  {
                    if (obj->carried_by && obj->wear_loc == WEAR_NONE)
                      send_to_char ("(In Inventory) ", ch);
                    if (obj->carried_by && obj->wear_loc != WEAR_NONE)
                      send_to_char ("(Equipped) ", ch);
                    if (!obj->carried_by)
                      send_to_char ("(On the Ground) ", ch);
                    sprintf (buf, "It's %s of lamp oil.\n\r",
                             percent < 10 ? "almost empty" :
                             percent < 40 ? "less than half full" :
                             percent < 60 ? "half full" :
                             percent < 70 ? "more than half full" :
                             "full");
                  }
                else
                  {
                    if (obj->carried_by && obj->wear_loc == WEAR_NONE)
                      send_to_char ("(In Inventory) ", ch);
                    if (obj->carried_by && obj->wear_loc != WEAR_NONE)
                      send_to_char ("(Equipped) ", ch);
                    if (!obj->carried_by)
                      send_to_char ("(On the Ground) ", ch);
                    sprintf (buf, "\x1b[0;36mIt's %s of a %s liquid.\x1b[0;37m\n\r",
                             percent < 10 ? "almost empty" :
                             percent < 40 ? "less than half full" :
                             percent < 60 ? "half full" :
                             percent < 70 ? "more than half full" :
                             "full",
                             (dr->liquid_type > 12 ? "ERROR!!" : liq_table[dr->liquid_type].liq_color));
		    if (percent <= 0)
			sprintf (buf, "It's empty.\n\r");
                  }
                send_to_char (buf, ch);
              }
            break;
          }
        case ITEM_CONTAINER:
        case ITEM_CORPSE_NPC:
        case ITEM_CORPSE_PC:
          {
            I_CONTAINER *con = (I_CONTAINER *) obj->more;
            if (obj->carried_by && obj->wear_loc == WEAR_NONE)
              send_to_char ("(In Inventory) ", ch);
            if (obj->carried_by && obj->wear_loc != WEAR_NONE)
              send_to_char ("(Equipped) ", ch);
            if (!obj->carried_by)
              send_to_char ("(On the Ground) ", ch);
            if (IS_SET (con->flags, CONT_CLOSED))
              {
                send_to_char ("It is closed.\n\r", ch);
                break;
              }
            if (LEVEL (ch) > 109)
              {
                print_reasons (ch, obj);
              }
            act ("$p\x1B[0m contains:", ch, obj, NULL, TO_CHAR);
            if (con->money > 0)
              {
                char ttu[500];
                sprintf (ttu, "%s coins.\n\r", name_amount (con->money));
                send_to_char (ttu, ch);
              }
            show_list_to_char (obj->contains, ch, TRUE, TRUE);
            break;
          }
        }
      return;
    }
  if ((victim = get_char_room (ch, arg1)) != NULL)
    {
      show_char_to_char_1 (victim, ch);
      if (IS_MOB (victim) && (FIGHTING (victim) == NULL) &&
          IS_SET (victim->act, ACT_ATTITUDE) && can_see (victim, ch) &&
          IS_AWAKE (victim))
        {
          do_say (victim, "How dare you look at me! You will die!");
          multi_hit (victim, ch, TYPE_UNDEFINED);
        }
      return;
    }
  count = number_argy (arg1, arg2);
  number = 0;
  for (obj = ch->carrying; obj && obj != NULL; obj = obj->next_content)
    {
      if (can_see_obj (ch, obj))
        {
          if (is_name (arg2, obj->pIndexData->name))
            {
              if (++number != count)
                continue;
              send_to_char ("(This object is in your inventory)\n\r", ch);
	      if (obj->pIndexData->value[10] != 0)
	      {
		char buf[500];
		int i=0;
		bool prev=FALSE;
		send_to_char ("This item requires the training of the following guilds: \x1B[1;37m", ch);
		for (i = 0; str_cmp(guild_data[i].what_you_type,"end_of_list");i ++)
	        {
		   if (IS_SET(obj->pIndexData->value[10], guild_data[i].mob_guildmaster_bit))
		   {
			if (prev)
			    send_to_char("\x1B[0;37m,\x1B[1;37m", ch);
			sprintf (buf, guild_data[i].what_you_see);
			send_to_char (buf, ch);
			prev = TRUE;
		   }
		}
		send_to_char ("\x1B[0;37m.\n\r", ch);
	      }
              pdesc = get_extra_descr (arg1, obj->pIndexData->extra_descr);
              if (!pdesc && obj->pIndexData->extra_descr && (!str_cmp(obj->pIndexData->name,arg1) ||
                             !str_cmp(obj->pIndexData->name,arg2)))
                        pdesc=obj->pIndexData->extra_descr->description;

              if (pdesc != NULL)
                {

                  page_to_char (wordwrap(pdesc,79), ch);
                }
               else
              if (pdesc==NULL)
                act ("You see nothing special about $p.", ch, obj, ch, TO_CHAR);
              return;
            }
        }
    }
  for (obj = ch->carrying; obj && obj != NULL; obj = obj->next_content)
    {
      if (can_see_obj (ch, obj))
        {
          if (is_approx_name (arg2, obj->pIndexData->name))
            {
              if (++number != count)
                continue;
              send_to_char ("(This object is in your inventory)\n\r", ch);
              pdesc = get_extra_descr (arg1, obj->pIndexData->extra_descr);
              if (pdesc != NULL)
                {
                  page_to_char (pdesc, ch);
                  return;
                }
              act ("You see nothing special about $p.", ch, obj, ch, TO_CHAR);
              return;
            }
        }
    }
  number = 0;
  for (obj = ch->in_room->more->contents; obj && obj != NULL; obj = obj->next_content)
    {
      if (can_see_obj (ch, obj) && !(IS_SET(obj->extra_flags,
ITEM_UNSEEN) && !IS_SET(ch->act, PLR_HOLYLIGHT)))
        {
          if (is_name (arg2, obj->pIndexData->name))
            {
              if (++number != count)
                continue;
              send_to_char ("(This object is in the room)\n\r", ch);
              pdesc = get_extra_descr (arg1, obj->pIndexData->extra_descr);
              if (pdesc != NULL)
                {
                  page_to_char (pdesc, ch);
                  return;
                }
              act ("You don't see anything special about $p.", ch, obj, ch, TO_CHAR);
              return;
            }
        }
    }
  for (obj = ch->in_room->more->contents; obj && obj != NULL; obj = obj->next_content)
    {
      if (can_see_obj (ch, obj) && !(IS_SET(obj->extra_flags,
ITEM_UNSEEN) && !IS_SET(ch->act, PLR_HOLYLIGHT)))
        {
          if (is_approx_name (arg2, obj->pIndexData->name))
            {
              if (++number != count)
                continue;
              send_to_char ("(This object is in the room)\n\r", ch);
              pdesc = get_extra_descr (arg1, obj->pIndexData->extra_descr);
              if (pdesc != NULL)
                {
                  page_to_char (pdesc, ch);
                  return;
                }
              act ("You don't see anything special about $p.", ch, obj, ch, TO_CHAR);
              return;
            }
        }
    }
  pdesc = NULL;
  if (ch->in_room->more)
    pdesc = get_extra_descr (arg1, ch->in_room->more->extra_descr);
  if (pdesc != NULL)
    {
      page_to_char (pdesc, ch);
      return;
    }
  send_to_char ("You do not see that here.\n\r", ch);
  return;
lookdirection:
  if (!str_cmp (arg1, "n") || !str_cmp (arg1, "north"))
    door = 0;
  else if (!str_cmp (arg1, "e") || !str_cmp (arg1, "east"))
    door = 1;
  else if (!str_cmp (arg1, "s") || !str_cmp (arg1, "south"))
    door = 2;
  else if (!str_cmp (arg1, "w") || !str_cmp (arg1, "west"))
    door = 3;
  else if (!str_cmp (arg1, "u") || !str_cmp (arg1, "up"))
    door = 4;
  else if (!str_cmp (arg1, "d") || !str_cmp (arg1, "down"))
    door = 5;
  else
    {
      send_to_char ("You can't look that way.\n\r", ch);
      return;
    }
/* 'look direction' */
  if ((pexit = ch->in_room->exit[door]) == NULL)
    {
      if (!ch->ced || !ch->ced->scanning)
        send_to_char ("You do not see anything in that direction.\n\r", ch);
      return;
    }
  if (!ch->ced || !ch->ced->scanning)
    {
      if (pexit->d_info && pexit->d_info->str < 0)
        {
          send_to_char ("There used to be a door there, but now it's just rubble.\n\r",
                        ch);
          goto doscan;
        }
      if (pexit->d_info && pexit->d_info->description != NULL && pexit->d_info->description[0] != '\0')
        {
          char *t;
          char ed[20];
          int ct = 0;
          for (t = pexit->d_info->description; *t != '\0' && ct < 20; t++)
            {
              if (*t != '\n' && *t != '\r')
                {
                  ed[ct] = *t;
                  ct++;
                }
            }
          ed[ct] = '\0';
          if (is_number (ed))
            {
              int lvnum;
              ROOM_DATA *lr;
              lvnum = atoi (ed);
              if ((lr = get_room_index (lvnum)) != NULL)
                {
                  char ttt[200];
                  send_to_char ("You look and see........\n\r", ch);
                  sprintf (ttt, "%d look", lvnum);
                  do_at (ch, ttt);
                }
            }
          else
            send_to_char (pexit->d_info->description, ch);
        }
      else
        send_to_char ("You do not see anything in that direction.\n\r", ch);
      if ((ROOM_DATA *) pexit->to_room == NULL)
        return;
      if (pexit->d_info && pexit->d_info->keyword != NULL
          && pexit->d_info->keyword[0] != '\0'
          && pexit->d_info->keyword[0] != ' ')
        {
        }
      /*end if sc->scanning */
      if (pexit->d_info && IS_SET (pexit->d_info->exit_info, EX_HIDDEN) && IS_SET (pexit->d_info->exit_info, EX_CLOSED))
        {
          return;
        }
      if (!ch->ced || !ch->ced->scanning)
        {
          if (pexit->d_info && IS_SET (pexit->d_info->exit_info, EX_CLOSED) && pexit->d_info->keyword != NULL && pexit->d_info->keyword[0] != '\0')
            {
              act ("The $T\x1B[0m is closed.", ch, NULL, pexit->d_info->keyword, TO_CHAR);
              if (!str_cmp ("door", pexit->d_info->keyword) || !str_prefix ("door", pexit->d_info->keyword)
                  || !str_suffix ("door", pexit->d_info->keyword))
                {
                  if (pexit->d_info->str > 50)
                    send_to_char ("The door looks impenetrable!\n\r", ch);
                  else if (pexit->d_info->str > 40)
                    send_to_char ("The door looks very solid!\n\r", ch);
                  else if (pexit->d_info->str > 30)
                    send_to_char ("The door could hold up pretty well.\n\r", ch);
                  else if (pexit->d_info->str > 20)
                    send_to_char ("It wouldn't take a big man long to break.\n\r", ch);
                  else if (pexit->d_info->str > 10)
                    send_to_char ("What could this thing keep out?!\n\r", ch);
                  else
                    send_to_char ("You call that a door?!?!\n\r", ch);
                  return;
                }
            }
          else if (pexit->d_info && IS_SET (pexit->d_info->exit_info, EX_ISDOOR) && pexit->d_info->keyword != NULL && pexit->d_info->keyword[0] != '\0')
            {
              act ("The $T\x1B[0m is open.", ch, NULL, pexit->d_info->keyword, TO_CHAR);
              act ("As you peer for signs of life, you find that you cannot make out any\n\rlifeforms due to the limited line of sight behind the open $T.", ch, NULL, pexit->d_info->keyword, TO_CHAR);
              return;
            }
        }
      else
        {
          if (pexit && (ROOM_DATA *) pexit->to_room != NULL)
            scan_direction (ch, door);
          return;
        }
    }
/*end if !ch->scanning */
doscan:
  if ((ROOM_DATA *) pexit->to_room != NULL)
    scan_direction (ch, door);
  return;
}

void
do_examine (CHAR_DATA * ch, char *argy)
{
  char buf[STD_LENGTH];
  char arg[SML_LENGTH];
  SINGLE_OBJECT *obj;
  DEFINE_COMMAND ("examine", do_examine, POSITION_RESTING, 0, LOG_NORMAL, "This command first looks at an object, then looks in the object.")

    one_argy (argy, arg);
  if (arg[0] == '\0')
    {
      send_to_char ("Examine what?\n\r", ch);
      return;
    }
  if ((obj = get_obj_wear (ch, arg)) != NULL)
    {
      do_look (ch, obj->pIndexData->name);
      switch (obj->pIndexData->item_type)
        {
        default:
          break;
        case ITEM_LIGHT:
          {
            I_LIGHT *light = (I_LIGHT *) obj->more;
            if (light->max_light != 0)
              {
                int percent = PERCENTAGE (light->light_now, light->max_light);
                sprintf (buf, "\n\r%s\x1B[0m %s %s\n\r",
                         capitalize (OOSTR (obj, short_descr)),
                         is_are (OOSTR (obj, short_descr)),
                         !IS_LIT (obj) ? "extinguished." :
                         percent < 10 ? "flickering and sputtering." :
                         percent < 20 ? "flickering." :
                         percent < 30 ? "flickering slightly." :
                         percent < 40 ? "providing ample light." :
                         "glowing brightly.");
                send_to_char (buf, ch);
              }
            break;
          }
        case ITEM_GEM:
          {
            I_GEM *gem = (I_GEM *) obj->more;
            if (gem->mana_now)
              {
                int percent;
                percent = gem->mana_now;
                sprintf (buf, "\n\r%s\x1B[0m %s\n\r",
                         capitalize (OOSTR (obj, short_descr)),
                         percent < 10 ? "is nearly drained." :
                         percent < 30 ? "glows softly." :
                         percent < 50 ? "glows with moderate energy." :
                         percent < 70 ? "glows brightly." :
                         percent < 90 ? "blinds you with its intense glow." :
                         percent < 110 ? "radiates with a powerful energy." :
                         "glows with an amazing white light.");
                send_to_char (buf, ch);
                return;
              }
            break;
          }
        case ITEM_CONTAINER:
           {
            I_CONTAINER *con = (I_CONTAINER *)obj->more;
            if (IS_SET(con->flags, CONT_HASTRAP) && IS_PLAYER(ch) &&
               number_range(1,140) <ch->pcdata->learned[gsn_locate_trap])
              {
            send_to_char("That container appears to be trapped.\n\r", ch);
            return;
               }
          send_to_char ("(Equipped) You look inside and see:\n\r", ch);
          sprintf (buf, "in %s", arg);
          do_look (ch, buf);

           }
         break;
        case ITEM_DRINK_CON:
        case ITEM_CORPSE_NPC:
        case ITEM_CORPSE_PC:
          send_to_char ("(Equipped) You look inside and see:\n\r", ch);
          sprintf (buf, "in %s", arg);
          do_look (ch, buf);
        }
      return;
    }
  if ((obj = get_obj_here (ch, arg, SEARCH_INV_FIRST)) != NULL)
    {
      do_look (ch, obj->pIndexData->name);
      switch (obj->pIndexData->item_type)
        {
        default:
          break;
        case ITEM_DRINK_CON:
        case ITEM_CORPSE_NPC:
        case ITEM_CORPSE_PC:
          if (obj->carried_by)
            send_to_char ("\x1B[37;0m(In Inventory) You look inside and see:\n\r", ch);
          else
            send_to_char ("\x1B[37;0m(On the Ground) You look inside and see:\n\r", ch);
          sprintf (buf, "in %s", arg);
          do_look (ch, buf);
          break;
        case ITEM_CONTAINER:
           {
            I_CONTAINER *con = (I_CONTAINER *)obj->more;
            if (IS_SET(con->flags, CONT_HASTRAP) && IS_PLAYER(ch) &&
               number_range(1,140) <ch->pcdata->learned[gsn_locate_trap])
              {
            send_to_char("That container appears to be trapped.\n\r", ch);
            return;
               }
          if (obj->carried_by)
            send_to_char ("\x1B[37;0m(In Inventory) You look inside and see:\n\r", ch);
          else
            send_to_char ("\x1B[37;0m(On the Ground) You look inside and see:\n\r", ch);
          sprintf (buf, "in %s", arg);
          do_look (ch, buf);
          break;
          }
        }
      return;
    }
  do_look (ch, arg);
  return;
}

void
do_dir (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("dir", do_dir, POSITION_RESTING, 0, LOG_NORMAL, "See 'exits'.")

    do_exits (ch, argy);
  return;
}

void
do_exits (CHAR_DATA * ch, char *argy)
{
  extern char *const dir_name[];
  char buf[STD_LENGTH];
  char buf1[50];
  EXIT_DATA *pexit;
  bool found;
  bool fAuto;
  int door;
  ROOM_DATA *troom;
  DEFINE_COMMAND ("exits", do_exits, POSITION_RESTING, 0, LOG_NORMAL, "This displays the available exit directions.  It may not show doors or unaccessable exits.")
    buf[0] = '\0';
  fAuto = !str_cmp (argy, "auto");
  if (!check_blind (ch))
    return;
  strcpy (buf, fAuto ? "\r \x1b[1;34m[Exits:" : "Obvious exits:\x1b[0;37m\n\r");
  found = FALSE;
  for (door = 0; door <= 5; door++)
    {
      if ((pexit = ch->in_room->exit[door]) != NULL
          && (troom = (ROOM_DATA *) pexit->to_room) != NULL)
        {
          if (fAuto)
            {
	      switch(troom->sector_type)
		{
		case SECT_AIR:
		case SECT_CLOUDS:
		  {
		    char buf1[50];
		    if (IS_AFFECTED (ch, AFF_FLYING) || IS_SET (ch->act, PLR_HOLYWALK))
		      {
			sprintf (buf1, " <%s>", dir_name[door]);
			found = TRUE;
			strcat (buf, buf1);
		      }
		    else if (IS_SET (troom->room_flags2, ROOM2_FALLSFAST))
		      {
			sprintf(buf1, " %s", dir_name[door]);
			found = TRUE;
			strcat (buf, buf1);
		      }
		  }
		break;
		case SECT_WATER_SWIM:
		case SECT_WATER_NOSWIM:
		  {
		    sprintf (buf1, " /%s\\", dir_name[door]);
		    found = TRUE;
		    strcat (buf, buf1);
		  }
		break;
		case SECT_UNDERWATER:
		  {
		    if (IS_AFFECTED (ch, AFF_BREATH_WATER) || IS_SET (ch->act, PLR_HOLYWALK) || IS_SET(ch->act, PLR_HAS_SCUBA))
		      {
			if (pexit->d_info && IS_SET (pexit->d_info->exit_info, EX_CLOSED) && IS_SET(pexit->d_info->exit_info, EX_ISDOOR))
			  sprintf (buf1, " {[%s]}", dir_name[door]);
			else
			  sprintf (buf1, " {%s}", dir_name[door]);
			found = TRUE;
			strcat (buf, buf1);
		      }
		  }
		break;
		default:
		  if (pexit->d_info &&
		      IS_SET (pexit->d_info->exit_info, EX_ISDOOR) &&
		      IS_SET(pexit->d_info->exit_info, EX_CLOSED))
		    {
		      if (IS_SET(pexit->d_info->exit_info, EX_HIDDEN))
			{
			  if (IS_SET(ch->act, PLR_HOLYLIGHT))
			    {
			      found = TRUE;
			      sprintf (buf1, " (%s)", dir_name[door]);
			      strcat (buf, buf1);
			    }
			}
		      else
			{
			  found = TRUE;
			  sprintf (buf1, " [%s]", dir_name[door]);
			  strcat (buf, buf1);
			}
		    }
		  else
		    {
		      sprintf(buf1, " %s", dir_name[door]);
		      found = TRUE;
		      strcat (buf, buf1);
		    }
		  break;
		}
	    }
          else
	    {
	      if (!pexit->d_info
		  || !IS_SET (pexit->d_info->exit_info, EX_CLOSED)
		  || IS_SET (ch->act, PLR_HOLYLIGHT))
		{
		  found = TRUE;
		  sprintf (buf + strlen (buf), "%-5s - %s\n\r",  capitalize (dir_name[door]),  (!IS_SET (ch->act, PLR_HOLYLIGHT) && room_is_dark (troom)) ? "Too dark to tell" : (show_room_name (ch, (troom->name))));
		}
	    }
	}
    }
  if (!found)
    strcat (buf, fAuto ? " none" : "None.\n\r");
  if (fAuto)
    strcat (buf, "]\n\r");
  send_to_char (buf, ch);
  return;
}

void
do_purse (CHAR_DATA * ch, char *argy)
{
  char buffy[500];
  int found;
  DEFINE_COMMAND ("purse", do_purse, POSITION_SLEEPING, 0, LOG_NORMAL, "This command shows a convenient display of the coins you are carrying, and those in your bank.")

    sprintf (buffy, "You have \x1B[1;37m%ld\x1B[0m coins in the bank.\n\r", ch->pcdata->bank);
  send_to_char (buffy, ch);
  send_to_char ("\n\r", ch);
  if (!ch->gold && !ch->copper)
    {
      send_to_char ("You are carrying no coins on your person.\n\r", ch);
      return;
    }
  if (ch->gold > 0)
    {
      found = 1;
      sprintf (buffy, "%d \x1B[1;33mgold\x1B[37;0m coins.\n\r", ch->gold);
      send_to_char (buffy, ch);
    }
  if (ch->copper > 0)
    {
      found = 1;
      sprintf (buffy, "%d \x1B[33;0mcopper\x1B[37;0m coins.\n\r", ch->copper);
      send_to_char (buffy, ch);
    }
  return;
}


void
do_inventory (CHAR_DATA * ch, char *argy)
{
  char buffy[1000];
  int found;
  DEFINE_COMMAND ("inventory", do_inventory, POSITION_DEAD, 0, LOG_NORMAL, "This command shows what is currently in your inventory.")
inventory=TRUE;
    send_to_char ("You are carrying:\n\r", ch);
  found = 0;
  if (ch->gold > 0)
    {
      found = 1;
      sprintf (buffy, "%d \x1B[1;33mgold\x1B[37;0m coins.\n\r", ch->gold);
      send_to_char (buffy, ch);
    }
  if (ch->copper > 0)
    {
      found = 1;
      sprintf (buffy, "%d \x1B[33;0mcopper\x1B[37;0m coins.\n\r", ch->copper);
      send_to_char (buffy, ch);
    }

  show_list_to_char (ch->carrying, ch, TRUE, TRUE);
  if (!found && ch->carrying == NULL)
    send_to_char (" Nothing.\n\r", ch);

  return;
}

void
do_vequipment (CHAR_DATA * ch, char *argy)
{
  SINGLE_OBJECT *obj;
  int iWear;
  bool found;
  DEFINE_COMMAND ("vequipment", do_vequipment, POSITION_DEAD, 0, LOG_NORMAL, "This command shows what objects you currently are wearing/have equipped.")

    send_to_char ("You are using:\n\r", ch);
  found = FALSE;
  for (iWear = 0; iWear < MAX_WEAR; iWear++)
    {
      if ((obj = get_eq_char (ch, iWear)) == NULL)
        continue;
      send_to_char (where_name[iWear], ch);
      if (can_see_obj (ch, obj))
        {
          send_to_char (format_obj_to (obj, ch, TRUE), ch);
          send_to_char ("\x1B[0m", ch);
        }
      else
        {
          send_to_char ("\x1B[1;30msomething\x1B[0m.\n\r", ch);
        }
      found = TRUE;
    }

  if (!found)
    send_to_char (" Nothing.\n\r", ch);
  return;
}



void
do_equipment (CHAR_DATA * ch, char *argy)
{
  SINGLE_OBJECT *obj;
  int iWear;
  bool found;
  DEFINE_COMMAND ("equipment", do_equipment, POSITION_DEAD, 0, LOG_NORMAL, "This command shows what objects you currently are wearing/have equipped.")


    send_to_char ("You are using:\n\r", ch);
  found = FALSE;
  for (iWear = 0; iWear < MAX_WEAR; iWear++)
    {
      if ((obj = get_eq_char (ch, iWear)) == NULL)
        continue;
      send_to_char (where_name[iWear], ch);
      if (can_see_obj (ch, obj))
        {
          send_to_char (format_obj_to (obj, ch, TRUE), ch);
          send_to_char ("\x1B[0m", ch);
        }
      else
        {
          send_to_char ("\x1B[1;30msomething\x1B[0m.\n\r", ch);
        }
      found = TRUE;
    }

  if (!found)
    send_to_char (" Nothing.\n\r", ch);
  return;
}


void
do_report (CHAR_DATA * ch, char *argy)
{
  char buf[SML_LENGTH];
  DEFINE_COMMAND ("report", do_report, POSITION_SLEEPING, 0, LOG_NORMAL, "This command reports your condition to the room you're in.  It works while asleep as well.")

 sprintf (buf, "You report: <%s and I'm %s.>  I am also \x1b[0;36ml\x1b[1;36meve\x1b[0;36ml\x1B[37;0m \x1B[1;37m%d\x1B[0;37m, have \x1B[1;31m%ld\x1B[37;0m \x1b[0;31mw\x1b[1;31marpoint\x1b[0;31ms\x1B[37;0m, and \x1b[1;36m%d\x1b[0;37m \x1b[1;36mr\x1b[0;36memort\x1b[1;36ms\x1B[37;0m.", 
STRING_HITS (ch), STRING_MOVES (ch),
ch->pcdata->level, ch->pcdata->warpoints, ch->pcdata->remort_times);

  send_to_char (buf, ch);
   sprintf (buf, "$n reports: <%s and I'm %s.> I am also \x1b[0;36ml\x1b[1;36meve\x1b[0;36ml\x1B[37;0m \x1B[1;37m%d\x1B[0;37m, have \x1B[1;31m%ld\x1B[37;0m \x1b[0;31mw\x1b[1;31marpoint\x1b[0;31ms\x1B[37;0m, and \x1b[1;36m%d\x1b[0;37m \x1b[1;36mr\x1b[0;36memort\x1b[1;36ms\x1B[37;0m.", 
STRING_HITS (ch), STRING_MOVES
(ch), ch->pcdata->level, ch->pcdata->warpoints, ch->pcdata->remort_times);
  act (buf, ch, NULL, NULL, TO_ROOM); 
  return;
}


void
do_password (CHAR_DATA * ch, char *argy)
{
  char arg1[SML_LENGTH];
  char arg2[SML_LENGTH];
  char *pArg;
  char *pwdnew;
  char *p;
  char cEnd;
  DEFINE_COMMAND ("password", do_password, POSITION_DEAD, 0, LOG_NEVER, "This command allows you to change your password.")

    if (IS_MOB (ch))
    return;
/*
   * Can't use one_argy here because it smashes case.
   * So we just steal all its code. Bleagh.
 */
  pArg = arg1;
  while (isspace (*argy))
    argy++;
  cEnd = ' ';
  if (*argy == '\'' || *argy == '"')
    cEnd = *argy++;
  while (*argy != '\0')
    {
      if (*argy == cEnd)
        {
          argy++;
          break;
        }
      *pArg++ = *argy++;
    }

  *pArg = '\0';
  pArg = arg2;
  while (isspace (*argy))
    argy++;
  cEnd = ' ';
  if (*argy == '\'' || *argy == '"')
    cEnd = *argy++;
  while (*argy != '\0')
    {
      if (*argy == cEnd)
        {
          argy++;
          break;
        }
      *pArg++ = *argy++;
    }

  *pArg = '\0';
  if (arg1[0] == '\0' || arg2[0] == '\0')
    {
      send_to_char ("Syntax: password <old> <new>.\n\r", ch);
      return;
    }

  if (strcmp  (arg1, ch->pcdata->pwd) &&
      strcmp(arg1, ch->pcdata->rpwd))
    {
      WAIT_STATE (ch, 40);
      send_to_char ("Wrong password. Wait 10 seconds.\n\r", ch);
      return;
    }

  if (strlen (arg2) < 5)
    {
      send_to_char (
             "New password must be at least five characters long.\n\r", ch);
      return;
    }

/*
   * No tilde allowed because of player file format.
 */
  pwdnew = (arg2, ch->pcdata->name);
  for (p = pwdnew; *p != '\0'; p++)
    {
      if (*p == '~')
        {
          send_to_char (
                         "New password not acceptable, try again.\n\r", ch);
          return;
        }
    }

  if (ch->pcdata->pwd != NULL)
    {
      free_string (ch->pcdata->pwd);
    }
  ch->pcdata->pwd = str_dup (pwdnew);
  if (ch->pcdata->rpwd)
    free_string(ch->pcdata->rpwd);
  ch->pcdata->rpwd = str_dup(arg2);
  save_char_obj (ch);
  send_to_char ("Ok.\n\r", ch);
  return;
}

/*void do_channels( CHAR_DATA *ch, char *argy )
   {
   char arg[SML_LENGTH];
   DEFINE_COMMAND("channels",do_channels,POSITION_DEAD,0,LOG_NORMAL,"This command allows you to toggle channels on/off using channel -name or channel +name.")

   one_argy( argy, arg );
   if ( arg[0] == '\0' )
   {
   if ( IS_PLAYER(ch) && IS_SET(ch->act, PLR_SILENCE) )
   {
   send_to_char( "You are silenced.\n\r", ch );
   return;
   }
   send_to_char( "\x1B[1;33m\n\rChannels:\x1B[37;0m\n\r\n\r", ch );
   #ifdef GLOBAL_GOSSIP

   send_to_char( !IS_SET(ch->pcdata->deaf, CHANNEL_AUCTION)
   ? "\x1B[1;37m*\x1B[0mAUCTION - You are listening to the auction channel.\n\r"
   : " auction - You are ignoring the auction channel.\n\r",
   ch );
   #endif

   send_to_char( !IS_SET(ch->pcdata->deaf, CHANNEL_NOTIFY)
   ? "\x1B[1;37m*\x1B[0mNOTIFY - You are listening to the game event notification channel.\n\r"
   : " notify - You are ignoring the game event notification channel.\n\r",
   ch );
   send_to_char( !IS_SET(ch->pcdata->deaf, CHANNEL_CHAT)
   ? "\x1B[1;37m*\x1B[0mCHAT - You are listening to the chat channel.\n\r"
   : " chat - You are ignoring the chat channel.\n\r",
   ch );
   if ( LEVEL(ch)>=100 )
   {
   send_to_char( !IS_SET(ch->pcdata->deaf, CHANNEL_IMMTALK)
   ? "\x1B[1;37m*\x1B[0mIMMTALK - You are listening to immortal talk.\n\r"
   : " immtalk - You are ignoring immortal talk.\n\r",
   ch );
   }
   if (clan_number(ch)>0)
   {
   send_to_char( !IS_SET(ch->pcdata->deaf, CHANNEL_CLANTALK)
   ? "\x1B[1;37m*\x1B[0mCLANTALK - You are listening to clan telepathy.\n\r"
   : " clantalk - You are ignoring clan telepathy.\n\r",
   ch );
   }
   send_to_char( !IS_SET(ch->pcdata->deaf, CHANNEL_YELL)
   ? "\x1B[1;37m*\x1B[0mYELL - You are listening to yells/gossips.\n\r"
   : " yell - You are ignoring yells.\n\r",
   ch );
   }

   else
   {
   bool fClear;
   int bit;
   if ( arg[0] == '+' ) fClear = TRUE;
   else if ( arg[0] == '-' ) fClear = FALSE;
   else
   {
   send_to_char( "Channels -channel or +channel?\n\r", ch );
   return;
   }
   if ( !str_cmp( arg+1, "chat" ) ) bit = CHANNEL_CHAT;
   else if ( !str_cmp( arg+1, "immtalk" ) ) bit = CHANNEL_IMMTALK;
   else if ( !str_cmp( arg+1, "auction" ) ) bit = CHANNEL_AUCTION;
   else if ( !str_cmp( arg+1, "clantalk" ) ) bit = CHANNEL_CLANTALK;
   else if ( !str_cmp( arg+1, "notify" ) ) bit = CHANNEL_NOTIFY;
   else if ( !str_cmp( arg+1, "yell" ) ) bit = CHANNEL_YELL;
   else
   {
   send_to_char( "Set or clear which channel?\n\r", ch );
   return;
   }
   if ( fClear )
   {
   char buf[50];
   REMOVE_BIT (ch->pcdata->deaf, bit);
   sprintf(buf,"You now are aware of %ss.\n\r",arg+1);
   send_to_char(buf,ch);
   }
   else
   {
   char buf[50];
   SET_BIT (ch->pcdata->deaf, bit);
   sprintf(buf,"You now ignore %ss.\n\r",arg+1);
   send_to_char(buf,ch);
   }
   }
   return;
   }
 */

void
do_actual_search (CHAR_DATA * ch, char *argy)
{
  char buf[STD_LENGTH];
  SINGLE_OBJECT *obj;
  CHAR_DATA *oo;
  char arg1[STD_LENGTH];
  short door;
  int found = 0;
  door = -1;
  if (IS_MOB (ch))
    return;
  NEW_POSITION(ch, POSITION_STANDING);
  sprintf (buf, "You search the room, but find nothing.\n\r");
  if (argy[0] != '\0')
    strcpy (arg1, argy);
  if (!str_cmp (arg1, "n") || !str_cmp (arg1, "north"))
    door = 0;
  else if (!str_cmp (arg1, "e") || !str_cmp (arg1, "east"))
    door = 1;
  else if (!str_cmp (arg1, "s") || !str_cmp (arg1, "south"))
    door = 2;
  else if (!str_cmp (arg1, "w") || !str_cmp (arg1, "west"))
    door = 3;
  else if (!str_cmp (arg1, "u") || !str_cmp (arg1, "up"))
    door = 4;
  else if (!str_cmp (arg1, "d") || !str_cmp (arg1, "down"))
    door = 5;
  if (door >= 0)
    {
/* 'look direction' */
      if ((ch->in_room->exit[door]) == NULL)
        {
          send_to_char ("You see nothing special in that direction.\n\r", ch);
          return;
        }
      if(ch->in_room->exit[door]->d_info && IS_SET(ch->in_room->exit[door]->d_info->exit_info, EX_HASTRAP) && number_range(1,150) < ch->pcdata->learned[gsn_locate_trap])
	{
	  send_to_char("You have detected a trap!\n\r", ch);
	}
      if (!ch->in_room->exit[door]->d_info || ch->in_room->exit[door]->d_info->keyword == NULL || ch->in_room->exit[door]->d_info->keyword[0] == '\0')
        {
          send_to_char ("You see nothing special in that direction.\n\r", ch);
          return;
        }

      if (ch->in_room->exit[door]->d_info && !IS_SET (ch->in_room->exit[door]->d_info->exit_info, EX_HIDDEN))
        {
          arg1[1] = '\0';
          do_look (ch, arg1);
          return;
        }

	if (ch->in_room->exit[door]->d_info && ch->in_room->exit[door]->d_info->keyword != NULL && ch->in_room->exit[door]->d_info->keyword[0] != '\0')
        if (number_range (0, 120) < UMAX (8, ch->pcdata->learned[gsn_search])
            || dice (1, 500) == 149)
          {
            sprintf (buf, "You discovered a hidden %s!\n\r", ch->in_room->exit[door]->d_info->keyword);
            send_to_char (buf, ch);
            return;
          }
        else
          {
            sprintf (buf, "You see nothing special in that direction.\n\r");
            send_to_char (buf, ch);
            return;
          }
      return;
    }

  for (obj = ch->in_room->more->contents; obj != NULL; obj = obj->next_content)
    {
      if (number_range (0, 120) < UMAX (8, ch->pcdata->learned[gsn_search])
          && IS_SET (obj->extra_flags, ITEM_UNSEEN)
          && IS_SET (obj->pIndexData->wear_flags, ITEM_TAKE))
        {
          sprintf (buf, "You search the room and find %s.\n\r",
                   OOSTR (obj, short_descr));
          send_to_char (buf, ch);
          REMOVE_BIT (obj->extra_flags, ITEM_UNSEEN);
          get_obj (ch, obj, NULL, FALSE);
          skill_gain (ch, gsn_search, FALSE);
          found = 1;
          return;
        }
    }

  for (door = 0; door < 5; door++)
    {
      if (number_range (0, 120) > UMAX (15, ch->pcdata->learned[gsn_search] + 10))
        continue;
      if (ch->in_room->exit[door] != NULL && ch->in_room->exit[door]->d_info && IS_SET (ch->in_room->exit[door]->d_info->exit_info, EX_HIDDEN))
        {
          send_to_char ("You seem to notice something strange; an exit might be hidden here.\n\r", ch);
          send_to_char ("It might be wise to search <direction> to try to find the hidden exit.\n\r", ch);
          return;
        }
    }



  for (oo = ch->in_room->more->people; oo != NULL; oo = oo->next_in_room)
    if (number_range (0, 120) < UMAX (8, ch->pcdata->learned[gsn_search]))
      {
        if (IS_AFFECTED (oo, AFF_HIDE) && number_range (1, 2) == 2)
          {
            act ("$n search the room and finds $N!", ch, NULL, oo, TO_NOTVICT);
            act ("You search the room and find $N!", ch, NULL, oo, TO_CHAR);
            act ("$n searches the room and finds your hiding spot!", ch, NULL, oo, TO_VICT);
            REMOVE_BIT (oo->affected_by, AFF_HIDE);
            return;
          }
      }


  if (!found)
    send_to_char (buf, ch);
  return;
}

void
do_search (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("search", do_search, POSITION_STANDING, 0, LOG_NORMAL, "This command lets you search <direction> or search the room for players and/or hidden obejcts.")

    if (IS_MOB (ch))
    return;
  if (RIDING (ch) != NULL)
    {
      send_to_char ("You cannot search while riding!\n\r", ch);
      return;
    }

  if (ch->position == POSITION_SEARCHING)
    {
      send_to_char ("You're already doing a search!\n\r", ch);
      return;
    }

  send_to_char ("You begin to search...\n\r", ch);
  act ("$n looks as if $e is searching for something...", ch, NULL, ch, TO_ROOM);
  ch->pcdata->tickcounts = 9;
  if (strlen (argy) > 30)
    return;
  check_temp (ch);
  if (argy[0] != '\0')
    strcpy (ch->pcdata->temp->temp_string, argy);
  if (argy[0] == '\0')
    ch->pcdata->temp->temp_string[0] = '\0';
  NEW_POSITION(ch, POSITION_SEARCHING);
  return;
}

