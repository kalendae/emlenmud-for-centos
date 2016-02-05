/* Groundfighting!! */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emlen.h"


/* Groundfighting changed to give more advantages to warriors. 
   Choke now requires 3 steps to break...
   and its based on str and dex and augments and height! */



void 
bite (CHAR_DATA * ch, CHAR_DATA * victim, bool break_choke) 
{
  int dam;
  char buff[SML_LENGTH];
  dam = number_range (5, 12) + UMIN ((LEVEL (ch) / 5), 12);
  if (IS_AUGMENTED(ch, AUG_FACE_SPIKE)) dam += 5;
  if (IS_PLAYER(ch) &&  (!str_cmp(race_info[ch->pcdata->race].name, "minotaur") || !str_cmp(race_info[ch->pcdata->race].name, "gargoyle"))) dam += 3;
  if (IS_PLAYER(victim) && (!str_cmp(race_info[ch->pcdata->race].name, "minotaur") || !str_cmp(race_info[ch->pcdata->race].name, "gargoyle"))) dam /= 1;
  if (IS_PLAYER(ch) && ch->pcdata->learned[gsn_grapple] != -100)
    dam += (ch->pcdata->learned[gsn_grapple]/7);
  if (number_range(1,70) == 3) skill_gain(ch, gsn_grapple, TRUE);

  if (ch->ced->ground_pos == FP_CHOKE_3 || 
      ch->ced->ground_pos == FP_CHOKE_1 ||
      ch->ced->ground_pos == FP_CHOKE_2)
    {
      if (break_choke)
	{
	  act ("You manage to swivel your head and bite $N's wrist!", ch, NULL, victim, TO_CHAR);      
	  act ("$n turns $s head and bites $N's wrist!", ch, NULL, victim, TO_NOTVICT);      
	  act ("$n turns $s head and bites your wrist!", ch, NULL, victim, TO_VICT);      
	  act ("$N quickly lets go of your neck! You can BREATH again!", ch, NULL, victim, TO_CHAR);      
	  act ("$N can no longer keep the chokehold on $n!", ch, NULL, victim, TO_NOTVICT);      
	  act ("OUCH! You no longer have a chokehold on $n!", ch, NULL, victim, TO_VICT);      
	  ch->ced->ground_pos = 0;      
	  victim->hit -= dam;      
	  return;      
	}
      else
	{
	  act ("You try to twist around to bite $N's wrist, but can't!", ch, NULL, victim, TO_CHAR);      
	  act ("$n turns $s head tries to bite $N's wrist!", ch, NULL, victim, TO_NOTVICT);      
	  act ("$n turns $s head and tries to bite your wrist!", ch, NULL, victim, TO_VICT);  
	  return;
	}
    }
  if (CHALLENGE (ch) == 10 && number_range (1, 2) == 2)
    {
      sprintf (buff, "($B$1Arena$N$7) $B%s gets a mouthful of flesh with a powerful bite!", NAME (ch));
      do_arenaecho (ch, buff, TRUE);
    }
  
  if (number_range (1, 2) == 1)
    {      
      act ("Your bite $N tears a chunk of flesh from $s shoulder!", ch, NULL, victim, TO_CHAR);	
      act ("$n bites $N in the shoulder; blood runs everywhere!", ch, NULL, victim, TO_NOTVICT);	
      act ("$n tears some flesh out of your shoulder with $s bite!", ch, NULL, victim, TO_VICT);
      dam -= 1;
    }
  else
    {
      act ("You bite $N's arm! Blood runs everywhere!", ch, NULL, victim, TO_CHAR);
      act ("$n bites $N's arm and spits out a chunk of flesh!", ch, NULL, victim, TO_NOTVICT);
      act ("$n bites your arm and you begin to bleed!", ch, NULL, victim, TO_VICT);
      dam += 3;
    }
  victim->hit -= dam;
  return;
}

void 
strangle (CHAR_DATA * ch, CHAR_DATA * victim, bool break_choke) 
{
  int dam; 
  dam = number_range (10, (get_curr_str (ch)) + UMIN ((LEVEL (ch) / 10), 6));
  if (IS_AUGMENTED(ch, AUG_FACE_SPIKE)) dam += 5;
  if (IS_PLAYER(ch) && ch->pcdata->learned[gsn_grapple] != -100)
    dam += (ch->pcdata->learned[gsn_grapple]/7);
  if (IS_PLAYER(ch) &&  (!str_cmp(race_info[ch->pcdata->race].name, "minotaur") || !str_cmp(race_info[ch->pcdata->race].name, "gargoyle"))) dam += 3;
  if (IS_PLAYER(victim) && (!str_cmp(race_info[ch->pcdata->race].name, "minotaur") || !str_cmp(race_info[ch->pcdata->race].name, "gargoyle"))) dam /= 1;
  if (number_range(1,70) == 3) skill_gain(ch, gsn_grapple, TRUE);

  if (ch->ced->ground_pos == FP_CHOKE_1 || 
      ch->ced->ground_pos == FP_CHOKE_2 ||
      ch->ced->ground_pos == FP_CHOKE_3)
    {
      act ("You claw at $N, trying to loosen $S chokehold...", ch, NULL, victim, TO_CHAR);       
      if (break_choke)
	{
	  act ("You succeed! You can BREATH again!!", ch, NULL, victim, TO_CHAR); 
	  act ("$n claws at $N and breaks the chokehold!", ch, NULL, victim, TO_NOTVICT);	  
	  act ("$n claws at your eyes and you loose the chokehold.", ch, NULL, victim, TO_VICT);	  
	  ch->ced->ground_pos = 0;	  
	  victim->hit -= 9;	  
	  return;	  
	}
      else
	{
	  act ("You can't manage to loosen $N's chokehold!", ch, NULL, victim, TO_CHAR);	  
	  act ("$n tries to loosen $N's chokehold but fails.", ch, NULL, victim, TO_NOTVICT);	  
	  act ("$n tries to loosen your chokehold but fails.", ch, NULL, victim, TO_VICT);	  
	  return;	  
	}  
    }
	
  if (number_range (1, 2) == 1)
    {      
      act ("You grab $N's throat and squeeze...", ch, NULL, victim, TO_CHAR);      
      act ("$n grabs $N's windpipe and tries to rip it out!", ch, NULL, victim, TO_NOTVICT);      
      act ("$n grabs your windpipe! You can't BREATHE!!", ch, NULL, victim, TO_VICT);      
      victim->ced->ground_pos = FP_CHOKE_1;      
      dam += 3;      
    }
  else    
    {      
      act ("You successfully put a air-choke on $N!", ch, NULL, victim, TO_CHAR);      
      act ("$n begins to strangle $N!", ch, NULL, victim, TO_NOTVICT);      
      act ("$n begins to strangle you!", ch, NULL, victim, TO_VICT);
      dam += 5;      
      victim->ced->ground_pos = FP_CHOKE_1;      
    }  
  victim->hit -= dam;  
  return;
}


