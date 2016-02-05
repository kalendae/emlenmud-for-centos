
#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emlen.h"

#ifdef WINDOWS
void bzero(char *spot, int len) {
	while (len>0) {*spot='\0'; len--; spot++;}
return;
}

void dostuff(int sk) {return;}
#endif

void do_repop (CHAR_DATA * ch, char *argy)
{
  AREA_DATA *a;
  char buf[STD_LENGTH];
  DEFINE_COMMAND ("repop", do_repop, POSITION_DEAD, MAX_LEVEL, LOG_NORMAL, "Shows you the state of zone repops.")
    hugebuf_o[0] = '\0';
  for (a = area_first; a != NULL; a = a->next)
    {
      sprintf (buf, "[%3d] %-30s \x1B[1;37m%3d\x1B[37;0m/\x1B[1;37m%-3d\x1B[37;0m\n\r",
	       a->vnum, a->name, a->repop_counter, a->repop_rate);
      strcat (hugebuf_o, buf);
    }
  page_to_char (hugebuf_o, ch);
  return;
}

/* ------------------------------------------------------------------------ */
/* This function adds extra 'commands' to the user when in the area editor,
   aedit.  It is called from comm.c if the user is in the area editor. */
/* ------------------------------------------------------------------------ */

void aedit (CHAR_DATA * ch, char *argy)
{
  AREA_DATA *pArea;
  char arg[STD_LENGTH];
  char arg1[STD_LENGTH];
  char arg2[STD_LENGTH];
  char buf[STD_LENGTH];
  int value;

  pArea = (AREA_DATA *) ch->desc->pEdit;
  strcpy (arg, argy);
  smash_tilde (argy);
  argy = one_argy (argy, arg1);
  strcpy (arg2, argy);

/* pArea is the area being edited, arg is the full argument typed,
   arg1 is the first argument, arg2 is the second argument */

  if (!IS_BUILDER (ch, pArea))
    {
      send_to_char ("You are not a builder in this area!\n\r", ch);
    }
  if (!str_cmp (arg1, "show") || arg_count (arg) == 0)
    {
      sprintf (buf, "%d", pArea->vnum);
      do_astat (ch, buf);
      return;
    }

/* ---------------------------------------------------------------------------- */
/* Exit the area editor, set the user's connected state back to PLAYING */
/* ---------------------------------------------------------------------------- */
  if (!str_cmp (arg1, "done"))
    {
      ch->desc->pEdit = NULL;
      ch->desc->connected = CON_PLAYING;
      return;
    }

  if (!str_cmp (arg1, "?"))
    {
      do_help (ch, "aedit");
      return;
    }

/* ------------------------------------------------------------------------ */
/* If the user isn't a builder, just interpret his/her command as if he/she
   was not in area editor. */
/* ------------------------------------------------------------------------ */
  if (!IS_BUILDER (ch, pArea))
    {
      interpret (ch, arg);
      return;
    }

/* ---------------------------------------------------------------------------- */
/* CAN_QUIT toggles between 3 quit rules for this area: quit allowed if inside,
   quit anywhere, or cannot quit. */
/* ---------------------------------------------------------------------------- */
  if (!str_cmp (arg1, "can_quit"))
    {
      if (pArea->can_quit == 2)
	{
	  send_to_char ("This area is now a normal quit zone.\n\r", ch);
	  pArea->can_quit = 0;
	}
      else if (pArea->can_quit == 0)
	{
	  send_to_char ("Players may now quit anywhere in this area.\n\r", ch);
	  pArea->can_quit = 1;
	}
      else if (pArea->can_quit == 1)
	{
	  send_to_char ("Players may now NOT quit anywhere in this zone.\n\r", ch);
	  pArea->can_quit = 2;
	}
      SET_BIT (pArea->area_flags, AREA_CHANGED);
      return;
    }

/* ---------------------------------------------------------- */
/* Homelands do not have the pkill groupsize limits.......... */
/* ---------------------------------------------------------- */

  if (!str_cmp(arg1, "homeland"))
    {
      if (pArea->homeland)
	{
	  send_to_char("This area is no longer a homeland!\n\r", ch);
	  pArea->homeland = FALSE;
	}
      else
	{
	   send_to_char("This area is now a homeland with no limits on the sizes of pkill groups that can get exp in there!\n\r", ch);
	  pArea->homeland = TRUE;;
	}
      SET_BIT(pArea->area_flags, AREA_CHANGED);
      return;
    }


/* ---------------------------------------------------------- */
/* Rate sets the repop rate, in hours, for this current zone. */
/* ---------------------------------------------------------- */
  if (!str_cmp (arg1, "rate") && is_number (arg2))
    {
      int i;
      i = atoi (arg2);
      pArea->repop_rate = i;
      if (pArea->repop_counter > pArea->repop_rate)
	pArea->repop_counter = pArea->repop_rate;
      send_to_char ("Area repop rate set.\n\r", ch);
      SET_BIT (pArea->area_flags, AREA_CHANGED);
      return;
    }

/* ------------------------------------------------------------------------ */
/* Open the area to mortals.  Must be defined in IS_REAL_GOD for permission */
/* ------------------------------------------------------------------------ */
  if (!str_cmp (arg1, "open"))
    {
      if (!IS_REAL_GOD (ch))
	return;
      send_to_char ("This area is now open to mortals!\n\r", ch);
      pArea->open = TRUE;
      SET_BIT (pArea->area_flags, AREA_CHANGED);
      return;
    }

/* ------------------------------------------------------------------------- */
/* Close the area to mortals.  Must be defined in IS_REAL_GOD for permission */
/* ------------------------------------------------------------------------- */
  if (!str_cmp (arg1, "closed"))
    {
      if (!IS_REAL_GOD (ch))
	return;
      send_to_char ("This area is now closed to mortals!\n\r", ch);
      pArea->open = FALSE;
      SET_BIT (pArea->area_flags, AREA_CHANGED);
      return;
    }

/* -------------------------------------------------------------- */
/* Check for area flags.  If command was an area flag, toggle it. */
/* -------------------------------------------------------------- */
  if ((value = get_area_flags_number (arg1)) != AREA_NONE)
    {
      pArea->area_flags ^= value;
      send_to_char ("Flag toggled.\n\r", ch);
      return;
    }


  /* -------------------------------------------------------------------------- */
/* Create a new area, and enter the area editor using it as the current area. */
/* -------------------------------------------------------------------------- */
  if ((!str_cmp (arg1, "create")) && (LEVEL (ch) == MAX_LEVEL))
    {
      pArea = new_area ();
      top_area++;
      area_sort (pArea);
      ch->desc->pEdit = (void *) pArea;
      send_to_char ("Area Created.\n\r", ch);
      save_area_list ();
      return;
    }

/* ------------------- */
/* Set the area's name */
/* ------------------- */
  if (!str_cmp (arg1, "name"))
    {
      if (arg2[0] == '\0')
	{
	  send_to_char ("Syntax: name <name>\n\r", ch);
	  return;
	}
      free_string (pArea->name);
      pArea->name = str_dup (arg2);
      SET_BIT (pArea->area_flags, AREA_CHANGED);
      send_to_char ("Name set.\n\r", ch);
      return;
    }

/* ----------------------- */
/* Set the area's filename */
/* ----------------------- */
  if (!str_cmp (arg1, "filename") || !str_cmp (arg1, "file"))
    {
      if (argy[0] == '\0')
	{
	  send_to_char ("Syntax: filename <file>\n\r", ch);
	  return;
	}
      free_string (pArea->filename);
      pArea->filename = str_dup (arg2);
      SET_BIT (pArea->area_flags, AREA_CHANGED);
      send_to_char ("Filename set.\n\r", ch);
      save_area_list ();
      return;
    }




/* ------------------------------------------------------------------- */
/* Toggles if this area is a clanhouse zone.  Currently has no effect. */
/* ------------------------------------------------------------------- */
  if (!str_cmp (arg1, "clanzone"))
    {
      if (pArea->clanzone)
	{
	  pArea->open = 0;
	  send_to_char ("This area is no longer a CLAN ZONE.\n\r", ch);
	  pArea->clanzone = FALSE;
	  SET_BIT (pArea->area_flags, AREA_CHANGED);
	  return;
	}
      if (!pArea->clanzone)
	{
	  if (pArea->open == 0)
	    pArea->open = 1;
	  send_to_char ("This area is now a designated CLAN ZONE.\n\r", ch);
	  pArea->clanzone = TRUE;
	  SET_BIT (pArea->area_flags, AREA_CHANGED);
	  return;
	}
    }

/* ---------------------------------------------------------- */
/* Set the repop string, shown to users when the zone resets. */
/* ---------------------------------------------------------- */
  if (!str_cmp (arg1, "repop"))
    {
      string_append (ch, &pArea->repop);
      SET_BIT (pArea->area_flags, AREA_CHANGED);
      return;
    }

/* -------------------------------------------------- */
/* Set the area's security level.  Currently useless. */
/* -------------------------------------------------- */
  if (!str_cmp (arg1, "security"))
    {
      if (!is_number (arg2) || arg2[0] == '\0')
	{
	  send_to_char ("Syntax: security <level>\n\r", ch);
	  return;
	}
      value = atoi (arg2);
      pArea->security = value;
      SET_BIT (pArea->area_flags, AREA_CHANGED);
      send_to_char ("Security set.\n\r", ch);
      return;
    }

/* ---------------------------------------------------------------------------- */
/* Adds or removes builders who have access to this area from the builder list. */
/* ---------------------------------------------------------------------------- */
  if (!str_cmp (arg1, "builder"))
    {
      argy = one_argy (argy, arg2);
      if (arg2[0] == '\0')
	{
	  send_to_char ("Syntax: builder <name toggle>\n\r", ch);
	  return;
	}
      arg2[0] = UPPER (arg2[0]);
      if (strstr (pArea->builders, arg2) != NULL)
	{
	  pArea->builders = string_replace (pArea->builders, arg2, "\0");
	  pArea->builders = string_unpad (pArea->builders);
	  SET_BIT (pArea->area_flags, AREA_CHANGED);
	  send_to_char ("Builder removed.\n\r", ch);
	  return;
	}
      else
	{
	  buf[0] = '\0';
	  if (pArea->builders[0] != '\0')
	    {
	      strcat (buf, pArea->builders);
	      strcat (buf, " ");
	    }
	  strcat (buf, arg2);
	  free_string (pArea->builders);
	  pArea->builders = string_proper (str_dup (buf));
	  SET_BIT (pArea->area_flags, AREA_CHANGED);
	  send_to_char ("Builder added.\n\r", ch);
	  return;
	}
    }

/* ------------------------------------------------------------------- */
/* Sets the vnum range of the area.  Syntax is 'vnum <lower> <upper>'. */
/* ------------------------------------------------------------------- */
  if (!str_cmp (arg1, "vnum"))
    {
      argy = one_argy (argy, arg1);
      strcpy (arg2, argy);
      if (!is_number (arg1) || arg1[0] == '\0'
	  || !is_number (arg2) || arg2[0] == '\0')
	{
	  send_to_char ("Syntax: vnum <lower> <upper>\n\r", ch);
	  return;
	}
      value = atoi (arg1);
      if (get_vnum_area (value) != NULL
	  && get_vnum_area (value) != pArea)
	{
	  send_to_char ("Lower vnum already assigned.\n\r", ch);
	  return;
	}
      pArea->lvnum = value;
      send_to_char ("Lower vnum set.\n\r", ch);
      value = atoi (arg2);
      if (get_vnum_area (value) != NULL
	  && get_vnum_area (value) != pArea)
	{
	  send_to_char ("Upper vnum already assigned.\n\r", ch);
	  return;
	}
      pArea->uvnum = value;
      send_to_char ("Upper vnum set.\n\r", ch);
      SET_BIT (pArea->area_flags, AREA_CHANGED);
      return;
    }

/* -------------------------------------------------------------------- */
/* If none of the other commands were recognized, interpret the command
   as if it was typed while playing the game */
/* -------------------------------------------------------------------- */
  interpret (ch, arg);
  return;
}



