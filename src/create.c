#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emlen.h"


/* Used in forest/woods areas */
void do_chop (CHAR_DATA *ch, char *argy)
{

  SINGLE_OBJECT *axe;
  int material;
  bool found = TRUE;


 DEFINE_COMMAND( "chop", do_chop, POSITION_STANDING, 0, LOG_NORMAL, "This command is used by players to chop down trees to build stuff.")
   
   if (IS_MOB(ch)) return;
 if(!ch->in_room) return;
 
 if((axe = get_item_held(ch,ITEM_WEAPON)) == NULL || axe->pIndexData->made_of == 0 || !is_name("axe", axe->pIndexData->name))
   {
      send_to_char("You must be holding a forged axe to chop down trees!\n\r", ch);
      return;
    }
 material = axe->pIndexData->made_of;
 if (ch->in_room->sector_type != SECT_FOREST &&
     ch->in_room->sector_type != SECT_WOODS &&
     ch->in_room->sector_type != SECT_BRUSH)
   {
     
     found = FALSE;
   }
 if (!check_skill(ch, gsn_chop) || number_range(1,300) > ch->pcdata->learned[gsn_chop])
   {
     
     found = FALSE;
   }
 if (ch->pcdata->online_spot->chopped > 200 && LEVEL(ch) < 100)
   {
     
     
     found = FALSE;
     
   }
 if (ch->in_room->minerals <= 0)
   {
     found = FALSE;
   }
 if (!found)
   {
     send_to_char("You look around for something to chop down, but find nothing worth cutting.", ch);
     WAIT_STATE(ch, 3*PULSE_VIOLENCE);
     return;
   }
 else
   {
     long vnum;
     SINGLE_OBJECT *log;
     vnum = 551 + number_range(0,UMIN(9, material+1));
     send_to_char("CHOP!\n\r  CHOP!\n\r      CHOP!\n\r\n\n\n   TIIIMMMMMBBBBEEERRRR!!!!.\n\r", ch);
    
     if ((log = create_object(get_obj_index(vnum),1)) == NULL)
       {
	 char buf[100];
	 sprintf(buf, "There seems to be an error with vnum %d.\n\r", vnum);
	 send_to_char(buf, ch);
	 send_to_char("Report this to an admin please.\n\r", ch);
	 return;
       }
     else
       {
	 log = create_object(get_obj_index(vnum),1);
	 obj_to(log, ch->in_room);
	 ch->in_room->minerals--;
	 ch->pcdata->online_spot->chopped++;
	 WAIT_STATE(ch, 3*PULSE_VIOLENCE);
       }
   }
 return;
}

/* Need hammer and wood to use */
void do_build (CHAR_DATA *ch, char *argy)
{
 DEFINE_COMMAND( "build", do_build, POSITION_STANDING, 0, LOG_NORMAL, "This command is used by players to build machines to make stuff.")

   send_to_char("Not ready yet.\n\r", ch);
 return;
}




void do_tan (CHAR_DATA *ch, char *argy)
{
 DEFINE_COMMAND( "tan", do_tan, POSITION_STANDING, 0, LOG_NORMAL, "This command is used by players to make clothing from animal skins.")

send_to_char("Not ready yet.\n\r", ch);
 return;
}


/* When certain kinds of animals leave corpses, they can be skinned...
based on the name of the animal only certain kinds of animals have valuable
skins. */



void do_skin (CHAR_DATA *ch, char *argy)
{
  SINGLE_OBJECT *knife;
  SINGLE_OBJECT *obj;
  SINGLE_OBJECT *skin;
  char arg1[STD_LENGTH];
  int skin_type = 0;
  int material = 0;
  int i = 0;
  bool found = FALSE;
 DEFINE_COMMAND( "skin", do_skin, POSITION_STANDING, 0, LOG_NORMAL, "This command is used by players to make animal pelts out of corpses.")
   if (IS_MOB(ch)) return;
 if(!ch->in_room) return;
 argy = one_argy(argy, arg1);


 
 check_room_more (ch->in_room);
 for (i = 0; str_cmp(skin_types[i].name, "end_of_list") && i < 20; i ++)
   {
     if (!str_cmp(arg1, skin_types[i].name))
       {
	 skin_type = i+1;
	 break;
       }
   }
 
 if (skin_type == 0)
   {
     send_to_char("Exactly what kind of animal skin did you want to make?\n\r", ch);
     return;
   }
 
 for(obj = ch->in_room->more->contents; obj != NULL; obj = obj->next_content)
   {
     if (obj->pIndexData->item_type == ITEM_CORPSE_NPC && is_name(skin_types[i].name, obj->short_descr))
       {
	 skin = obj;
	 found = TRUE;
	 break;
       }
   }
 if (!found)
   {
     send_to_char("There is no corpse of that type here.\n\r", ch);
     return;
   }
 
 if((knife = get_item_held(ch,ITEM_WEAPON)) == NULL || (material = knife->pIndexData->made_of) == 0 || !is_name("dagger", knife->pIndexData->name) )
   {
     send_to_char("You must be holding a forged dagger to skin corpses!\n\r", ch);
     obj_from(obj);
     free_it(obj);
     return;
   }
 if (!check_skill(ch, gsn_skin))
   {
     send_to_char("You fail to skin the corpse correctly, ruining it.\n\r", ch);
     return;
   }
 if (skin_type > (material+1))
   {
     send_to_char("You attempt to skin the hide, but your knife is not powerful enough!\n\rYou ruin the corpse.\n\r", ch);
     obj_from(obj);
     free_it(obj);
     return;
   }
 {
   long vnum;
   SINGLE_OBJECT *skin;
   vnum = 602 + skin_type;
   send_to_char("You carefully remove the hide from the corpse.\n\r", ch);
   obj_from(obj);
   free_it(obj);
   if ((skin = create_object(get_obj_index(vnum),1)) == NULL)
     {
       char buf[100];
       sprintf(buf, "There seems to be an error with vnum %ld.\n\r", vnum);
       send_to_char(buf, ch);
       send_to_char("Report this to an admin please.\n\r", ch);
       return;
     }
   else
     {
       skin = create_object(get_obj_index(vnum),1);
       obj_to(skin, ch);
       WAIT_STATE(ch, 3*PULSE_VIOLENCE);
     }
 }
 return;
}