void 
juji_gatame (CHAR_DATA * ch, CHAR_DATA * victim, bool break_choke) 
{  
  int dam;  
   char buff[SML_LENGTH];
  dam = number_range (10, (get_curr_str (ch)) + UMIN ((LEVEL (ch) / 10), 7));
  if (IS_AUGMENTED(ch, AUG_FACE_SPIKE)) dam += 5;
  if (IS_PLAYER(ch) && ch->pcdata->learned[gsn_grapple] != -100)
    dam += (ch->pcdata->learned[gsn_grapple]/7);
  if (IS_PLAYER(ch) &&  (!str_cmp(race_info[ch->pcdata->race].name, "minotaur") || !str_cmp(race_info[ch->pcdata->race].name, "gargoyle"))) dam += 3;
  if (IS_PLAYER(victim) && (!str_cmp(race_info[ch->pcdata->race].name, "minotaur") || !str_cmp(race_info[ch->pcdata->race].name, "gargoyle"))) dam /= 1;
  if (number_range(1,70) == 3) skill_gain(ch, gsn_grapple, TRUE);
  
  if (ch->ced->ground_pos == FP_CHOKE_3 || 
      ch->ced->ground_pos == FP_CHOKE_1 ||
      ch->ced->ground_pos == FP_CHOKE_2)
    {
      if (break_choke)
	{
	  act ("You grab ahold of one of the hands choking you and twist it painfully!", ch, NULL, victim, TO_CHAR);
	  act ("$n grabs $N's hand that is on $s neck and twists.", ch, NULL, victim, TO_NOTVICT);
	  act ("$n grabs a hand you are using for the choke and twists!", ch, NULL, victim, TO_VICT);
	  act ("You are free of the choke-hold!! You can BREATHE!!", ch, NULL, victim, TO_CHAR);
	  act ("$n is free of the choke, and pants heavily.", ch, NULL, victim, TO_ROOM);
	  victim->hit -= dam;
	  ch->ced->ground_pos = 0;
	  return;
	}
      else
	{
	  act ("You can't manage to grab ahold of $N's hands! $N is choking your neck too tightly! You can't BREATHE!!", ch, NULL, victim, TO_CHAR);
	  act ("$n fails to grab ahold of $N's hands.", ch, NULL, victim, TO_NOTVICT);
	  act ("$n fails to grab your hands, and you laugh as you continue your firm chokehold.", ch, NULL, victim, TO_VICT);
	  return;
	}
    }
  if (CHALLENGE (ch) == 10 && number_range (1, 2) == 2)
    {  
      sprintf (buff, "($B$1Arena$N$7) $BYou can hear bones cracking as %s twists %s's arm!", NAME (ch), NAME (victim));
      do_arenaecho (ch, buff, TRUE);
    }
  
  if (number_range (1, 2) == 1)
    {
      act ("You skillfully bend $N's arm into a painful position...", ch, NULL, victim, TO_CHAR);
      act ("$n twists $N's arm into a painful position!", ch, NULL, victim, TO_NOTVICT);
      act ("$n twists your arm painfully! Ouch!!", ch, NULL, victim, TO_VICT);
      dam -= 1;
    }
  else
    {
      act ("You straighten $N's arm and hyperextend $S elbow!", ch, NULL, victim, TO_CHAR);
      act ("$n straightens $N's arm, hyperextending $S elbow!", ch, NULL, victim, TO_NOTVICT);
      act ("$n grabs your arm and hyperextends your elbow joint!", ch, NULL, victim, TO_VICT);
      dam += 3;
    }
  victim->hit -= dam;
  return;
}

