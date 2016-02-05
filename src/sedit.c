/*Spell editor */
#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emlen.h"
#define KEY( literal, field, value ) if ( !str_cmp( buf, literal ) ) {field=value; fMatch=TRUE;}

static bool prereq_count[SKILL_COUNT];
/* This function places all spells in the correct hashed spell list by
   name spots... called whenever a spell name is changed :) */



void 
update_prereq (void)
{
  SPELL_DATA *spell;
  SPELL_DATA *prereq;
  int i, j, counter;
  
  /* The idea here is to set up something so that its alot faster
     doing the bigass prereq trees. */

  for (i = 0; i < SPELL_HASH; i++)
    {
      for (spell = spell_list[i]; spell != NULL; spell = spell->next)
	{
	  
	  if ((prereq = skill_lookup(spell->prereq_1, -1)) != NULL)
	    {
	      spell->pre1 = prereq;
	    }
	  if ((prereq = skill_lookup(spell->prereq_2, -1)) != NULL)
	    {
	      spell->pre2 = prereq;
	    }
	  
	}
    }

  /* This is so that we dont have to recalc the number of prereqs for
     any given skill each time it asks for it. */

  for (i = 0; i < SPELL_HASH; i++)
    {
      for (spell = spell_list[i]; spell != NULL; spell = spell->next)
	{
	  for (j = 0; j < SKILL_COUNT; j++)
	    {
	      prereq_count[j] = FALSE;
	    }
	  check_prereq(spell);
	  counter = 0;
	  for (j = 0; j < SKILL_COUNT; j++)
	    {
	      if (prereq_count[j] == TRUE)
		{
		  counter++;
		}
	    }
	  spell->num_prereqs = counter;

        /* Now we set up the check for whether or not we make a vert line
           to the second prereq in the spell tree. */
          if(spell->pre1 && spell->pre2 && spell->pre1 != spell->pre2)
             {
                spell->go_to_second = check_prereq_depth(spell,spell->pre2);
             }
           else
            spell->go_to_second = FALSE;
	}
    }
  return;
  
}

void check_prereq(SPELL_DATA *spell)
{
  prereq_count[spell->gsn] = TRUE;
  if (spell->pre1 != NULL && !prereq_count[spell->pre1->gsn])
    check_prereq(spell->pre1);
  if (spell->pre2 != NULL && !prereq_count[spell->pre2->gsn])
    check_prereq(spell->pre2);
  return;
}




void 
update_name_hash (void)
{
  SPELL_DATA *spell;
  int i;
  bzero (spell_list_2, sizeof (spell_list_2));
  for (i = 0; i < SPELL_HASH; i++)
    {
      for (spell = spell_list[i]; spell != NULL; spell = spell->next)
	{
	  spell->next_a = spell_list_2[UPPER (spell->spell_funky_name[0]) % SPELL_HASH];
	  spell_list_2[UPPER (spell->spell_funky_name[0]) % SPELL_HASH] = spell;
	}
    }
  return;
}

/* This function is called when gsn's are changed... for instance,
   right at bootup when it assigns a high gsn to a skill... */
void 
update_gsn_hash (void)
{
  SPELL_DATA *spell;
  int i;
  bzero (spell_list, sizeof (spell_list));
  for (i = 0; i < SPELL_HASH; i++)
    {
      for (spell = spell_list_2[i]; spell != NULL; spell = spell->next_a)
	{
	  spell->next = spell_list[spell->gsn % SPELL_HASH];
	  spell_list[spell->gsn % SPELL_HASH] = spell;
	}
    }
  return;
}

bool 
is_digit (char test)
{
  if (test >= '0' && test <= '9')
    return TRUE;
  else
    return FALSE;
}

/* SPELLS/SKILLS are now hashed by name and spell gsn... fast lookups */
SPELL_DATA *
skill_lookup (char *txt, int sn)
{
  SPELL_DATA *spell;
  if (sn < 0 && !txt)
    return NULL;
  if (sn < 0)
    {
      for (spell = spell_list_2[UPPER (txt[0]) % SPELL_HASH]; spell != NULL; spell = spell->next_a)
	{
	  if (!str_prefix (txt, spell->spell_name) || !str_prefix (txt, spell->spell_funky_name))
	    return spell;
	}
      return NULL;
    }
  else
    for (spell = spell_list[sn % SPELL_HASH]; spell != NULL; spell = spell->next)
      {
	if (spell->gsn == sn)
	  return spell;
      }
  return NULL;
}

void 
fix_skillst (void)
{
  gsn_invest = lookup_gsn ("Invest");
  gsn_patch = lookup_gsn ("Patch");
  gsn_detrap = lookup_gsn ("Detrap");
  gsn_locate_trap = lookup_gsn ("Locate Traps");
  gsn_awareness = lookup_gsn ("Alertness");
  gsn_block = lookup_gsn ("Block");
  gsn_mine = lookup_gsn ("Mine");
  gsn_forge = lookup_gsn ("Forge");
  gsn_weave = lookup_gsn ("Weave");
  gsn_stitch = lookup_gsn ("Stitch");
  gsn_lore = lookup_gsn ("Lore");
  gsn_brew = lookup_gsn ("Brew"); 
  gsn_enchant = lookup_gsn ("Enchant");
  gsn_chop = lookup_gsn ("Chop");
  gsn_shoot = lookup_gsn ("Shoot"); 
  gsn_ranged = lookup_gsn ("Ranged");
  gsn_build = lookup_gsn ("Build");
  gsn_throw = lookup_gsn ("Throw"); 
  gsn_critical = lookup_gsn ("Critical");
  gsn_skin = lookup_gsn ("Skin");
  gsn_tanning = lookup_gsn ("Tanning");
  gsn_riding = lookup_gsn ("Riding");
  gsn_flail = lookup_gsn ("Flail");
  gsn_warcry = lookup_gsn ("Warcry");
  gsn_circle = lookup_gsn ("Circle");
  gsn_penetration = lookup_gsn ("Armor Penetration");
  gsn_grapple = lookup_gsn ("Grapple");
  gsn_trapset = lookup_gsn ("Trapset");
  gsn_swim = lookup_gsn ("Swim");
  gsn_butcher = lookup_gsn ("Butcher");
  gsn_backstab = lookup_gsn ("Backstab");
  gsn_dodge = lookup_gsn ("Dodge");
  gsn_evade = lookup_gsn ("Evade");
  gsn_hide = lookup_gsn ("Hide");
  gsn_peek = lookup_gsn ("Peek");
  gsn_pick_lock = lookup_gsn ("Lock");
  gsn_sneak = lookup_gsn ("Sneak");
  gsn_steal = lookup_gsn ("Steal");
  gsn_bash = lookup_gsn ("Bash");
  gsn_track = lookup_gsn ("Track"); 
  gsn_disarm = lookup_gsn ("Disarm");
  gsn_enhanced_damage = lookup_gsn ("Enhanced Dam");
  gsn_accuracy = lookup_gsn ("Accuracy");
  gsn_resilience = lookup_gsn ("Resilience");
  gsn_kick = lookup_gsn ("Kick");
  gsn_tornado = lookup_gsn ("Tornado Kick");
  gsn_impale = lookup_gsn ("Impale");
  gsn_charge = lookup_gsn ("Charge");
  gsn_double_kick = lookup_gsn ("Double Kick");
  gsn_smash = lookup_gsn ("Smash");
  gsn_tackle = lookup_gsn ("Tackle");
  gsn_flurry = lookup_gsn ("Flurry");
  gsn_flash = lookup_gsn ("Flash");
  gsn_parry = lookup_gsn ("Parry");
  gsn_blink = lookup_gsn ("Blink");
  gsn_guard = lookup_gsn ("Guard");
  gsn_steadiness = lookup_gsn ("Steadiness");
  gsn_shield_block = lookup_gsn ("Shield Block");
  gsn_rescue = lookup_gsn ("Rescue");
  gsn_second_attack = lookup_gsn ("Second Attack");
  gsn_third_attack = lookup_gsn ("Third Attack");
  gsn_dual_wield = lookup_gsn ("Dual Wield");
  gsn_pierce = lookup_gsn ("Pierc");
  gsn_unarmed = lookup_gsn ("Unarm");
  gsn_buddha = lookup_gsn ("Buddhas Palm");
  gsn_nerve = lookup_gsn ("Nerve Strike");
  gsn_spiritregen = lookup_gsn ("Spirit Regeneration");
  gsn_slashing = lookup_gsn ("Slash");
  gsn_concussion = lookup_gsn ("Concus");
  gsn_polearm = lookup_gsn ("Polearm");
  gsn_whip = lookup_gsn ("Whip");
  gsn_search = lookup_gsn ("Search");
  gsn_sixth_sense = lookup_gsn("Sixth Sense");
  update_name_hash ();
  update_gsn_hash ();
  update_prereq ();
  return;
}

