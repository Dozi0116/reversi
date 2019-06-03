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
                'AI', 
                bot.changeable_mini_max,
                name='CHANGE_MINI_MAX_BOT', 
                max_depth=4
            ),
            reversi.Player(
                'gray99', 
                'AI', 
                bot.mini_max, 
                name='MINI_MAX_BOT'
            )
        ],
        area = 8
    )

    game.game_init()

    # game main loop
    score = '' # 棋譜
    while True:
        # print('PLAYER {} TURN'.format(game.ORDER.index(game.turn)+1))
        pos = game.turn.rogic(game)
        # print('put position -> ({}, {})'.format(pos[0], pos[1]))
        # print('{}{}'.format(chr(ord('A') + (pos[0] - 1)), pos[1]), end='')
        score += chr(ord('A') + (pos[0] - 1)) +  str(pos[1])

        game.put_stone(pos, game.turn)
        
        game.nextturn()

        game.putlist = game.make_putlist(game.turn)

        pass_player = game.pass_check()
        if len(pass_player) == len(game.ORDER):
            # ゲーム終了
            game.end()
            print(score)
            return
        else:
            for p in pass_player:
                # print('\n\nGAME INFO\nPLAYER {}, PASS\n'.format(game.ORDER.index(game.turn)+1))
                pass

    return

if __name__ == '__main__':
    main()