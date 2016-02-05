#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emlen.h"

void 
do_mount (CHAR_DATA * ch, char *argy)
{
  char buf[STD_LENGTH];
  CHAR_DATA *victim;
  DEFINE_COMMAND ("mount", do_mount, POSITION_STANDING, 0, LOG_NORMAL, "Allows you to mount a rideable creature or person.")

    one_argy (argy, buf);
  if (buf[0] == '\0')
    {
      send_to_char ("Mount whom?\n\r", ch);
      return;
    }
  if ((victim = get_char_room (ch, buf)) == NULL)
    {
      send_to_char ("They aren't here.\n\r", ch);
      return;
    }
  if (victim == ch)
    {
      send_to_char ("You can't mount yourself!\n\r", ch);
      return;
    }
  if (RIDING (ch) != NULL)
    {
      send_to_char ("Your riding someone else!\n\r", ch);
      return;
    }
  if (MOUNTED_BY (ch) != NULL)
    {
      send_to_char ("Your being ridden by someone else!\n\r", ch);
      return;
    }
  if (IS_SET (ch->affected_by, AFF_FLYING))
    {
      send_to_char ("You can't ride and fly at the same time!\n\r", ch);
      return;
    }
  if (IS_PLAYER (ch) &&
      !IS_SET (victim->act, ACT_MOUNTABLE) && !IS_IMMORTAL (ch))
    {
      send_to_char ("You can't ride that.\n\r", ch);
      return;
    }
  if (RIDING (victim) != NULL)
    {
      send_to_char ("They are riding someone else!\n\r", ch);
      return;
    }
  if (MOUNTED_BY (victim) != NULL)
    {
      send_to_char ("They are being ridden by someone else!\n\r", ch);
      return;
    }
/*if (victim->level>ch->level) 
   {
   send_to_char("You cannot control mounts that are a higher level than your level.\n\r",ch); 
   return;
   }
 */
  check_fgt (ch);
  check_fgt (victim);
  ch->fgt->riding = victim;
  victim->fgt->mounted_by = ch;
  act ("$n mounts $N, and starts riding $M.", ch, NULL, victim, TO_ROOM);
  act ("You mount $N, and start riding $M.", ch, NULL, victim, TO_CHAR);
  act ("$n mounts you.", ch, NULL, victim, TO_VICT);
  return;
}

void 
do_dismount (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("dismount", do_dismount, POSITION_RESTING, 0, LOG_NORMAL, "Allows you to dismount from a mount.")

    if (RIDING (ch) == NULL)
    {
      send_to_char ("You're not riding anything.\n\r", ch);
      return;
    }
  act ("$n dismounts from $N.", ch, NULL, RIDING (ch), TO_NOTVICT);
  act ("You dismount from $N.", ch, NULL, RIDING (ch), TO_CHAR);
  act ("$n dismounts from you.", ch, NULL, RIDING (ch), TO_VICT);
  check_fgt (ch);
  check_fgt (ch->fgt->riding);
  ch->fgt->riding->fgt->mounted_by = NULL;
  ch->fgt->riding = NULL;
  return;
}

void 
do_buck (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("buck", do_buck, POSITION_RESTING, 0, LOG_NORMAL, "Bucks off your current rider.")

    if (MOUNTED_BY (ch) == NULL)
    {
      send_to_char ("There is no one riding you.\n\r", ch);
      return;
    }
  act ("$n bucks $N!", ch, NULL, MOUNTED_BY (ch), TO_NOTVICT);
  act ("You buck $M!", ch, NULL, MOUNTED_BY (ch), TO_CHAR);
  act ("$n bucks you from $m!", ch, NULL, MOUNTED_BY (ch), TO_VICT);
  check_fgt (ch);
  check_fgt (ch->fgt->mounted_by);
  ch->fgt->mounted_by->fgt->riding = NULL;
  NEW_POSITION(ch->fgt->mounted_by, POSITION_RESTING);
  ch->fgt->mounted_by = NULL;
  return;
}

/* #define TRACK_THROUGH_DOORS */
/* track idea taken from another mud code.  Not used - we use footprints instead */
#define BFS_ERROR -1
#define BFS_ALREADY_THERE -2
#define BFS_NO_PATH -3
struct bfs_queue_struct
{
  ROOM_DATA *room;
  short dir;
  short depth;
  struct bfs_queue_struct *next;
}

 ;