void 
write_spells (void)
{
  SPELL_DATA *spell;
  FILE *sfp;
  int i;
  if ((spell = skill_lookup ("Slashing", -1)) == NULL || spell->spell_level == -1)
    setup_skills ();
  update_name_hash ();
  update_gsn_hash ();
  update_prereq();
  if ((sfp = fopen ("spells.dat", "w")) == NULL)
    {
      fprintf (stderr, "Shit, failed on spell write!!\n");
      exit (99);
    }
  for (i = 0; i < SPELL_HASH; i++)
    {
      for (spell = spell_list[i]; spell != NULL; spell = spell->next)
	{
	  fprintf (sfp, "#SPELL\n");
	  fprintf (sfp, "Slot %d\n", spell->slot);
	  fprintf (sfp, "Gsn %d\n", spell->gsn);
	  if (spell->slot == 1)
	    fprintf (sfp, "SType %d\n", spell->spell_type);
	  if (spell->spell_bits != 0)
	    fprintf (sfp, "SBits %d\n", spell->spell_bits);
	  if (spell->spell_bits2 != 0)
	    fprintf (sfp, "SBitT %d\n", spell->spell_bits2);
	  if (spell->bitvector2 != 0)
	    fprintf (sfp, "BitT %d\n", spell->bitvector2);
	  
	  fprintf (sfp, "Sname %s~\n", spell->spell_name);
	  fprintf (sfp, "SFname %s~\n", spell->spell_funky_name);
	  fprintf (sfp, "SLev %d\n", spell->spell_level);
	  if (spell->prereq_1 != NULL && str_cmp(spell->prereq_1, "none") && str_cmp(spell->prereq_1, "None"))
	    fprintf (sfp, "Pr1 %s~\n", spell->prereq_1);
	  if (spell->prereq_2 != NULL &&str_cmp(spell->prereq_2, "none") && str_cmp(spell->prereq_2, "None"))
	    fprintf (sfp, "Pr2 %s~\n", spell->prereq_2);
	    fprintf (sfp, "Noun %s~\n", spell->noun_damage);
	  if (spell->act_to_ch_1 != NULL)
	    fprintf (sfp, "A2C1 %s~\n", spell->act_to_ch_1);
	  if (spell->act_to_ch_2 != NULL)
	    fprintf (sfp, "A2C2 %s~\n", spell->act_to_ch_2);
	  if (spell->act_to_vict_1 != NULL)
	    fprintf (sfp, "A2V1 %s~\n", spell->act_to_vict_1);
	  if (spell->act_to_vict_2 != NULL)
	    fprintf (sfp, "A2V2 %s~\n", spell->act_to_vict_2);
	  if (spell->act_to_notvict_1 != NULL)
	    fprintf (sfp, "A2NV1 %s~\n", spell->act_to_notvict_1);
	  if (spell->act_to_notvict_2 != NULL)
	    fprintf (sfp, "A2NV2 %s~\n", spell->act_to_notvict_2);
	  
	  if (spell->linked_to != NULL)
	    {
	      fprintf (sfp, "Linkd %s~\n", spell->linked_to);
	    }
	  fprintf (sfp, "Durat %s~\n", (spell->duration == NULL ? "0" : spell->duration));
	  if (spell->duration != NULL)
	    {
	      int jj;
	      for (jj = 0; jj < 5; jj++)
		{
		  if(spell->location[jj] > 0)
		    fprintf(sfp, "Locat%d %d\n", jj, spell->location[jj]);
		  if(spell->modifier[jj] != NULL && str_cmp(spell->modifier[jj], "0"))
		    fprintf(sfp, "Modif%d  %s~\n", jj, spell->modifier[jj]);
		}
	    }
	  if (spell->bitvector != 0)
	    fprintf (sfp, "BitVe %d\n", spell->bitvector);
	  if (spell->damage != NULL)
	    fprintf (sfp, "Dam %s~\n", spell->damage);
	  if (spell->mana > 1)
	    fprintf (sfp, "Mana %d\n", spell->mana);
	  if (spell->mana_type > 0)
	    fprintf (sfp, "ManaT %d\n", spell->mana_type);
	  if (spell->position != POSITION_RESTING)
	    fprintf (sfp, "Pos %d\n", spell->position);
	  fprintf (sfp, "CastT %d\n", spell->casting_time);
	    fprintf (sfp, "WearO %s~\n", spell->wear_off_msg);
	  if (spell->creates_obj != 0)
	    fprintf (sfp, "CrObj %d\n", spell->creates_obj);
	  if (spell->min_int != 0)
	    fprintf (sfp, "Minint %d\n", spell->min_int);
	  if (spell->min_wis != 0)
	    fprintf (sfp, "Minwis %d\n", spell->min_wis);
	  if (spell->guilds != 0)
	    fprintf (sfp, "Guilds %d\n", spell->guilds);
	  if(spell->in_room_good != 0)
	    fprintf (sfp, "InRoomGd %d\n", spell->in_room_good);
	  if(spell->in_room_evil != 0)
	    fprintf (sfp, "InRoomEv %d\n", spell->in_room_evil);
	  if(spell->spell_lag != 0)
	    fprintf (sfp, "SplLag %d\n", spell->spell_lag);
	  if(spell->fail_chance != 0)
	    fprintf (sfp, "FailChan %d\n", spell->fail_chance);
	  if(spell->pk_fail_chance != 0)
	    fprintf (sfp, "PKChan %d\n", spell->pk_fail_chance);
	}  
      
    }
  fprintf (sfp, "#END");
  fclose (sfp);
  return;
}

void 
load_spells (void)
{
  SPELL_DATA *spell = NULL;
  FILE *sfp;
  char buf[500];
  bool fMatch;
  if ((sfp = fopen ("spells.dat", "r")) == NULL)
    {
      write_spells ();
      return;
    }
  for (;;)
    {
      strcpy (buf, fread_word (sfp));
      fMatch = FALSE;
      if (!str_cmp ("END", buf) || !str_cmp ("#END", buf))
	break;
      if (!str_cmp (buf, "SPELL") || !str_cmp ("#SPELL", buf))
	{
	  spell = new_spell ();
	  free_string (spell->spell_name);
	  free_string (spell->spell_funky_name);
	  free_string (spell->noun_damage);
	  free_string (spell->wear_off_msg);
	  continue;
	}
      if (spell == NULL)
	{
	  fprintf (stderr, "BAD FIRST HEADER IN SPELLS.DAT - %s!\n", buf);
	  exit (99);
	}
      KEY ("Slot", spell->slot, fread_number (sfp));
      KEY ("SType", spell->spell_type, fread_number (sfp));
      KEY ("SBits", spell->spell_bits, fread_number (sfp));
      KEY ("SBitT", spell->spell_bits2, fread_number (sfp));
      KEY ("BitT", spell->bitvector2, fread_number (sfp));
      KEY ("Gsn", spell->gsn, fread_number (sfp));
      KEY ("Sname", spell->spell_name, fread_string (sfp, NULL));
      KEY ("SFname", spell->spell_funky_name, fread_string (sfp, NULL));
      KEY ("SLev", spell->spell_level, fread_number (sfp));
      KEY ("Pr1", spell->prereq_1, fread_string (sfp, NULL));
      KEY ("Pr2", spell->prereq_2, fread_string (sfp, NULL));
      KEY ("Noun", spell->noun_damage, fread_string (sfp, NULL));
      KEY ("A2C1", spell->act_to_ch_1, fread_string (sfp, NULL));
      KEY ("A2C2", spell->act_to_ch_2, fread_string (sfp, NULL));
      KEY ("A2V1", spell->act_to_vict_1, fread_string (sfp, NULL));
      KEY ("A2V2", spell->act_to_vict_2, fread_string (sfp, NULL));
      KEY ("A2NV1", spell->act_to_notvict_1, fread_string (sfp, NULL));
      KEY ("A2NV2", spell->act_to_notvict_2, fread_string (sfp, NULL));
     
      KEY ("Linkd", spell->linked_to, fread_string (sfp, NULL));
      KEY ("Durat", spell->duration, fread_string (sfp, NULL));
     KEY ("Modif0", spell->modifier[0], fread_string (sfp, NULL));
      KEY ("Locat0", spell->location[0], fread_number (sfp)); 
      KEY ("Modif1", spell->modifier[1], fread_string (sfp, NULL));
      KEY ("Locat1", spell->location[1], fread_number (sfp));
      KEY ("Modif2", spell->modifier[2], fread_string (sfp, NULL));
      KEY ("Locat2", spell->location[2], fread_number (sfp));
      KEY ("Modif3", spell->modifier[3], fread_string (sfp, NULL));
      KEY ("Locat3", spell->location[3], fread_number (sfp));
      KEY ("Modif4", spell->modifier[4], fread_string (sfp, NULL));
      KEY ("Locat4", spell->location[4], fread_number (sfp));
      KEY ("BitVe", spell->bitvector, fread_number (sfp));
      KEY ("Dam", spell->damage, fread_string (sfp, NULL));
      KEY ("Mana", spell->mana, fread_number (sfp));
      KEY ("ManaT", spell->mana_type, fread_number (sfp));
      KEY ("CastT", spell->casting_time, fread_number (sfp));
      KEY ("Pos", spell->position, fread_number (sfp));
      KEY ("WearO", spell->wear_off_msg, fread_string (sfp, NULL));
      KEY ("CrObj", spell->creates_obj, fread_number (sfp));
      KEY ("Minint", spell->min_int, fread_number (sfp));
      KEY ("Minwis", spell->min_wis, fread_number (sfp));
      KEY ("Guilds", spell->guilds, fread_number (sfp));
      KEY ("InRoomGd", spell->in_room_good, fread_number (sfp));
      KEY ("InRoomEv", spell->in_room_evil, fread_number (sfp));
      KEY ("SplLag", spell->spell_lag, fread_number (sfp));
      KEY ("FailChan", spell->fail_chance, fread_number (sfp));
      KEY ("PKChan", spell->pk_fail_chance, fread_number (sfp));
      
      if (!fMatch)
	{
	  fprintf (stderr, "%s\n", buf);
	  bug ("Fread_spell: no match.", 0);
	  if (!feof (sfp))
	    fread_to_eol (sfp);
	}
      if (spell->gsn > 340 && spell->gsn < 400)
	spell->slot = 2;
      if (spell->gsn > 500)
	spell->slot = 0;
     
  
}
  fclose (sfp);
  update_name_hash ();
  update_gsn_hash ();
  fix_skillst ();
  update_name_hash ();
  update_gsn_hash ();
  update_prereq();
  return;
}