/* ------------------ */
/* The social editor. */
/* ------------------ */
void
soc_edit (CHAR_DATA * ch, char *argy)
{
  SOCIAL *s;
  char arg[STD_LENGTH];
  char arg1[STD_LENGTH];
  char arg2[STD_LENGTH];

  s = (SOCIAL *) ch->desc->pEdit;
  strcpy (arg, argy);
  smash_tilde (argy);
  argy = one_argy (argy, arg1);
  strcpy (arg2, argy);

  if (!str_cmp (arg1, "show") || arg_count (arg) == 0)
    {
      show_social (s, ch);
      return;
    }

  if (!str_cmp (arg1, "done"))
    {
      ch->desc->pEdit = NULL;
      ch->desc->connected = CON_PLAYING;
      social_update_hash ();
      save_new_socials ();
      return;
    }

  if (!str_cmp (arg1, "create"))
    {
      social_update_hash ();
      s = new_social ();
      strcpy (s->name, "none");
      ch->desc->pEdit = s;
      send_to_char ("Social created.\n\r", ch);
      return;
    }

/* ---------------------------------------------------------------------- */
/* Social's name.  Corresponds to what people type to trigger this social */
/* ---------------------------------------------------------------------- */
  if (!str_cmp (arg1, "name"))
    {
      if (strlen (arg2) < 20)
	{
	  send_to_char ("Name set.\n\r", ch);
	  strcpy (s->name, arg2);
	}
      return;
    }

/* -------------------------------------------------------------- */
/* What is displayed to the character when no arguments are used. */
/* -------------------------------------------------------------- */
  if (!str_cmp (arg1, "to_char"))
    {
      if (arg2[0] == '\0')
	{
	  free_string (s->char_no_arg);
	  s->char_no_arg = NULL;
	  send_to_char ("Field cleared.\n\r", ch);
	  return;
	}
      free_string (s->char_no_arg);
      s->char_no_arg = str_dup (arg2);
      send_to_char ("Field set.\n\r", ch);
      return;
    }

/* -------------------------------------------------------------- */
/* What is displayed to others in the room when no args are used. */
/* -------------------------------------------------------------- */
  if (!str_cmp (arg1, "to_others"))
    {
      if (arg2[0] == '\0')
	{
	  free_string (s->others_no_arg);
	  s->others_no_arg = NULL;
	  send_to_char ("Field cleared.\n\r", ch);
	  return;
	}
      free_string (s->others_no_arg);
      s->others_no_arg = str_dup (arg2);
      send_to_char ("Field set.\n\r", ch);
      return;
    }

/* ----------------------------------------------------------------- */
/* If another character is specified, this is shown to the character
   performing the social. */
/* ----------------------------------------------------------------- */
  if (!str_cmp (arg1, "char_found"))
    {
      if (arg2[0] == '\0')
	{
	  free_string (s->char_found);
	  s->char_found = NULL;
	  send_to_char ("Field cleared.\n\r", ch);
	  return;
	}
      free_string (s->char_found);
      s->char_found = str_dup (arg2);
      send_to_char ("Field set.\n\r", ch);
      return;
    }

/* --------------------------------------------------------------------- */
/* This is shown to others if a character performs the social on another
   character. */
/* --------------------------------------------------------------------- */
  if (!str_cmp (arg1, "others"))
    {
      if (arg2[0] == '\0')
	{
	  free_string (s->others_found);
	  s->others_found = NULL;
	  send_to_char ("Field cleared.\n\r", ch);
	  return;
	}
      free_string (s->others_found);
      s->others_found = str_dup (arg2);
      send_to_char ("Field set.\n\r", ch);
      return;
    }

/* ------------------------------------------------------------ */
/* This is shown to the 'target' of the social, when specified. */
/* ------------------------------------------------------------ */
  if (!str_cmp (arg1, "to_vict"))
    {
      if (arg2[0] == '\0')
	{
	  free_string (s->vict_found);
	  s->vict_found = NULL;
	  send_to_char ("Field cleared.\n\r", ch);
	  return;
	}
      free_string (s->vict_found);
      s->vict_found = str_dup (arg2);
      send_to_char ("Field set.\n\r", ch);
      return;
    }

/* ----------------------------------------------------------------------- */
/* Displayed when person specifies a target for the social but that target
   is not found. */
/* ----------------------------------------------------------------------- */
  if (!str_cmp (arg1, "char_not"))
    {
      if (arg2[0] == '\0')
	{
	  free_string (s->char_not_found);
	  s->char_not_found = NULL;
	  send_to_char ("Field cleared.\n\r", ch);
	  return;
	}
      free_string (s->char_not_found);
      s->char_not_found = str_dup (arg2);
      send_to_char ("Field set.\n\r", ch);
      return;
    }

/* ---------------------------------------------- */
/* Shown to character if performed on him/herself */
/* ---------------------------------------------- */
  if (!str_cmp (arg1, "char_self"))
    {
      if (arg2[0] == '\0')
	{
	  free_string (s->char_auto);
	  s->char_auto = NULL;
	  send_to_char ("Field cleared.\n\r", ch);
	  return;
	}
      free_string (s->char_auto);
      s->char_auto = str_dup (arg2);
      send_to_char ("Field set.\n\r", ch);
      return;
    }

/* ------------------------------------------------------------------------------ */
/* Shown to others in the room when character performs the social on him/herself. */
/* ------------------------------------------------------------------------------ */
  if (!str_cmp (arg1, "others_self"))
    {
      if (arg2[0] == '\0')
	{
	  free_string (s->others_auto);
	  s->others_auto = NULL;
	  send_to_char ("Field cleared.\n\r", ch);
	  return;
	}
      free_string (s->others_auto);
      s->others_auto = str_dup (arg2);
      send_to_char ("Field set.\n\r", ch);
      return;
    }

/* --------------------------------------------------------------------- */
/* Command not a social editor command, interpret as if playing the game */
/* --------------------------------------------------------------------- */
  interpret (ch, arg);
  return;
}
