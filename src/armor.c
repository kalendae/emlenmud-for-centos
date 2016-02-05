#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emlen.h"
#define ARC  8

int headp;
int bodyp;
int legsp;
int armsp;

bool 
onepass_grab_armor (CHAR_DATA * ch, int attack_type)
{
  SINGLE_OBJECT *o;
  I_ARMOR *r;
  int tprotect = 0;
  
  if (IS_PLAYER (ch))
    {
      tprotect = ch->armor / ARC;
      headp = tprotect;
      bodyp = tprotect;
      legsp = tprotect;
      armsp = tprotect;
    }
  for (o = ch->carrying; o != NULL; o = o->next_content)
    {
      if (o->wear_loc == -1)
	continue;
      if (o->pIndexData->item_type != ITEM_ARMOR)
	continue;
      r = (I_ARMOR *) o->more;
      if (r->max_condition == 0)
	    {
	      bodyp += (r->protects_body);
	      headp += (r->protects_head);
	      armsp += (r->protects_legs);
	      legsp += (r->protects_arms);
	    }
	  else
	    {
	      legsp += (r->protects_legs * r->condition_now) / (r->max_condition);
	      bodyp += (r->protects_body * r->condition_now) / (r->max_condition);
	      headp += (r->protects_head * r->condition_now) / (r->max_condition);
	      armsp += (r->protects_arms * r->condition_now) / (r->max_condition);
	    }
    }
  return (armsp + legsp + headp + bodyp) / 4;
}


int 
find_arm_protection (CHAR_DATA * ch, int attack_type)
{
  SINGLE_OBJECT *o;
  I_ARMOR *r;
  int tprotect = 0;

  if (IS_PLAYER (ch))
    tprotect = ch->armor / ARC;
  for (o = ch->carrying; o != NULL; o = o->next_content)
    {
      if (o->wear_loc == -1)
	continue;
      if (o->pIndexData->item_type != ITEM_ARMOR)
	continue;
      r = (I_ARMOR *) o->more;
      if (r->protects_arms < 1)
	continue;
      if (r->max_condition == 0)
	tprotect += r->protects_arms;
      else
	tprotect += (r->protects_arms) * (r->condition_now) / (r->max_condition);
    }
  return tprotect;
}



int 
find_leg_protection (CHAR_DATA * ch, int attack_type)
{
  SINGLE_OBJECT *o;
  I_ARMOR *r;
  int tprotect = 0;

  if (IS_PLAYER (ch))
    tprotect = ch->armor / ARC;
  for (o = ch->carrying; o != NULL; o = o->next_content)
    {
      if (o->wear_loc == -1)
	continue;
      if (o->pIndexData->item_type != ITEM_ARMOR)
	continue;
      r = (I_ARMOR *) o->more;
      if (r->protects_legs < 1)
	continue;
      if (r->max_condition == 0)
	tprotect += r->protects_legs;
      else
	tprotect += (r->protects_legs) * (r->condition_now) / (r->max_condition);
    }
  return tprotect;  
}



int 
find_body_protection (CHAR_DATA * ch, int attack_type)
{
   SINGLE_OBJECT *o;
  I_ARMOR *r;
  int tprotect = 0;

  if (IS_PLAYER (ch))
    tprotect = ch->armor / ARC;
  for (o = ch->carrying; o != NULL; o = o->next_content)
    {
      if (o->wear_loc == -1)
	continue;
      if (o->pIndexData->item_type != ITEM_ARMOR)
	continue;
      r = (I_ARMOR *) o->more;
      if (r->protects_body < 1)
	continue;
      if (r->max_condition == 0)
	tprotect += r->protects_body;
      else
	tprotect += (r->protects_body) * (r->condition_now) / (r->max_condition);
    }
  return tprotect;  
}



int 
find_head_protection (CHAR_DATA * ch, int attack_type)
{
   SINGLE_OBJECT *o;
  I_ARMOR *r;
  int tprotect = 0;

  if (IS_PLAYER (ch))
    tprotect =ch->armor / ARC;
  for (o = ch->carrying; o != NULL; o = o->next_content)
    {
      if (o->wear_loc == -1)
	continue;
      if (o->pIndexData->item_type != ITEM_ARMOR)
	continue;
      r = (I_ARMOR *) o->more;
      if (r->protects_head < 1)
	continue;
      if (r->max_condition == 0)
	tprotect += r->protects_head;
      else
	tprotect += (r->protects_head) * (r->condition_now) / (r->max_condition);
    }
  return tprotect; 
}


