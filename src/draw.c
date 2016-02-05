#include <sys/types.h> 
#include <ctype.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <time.h> 
#include "emlen.h"

/* MAXX holds the maximum size of the 'visited already' array.
   Make this dynamic later, since this is using 100/200k */

#ifdef NEW_WORLD
#define MAXX 100000
#else
#define MAXX 200000
#endif

/* Is there an exit from room 'room' in direction 'dir'? */

#define CAN_GO(room,dir) (room->exit[dir] && room->exit[dir]->to_room)

bool kk = FALSE;		/* Internal use */
bool mb = FALSE;		/* Show all mobiles option */
bool flip = FALSE;		/* Rotate 90 degrees option */
bool noibm = FALSE;		/* No Extended IBM char set option */
bool dbs = FALSE;		/* Double space option */

bool disp_already[MAXX];	/* Displayed this coordinate already? */
int ctr;
char outp[80][25][2];		/* Virtual Screen for character output */
char scrnn[80][25][15];		/* Virtual Screen - holds ANSI color values */
signed char colrs[80][25];	/* Virtual Screen - numeric color values */


char smlmap[MAP_WIDTH][MAP_HEIGHT][2];
char smlcolors[MAP_WIDTH][MAP_HEIGHT];

char max_col[25];
bool now = FALSE;
char cur_col = -6;
char last_col;


/* Clears the screen. */
void
clr (CHAR_DATA * ch)
{
  write_to_buffer (ch->desc, "\x1B[37;0m\x1B[2J\x1B[1;1f", 0);
  return;
}

/* Jump to x,y with writing the positioning code to user's output buffer */
void
gotoxy (CHAR_DATA * ch, int x, int y)
{
  char tt[50];
  sprintf (tt, "\x1B[%d;%df", y, x);
  write_to_buffer (ch->desc, tt, 0);
  return;
}

/* Jump to x,y with direct write to port - for updates that won't
   trigger a new prompt */
void
gotoxy_dir (CHAR_DATA * ch, int x, int y)
{
  char tt[50];
  sprintf (tt, "\x1B[%d;%df", y, x);
  write_to_descriptor (ch->desc->descriptor, tt, 0);
  return;
}

/* Gotoxy returning a string */
char *
ggotoxy_dir (CHAR_DATA * ch, int x, int y)
{
  static char tt[50];
  sprintf (tt, "\x1B[%d;%df", y, x);
  return tt;
}


void
do_drawrooms (CHAR_DATA * ch, char *argy)
{
 
  DEFINE_COMMAND ("codeone", do_drawrooms, POSITION_DEAD, IMM_LEVEL, LOG_NORMAL, "This shows an overhead map of the current area around you.")
    
    reset_data();
  do_rawclear(ch);
  if (!ch->desc)
    return;
  
  drawstuff(ch, argy, 22, 78);
  return;
}

 
 

/* Reset all data used for mapping */
void
reset_data (void)
{
  bzero (max_col, sizeof (max_col));
  bzero (scrnn, sizeof (scrnn));
  bzero (disp_already, sizeof (disp_already));
  bzero (outp, sizeof (outp));
  bzero (colrs, sizeof (colrs));
  cur_col = -5;
  return;
}

void
drawstuff (CHAR_DATA *ch, char* argy, int maxy, int maxx)
{
  char *y = argy;
  char tt[50];
  int i,j,m;
  if (IS_MOB(ch)) return;
  if (IS_SET(ch->pcdata->act2, PLR_NOIBM))
    noibm = TRUE;
  else 
    noibm = FALSE;
  flip = FALSE;
  kk = FALSE;
  mb = FALSE;
  for (; *y != '\0'; y++)
    {
      if (*y == 'N' || *y == 'n')
	noibm = TRUE;
      if (*y == 'D' || *y == 'd')
	kk = TRUE;
      if (*y == 'M' || *y == 'm')
	mb = TRUE;
      if (*y == 'F' || *y == 'f')
	flip = TRUE;
    }
  for (i=0; i<80; i++)
    {
      for (j=0; j<25; j++)
	{
	  outp[i][j][0] = ' ';
	  outp[i][j][1] = '\0';
	  for (m=0; m < 15; m++)
	    scrnn[i][j][m]='\0';
	  colrs[i][j]='\0';
	}
    }
  reset_data();
  ch->pcdata->maxx = maxx;
  ch->pcdata->maxy = maxy;
  go_display (ch, ch->in_room, (maxx/2), (maxy/2), maxy, maxx);
  outp[(maxx/2)][(maxy/2)][0] = 'X';
  outp[(maxx/2)][(maxy/2)][1] = '\0';
  draw_data (ch, maxy);
  write_to_buffer(ch->desc, "\x1B[0;37m", 0);
  gotoxy (ch, 1, maxy+1);
  undisplay(ch, ch->in_room, 150); 
  gotoxy(ch, 1, ch->pcdata->pagelen);
  mb = FALSE;
  kk = FALSE;
  noibm = FALSE;
  flip = FALSE;
  return;
}