void 
knee_stomach (CHAR_DATA * ch, CHAR_DATA * victim, bool break_choke) 
{
  int dam;
   char buff[SML_LENGTH];
  dam = number_range (15, (get_curr_str (ch)) + UMIN ((LEVEL (ch) / 10), 12));
  if (IS_AUGMENTED(ch, AUG_FACE_SPIKE)) dam += 5;
  if (IS_PLAYER(ch) && ch->pcdata->learned[gsn_grapple] != -100)
    dam += (ch->pcdata->learned[gsn_grapple]/7);
  if (IS_PLAYER(ch) &&  (!str_cmp(race_info[ch->pcdata->race].name, "minotaur") || !str_cmp(race_info[ch->pcdata->race].name, "gargoyle"))) dam += 3;
  if (IS_PLAYER(victim) && (!str_cmp(race_info[ch->pcdata->race].name, "minotaur") || !str_cmp(race_info[ch->pcdata->race].name, "gargoyle"))) dam /= 1;
  if (number_range(1,70) == 3) skill_gain(ch, gsn_grapple, TRUE);

  if (ch->ced->ground_pos == FP_CHOKE_3 || 
      ch->ced->ground_pos == FP_CHOKE_1 ||
      ch->ced->ground_pos == FP_CHOKE_2)
    {
      act ("You manage to drive a knee into $N's stomach, but it doesn't loosen $S hands around your neck!", ch, NULL, victim, TO_CHAR);
      if (strlen (NAME (victim)) > 13)
	act ("$n knees $N, but $N keeps $S chokehold firmly around $n's neck.", ch, NULL, victim, TO_NOTVICT);
      else
	act ("$n knees $N, but $N keeps $S chokehold.", ch, NULL, victim, TO_NOTVICT);
      act ("$n knees you, but you keep your chokehold...", ch, NULL, victim, TO_VICT);
      dam -= 6;
      if (dam < 2)
	dam = 2;
      victim->hit -= dam;
      return;
    }

  if (CHALLENGE (ch) == 10 && number_range (1, 2) == 2)
    {	      
      sprintf (buff, "($B$1Arena$N$7) $B%s gasps for air as knee and stomach collide...", NAME (victim));
      do_arenaecho (ch, buff, TRUE);	      
    }
  
  if (number_range (1, 2) == 1)
    {
      act ("You manage to drive a knee into $N's stomach...", ch, NULL, victim, TO_CHAR);
      act ("$n knees $N in the stomach!", ch, NULL, victim, TO_NOTVICT);
      act ("$n knees you in the solar plexus!!", ch, NULL, victim, TO_VICT);
      dam -= 1;
    }
  else if (SEX (victim) != SEX_FEMALE)
    {
      act ("You drive your knee into $N's groin area!", ch, NULL, victim, TO_CHAR);
      act ("$n drives $s knee up into $N's groin area!", ch, NULL, victim, TO_NOTVICT);
      act ("$n drives $s knee into your groin area!", ch, NULL, victim, TO_VICT);
      dam += 3;
    }
  else
    {
      act ("You drive your knee into $N's leg!", ch, NULL, victim, TO_CHAR);
      act ("$n drives $s knee up into $N's leg!", ch, NULL, victim, TO_NOTVICT);
      act ("$n drives $s knee into your leg!", ch, NULL, victim, TO_VICT);
      dam += 3;
    }
  victim->hit -= dam;
  return;
}

void 
elbow (CHAR_DATA * ch, CHAR_DATA * victim, bool break_choke) 
{
  SINGLE_OBJECT * obj_2;
   char buff[SML_LENGTH];
  int dam;
  dam = (get_curr_str (ch) / 2) + number_range (1, LEVEL (ch) / 5);
  if (IS_AUGMENTED(ch, AUG_FACE_SPIKE)) dam += 5;
  if (IS_PLAYER(ch) && ch->pcdata->learned[gsn_grapple] != -100)
    dam += (ch->pcdata->learned[gsn_grapple]/7);
  if (IS_PLAYER(ch) &&  (!str_cmp(race_info[ch->pcdata->race].name, "minotaur") || !str_cmp(race_info[ch->pcdata->race].name, "gargoyle"))) dam += 3;
  if (IS_PLAYER(victim) && (!str_cmp(race_info[ch->pcdata->race].name, "minotaur") || !str_cmp(race_info[ch->pcdata->race].name, "gargoyle"))) dam /= 1;
  if (number_range(1,70) == 3) skill_gain(ch, gsn_grapple, TRUE);
  obj_2 = get_eq_char (ch, WEAR_FACE);
  
  if (ch->ced->ground_pos == FP_CHOKE_3 || 
      ch->ced->ground_pos == FP_CHOKE_1 ||
      ch->ced->ground_pos == FP_CHOKE_2)
    {
      if (break_choke)
	{
	  act ("You elbow $N, breaking $S chokehold!", ch, NULL, victim, TO_CHAR);
	  act ("$n elbows $N, breaking $S chokehold!", ch, NULL, victim, TO_NOTVICT);
	  act ("$n elbows you, breaking your chokehold!", ch, NULL, victim, TO_VICT);
	  victim->hit -= dam;
	  ch->ced->ground_pos = 0;
	  return;
	}
      else
	{
	  act ("You try to elbow $N, but $S hands are around your throat!", ch, NULL, victim, TO_CHAR);
	  act ("$n tries to elbow $N, but fails because $N still has a firm choke-hold!", ch, NULL, victim, TO_NOTVICT);
	  act ("$n tries to elbow you, but you keep squeezing $s neck, and he can't manage to get $s elbow to your face.", ch, NULL, victim, TO_VICT);
	  return;
	}
    }
  
  if (CHALLENGE (ch) == 10 && number_range (1, 2) == 2)	    
    {	      
      sprintf (buff, "($B$1Arena$N$7) $B%s smashes %s with a hard elbow!", NAME (ch), NAME (victim));
      do_arenaecho (ch, buff, TRUE);	      
    }

  
  if (number_range (1, 2) == 1)
    {
      if (obj_2 == NULL)
	{
	  act ("You send a hard elbow right into $N's face!", ch, NULL, victim, TO_CHAR);
	  act ("$n slams $s elbow down hard into $N's face!", ch, NULL, victim, TO_NOTVICT);
	  act ("$n slams $s elbow into your face! Ouch!", ch, NULL, victim, TO_VICT);
	  victim->hit -= dam;
	  return;
	}
      if (obj_2->pIndexData->item_type == ITEM_ARMOR)
	{
	  act ("You send a hard elbow right into $N's face!", ch, NULL, victim, TO_CHAR);
	  act ("$n slams $s elbow down hard into $N's face!", ch, NULL, victim, TO_NOTVICT);
	  act ("$n slams $s elbow into your face! Ouch!", ch, NULL, victim, TO_VICT);
	  act ("Your face begins to bleed as $p digs into your flesh.", ch, obj_2, victim, TO_CHAR);
	  act ("$n's nose bleeds as $p digs into $s face.", ch, obj_2, victim, TO_ROOM_SPAM);
	  dam += 7;
	  victim->hit -= dam;
	  return;
	}
    }
  else
    {
      act ("You smash your elbow into the side of $N's head!", ch, NULL, victim, TO_CHAR);
      act ("$n smashes $s elbow into $N's jaw!", ch, NULL, victim, TO_NOTVICT);
      act ("$n's elbow smashes into your jaw! Ouch!", ch, NULL, victim, TO_VICT);
      dam += number_range (1, 5);
      victim->hit -= dam;
    }
  return;
}

