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

void mechanics_summon(struct conn_client_s *c, struct chooseoption_s *opt, struct card_s *attacker, struct card_list_s **defenders)
{
    struct hs_holder_s *p;
    struct deck_s *deck;
    //int i;
    //struct card_s *card;
    //struct card_list_s *item;

    p = c->data;
    deck = p->deck_copy;

    assert(deck);

    assert(attacker);

    /*
       if(flag(&attacker->state, CARD_SUMMON_CS2_MIRROR, FLAG_ISSET)) {
       for(i = 0; i < 2; i++) {
       card = add_new_card(p->game, deck, -1, "CS2_mirror", -1);
       int pos = cards_board_last(deck, card);
       mechanics_put_card_onboard(c, pos, card, 1, NULL, NULL);
       }
       }

       if(flag(&attacker->state, CARD_HP_REINFORCE, FLAG_ISSET)) {
       card = add_new_card(p->game, deck, -1, "CS2_101t", -1);
       int pos = cards_board_last(deck, card);
       mechanics_put_card_onboard(c, pos, card, 1, NULL, NULL);
       }

       if(flag(&attacker->state, CARD_HP_ARMORUP, FLAG_ISSET)) {
       deck->hero->armor += 2;

       flag(&deck->hero->state, MECHANICS_ARMOR, FLAG_SET);

       LIST_ADD(*defenders, item, deck->hero);
       }

       if(flag(&attacker->state, CARD_HP_LIFETAP, FLAG_ISSET)) {
       deck->hero->health -= 2;
       flag(&deck->hero->state, MECHANICS_HEALTH, FLAG_SET);

       card = draw_random_card(deck, p->entity_id);
       flag(&card->state, CARD_HAND, FLAG_SET);
       LIST_ADD(*defenders, item, card);
       LIST_ADD(*defenders, item, deck->hero);
       }

       if(flag(&attacker->state, CARD_HP_SHAPESHIFT, FLAG_ISSET)) {
       deck->hero->attack += 1;
       deck->hero->armor += 1;

       mechanics_expire(p->game->turn + 1, deck->hero, EXPIRE_ATTACK, -1, NULL);

       flag(&deck->hero->state, MECHANICS_ATTACK, FLAG_SET);
       flag(&deck->hero->state, MECHANICS_ARMOR, FLAG_SET);

       LIST_ADD(*defenders, item, deck->hero);
       }

       if(flag(&attacker->state, CARD_HP_DAGGERMASTERY, FLAG_ISSET)) {
       card = add_new_card(p->game, deck, -1, "CS2_082", -1);

       flag(&card->state, CARD_BOARD, FLAG_SET);

       if(deck->heroweapon) {
       deck->hero->attack -= deck->heroweapon->attack;
       }

       deck->heroweapon = card;

       deck->hero->attack += card->attack;
       flag(&deck->hero->state, MECHANICS_ATTACK, FLAG_SET);

       LIST_ADD(*defenders, item, deck->hero);
       }

       if(flag(&attacker->state, CARD_SUMMON_ANIMAL_COMPANION, FLAG_ISSET)) {
       int r = random_number(MAX_BEASTS);
       card = add_new_card(p->game, deck, -1, race_beast[r], -1);
       int pos = cards_board_last(deck, card);
       mechanics_put_card_onboard(c, pos, card, 1, defenders, NULL);
       }

       if(flag(&attacker->state, CARD_HP_TOTEMIC_CALL, FLAG_ISSET)) {
    //int r = random_number(MAX_TOTEMIC_CALL);
    card = add_new_card(p->game, deck, -1, totemic_call[3], -1);
    int pos = cards_board_last(deck, card);
    mechanics_put_card_onboard(c, pos, card, 1, NULL, NULL);

}
*/

if(flag(&attacker->state, CARD_SPELL, FLAG_ISSET)) {
    if(!flag(&attacker->state, CARD_HEROPOWER, FLAG_ISSET)) {
        flag(&attacker->state, CARD_DESTROYED, FLAG_SET);
        cards_reorder(p, deck, attacker, opt->position, 0);
    } else {
        flag(&attacker->state, CARD_EXHAUSTED, FLAG_SET);
    }
}
}
