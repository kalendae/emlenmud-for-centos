#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emlen.h"

// some damage code from other codebase

char you_hit[STD_LENGTH];		/* You hit ... */
char other_hit[STD_LENGTH];		/* ... hit ... */
char hit_you[STD_LENGTH];		/* ... hit YOU. */

bool nocolor;

void 
general_hit (int dam, CHAR_DATA * ch, CHAR_DATA * victim)
{
  int i;
  int option;

  if( IS_MOB(ch) && ch->pIndexData->mobtype == MOB_DUMMY )
    return;

  if (dam == 0)
    {
      nocolor = TRUE;
      switch (number_range(1,4))
	{
	case 1:
	  strcpy (you_hit, "You circle around $N, but you find no opening.");
	  strcpy (other_hit, "$n circles $N, looking for an opening.");
	  strcpy (hit_you, "$n can't find an opening in your defenses.");
	  break;
	case 4:
	  strcpy (you_hit, "You miss $N with your attack.");
	  strcpy (other_hit, "$n swings at $N and misses.");
	  strcpy (hit_you, "$n attacks you, but misses.");
	  break;
	case 2:
	  strcpy (you_hit, "You swing at $N, and miss wide.");
	  strcpy (other_hit, "$n swings at $N, and misses wide.");
	  strcpy (hit_you, "$n attacks you, but misses wide.");
	  break;
	case 3:
	default:
	  strcpy (you_hit, "You attack $N, but miss your target.");
	  strcpy (other_hit, "$n attacks $N, but misses the target.");
	  strcpy (hit_you, "$n attacks and misses.");
	  break;
	}
      return;
    }
  else if (dam <= 1)
    {
      nocolor = TRUE;
      switch (number_range(1,3))
	{
	case 1:
	  strcpy (you_hit, "Your %s is absorbed completely by $N's armor.");
	  strcpy (other_hit, "$n's %s is completely absorbed by $N's armor.");
	  strcpy (hit_you, "Your armor protects you from $n's %s.");
	  break;
	case 2:
	  strcpy (you_hit, "$N is hardly even bruised by your puny attack!");
	  strcpy (other_hit, "$N is lightly bruised by $n's puny attack.");
	  strcpy (hit_you, "You hardly notice $n's puny %s.");
	  break;
	case 3:
	default:
	 strcpy (you_hit, "You glancing blow barely touches $N!");
	  strcpy (other_hit, "$N is slightly nicked by a glancing blow from $n.");
	  strcpy (hit_you, "$n's %s bounces off of your armor.");
	  break;
	} 
      return;
    }
  for (i = 0; i < 20; i++)
    {
      if (dam >= dmsg.generic_range[i][0] && dam <= dmsg.generic_range[i][1])
	{
	  strcpy (you_hit, dmsg.gen_m[i]);
	  strcpy (other_hit, dmsg.gen_m1[i]);
	  strcpy (hit_you, dmsg.gen_m2[i]);
	  return;
	}

    }
  return;
}