void 
rip_ears (CHAR_DATA * ch, CHAR_DATA * victim, bool break_choke) 
{
  char buf[85];  
  int dam;
  dam = 45;
  if (IS_AUGMENTED(ch, AUG_FACE_SPIKE)) dam += 5;
  if (IS_PLAYER(ch) && ch->pcdata->learned[gsn_grapple] != -100)
    dam += (ch->pcdata->learned[gsn_grapple]/7);
  if (number_range(1,70) == 3) skill_gain(ch, gsn_grapple, TRUE);
  if (IS_PLAYER(ch) &&  (!str_cmp(race_info[ch->pcdata->race].name, "minotaur") || !str_cmp(race_info[ch->pcdata->race].name, "gargoyle"))) dam += 3;
  if (IS_PLAYER(victim) && (!str_cmp(race_info[ch->pcdata->race].name, "minotaur") || !str_cmp(race_info[ch->pcdata->race].name, "gargoyle"))) dam /= 1;
  if (get_eq_char (ch, WEAR_HEAD) == NULL)
    return; 

  if (ch->ced->ground_pos == FP_CHOKE_3 || 
      ch->ced->ground_pos == FP_CHOKE_1 ||
       ch->ced->ground_pos == FP_CHOKE_2)
     {
       if (break_choke)
	 {
	   act ("You grab $N's ear and pull! It rips off in a bloody mess and $N quickly stops choking you! You can BREATH again!", ch, NULL, victim, TO_CHAR);      
	   act ("$n rips $N's ear clean off! $N quickly stops choking $n!", ch, NULL, victim, TO_NOTVICT);      
	   sprintf (buf, "OUCH! %s just ripped my ear off!!", NAME (ch));
	   do_yell (victim, buf); 
	   victim->hit -= dam;
	   ch->ced->ground_pos = 0;
	   return;
	 }
       else
	 {
	   act("You try to grab hold of $N's ear, but cannot!", ch, NULL, victim, TO_CHAR);
	   act("$n tries to grab hold of your ear, but you keep choking $n!", ch, NULL, victim, TO_VICT);
	   act("$n cannot escape $N's chokehold!", ch, NULL, victim, TO_CHAR);
	   return;
	 }
     }

  if (number_range (1, 2) == 1) 
    {
      act ("You just bit $N's ear off! Blood oozes everywhere!", ch, NULL, victim, TO_CHAR);      
      act ("$n grabs onto $N's ear with $s teeth and rips it clean off! A large pool of blood begins to form on the ground.", ch, NULL, victim, TO_NOTVICT);      
      sprintf (buf, "The pain, the PAIN!");      
      do_yell (victim, buf);      
      victim->hit -= dam;      
      return;      
    }
  else  
    {      
      act ("You grab ahold of $N's ear and rip it off!", ch, NULL, victim, TO_CHAR);      
      act ("$n grabs onto $N's ear and rips it off! $n smiles as $s tosses the ear aside!", ch, NULL, victim, TO_NOTVICT);      
      sprintf (buf, "Damn that hurt!!");
      do_yell (victim, buf);      
      dam += number_range (1, 5);
      victim->hit -= dam;      
    }
  return;
}


