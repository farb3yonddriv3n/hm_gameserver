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

void mechanics_attack_aoe(struct conn_client_s *c, u64 position, struct card_s *attacker, struct card_list_s **defenders, struct card_s **fake_levelup)
{
    struct hs_holder_s *p;
    struct deck_s *deck;
    struct card_list_s *item;

    p = c->data;
    deck = p->deck_copy;

    assert(deck);

    hm_log(LOG_DEBUG, c->log, "Attack %d > defenders", attacker->id);

    if(!flag(&attacker->state, CARD_TRACKING, FLAG_ISSET)) {
        game_attack_aoe(p, attacker, *defenders, fake_levelup);
    } else {
        if(defenders) attacker->target.card = *defenders;
    }

    if(flag(&attacker->state, CARD_HEROPOWER, FLAG_ISSET)) {
        flag(&attacker->state, MECHANICS_HEROPOWER, FLAG_SET);
        flag(&attacker->state, CARD_EXHAUSTED, FLAG_SET);
        hm_log(LOG_DEBUG, c->log, "Card [%s](%d) heropower triggered", attacker->entity->desc, attacker->id);
    } else {
        hm_log(LOG_DEBUG, c->log, "Card [%s](%d) heropower not triggered", attacker->entity->desc, attacker->id);
    }

    // hero attack with an equiped weapon
    if(flag(&attacker->state, CARD_HERO, FLAG_ISSET)) {
        if(deck->heroweapon) {
            hm_log(LOG_DEBUG, c->log, "%s Hero %d attacks with a weapon %d", LOG_MECHANICS, attacker->id, deck->heroweapon->id);
            flag(&(deck->heroweapon->state), MECHANICS_DURABILITY, FLAG_SET);
            LIST_ADD((*defenders), item, deck->heroweapon);

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


    if(flag(&attacker->state, CARD_FAN_OF_KNIVES, FLAG_ISSET)) {
        struct card_s *card = draw_random_card(deck, p->entity_id, -1);
        flag(&card->state, CARD_HAND, FLAG_SET);
        LIST_ADD(*defenders, item, card);
    }

    if(flag(&attacker->state, CARD_SPELL, FLAG_ISSET) && !flag(&attacker->state, CARD_HEROPOWER, FLAG_ISSET)) {
        flag(&attacker->state, CARD_DESTROYED, FLAG_SET);
        cards_reorder(p, deck, attacker, position, 0);
    }
}

