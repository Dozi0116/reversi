# -*- Coding: utf-8 -*-

import reversi
import bot_methods as bot

def main():
    # reversi program
    
    ## game initialize
    game = reversi.Reversi([reversi.Player('gray1', 'AI', bot.mini_max), \
                    reversi.Player('gray99', 'AI', bot.mini_max)], \
                    area = 8)

    game.game_init()

    # game main loop
    while True:
        # print('PLAYER {} TURN'.format(game.ORDER.index(game.turn)+1))
        pos = game.turn.rogic(game)
        # print('put position -> ({}, {})'.format(pos[0], pos[1]))
        print('{}{}'.format(chr(ord('A') + (pos[0] - 1)), pos[1]), end='')

        game.put_stone(pos, game.turn)
        
        game.nextturn()

        game.putlist = game.make_putlist(game.turn)

        start_player = game.turn
        # パス判定
        while len(game.putlist) == 0:
            
            # print('\n\nGAME INFO\nPLAYER {}, PASS\n'.format(game.ORDER.index(game.turn)+1))
            game.nextturn()

            game.putlist = game.make_putlist(game.turn)

            if start_player == game.turn:
                break

        if len(game.putlist) == 0:
            # 終了
            game.end()
            return

    return

if __name__ == '__main__':
    main()