int 
lookup_gsn (char *nm)
{
  SPELL_DATA *spell = NULL;
  spell = skill_lookup (nm, -1);
  if (spell)
    return spell->gsn;
  else
    return -1;
}

int 
tally_spsk (CHAR_DATA * ch, int slot)
{
  SPELL_DATA *spell;
  int tally = 0;
  int i, j, low_gsn, high_gsn;
  if (IS_MOB (ch))
    return 0;
  if (slot < 0 || slot > 2) 
    return 0;
  for (j = 0; j < 3; j++)
    {
      ch->pcdata->learned[race_info[ch->pcdata->race].nat_skills[j]] = -100;
      ch->pcdata->learned[align_info[ch->pcdata->alignment].nat_skills[j]] = -100;
    }
  if (slot == 0)
    {
      low_gsn = 549;
      high_gsn = SKILL_COUNT;
    }
  else if (slot == 1)
    {
      low_gsn = 1;
      high_gsn = 200;
    }
  else if (slot == 2)
    {
      low_gsn = 349;
      high_gsn = 400;
    }
  for (i = low_gsn; i < high_gsn; i++)
    {
      if (ch->pcdata->learned[i] > 0)
	{
          tally++;
        }
    }
  for (j = 0; j < 3; j++)
    {
      ch->pcdata->learned[race_info[ch->pcdata->race].nat_skills[j]] = 100;
      ch->pcdata->learned[align_info[ch->pcdata->alignment].nat_skills[j]] = 100;
    }

  return tally;
}


void 
setup_skills (void)
{
  SPELL_DATA *spell;

  spell = new_spell ();
  spell->slot = 0;
  spell->gsn = 549;
  gsn_charge = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Charge");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Charge");
  free_string (spell->noun_damage);
  spell->noun_damage = str_dup ("charge");

  spell = new_spell ();
  spell->slot = 0;
  spell->gsn = 550;
  gsn_circle = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Circle");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Circle");
  free_string (spell->noun_damage);
  spell->noun_damage = str_dup ("circle");

  spell = new_spell ();
  spell->slot = 0;
  spell->gsn = 551;
  gsn_penetration = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Armor Penetration");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Armor Penetration");
  spell = new_spell ();
  spell->slot = 0;
  spell->gsn = 552;
  gsn_block = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Block");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Block");
  spell = new_spell ();
  spell->slot = 2;
  spell->gsn = 352;
  gsn_swim = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Swim");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Swim");
  spell = new_spell ();
  spell->slot = 2;
  spell->gsn = 350;
  gsn_patch = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Patch");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Patch");
  spell = new_spell ();
  spell->slot = 2;
  spell->gsn = 351;
  gsn_detrap = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Detrap");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Detrap");
  spell = new_spell ();
  spell->slot = 2;
  spell->gsn = 368;
  gsn_stitch = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Stitch");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Stitch");
  spell = new_spell ();
  spell->slot = 2;
  spell->gsn = 367;
  gsn_weave = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Weave");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Weave");
  spell = new_spell ();
  spell->slot = 2;
  spell->gsn = 366;
  gsn_tanning = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Tanning");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Tanning");
  spell = new_spell ();
  spell->slot = 2;
  spell->gsn = 365;
  gsn_skin = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Skin");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Skin");
  spell = new_spell ();
  spell->slot = 2;
  spell->gsn = 364;
  gsn_mine = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Mine");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Mine");
  spell = new_spell ();
  spell->slot = 2;
  spell->gsn = 353;
  gsn_butcher = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Butcher");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Butcher");
  spell = new_spell ();
  spell->slot = 2;
  spell->gsn = 370;
  gsn_chop = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Chop");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Chop");
  spell = new_spell ();
  spell->slot = 2;
  spell->gsn = 371;
  gsn_build = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Build");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Build");
  spell = new_spell ();
  spell->slot = 2;
  spell->gsn = 354;
  gsn_locate_trap = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Locate Traps");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Locate Traps");
  spell = new_spell ();
  spell->slot = 2;
  spell->gsn = 585;
  gsn_awareness = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Alertness");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Alertness");
  spell->gsn = 554;
  gsn_backstab = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Backstab");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Backstab");
  free_string (spell->noun_damage);
  spell->noun_damage = str_dup ("backstab");
  spell = new_spell ();
  spell->slot = 0;
  spell->gsn = 555;
  gsn_dodge = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Dodge");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Dodge");
  spell = new_spell ();
  spell->slot = 0;
  spell->gsn = 560;
  gsn_evade = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Evade");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Evade");
  spell = new_spell ();
  spell->slot = 0;
  spell->gsn = 553;
  gsn_guard = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Guard");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Guard");
  spell = new_spell ();
  spell->slot = 2;
  spell->gsn = 356;
  gsn_hide = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Hide");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Hide");
  spell = new_spell ();
  spell->slot = 2;
  spell->gsn = 357;
  gsn_peek = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Peek");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Peek");
  spell = new_spell ();
  spell->slot = 2;
  spell->gsn = 358;
  gsn_pick_lock = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Locksmithy");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Locksmithy");
  spell = new_spell ();
  spell->slot = 2;
  spell->gsn = 359;
  gsn_sneak = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Sneaking");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Sneaking");
  spell = new_spell ();
  spell->slot = 2;
  spell->gsn = 360;
  gsn_steal = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Steal");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Steal");
  spell = new_spell ();
  spell->slot = 0;
  spell->gsn = 561;
  gsn_bash = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Bash");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Bash");
  free_string (spell->noun_damage);
  spell->noun_damage = str_dup ("bash");
  spell = new_spell ();
  spell->slot = 2;
  spell->gsn = 362;
  gsn_track = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Tracking");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Tracking");
  spell = new_spell ();
  spell->slot = 2;
  spell->gsn = 361;
  gsn_forge = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Forge");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Forge");
  spell = new_spell ();
  spell->slot = 2;
  spell->gsn = 363;
  gsn_brew = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Brew");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Brew");
  spell = new_spell ();
  spell->slot = 2;
  spell->gsn = 371;
  gsn_enchant = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Enchant");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Enchant");
  spell = new_spell ();
  spell->slot = 2;
  spell->gsn = 372;
  gsn_brew = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Lore");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Lore");
  spell = new_spell ();
  spell->slot = 0;
  spell->gsn = 563;
  gsn_disarm = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Disarm");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Disarm");
  spell = new_spell ();
  spell->slot = 0;
  spell->gsn = 564;
  gsn_enhanced_damage = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Enhanced Damage");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Enhanced Damage");
  spell = new_spell ();
  spell->slot = 0;
  spell->gsn = 565;
  gsn_kick = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Kick");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Kick");
  free_string (spell->noun_damage);
  spell->noun_damage = str_dup ("kick");
  spell = new_spell ();
  spell->slot = 0;
  spell->gsn = 559;
  gsn_tornado = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Tornado Kick");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Tornado Kick");
  free_string (spell->noun_damage);
  spell->noun_damage = str_dup ("kick");
  spell = new_spell ();
  spell->slot = 0;
  spell->gsn = 588;
  gsn_impale = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Impale");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Impale");
  free_string (spell->noun_damage);
  spell->noun_damage = str_dup ("impale");
  spell = new_spell ();
  spell->slot = 0;
  spell->gsn = 590;
  gsn_tackle = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Tackle");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Tackle");
  free_string (spell->noun_damage);
  spell->noun_damage = str_dup ("tackle");
  spell = new_spell ();
  spell->slot = 0;
  spell->gsn = 591;
  gsn_flurry = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Flurry");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Flurry");
  free_string (spell->noun_damage);
  spell->noun_damage = str_dup ("flurry");
  spell = new_spell ();
  spell->slot = 0;
  spell->gsn = 589;
  gsn_flash = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Flash");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Flash");
  free_string (spell->noun_damage);
  spell->noun_damage = str_dup ("flash");
  spell = new_spell ();
  spell->slot = 0;
  spell->gsn = 566;
  gsn_parry = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Parry");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Parry");
  spell = new_spell ();
  spell->slot = 0;
  spell->gsn = 592;
  gsn_steadiness = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Steadiness");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Steadiness");
  spell = new_spell ();
  spell->slot = 0;
  spell->gsn = 593;
  gsn_double_kick = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Double Kick");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Double Kick");
  spell = new_spell ();
  spell->slot = 0;
  spell->gsn = 594;
  gsn_smash = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Smash");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Smash");
  spell = new_spell ();
  spell->slot = 0;
  spell->gsn = 595;
  gsn_accuracy = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Accuracy");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Accuracy");
  spell = new_spell ();
  spell->slot = 0;
  spell->gsn = 596;
  gsn_resilience = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Resilience");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Resilience");
  spell = new_spell ();
  spell->slot = 2;
  spell->gsn = 397;
  gsn_sixth_sense = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Sixth Sense");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Sixth Sense");
  spell = new_spell ();
  spell->slot = 0;
  spell->gsn = 576;
  gsn_shoot = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Shoot");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Shoot");
  spell = new_spell ();
  spell->slot = 0;
  spell->gsn = 577;
  gsn_throw = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Throw");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Throw");
  spell = new_spell ();
  spell->slot = 0;
  spell->gsn = 567;
  gsn_shield_block = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Shield Block");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Shield Block");
  spell = new_spell ();
  spell->slot = 0;
  spell->gsn = 568;
  gsn_rescue = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Rescue");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Rescue");
  spell = new_spell ();
  spell->slot = 0;
  spell->gsn = 569;
  gsn_second_attack = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Second Attack");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Second Attack");
  spell = new_spell ();
  spell->slot = 0;
  spell->gsn = 570;
  gsn_third_attack = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Third Attack");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Third Attack");
  spell = new_spell ();
  spell->slot = 0;
  spell->gsn = 571;
  gsn_dual_wield = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Dual Wield");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Dual Wield");
  spell = new_spell ();
  spell->slot = 0;
  spell->gsn = 572;
  gsn_pierce = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Piercing Weapons");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Piercing Weapons");
  spell = new_spell ();
  spell->slot = 0;
  spell->gsn = 556;
  gsn_unarmed = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Unarmed Combat");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Unarmed Combat");
  spell = new_spell ();
  spell->slot = 0;
  spell->gsn = 557;
  gsn_spiritregen = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Spirit Regeneration");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Spirit Regeneration");
  spell = new_spell ();
  spell->slot = 0;
  spell->gsn = 558;
  gsn_buddha = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Buddhas Palm");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Buddhas Palm");
  spell = new_spell ();
  spell->slot = 0;
  spell->gsn = 561;
  gsn_nerve = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Nerve Strike");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Nerve Strike");
  spell = new_spell ();
  spell->slot = 0;
  spell->gsn = 573;
  gsn_slashing = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Slashing Weapons");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Slashing Weapons");
  spell = new_spell ();
  spell->slot = 0;
  spell->gsn = 574;
  gsn_concussion = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Concussion Weapons");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Concussion Weapons");
  spell = new_spell ();
  spell->slot = 0;
  spell->gsn = 575;
  gsn_whip = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Whipping Weapons");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Whipping Weapons");
  spell = new_spell ();
  spell->slot = 2;
  spell->gsn = 376;
  gsn_search = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Searching");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Searching");
  spell = new_spell ();
  spell->slot = 2;
  spell->gsn = 378;
  gsn_invest = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Investigation");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Investigation");
  spell = new_spell ();
  spell->slot = 2;
  spell->gsn = 377;
  gsn_riding = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Riding");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Riding");
  spell = new_spell ();
  spell->slot = 0;
  spell->gsn = 580;
  gsn_flail= spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Flail");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Flail");
  spell = new_spell ();
  spell->slot = 0;
  spell->gsn = 579;
  gsn_warcry = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Warcry");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Warcry");
  spell = new_spell ();
  spell->slot = 0;
  spell->gsn = 583;
  gsn_polearm = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Polearm Weapons");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Polearm Weapons");
  spell = new_spell ();
  spell->slot = 0;
  spell->gsn = 584;
  gsn_critical = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Critical Hit");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Critical Hit");
  spell = new_spell ();
  spell->slot = 0;
  spell->gsn = 599;
  gsn_grapple = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Grapple");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Grapple");
  spell = new_spell ();
  spell->slot = 2;
  spell->gsn = 398;
  gsn_grapple = spell->gsn;
  free_string (spell->spell_name);
  spell->spell_name = str_dup ("Trapset");
  free_string (spell->spell_funky_name);
  spell->spell_funky_name = str_dup ("Trapset");
  update_name_hash ();
  update_gsn_hash ();
  update_prereq();
  return;
}