/* Gather materials from field/sheep/grassland/savannah */
/* You always need 5 pieces of raw materials to weave something. */
void do_weave (CHAR_DATA *ch, char *argy)
{
  SINGLE_OBJECT *loom = NULL;
  SINGLE_OBJECT *obj;
  SINGLE_OBJECT *obj_next;
  int num_have = 0;
  int num_needed = 5; /* This is essentially constant..but can be changed easier up here */
  
  bool found = FALSE;
  char arg1[STD_LENGTH];
  int i;
  int cloth_type = 0;
 DEFINE_COMMAND( "weave", do_weave, POSITION_STANDING, 0, LOG_NORMAL, "This command is used by players to weave cloth out of raw materials.")
   
   if (IS_MOB(ch)) return;
 
 
 argy = one_argy(argy, arg1);
 
 check_room_more (ch->in_room);
 for (i = 0; str_cmp(cloth_types[i].name, "end_of_list") && i < 20; i ++)
   {
     if (!str_cmp(arg1, cloth_types[i].name))
       {
	 cloth_type = i+1;
	 break;
       }
   }
 
 if (cloth_type == 0)
   {
     send_to_char("Exactly what kind of cloth did you want to weave?\n\r", ch);
     return;
   }

 
  for(obj = ch->in_room->more->contents; obj != NULL; obj = obj->next_content)
    {
      if (obj->pIndexData->item_type == ITEM_FURNITURE && ((I_FURNITURE *) obj->more)->type == FURN_LOOM)
	{
	  loom = obj;
	  found = TRUE;
	  break;
	  
	}
    }

  if (!found)
    {
      send_to_char("You need a loom in the room in order to weave!\n\r", ch);
      return;
    }

  if (((I_FURNITURE *)loom->more)->strip_possessions < cloth_type)
    {
      send_to_char("Your loom is not good enough to weave this type of material!\n\r", ch);
      return;
    }
  num_have = 0;
  for(obj = ch->in_room->more->contents; obj != NULL; obj = obj->next_content)
    {
      if (obj->pIndexData->item_type == ITEM_RAW && ((I_RAW *) obj->more)->type == cloth_type)
	{
	  num_have ++;
	}
      if (num_have >= num_needed)
	break;
    }
  
  if (num_have < num_needed)
    {
      send_to_char("You need five pieces of raw materials to weave that kind of cloth!\n\r", ch);
      return;
    }
  
  if (!check_skill(ch, gsn_weave))
    {
      int num_lost = 0;
      send_to_char("You attempt to weave some cloth, but manage to get it all tangled up in the loom!\n\r", ch);
      for(obj = ch->in_room->more->contents; obj != NULL; obj = obj_next)
	{
	  obj_next = obj->next_content;
	  if (obj->pIndexData->item_type == ITEM_RAW && ((I_RAW *) obj->more)->type == cloth_type)
	    {
	      num_lost++;
	      obj_from(obj);
	      free_it(obj);
	      if (num_lost > num_needed/2)
		break;
	    }
	  
	}
    }
  else
    {
      int num_lost = 0;
      int vnum = 619 + 2*cloth_type; /* 621 = cotton, 623 = wool, 625 = silk */
      SINGLE_OBJECT *cloth;


      send_to_char("Ok, you weave some cloth.\n\r", ch);
      for(obj = ch->in_room->more->contents; obj != NULL; obj = obj_next)
	{
	  obj_next = obj->next_content;
	  if (obj->pIndexData->item_type == ITEM_RAW && ((I_RAW *) obj->more)->type == cloth_type)
	    {
	      num_lost++;
	      obj_from(obj);
	      free_it(obj);
	      if (num_lost >= num_needed) 
		break;
	    }
	}
      if ((cloth = create_object(get_obj_index(vnum),1)) == NULL)
	{
	  char buf[500];
	  sprintf(buf, "There seems to be an error with vnum %d.\n\r", vnum);
	  send_to_char(buf, ch);
	  send_to_char("Report this to an admin please.\n\r", ch);
	  return;
	}
      else
	{
	  obj_to(cloth, ch);
	  send_to_char("Enjoy it.\n\r", ch);
	}
    }

  return;
}