void 
undisplay (CHAR_DATA *ch, ROOM_DATA *rid, int distance)
{
  int i;
  ROOM_DATA *troom;
  EXIT_DATA *pexit;
  if (!rid->y) return;
  rid->y = FALSE;
  for (i = 0; i < 4; i++)
    {
      if (rid->mapexit[i] != NULL)
	{
	  undisplay(ch, rid->exit[i]->to_room, distance--);
	}
    }
  return;
}
void
go_display (CHAR_DATA * ch, ROOM_DATA * rid, int x, int y, int maxy, int maxx)
{
  int i, newx = x, newy = y;
  EXIT_DATA *pexit, *backexit;
  ROOM_DATA *troom;
  if (x > maxx || x < 0) /* Boundary x case stop */
    return;
  if (y > maxy || y < 0) /* Boundary y case stop */
    return;
  if (rid->y) return; /* Don't do same room twice. */
  display_room (ch, rid, x, y);
	  

  for (i = 0; i < 4; i ++)
    {
      if ((pexit = rid->exit[i]) != NULL && (troom = pexit->to_room) !=
NULL && troom == rid->mapexit[i])
	{
	  if ((i % 2) == 0) 
	    if (flip) 
	      newx += (i-1); 
	    else
	      newy += (i-1); 
	  else 
	    if (flip)
	      newy += (2-i); 
	    else
	      newx += (2-i);
	  go_display(ch, rid->mapexit[i], newx, newy, maxy, maxx);
	  newy = y;
	  newx = x;
	}
    }
  return;
}	    

/*
Draw the actual data to the user's output buffer.
*/
void
draw_data (CHAR_DATA * ch, int lines)
{
  int i, j;
  bool ibm = !IS_SET(ch->pcdata->act2, PLR_NOIBM);
  gotoxy (ch, 1, 1); /* Home cursor */
  last_col = 0;     /* Last column used for truncating trailing spaces */
  
  for (i = 1; i <= lines; i++)
    {
      for (j = 1; j < ch->pcdata->maxx+1; j++)
	{
	  if (outp[j][i][0] == '\0' || outp[j][i][0] == ' ')  /* Null -> Space */
	    {
	      write_to_buffer (ch->desc, " ", 0);
	    }
	  else
	    {  /* Optimize colors so we don't send repeat color
                  sequences that are unnecessary */
	      if (colrs[j][i] == 0 || colrs[j][i] != last_col)
		{
		  write_to_buffer (ch->desc, scrnn[j][i], 0);
		}
	      last_col = colrs[j][i]; /* Set the last color drawn */
	      write_to_buffer (ch->desc, outp[j][i], 0); /* Draw the char */
	    }
	}
      if (max_col[i] < 20) send_to_char("                      ", ch);
      send_to_char ("\n\r", ch);  /* Next line */
    }
  send_to_char ("\x1B[0;37m", ch); /* Back to gray */
  return;
}


void
do_drawtop (CHAR_DATA * ch, char *argy)
{
  char tt[50];
  DEFINE_COMMAND ("drawtop", do_drawtop, POSITION_DEAD, IMM_LEVEL, LOG_NORMAL, "This command shows an updated map in the upper half of the screen.")
    
    if (!ch->desc)
      return;
 
/* Vt100 windowing code - set up window */


  drawstuff(ch, argy, 10, 78);
  write_to_buffer (ch->desc, "\x1B[0;37m", 0);
  send_to_char ("\x1B[10;1f\x1B[1;37m-------------------------------------------------------------------------------\x1B[0;37m", ch);
  sprintf (tt, "\x1B[%d;1f", ch->pcdata->pagelen);
  send_to_char (tt, ch);
  
  return;
}