char *
stpos (int pos)
{
  static char retb[25];
  retb[0] = '\0';
  if (pos == POSITION_STANDING)
    {
      strcpy (retb, "STANDING");
    }
  if (pos == POSITION_RESTING)
    {
      strcpy (retb, "RESTING");
    }
  if (pos == POSITION_FIGHTING)
    {
      strcpy (retb, "FIGHTING");
    }
  return retb;
}

int 
stpos_rev (char *pos)
{
  if (!str_cmp (pos, "standing"))
    return POSITION_STANDING;
  if (!str_cmp (pos, "resting"))
    return POSITION_RESTING;
  if (!str_cmp (pos, "fighting"))
    return POSITION_FIGHTING;
  return -99;
}

char *
mtype (int mana)
{
  static char retb[50];
  int i;
  bool found = FALSE;
  retb[0] = '\0';
  for (i = 0; str_cmp(mana_data[i].what_you_type, "end_of_list"); i++ )
    {
      if (mana & mana_data[i].flagname)
	{
	  strcat(retb, mana_data[i].how_it_appears);
	  found = TRUE;
	}
    }
  if (!found)
    {
      strcat(retb, "None");
    }
  
  return retb;
}




int 
mtype_rev (char *mana)
{
  int i;
  if (!str_cmp (mana, "none"))
    return MANA_NONE;
  for (i = 0; str_cmp(mana_data[i].what_you_type, "end_of_list"); i++ )
    {
      if (!str_cmp(mana, mana_data[i].what_you_type))
	return mana_data[i].flagname;
    }
  return -99;
}


int
spell_extra_bits2_rev (char *wrd)
{
 int i;
  for (i = 0; str_cmp(spell_flags2[i].what_you_type, "end_of_list"); i++)
    if (!str_cmp(wrd, spell_flags2[i].what_you_type))
      return spell_flags2[i].flagname; 
  return -99;
}


int
spell_extra_bits_rev (char *wrd)
{
  int i;
  for (i = 0; str_cmp(spell_flagss[i].what_you_type, "end_of_list"); i++)
    if (!str_cmp(wrd, spell_flagss[i].what_you_type))
      return spell_flagss[i].flagname;
  return -99;
}



char * 
spell_extra_bits2 (int bits)
{
   int i;
  static char retb[200];
  retb[0] = '\0';
  for (i = 0; str_cmp(spell_flags2[i].what_you_type, "end_of_list"); i ++)
    if (bits & spell_flags2[i].flagname)
      strcat(retb, spell_flags2[i].how_it_appears);
  return retb;
}

char *
spell_extra_bits (int bits)
{
  int i;
  static char retb[200];
  retb[0] = '\0';
  for (i = 0; str_cmp(spell_flagss[i].what_you_type, "end_of_list"); i ++)
    if (bits & spell_flagss[i].flagname)
      strcat(retb, spell_flagss[i].how_it_appears);
  return retb;
}

char *
styp (int stype)
{
  static char retb[20];
  retb[0] = '\0';
  if (stype == TAR_IGNORE) {
    strcpy (retb, "NONE.");
  }
  if (stype == TAR_CHAR_OFFENSIVE) {
    strcpy (retb, "OFFNS");
  }
  if (stype == TAR_CHAR_DEFENSIVE) {
    strcpy (retb, "DEFNS");
  }
  if (stype == TAR_CHAR_SELF) {
    strcpy (retb, "SELF");
  }
  if (stype == TAR_OBJ_INV){
    strcpy (retb, "OBJINV");
  }
  if (stype == TAR_OBJ_ROOM) {
    strcpy (retb, "OBJROOM");
  }
  return retb;
}

int 
styp_rev (char *stype)
{
  if (!str_cmp (stype, "none"))
    return TAR_IGNORE;
  if (!str_cmp (stype, "offensive"))
    return TAR_CHAR_OFFENSIVE;
  if (!str_cmp (stype, "defensive"))
    return TAR_CHAR_DEFENSIVE;
  if (!str_cmp (stype, "self"))
    return TAR_CHAR_SELF;
  if (!str_cmp (stype, "objinv"))
    return TAR_OBJ_INV;
  if (!str_cmp (stype, "objroom"))
    return TAR_OBJ_ROOM;
  return -99;
}