struct room_list_struct
  {
    ROOM_DATA *room;
    struct room_list_struct *next;
  }

 ;
static struct bfs_queue_struct *queue_head = NULL, *queue_tail = NULL;
static struct room_list_struct *list_head = NULL, *list_tail = NULL;
/*
   extern const char * dir_name [];
 */
/* Utility macros */
#define MARK( room ) ( SET_BIT( ( room )->room_flags, R_MARK))
#define UNMARK( room ) ( REMOVE_BIT( ( room )->room_flags, R_MARK))
#define IS_MARKED( room ) ( IS_SET( ( room )->room_flags, R_MARK))
/*
   #define TOROOM( room, y ) ( (ROOM_DATA *) ( room )->exit[ ( y ) ]->to_room )
 */
/*#define IS_CLOSED( room, y ) ( IS_SET( ( room )->exit[( y )]->exit_info, \
   EX_CLOSED))
 */
/*#ifdef TRACK_THROUGH_DOORS
   #define VALID_EDGE( room, y ) \
   ( ( room )->exit[( y )] && \
   ( TOROOM(( room ), ( y ) ) != NOWHERE ) && \
   ( !IS_MARKED( TOROOM( ( room ), ( y ) ) ) ) )
   #else */
/*#define VALID_EDGE( room , y 
   ) \
   ( ( room )->exit[ ( y ) ] && \
   ( TOROOM( ( room ), ( y ) ) != NULL ) && \
   ( !IS_CLOSED( (room), ( y ) ) ) && \
   ( !IS_MARKED( TOROOM( ( room ), ( y ) ) ) ) ) */
/* #endif */
void 
list_enqueue (ROOM_DATA * room)
{
  static struct room_list_struct *curr;
  curr = mem_alloc (sizeof (struct room_list_struct));
  curr->room = room;
  curr->next = NULL;
  if (list_tail != NULL)
    {
      list_tail->next = curr;
      list_tail = curr;
    }
  else
    list_head = list_tail = curr;
  return;
}

void 
bfs_enqueue (ROOM_DATA * room, short dir, short depth)
{
  struct bfs_queue_struct *curr;
  curr = mem_alloc (sizeof (struct bfs_queue_struct));
  curr->room = room;
  curr->dir = dir;
  curr->depth = depth + 1;
  curr->next = NULL;
  if (queue_tail != NULL)
    {
      queue_tail->next = curr;
      queue_tail = curr;
    }
  else
    queue_head = queue_tail = curr;
  list_enqueue (room);
  return;
}

void 
bfs_dequeue (void)
{
  struct bfs_queue_struct *curr;
  curr = queue_head;
  if ((queue_head = queue_head->next) == NULL)
    queue_tail = NULL;
  free_m (curr);
  return;
}

void 
list_dequeue (void)
{
  struct room_list_struct *curr;
  curr = list_head;
  if ((list_head = list_head->next) == NULL)
    list_tail = NULL;
  if (curr->room != NULL)
    {
      UNMARK (curr->room);
      free_m (curr);
    }
  return;
}

void 
bfs_clear_queue (void)
{
  while (queue_head != NULL)
    bfs_dequeue ();
  return;
}

void 
list_clear_queue (void)
{
  while (list_head != NULL)
    list_dequeue ();
}

int 
find_first_step (ROOM_DATA * src, ROOM_DATA * target)
{
  int curr_dir;

  if (src == target)
    return BFS_ALREADY_THERE;
  queue_head = queue_tail = NULL;
  list_head = list_tail = NULL;
  MARK (src);
  list_enqueue (src);
  for (curr_dir = 0; curr_dir < 6; curr_dir++)
    if (VALID_EDGE (src, curr_dir) &&
	!IS_SET (src->room_flags, ROOM_UNTRACKABLE))
      {
	MARK (TOROOM (src, curr_dir));
	bfs_enqueue (TOROOM (src, curr_dir), curr_dir, 0);
      }
  while (queue_head)
    {
      if (queue_head->depth >= (state_var_1 ? 90 : 15))
	{
	  bfs_clear_queue ();
	  list_clear_queue ();
	  return BFS_NO_PATH;
	}
      if (queue_head->room == target)
	{
	  curr_dir = queue_head->dir;
	  bfs_clear_queue ();
	  list_clear_queue ();
	  return curr_dir;
	}
      else
	{
	  for (curr_dir = 0; curr_dir < 6; curr_dir++)
	    {
	      if (VALID_EDGE (queue_head->room, curr_dir))
		{
		  MARK (TOROOM (queue_head->room, curr_dir));
		  bfs_enqueue (TOROOM (queue_head->room, curr_dir),
			       queue_head->dir, queue_head->depth);
		}
	    }
	  bfs_dequeue ();
	}
    }
  list_clear_queue ();
  return BFS_NO_PATH;
}



