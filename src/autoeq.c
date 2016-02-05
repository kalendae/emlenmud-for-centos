
#include <ctype.h>
#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "emlen.h"
#ifndef WINDOWS
#include <sys/errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <dirent.h>
#endif

int aan(int i)
{
  if (materials[i].name[0] == 'a' ||
      materials[i].name[0] == 'e' ||
      materials[i].name[0] == 'i' ||
      materials[i].name[0] == 'o' ||
      materials[i].name[0] == 'u')
    return 1;
  return 0;
}
const struct weapon_damage wdt[] =
{
  {1,3},
  {1,4},
  {3,1},
  {2,2},
  {1,5},

  {1,6},
  {4,1},
  {2,3},
  {1,7},
  {3,2},



  {1,8},
  {5,1},
  {2,4},
  {1,9},
  {1,10},

  {6,1},
  {3,3},
  {2,5},
  {1,11},
  {1,12},



  {7,1},
  {2,6},
  {1,13},
  {5,2},
  {3,4},

  {1,14},
  {8,1},
  {4,3},
  {2,7},
  {9,1},




  {6,2},
  {3,5},
  {2,8},
  {10,1},
  {5,3},

  {4,4},
  {2,9},
  {7,2},
  {3,6},
  {2,10},




  {8,2},
  {6,3},
  {4,5},
  {3,7},
  {2,11},

  {5,4},
  {9,2},
  {7,3},
  {10,2},
};


