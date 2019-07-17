# -*- Coding: utf-8 -*-

import reversi
import bot_methods as bot
import load

def main():
    # reversi program
    
    ## game initialize
    game = reversi.Reversi(
        load.parse_config('play_config.json'),
        area = 8,
        mode='gui'
    )

    game.game_init()

    game.window.mainloop()

    return



if __name__ == '__main__':
    main()