void 
dam_message (CHAR_DATA * ch, CHAR_DATA * victim, int dam, int dt, int p)
{
  char buf[256], buf1[256], buf2[256], buf3[256];
  char *tt;
  int ty;
  char hit_you_far[2000];
  char other_hit_far[2000];
  char subbuf[256], *bufptr;
  char attack[256];
  char bare[256];
  char punct;
  SINGLE_OBJECT *obj = NULL;
  you_hit[0] = '\0';
  bare[0] = '\0';
  attack[0] = '\0';
  other_hit[0] = '\0';
  hit_you[0] = '\0';
  hit_you_far[0] = '\0';
  other_hit_far[0] = '\0';
  nocolor = FALSE;
  if (FIGHTING (ch) != NULL && FIGHTING (ch)->position == POSITION_GROUNDFIGHTING && dam < 3)
    dam += 3;

  if(IS_MOB(ch) && ch->pIndexData->mobtype == MOB_DUMMY) return;
  general_hit (dam, ch, victim);

  punct = (dam <= 20) ? '.' : '!';
  if (IS_MOB (ch))
    {
      if (!ch->pIndexData->attackname || !str_cmp ("(null)", ch->pIndexData->attackname) || !str_cmp ("N/A", ch->pIndexData->attackname) || ch->pIndexData->attackname == NULL || ch->pIndexData->attackname[0] == ' ')
	{
	  if (!str_cmp (mob_type_attack (ch->pIndexData->mobtype), "punch") && FIGHTING (ch) != NULL &&
	      FIGHTING (ch)->position == POSITION_GROUNDFIGHTING)
	    strcpy (bare, "stomp");
	  else
	    strcpy (bare, mob_type_attack (ch->pIndexData->mobtype));
	}
      else
	{
	  if (!str_cmp (ch->pIndexData->attackname, "punch") && FIGHTING (ch) != NULL && FIGHTING (ch)->position == POSITION_GROUNDFIGHTING)
	    strcpy (bare, "stomp");
	  else
	    strcpy (bare, ch->pIndexData->attackname);
	}
    }
  else
    {
      if (FIGHTING (ch) != NULL && FIGHTING (ch)->position == POSITION_GROUNDFIGHTING)
	strcpy (bare, "stomp");
      else
	strcpy (bare, "punch");
      if (is_member(ch, GUILD_MONK))
      {
        int randhit = number_range(1,12);
        if (randhit == 1)
          strcpy (bare, "punch");
        else if (randhit == 2)
          strcpy (bare, "kick");
        else if (randhit == 3)
          strcpy (bare, "leg sweep");
        else if (randhit == 4)
          strcpy (bare, "elbow");
        else if (randhit == 5)
          strcpy (bare, "flipkick");
        else if (randhit == 6)
          strcpy (bare, "headbutt");
        else if (randhit == 7)
          strcpy (bare, "shinkick");
        else if (randhit == 8)
          strcpy (bare, "palm strike");
        else if (randhit == 9)
          strcpy (bare, "knee");
        else if (randhit == 10)
          strcpy (bare, "backfist");
        else if (randhit == 11)
          strcpy (bare, "uppercut");
        else if (randhit == 12)
          strcpy (bare, "quick jab");
      }
    }
  if (dt == gsn_circle)
    {
      if ((obj = get_item_held (ch, ITEM_WEAPON)) == NULL)
	return;
      if (dam > 0)
	{
	  sprintf (buf1, "\x1B[1;37m$n\x1B[0;37m sneaks up on $N... $n thrusts $p\x1B[0;37m into $S back!");
	  if (dam < 20)
	    sprintf (buf2, "You circle and thrust $p\x1B[0m into \x1B[1m$N\x1B[0m's back.");
	  if ((dam > 19) && (dam < 50))
	    sprintf (buf2, "You circle and thrust $p\x1B[0m into \x1B[1m$N\x1B[0m's back, causing cries of agony and pain.");
	  if ((dam > 49) && (dam < 100))
	    sprintf (buf2, "You circle your opponent.... $p\x1B[0m finds its mark in \x1B[1m$N\x1B[0m's back, causing $S body to spasm in pain.");
	  if ((dam > 99) && (dam < 200))
	    sprintf (buf2, "You circle your opponent.... $p\x1B[0m sinks deeply into \x1B[1m$N\x1B[0m's back, and blood runs over your hands.");
	  if ((dam > 199) && (dam < 275))
	    sprintf (buf2, "You circle your opponent.... $p\x1B[0m has struck a pressure point in \x1B[1m$N\x1B[0m's back!\n\rThat's gotta hurt!");
	  if ((dam > 274) && (dam < 350))
	    sprintf (buf2, "You circle around and twist $p\x1B[0m around several times in \x1B[1m$N\x1B[0m's back! That's gotta hurt!");
	  if (dam > 349)
	    sprintf(buf2, "Your $p\x1B[0m penetrates the base of \x1B[1m$N\x1B[0m's neck spraying spinal fluid onto your armor!");
	  sprintf (buf3, "\x1B[1m$n\x1B[0m is behind you before you know it, and slips $p\x1B[0m into your back.\n\r");
	}
    }
  else if (dt == gsn_backstab)
    {
      if ((obj = get_item_held (ch, ITEM_WEAPON)) == NULL)
	return;
      if (dam > 0)
	{ 
	  if (dam <100)  
	     sprintf (buf1, "\x1B[1;37m$n\x1B[0;37m thrusts $p\x1B[0;37m into \x1B[1;37m$N\x1B[0m's back.\n\r");
	  if ((dam > 99) && (dam < 200))
     	     sprintf (buf1, "Blood runs over \x1B[1;37m$n\x1B[0;37m hands as $p\x1B[0;37m\n\ris thrust into \x1B[1;37m$N\x1B[0m's back.\n\r");
	  if ((dam > 199) && (dam < 275))
     	     sprintf (buf1, "\x1B[1;37m$n\x1B[0;37m hits a pressure point as $p\x1B[0;37m\n\ris thrust into \x1B[1;37m$N\x1B[0m's back.\n\r");
	  if ((dam > 274) && (dam < 350))
     	     sprintf (buf1, "\x1B[1;37m$n\x1B[0;37m twists $p\x1B[0;37m around several times\n\rin \x1B[1;37m$N\x1B[0m's back.\n\r");
	  if ((dam > 349) && (dam < 425))
     	     sprintf (buf1, "Spinal fluid sprays as \x1B[1;37m$n\x1B[0;37m thrusts $p\x1B[0;37m\n\rinto \x1B[1;37m$N\x1B[0m's back. That had to HURT!\n\r");
	  if (dam > 424)
     	     sprintf (buf1, "\x1B[1;37m$n\x1B[0;37m uses $p\x1B[0;37m to punch a hole \n\rclean through \x1B[1;37m$N\x1B[0m's neck! Guts spray everywhere!\n\rThat REALLY had to HURT!");
	     
	  if (dam < 20)
	    sprintf (buf2, "You thrust $p\x1B[0m into \x1B[1m$N\x1B[0m's back.");
	  if ((dam > 19) && (dam < 50))
	    sprintf (buf2, "You thrust $p\x1B[0m into \x1B[1m$N\x1B[0m's back, causing cries of agony and pain.");
	  if ((dam > 49) && (dam < 100))
	    sprintf (buf2, "$p\x1B[0m finds its mark in \x1B[1m$N\x1B[0m's back, causing $s body to spasm.");
	  if ((dam > 99) && (dam < 200))
	    sprintf (buf2, "$p\x1B[0m sinks deeply into \x1B[1m$N\x1B[0m's back, and blood runs over your hands.");
	  if ((dam > 199) && (dam < 275))
	    sprintf (buf2, "$p\x1B[0m has struck a pressure point in \x1B[1m$N\x1B[0m's back! \n\rThat's gotta hurt!");
	  if ((dam > 274) && (dam < 350))
	    sprintf (buf2, "You twist $p\x1B[0m around several times in \x1B[1m$N\x1B[0m's back!\n\rThat's gotta hurt!");
	  if ((dam > 349) && (dam < 425))
	    sprintf(buf2, "Your $p\x1B[0m penetrates the base of \x1B[1m$N\x1B[0m's neck spraying spinal fluid onto your armor!\n\rThat REALLY had to HURT!");
	  if (dam > 424)
	    sprintf(buf2, "Your $p\x1B[0m punches a hole clean through \x1B[1m$N\x1B[0m's neck spraying guts all over you!\n\rThat REALLY had to HURT SEVERELY!");

	/* Added Damage Display for backstabee j. ellis (cyric) 2-6-01 */  

	if (dam <100)
	sprintf (buf3, "Before you know what's going on, \x1B[1m$n\x1B[0m\n\ris behind you, and thrusts $p\x1B[0m into your back.\n\r");
	if ((dam > 99) && (dam < 200))
 	   sprintf (buf3, "Blood runs all over \x1B[1m$n\x1B[0m's hands as $p\x1B[0m\n\ris thrust into your back.\n\r");
	if ((dam >199) && (dam < 275))
	   sprintf (buf3, "\x1B[1m$n\x1B[0m strikes a pressure point as $p\x1B[0m\n\ris thrust into your back.\n\r");
	if ((dam >274) && (dam < 350))
	   sprintf (buf3, "\x1B[1m$n\x1B[0m twists $p\x1B[0m around several times\n\rin your back! That HURT!");
	if ((dam >349) && (dam < 425))
	   sprintf (buf3, "\x1B[1m$n\x1B[0m penetrates the base of your neck with $p\x1B[0m\n\rYour spinal fluid sprays everywhere!\n\rThat REALLY HURT!");
	if (dam >424)
	   sprintf (buf3, "\x1B[1m$n\x1B[0m punches a hole clean through your neck with \n\r$p\x1B[0m, That REALLY HURT!");


/*	sprintf (buf3, "Before you know what's going on, \x1B[1m$n\x1B[0m\n\ris behind you, and thrusts $p\x1B[0m\n\r into your back.\n\r");*/
	}
      else
	{
	  sprintf (buf1, "\x1B[1m$n\x1B[0m fails to lunge $p\x1B[0m in \x1B[1m$N\x1B[0m's back.");
	  sprintf (buf2, "You fail to place $p\x1B[0m into \x1B[1m$N\x1B[0m's back.");
	  sprintf (buf3, "\x1B[1m$n\x1B[0m's \x1B[1;30mbackstab\x1B[0m just barely misses you.");
	}
    }

  else if (dt == gsn_fireshield)
  {
    if(dam > 0)
    {
      if( dam > 0 && dam <= 20 )
      {
        sprintf (buf1, "\x1B[1m$n\x1B[0m's shield of fire scorches \x1B[1m$N\x1B[0m!");
        sprintf (buf2, "Your shield of fire \x1B[1mburns\x1B[0m $N!");
        sprintf (buf3, "\x1B[1m$n\x1B[0m's \x1B[1;30mfire shield\x1B[0m chars you!");
      }
      else if( dam > 20 && dam <= 40)
      {
        sprintf (buf1, "\x1B[1m$n\x1B[0m's shield of fire engulfs \x1B[1m$N\x1B[0m!");
        sprintf (buf2, "Your shield of fire \x1B[1minflames\x1B[0m $N!");
        sprintf (buf3, "Ouch!! \x1B[1m$n\x1B[0m's \x1B[1;30mfire shield\x1B[0m engulfs you!");
      }
      else if( dam > 40 && dam < 100)
      {
        sprintf (buf1, "\x1B[1m$n\x1B[0m's shield of fire roasts \x1B[1m$N\x1B[0m!");
        sprintf (buf2, "Your shield of fire \x1B[1mROASTS\x1B[0m $N!");
        sprintf (buf3, "Oof!! \x1B[1m$n\x1B[0m's \x1B[1;30mfire shield\x1B[0m ROASTS you!");
      }
      else if( dam >= 100)
      {
        sprintf (buf1, "\x1B[1m$n\x1B[0m's shield of fire OBLITERATES \x1B[1m$N\x1B[0m!!");
        sprintf (buf2, "Your shield of fire \x1B[1mOBLITERATES\x1B[0m $N!");
        sprintf (buf3, "AARRGGHH!!! \x1B[1m$n\x1B[0m's \x1B[1;30mfire shield\x1B[0m OBLITERATES you!");
      }
      else
      {
        sprintf (buf1, "\x1B[1m$n\x1B[0m's shield of fire OBLITERATES \x1B[1m$N\x1B[0m!! [%d]",dam);
        sprintf (buf2, "Your shield of fire \x1B[1mOBLITERATES\x1B[0m $N! [%d]",dam);
        sprintf (buf3, "AARRGGHH!!! \x1B[1m$n\x1B[0m's \x1B[1;30mfire shield\x1B[0m OBLITERATES you! [%d]",dam);
      }
    }
    else
    {
      sprintf (buf1, "\x1B[1m$n\x1B[0m's fire shield fails to affect $p\x1B[0m in \x1B[1m$N\x1B[0m.");
      sprintf (buf2, "\x1B[1m$N\x1B[0m's attack goes right through your fire shield!.");
      sprintf (buf3, "\x1B[1m$n\x1B[0m's fire shield fails to protect him from your attack.");
    }
  }
  else if (dt == TYPE_HIT || dt < 0)
    {
      if (nocolor)
	{
	  sprintf (buf, "%s", other_hit);
	  sprintf (buf1, buf, bare);
	  sprintf (buf, "%s", you_hit);
	  sprintf (buf2, buf, bare);
	  sprintf (buf, "%s", hit_you);
	  sprintf (buf3, buf, bare);
	  if (RIDING (ch) != NULL)
	    {
	      send_to_char ("ACK! You are having problems punching while riding!\n\r", ch);
	    }
	}
      else
	{
	  sprintf (buf, "\x1B[1;37m%s\x1B[0m", other_hit);
	  sprintf (buf1, buf, bare);
	  sprintf (buf, "\x1B[1;35m%s\x1B[0m", you_hit);
	  sprintf (buf2, buf, bare);
	  sprintf (buf, "\x1B[1;36m%s\x1B[0m", hit_you);
	  sprintf (buf3, buf, bare);
	}
    }
  else
    {
      if (dt >= 0 && dt < SKILL_COUNT)
	{
	  SPELL_DATA *spell;
	  if ((spell = skill_lookup (NULL, dt)) == NULL)
	    return;
	  strcpy (attack, spell->noun_damage);
	}
      else if ((dt - TYPE_HIT) < MAX_ATTACK)
	{
	  strcpy (attack, attack_table[dt - TYPE_HIT].name);
	}
      else
	{
	  bug ("Dam_message: bad dt %d.", dt);
	  dt = TYPE_HIT;
	  strcpy (attack, attack_table[0].name);
	}
      if (nocolor)
	{
	  sprintf (buf, "%s", other_hit);
	  sprintf (buf1, buf, attack);
	  sprintf (buf, "%s", you_hit);
	  sprintf (buf2, buf, attack);
	  sprintf (buf, "%s", hit_you);
	  sprintf (buf3, buf, attack);
	}
      else
	{
	  sprintf (buf, "\x1B[1;37m%s\x1B[0m", other_hit);
	  sprintf (buf1, buf, attack);
	  sprintf (buf, "\x1B[1;35m%s\x1B[0m", you_hit);
	  sprintf (buf2, buf, attack);
	  sprintf (buf, "\x1B[1;36m%s\x1B[0m", hit_you);
	  sprintf (buf3, buf, attack);
	}
    }
  if ((bufptr = strstr (buf1, "hs ")))
    {
      sprintf (subbuf, bufptr + 2);
      *(bufptr + 1) = 'e';
      *(bufptr + 2) = 's';
      *(bufptr + 3) = '\0';
      strcat (buf1, subbuf);
    }
  if ((bufptr = strstr (buf2, "hs ")))
    {
      sprintf (subbuf, bufptr + 2);
      *(bufptr + 1) = 'e';
      *(bufptr + 2) = 's';
      *(bufptr + 3) = '\0';
      strcat (buf2, subbuf);
    }
  if ((bufptr = strstr (buf3, "hs ")))
    {
      sprintf (subbuf, bufptr + 2);
      *(bufptr + 1) = 'e';
      *(bufptr + 2) = 's';
      *(bufptr + 3) = '\0';
      strcat (buf3, subbuf);
    }
  if (dam <= 1)
    {
      act(buf1, ch, obj, victim, TO_NOTVICT_SPAM + 1000);
      act(buf2, ch, obj, victim, TO_CHAR_SPAM + 1000);
      act(buf3, ch, obj, victim, TO_VICT_SPAM + 1000);
    }
  else
    {
      act(buf1, ch, obj, victim, TO_NOTVICT + 1000);
      act(buf2, ch, obj, victim, TO_CHAR + 1000);
      act(buf3, ch, obj, victim, TO_VICT + 1000);
      if (victim->in_room != ch->in_room)
	{
	  ty = 0;
	  for (tt = buf1; *tt != '\0'; tt++)
	    {
	      if (*tt == '$')
		{
		  ty++;
		  tt++;
		  switch(*tt)
		    {
		    case 'N':
		      buf1[ty] = 'n';
		      break;
		    case 'n':
		      buf1[ty] = 'N';
		      break;
		    case 'S':
		      buf1[ty] = 's';
		      break;
		    case 's':
		      buf1[ty] = 'S';
		      break;
		    }
		}
	      ty++;
	    }
	  ty = 0;
	  for (tt = buf3; *tt != '\0'; tt++)
	    {
	      if (*tt == '$')
		{
		  ty++;
		  tt++;
		  switch(*tt)
		    {
		    case 'N':
		      buf3[ty] = 'n';
		      break;
		    case 'n':
		      buf3[ty] = 'N';
		      break;
		    case 'S':
		      buf3[ty] = 's';
		      break;
		    case 's':
		      buf3[ty] = 'S';
		      break;
		    }
		}
	      ty++;
	    }
	   act(buf1, victim, obj, ch, TO_NOTVICT + 1000);
	   act(buf3, victim, obj, ch, TO_CHAR + 1000);
	}
    }
  if (dt == gsn_backstab && dam > 0)
    check_social (victim, "wince", "");
  return;
}