void 
create_standards(CHAR_DATA *ch, char *argy) 
{
  int mat=0;
  int loc=0;
  AREA_DATA *pArea;
  char buf[STD_LENGTH];
  int iHash;
  char strr[STD_LENGTH];
  DEFINE_COMMAND("standards", create_standards, POSITION_STANDING, 0, LOG_ALWAYS, "This creates the standard items for forging 90000-90999 area.")
    
    if (IS_MOB(ch)) return;
  if (LEVEL(ch) != MAX_LEVEL) return;
  if (!ch->in_room) return;
  if (ch->in_room->vnum != 90000)
    {
      send_to_char("You must be in room 90000 to use this command with a 1000 room area set up.\n\r", ch);
      return;
    }
  pArea = ch->in_room->area;
  if (str_cmp(NAME(ch), "Flatline"))
    {
      send_to_char("Only Flatline can do this cuz if you do this more than once it REALLY screws things up.\n\r", ch);
      send_to_char("If you are now running the mud, be sure to DELETE the area 90000-90999 probably called stuff2.are from playarea.lst then recreate the area and then goto room 90000 and then type standards. It might also be a good idea to make this function just return after you have things set up right. As of now this function cannot be accessed without changing something in the source just to be careful.\n\r", ch);
      return;
    }
  
  
  /* This loop makes the forged equipment. */
  
  for (mat = 0; str_cmp(materials[mat].name,"END"); mat++)
    {
      for (loc = 0; str_cmp(forged[loc].name, "end_of_list"); loc++) 
	{
	  OBJ_PROTOTYPE *pObj;
	  pObj = new_obj_index ();
	  /* set up general information These are special items..be careful */
	  
	  pObj->item_type = forged[loc].item_type;
	  pObj->made_of = mat+1;
	  pObj->max_in_room = 0;
	  pObj->timer = 0;
	  pObj->weight = materials[mat].weight/forged[loc].weight_divisor;
	  pObj->cost = materials[mat].cost/forged[loc].cost_divisor;
	  pObj->wear_flags = (ITEM_TAKE + forged[loc].wear_loc);
	  pObj->vnum = 90000 + 40*mat + loc; 
	  pObj->extra_flags = materials[mat].initflags;
	  pObj->affected = NULL;
	  if (materials[mat].bigbonus != 0)
	    pObj->extra_flags |= ITEM_NOSTORE;
	  
	  
	  pObj->area = pArea;
	  if (pObj->vnum > top_vnum_obj)
	    top_vnum_obj = pObj->vnum;
	  iHash = pObj->vnum % HASH_MAX;
	  pObj->next = obj_index_hash[iHash];
	  obj_index_hash[iHash] = pObj;
	  pArea->obj_count++;
	  /* Set up the name */
	  
	  free_string(pObj->name);
	  sprintf(buf, "%s %s %s %s", materials[mat].name, forged[loc].name, forged[loc].name2, (!str_infix(forged[loc].name, "bow") ? "bow" : ""));
	  pObj->name = str_dup (buf);
	  
	  /* Set up the short descr If it is a wpn or armor use name, otherwise
	     use name and name2 as descriptive name. */
	  
	  free_string(pObj->short_descr);
	  strr[0] = '\0';
	  buf[0] = '\0';
	  sprintf(buf, materials[mat].colorname);
	  strcpy(strr, anseval (str_dup(buf), ch));
	  sprintf(buf, "a %s %s %s", forged[loc].name2, strr, forged[loc].name);
	  pObj->short_descr = str_dup(buf);
	  
	  
	  /* Set up the desc you see in the room. */
	  
	  free_string(pObj->description);
	  sprintf(buf, "A %s %s %s %s\n\r", forged[loc].name2, materials[mat].name, forged[loc].name, (number_range(1,3) == 2 ? "has been left here." : (number_range(1,2) == 2 ? "lies here gathering dust." : "is here.")));
	  pObj->description = str_dup(buf);
	  
	  
	  switch(pObj->item_type)
	    {
	    case ITEM_ARMOR:
	      {
		pObj->value[6] = materials[mat].strength;
		pObj->value[7] = materials[mat].strength;
		pObj->value[9] = (mat+4)/4;
		switch(forged[loc].armor_location)
		  {
		  case 0: 
		  case 1:
		  case 2:
		  case 3:
		    {
		      pObj->value[forged[loc].armor_location] = materials[mat].armor + forged[loc].adjust;
		    }
		  break;
		  case 4:
		    {
		      int i;
		      for (i = 0; i < 4; i++)
			{
			  pObj->value[i] = UMAX(1, (materials[mat].armor + forged[loc].adjust)*2/3);
			}
		    }
		  }
		
		
		/* Now set up extra affects */
		
		if (materials[mat].bigbonus > 0 && (forged[loc].location != 2 || forged[loc].num_wear_slots != 2))
		  {
		    AFFECT_DATA *pAf;
		    pAf = new_affect ();
		    pAf->modifier = 1;
		    pAf->type = 1;
		    pAf->next = pObj->affected;
		    pObj->affected = pAf;
		    pAf->duration = 0;
		    pAf->bitvector = 0;
		    switch (forged[loc].location)
		      {
		      case 0:
			pAf->location = (forged[loc].num_wear_slots == 1 ? APPLY_INT : APPLY_WIS);   
			break;
		      case 1:
			pAf->location = APPLY_DAMROLL;
			break;
		      case 2:
			pAf->location = APPLY_DEX;
			break;
		      }
		  }
		if (materials[mat].kickbonus/forged[loc].num_wear_slots > 0 && forged[loc].location == 2) 
		  {
		    AFFECT_DATA *pAf;
		    pAf = new_affect ();
		    pAf->modifier = materials[mat].kickbonus/forged[loc].num_wear_slots;
		    pAf->type = 1;
		    pAf->next = pObj->affected;
		    pObj->affected = pAf;
		    pAf->duration = 0;
		    pAf->bitvector = 0;
		    pAf->location = APPLY_KICK_DAMAGE;
		  }
		if (materials[mat].bigbonus > 0 && (loc == 0 || loc == 6)) 
		  {
		    AFFECT_DATA *pAf;
		    pAf = new_affect ();
		    pAf->modifier = 1;
		    pAf->type = 1;
		    pAf->next = pObj->affected;
		    pObj->affected = pAf;
		    pAf->duration = 0;
		    pAf->bitvector = 0;
		    pAf->location = APPLY_STR;
		  }
		
		if ((materials[mat].bonushit)/forged[loc].num_wear_slots > 0)
		  {
		    AFFECT_DATA *pAf;
		    pAf = new_affect ();
		    pAf->modifier = (materials[mat].bonushit)/forged[loc].num_wear_slots;
		    pAf->type = 1;
		    pAf->next = pObj->affected;
		    pObj->affected = pAf;
		    pAf->duration = 0;
		    pAf->bitvector = 0;
		    pAf->location = APPLY_HITROLL;
		  }
		if ((materials[mat].bonushp)/forged[loc].num_wear_slots> 0)
		  {
		    AFFECT_DATA *pAf;
		    pAf = new_affect ();
		    pAf->modifier = (materials[mat].bonushp)/forged[loc].num_wear_slots;
		    pAf->type = 1;
		    pAf->next = pObj->affected;
		    pObj->affected = pAf;
		    pAf->duration = 0;
		    pAf->bitvector = 0;
		    pAf->location = APPLY_HIT;
		  }
		if ((materials[mat].bonusmove)/forged[loc].num_wear_slots > 0)
		  {
		    AFFECT_DATA *pAf;
		    pAf = new_affect ();
		    pAf->modifier = (materials[mat].bonusmove)/forged[loc].num_wear_slots;
		    pAf->type = 1;
		    pAf->next = pObj->affected;
		    pObj->affected = pAf;
		    pAf->duration = 0;
		    pAf->bitvector = 0;
		    pAf->location = APPLY_MOVE;
		  }
		if ((materials[mat].bonushit)/forged[loc].num_wear_slots > 0 && forged[loc].location == 0)
		  {
		    AFFECT_DATA *pAf;
		    pAf = new_affect ();
		    pAf->modifier = -(materials[mat].bonushit)/forged[loc].num_wear_slots;
		    pAf->type = 1;
		    pAf->next = pObj->affected;
		    pObj->affected = pAf;
		    pAf->duration = 0;
		    pAf->bitvector = 0;
		    pAf->location = APPLY_SAVING_THROW;
		  }
	      }
	    break;
	    case ITEM_AMMO: /* Ammo has no affects on it. */
	      {
		pObj->value[0] = wdt[UMAX(0, 3*mat+forged[loc].adjust)].firstdice;
		pObj->value[1] = wdt[UMAX(0, 3*mat+forged[loc].adjust)].seconddice;
		pObj->value[4] = (loc > 28 ? (loc-29) %3 +1 : 0);  
	      }
	    break;
	    case ITEM_WEAPON:
	      {
		pObj->value[3] = forged[loc].armor_location;
		pObj->extra_flags |= forged[loc].location;
		pObj->value[1] = materials[mat].strength;
		pObj->value[7] = wdt[UMAX(0, 3*mat+forged[loc].adjust)].firstdice;
		pObj->value[8] = wdt[UMAX(0, 3*mat+forged[loc].adjust)].seconddice;
		pObj->value[4] = (loc > 28 ? (loc-29) % 3 +1 : 0); /* Only ranged wpns and ammo get item types... */
		if (mat > 12)
		  pObj->extra_flags |= ITEM_POWER;
		if (mat > 10)
		  pObj->extra_flags |= ITEM_WOUND;
		if (mat == 9 || mat == 10)
		  pObj->extra_flags |= ITEM_ELEMENTAL;
		if (loc <= 28)
		  {
		    if ((mat/7 + 26-loc) != 0)
		      {
			AFFECT_DATA *pAf;
			pAf = new_affect ();
			pAf->modifier = mat/7 + 26-loc;
			pAf->type = 1;
			pAf->next = pObj->affected;
			pObj->affected = pAf;
			pAf->duration = 0;
			pAf->bitvector = 0;
			pAf->location = APPLY_HITROLL;
		      }
		    if ((mat/7+loc-26) > 0)
		      {
			AFFECT_DATA *pAf;
			pAf = new_affect ();
			pAf->modifier = URANGE(1, mat/7+loc-26, 3);
			pAf->type = 1;
			pAf->next = pObj->affected;
			pObj->affected = pAf;
			pAf->duration = 0;
			pAf->bitvector = 0;
			pAf->location = APPLY_DAMROLL;
		      }
		  }
	      }
	    break;
	    case ITEM_TOOL:
	    case ITEM_FURNITURE:
	      {
		pObj->value[0] = forged[loc].adjust;
	      }
	    break;
	    case ITEM_CONTAINER:
	      {
		pObj->value[1] = forged[loc].adjust;
		pObj->value[0] = 100;
	      }
	    break;
	    case ITEM_MINERAL:
	      {
		pObj->value[0] = mat+1;
		pObj->weight = 2;
	      }
	    break;
	    }
	}
    }
  return;
}
	 
	     
	      
	      
	      