int 
general_ac (CHAR_DATA * ch)
{
  return onepass_grab_armor (ch, -1);
}


int 
new_damage (CHAR_DATA * ch, int damage, int hit_loc, int dt)
{
  int i = 0;
  int prot;
  i = ch->armor/ARC;
  i += ch->pcdata->armor[hit_loc -1];
  if (i < 2) return damage;
  prot = number_range (i/3, i);
  if (prot >= damage)
    return 1;
  return (damage - prot);
}

char *
pval (int i)
{
  static char rv[50];
  int j;
  if (i < 0) i = 0;
  rv[0] = '\0';
  for (j = 0; j < 20; j++)
    {
      if (pow.armor_lev_word[j] && i >= pow.armor_lev_min[j] && i <= pow.armor_lev_max[j])
	{
	  sprintf (rv, pow.armor_lev_word[j]);
	  return rv;
	}
    }
  return rv;
}


void 
do_armor (CHAR_DATA * ch, char *argy)
{
  char tmp[500];
  int tt;
  DEFINE_COMMAND ("armor", do_armor, POSITION_DEAD, 0, LOG_NORMAL, "This command shows your current armor protection levels.")
    if (IS_MOB(ch)) return;
    tt = onepass_grab_armor (ch, -1);
  
  sprintf (tmp, "      \x1b[1;37m O \x1b[1;30m        <\x1b[0;33m-- Head/Neck\x1b[1;37m:\x1b[1;30m  %s\n\r",pval (UMAX(0,headp)));
  send_to_char (tmp, ch);
  sprintf (tmp, "    \x1b[1;30m/--|--\\ \x1b[1;30m     <\x1b[0;33m-- Arms/Hands\x1b[1;37m:\x1b[1;30m %s\n\r",pval (UMAX(0,armsp)));
  send_to_char (tmp, ch);
  sprintf (tmp, "       \x1b[0;37m| \x1b[1;30m        <\x1b[0;33m-- Body/Waist\x1b[1;37m:\x1b[1;30m %s\n\r", pval (UMAX(0,bodyp)));
  send_to_char (tmp, ch);
  sprintf (tmp, "      \x1b[1;30m/ \\ \x1b[1;30m       <\x1b[0;33m-- Legs/Feet\x1b[1;37m:\x1b[1;30m  %s\n\r", pval (UMAX(0,legsp)));
  send_to_char (tmp, ch);
  sprintf (tmp, "    \x1b[1;30m_/   \\_ \x1b[1;30m         ------------------------\n\r");
  send_to_char (tmp, ch);
  sprintf (tmp, "                   \x1b[1;30m      %s\n\r\x1b[0;37m", (pval (tt)));
  send_to_char (tmp, ch);
  return;
}

void 
get_condition (SINGLE_OBJECT * obj)
{
  char buf[500];
  condition_var[0] = '\0';
  if (obj->pIndexData->item_type == ITEM_ARMOR)
    {
      I_ARMOR *armor = (I_ARMOR *) obj->more;
      int percent;
      strcpy (buf, "Unknown");
      if (armor->max_condition == 0 && armor->condition_now == 0)
	strcpy (buf, "Indestructable");
      else
	{
	  percent = PERCENTAGE (armor->condition_now, armor->max_condition);
	  if (percent < 10)
	    strcpy (buf, "Almost unusable");
	  else if (percent < 30)
	    strcpy (buf, "Falling apart");
	  else if (percent < 50)
	    strcpy (buf, "Poor condition");
	  else if (percent < 65)
	    strcpy (buf, "Fair condition");
	  else if (percent < 90)
	    strcpy (buf, "Good condition");
	  else
	    strcpy (buf, "Brand new");
	}
    }
  else if (obj->pIndexData->item_type == ITEM_WEAPON)
    {
      I_WEAPON *weap = (I_WEAPON *) obj->more;
      if (weap->damage_p < 3)
	strcpy (buf, "Broken");
      else if (weap->damage_p < 10)
	strcpy (buf, "Almost broken");
      else if (weap->damage_p < 40)
	strcpy (buf, "Badly damaged");
      else if (weap->damage_p < 70)
	strcpy (buf, "Damaged");
      else if (weap->damage_p < 90)
	strcpy (buf, "Good condition");
      else
	strcpy (buf, "Brand new");
    }
  strcpy (condition_var, buf);
  return;
}



