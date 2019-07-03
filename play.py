# -*- Coding: utf-8 -*-

import reversi
import bot_methods as bot

def main():
    # reversi program
    
    ## game initialize
    game = reversi.Reversi(
        [
            reversi.Player(
                'gray1',
                'Player', 
                name='PLAYER'
            ),
            reversi.Player(
                'gray99', 
                'AI', 
                bot.alpha_beta, 
                name='BOT',
                max_depth=4
            )
        ],
        area = 8,
        mode='gui'
    )

    game.game_init()

    game.window.mainloop()

    return



if __name__ == '__main__':
    main()