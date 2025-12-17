#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#define GAME_KEY_SEEN 1
#define GAME_KEY_DOWN 2

#define DISPLAY_BUFFER_WIDTH 1020.0
#define DISPLAY_BUFFER_HEIGHT 840.0

#define DISPLAY_SCALE 1.0
#define DISPLAY_WIDTH DISPLAY_SCALE* DISPLAY_BUFFER_WIDTH
#define DISPLAY_HEIGHT DISPLAY_SCALE* DISPLAY_BUFFER_HEIGHT

#define DRAW_DECK_X 150          // Canto Esquerdo
#define DRAW_DECK_Y 690         // Altura visível (antes estava 910)

#define DECK_WIDTH 294
#define DECK_HEIGHT 369
#define DECK_SCALE 0.4


#define DRAW_DISCARD_X 860      
#define DRAW_DISCARD_Y 690      // Mesma altura do deck
#define DISCARD_SCALE 0.4

#define PLAYER_BEGIN_X 100
#define PLAYER_BEGIN_Y 430
#define PLAYER_WIDTH 500
#define PLAYER_HEIGHT 500
#define PLAYER_SCALE 0.4

#define MOB1_BEGIN_X 800
#define MOB1_BEGIN_Y 430
#define MOB1_WIDTH 500
#define MOB1_HEIGHT 500
#define MOB1_SCALE 0.4

#define MOB2_BEGIN_X 500
#define MOB2_BEGIN_Y 430
#define MOB2_WIDTH 500
#define MOB2_HEIGHT 500
#define MOB2_SCALE 0.4


#define BOSS_BEGIN_X 550
#define BOSS_BEGIN_Y 250
#define BOSS_WIDTH 500
#define BOSS_HEIGHT 500
#define BOSS_SCALE 0.8


#define BACKGROUND_BEGIN_X 0
#define BACKGROUND_BEGIN_Y 0
#define BACKGROUND_WIDTH 1024
#define BACKGROUND_HEIGHT 1024
#define BACKGROUND_SCALE 0.82

#define ENERGIA_X 10
#define ENERGIA_Y 720
#define ENERGIA_SCALE 1.2

#define HEALTH_BAR_HEIGHT 20
#define HEALTH_BAR_BACKGROUND_EXTRA 1

#define HEALTH_BAR_RX 0
#define HEALTH_BAR_RY 0

#define SHIELD_BAR_HEIGHT 20
#define SHIELD_BAR_BACKGROUND_EXTRA 1

#define SHIELD_BAR_RX 0
#define SHIELD_BAR_RY 0

#define HAND_BEGIN_X 290
//começou em 325
#define HAND_BEGIN_Y 710

#define CARD_WIDTH 150
#define CARD_HEIGHT 180


// You might want to use this.
typedef enum {
  MOVE_LEFT = 0,
  MOVE_RIGHT = 1,
} CursorMovementDirection;  

#endif