void 
do_track (CHAR_DATA * ch, char *argy)
{
  char arg[SML_LENGTH];
  CHAR_DATA *vict;
  DEFINE_COMMAND ("fdsjfsdjsdfjmtrack", do_track, POSITION_STANDING, 0, LOG_NORMAL, "Allows you to search for tracks.  See help track for more info.")
    if (ch->pcdata->voting_on == 998 || PHUNTING(ch) != NULL)
      {
	send_to_char("You can only track one thing at once!", ch);
	return;
      }
    
    if (ch->position == POSITION_FIGHTING)
    return;
  if (IS_PLAYER (ch) && ch->pcdata->learned[gsn_track] < 3 && !is_member (ch, GUILD_RANGER))
    {
      send_to_char ("You really have no clue what you are doing. It would be helpful to\n\rlearn the TRACK skill first!\n\r", ch);
      return;
    }
  one_argy (argy, arg);
  if (arg[0] == '\0' && HUNTING (ch) == NULL)
    {
      send_to_char ("Who are you trying to track?\n\r", ch);
      if (IS_PLAYER (ch))
	{
	  ch->pcdata->voting_on = 0;
	}
      return;
    }
  if (arg[0] == '\0' && HUNTING (ch) != NULL)
    {
      do_track (ch, "continue");
      if (IS_PLAYER (ch))
	{
	  ch->pcdata->voting_on = 0;
	}
      return;
    }
  if (strcmp (arg, "continue"))
    {
      if ((vict = get_char_world (ch, arg)) == NULL)
	{
	  if (!str_cmp (arg, NAME (ch)))
	    {
	      send_to_char ("Strange... the tracks seem to end right here.\n\r", ch);
	      if (HUNTING (ch) != NULL)
		{
		  free_string (ch->fgt->hunting);
		  ch->fgt->hunting = NULL;
		}
	      if (IS_PLAYER (ch))
		{
		  ch->pcdata->voting_on = 0;
		}
	      return;
	    }
	  if (HUNTING (ch) != NULL)
	    {
	      send_to_char (
			     "Type TRACK and your name to stop tracking your quarry.\n\r", ch);
	      if (IS_PLAYER (ch))
		{
		  ch->pcdata->voting_on = 0;
		}
	      return;
	    }

	  send_to_char ("You begin to look for tracks...\n\r", ch);
	  if (HUNTING (ch) == NULL)
	    {
	      check_fgt (ch);
	      if (strlen (arg) < 16)
		ch->fgt->hunting = str_dup (arg);
	    }
	  if (IS_PLAYER (ch))
	    {
	      ch->pcdata->tickcounts = 9;
              if (ch->position == POSITION_CASTING) {
                NEW_POSITION(ch, POSITION_STANDING); // prevent casting
                send_to_char("You stop casting your spell.\n\r", ch);
              }
	      ch->pcdata->voting_on = 999;
	    }
	  return;
	}

      if (ch != vict && HUNTING (ch) != NULL)
	{
	  send_to_char (
	  "Type TRACK and your name to stop tracking your quarry.\n\r", ch);
	  if (IS_PLAYER (ch))
	    {
	      ch->pcdata->voting_on = 0;
	    }
	  return;
	}
      if (ch == vict)
	{
	  send_to_char ("Strange... the tracks seem to end right here.\n\r", ch);
	  if (HUNTING (ch) != NULL)
	    {
	      free_string (ch->fgt->hunting);
	      ch->fgt->hunting = NULL;
	    }
	  if (IS_PLAYER (ch))
	    {
	      ch->pcdata->voting_on = 0;
	    }
	  return;
	}
    }
  else
    {
      if (HUNTING (ch) == NULL)
	{
	  send_to_char ("You aren't tracking anyone.\n\r", ch);
	  if (IS_PLAYER (ch))
	    {
	      ch->pcdata->voting_on = 0;
	    }
	  return;
	}
      if ((vict = get_char_world (ch, HUNTING (ch))) == NULL)
	{
/*      send_to_char("You find some footprints of some sort, but aren't good enough to determine\n\r",ch);
   send_to_char("if they are your victim's, how old they are, or where they lead...\n\r",ch);
   if (HUNTING(ch)!=NULL) 
   {
   free_string(ch->fgt->hunting); ch->fgt->hunting=NULL;
   }
   if (IS_PLAYER(ch)) 
   {
   ch->pcdata->voting_on=0;
   }
   return; */
	}
    }
  send_to_char ("You begin to look for tracks...\n\r", ch);
  if (HUNTING (ch) == NULL)
    {
      check_fgt (ch);
      if (strlen (arg) < 16)
	ch->fgt->hunting = str_dup (arg);
    }
  if (IS_PLAYER (ch))
    {
      ch->pcdata->tickcounts = 9;
              if (ch->position == POSITION_CASTING) {
                NEW_POSITION(ch, POSITION_STANDING); // prevent casting
                send_to_char("You stop casting your spell.\n\r", ch);
              }
      ch->pcdata->voting_on = 999;
    }
  return;
}