/* Make clothing using needles and objects build with build */
void do_stitch (CHAR_DATA *ch, char *argy)
{
 DEFINE_COMMAND( "stitch", do_stitch, POSITION_STANDING, 0, LOG_NORMAL, "This command is used by players to make clothing out of cloth.")

send_to_char("Not ready yet.\n\r", ch);
 return;
}


void do_forge(CHAR_DATA * ch, char * argy)
{

  SINGLE_OBJECT *hammer = NULL;
  SINGLE_OBJECT *forge = NULL;
  SINGLE_OBJECT *anvil = NULL;
  SINGLE_OBJECT *obj;
  SINGLE_OBJECT *obj_next;
  SINGLE_OBJECT *forged_object;
  char arg1[STD_LENGTH];
  char arg2[STD_LENGTH];
  char buf[200];
  int material = 0;
  int i = 0;
  int num_needed = -1;
  int num_chose = -1;
  int num_have = 0;
  long vnum = 0;
  
  DEFINE_COMMAND( "forge", do_forge, POSITION_STANDING, 0, LOG_NORMAL, "This command is used by players to forge equipment from minerals.")

    if (IS_MOB(ch)) return;

  argy = one_argy(argy, arg1);
  argy = one_argy(argy, arg2);
  if (arg1 == "" || arg1[0] == '\0' || arg2 == "" || arg2[0] == '\0')
    {
      send_to_char("Forge what????\n\r", ch);
      return;
    }
  for (i = 0; str_cmp(materials[i].name, "END"); i++)
    {
      if(!str_cmp(materials[i].name, arg1))
	{
	  material = i+1;
	  break;
	}
    }
  if (material == 0)
    {
      send_to_char("Forge an item using which kind of material?\n\r", ch);
      return;
    }
   
  if (!str_cmp(arg2, "chunk"))
    {
      send_to_char("You cannot forge a chunk.\n\r", ch);
      return;
    }
  for (i = 0; str_cmp(forged[i].name, "end_of_list"); i++)
    {
      if (!str_cmp(forged[i].name, arg2))
	{
	  num_chose = i;
	  num_needed = forged[i].chunk_cost;
	  break;
	}
    }
  if (num_chose == -1 || num_needed == -1)
    {
      send_to_char("You try and forge something unknown. Try making some kind of armor or weapon or something.\n\r", ch);
      return;
    }
  if ((hammer = get_item_held(ch, ITEM_TOOL)) == NULL)
    {
      send_to_char("You must be holding a forging hammer to forge!\n\r", ch);
      return;
    }
  if (!IS_SET(((I_TOOL *) hammer->more)->tool_type, TOOL_FORGING))
    {
      send_to_char("You must be holding a forging hammer to forge!\n\r", ch);
      return;
    }
  if ((hammer->pIndexData->made_of < material && num_chose != 17 && num_chose != 18) || hammer->pIndexData->made_of < material-1)
    {
      send_to_char("That hammer is not powerful enough to forge what you want.\n\r", ch);
      return;
    }
  if (!ch->in_room) return;
  check_room_more(ch->in_room);
  for(obj = ch->in_room->more->contents; obj != NULL; obj = obj_next)
    {
      obj_next = obj->next_content;
      if (obj->pIndexData->item_type == ITEM_FURNITURE && (IS_SET(((I_FURNITURE *) obj->more)->type, FURN_ANVIL)))
	{
	  anvil = obj;
	}
      if (obj->pIndexData->item_type == ITEM_FURNITURE && (IS_SET(((I_FURNITURE *) obj->more)->type, FURN_FORGE)))
	{
	  forge = obj;
	}
    }
  if (forge == NULL)
    {
      send_to_char("There is no forge here.\n\r", ch);
      return;
    }
  if (anvil == NULL || (anvil->pIndexData->made_of < material && num_chose != 17 && num_chose != 18) || (anvil->pIndexData->made_of < (material -1)))
    {
      send_to_char("There is no anvil here, or it is not powerful enough.\n\r", ch);
      return;
    }
 
   for(obj = ch->in_room->more->contents; obj != NULL; obj = obj->next_content)
    {
      if (obj->pIndexData->item_type == ITEM_MINERAL && obj->pIndexData->made_of == material)
	num_have ++;
      if (num_have >= num_needed)
	break;
    }
  if (num_have < num_needed)
    {
      sprintf(buf, "You need %d chunks of %s, and you only have %d here.\n\r", num_needed, materials[material-1].name, num_have);
      send_to_char(buf, ch);
      send_to_char("You don't seem to have enough minerals in the room to forge into the item you want.\n\r", ch);
      return;
    }
  send_to_char("Ok, you attempt to forge an item.\n\r", ch);
  if (!check_skill(ch, gsn_forge))
    {
      send_to_char("You FAIL to forge the item! Some of the minerals are ruined.\n\r", ch);
      for(obj = ch->in_room->more->contents; obj != NULL; obj = obj_next)
	{
	  obj_next = obj->next_content;
	  if (obj->pIndexData->item_type == ITEM_MINERAL && obj->pIndexData->made_of == material && number_range(1,4) == 2)
	    {
	      obj_from(obj);
	      free_it(obj);
	    }
	}
    }
  else
    {
      sprintf(buf, "Ok, you forge your %s %s.\n\r", arg1, arg2);
      send_to_char(buf, ch);
      vnum = 90000 + (40 * (material-1)) + num_chose;
      if ((forged_object =
create_object(get_obj_index(vnum),1)) == NULL)
	{
	  sprintf(buf, "There seems to be an error with vnum %d.\n\r", vnum);
	  send_to_char(buf, ch);
	  send_to_char("Report this to an admin please.\n\r", ch);
	  return;
	}
      else
	{
	  obj_to(forged_object, ch);
	  send_to_char("Enjoy it.\n\r", ch);
	}
      num_have = 0;
      for(obj = ch->in_room->more->contents; obj != NULL; obj = obj_next)
	{
	  obj_next = obj->next_content;
	  if (obj->pIndexData->item_type == ITEM_MINERAL && ((I_MINERAL *) obj->more)->type == material)
	    {
	      obj_from(obj);
	      free_it(obj);
	      num_have++;
	    }
	  if(num_have >= num_needed)
	    break;
	}
      if (num_chose == 17)
	{
	  send_to_char("The old hammer you used is now useless and it disappears.\n\r", ch);
	  obj_from(hammer);
	  free_it(hammer);
	}
      

      if (num_chose == 18)
	{
	  send_to_char("The old anvil you used is now useless and it disappears.\n\r", ch);
	  obj_from(anvil);
	  free_it(anvil);
	}
      return;
    }
  return;
}




