/*
 * Mammut videogame prototype
 *
 * FILE DESCRIPTION: Functions for new()ing,
 *                                 constructing,
 *                                 deleting
 *                   structs defined in data.h
 * 
 *                   Also, functions assigned to the function pointers in those classes. These functions ARE NOT AVAILABLE OUTSIDE THESE CLASSES OUTSIDE THIS FILE.
 */

#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "data.h"

SDL_Rect Player_screenPos( const Player* self, const int screen_width, const int screen_height );

void GameInstance_step_player0( GameInstance* self, TurnChanges* t );
void GameInstance_step_player1( GameInstance* self, TurnChanges* t );
void GameInstance_step_player2( GameInstance* self, TurnChanges* t );
void GameInstance_step_player3( GameInstance* self, TurnChanges* t );

void GameInstance_step( GameInstance* self, TurnChanges* t, int player );


//@{

Player* Player_new()
{
    Player* self = calloc( 1, sizeof( Player ) );
    if( self == NULL ) {
        return NULL;
    }
    
    *self = Player_construct();
    return self;
}

Player Player_construct()
{
    Player self;
    
    self.spear_card     = SDL_TRUE;
    self.is_targeted    = 0;
    self.berry_card     = SDL_TRUE;
    self.carrying_point = SDL_FALSE;
    self.position       = CAVE;
    self.points         = 0;
    
    // assign function pointers
    self.screenPos = Player_screenPos;
    
    return self;
}

void Player_destroy( Player* self )
{
    memset( self, 0, sizeof( Player ) );
}

void Player_delete( Player* self )
{
    Player_destroy( self );
    free( self );
}

SDL_Rect ConvertPlayerPositionToScreenCoord( const PlayerPosition p_pos, const int screen_width, const int screen_height )
{
    SDL_Rect toreturn;
    memset( &toreturn, 0, sizeof( toreturn ) );
    
    toreturn.y =   screen_height == 0
                 ? 0
                 : screen_height / 2;
    int tenth =   screen_width == 0
                ? 0
                : screen_height / 10;
    toreturn.x =   ( tenth * p_pos )
                 + (   tenth == 0
                     ? 0
                     : tenth / 2 );
    toreturn.w = PLAYER_ICON_WIDTH;
    toreturn.h = PLAYER_ICON_HEIGHT;
    
    return toreturn;
}

SDL_Rect Player_screenPos( const Player* self, const int screen_width, const int screen_height )
{
    return ConvertPlayerPositionToScreenCoord( self -> position, screen_width, screen_height );
}

//@}

//@{
TurnChanges* TurnChanges_new()
{
    TurnChanges* self = calloc( 1, sizeof( TurnChanges ) );
    if( self == NULL ) {
        return NULL;
    }
    *self = TurnChanges_construct();
    return self;
}

TurnChanges TurnChanges_construct()
{
    TurnChanges t;
    memset( &t, 0, sizeof( t ) );
    t.spear_card_played = SDL_FALSE;
    t.berry_card_played = SDL_FALSE;
    t.spear_card_target = -1;
    t.diceroll_result = 0;
    return t;
}

void TurnChanges_destroy( TurnChanges* self )
{
    memset( self, 0, sizeof( TurnChanges ) );
}

void TurnChanges_delete( TurnChanges* self )
{
    TurnChanges_destroy( self );
    free( self );
}
//@}

//@{
void GameInstance_step_player0( GameInstance* self, TurnChanges* t )
{
    Player p = *( self -> players[0] );
    if( self -> players[0] -> is_targeted > 0 ) {
        self -> players[0] -> is_targeted--;
        GameInstance_step_player1( self, t );
        return;
    }
    GameInstance_step( self, t, 0 );
    
    if(   p.berry_card == SDL_TRUE
       && self -> players[0] -> berry_card == SDL_FALSE ) {
        self -> step = GameInstance_step_player0;
    } else {
        self -> step = GameInstance_step_player1;
    }
}

void GameInstance_step_player1( GameInstance* self, TurnChanges* t )
{
    Player p = *( self -> players[1] );
    if( self -> players[1] -> is_targeted > 0 ) {
        self -> players[1] -> is_targeted--;
        if( self -> player_count > 2 ) {
            GameInstance_step_player2( self, t );
        } else {
            GameInstance_step_player0( self, t );
        }
        return;
    }
    GameInstance_step( self, t, 1 );
    
    if(   p.berry_card == SDL_TRUE
       && self -> players[1] -> berry_card == SDL_FALSE ) {
        self -> step = GameInstance_step_player1;
    } else if( self -> player_count > 3 ) {
        self -> step = GameInstance_step_player2;
        if( self -> players[2] -> is_targeted ) {
            self -> current_player = 3;
            if( self -> players[3] -> is_targeted ) {
                self -> current_player = 0;
                if( self -> players[0] -> is_targeted ) {
                    self -> current_player = 1;
                    if( self -> players[1] -> is_targeted ) {
                        self -> current_player = 2;
                    }
                }
            }
        } else {
            self -> current_player = 0;
        }
    } else {
        if( self -> players[0] -> is_targeted ) {
            self -> current_player = 1;
            if( self -> players[1] -> is_targeted ) {
                self -> current_player = 0;
            }
        } else {
            self -> current_player = 0;
        }
        self -> step = GameInstance_step_player0;
    }
}