void 
do_track_t (CHAR_DATA * ch, char *argy)
{
  char buf[STD_LENGTH];
  char arg[SML_LENGTH];
  TRACK_DATA *tr;
  CHAR_DATA *vict;
  int dir;
  if (ch->position == POSITION_FIGHTING)
    return;
  if (IS_PLAYER (ch) && ch->pcdata->learned[gsn_track] < 3 && !is_member (ch, GUILD_RANGER))
    {
      send_to_char ("You really have no clue what you are doing. It would be helpful to\n\rlearn the TRACK skill first!\n\r", ch);
      return;
    }
/* if( !can_use_skill( ch, gsn_track ) )
   {
   send_to_char( "You can't seem to find any tracks here.\n\r", ch );
   return;
   }
 */
  one_argy (argy, arg);
  if (arg[0] == '\0' && HUNTING (ch) == NULL)
    {
      send_to_char ("Whom are you trying to track?\n\r", ch);
      return;
    }
  if (arg[0] == '\0' && HUNTING (ch) != NULL)
    {
      do_track (ch, "continue");
      return;
    }
  if (strcmp (arg, "continue"))
    {
      if ((vict = get_char_world (ch, arg)) == NULL)
	{
	  send_to_char ("You find some footprints of some sort, but aren't good enough to determine\n\r", ch);
	  send_to_char ("if they are your victim's, how old they are, or where they lead...\n\r", ch);
/*          if (HUNTING(ch)!=NULL) 
   {
   free_string(ch->fgt->hunting); ch->fgt->hunting=NULL;
   } */
	  return;
	}
      if (ch == vict && HUNTING (ch) != NULL)
	{
	  send_to_char ("You find some footprints of some sort, but aren't good enough to determine\n\r", ch);
	  send_to_char ("if they are your victim's, how old they are, or where they lead...\n\r", ch);
	  if (HUNTING (ch) != NULL)
	    {
	      free_string (ch->fgt->hunting);
	      ch->fgt->hunting = NULL;
	    }
	  return;
	}
      if (ch != vict && HUNTING (ch) != NULL)
	{
	  send_to_char (
	  "Type TRACK and your name to stop tracking your quarry.\n\r", ch);
	  return;
	}
    }
  else
    {
      if (HUNTING (ch) == NULL)
	{
	  send_to_char ("You aren't tracking anyone.\n\r", ch);
	  return;
	}
      if ((vict = get_char_world (ch, HUNTING (ch))) == NULL)
	{
	  send_to_char ("You find some footprints of some sort, but aren't good enough to determine\n\r", ch);
	  send_to_char ("if they are your victim's, how old they are, or where they lead...\n\r", ch);
	  /*  if (HUNTING(ch)!=NULL) 
	     {
	     free_string(ch->fgt->hunting); ch->fgt->hunting=NULL;
	     } */
	  return;
	}
    }

  if (IS_PLAYER (ch) &&
      (ch->pcdata->learned[gsn_track] < number_range (0, 50) && !is_member (ch, GUILD_RANGER)))
    {
      send_to_char ("You find some footprints of some sort, but aren't good enough to determine\n\r", ch);
      send_to_char ("if they are your victim's, how old they are, or where they lead...\n\r", ch);
/*      if (HUNTING(ch)!=NULL) 
   {
   free_string(ch->fgt->hunting); ch->fgt->hunting=NULL;
   }
   if (IS_PLAYER(ch)) 
   {
   ch->pcdata->voting_on=0;
   } */
      return;
    }

  for (tr = ch->in_room->tracks; tr != NULL; tr = tr->next_track_in_room)
    {
      if ((IS_PLAYER (tr->ch) && !str_cmp (HUNTING (ch), NAME (tr->ch))) ||
      (IS_MOB (tr->ch) && is_name (HUNTING (ch), tr->ch->pIndexData->name)))
	{
	  if (tr->dir_left == 10 || tr->ch->in_room == ch->in_room)
	    {
	      send_to_char ("Strange... the tracks seem to end right here.\n\r", ch);
	      if (HUNTING (ch) != NULL)
		{
		  free_string (ch->fgt->hunting);
		  ch->fgt->hunting = NULL;
		}
	      if (IS_PLAYER (ch))
		{
		  ch->pcdata->voting_on = 0;
		}
	      return;
	    }

	  dir = tr->dir_came;
	  if (dir != 10)
	    {
	      if (dir >= 4)
		sprintf (buf, "The tracks suggest that %s entered the area from %s.\n\r",
			 SEX (tr->ch) == SEX_FEMALE ? "she" :
			 (SEX (tr->ch) == SEX_MALE ? "he" : "it"),
			 dir == 4 ? "above" : "below");
	      else
		sprintf (buf, "The tracks suggest that %s entered the area from the %s.\n\r",
			 SEX (vict) == SEX_FEMALE ? "she" :
			 (SEX (vict) == SEX_MALE ? "he" : "it"),
			 dir_name[dir]);
	      send_to_char (buf, ch);
	    }
	  dir = tr->dir_left;
	  if (dir >= 4)
	    sprintf (buf, "%s tracks appear to leave the area right %s you!\n\r",
		     SEX (tr->ch) == SEX_FEMALE ? "Her" :
		     (SEX (tr->ch) == SEX_MALE ? "His" : "Its"),
		     dir == 4 ? "above" : "below");
	  else
	    sprintf (buf, "%s tracks appear to leave the area heading %s!\n\r",
		     SEX (vict) == SEX_FEMALE ? "Her" :
		     (SEX (vict) == SEX_MALE ? "His" : "Its"),
		     dir_name[dir]);
	  send_to_char (buf, ch);
	  if (HUNTING (ch) == NULL)
	    {
	      check_fgt (ch);
	      if (strlen (arg) < 16)
		ch->fgt->hunting = str_dup (arg);
	    }
	  if (IS_PLAYER (ch) && number_range (1, 3) == 2)
	    skill_gain (ch, gsn_track, TRUE);
	  return;
	}
    }

  send_to_char ("You find some footprints of some sort, but aren't good enough to determine\n\r", ch);
  send_to_char ("if they are your victim's, how old they are, or where they lead...\n\r", ch);



/*
   dir = find_first_step( ch->in_room, vict->in_room );
   switch( dir )
   {
   case BFS_ERROR:
   send_to_char("Hmm.. something seems to be wrong.\n\r", ch);
   if (HUNTING(ch)!=NULL) 
   {
   free_string(ch->fgt->hunting); ch->fgt->hunting=NULL;
   }
   break;
   case BFS_ALREADY_THERE:
   if( HUNTING(ch) == NULL )
   send_to_char("Hmm, the tracks seem to end right here.\n\r", ch);
   else
   send_to_char("You've found your quarry!\n\r", ch );
   if (HUNTING(ch)!=NULL) 
   {
   free_string(ch->fgt->hunting); ch->fgt->hunting=NULL;
   }
   if (IS_PLAYER(ch)) 
   {
   ch->pcdata->voting_on=0;
   }
   break;
   case BFS_NO_PATH:
   sprintf(buf, "You're unable to pick up tracks of %s.\n\r",
   vict->sex == SEX_FEMALE ? "her" : "him" );
   if (IS_PLAYER(ch)) 
   {
   ch->pcdata->voting_on=0;
   }
   send_to_char(buf, ch);
   if (HUNTING(ch)!=NULL) 
   {
   free_string(ch->fgt->hunting); ch->fgt->hunting=NULL;
   }
   break;
   default:
   if( IS_PLAYER(ch) &&
   (ch->pcdata->learned[gsn_track]<number_range(0,90) && !is_member(ch,GUILD_RANGER)))
   dir = number_range( 0, 5 );
   if (IS_SET(vict->in_room->room_flags,ROOM_UNTRACKABLE)) 
   {
   send_to_char("You are unable to tell which direction the tracks lead.\n\r",ch); return;
   }
   if ( dir >= 4 )
   sprintf(buf, "You've picked up %s tracks leading %s you!\n\r",
   vict->sex == SEX_FEMALE ? "her" :
   (vict->sex == SEX_MALE ? "his" : "its" ),
   dir == 4 ? "above" : "below" );
   else
   sprintf(buf, "You've picked up %s tracks leading %s of here!\n\r",
   vict->sex == SEX_FEMALE ? "her" :
   (vict->sex == SEX_MALE ? "his" : "its" ),
   dir_name[dir]);
   send_to_char(buf, ch);
   if( HUNTING(ch) == NULL ) 
   {
   check_fgt(ch);
   if (strlen(arg)<16)
   ch->fgt->hunting=str_dup( arg );
   }
   break;
   }
 */
  return;
}

