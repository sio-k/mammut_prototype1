#pragma once
/*
 * Mammut videogame prototype
 *
 * FILE DESCRIPTION: Player structs, struct holding all relevant data for a game session, all relevant #defines and globally required constants
 */

#include <SDL2/SDL.h>

//@{

#define BACKGROUND_IMAGE "./data/sprites/background.jpg"
#define ROLL_DICE_BUTTON_IMAGE "./data/sprites/roll_dice.png"

#define PLAYER_ICON_WIDTH 64
#define PLAYER_ICON_HEIGHT 64
#define PLAYER0_ICON "./data/sprites/player0.png"
#define PLAYER1_ICON "./data/sprites/player1.png"
#define PLAYER2_ICON "./data/sprites/player2.png"
#define PLAYER3_ICON "./data/sprites/player3.png"

#define SPEAR_CARD_BUTTON_ICON "./data/sprites/spear_card.png"
#define BERRY_CARD_BUTTON_ICON "./data/sprites/berry_card.png"

#define NUMBER_0 "./data/sprites/number_1.png"
#define NUMBER_1 "./data/sprites/number_2.png"
#define NUMBER_2 "./data/sprites/number_3.png"
#define NUMBER_3 "./data/sprites/number_4.png"
//@}

//@{
typedef enum {
    POS_MAMMOTH          = 9,
    POS_A                = 8,
    POS_B_HORIZONTAL_LOG = 7,
    POS_C                = 6,
    POS_D                = 5,
    POS_E                = 4,
    POS_F                = 3,
    POS_G_VERTICAL_LOG   = 2,
    POS_H                = 1,
    CAVE                 = 0 
} PlayerPosition;

typedef struct Player {
    SDL_bool       spear_card;
    int            is_targeted;
    SDL_bool       berry_card;
    SDL_bool       carrying_point;
    PlayerPosition position;
    int            points; /* amount of meat collected */
    
    SDL_Rect     ( *screenPos )( const struct Player* self, const int screen_width, const int screen_height ); /* get screen position */
} Player;

Player* Player_new();
Player Player_construct();
void Player_destroy( Player* self );
void Player_delete( Player* self );
//@}
//@{
// struct to chronicle changes during a turn
typedef struct TurnChanges {
    SDL_bool spear_card_played;
    SDL_bool berry_card_played;
    int spear_card_target;
    int diceroll_result;
} TurnChanges;

TurnChanges* TurnChanges_new();
TurnChanges TurnChanges_construct();
void TurnChanges_destroy( TurnChanges* self );
void TurnChanges_delete( TurnChanges* self );
//@}
//@{
typedef struct GameInstance {
    SDL_Window* win;
    SDL_Renderer* ren;
    
    SDL_Texture* background;
    SDL_Texture* player_icons[4];
    SDL_Texture* roll_dice_button_icon;
    SDL_Texture* spear_card_icon;
    SDL_Texture* berry_card_icon;
    
    Player* players[4];
    int player_count;
    
    int current_player;
    SDL_Texture* numbers[4];
    
    int turn_counter;
    void ( *step )( struct GameInstance* self, TurnChanges* t );
    void ( *reset )( struct GameInstance* self, int player_count );
} GameInstance;

GameInstance* GameInstance_new( int player_count );
GameInstance GameInstance_construct( int player_count );
void GameInstance_destroy( GameInstance* self );
void GameInstance_delete( GameInstance* self );
//@}