void GameInstance_step_player2( GameInstance* self, TurnChanges* t )
{
    Player p = *( self -> players[2] );
    if( self -> players[2] -> is_targeted > 0 ) {
        self -> players[2] -> is_targeted--;
        if( self -> player_count > 3 ) {
            GameInstance_step_player3( self, t );
        } else {
            GameInstance_step_player0( self, t );
        }
        return;
    }
    GameInstance_step( self, t, 2 );
    
    if(   p.berry_card == SDL_TRUE
       && self -> players[2] -> berry_card == SDL_FALSE ) {
        self -> step = GameInstance_step_player2;
    } else if( self -> player_count > 3 ) {
        self -> step = GameInstance_step_player3;
        if( self -> players[3] -> is_targeted ) {
            self -> current_player = 0;
            if( self -> players[0] -> is_targeted ) {
                self -> current_player = 1;
                if( self -> players[1] -> is_targeted ) {
                    self -> current_player = 2;
                    if( self -> players[2] -> is_targeted ) {
                        self -> current_player = 3;
                    }
                }
            }
        } else {
            self -> current_player = 3;
        }
    } else {
        perror( "invalid state reached" );
        self -> step = GameInstance_step_player0;
        self -> current_player = 0;
    }
}

void GameInstance_step_player3( GameInstance* self, TurnChanges* t )
{
    Player p = *( self -> players[3] );
    if( self -> players[3] -> is_targeted > 0 ) {
        self -> players[3] -> is_targeted--;
        GameInstance_step_player0( self, t );
        return;
    }
    GameInstance_step( self, t, 3 );
    
    if(   p.berry_card == SDL_TRUE
       && self -> players[3] -> berry_card == SDL_FALSE ) {
        self -> step = GameInstance_step_player3;
    } else {
        self -> step = GameInstance_step_player0;
        if( self -> players[0] -> is_targeted ) {
            self -> current_player = 1;
            if( self -> players[1] -> is_targeted ) {
                self -> current_player = 2;
                if( self -> players[2] -> is_targeted ) {
                    self -> current_player = 3;
                    if( self -> players[3] -> is_targeted ) {
                        self -> current_player = 0;
                    }
                }
            }
        } else {
            self -> current_player = 0;
        }
    }
}

void GameInstance_step( GameInstance* self, TurnChanges* t, int player )
{
    if( t -> spear_card_played == SDL_TRUE ) {
        if( t -> spear_card_target > -1 && t -> spear_card_target < 4 ) {
            self -> players[t -> spear_card_target] -> is_targeted++;
        } else {
            self -> players[rand() % self -> player_count] -> is_targeted++;
        }
        self -> players[player] -> spear_card = SDL_FALSE;
    }
    
    if( t -> berry_card_played == SDL_TRUE ) {
        self -> players[player] -> berry_card = SDL_FALSE;
    }
    
    if( t -> diceroll_result > 0 && t -> diceroll_result < 7 ) {
        int c_pos = self -> players[player] -> position;
        if( self -> players[player] -> carrying_point == SDL_TRUE ) {
            if( c_pos - t -> diceroll_result < 0 ) {
                self -> players[player] -> position = CAVE;
                self -> players[player] -> carrying_point = SDL_FALSE;
                self -> players[player] -> points++;
                if( self -> players[player] -> points > 3 ) {
                    char awinneris[12] = "Winner is ";
                    // works because ASCII
                    awinneris[10] = self -> current_player + 48;
                    awinneris[11] = '\0';
                    SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_INFORMATION, awinneris, awinneris, self -> win );
                    self -> reset( self, self -> player_count );
                    return;
                }
            } else {
                self -> players[player] -> position -= t -> diceroll_result;
            }
        } else {
            if( c_pos + t -> diceroll_result > 10 ) {
                self -> players[player] -> position = POS_MAMMOTH;
                self -> players[player] -> carrying_point = SDL_TRUE;
            } else {
                self -> players[player] -> position += t -> diceroll_result;
            }
        }
    }
}

void GameInstance_reset( GameInstance* self, int player_count )
{
    GameInstance_destroy( self );
    *self = GameInstance_construct( player_count );
}

GameInstance* GameInstance_new( int player_count )
{
    GameInstance* self = calloc( 1, sizeof( GameInstance ) );
    *self = GameInstance_construct( player_count );
    return self;
}