void show_gen_skill (CHAR_DATA * ch, SPELL_DATA * spell) {

  char buf[500];
//  sprintf (buf, "\x1B[1;1f\x1B[K\x1B[2;1f\x1B[K\x1B[3;1f;%dr", ch->pcdata->pagelen);
  sprintf (buf, "\x1B[1;1f\x1B[K\x1B[2;1f\x1B[K\x1B[3;1f\x1B[K\x1B[4;1f");
  send_to_char (buf, ch);
//  send_to_char ("\x1B[1;1f\x1B[K", ch);


  if (spell->gsn == gsn_backstab || spell->gsn == gsn_circle)
    sprintf (buf, "Skill %s #%d  Delay (Post): %d  Moves=(Pre-%s delay): %d\n\r",
	     spell->spell_name, spell->gsn, spell->casting_time, spell->spell_name, spell->mana);
  else
    sprintf (buf, "Skill %s (#%d).  Delay: %d.  Moves: %d.\n\r",
	   spell->spell_name, spell->gsn, spell->casting_time, spell->mana);

  send_to_char (buf, ch);

  sprintf (buf, "Level: %d  MStr: %d  MCon: %d  (Min str/con to practice this skill fully).\n\r",
	   spell->spell_level, spell->min_int, spell->min_wis);
  send_to_char (buf, ch);
  send_to_char ("\x1B[K----------------------------------------------------------------------------", ch);
  sprintf (buf, "\x1B[%d;1f", ch->pcdata->pagelen);
  if (spell->gsn > 500) spell->slot = 0;
  send_to_char (buf, ch);
  return;
}






void 
show_spell (CHAR_DATA * ch, SPELL_DATA * spell, int row)
{
  char buf[500];
  if (spell->gsn >= 500) {
      show_gen_skill (ch, spell);
      spell->slot = 0;
      return;
  }



/*
   Spel: 43/112 Mana: 38 Lvl: 7 MType: WATER Pos: STANDING Time: 8 Mint:
   Type: OFFNS damg: blast Name: Hands of Wind Name2: Hanskdlskjds Mwis:
   Pre1: Fireball Pre2: Joeblow Creates: 0 Val: 0 0 0 0 0 0 0 0 0 0
   Lctn: None Duration: 2df4343jf Modifier: 84yf8 Bit: AFF_POISON
   Damg: 84cfds2 Slot: 1 Wear_off: The spell has worn off.
   TO_CHAR1..... Your wind worked... wham! Right into $N!
   TO_CHAR2.....
   TO_VICT1..... WHAM! $n's wind knocks you down!
   TO_VICT2.....
   TO_NOTVICT1.. Ouch! $n's wind knocks $N down to the ground...
   TO_NOTVICT2.. 
   Extra_bits..:                               Linked:
 
   -----------------------------------------------------------------------------
 */
  if (row == -1)
    {
      sprintf (buf, "\x1B[1;1f\x1B[K\x1B[2;1f\x1B[K\x1B[3;1f\x1B[K\x1B[4;1f\x1B[K\x1B[5;1f\x1B[K\x1B[6;1f\x1B[K\x1B[7;1f\x1B[K\x1B[8;1f\x1B[K\x1B[9;1f\x1B[K\x1B[10;1f\x1B[K\x1B[11;1f\x1B[K\x1B[12;1f\x1B[K\x1B[13;1f\x1B[K\x1B[14;1f\x1B[K\x1B[15;1f\x1B[K\x1B[16;1f\x1B[17;%dr", ch->pcdata->pagelen);
      send_to_char (buf, ch);
    }
  if (row == -1 || row == 1)
    {
      send_to_char ("\x1B[1;1f\x1B[K", ch);
      sprintf (buf, "Spel: %d/%d Mana: %d Lvl: %d MType: %s Pos: %s Time: %d Mint: %d",
	       spell->gsn, top_spell, spell->mana, spell->spell_level, mtype (spell->mana_type),
	       stpos (spell->position), spell->casting_time, spell->min_int);
      send_to_char (buf, ch);
    }
  if (row == -1 || row == 2)
    {
      send_to_char ("\x1B[2;1f\x1B[K", ch);
      sprintf (buf, "Type: %s ndam: %s Name: %s Name2: %s Mwis: %d",
	       styp (spell->spell_type), (spell->noun_damage == NULL ? "N/S" : spell->noun_damage), spell->spell_name, spell->spell_funky_name, spell->min_wis);
      send_to_char (buf, ch);
    }
  if (row == -1 || row == 3)
    {
      send_to_char ("\x1B[3;1f\x1B[K", ch);
      sprintf (buf, "Pre1: %s Pre2: %s Creates: %d Lag: %d, Fail: %d PKFail: %d",
	       (spell->prereq_1 == NULL ? "None" : spell->prereq_1),
	       (spell->prereq_2 == NULL ? "None" : spell->prereq_2),
	       spell->creates_obj, spell->spell_lag, spell->fail_chance, spell->pk_fail_chance);
      send_to_char (buf, ch);
    }
  if (row == -1 || row == 4)
    {
      send_to_char ("\x1B[4;1f\x1B[K", ch);
      sprintf (buf, "Duration: %s Bit: %s Bit2: %s",
	       spell->duration,
	       affect_bit_name (spell->bitvector),
	       affect_bit_name_two (spell->bitvector2));
      send_to_char (buf, ch);
    }
  if (row == -1 || row == 5)
    {
      send_to_char ("\x1B[5;1f\x1B[K", ch);
      sprintf (buf, "Damg: %s Slot: %d Wear_off: %s",
	       (spell->damage == NULL ? "N/A" : spell->damage),
	       spell->slot, (spell->wear_off_msg == NULL ? "None" : spell->wear_off_msg));
      send_to_char (buf, ch);
    }
  if (row == -1 || row == 6)
    {
      send_to_char ("\x1B[6;1f\x1B[K", ch);
      sprintf (buf, "TO_CHAR1..... %s", (spell->act_to_ch_1 == NULL ? "" : spell->act_to_ch_1));
      send_to_char (buf, ch);
    }
  if (row == -1 || row == 7)
    {
      send_to_char ("\x1B[7;1f\x1B[K", ch);
      sprintf (buf, "TO_CHAR2..... %s", (spell->act_to_ch_2 == NULL ? "" : spell->act_to_ch_2));
      send_to_char (buf, ch);
    }
  if (row == -1 || row == 8)
    {
      send_to_char ("\x1B[8;1f\x1B[K", ch);
      sprintf (buf, "TO_VICT1..... %s", (spell->act_to_vict_1 == NULL ? "" : spell->act_to_vict_1));
      send_to_char (buf, ch);
    }
  if (row == -1 || row == 9)
    {
      send_to_char ("\x1B[9;1f\x1B[K", ch);
      sprintf (buf, "TO_VICT2..... %s", (spell->act_to_vict_2 == NULL ? "" : spell->act_to_vict_2));
      send_to_char (buf, ch);
    }
  if (row == -1 || row == 10)
    {
      send_to_char ("\x1B[10;1f\x1B[K", ch);
      sprintf (buf, "TO_NOTVICT1.. %s", (spell->act_to_notvict_1 == NULL ? "" : spell->act_to_notvict_1));
      send_to_char (buf, ch);
    }
  if (row == -1 || row == 11)
    {
      send_to_char ("\x1B[11;1f\x1B[K", ch);
      sprintf (buf, "TO_NOTVICT2.. %s", (spell->act_to_notvict_2 == NULL ? "" : spell->act_to_notvict_2));
      send_to_char (buf, ch);
    }
  if (row == -1 || row == 12)
    {
      send_to_char ("\x1B[12;1f\x1B[K", ch);
      sprintf (buf, "Extra_bits..: %s %s  Linked: %s", spell_extra_bits (spell->spell_bits), spell_extra_bits2 (spell->spell_bits2),
	       (spell->linked_to == NULL ? " " : spell->linked_to));
      send_to_char (buf, ch);
    }
  if (row == -1 || row == 13)
    {
      send_to_char ("\x1B[13;1f\x1B[K", ch);
      sprintf(buf, "Guild(s):");
      send_to_char(buf, ch);
      if (spell->guilds != 0)
	{
	  char buf[500];
	  int i=0;
	  bool prev= FALSE;
	  for (i = 0; str_cmp(guild_data[i].what_you_type, "end_of_list"); i++)
	    {
	      if (IS_SET(spell->guilds, guild_data[i].mob_guildmaster_bit))
		{
		  if (prev)
		    send_to_char(",", ch);
		  sprintf(buf, guild_data[i].what_you_see);
		  send_to_char(buf, ch);
		  prev = TRUE;
		}
	    }
	}
      else
	{
	  sprintf(buf, " None");
	  send_to_char(buf, ch);
	}
      sprintf(buf, ".\n\r");
      send_to_char(buf, ch);
    }
  if (row == -1 || row == 14)
    {
      int jj;
      send_to_char ("\x1B[14;1f\x1B[K", ch);
      sprintf(buf, "Lctns: ");
      for (jj = 0; jj < 5; jj++)
	{
	  char buff[100];
	  sprintf(buff, "%-13s ", affect_loc_name(spell->location[jj]));
	  strcat(buf, buff);
	}
      strcat(buf, "\n\r");
      send_to_char (buf, ch);
    }
  if (row == -1 || row == 15)
    {
      send_to_char ("\x1B[15;1f\x1B[K", ch);
      sprintf(buf, "Modif: %-13s %-13s %-13s %-13s %-13s", (spell->modifier[0] == NULL ? "None" : spell->modifier[0]),(spell->modifier[1] == NULL ? "None" : spell->modifier[1]),(spell->modifier[2] == NULL ? "None" : spell->modifier[2]),(spell->modifier[3]
 == NULL ? "None" : spell->modifier[3]),(spell->modifier[4] == NULL ? "None" : spell->modifier[4])),
	send_to_char (buf, ch);
    }
  if (row == -1)
    {
      send_to_char ("\x1B[17;1f\x1B[K----------------------------------------------------------------------------", ch);
    }
  sprintf (buf, "\x1B[%d;1f", ch->pcdata->pagelen);
  send_to_char (buf, ch);
  return;
}