void do_brew (CHAR_DATA * ch, char * argy)
{
  SINGLE_OBJECT *mortar;
  SINGLE_OBJECT *pestle;
  SINGLE_OBJECT *herb;
  SINGLE_OBJECT *herb_next;
  SINGLE_OBJECT *potion;
  bool used_it = FALSE;
  int potion_vnum = 0;
  int potion_timer = 200;
  int rank = 0;
  SPELL_DATA *spell[3];
  int pot_number[3]; /* The number of this spell in the potion info struct */
  char arg[3][SML_LENGTH];
 

 /* UP to 3 spells per potion so you need up to 10 ingredients for each spell so we check the ingredients for each spell separately */
  bool has_ingredient[3][10];
  int i,j;
  
  DEFINE_COMMAND( "brew", do_brew, POSITION_STANDING, 0, LOG_NORMAL, "This command is used by players to brew potions from herbs.")
    if (IS_MOB(ch)) return;

  WAIT_STATE(ch, 5*PULSE_VIOLENCE);
  /* Check to see if the player is actually trying to brew something legit */
  
  if (argy == "" || argy[0] == '\0')
    {
      send_to_char("Brew what?\n\r", ch); 
      return;
    }


  for(i=0; i < 3; i++)
    {
      arg[i][0] = '\0';
      pot_number[i] = -1;
      argy = one_argy(argy, arg[i]);
      if (arg[i] != "" && arg[i][0] != '\0')
	{
	  if ((spell[i] = skill_lookup(arg[i], -1)) == NULL)
	    {
	      send_to_char("Unknown spell name.\n\r", ch);
	      return;
	    }
	  for (j = 0; (pot_info[j].gsn > 0); j++)
	    {
	      if(spell[i]->gsn == pot_info[j].gsn)
		{
		  pot_number[i] = j;
		  break;
		}
	    }
	  if(pot_number[i] == -1)
	    {
	      send_to_char("You cannot brew that kind of a potion.\n\r", ch);
	      return;
	    }
	  if(ch->pcdata->learned[spell[i]->gsn] < pow.max_prac_spells)
	    {
	      send_to_char("You do not know how to cast that spell well enough to brew it into a potion.\n\r", ch);
	      return;
	    }
	}
    }
  if (pot_number[0] == -1 && pot_number[1] == -1 && pot_number[2] == -1)
    {
       send_to_char("You need to brew something, not just brew.\n\r", ch);
       return;
    }

 
  /* Make sure the player is holding the required pestle and mortar */


  if ((pestle = get_item_held(ch, ITEM_TOOL)) == NULL || !IS_SET(((I_TOOL *)pestle->more)->tool_type, TOOL_PESTLE))
    {
      send_to_char("You need to be holding a pestle to brew something.\n\r", ch);
      return;
    }
  if((mortar = get_obj_carry(ch, "mortar")) == NULL || !IS_SET(((I_CONTAINER *)mortar->more)->flags, CONT_MORTAR))
    {
      send_to_char("You must be carrying a mortar with herbs in it to brew.\n\r", ch);
      return;
    }
  
  rank = UMIN(pestle->pIndexData->made_of, mortar->pIndexData->made_of);

  /* Check to see if the player equipment is powerful enough for what they 
     want to make */

  for (i = 0; i < 3; i++)
    {
      if (pot_number[i] != 1 && rank < pot_info[pot_number[i]].equipment_level)
	{
	  char buff[SML_LENGTH];
	  sprintf(buff, "You need to have a mortar and pestle made of %s to brew this potion.\n\r", materials[pot_info[pot_number[i]].equipment_level].name);
	  send_to_char(buff, ch);
	  return;
	}
    }
  
  /* Set up our flag matrix... if the person is brewing a specific spell which
     is brewable and that spell requires an ingredient in that spot then the
     has_ingredient is false, otherwise it is true. */
  
  for (i = 0; i < 10; i++)
    {
      for(j = 0; j < 3; j++)
	{
	  if (pot_number[j] != -1 && pot_info[pot_number[j]].ingredients[i] != 0)
	    has_ingredient[j][i] = FALSE;
	  else
	    has_ingredient[j][i] = TRUE;
	}
    }

  /* Now search the stuff inside the mortar for herbs. For each herb, it goes
     through all the needed potion ingredients for the potion and as it
     finds the herbs, it checks them off basically. If all of the things
     are true at the end, then we have all the stuff we need. */

  for (herb = mortar->contains; herb != NULL; herb = herb->next_content)
    {
      used_it = FALSE;
      for (j = 0; j < 3 && !used_it; j++)
	{
	  for (i = 0; i < 10 && !used_it; i++)
	    {
	      if(!has_ingredient[j][i])
		{
		  if (herb->pIndexData->vnum == (pot_info[pot_number[j]].ingredients[i]+560))
		    {
		      has_ingredient[j][i] = TRUE;
		      used_it = TRUE;
		    }
		}
	    }
	}
    }
  

  /* Now check to make sure we have the ingredients */

  for(j = 0; j < 3; j++)
    {
      for (i= 0; i < 10; i++)
	{
	  if (!has_ingredient[j][i])
	    {
	      send_to_char("You do not have the proper ingredients for this potion.\n\r", ch);
	      return;
	    }
	}
    }


  /* Ok now check to make sure the person is good enough at brewing. */
  
  if(!check_skill(ch, gsn_brew))
    {
      send_to_char("You did not brew the ingredients correctly. Some of the herbs are ruined.\n\r", ch);

      for(herb = mortar->contains; herb != NULL; herb = herb_next)
	{
	  herb_next = herb->next_content;
	  if(number_range(1,8) == 3)
	    {
	      obj_from(herb);
	      free_it(herb);
	    }  
	}
      return;
    }
  skill_gain(ch, gsn_brew, TRUE);
  

  for(i=0;i<3;i++)
    {
      potion_vnum += pot_info[pot_number[i]].gsn;
      potion_timer = UMIN(potion_timer, pot_info[pot_number[i]].timer);
    }
  /* There are 15 potions starting at 750 we can use for this. */
 
  potion_vnum = 750 + number_range(0,14);

  if((potion = create_object(get_obj_index(potion_vnum),1)) == NULL)
    {
      send_to_char("This does not exist. Tell an admin.\n\r", ch);
      return;
    }
  {
    I_POTION *pot = (I_POTION *)potion->more;
    pot->spell_level = UMAX(10, LEVEL(ch) - 10);
    potion->timer = potion_timer;
    for (i = 0; i < 3; i++)
      {
	if (spell[i] != NULL)
	  pot->spells[i] = spell[i]->gsn;
	else
	  pot->spells[i] = 0;
      }
  }
  send_to_char("You mix the ingredients and \x1b[1;31m!\x1b[1;37m!\x1b[1;31mPOOF\x1b[1;37m!\x1b[1;31m!\x1b[0;37m A potion appears.\n\r", ch);
  obj_from(potion);
  obj_to(potion, ch);
  
  /* Ok now the char has the stuff, we then get rid of the herbs and mortar */
  
  for(herb = mortar->contains; herb != NULL; herb = herb_next)
    {
      herb_next = herb->next_content;
      obj_from(herb);
      free_it(herb);
    }  
  obj_from(mortar);
  free_it(mortar);
  return;
}