void 
hunt_victim (CHAR_DATA * ch)
{
  char buf[STD_LENGTH];
  int dir;
  bool found = FALSE;
  PTRACK_DATA *ptrk;
  CHAR_DATA *vict = NULL;
  if (ch->in_room && ch->in_room->ptracks != NULL)
    for (ptrk = ch->in_room->ptracks; (ptrk != NULL && !found); ptrk = ptrk->next_track_in_room)
      {
	if (!str_cmp(PHUNTING(ch), ptrk->online->name))
	  {
	    dir = ptrk->dir_left;
	    if (dir != 10)
	      move_char(ch, dir);
	    return;
	  }
      } 
  if (ch == NULL || HUNTING (ch) == NULL)
    return;
  if (!IS_AWAKE (ch))
    return;
  if (!is_number (HUNTING (ch)))
    {
      /* make sure the char still exists */
      vict = get_char_world (ch, HUNTING (ch));
      if (vict == NULL)
	{
	  
	  return;
	}
      dir = find_first_step (ch->in_room, vict->in_room);
    }
  else
    {
      state_var_1 = TRUE;
      dir = find_first_step (ch->in_room, get_room_index (atoi (HUNTING (ch))));
      state_var_1 = FALSE;
    }
  if (dir < 0)
    {
      return;
    }
  else
    {
      if (!ch->in_room->exit[dir] || (ROOM_DATA *) ! ch->in_room->exit[dir]->to_room)
	return;
      if (!is_number (HUNTING (ch)) && ch->in_room->exit[dir] && (ROOM_DATA *) ch->in_room->exit[dir]->to_room &&
	  ch->in_room->area != ((ROOM_DATA *) ch->in_room->exit[dir]->to_room)->area)
	return;
      if (ch->in_room && ch->in_room->exit[dir] && ch->in_room->exit[dir]->d_info &&
	  IS_SET (ch->in_room->exit[dir]->d_info->exit_info, EX_CLOSED) &&
	  IS_SET (ch->in_room->exit[dir]->d_info->exit_info, EX_ISDOOR) &&
	  !IS_SET (ch->in_room->exit[dir]->d_info->exit_info, EX_LOCKED) &&
	  !IS_SET (ch->in_room->exit[dir]->d_info->exit_info, EX_HIDDEN))
	{
	  do_open (ch, ch->in_room->exit[dir]->d_info->keyword);
	}
      move_char (ch, dir);
      if (is_number (HUNTING (ch)) && ch->in_room->vnum == atoi (HUNTING (ch)))
	{
	  if (HUNTING (ch) != NULL)
	    {
	      free_string (ch->fgt->hunting);
	      ch->fgt->hunting = NULL;
	    }
	  return;
	}
      if (!is_number (HUNTING (ch)) && ch->in_room == vict->in_room)
	{
	  if (MASTER (ch) == vict)
	    {
	      if (ch->fgt && ch->fgt->hunting)
		{
		  free_string (ch->fgt->hunting);
		  ch->fgt->hunting = NULL;
		}
	      return;
	    }
	  if (IS_MOB (ch) && IS_MOB (vict) && !IS_SET (ch->pcdata->act3, ACT3_MUTE) &&
	      !is_safe (ch, vict))
	    {
	      sprintf (buf, "%s! You can run, but you can't hide!", NAME (vict));
	      do_yell (ch, buf);
	    }
	  multi_hit (ch, vict, TYPE_UNDEFINED);
	}
      return;
    }
  return;
}
