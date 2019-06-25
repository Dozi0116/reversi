# -*- Coding: utf-8 -*-

"""
現在botは2人用を想定した設計で制作。
そのため、2人用に特化したreversi_duelを用いると(ほんの少し)動作が早くなります。

GUIの方にreversi_duelは未対応。
"""

import reversi as reversi
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
                name='BOT1',
                max_depth=2
            ),
            reversi.Player(
                'gray99', 
                'AI', 
                bot.changeable_mini_max, 
                name='BOT2',
                max_depth=5
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
        print('put position -> ({}, {})'.format(pos[0], pos[1]))
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

import time
if __name__ == '__main__':
    start = time.time()
    main()
    elapsed_time = time.time() - start
    print("elapsed time is {} [sec]".format(elapsed_time))