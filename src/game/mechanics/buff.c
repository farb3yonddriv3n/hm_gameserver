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

void mechanics_buff(struct conn_client_s *c, struct chooseoption_s *opt, struct card_s *attacker, struct card_list_s **d)
{
    struct hs_holder_s *p;
    struct deck_s *deck;
    struct card_list_s *def;

    p = c->data;
    deck = p->deck_copy;
#define defender ((*d != NULL) ? (*d)->card : NULL)
#define defenders (*d)

    if(defender) {
        hm_log(LOG_DEBUG, c->log, "Buff [%s](%d) to [%s](%d)", attacker->entity->desc, attacker->id, defender->entity->desc, defender->id);
    }


    // set destroyed if spell
    if(flag(&attacker->state, CARD_SPELL, FLAG_ISSET)) {

        {
            struct card_s *__fake_defender = attacker;
            __SECTION_DESTROYED
        }

        flag(&attacker->state, CARD_DESTROYED, FLAG_SET);
        cards_reorder(p, deck, attacker, opt->position, 0);
    }

    __SECTION_BUFF

        // mind control
        if(flag(&attacker->state, CARD_MIND_CONTROL, FLAG_ISSET)) {
            struct card_s *card = NULL;

            if(defenders->card->controller == 1) {
                defenders->card->controller = 2;
            } else {
                defenders->card->controller = 1;
            }

            // set MECHANICS_CHANGESIDES to both cards
            flag(&defenders->card->state, MECHANICS_CHANGESIDES, FLAG_SET);

            copy_card(&card, defenders->card);

            deck->ncards++;
            deck->cards = realloc(deck->cards, sizeof(void *) * (deck->ncards));
            deck->cards[deck->ncards - 1] = card;

            // set post copy
            flag(&defenders->card->state, CARD_BOARD, FLAG_UNSET);

            //mechanics_put_card_onboard(c, u64 position, struct card_s *attacker, 0, NULL);
        }

    // mind vision
    /*
       if(flag(&attacker->state, CARD_MIND_VISION, FLAG_ISSET)) {
       struct card_s *card = cards_get_hand_random(p->opponent.deck_copy);
       if(card) {
       struct card_s *nc = add_new_card(p->game, deck, -1, card->entity->name, -1);
    //int pos = cards_board_last(deck, nc);
    //mechanics_put_card_onboard(c, pos, nc, 1);
    flag(&nc->state, CARD_HAND, FLAG_SET);
    nc->zone_position = ++(p->deck_copy->handposition);
    }
    }
    */

    /*
       if(flag(&attacker->state, CARD_WINDFURY, FLAG_ISSET)) {
       flag(&defenders->card->state, CARD_WINDFURY, FLAG_SET);
       }
       */

    // transform
    /*
       if(flag(&attacker->state, CARD_TRANSFORM, FLAG_ISSET)) {
       struct card_s *card;
       const char *name = NULL;

       if(flag(&attacker->state, CARD_POLYMORPH, FLAG_ISSET)) {
       name = "CS2_tk1";
       } else if(flag(&attacker->state, CARD_HEX, FLAG_ISSET)) {
       name = "hexfrog";
       }

       assert(name);

       card = add_new_card(p->game, p->opponent.deck_copy, -1, name, -1);
       card->zone_position = defenders->card->zone_position;
       mechanics_put_card_onboard(c, card->zone_position, card, 1, NULL, NULL);

       defenders->card->linked = card->id;
       flag(&defenders->card->state, CARD_LINKED, FLAG_SET);
       flag(&defenders->card->state, CARD_HIDE, FLAG_SET);
       flag(&defenders->card->state, CARD_BOARD, FLAG_UNSET);
       }
       */

    // corruption
    /*
       if(flag(&attacker->state, CARD_CORRUPTION, FLAG_ISSET)) {
       flag(&defenders->card->state, CARD_CORRUPTED, FLAG_SET);
       mechanics_expire(p->game->turn + 2, defenders->card, EXPIRE_DESTROY, 1, NULL);

       add_single_attachment(p, deck, attacker, defenders->card);
       mechanics_expire(p->game->turn + 2, defenders->card, EXPIRE_ATTACHMENT, 0, attacker->attachment);
       }
       */

    // hunter's mark
    if(flag(&attacker->state, CARD_HUNTERS_MARK, FLAG_ISSET)) {
        // TODO: finish this
        /*
           int diff = defenders->card->default_health - 1;
           if(defenders->card->health <= diff) {
           defenders->card->health = defenders->card->total_health = 1;
           } else {

           }

           add_single_attachment(p, deck, attacker, defenders->card);
           mechanics_expire(p->game->turn + 2, defenders->card, EXPIRE_ATTACHMENT, 0, attacker->attachment);
           */
    }

    // inner fire
    /*
       if(flag(&attacker->state, CARD_INNERFIRE, FLAG_ISSET)) {
       defenders->card->attack = defenders->card->health;
       flag(&defenders->card->state, MECHANICS_ATTACK, FLAG_SET);
       }
       */

    /*
    // divine spirit
    if(flag(&attacker->state, CARD_DIVINESPIRIT, FLAG_ISSET)) {
    defenders->card->health *= 2;
    flag(&defenders->card->state, MECHANICS_HEALTH, FLAG_SET);
    }
    */

    // blessing of kings
    if(flag(&attacker->state, CARD_BLESSING_OF_KINGS, FLAG_ISSET)) {
        defenders->card->total_health += 4;
        defenders->card->health += 4;
        defenders->card->attack += 4;

        flag(&defenders->card->state, MECHANICS_ATTACK, FLAG_SET);
        flag(&defenders->card->state, MECHANICS_HEALTH, FLAG_SET);

        add_single_attachment(p, deck, attacker, defenders->card);
    }

    /*
    // power word shield
    if(flag(&attacker->state, CARD_POWER_WORD_SHIELD, FLAG_ISSET)) {
    defenders->card->total_health += 2;
    defenders->card->health += 2;

    flag(&defenders->card->state, MECHANICS_HEALTH, FLAG_SET);

    add_single_attachment(p, deck, attacker, defenders->card);

    struct card_list_s *item;
    struct card_s *card = draw_random_card(deck, p->entity_id);
    flag(&card->state, CARD_HAND, FLAG_SET);
    LIST_ADD(*d, item, card);
    }
    */

    // charge buff
    if(flag(&attacker->state, CARD_CHARGE_BUFF, FLAG_ISSET)) {
        defenders->card->attack += 2;
        flag(&defenders->card->state, MECHANICS_ATTACK, FLAG_SET);

        flag(&defenders->card->state, CARD_CHARGE, FLAG_SET);

        if(flag(&defenders->card->state, CARD_EXHAUSTED, FLAG_ISSET)) {
            flag(&defenders->card->state, CARD_EXHAUSTED, FLAG_UNSET);
        }

        add_single_attachment(p, deck, attacker, defenders->card);
    }

    // assasinate, execute, sw: death, sw: pain
    if(flag(&attacker->state, CARD_ASSASINATE, FLAG_ISSET) ||
            flag(&attacker->state, CARD_SHADOW_WORD_PAIN, FLAG_ISSET) ||
            flag(&attacker->state, CARD_SHADOW_WORD_DEATH, FLAG_ISSET) ||
            flag(&attacker->state, CARD_EXECUTE, FLAG_ISSET)) {

        flag(&defenders->card->state, CARD_DESTROYED, FLAG_SET);
    }

    // hand of protection
    if(flag(&attacker->state, CARD_HAND_OF_PROTECTION, FLAG_ISSET)) {
        flag(&defenders->card->state, CARD_DIVINE_SHIELD, FLAG_SET);
    }

    // ancestral healing
    if(flag(&attacker->state, CARD_ANCESTRAL_HEALING, FLAG_ISSET)) {
        if(defenders->card->health != defenders->card->total_health) {
            defenders->card->health = defenders->card->total_health;
            flag(&defenders->card->state, MECHANICS_HEALTH, FLAG_SET);
        }

        flag(&defenders->card->state, CARD_TAUNT, FLAG_SET);
        add_single_attachment(p, deck, attacker, defenders->card);
    }

    // rockbiter weapon
    /*
       if(flag(&attacker->state, CARD_ROCKBITER_WEAPON, FLAG_ISSET)) {
       defenders->card->attack += 3;
       flag(&defenders->card->state, MECHANICS_ATTACK, FLAG_SET);
       mechanics_expire(p->game->turn + 1, defenders->card, EXPIRE_ATTACK, -3, NULL);

       add_single_attachment(p, deck, attacker, defenders->card);
       mechanics_expire(p->game->turn + 1, defenders->card, EXPIRE_ATTACHMENT, 0, attacker->attachment);
       }
       */

    // mark of the wild
    if(flag(&attacker->state, CARD_MARK_OF_THE_WILD, FLAG_ISSET)) {
        defenders->card->total_health += 2;
        defenders->card->health += 2;
        defenders->card->attack += 2;

        flag(&defenders->card->state, MECHANICS_ATTACK, FLAG_SET);
        flag(&defenders->card->state, MECHANICS_HEALTH, FLAG_SET);
        flag(&defenders->card->state, CARD_TAUNT, FLAG_SET);

        add_single_attachment(p, deck, attacker, defenders->card);
    }

    // bloodlust
    if(flag(&attacker->state, CARD_BLOODLUST, FLAG_ISSET)) {
        for(def = defenders; def != NULL; def = def->next) {
            def->card->attack += 3;
            flag(&def->card->state, MECHANICS_ATTACK, FLAG_SET);
            mechanics_expire(p->game->turn + 1, def->card, EXPIRE_ATTACK, -3, NULL);

            add_single_attachment(p, deck, attacker, def->card);
            mechanics_expire(p->game->turn + 1, def->card, EXPIRE_ATTACHMENT, 0, attacker->attachment);
        }
    }

    // savage roar
    if(flag(&attacker->state, CARD_SAVAGE_ROAR, FLAG_ISSET)) {
        for(def = defenders; def != NULL; def = def->next) {
            def->card->attack += 2;
            flag(&def->card->state, MECHANICS_ATTACK, FLAG_SET);
            mechanics_expire(p->game->turn + 1, def->card, EXPIRE_ATTACK, -2, NULL);

            add_single_attachment(p, deck, attacker, def->card);
            mechanics_expire(p->game->turn + 1, def->card, EXPIRE_ATTACHMENT, 0, attacker->attachment);
        }
    }

    /*
    // claw
    if(flag(&attacker->state, CARD_CLAW, FLAG_ISSET)) {
    for(def = defenders; def != NULL; def = def->next) {
    def->card->attack += 2;
    flag(&def->card->state, MECHANICS_ATTACK, FLAG_SET);
    mechanics_expire(p->game->turn + 1, def->card, EXPIRE_ATTACK, -2, NULL);

// armor doesn't expire
def->card->armor += 2;
flag(&def->card->state, MECHANICS_ARMOR, FLAG_SET);

add_single_attachment(p, deck, attacker, def->card);
mechanics_expire(p->game->turn + 1, def->card, EXPIRE_ATTACHMENT, 0, attacker->attachment);
}
}

// heroic strike
if(flag(&attacker->state, CARD_HEROIC_STRIKE, FLAG_ISSET)) {
for(def = defenders; def != NULL; def = def->next) {
def->card->attack += 4;
flag(&def->card->state, MECHANICS_ATTACK, FLAG_SET);
mechanics_expire(p->game->turn + 1, def->card, EXPIRE_ATTACK, -4, NULL);

add_single_attachment(p, deck, attacker, def->card);
mechanics_expire(p->game->turn + 1, def->card, EXPIRE_ATTACHMENT, 0, attacker->attachment);
}
}
*/

// sap
/*
   if(flag(&attacker->state, CARD_SAP, FLAG_ISSET)) {
   for(def = defenders; def != NULL; def = def->next) {
   flag(&def->card->state, CARD_BOARD, FLAG_UNSET);
   flag(&def->card->state, CARD_HAND, FLAG_SET);
   def->card->zone_position = ++(p->opponent.deck_copy->handposition);
//cards_reorder(p->opponent.deck_copy, def->card, opt);
}
}
*/

// vanish
/*
   if(flag(&attacker->state, CARD_VANISH, FLAG_ISSET)) {
   for(def = defenders; def != NULL; def = def->next) {
   flag(&def->card->state, CARD_BOARD, FLAG_UNSET);
   flag(&def->card->state, CARD_HAND, FLAG_SET);

   if(p->id == def->card->parent->deck_copy->controller)
   def->card->zone_position = ++(p->deck_copy->handposition);
   else
   def->card->zone_position = ++(p->opponent.deck_copy->handposition);
   }
   }
   */

// set exhausted if minion
if(flag(&attacker->state, CARD_MINION, FLAG_ISSET)) {
    flag(&attacker->state, CARD_EXHAUSTED, FLAG_SET);
}

}