void do_mine (CHAR_DATA *ch, char *argy)
{

 DEFINE_COMMAND( "mine", do_mine, POSITION_STANDING, 0, LOG_NORMAL, "This command is used by players to mine stuff from rooms and caves.")

   if (IS_MOB(ch)) return;
 send_to_char("You begin to chip away at the rock.\n\r", ch);
 NEW_POSITION(ch, POSITION_MINING);
   ch->pcdata->tickcounts = number_range(10,45);
   return;
}
void do_actual_mine (CHAR_DATA *ch, char *argy)
{
  bool found = TRUE;
  SINGLE_OBJECT *pickaxe;

 
    if (IS_MOB(ch)) return;
  
  if ((pickaxe = get_item_held(ch, ITEM_TOOL)) == NULL || !IS_SET(((I_TOOL *)pickaxe->more)->tool_type, TOOL_PICKAXE))
    {
      send_to_char("You must be holding a pickaxe to mine!\n\r", ch);
      return;
    }
  ch->position = POSITION_STANDING;
  if (ch->in_room->sector_type != SECT_MOUNTAIN &&
      ch->in_room->sector_type != SECT_HILLS &&
      ch->in_room->sector_type != SECT_ROCKY &&
      ch->in_room->sector_type != SECT_CANYON &&
      ch->in_room->sector_type < SECT_CAVE)
    {
      found = FALSE;
    }
  if (LEVEL(ch) < MAX_LEVEL) 
    {
      if (!check_skill(ch, gsn_mine) || number_range(1,3) != 2)
	{
	  found = FALSE;
	}
      if (ch->pcdata->online_spot->mined > 200)
	{
	  found = FALSE;
	  
	}
      if (ch->in_room->minerals <= 0)
	{
      found = FALSE;
	}
    }
  if (!found)
    {
      send_to_char("You hack away at the rock for a while, but find nothing.\n\r", ch);
      WAIT_STATE(ch, 3*PULSE_VIOLENCE);
      return;
    }
  else
    {
      int i;
      long vnum;
      SINGLE_OBJECT *chunk;
      
      send_to_char("You manage to break off a chunk of mineral.\n\r", ch);
      for (i=0;i < 15; i++)
	if (number_range(1,4) == 2)
	  break;
      if (i >= 14)
        if (number_range(1,4) != 2)
	  i = number_range(0,10);
        else
          i = 14;
      vnum = 90000 + (40 * i) + 19;
      if ((chunk = create_object(get_obj_index(vnum),1)) == NULL)
	{
	  char buf[100];
	  sprintf(buf, "There seems to be an error with vnum %d.\n\r", vnum);
	  send_to_char(buf, ch);
	  send_to_char("Report this to an admin please.\n\r", ch);
	  return;
	}
      else
	{
          chunk = create_object(get_obj_index(vnum),1);
	  obj_to(chunk, ch);
          ch->in_room->minerals--;
	  ch->pcdata->online_spot->mined++;
          if (number_range(1,50) == 3)
            skill_gain(ch, gsn_mine, TRUE);
	}
    }
  return;
}