void 
do_sedit (CHAR_DATA * ch, char *argy)
{
  char arg1[SML_LENGTH];
  int value;
  SPELL_DATA *spell;
  DEFINE_COMMAND ("sedit", do_sedit, POSITION_DEAD, MAX_LEVEL, LOG_NORMAL, "The spell editor... use with caution!")

  // umm no mobs sediting! bad mobs!
  if (IS_MOB (ch)) return;
  
  argy = one_argy (argy, arg1);
  if (is_number (arg1))
    {
      value = atoi (arg1);
      if ((spell = skill_lookup (NULL, value)) == NULL)
	{
	  send_to_char ("That spell number was not found.\n\r", ch);
	  return;
	}
     
      ch->desc->pEdit = (void *) spell;
      ch->desc->connected = CON_SEDITOR;
      show_spell (ch, spell, -1);
      return;
    }

  if (!str_cmp (arg1, "create"))
    {
      spell = new_spell ();
      changed_spells = TRUE;
      ch->desc->pEdit = (void *) spell;
      ch->desc->connected = CON_SEDITOR;
      update_name_hash ();
      update_gsn_hash ();
      show_spell (ch, spell, -1);
      return;
    }
  if (arg1[0] != '\0' && (spell = skill_lookup (arg1, -1)) != NULL)
    {
      ch->desc->pEdit = (void *) spell;
      ch->desc->connected = CON_SEDITOR;
      show_spell (ch, spell, -1);
      if (spell->gsn > 300)
	spell->slot = 2;
      if (spell->gsn > 500) 
        spell->slot = 0;
      return;
    }
  send_to_char ("Syntax: SEDIT #, SEDIT 'Spell_name', or SEDIT CREATE.\n\r", ch);
  return;
}