void 
head_butt (CHAR_DATA * ch, CHAR_DATA * victim, bool break_choke) 
{
  int dam; 
   char buff[SML_LENGTH];
  dam = (get_curr_str (ch) / 3) + (number_range (1, LEVEL (ch) / 10));
  if (IS_AUGMENTED(ch, AUG_FACE_SPIKE)) dam += 20;
  if (IS_PLAYER(ch) && ch->pcdata->learned[gsn_grapple] != -100)
    dam += (ch->pcdata->learned[gsn_grapple]/7);
  if (IS_PLAYER(ch) &&  (!str_cmp(race_info[ch->pcdata->race].name, "minotaur") || !str_cmp(race_info[ch->pcdata->race].name, "gargoyle"))) dam += 3;
  if (IS_PLAYER(victim) && (!str_cmp(race_info[ch->pcdata->race].name, "minotaur") || !str_cmp(race_info[ch->pcdata->race].name, "gargoyle"))) dam /= 1;
  if (number_range(1,70) == 3) skill_gain(ch, gsn_grapple, TRUE);
  
   if (ch->ced->ground_pos == FP_CHOKE_3 || 
      ch->ced->ground_pos == FP_CHOKE_1 ||
      ch->ced->ground_pos == FP_CHOKE_2)
    {
      if (break_choke)
	{
	  act ("You manage to get your hands around $N's head! You pull your arms into you quickly, and ram $S head into yours!", ch, NULL, victim, TO_CHAR);	  
	  act ("$n grabs $N's head and pulls it into $s!", ch, NULL, victim, TO_NOTVICT);	  
	  act ("$n grabs your head and pulls it into $s forehead!", ch, NULL, victim, TO_VICT);	  
	  act ("$N is dazed and stops choking you!!", ch, NULL, victim, TO_CHAR);	  
	  act ("$n got free of the choke!", ch, NULL, victim, TO_NOTVICT);	  
	  act ("You feel dazed and let go of your chokehold on $n.", ch, NULL, victim, TO_VICT);	  
	  ch->ced->ground_pos = 0;	  
	  dam += 3;	  
	  victim->hit -= dam;	 
	  return;	  
	}      
      else	
	{	  
	  act ("Your head spins from lack of oxygen!", ch, NULL, victim, TO_CHAR);	  
	  act ("$n looks dazed and confused from lack of oxygen!", ch, NULL, victim, TO_ROOM);	  
	  return;	  
	}      
    }
   
   if (CHALLENGE (ch) == 10 && number_range (1, 2) == 2)
     {	      
       sprintf (buff, "($B$1Arena$N$7) $B%s's nose spurts blood from a mean headbutt!", NAME (victim));	      
       do_arenaecho (ch, buff, TRUE);	      
     }
   
   if (IS_PLAYER(ch) && IS_AUGMENTED(ch, AUG_FACE_SPIKE) &&
number_range(1,6) == 2)       
     {
       if (!IS_AFFECTED_EXTRA(victim, AFF_PROT_BLIND))
	 {
	   AFFECT_DATA paf;
	   bzero (&paf, sizeof (paf));
	   paf.type = gsn_blindness;
	   paf.duration = 3;
	   paf.location = 0;
	   paf.modifier = 0;
	   paf.bitvector = AFF_BLIND;
	   paf.bitvector2 = 0;
	   act ("You pierce one of $N's eyes.", ch, NULL, victim, TO_CHAR);
	   act("$n blinds $N with $s face spike!", ch, NULL, victim,TO_NOTVICT);
	   act ("$n impales you with $s face spike!", ch, NULL, victim, TO_VICT);
	   renew_affect (victim, &paf);

	 }
     }
   
   switch(number_range(1,3))
    {
    case 1:
      act ("You ram your forehead into $N's face!", ch, NULL, victim, TO_CHAR);	  
      act ("$n slams $s forehead into $N's face!", ch, NULL, victim, TO_NOTVICT);	  
      act ("$n slams $s forehead into your face! Ouch!", ch, NULL, victim, TO_VICT);	  
      victim->hit -= dam;	  	  
      break;
    case 2:
      act ("You ram your head into $N's jaw!", ch, NULL, victim, TO_CHAR);	  
      act ("$n slams $s head into $N's jaw!", ch, NULL, victim, TO_NOTVICT);	  
      act ("$n slams $s head into your jaw! Ouch!", ch, NULL, victim, TO_VICT);
      dam += 7;	  
      victim->hit -= dam;	  	  
      break;
    case 3:
    default:
      act ("You ram your forehead into $N's cheekbone!", ch, NULL, victim, TO_CHAR);  
      act ("$n rams $s forehead into $N's cheekbone!", ch, NULL, victim, TO_NOTVICT);      
      act ("$n's forehead rams into your cheekbone! Ouch!", ch, NULL, victim, TO_VICT);      
      dam += number_range (1, 5);      
      victim->hit -= dam;      
    } 
  return;
}


void 
get_better_pos (CHAR_DATA * ch, CHAR_DATA * victim, bool break_choke) 
{
  int  dam = 0;

  if (IS_AUGMENTED(ch, AUG_FACE_SPIKE)) dam += 5;
  if (IS_PLAYER(ch) && ch->pcdata->learned[gsn_grapple] != -100)
    dam += (ch->pcdata->learned[gsn_grapple]/7);
  if (IS_PLAYER(ch) &&  (!str_cmp(race_info[ch->pcdata->race].name, "minotaur") || !str_cmp(race_info[ch->pcdata->race].name, "gargoyle"))) dam += 3;
  if (IS_PLAYER(victim) && (!str_cmp(race_info[ch->pcdata->race].name, "minotaur") || !str_cmp(race_info[ch->pcdata->race].name, "gargoyle"))) dam /= 1;
  if (number_range(1,70) == 3) skill_gain(ch, gsn_grapple, TRUE);
  victim->hit -= dam;
  act ("$n claws at you, trying to get a better position.", ch, NULL, victim, TO_VICT);  
  act ("$n claws at $N, trying to get a superior position.", ch, NULL, victim, TO_NOTVICT_SPAM);  
  act ("You claw at $N, trying to get a better position.", ch, NULL, victim, TO_CHAR);  
  return;  
}