void
display_room (CHAR_DATA * ch, ROOM_DATA * rid, int x, int y)
{
  bool dir[4];
  char out = ' ';
  int i;
  bool found = TRUE;
  char outpt[20];
  char color[25];
  rid->y = TRUE;
  for (i = 0; i < 4; i ++)
    {
      dir[i] = (rid->exit[i] != NULL);
    }
  if (flip)
    {
      bool temp;
      for (i = 0; i < 4; i ++)
	{
	  temp = dir[i];
	  dir[i] = dir[(i+1) % 4];
	  dir[(i+1) % 4] = temp;
	}
    }
  if (kk && DESCRIPTED (rid) && LEVEL(ch) > IMM_LEVEL)
    out = 'D';
  else if (mb && (rid->more && rid->more->people != NULL))
    {
      out = (rid->more->pcs > 0 ? 'P' : 'M');
    }
  else 
    {
      if (rid->a != '\0')
	{
	  out = rid->a;
	}
      else if (noibm)
	{
	  switch (rid->sector_type)
	    {
	    default: 
	      found = FALSE;
	      break;
	    case SECT_HILLS:
	    case SECT_MOUNTAIN:
	    case SECT_ROCKY:
	    case SECT_CANYON:
	      {
		out = '^';
		break;
	      }
	    case SECT_FOREST:
	    case SECT_WOODS:
	    case SECT_BRUSH:
	      {
		out = '*';
		break;
	      }
	    case SECT_DESERT:
	    case SECT_WASTELAND:
	      {
		out = ',';
		break;
	      }
	    case SECT_WATER_SWIM:
	    case SECT_WATER_NOSWIM:
	    case SECT_UNDERWATER:
	      {
		out = '#';
		break;
	      }
	    case SECT_SWAMP:
	    case SECT_MARSH:
	    case SECT_HOLLOW:
	    case SECT_ICE:
	    case SECT_SNOW:
	    case SECT_ARCTIC:
	      {
		out = '"';
		break;
	      }
	    case SECT_FIELD:
	    case SECT_GRASSLANDS:
	    case SECT_PLAINS:
	    case SECT_SAVANNAH:
	    case SECT_STEPPE:
	      {
		out = '.';
		break;
	      }
            case SECT_ROAD:
            case SECT_TRAIL:
            case SECT_PATH:
              { 
                out = '+';
                break;
              }
	    }
	}
      if (!found || !noibm)
	{
	  if (dir[DIR_EAST])
	    {
	      if (dir[DIR_WEST])
		{
		  if (dir[DIR_NORTH])
		    {
		      if (dir[DIR_SOUTH]) /* N E W S */
			{
			      out = (noibm ? '+' : '\305');
			}
		      else /* N E W */
			{
			  out = (noibm ? '+' : '\301');
			}
		    }
		  else if (dir[DIR_SOUTH]) /* Not North */
		    {
		      out = (noibm ? '+' : '\302'); /* E W S */
		    }
		  else  /* E W */
		    {
		      out = (noibm ? '-' : '\304');
		    }
		}
	      else /* East and Not West */
		{
		  if (dir[DIR_NORTH])
		    {
		      if(dir[DIR_SOUTH])
			{
			  out = (noibm ? '}' : '\303'); /* E N S */
			}
		      else
			{
			  out = (noibm ? '\\' : '\330'); /* E N */
			}
		    }
		  else if (dir[DIR_SOUTH]) /* E S */
		    {
		      out = (noibm ? '/' : '\332');
		    }
		}
	    }
	  else if (dir[DIR_WEST]) /* West but not east */
	    {
	      if (dir[DIR_NORTH])
		{
		  if (dir[DIR_SOUTH])
		    {
		      out = (noibm ? '{' : '\264'); /* N S W */
		    }
		  else
		    {
		      out = (noibm ? '/' : '\331'); /* W N */
		    }
		}
	      else if (dir[DIR_SOUTH]) /* S W */
		{
		  out = (noibm ? '\\' : '\277');
		}
	    }
	  else if (dir[DIR_NORTH] ||  dir[DIR_SOUTH])
	    {
	      out = (noibm ? '|' : '\263'); /* N || S */
	    }
	}
    }
  if (rid->c != '\0')
    {
      int iii;
      for (iii = 1; iii <= 15; iii++)
	{
	  if (rid->c == color_table[iii].number)
	    {
	      if (iii < 8)
		{
		  cur_col = 0;
		  sprintf (color, "\x1B[0m%s", color_table[iii].code);
		}
	      else
		{
		  cur_col = 0;
		  sprintf (color, color_table[iii].code);
		}
	    }
	}
    }
  else 
    {
      if ((rid->sector_type >= 0) && (rid->sector_type <= (SECT_MAX-1)))
	{
	  cur_col = (rid->sector_type)+1;
	  sprintf(color, sectors[rid->sector_type].color);
	}
      else
	{
	  cur_col = SECT_MAX;
	  sprintf (color, "\x1B[0;37m");
	}
    }
  sprintf (outpt, "%s%c", color, out);
  if (x > max_col[y])
    max_col[y] = x;
  outp[x][y][0] = out;
  outp[x][y][1] = '\0';
  strcpy (scrnn[x][y], color);
  colrs[x][y] = cur_col;
  return;
}