void 
do_enchant (CHAR_DATA *ch, char* argy)
{
  char arg1[STD_LENGTH];
  char arg2[STD_LENGTH];
  char arg3[STD_LENGTH];
  char buf[STD_LENGTH];
  SINGLE_OBJECT *obj;
  AFFECT_DATA *paf;
  int location;
  int modifier;
  int cost = 0;
  int i, j;
  bool affected_already[30];
  DEFINE_COMMAND( "enchant", do_enchant, POSITION_STANDING, 0, LOG_NORMAL, "This command lets you enchant items for specific enhancements.")
    
    if (IS_MOB(ch)) return;
  
  for (i = 0; i < 30; i++)
      affected_already[i] = FALSE;
  


    argy = one_argy(argy, arg1);
  if ((obj = get_obj_inv(ch, arg1)) == NULL)
    {
      send_to_char("You don't have that object in your inventory.\n\r", ch);
      return;
    }
  
  argy = one_argy(argy, arg2);
  if ((location = affect_name_loc (arg2)) == -99)
    {
      send_to_char("Unknown location.\n\r", ch);
      return;
    }
  if (LEVEL(ch) == MAX_LEVEL)
    {
      argy = one_argy(argy, arg3);
      if (is_number(arg3))
	modifier = atoi(arg3);
      if (modifier > 100 || modifier < -100)
	{
	  send_to_char("Modifiers must be between -100 and 100.\n\r", ch);
	  return;
	}
      paf = new_affect ();
      paf->location = location;
      paf->modifier = modifier;
      paf->type = 1;
      paf->duration = 0;
      paf->bitvector = 0;
      paf->next = obj->affected;
      obj->affected = paf;
      send_to_char ("Enchantment added.\n\r", ch);
      return;
    }
  else
    {
      if (obj->pIndexData->item_type != ITEM_WEAPON && obj->pIndexData->item_type != ITEM_ARMOR)
    {
      send_to_char("You can only enchant armor and weapons.\n\r", ch);
      return;
    }
  for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next)
    {
      affected_already[paf->location] = TRUE;
      if ((paf->location > 0 && paf->location < 6) || paf->location == APPLY_DAMROLL)
	affected_already[29] = TRUE;

      if (paf->location == APPLY_KICK_DAMAGE && paf->modifier >=3)
	affected_already[29] = TRUE;
      
    }

      if (ch->max_hit < 800)
	{
	  send_to_char("You do not have enough health to enchant.\n\r", ch);
	  return;
	}
      if (ch->pcdata->learned[gsn_enchant] < pow.max_prac_skills)
	{
	  send_to_char("You are not skilled enough at enchanting.\n\r", ch);
	  return;
	}
      if (obj->pIndexData->item_type != ITEM_WEAPON && obj->pIndexData->item_type != ITEM_ARMOR)
	{
	  send_to_char("You can only enchant armor and weapons.\n\r", ch);
	  return;
	}
      if (obj->affected)
	{
	  CHAR_DATA *rch;
	  send_to_char("You attempt to enchant an item more than once and it implodes under the force!\n\r", ch);
	  send_to_char("The Angel of the Anti-Cheater places \x1b[1;31mThe Bloody Dagger of Infinite Pain and Unbearable Suffering\x1b[0;37m into your back! OUCH!!!! That was *NOT* pleasant!\n\r", ch);
	  obj_from(obj);
	  free_it(obj);
	  ch->hit = 1;
	  return;
	}
      if(affected_already[location])
	{
	  send_to_char("That item already has that kind of enchantment, and cannot accept any more.\n\r", ch);
	  return;
	}
      switch (location)
	{
   /*	case APPLY_STR: */
	case APPLY_DAMROLL:
   /*	case APPLY_INT: */
  /*	case APPLY_WIS: */
  /*	case APPLY_DEX: */
	  {
	    if (LEVEL(ch) < 60)
	      {
		send_to_char("You are too weak to enchant something this powerful!\n\r", ch);
		return;
	      }
	    modifier = 1;
	    cost = 300000;
	    
	    break;
	  }
  /*  	case APPLY_CON: */
	  {
	    send_to_char("Uh, no. Nice try tho.\n\r", ch);
	    return;
	  }
	break;
	case APPLY_HIT:
	case APPLY_MOVE:
	case APPLY_SNEAK:
	case APPLY_HIDE:
	  {
	    modifier = 3 + LEVEL(ch)/8;
	    cost = 50000;
	    break;
	  }
	case APPLY_AC:
	case APPLY_SAVING_THROW:
	  {
	    modifier = -1 - LEVEL(ch)/37;
	    cost = 10000;
	    break;
	  }
	case APPLY_HITROLL:
	  {
	    modifier = 1 + LEVEL(ch)/29;
	    cost = 10000;
	    break;
	  }
	case APPLY_KICK_DAMAGE:
	  {
	    modifier = 1 + LEVEL(ch)/25;
	    if (!CAN_WEAR(obj, ITEM_WEAR_FEET) && !CAN_WEAR(obj, ITEM_WEAR_ANKLE) &&
		!CAN_WEAR(obj, ITEM_WEAR_LEGS) && !CAN_WEAR (obj, ITEM_WEAR_KNEE))
	      {
		send_to_char("You cannot put kickdam on a piece of eq worn here.\n\r", ch);
		return;
	      }
	    if (CAN_WEAR(obj, ITEM_WEAR_FEET) || CAN_WEAR(obj, ITEM_WEAR_LEGS))
	      {
		modifier = 1 + LEVEL(ch)/25;
	      }
	    else
	      {
		modifier = 1 +  LEVEL(ch)/45;
	      }
	    break;
	  }
	default: 
	  cost = 1000000;
	  modifier = 0;
	  break;
	}
      if (!CAN_WEAR(obj, ITEM_WEAR_LEGS) && !CAN_WEAR(obj, ITEM_WEAR_ARMS) && !CAN_WEAR(obj, ITEM_WEAR_BODY) && !CAN_WEAR(obj, ITEM_WEAR_WAIST) && !CAN_WEAR(obj, ITEM_WEAR_HEAD) && !CAN_WEAR(obj, ITEM_WEAR_FEET) && !CAN_WEAR(obj, ITEM_WEAR_SHIELD))
	cost *= 2;
      if (affected_already[29])
	cost *=2;
      if (ch->pcdata->bank < cost)
	{
	  sprintf(buf, "You need %d coins in the bank to put this enchantment on this item.\n\r", ch);
	  send_to_char(buf, ch);
	  return;
	}
      if (!check_skill(ch, gsn_enchant) || number_range(1,3) != 2)
	{
	  send_to_char("You fail your enchantment.\n\r", ch);
	  ch->max_hit--;
	  ch->pcdata->bank -= cost/10;
	  if (number_range(1,20) == 3)
	    {
	      send_to_char("The object is destroyed!\n\r", ch);
	      obj_from(obj);
	      free_it(obj);
	      ch->hit = 1;
	    }
	  return;
	}
      skill_gain(ch, gsn_enchant, TRUE);
      paf = new_affect ();
      paf->location = location;
      paf->modifier = modifier;
      paf->type = 1;
      paf->duration = 0;
      paf->bitvector = 0;
      paf->next = obj->affected;
      obj->affected = paf;
      ch->max_hit--;
      ch->pcdata->bank -= cost;
      send_to_char ("Enchantment added.\n\r", ch);
      return;
    }
  return;
}