void 
fatality (CHAR_DATA * ch, CHAR_DATA * victim) 
{  
  short nummy; 
  nummy = number_range (1, 6); 
  switch(number_range(1,6))
    {
    case 1:
      act ("You shove your thumb into $N's eyes! You can feel $S brain as your thumb penetrates deep...", ch, NULL, victim, TO_CHAR);      
      act ("$n shoves $s thumb into $N's eyes! $N's body spasms and blood runs all over...", ch, NULL, victim, TO_NOTVICT);      
      act ("$n shoves $s thumb into your eye socket! The PAIN!", ch, NULL, victim, TO_VICT);      
      return;      
    case 2:
      act ("You grab $N's neck and twist! It snaps!", ch, NULL, victim, TO_CHAR);      
      act ("$n grabs $N's neck, breaking it in a mighty twist!", ch, NULL, victim, TO_NOTVICT);      
      act ("$n grabs your head! You feel your neck snapping...ARGH!", ch, NULL, victim, TO_VICT);      
      return;      
    case 3:
      act ("You smash a hard elbow into $N's back! You can feel $S spine break, and $S body stiffens.", ch, NULL, victim, TO_CHAR);      
      act ("$n smashes $s elbow into $N's spine!", ch, NULL, victim, TO_NOTVICT);      
      act ("You feel a painful elbow cracking down on your spine...", ch, NULL, victim, TO_VICT);
      return; 
    case 4:
      act ("Blood runs everywhere as you rip $N's neck with your teeth!", ch, NULL, victim, TO_CHAR);      
      act ("A fountain of blood flows as $n rips $N's throat apart with $s teeth! That's gotta hurt!", ch, NULL, victim, TO_NOTVICT);      
      act ("You feel $n's jaws close around your throat!", ch, NULL, victim, TO_VICT);      
      return;      
    case 5:
      act ("You lift $N's head and smash it hard against the ground! Blood and chunks of brain ooze out of $S broken skull.", ch, NULL, victim, TO_CHAR);      
      act ("$n smashes $N's head against the ground... Blood and bits of brain ooze everywhere as $N's cracked skull empties its contents on the floor.", ch, NULL, victim, TO_NOTVICT);  
      act ("$n smashes your head against the ground! Everything goes dark!", ch, NULL, victim, TO_VICT);      
      return;      
    case 6:
    default:
      if (strlen (NAME (victim)) < 8)	
	act ("You press your forearm down hard against $N's windpipe! $N's throat collapses, and blood trickles out of $N's mouth!", ch, NULL, victim, TO_CHAR);
      else	
	act ("You press your forearm down hard against $N's windpipe! $N's throat collapses, and blood trickles out of the corner of $N's mouth!", ch, NULL, victim, TO_CHAR);      
      act ("$n presses against $N's windpipe and blood trickles out $N's mouth and runs onto the ground.", ch, NULL, victim, TO_NOTVICT);      
      act ("$n presses against your throat; your vision blurs...", ch, NULL, victim, TO_VICT); 
      return;      
    }  
  return;
}