void 
sedit (CHAR_DATA * ch, char *argy)
{
  char *orarg = argy;
  char arg1[SML_LENGTH];
  char arg2[SML_LENGTH];
  bool fnn;
  SPELL_DATA *spell;
  spell = (SPELL_DATA *) ch->desc->pEdit;
  if (!spell)
    {
      interpret (ch, argy);
      return;
    }
  if (spell->gsn > 500) 
    spell->slot = 0;
  arg1[0] = '\0';
  arg2[0] = '\0';
  argy = one_argy (argy, arg1);
  if (arg1[0] == '\0')
    {
      show_spell (ch, spell, -1);
      return;
    }
  if (!str_cmp (arg1, "done"))
    {
      ch->desc->pEdit = NULL;
      ch->desc->connected = CON_PLAYING;
      sprintf (arg1, "\x1B[1;%dr\x1B[2J\x1B[%d;1f", ch->pcdata->pagelen, ch->pcdata->pagelen);
      send_to_char (arg1, ch);
      return;
    }
    
  if (!str_cmp (arg1, "name"))
    {
      if (argy == NULL || argy[0] == '\0' || argy == "")
	{
	  send_to_char ("Syntax: Name <spell_name>.\n\r", ch);
	  return;
	}
      free_string (spell->spell_name);
      spell->spell_name = str_dup (argy);
      changed_spells = TRUE;
      update_name_hash ();
      show_spell (ch, spell, 2);
      return;
    }
  if (!str_cmp (arg1, "name2"))
    {
      if (argy == NULL || argy[0] == '\0' || argy == "")
	{
	  send_to_char ("Syntax: Name2 <spell_name_2>.\n\r", ch);
	  return;
	}
      free_string (spell->spell_funky_name);
      spell->spell_funky_name = str_dup (argy);
      changed_spells = TRUE;
      show_spell (ch, spell, 2);
      return;
    }
  if (!str_cmp (arg1, "ndam"))
    {
      if (argy == NULL || argy[0] == '\0' || argy == "")
	{
	  send_to_char ("Syntax: Ndam <word>.\n\r", ch);
	  return;
	}
      if (spell->noun_damage != NULL)
	free_string (spell->noun_damage);
      spell->noun_damage = str_dup (argy);
      changed_spells = TRUE;
      show_spell (ch, spell, 2);
      return;
    }
  if (!str_cmp (arg1, "guild"))
    {
      int i;
      for (i = 0; str_cmp(guild_data[i].what_you_type, "end_of_list"); i++)
	{
	  if (!str_cmp(guild_data[i].what_you_type, argy))
	    {
	      char buf[100];
	      sprintf(buf, guild_data[i].what_you_see);
	      if (IS_SET(spell->guilds, guild_data[i].mob_guildmaster_bit))
		send_to_char("Guild Removed\n\r", ch);
	      else
		send_to_char("Guild Added\n\r", ch);
	      spell->guilds ^= guild_data[i].mob_guildmaster_bit;
	      changed_spells = TRUE;
	      show_spell (ch, spell, 13);
	      return;
	    }
	}
 
      send_to_char ("Unknown guild.\n\r", ch);
      return;
    }
  if (!str_prefix ("link", arg1))
    {
      if (argy == NULL || argy[0] == '\0' || argy == "")
	{
      	if (spell->linked_to != NULL)
	  free_string (spell->linked_to);
      spell->linked_to = NULL;
	  send_to_char ("Linked_to cleared.\n\r", ch);
	  return;
	}
      if (spell->linked_to != NULL)
	free_string (spell->linked_to);
      spell->linked_to = str_dup (argy);
      changed_spells = TRUE;
      show_spell (ch, spell, 12);
      return;
    }
  if (!str_cmp (arg1, "damg"))
    {
      if (argy == NULL || argy[0] == '\0' || argy == "")
	{
	  send_to_char ("Syntax: Damg <string>.\n\r", ch);
	  return;
	}
      if (spell->damage != NULL)
	free_string (spell->damage);
      spell->damage = str_dup (argy);
      changed_spells = TRUE;
      show_spell (ch, spell, 5);
      return;
    }
  
  if (!str_cmp (arg1, "wear_off"))
    {
      if (argy == NULL || argy[0] == '\0' || argy == "")
	{
	  send_to_char ("Syntax: wear_off <string>.\n\r", ch);
	  return;
	}
      if (spell->wear_off_msg != NULL)
	free_string (spell->wear_off_msg);
      spell->wear_off_msg = str_dup (argy);
      changed_spells = TRUE;
      show_spell (ch, spell, 5);
      return;
    }
  if (!str_cmp (arg1, "duration"))
    {
      if (argy == NULL || argy[0] == '\0' || argy == "")
	{
	  send_to_char ("Syntax: duration <duration val>.\n\r", ch);
	  return;
	}
      if (spell->duration != NULL)
	free_string (spell->duration);
      spell->duration = str_dup (argy);
      changed_spells = TRUE;
      show_spell (ch, spell, 4);
      return;
    }
  if (!str_cmp (arg1, "to_char1"))
    {
      if (argy == NULL || argy[0] == '\0' || argy == "")
	{
	  if (spell->act_to_ch_1 != NULL)
	    free_string (spell->act_to_ch_1);
	  spell->act_to_ch_1 = NULL;
	  return;
	}
      if (spell->act_to_ch_1 != NULL)
	free_string (spell->act_to_ch_1);
      spell->act_to_ch_1 = str_dup (argy);
      changed_spells = TRUE;
      show_spell (ch, spell, 6);
      return;
    }
  if (!str_cmp (arg1, "to_char2"))
    {
      if (argy == NULL || argy[0] == '\0' || argy == "")
	{
	  if (spell->act_to_ch_2 != NULL)
	    free_string (spell->act_to_ch_2);
	  spell->act_to_ch_2 = NULL;
	  return;
	}
      if (spell->act_to_ch_2 != NULL)
	free_string (spell->act_to_ch_2);
      changed_spells = TRUE;
      spell->act_to_ch_2 = str_dup (argy);
      show_spell (ch, spell, 7);
      return;
    }
  if (!str_cmp (arg1, "to_vict1"))
    {
      if (argy == NULL || argy[0] == '\0' || argy == "")
	{
	  if (spell->act_to_vict_1 != NULL)
	    free_string (spell->act_to_vict_1);
	  spell->act_to_vict_1 = NULL;
	  return;
	}
      if (spell->act_to_vict_1 != NULL)
	free_string (spell->act_to_vict_1);
      spell->act_to_vict_1 = str_dup (argy);
      changed_spells = TRUE;
      show_spell (ch, spell, 8);
      return;
    }
  if (!str_cmp (arg1, "to_vict2"))
    {
      if (argy == NULL || argy[0] == '\0' || argy == "")
	{
	  if (spell->act_to_vict_2 != NULL)
	    free_string (spell->act_to_vict_2);
	  spell->act_to_vict_2 = NULL;
	  return;
	}
      if (spell->act_to_vict_2 != NULL)
	free_string (spell->act_to_vict_2);
      spell->act_to_vict_2 = str_dup (argy);
      changed_spells = TRUE;
      show_spell (ch, spell, 9);
      return;
    }
  if (!str_cmp (arg1, "to_notvict1"))
    {
      if (argy == NULL || argy[0] == '\0' || argy == "")
	{
	  if (spell->act_to_notvict_1 != NULL)
	    free_string (spell->act_to_notvict_1);
	  spell->act_to_notvict_1 = NULL;
	  return;
	}
      if (spell->act_to_notvict_1 != NULL)
	free_string (spell->act_to_notvict_1);
      spell->act_to_notvict_1 = str_dup (argy);
      changed_spells = TRUE;
      show_spell (ch, spell, 10);
      return;
    }
  if (!str_cmp (arg1, "to_notvict2"))
    {
      if (argy == NULL || argy[0] == '\0' || argy == "")
	{
	  if (spell->act_to_notvict_2 != NULL)
	    free_string (spell->act_to_notvict_2);
	  spell->act_to_notvict_2 = NULL;
	  return;
	}
      if (spell->act_to_notvict_2 != NULL)
	free_string (spell->act_to_notvict_2);
      spell->act_to_notvict_2 = str_dup (argy);
      changed_spells = TRUE;
      show_spell (ch, spell, 11);
      return;
    }
  if (!str_cmp (arg1, "pre1"))
    {
      if (argy == NULL || argy[0] == '\0' || argy == "")
	{
	  if (spell->prereq_1 != NULL)
	    free_string (spell->prereq_1);
	  spell->prereq_1 = NULL;
	  return;
	}
      if (spell->prereq_1 != NULL)
	free_string (spell->prereq_1);
      spell->prereq_1 = str_dup (argy);
      changed_spells = TRUE;
      show_spell (ch, spell, 3);
      return;
    }
  if (!str_cmp (arg1, "pre2"))
    {
      if (argy == NULL || argy[0] == '\0' || argy == "")
	{
	  if (spell->prereq_2 != NULL)
	    free_string (spell->prereq_2);
	  spell->prereq_2 = NULL;
	  return;
	}
      if (spell->prereq_2 != NULL)
	free_string (spell->prereq_2);
      spell->prereq_2 = str_dup (argy);
      changed_spells = TRUE;
      show_spell (ch, spell, 3);
      return;
    }
  if (!str_cmp (arg1, "modifier"))
    {
      char arg2[SML_LENGTH];
      int num;
      argy = one_argy(argy, arg2);
      if (!is_number(arg2))
	{
	  send_to_char("Syntax: modifier <number> <modifier val>\n\r", ch);
	  return;
	}
      num = atoi(arg2);
      if (num < 0 || num > 4)
	{
	  send_to_char("Pick a number from 0-4 to modify.\n\r", ch);
	  return;
	}
      if (argy == NULL || argy[0] == '\0' || argy == "")
	{
	  send_to_char ("Syntax: modifier <number> <modifier val>.\n\r", ch);
	  return;
	}
      if (spell->modifier[num] != NULL)
	free_string (spell->modifier[num]);
      spell->modifier[num] = str_dup (argy);
      changed_spells = TRUE;
      show_spell (ch, spell, 15);
      return;
    }
  if ((!str_cmp (arg1, "mwis") || !str_cmp (arg1, "mdex")) && is_number (argy))
    {
      int ii;
      ii = atoi (argy);
      spell->min_wis = ii;
      changed_spells = TRUE;
      show_spell (ch, spell, 2);
      return;
    }
  if ((!str_cmp (arg1, "mint") || !str_cmp (arg1, "mstr")) && is_number (argy))
    {
      int ii;
      ii = atoi (argy);
      spell->min_int = ii;
      changed_spells = TRUE;
      show_spell (ch, spell, 1);
      return;
    }
  if ((!str_cmp (arg1, "mana") || !str_cmp (arg1, "moves")) && is_number (argy))
    {
      int ii;
      ii = atoi (argy);
      spell->mana = ii;
      changed_spells = TRUE;
      show_spell (ch, spell, 1);
      return;
    }
  if (!str_cmp (arg1, "lvl") && is_number (argy))
    {
      int ii;
      ii = atoi (argy);
      spell->spell_level = ii;
      changed_spells = TRUE;
      show_spell (ch, spell, 1);
      return;
    }
  if (!str_cmp (arg1, "goodroom") && is_number (argy))
    {
      int ii;
      ii = atoi (argy);
      spell->in_room_good = ii;
      changed_spells = TRUE;
      show_spell (ch, spell, 1);
      return;
    }
  if (!str_cmp (arg1, "evilroom") && is_number (argy))
    {
      int ii;
      ii = atoi (argy);
      spell->in_room_evil = ii;
      changed_spells = TRUE;
      show_spell (ch, spell, 3);
      return;
    }
  if (!str_cmp (arg1, "fail_chance") && is_number (argy))
    {
      int ii;
      ii = atoi (argy);
      spell->fail_chance = ii;
      changed_spells = TRUE;
      show_spell (ch, spell, 3);
      return;
    }
  if (!str_cmp (arg1, "spell_lag") && is_number (argy))
    {
      int ii;
      ii = atoi (argy);
      spell->spell_lag = ii;
      changed_spells = TRUE;
      show_spell (ch, spell, 3);
      return;
    }
  if (!str_cmp (arg1, "pk_fail") && is_number (argy))
    {
      int ii;
      ii = atoi (argy);
      spell->pk_fail_chance = ii;
      changed_spells = TRUE;
      show_spell (ch, spell, 3);
      return;
    }
  if ((!str_cmp (arg1, "time") || !str_cmp (arg1, "delay")) && is_number (argy))
    {
      int ii;
      ii = atoi (argy);
      spell->casting_time = ii;
      changed_spells = TRUE;
      show_spell (ch, spell, 1);
      return;
    }
  if (!str_cmp (arg1, "creates_obj") && is_number (argy))
    {
      int ii;
      ii = atoi (argy);
      spell->creates_obj = ii;
      changed_spells = TRUE;
      return;
    }
  if (!str_cmp (arg1, "slot") && is_number (argy))
    {
      int ii;
      ii = atoi (argy);
      if (ii == 0)
	{
	  send_to_char ("You can not set slot number to 0.\n\r", ch);
	  return;
	}
      spell->slot = ii;
      changed_spells = TRUE;
      show_spell (ch, spell, 5);
      return;
    }
  fnn = FALSE;
  if (stpos_rev (arg1) != -99)
    {
      spell->position = stpos_rev (arg1);
      changed_spells = TRUE;
      show_spell (ch, spell, 1);
      fnn = TRUE;
    }
  if (mtype_rev (arg1) != -99)
    {
      spell->mana_type ^= mtype_rev (arg1);
      changed_spells = TRUE;
      show_spell (ch, spell, 1);
      fnn = TRUE;
    }
  if (styp_rev (arg1) != -99)
    {
      spell->spell_type = styp_rev (arg1);
      changed_spells = TRUE;
      show_spell (ch, spell, 2);
      fnn = TRUE;
    }
  if (spell_extra_bits_rev (arg1) != -99)
    {
      spell->spell_bits ^= spell_extra_bits_rev (arg1);
      changed_spells = TRUE;
      show_spell (ch, spell, 12);
      fnn = TRUE;
    }
  if (spell_extra_bits2_rev (arg1) != -99)
    {
      spell->spell_bits2 ^= spell_extra_bits2_rev (arg1);
      changed_spells = TRUE;
      show_spell (ch, spell, 12);
      fnn = TRUE;
    }
  if (affect_name_bit (arg1) != 0)
    {
      spell->bitvector ^= affect_name_bit (arg1);
      changed_spells = TRUE;
      show_spell (ch, spell, 4);
      fnn = TRUE;
    }
  if (affect_name_bit_two (arg1) != 0)
    {
      spell->bitvector2 ^= affect_name_bit_two (arg1);
      changed_spells = TRUE;
      show_spell (ch, spell, 4);
      fnn = TRUE;
    }
  if( !str_cmp(arg1, "location"))
    {
      char arg2[SML_LENGTH];
      int num;
      argy = one_argy(argy, arg2);
      if (!is_number(arg2))
	{
	  send_to_char("Syntax: location <number> <type>\n\r", ch);
	  return;
	}
      num = atoi(arg2);
      if (num < 0 || num > 4)
	{
	  send_to_char("Pick a number from 0-4 to set the location.\n\r", ch);
	  return;
	}
      if (argy == NULL || argy[0] == '\0' || argy == "")
	{
	  send_to_char ("Syntax: location <number> <modifier val>.\n\r", ch);
	  return;
	}
      if (affect_name_loc (argy) != -99)
	{
	  spell->location[num] = affect_name_loc (argy);
	  changed_spells = TRUE;
	  show_spell (ch, spell, 14);
	  fnn = TRUE;
	}
      else
	{
	  send_to_char("Location not found.\n\r", ch);
	   show_spell (ch, spell, 14);
	  return;
	}
    }
  if (!fnn)
    interpret (ch, orarg);
  return;
}