GameInstance GameInstance_construct( int player_count )
{
    GameInstance self;
    memset( &self, 0, sizeof( self ) );
    
    if( player_count == 2 || player_count == 4 ) {
        // everything fine, either 2 or 4 players
    } else {
        // invalid player count
        return self;
    }
    
    self.win = SDL_CreateWindow( "Mammut prototype",
                                 SDL_WINDOWPOS_CENTERED,
                                 SDL_WINDOWPOS_CENTERED,
                                 640,
                                 480,
                                   SDL_WINDOW_SHOWN
                                 | SDL_WINDOW_RESIZABLE
                                 | SDL_WINDOW_INPUT_FOCUS );
    if( self.win == NULL ) {
        perror( "Cannot create window for instance" );
        exit( 1 );
    }
    
    self.ren = SDL_CreateRenderer( self.win,
                                   -1,
                                     SDL_RENDERER_ACCELERATED
                                   | SDL_RENDERER_PRESENTVSYNC );
    if( self.ren == NULL ) {
        perror( "Cannot create renderer for window" );
        SDL_DestroyWindow( self.win );
        exit( 1 );
    }
    //                                  R     G     B   Alpha
    SDL_SetRenderDrawColor( self.ren, 0x13, 0x13, 0x13, 0xFF );
    
    self.player_count = player_count;
    
    for( int i = 0; i < self.player_count; i++ ) {
        self.players[i] = Player_new();
    }
    
    self.turn_counter = 0;
    
    // load images
    //@{
    SDL_Surface* temp = NULL;
    
    temp = IMG_Load( BACKGROUND_IMAGE );
    if( temp ) {
        self.background = SDL_CreateTextureFromSurface( self.ren, temp );
    }
    SDL_FreeSurface( temp );
    temp = NULL;
    
    temp = IMG_Load( ROLL_DICE_BUTTON_IMAGE );
    if( temp ) {
        self.roll_dice_button_icon = SDL_CreateTextureFromSurface( self.ren, temp );
    }
    SDL_FreeSurface( temp );
    temp = NULL;
    
    temp = IMG_Load( PLAYER0_ICON );
    if( temp ) {
        self.player_icons[0] = SDL_CreateTextureFromSurface( self.ren, temp );
    }
    SDL_FreeSurface( temp );
    temp = NULL;
    
    temp = IMG_Load( PLAYER1_ICON );
    if( temp ) {
        self.player_icons[1] = SDL_CreateTextureFromSurface( self.ren, temp );
    }
    SDL_FreeSurface( temp );
    temp = NULL;
    
    temp = IMG_Load( PLAYER2_ICON );
    if( temp ) {
        self.player_icons[2] = SDL_CreateTextureFromSurface( self.ren, temp );
    }
    SDL_FreeSurface( temp );
    temp = NULL;
    
    temp = IMG_Load( PLAYER3_ICON );
    if( temp ) {
        self.player_icons[3] = SDL_CreateTextureFromSurface( self.ren, temp );
    }
    SDL_FreeSurface( temp );
    temp = NULL;
    
    temp = IMG_Load( NUMBER_0 );
    if( temp ) {
        self.numbers[0] = SDL_CreateTextureFromSurface( self.ren, temp );
    }
    SDL_FreeSurface( temp );
    temp = NULL;
    
    temp = IMG_Load( NUMBER_1 );
    if( temp ) {
        self.numbers[1] = SDL_CreateTextureFromSurface( self.ren, temp );
    }
    SDL_FreeSurface( temp );
    temp = NULL;
    
    temp = IMG_Load( NUMBER_2 );
    if( temp ) {
        self.numbers[2] = SDL_CreateTextureFromSurface( self.ren, temp );
    }
    SDL_FreeSurface( temp );
    temp = NULL;
    
    temp = IMG_Load( NUMBER_3 );
    if( temp ) {
        self.numbers[3] = SDL_CreateTextureFromSurface( self.ren, temp );
    }
    SDL_FreeSurface( temp );
    temp = NULL;
    
    temp = IMG_Load( SPEAR_CARD_BUTTON_ICON );
    if( temp ) {
        self.spear_card_icon = SDL_CreateTextureFromSurface( self.ren, temp );
    }
    SDL_FreeSurface( temp );
    temp = NULL;
    
    temp = IMG_Load( BERRY_CARD_BUTTON_ICON );
    if( temp ) {
        self.berry_card_icon = SDL_CreateTextureFromSurface( self.ren, temp );
    }
    SDL_FreeSurface( temp );
    temp = NULL;
    //@}
    
    self.step = GameInstance_step_player0;
    self.reset = GameInstance_reset;
    
    return self;
}

void GameInstance_destroy( GameInstance* self )
{
    if( !self ) {
        return;
    }
    for( int i = 0; i < 4; i++ ) {
        SDL_DestroyTexture( self -> numbers[i] );
        SDL_DestroyTexture( self -> player_icons[i] );
    }
    SDL_DestroyTexture( self -> background );
    SDL_DestroyTexture( self -> roll_dice_button_icon );
    SDL_DestroyTexture( self -> spear_card_icon );
    SDL_DestroyTexture( self -> berry_card_icon );
    
    SDL_DestroyRenderer( self -> ren );
    SDL_DestroyWindow( self -> win );
    
    for( int i = 0; i < self -> player_count; i++ ) {
        Player_delete( self -> players[i] );
    }
    
    memset( self, 0, sizeof( GameInstance ) );
}

void GameInstance_delete( GameInstance* self )
{
    GameInstance_destroy( self );
    free( self );
}
//@}