void 
ground_hit (CHAR_DATA * ch, CHAR_DATA * victim, int dt) 
{
  int num_options = 0;
  int hit_type = 0;
  int num_used = 0;
  bool break_choke = FALSE;
  bool done_hit = FALSE;
  if (victim->position == POSITION_DEAD)
    {
      stop_fighting (ch, TRUE);
      ch->wait = 0;
      stop_fighting (victim, TRUE);
    }
  else
    {
      if (victim->position != POSITION_GROUNDFIGHTING || ch->position != POSITION_GROUNDFIGHTING)
	{
	  NEW_POSITION(victim, POSITION_FIGHTING);
	  NEW_POSITION(ch, POSITION_FIGHTING);
	}
    }
  check_ced (victim);
  check_ced (ch);
  check_fgt (victim);
  check_fgt (ch);
  add_to_fighting_list (ch);
  add_to_fighting_list (victim);
  ch->fgt->fighting = victim;
  victim->fgt->fighting = ch;
  
  /* First check if ch is being choked...if so then you get the 
     "you are being choked" message Also it gives a chance to break 
     the choke. And then it goes off to the appropriate attack to show
     how you are being choked or attacking. */
  
  
  if (ch->ced->ground_pos == FP_CHOKE_1 || 
      ch->ced->ground_pos == FP_CHOKE_2 || 
      ch->ced->ground_pos == FP_CHOKE_3)
    {
      int break_chance;
      break_chance = 50 + 
	(get_curr_str(ch) + get_curr_dex(ch) + ch->height /5 + 
	 (IS_AUGMENTED(ch, AUG_ARM_POWER) ? 5 : 0) +
	 (IS_AUGMENTED(ch, AUG_BODY_POWER) ? 5 : 0) +
	 (IS_AUGMENTED(ch, AUG_HAND_POWER) ? 5 : 0) +
	 (IS_AUGMENTED(ch, AUG_HAND_SPD) ? 5 : 0))
	-
	(get_curr_str(victim) + get_curr_dex(victim) + victim->height /5 + 
	 (IS_AUGMENTED(victim, AUG_ARM_POWER) ? 5 : 0) +
	 (IS_AUGMENTED(victim, AUG_BODY_POWER) ? 5 : 0) +
	 (IS_AUGMENTED(victim, AUG_HAND_POWER) ? 5 : 0) +
	 (IS_AUGMENTED(victim, AUG_HAND_SPD) ? 5 : 0));
      
      
      /* Note that now you need to break choke 3 times in order to get out of it,   and it is harder to do so if you are a wuss. */
      
      if (number_range(1,100) < break_chance)
	{
	  if (ch->ced->ground_pos == FP_CHOKE_3)
	    {
	      if (number_range(1,2) == 2)
		break_choke = TRUE;
	    }
	  else if (ch->ced->ground_pos == FP_CHOKE_2)
	    ch->ced->ground_pos = FP_CHOKE_3;
	  else if (ch->ced->ground_pos == FP_CHOKE_1)
	    ch->ced->ground_pos = FP_CHOKE_2;
	}
      if (number_range (1, 2) == 1 && CHALLENGE (ch) == 10)
	{
	  char bufu[500];
	  sprintf (bufu, "($B$1Arena$N$7) $B%s is choking %s!", NAME (victim), NAME (ch));
	  do_arenaecho (ch, bufu, TRUE);
	}      
      act ("$N is still choking you! You can't BREATHE!!", ch, NULL, victim, TO_CHAR);      
      act ("$N still has a firm choke-hold on $n!", ch, NULL, victim, TO_NOTVICT);      
      act ("You continue strangling $n!", ch, NULL, victim, TO_VICT);      
    } 
  

   /* Otherwise if you are choking the victim, you mess them up.  If the
      victim is not being choked, but ch is, then we have the chance to break
      the choke. */
  
  if (victim->ced->ground_pos == FP_CHOKE_1)    
    {      
      victim->hit -= (get_curr_str(ch) + (IS_AUGMENTED(ch, AUG_FACE_SPIKE) ? 20 : 0) + ((IS_PLAYER(ch) && ch->pcdata->learned[gsn_grapple] > 0)  ?   ch->pcdata->learned[gsn_grapple]/8 : 0));
    }
  else    
    {      
      if (IS_PLAYER(ch))
	{
	  if (IS_SET(ch->ced->fight_ops, F_HEADBUTT )) num_options++;
	  if (IS_SET(ch->ced->fight_ops, F_JUJI )) num_options++;
	  if (IS_SET(ch->ced->fight_ops, F_ELBOW )) num_options++;
	  if (IS_SET(ch->ced->fight_ops, F_KNEE )) num_options++;
	  if (IS_SET(ch->ced->fight_ops, F_BITE )) num_options++;
	  if (IS_SET(ch->ced->fight_ops, F_CHOKE )) num_options++;
	  num_options++;
	  hit_type = number_range(1,(num_options));
	}
      else
	{
	  hit_type = number_range(1,7);
	}
      if (!done_hit && IS_SET(ch->ced->fight_ops, F_HEADBUTT))
	{
	  num_used++;
	  if (num_used == hit_type)
	    {
	       head_butt(ch, victim, break_choke);
	       done_hit = TRUE;
	    }
	}
      if (!done_hit && IS_SET (ch->ced->fight_ops, F_ELBOW))
	{
	  num_used++;
	  if (num_used == hit_type)
	    {
	      elbow (ch, victim, break_choke);
	      done_hit = TRUE;
	     }
	}
      if (!done_hit && IS_SET (ch->ced->fight_ops, F_BITE))
	{
	  num_used++;
	  if (num_used == hit_type)
	    {
	      bite (ch, victim, break_choke);
	      done_hit = TRUE;
	    }
	}
      if (!done_hit && IS_SET (ch->ced->fight_ops, F_JUJI))	
	{
	  num_used++;
	  if (num_used == hit_type) 
	     {
	       juji_gatame (ch, victim, break_choke);
	       done_hit = TRUE;
	     }
	}  
      
      if (!done_hit && IS_SET (ch->ced->fight_ops, F_KNEE))
	{
	  num_used++;
	  if (num_used == hit_type)
	    {
	       knee_stomach (ch, victim, break_choke);
	       done_hit = TRUE;
	    }
	}
      if(!done_hit && IS_SET(ch->ced->fight_ops, F_CHOKE))
	{
	  num_used++;
	  if ((num_used == hit_type) && (number_range(1,6) == 2 || (IS_PLAYER(ch) && (!str_cmp(race_info[ch->pcdata->race].name, "minotaur") || !str_cmp(race_info[ch->pcdata->race].name, "gargoyle")) && number_range(1,2) == 2)))
	    {
	      strangle(ch, victim, break_choke);
	      done_hit = TRUE;
	    }
	}
      if (!done_hit)
	get_better_pos(ch, victim, break_choke);
      
    }
  update_pos (victim);
  switch (victim->position)
    {
    case POSITION_MORTAL:
      act ("$n is mortally wounded, and will die soon if not aided.", 
	   victim, NULL, NULL, TO_ROOM);
      send_to_char (
		    "You are mortally wounded, and will die soon if not aided.\n\r", 
		    victim);
      break;      
    case POSITION_INCAP:
      act ("$n is incapacitated and will suffer a slow, painful death if not aided.",    victim, NULL, NULL, TO_ROOM);
      send_to_char (
		    "You are incapacitated and will suffer a slow, painful death if not aided.\n\r",  victim);
      break;
    case POSITION_STUNNED:      
      act ("$n is stunned, but will probably recover.", 
	   victim, NULL, NULL, TO_ROOM);      
      send_to_char ("You are stunned, but will probably recover.\n\r", 
		    victim);      
      break;      
    case POSITION_DEAD:      
      fatality (ch, victim);      
      send_to_char ("You have been KILLED!!\n\r\n\r", victim);      
      ch->wait = 0;      
      if (IS_PLAYER (victim))
	{         
	  victim->pcdata->condition[COND_FULL] = 48;	  
	  victim->pcdata->condition[COND_THIRST] = 48;	  
	}      
      break;      
    default:      
      break;     
    }
  if (victim->position == POSITION_DEAD)     
    {       
      if (IS_PLAYER(victim) && IN_BATTLE (victim) == TRUE) 
	{	   
	  stop_fighting (victim, TRUE);	  
	  NEW_POSITION(victim, POSITION_STANDING);
	  NEW_POSITION(ch, POSITION_STANDING);	   	   
	  MAXHIT(victim);	  
	  char_from_room (victim);	   
	  char_to_room (victim, get_room_index (victim->fgt->wasroomtwo));    
	  return;	   
	}      
      if (CHALLENGE (victim) != 0)	
	{	  
	  stop_fighting (victim, TRUE);	  
	  NEW_POSITION(victim, POSITION_STANDING);
	  NEW_POSITION(ch, POSITION_STANDING);	  
	  ch->hit = ch->max_hit;	  
	  victim->hit = victim->max_hit;	  
	  end_arena (ch, victim);	  
	  return;	  
	}      
      group_gain (ch, victim);      
      ch->wait = 0;      
      if (IS_PLAYER (victim))
	{	  
	  int ii; 
	  char buf[STD_LENGTH];
	  
 /* JRAJRA */
	  victim->pcdata->no_quit_pk = 0;	  
	  sprintf (log_buf, "%s killed by %s at %d", 
		    NAME (victim), 
		   NAME (ch), 
		   victim->in_room->vnum);	  
	  log_string (log_buf);	  
	  sprintf (log_buf, "%s just got killed by %s!", NAME (victim), NAME (ch));	  
	  /* ii = clan_number(ch);
	     if (ii > 0)
	     {
	     CLAN_DATA *clan;
	     if ((clan = get_clan_index(ii)) != NULL && clan->motto[0] !='\0' && clan->motto[0] != "")
	     {
	     strcat(log_buf, " { ");
	     strcat(log_buf, clan->motto);
	     strcat(log_buf, " } ");
	     }
	     }*/
	  ii = clan_number (victim);	  
	  if (ii > 0)
	    clan_notify (log_buf, ii);	  
	  if (LEADER (victim) != NULL)
	    group_notify (log_buf, victim);	  
	  if (IS_PLAYER (ch))	    
	    {	      
	      sprintf (log_buf, "%s just killed %s!", NAME (ch), NAME (victim));
	      ii = clan_number (ch);	      
	      if (ii > 0)
		clan_notify (log_buf, ii);	      
	    }	  
	  sprintf (buf, "Notify> %s", log_buf);
	  NOTIFY (buf, LEVEL_IMMORTAL, WIZ_NOTIFY_DEATH);	  
	  if (IS_PLAYER (ch) && !IS_MOB (victim))	    
	    {
	      if (victim->pcdata->warpoints - (1 + (LEVEL (victim) / 10))
		  >= 0) 
		victim->pcdata->warpoints -= (1 + (LEVEL (victim) / 10));    
	    } 
	  if (IS_MOB (ch))
	    {	      
	      pc_death_penalty (ch, victim); 
	    }	  
	   else	    
	     gain_exp (victim, UMIN (-80000, -(LEVEL (victim) * 1100)));
	}
      if (IS_PLAYER(ch) && IS_PLAYER(victim) && DIFF_ALIGN(ch, victim))
        {
         
          strcpy(reasonfd, NAME(ch));
        } 
      raw_kill (victim, FALSE);      
       if (IS_PLAYER (ch) && IS_MOB (victim))	
	 {	  
	   int old_gold = 0; 
	   char buf[STD_LENGTH];	  
	   old_gold = tally_coins (ch);
	   if (IS_SET (ch->pcdata->act2, PLR_AUTOLOOT))
	     do_get (ch, "all corpse");	  
	   else if (IS_SET (ch->pcdata->act2, PLR_AUTOGOLD))
	     do_get (ch, "all.coins corpse");	  
	   if (IS_SET (ch->pcdata->act2, PLR_AUTOSAC))	    
	     do_sacrifice (ch, "corpse");	  
	   if (IS_SET (ch->pcdata->act2, PLR_AUTOSPLIT) 
	       &&tally_coins (ch) - old_gold > 1)	    
	     {	      
	       sprintf (buf, "%d", tally_coins (ch) - old_gold);	      
	       do_split (ch, buf);	      
	     }	  
	 }      
       ch->wait = 0;
       return;      
     }  
   if (victim == ch)    
     return; 
   if (IS_PLAYER (victim) && victim->desc == NULL)
     {
       if (number_range (0, victim->wait) == 0)
	 {
	   do_flee (victim, "");
	   return;
	 }
     }
   return;
}