char *
format_obj_to (SINGLE_OBJECT * obj, CHAR_DATA * ch, bool fShort)
{
  static char buf[STD_LENGTH];
  buf[0] = '\0';
  if (fShort)
    {
      char duh[500];
      sprintf (duh, "%s\x1B[37;0m", ans_uppercase (OOSTR (obj, short_descr)));
      strcat (buf, duh);

      if (obj->pIndexData->item_type == ITEM_ARMOR)
	{
	  I_ARMOR *armor = (I_ARMOR *) obj->more;
	  int percent;
	  if (armor->max_condition == 0 && armor->condition_now == 0)
	    strcat (buf, " (indestructable)");
	  else
	    {
	      percent = PERCENTAGE (armor->condition_now, armor->max_condition);
	      if (percent < 10)
		strcat (buf, " (almost unusable)");
	      else if (percent < 30)
		strcat (buf, " (falling apart)");
	      else if (percent < 50)
		strcat (buf, " (poor condition)");
	      else if (percent < 65)
		strcat (buf, " (fair condition)");
	      else if (percent < 90)
		strcat (buf, " (good condition)");
	      else
		strcat (buf, " (brand new)");
	    }
	}
      if (obj->pIndexData->item_type == ITEM_WEAPON)
	{
	  I_WEAPON *weap = (I_WEAPON *) obj->more;
	  if (weap->damage_p < 1)
	    strcat (buf, " (broken)");
	  else if (weap->damage_p < 10)
	    strcat (buf, " (almst broken)");
	  else if (weap->damage_p < 40)
	    strcat (buf, " (badly damged)");
	  else if (weap->damage_p < 64)
	    strcat (buf, " (damaged)");
	  else if (weap->damage_p < 90)
	    strcat (buf, " (good cond)");
	  else
	    strcat (buf, " (brand new)");
	}
      if (obj->pIndexData->item_type == ITEM_GEM)
	{
	  I_GEM *gem = (I_GEM *) obj->more;
	  if (gem->mana_now < 10)
	    strcat (buf, " (very dim)");
	  else if (gem->mana_now < 20)
	    strcat (buf, " (dim)");
	  else if (gem->mana_now < 65)
	    strcat (buf, " (soft glow)");
	  else if (gem->mana_now < 100)
	    strcat (buf, " (bright glow)");
	  else if (gem->mana_now < 140)
	    strcat (buf, " (intense glow)");
	  else
	    strcat (buf, " (radiating power)");
	}
      if (obj->pIndexData->item_type == ITEM_LIGHT && IS_LIT (obj))
	{
	  int percent;
	  I_LIGHT *lgt = (I_LIGHT *) obj->more;
	  percent = (PERCENTAGE (lgt->light_now, lgt->max_light) / 10);
	  switch (percent)
	    {
	    case 0:
	      strcat (buf, " (very dim)");
	      break;
	    case 1:
	      strcat (buf, " (dim)");
	      break;
	    case 2:
	      strcat (buf, " (flickering)");
	      break;
	    default:
	      strcat (buf, "\x1b[1;30m (\x1b[0;31mF\x1b[1;31ml\x1b[1;33ma\x1b[1;37mm\x1b[1;33mi\x1b[1;31mn\x1b[0;31mg\x1b[1;30m)\x1b[1;37m");
	      break;
	    }
	}
      if (obj->pIndexData->item_type == ITEM_DRINK_CON)
	{
	  I_DRINK * dr = (I_DRINK *) obj->more;
	  char buff[200];
	 int percent = PERCENTAGE (dr->liquid_now, dr->max_liquid);
	 if (dr->liquid_type == -1)
	   {
	     sprintf (buff, " (It's %s of lamp oil.)",
		      percent < 10 ? "almost empty" :
		      percent < 40 ? "less than half full" :
		      percent < 60 ? "half full" :
		      percent < 70 ? "more than half full" :
		      "full");
	   }
	 else
	   {
	     sprintf (buff, " (It's %s of a %s liquid.)",
		      percent < 10 ? "almost empty" :
		      percent < 40 ? "less than half full" :
		      percent < 60 ? "half full" :
		      percent < 70 ? "more than half full" :
		      "full",
		      (dr->liquid_type > 17 ? "ERROR" : liq_table[dr->liquid_type].liq_color));
	   }
	 if (percent < 5)
	     sprintf (buff, " (It's empty.)");
	 strcat (buf, buff);
	 
	}
      if (obj->pIndexData->item_type == ITEM_WAND || obj->pIndexData->item_type == ITEM_STAFF)
	{
	  I_WAND *ws = (I_WAND *) obj->more;
	  int percent;
	  percent = ws->current_charges;
	  switch (percent)
	    {
	    case 0:
	      strcat (buf, " (powerless)");
	      break;
	    case 1:
	      strcat (buf, " (dim glow)");
	      break;
	    case 2:
	      strcat (buf, " (glowing)");
	      break;
	    default:
	      strcat (buf, "\x1B[33;1m (bright glow)\x1B[37;0m");
	      break;
	    }
	}
      strcat (buf, "\x1B[0;37m");
    }
  else
    {
      if (obj->pIndexData->item_type == ITEM_LIGHT && IS_LIT (obj))
	{
	  I_LIGHT *light = (I_LIGHT *) obj->more;
	  int percent;
	  percent = (PERCENTAGE(light->light_now, light->max_light) / 10);
	  switch (percent)
	    {
	    case 0:
	      strcat (buf, "(Very dim)-> ");
	      break;
	    case 1:
	      strcat (buf, "(Dim)-> ");
	      break;
	    case 2:
	      strcat (buf, "(Flickering)-> ");
	      break;
	    default:
	      strcat (buf, "\x1B[33;1m(Brightly lit)->\x1B[36;0m ");
	      break;
	    }
	}
      strcat (buf, STRR (obj, description));
    }
  if (IS_OBJ_STAT (obj, ITEM_GLOW) && IS_OBJ_STAT (obj, ITEM_HUM))
      strcat (buf, "\x1B[1;37m..it Glows and Hums with power!\x1B[0;37m");
  else
    {
      if (IS_OBJ_STAT (obj, ITEM_GLOW))
	strcat (buf, "\x1B[1;37m..it Glows with energy!");
      if (IS_OBJ_STAT (obj, ITEM_HUM))
	strcat (buf, "\x1B[1;37m..it Hums with power!");
      if (IS_OBJ_STAT (obj, ITEM_FLAME))
        strcat (buf, "\x1B[1;37m..it burns with a\x1b[1;34m b\x1b[0;34mlu\x1b[1;34me\x1B[0;34m f\x1b[1;34ml\x1b[1;37mam\x1b[0;34me\x1b[0;37m!");
      if (IS_OBJ_STAT (obj, ITEM_HOLY))
        strcat (buf, "\x1B[1;37m..it glows with a \x1B[0;37mh\x1B[1;37mo\x1B[1;30ml\x1B[0;37my \x1B[1;37maura.\x1B[0;37m");
      if (IS_OBJ_STAT (obj, ITEM_UNHOLY))
       strcat (buf, "\x1B[1;37m..it glows with a \x1B[1;30mun\x1B[0;37mh\x1B[1;37mo\x1B[0;37ml\x1B[1;30my\x1B[1;37m aura.\x1B[0;37m");  
     }
  if (IS_OBJ_STAT (obj, ITEM_INVIS))
      strcat (buf, "\x1B[1;30m (Invis)");
  if (IS_OBJ_STAT (obj, ITEM_UNSEEN))
      strcat (buf, "\x1B[1;30m (Unseen)");
  if (IS_AFFECTED (ch, AFF_DETECT_MAGIC)
      && IS_OBJ_STAT (obj, ITEM_MAGIC))
    strcat (buf, "\x1B[1;33m (Magical)");
  if (obj->affected != NULL)
    strcat (buf, "\x1b[1;36m (\x1b[0;36mEnchanted\x1b[1;36m)");
  if(IS_OBJ_STAT(obj, ITEM_GLOW) || IS_OBJ_STAT(obj, ITEM_HUM) ||
       obj->affected != NULL || IS_OBJ_STAT(obj, ITEM_INVIS) ||
       IS_OBJ_STAT(obj, ITEM_UNSEEN) || IS_OBJ_STAT(obj, ITEM_MAGIC) 
     || fShort)
     strcat(buf, "\x1b[0;37m\n\r");
  return buf;
}
