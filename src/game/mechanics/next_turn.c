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

/*
   static void healing_totem(struct hs_holder_s *p, struct deck_s *deck)
   {
   struct card_list_s *c, *cl;
   struct card_s *found = NULL;

   cl = cards_get_board(deck, NULL, 0, 0);

   for(c = cl; c != NULL; c = c->next) {
   if(strcmp(c->card->entity->name, "NEW1_009") == 0) {
   hm_log(LOG_DEBUG, lg, "Healing totem found [%s](%d)", c->card->entity->desc, c->card->id);
   found = c->card;
   break;
   }
   }

   if(found == NULL) {
   hm_log(LOG_DEBUG, lg, "Healing totem not found");
   return;
   }

   for(c = cl; c != NULL; c = c->next) {
   entity_attack(p, found, c->card);
   }

   found->target.card = cl;

   flag(&found->state, MECHANICS_TURN_TRIGGER, FLAG_SET);
   }
   */

static void summon_npc(struct conn_client_s *c, struct hs_holder_s *p, struct deck_s *opponent)
{
    /*
       int pos = cards_board_last(opponent);

       hm_log(LOG_DEBUG, lg, "Summon NPC: last position %d", pos);

       if(pos > MAX_BOARD) {

       hm_log(LOG_DEBUG, lg, "Summon NPC: last position %d too high", pos);
       return;
       }

       struct card_s *card = add_new_card(p->game, opponent, -1, "CS2_boar", -1);
       if(card) {
       mechanics_put_card_onboard(c, pos, card, 1, NULL, NULL, opponent);
       card->zone_position = pos;
       }
       */
    hsl_summon_card(opponent, "EX1_158t", holder);
}

void mechanics_next_turn(struct conn_client_s *c, struct hs_holder_s *p)
{
    struct deck_s *opponent, *deck;

    deck = p->deck_copy;
    opponent = p->opponent.deck_copy;

    if(p->game->turn % 2 == 0 && p->deck->mana < 10) {
        deck->mana++;
        opponent->mana++;
    }

    p->game->turn++;

    reset_turn_plays(opponent);

    summon_npc(c, p, opponent);

    mechanics_expired(p, deck);
    mechanics_expired(p, opponent);

    cards_unset_flag(opponent, CARD_EXHAUSTED);

    register_turntimer(p->opponent.client);

    // fatigue?
    if(card_deck_count(opponent) == 0) {
        struct card_s *card = p->opponent.deck_copy->hero;
        assert(card);

        hm_log(LOG_DEBUG, lg, "Opponent's deck empty: Fatigue %d to hero [%s](%d)", p->opponent.deck_copy->fatigue, card->entity->desc, card->id);

        ++(opponent->fatigue);

        card->health -= opponent->fatigue;

        if(card->health <= 0) {
            opponent->gameover = 1;
        }
    }

    // healing totem?
    //healing_totem(p, deck);
    {
        struct deck_s *__fake_deck = deck;
        __SECTION_ENDTURN
    }
}