bool can_groundfight (CHAR_DATA * ch) 
{
    if (IS_PLAYER (ch))
    return TRUE;
    if (ch->pIndexData->mobtype == MOB_HUMAN 
	||ch->pIndexData->mobtype == MOB_DWARF 
	||ch->pIndexData->mobtype == MOB_ELF 
	||ch->pIndexData->mobtype == MOB_GNOME 
	||ch->pIndexData->mobtype == MOB_GIANT 
	||ch->pIndexData->mobtype == MOB_ORC 
	||ch->pIndexData->mobtype == MOB_TROLL
	||ch->pIndexData->mobtype == MOB_OGRE 
	||ch->pIndexData->mobtype == MOB_SPRITE 
	||ch->pIndexData->mobtype == MOB_DRACONIAN
	||ch->pIndexData->mobtype == MOB_HOBGOBLIN
	||ch->pIndexData->mobtype == MOB_MINOTAUR
	||ch->pIndexData->mobtype == MOB_KENDER
	||ch->pIndexData->mobtype == MOB_SKAVEN
	||ch->pIndexData->mobtype == MOB_ILLITHID
	||ch->pIndexData->mobtype == MOB_TROGLODYTE
	||ch->pIndexData->mobtype == MOB_GNOLL
	||ch->pIndexData->mobtype == MOB_HOBBIT)
    return TRUE;
    return FALSE;
}