void 
do_unenchant (CHAR_DATA *ch, char* argy)
{
  SINGLE_OBJECT *obj;
 AFFECT_DATA *paf;
 DEFINE_COMMAND( "unenchant", do_unenchant, POSITION_STANDING, MAX_LEVEL, LOG_NORMAL, "This command lets you unenchant items and make them normal again.")
    
 if ((obj = get_obj_inv(ch, argy)) == NULL)
   {
     send_to_char("You don't have that object in your inventory.\n\r", ch);
     return;
   }
 paf = obj->affected;
 if (paf == NULL)
   {
     send_to_char("No enchantments to remove.\n\r", ch);
     return;
   }
 obj->affected = obj->affected->next;
 free_affect(paf);
 send_to_char("Ok, enchantment removed.\n\r", ch);
 return;
}
	

void 
do_lore (CHAR_DATA *ch, char* argy)
{
  SPELL_DATA *spell;
  char arg1[SML_LENGTH];
  int i;
  int potion_number = -1;
  char buff[SML_LENGTH];
  bool used_this[10];
  int total_num = 0;
  int num_used = 0;
  
  DEFINE_COMMAND( "lore", do_lore, POSITION_STANDING, 0, LOG_NORMAL, "This command lets you find out information on certain potions.")
 

    if (IS_MOB(ch)) return;
  
  if(!is_member(ch, GUILD_THIEFG) || !is_member(ch, GUILD_MYSTIC) || !is_member(ch, GUILD_WIZARD) || !is_member(ch, GUILD_MYSTIC))
    {
      send_to_char("Only those who are thieves, mystics, conjurers and wizards have the knowledge necessary to use this skill.\n\r", ch);
      return;
    }
  for (i=0; i < 10; i++)
    used_this[i] = FALSE;
  arg1[0] = '\0';
  argy = one_argy(argy, arg1);
  
  if ((spell = skill_lookup(arg1, -1)) == NULL)
      {
	send_to_char("That spell does not exist, so you cannot brew it into a potion.\n\r", ch);
	return;
      }
  for (i = 0; (pot_info[i].gsn > 0); i++)
    {
      if(spell->gsn == pot_info[i].gsn)
	{
	  potion_number = i;
	  break;
	}
    }
  if (potion_number == -1)
    {
      send_to_char("You cannot brew a potion of that.\n\r", ch);
      return;
    }
  sprintf(buff, "In order to brew a potion of %s, you will need ", spell->spell_name);
  for(i = 0; i < 10; i++)
    {
      
      if (number_range(1,220) < ch->pcdata->learned[gsn_lore] && pot_info[potion_number].ingredients[i] > 0)
	{
	  used_this[i] = TRUE;
	  total_num++;
	}
    }
  if (total_num == 0)
    {
      strcat(buff, "nothing that you can tell.\n\r");
      send_to_char(buff, ch);
      return;
    }
  else if (total_num == 1)
    {
      for (i = 0; i < 10; i++)
	{
	  if (used_this[i] == TRUE)
	    {
	      strcat(buff, get_obj_index(560+pot_info[potion_number].ingredients[i])->short_descr);
	    }
	}
      strcat(buff, ".\n\r");
      send_to_char(buff, ch); 
      return;
    }
  else 
    {
      for (i = 0; i < 10; i++)
	{
	  if (used_this[i] == TRUE)
	    {
	      strcat(buff, get_obj_index(560+pot_info[potion_number].ingredients[i])->short_descr);
	      if (num_used < total_num-2)
		strcat(buff, ", ");
	      else 
		if (num_used == total_num-2)
		  strcat(buff, " and ");
		else
		  {
		    strcat(buff, ".\n\r");
		  }
	      num_used++;
	    }
	}
      send_to_char(buff, ch);
    }
  return;
}

  
