/*
   hm_gameserver -  hearthmod gameserver
   Copyright (C) 2016 Filip Pancik

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <game.h>

void mechanics_put_card_onboard(struct conn_client_s *c, u64 position, struct card_s *attacker, int summoned, struct card_list_s **d, struct chooseoption_s *opt, struct deck_s *deck)
{
    struct hs_holder_s *p;
    struct card_s *card;
    struct card_list_s *defenders = NULL;

    p = c->data;

    assert(deck);

    hm_log(LOG_DEBUG, c->log, "%s Putting card %d onboard summoned %d defenders %p", LOG_MECHANICS, attacker->id, summoned, defenders);
    flag(&attacker->state, CARD_HAND, FLAG_UNSET);

    // if charge or battlecry target
    if(!flag(&attacker->state, CARD_CHARGE, FLAG_ISSET) &&
            !flag(&attacker->state, CARD_BATTLECRY_TARGET, FLAG_ISSET) &&
            !flag(&attacker->state, CARD_WEAPON, FLAG_ISSET)
      ) {
        flag(&attacker->state, CARD_EXHAUSTED, FLAG_SET);
    }

    // check for present starving buzzards
    if(flag(&attacker->state, CARD_RACE_BEAST, FLAG_ISSET)) {
        int buzzards;
        // exclude starving buzzard when playing it
        if(flag(&attacker->state, CARD_STARVING_BUZZARD, FLAG_ISSET)) {
            buzzards = cards_get_board_flag(deck, CARD_STARVING_BUZZARD, attacker->id);
        } else {
            buzzards = cards_get_board_flag(deck, CARD_STARVING_BUZZARD, -1);
        }
        int i;
        struct card_list_s *item;
        for(i = 0; i < buzzards; i++) {
            card = draw_random_card(deck, p->entity_id, -1);
            flag(&card->state, CARD_HAND, FLAG_SET);
            if(defenders) {
                LIST_ADD(*d, item, card);
            }
        }
    }

    if(flag(&attacker->state, CARD_AURA, FLAG_ISSET)) {
        if(flag(&attacker->state, CARD_RAID_LEADER, FLAG_ISSET)) {
            deck_modifier(p, deck, attacker, DECK_MODIFIER_ATTACK, 1);
        }

        if(flag(&attacker->state, CARD_WARSONG_COMMANDER, FLAG_ISSET)) {
            deck_modifier(p, deck, attacker, DECK_MODIFIER_ATTACK_CHARGE, 1);
        }

        if(flag(&attacker->state, CARD_TIMBER_WOLF, FLAG_ISSET)) {
            deck_modifier(p, deck, attacker, DECK_MODIFIER_ATTACK_BEAST, 1);
        }

        if(flag(&attacker->state, CARD_STORMWIND_CHAMPION, FLAG_ISSET)) {
            deck_modifier(p, deck, attacker, DECK_MODIFIER_ATTACK, 1);
            deck_modifier(p, deck, attacker, DECK_MODIFIER_HEALTH, 1);
        }
    }

    deck_apply_modifiers(p, deck, attacker, position);

    // 1. apply existing attachments immediately
    card_apply_attachments(p, deck, attacker);

    if(attacker->attachment && flag(&attacker->state, CARD_AURA, FLAG_ISSET)) {
        hm_log(LOG_DEBUG, c->log, "%s Attachment [%s]", LOG_MECHANICS, attacker->attachment);

        int att_flag = 0;
        // raid leader
        // stormwind champion
        if(flag(&attacker->state, CARD_RAID_LEADER, FLAG_ISSET) ||
                flag(&attacker->state, CARD_STORMWIND_CHAMPION, FLAG_ISSET)
          ) {
            att_flag = 0;
        }
        else if(flag(&attacker->state, CARD_WARSONG_COMMANDER, FLAG_ISSET)
               ) {
            att_flag = CARD_CHARGE;
        }
        else if(flag(&attacker->state, CARD_TIMBER_WOLF, FLAG_ISSET)
               ) {
            att_flag = CARD_RACE_BEAST;
        }

        // 2. then add new attachment
        deck_add_attachment(deck, attacker->attachment, att_flag);

        // 3. apply new attachment to all cards
        cards_apply_attachment(p, deck, attacker, att_flag);
    }

    /*
       if(attacker->attachment) {
// raid leader
if(flag(&attacker->state, CARD_RAID_LEADER, FLAG_ISSET)) {
deck_add_attachment(deck, attacker->attachment);
struct card_list_s *defenders, *def;
defenders = cards_get_board(deck, NULL, -1, -1);

for(def = defenders; def != NULL; def = def->next) {
card = add_new_card(p->game, deck, -1, attacker->attachment, -1);
add_attachment(&card->attachments, def->card->id);

//def->card->attack += 1;
//flag(&def->card->state, MECHANICS_ATTACK, FLAG_SET);
}
}
}
*/

// spellpower
if(flag(&attacker->state, CARD_SPELLPOWER, FLAG_ISSET)) {
    deck->spellpower++; //attacker->spellpower;
    hm_log(LOG_DEBUG, c->log, "%s Player %d spellpower %d", LOG_MECHANICS, p->entity_id, deck->spellpower);


    // antonidas
    /*
       int antonidas_count = cards_get_board_count_flag(deck, attacker, CARD_ARCHMAGE_ANTONIDAS);
       int i;
       if(antonidas_count > 0) {
       for(i = 0; i < antonidas_count; i++) {
       struct card_s *card = add_new_card(p->game, deck, -1, "CS2_029", -1);
       flag(&card->state, CARD_HAND, FLAG_SET);
       struct card_list_s *item;
       LIST_ADD(*d, item, card);
       }
       }
       */
}

// regular minion or weapon
if(flag(&attacker->state, CARD_WEAPON, FLAG_ISSET)) {

    __SECTION_WEAPON

        /*
        // replace a weapon

        if(deck->heroweapon != NULL) {
        deck->heroweapon->priority_override = 900;
        flag(&(deck->heroweapon->state), CARD_DESTROYED, FLAG_SET);
        flag(&(deck->heroweapon->state), CARD_BOARD_WEAPON, FLAG_UNSET);
        flag(&(deck->heroweapon->state), CARD_HEROWEAPON, FLAG_UNSET);
        }

        flag(&attacker->state, CARD_BOARD_WEAPON, FLAG_SET);
        flag(&attacker->state, CARD_HEROWEAPON, FLAG_SET);

        deck->heroweapon = attacker;
        deck->hero->attack = attacker->attack;
        flag(&deck->hero->state, MECHANICS_ATTACK, FLAG_SET);
        */
} else {
    flag(&attacker->state, CARD_BOARD, FLAG_SET);
}

cards_reorder(p, deck, attacker, position, summoned);


// battlecries
if(flag(&attacker->state, CARD_BATTLECRY, FLAG_ISSET) &&
        //!flag(&attacker->state, CARD_BATTLECRY_TARGET, FLAG_ISSET) && // non-target battlecries
        !flag(&attacker->state, MECHANICS_CHANGESIDES, FLAG_ISSET)  // captured minions do not trigger BC
  ) {

    flag(&attacker->state, MECHANICS_BATTLECRY_TRIGGER, FLAG_SET);

    {
        struct card_s *__fake_levelup = NULL;
        __SECTION_BATTLECRY
    }
}

{
    struct card_s *__fake_attacker = attacker;
    __SECTION_IDLE
}

{
    __SECTION_ONBOARD
}

}