/*
   void mechanics_attack(struct conn_client_s *c, struct chooseoption_s *opt, struct card_s *attacker, struct card_list_s *defenders)
   {
   struct hs_holder_s *p;
   struct deck_s *deck;
   struct card_list_s *item;

   abort();

   p = c->data;
   deck = p->deck_copy;

   assert(deck);

// only 1 defender is allowed to be here
entity_attack(p, attacker, defenders->card);

if(flag(&attacker->state, CARD_HEROPOWER, FLAG_ISSET)) {
flag(&attacker->state, CARD_EXHAUSTED, FLAG_SET);
flag(&attacker->state, MECHANICS_HEROPOWER, FLAG_SET);
}

// hero attack with an equiped weapon
if(flag(&attacker->state, CARD_HERO, FLAG_ISSET)) {
if(deck->heroweapon) {
hm_log(LOG_DEBUG, c->log, "%s Hero %d attacks with a weapon %d", LOG_MECHANICS, attacker->id, deck->heroweapon->id);
flag(&(deck->heroweapon->state), MECHANICS_DURABILITY, FLAG_SET);
LIST_ADD(defenders, item, deck->heroweapon);

if(--(deck->heroweapon->durability) == 0) {
attacker->attack = 0;
flag(&(attacker->state), MECHANICS_ATTACK, FLAG_SET);
flag(&(deck->heroweapon->state), CARD_DESTROYED, FLAG_SET);

hm_log(LOG_DEBUG, c->log, "%s Hero weapon %d destroyed", LOG_MECHANICS, deck->heroweapon->id);

deck->heroweapon = NULL;
}
} else {
hm_log(LOG_DEBUG, c->log, "%s Hero %d attacks without a weapon", LOG_MECHANICS, attacker->id);
}
}

if(flag(&attacker->state, CARD_DAMAGE_DRAW_CARD, FLAG_ISSET)) {
struct card_s *card = draw_random_card(deck, p->entity_id);
if(card) {
flag(&card->state, CARD_HAND, FLAG_SET);
LIST_ADD(defenders, item, card);
}
}


if(flag(&attacker->state, CARD_SWIPE, FLAG_ISSET)) {
// get all cards, ignore defender - he get an extra portion
struct card_list_s *all = cards_get_board(NULL, p->opponent.deck_copy, 1, defenders->card->id);
struct card_list_s *cr;
defenders->card->receive.damage = 4;
for(cr = all; cr != NULL; cr = cr->next) {
cr->card->receive.damage = 1;
}
}

if(flag(&attacker->state, CARD_SOULFIRE, FLAG_ISSET)) {
struct card_s *card = cards_get_hand_random(deck);
if(card) {
flag(&card->state, CARD_DISCARD, FLAG_SET);
cards_reorder_hand_owner(deck, card->id);
LIST_ADD(defenders, item, card);
}
}

    if(flag(&attacker->state, CARD_DRAIN_LIFE, FLAG_ISSET) ||
            flag(&attacker->state, CARD_SACRIFITIAL_PACT, FLAG_ISSET)
      ) {
        int amount;

        if(flag(&attacker->state, CARD_DRAIN_LIFE, FLAG_ISSET)) {
            amount = 2;
        } else {
            amount = 5;
        }

        if(deck->hero) {
            if((deck->hero->health + amount) >= deck->hero->total_health) {
                deck->hero->health = deck->hero->total_health;
            } else {
                deck->hero->health += amount;
            }

            flag(&deck->hero->state, MECHANICS_HEALTH, FLAG_SET);
            LIST_ADD(defenders, item, deck->hero);
        }
    }

if(flag(&attacker->state, CARD_MORTAL_COIL, FLAG_ISSET) && defenders->card->health == 0) {
    hm_log(LOG_DEBUG, c->log, "%s Defender %d didn't survive Mortal Coil, drawing card", LOG_MECHANICS, defenders->card->id);
    struct card_s *card = draw_random_card(deck, p->entity_id);
    flag(&card->state, CARD_HAND, FLAG_SET);
    LIST_ADD(defenders, item, card);
}

if(flag(&attacker->state, CARD_SPELL, FLAG_ISSET) && !flag(&attacker->state, CARD_HEROPOWER, FLAG_ISSET)) {
    flag(&attacker->state, CARD_DESTROYED, FLAG_SET);
    cards_reorder(p, deck, attacker, opt->position, 0);
}
}
*/