SPELL_DATA *
new_spell (void)
{
  SPELL_DATA *spell;
  spell = mem_alloc (sizeof (*spell));
  bzero (spell, sizeof (*spell));
  top_spell++;
  spell->slot = 1;
  spell->linked_to = NULL;
  spell->noun_damage = str_dup ("Spell");
  spell->gsn = top_spell;
  spell->min_int = 0;
  spell->min_wis = 0;
  spell->spell_type = 0;
  spell->spell_bits = 0;
  spell->spell_name = str_dup ("Newspell");
  spell->spell_funky_name = str_dup ("NewSpell");
  spell->spell_level = 0;
  spell->prereq_1 = NULL;
  spell->prereq_2 = NULL;
  spell->act_to_ch_1 = NULL;
  spell->act_to_ch_2 = NULL;
  spell->act_to_vict_1 = NULL;
  spell->act_to_vict_2 = NULL;
  spell->act_to_notvict_1 = NULL;
  spell->act_to_notvict_2 = NULL;
  {
    int jj;
    for (jj = 0; jj < 5; jj++)
      {
	spell->location[jj] = 0;
	spell->modifier[jj] = NULL;
      }
  }
  spell->duration = NULL;
  spell->bitvector = 0;
  spell->damage = NULL;
  spell->mana = 0;
  spell->mana_type = 0;
  spell->casting_time = 0;
  spell->position = POSITION_RESTING;
  spell->wear_off_msg = str_dup ("The spell has worn off.");
  spell->creates_obj = 0;
  spell->guilds = 0;
  spell->spell_lag = 0;
  spell->in_room_good = 0;
  spell->in_room_evil = 0;
  spell->fail_chance = 0;
  spell->pk_fail_chance = 0;
  spell->next = spell_list[spell->gsn % SPELL_HASH];
  spell_list[spell->gsn % SPELL_HASH] = spell;
  spell->next_a = spell_list_2[UPPER (spell->spell_funky_name[0]) % SPELL_HASH];
  spell_list_2[UPPER (spell->spell_funky_name[0]) % SPELL_HASH] = spell;
  return spell;
}

/* Below is my translator. Variables accepted:
   l = level
   i = char's intelligence
   w = char's wisdom
   a = random from 1-10
   b = random from 10-20
   c = random from 20-30
   d = random from 1-level
   m = Syntax m42 would be max of 42 damage (numeric only)
   z = Syntax z3 would be minimum 3 damage (numeric only)
   k = kick damage
   g = Wizard's Guild then 1, else 0
   h = Healer's Guild then 1, else 0
   o = Warrior's Guild then 1, else 0
   t = max hit points... used for mob exp calc.
   please note: all operations are performed from left to right. This makes
   for simple coding, and a damn quick pass thru a complicated expression.
   sample:
   l+w/3 would be written in real terms as: l/3 + w/3 or (l+w)/3
   i+w+l*2/3+l would be written in real terms as: (((i+w)+l)*2)/3)+l
   You can now use parens :)
 */
char *pos;
long 
translate (char *line, int level, CHAR_DATA * ch)
{
  long dam = 0;
  long cur_num = 0;
  char *tp;
  char cur_op = '+';
  long ctt;
  if (!line)
    return 0;
  pos = line;
  for (pos = line; *pos != '\0'; pos++)
    {
      if (is_digit (*pos))
	{
	  cur_num *= 10;
	  cur_num += (*pos - '0');
	  continue;
	}
      if (*pos == ')')
	{
	  if (cur_op == '*')
	    dam *= cur_num;
	  if (cur_op == '/')
	    dam /= cur_num;
	  if (cur_op == '+')
	    dam += cur_num;
	  if (cur_op == '-')
	    dam -= cur_num;
	  if (cur_op == '\0')
	    dam = cur_num;
	  return dam;
	}
      if (*pos == '(')
	{
	  cur_num = translate (pos + 1, level, ch);
	  continue;
	}
      if (*pos == 't')
	{
	  cur_num = ch->max_hit;
	}
      if (*pos == 'g')
	{
	  cur_num = is_member (ch, GUILD_WIZARD) ? 1 : 0;
	}
      if (*pos == 'u')
	{
	  if (ch->pcdata->race > 10)
	    cur_num = 1;
          else
	    cur_num = 0;
	}
      if (*pos == 'h')
	{
	  cur_num = is_member (ch, GUILD_HEALER) ? 1 : 0;
	}
      if (*pos == 'o')
	{
	  cur_num = is_member (ch, GUILD_WARRIOR) ? 1 : 0;
	}
      if (*pos == 'z')
	{
	  int tnn = 0;
	  if (cur_op == '*')
	    dam *= cur_num;
	  if (cur_op == '/')
	    dam /= cur_num;
	  if (cur_op == '+')
	    dam += cur_num;
	  if (cur_op == '-')
	    dam -= cur_num;
	  if (cur_op == '\0')
	    dam = cur_num;
	  cur_op = '[';
	  cur_num = 0;
	  ctt = 0;
	  for (tp = pos + 1; *tp != '\0' && is_digit ((*tp)); tp++)
	    {
	      ctt++;
	      tnn *= 10;
	      tnn += (*tp - '0');
	    }
	  if (dam < tnn)
	    {
	      dam = tnn;
	      cur_num = tnn;
	    }
	  pos += ctt;
	  continue;
	}
      if (*pos == 'm')
	{
	  int tnn = 0;
	  if (cur_op == '*')
	    dam *= cur_num;
	  if (cur_op == '/')
	    dam /= cur_num;
	  if (cur_op == '+')
	    dam += cur_num;
	  if (cur_op == '-')
	    dam -= cur_num;
	  if (cur_op == '\0')
	    dam = cur_num;
	  cur_op = '[';
	  cur_num = 0;
	  ctt = 0;
	  for (tp = pos + 1; *tp != '\0' && is_digit ((*tp)); tp++)
	    {
	      ctt++;
	      tnn *= 10;
	      tnn += (*tp - '0');
	    }
	  if (dam > tnn)
	    {
	      dam = tnn;
	      cur_num = tnn;
	    }
	  pos += ctt;
	  continue;
	}
      if (*pos == '+')
	{
	  if (cur_op == '*')
	    dam *= cur_num;
	  if (cur_op == '/')
	    dam /= cur_num;
	  if (cur_op == '+')
	    dam += cur_num;
	  if (cur_op == '-')
	    dam -= cur_num;
	  if (cur_op == '\0')
	    dam = cur_num;
	  cur_op = '+';
	  cur_num = 0;
	}
      if (*pos == '-')
	{
	  if (cur_op == '*')
	    dam *= cur_num;
	  if (cur_op == '/')
	    dam /= cur_num;
	  if (cur_op == '+')
	    dam += cur_num;
	  if (cur_op == '-')
	    dam -= cur_num;
	  if (cur_op == '\0')
	    dam = cur_num;
	  cur_op = '-';
	  cur_num = 0;
	}
      if (*pos == '/')
	{
	  if (cur_op == '*')
	    dam *= cur_num;
	  if (cur_op == '/')
	    dam /= cur_num;
	  if (cur_op == '+')
	    dam += cur_num;
	  if (cur_op == '-')
	    dam -= cur_num;
	  if (cur_op == '\0')
	    dam = cur_num;
	  cur_op = '/';
	  cur_num = 0;
	}
      if (*pos == '*')
	{
	  if (cur_op == '*')
	    dam *= cur_num;
	  if (cur_op == '/')
	    dam /= cur_num;
	  if (cur_op == '+')
	    dam += cur_num;
	  if (cur_op == '-')
	    dam -= cur_num;
	  if (cur_op == '\0')
	    dam = cur_num;
	  cur_op = '*';
	  cur_num = 0;
	}
      if (*pos == 'd')
	cur_num = number_range (1, level);
      if (*pos == 'k')
	cur_num = ch->pcdata->plus_kick;
      if (*pos == 'l')
	cur_num = level;
      if (*pos == 'w' && ch)
	cur_num = get_curr_wis (ch);
      if (*pos == 'i' && ch)
	cur_num = get_curr_int (ch);
      if (*pos == 's' && ch)
	cur_num = get_curr_str (ch);
      if (*pos == 'x' && ch)
	cur_num = get_curr_con (ch);
      if (*pos == 'y' && ch)
	cur_num = get_curr_dex (ch);
      if (*pos == 'a')
	cur_num = number_range (1, 10);
      if (*pos == 'b')
	cur_num = number_range (10, 20);
      if (*pos == 'c')
	cur_num = number_range (20, 30);
    }
  if (cur_op == '*')
    dam *= cur_num;
  if (cur_op == '/')
    dam /= cur_num;
  if (cur_op == '+')
    dam += cur_num;
  if (cur_op == '-')
    dam -= cur_num;
  if (cur_op == '\0')
    dam = cur_num;
  return dam;
     
}