/* This is the recursive function that builds the map in memory and displays it.
   -> came_from stores the direction last came from, so we don't need to check
      that direction again.
   -> rid is the current room data being examined.
   -> x and y are the current screen coordinates that we are at.
   -> maxx and maxy are the maximum screen coordinates we should draw to.
      allows for us to make variable window sizes for mapping
   -> ds is for double space.  For double spacing the X direction to give
      an aspect ration of 40x25, not 80x25.
*/



void
sml_ibm (CHAR_DATA * ch, ROOM_DATA * rid, int x, int y)
{
  bool found = FALSE;
  int i, newx = x, newy = y;
  if (x > MAP_WIDTH || x < 0) /* Boundary x case stop */
    return;
  if (y > MAP_HEIGHT || y < 0) /* Boundary y case stop */
    return;
  if (rid->y) return; /* Don't do same room twice. */
  rid->y = TRUE;
  smlcolors[x][y] = rid->color;
  if (rid->more && kk && rid->more->pcs > 0)
    {
      CHAR_DATA *rch;
      for(rch = rid->more->people; rch != NULL; rch=rch->next_in_room)
        {
          if (IS_PLAYER(rch) && !DIFF_ALIGN(ch, rch))
            {
              found = TRUE;
              smlmap[x][y][0] = 'P';
              smlcolors[x][y] = 12;
              break;
            }
        }
    }
  if (!found && rid->more && mb && rid->more->people != NULL)
    {
      CHAR_DATA *rch;
      for (rch = rid->more->people; rch != NULL; rch = rch->next_in_room)
        {
           if (IS_MOB(rch))
             {
               found = TRUE;
               smlmap[x][y][0] = 'M';
               smlcolors[x][y] = 9;
               break;
             }
        }
    }
 if (!found)
    {
       smlmap[x][y][0] = rid->ibm[0];
    }
     for (i = 0; i < 4; i ++)
	{
	  if (rid->mapexit[i])
	    {
	      if ((i % 2) == 0) 
		newy += (i-1); 
	      else 
		newx += (2-i);
	      sml_ibm(ch, rid->mapexit[i], newx, newy);
	      newy = y;
	      newx = x;
	    }
	}
   
  
  return;
}
void
sml_noibm (CHAR_DATA * ch, ROOM_DATA * rid, int x, int y)
{
  int i, newx = x, newy = y;
  bool found = FALSE;
  if (x > MAP_WIDTH || x < 0) /* Boundary x case stop */
    return;
  if (y > MAP_HEIGHT || y < 0) /* Boundary y case stop */
    return;
  if (rid->y) return; /* Don't do same room twice. */
  rid->y = TRUE;
  smlcolors[x][y] = rid->color;
  if (rid->more && kk && rid->more->pcs > 0)
    {
      CHAR_DATA *rch;
      for(rch = rid->more->people; rch != NULL; rch=rch->next_in_room)
        {
         if(IS_PLAYER(rch) && !DIFF_ALIGN(ch,rch))
           {
         smlmap[x][y][0] = 'P';
         smlcolors[x][y] = 12;
         found = TRUE;
         break;
          }
       }
    }
  if (!found && rid->more && mb && rid->more->people > 0)
    {
       CHAR_DATA *rch;
       for (rch = rid->more->people; rch != NULL; rch =rch->next_in_room)
        {
          if (IS_MOB(rch))
            {
              found = TRUE;
          smlmap[x][y][0] = 'M';
	  smlcolors[x][y] = 9;
              break;
            }
	}
    }
  if (!found)
	{
	  smlmap[x][y][0] = rid->noibm[0];
	}
 
   for (i = 0; i < 4; i ++)
     {
	if (rid->mapexit[i])
	  {
	    if ((i % 2) == 0) 
               newy += (i-1); 
	     else 
	       newx += (2-i);
	      sml_noibm(ch, rid->mapexit[i], newx, newy);
	      newy = y;
	      newx = x;
	 }
     }
  
  return;
}




void
small_map (CHAR_DATA *ch)
{
  int i,j;
  if (IS_MOB(ch)) return;
  if (IS_SET(ch->pcdata->act2, PLR_NOIBM)) noibm = TRUE;
  if (IS_SET(ch->pcdata->act2, PLR_VIEWMOBS)) mb = TRUE;
  if (IS_SET(ch->pcdata->act2, PLR_VIEWPLAYERS)) kk = TRUE;
  for (i=0; i < MAP_WIDTH; i++)
    {
      for (j=0; j < MAP_HEIGHT; j++)
	{
	  smlmap[i][j][0] = ' ';
	  smlmap[i][j][1] = '\0';
	  smlcolors[i][j]= 0;
	}
    }
  reset_small();
  ch->pcdata->x = (MAP_WIDTH/2);
  ch->pcdata->y = (MAP_HEIGHT/2);
  if (noibm)
    sml_noibm(ch, ch->in_room,MAP_WIDTH/2, MAP_HEIGHT/2);
  else
    sml_ibm(ch, ch->in_room, MAP_WIDTH/2, MAP_HEIGHT/2);
  smlmap[MAP_WIDTH/2][MAP_HEIGHT/2][0] = 'X';
  smlmap[MAP_WIDTH/2][MAP_HEIGHT/2][1] = '\0';
  smlcolors[MAP_WIDTH/2][MAP_HEIGHT/2] = 12;  
  draw_small (ch);
  write_to_buffer(ch->desc, "\x1B[0;37m", 0);
  undisplay(ch, ch->in_room, 50); 
  gotoxy(ch, 1, ch->pcdata->pagelen);
  mb = FALSE;
  noibm = FALSE;
  kk = FALSE;
  return;
}

void
draw_small (CHAR_DATA * ch)
{
  int i, j;
  int currcolor = 0;
  char buf[100];
  sprintf(buf, "\x1b[%d;%dr", MAP_HEIGHT, ch->pcdata->pagelen);
  send_to_char(buf, ch);
  gotoxy(ch,1,1);
  for (i = 1; i <= MAP_HEIGHT; i++)
    {
      for (j = 1; j < MAP_WIDTH; j++)
	{
	  if (smlmap[j][i][0] == '\0')  
	    {
	      write_to_buffer (ch->desc, " ", 0);
	    }
	  if (smlcolors[j][i] != 0 && smlcolors[j][i] != currcolor)
	    {
	      write_to_buffer (ch->desc, color_table[smlcolors[j][i]].code, 0);
	      currcolor = smlcolors[j][i]; 
	    }
	  write_to_buffer (ch->desc, smlmap[j][i] , 0); 
	}
      send_to_char ("\n\r", ch);  /* Next line */
    }
  sprintf(buf, "\x1b[%d;1f", ch->pcdata->pagelen);
  send_to_char(buf, ch);
  send_to_char ("\x1B[0;37m", ch); /* Back to gray */
  return;
}

void
reset_small (void)
{
  bzero (smlcolors, sizeof(smlcolors));
  bzero (smlmap, sizeof(smlmap));
  return;
}
