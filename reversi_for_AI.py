import tkinter as tk
from tkinter import messagebox
import copy

'''
AI vs AI用のGUIを使わないやつ
'''

class Player:
    def __init__(self, color, type='human', rogic=None):
        self.color = color
        self.point = 0
        self.type = type
        self.rogic = rogic

        """
        self.rogicについて
        rogicはAIのときに使う変数。
        引数は(board, putlist)
        AIの手番のときに呼ばれ、(y,x)の形で返す。
        """


    def to_color(self):
        return self.color

class Cell:
    def __init__(self, board, y, x):
        self.y = y
        self.x = x
        self.stone = None
        self.board = board

    def on_click(self, event):
        print('clicked! ({}, {})'.format(self.y, self.x))
        Reversi.progress(self.board, (self.y, self.x))
        


class Reversi:
    def __init__(self):
        self.BOARD_WIDTH = self.BOARD_HEIGHT = 8
        self.board = [[None for _ in range(self.BOARD_WIDTH + 2)] for _ in range(self.BOARD_HEIGHT + 2)] # outer-side

        # cell配置
        for y in range(1, self.BOARD_HEIGHT+1):
            for x in range(1, self.BOARD_WIDTH+1):
                self.board[y][x] = Cell(self, y, x)
        
        self.PLAYER = Player('gray1', 'AI', Reversi.mini_max)
        self.OPPONENT = Player('gray99', 'AI', Reversi.mini_max)
        self.ORDER = [self.PLAYER, self.OPPONENT]
        self.putlist = []
        self.order_index = 0
        self.DIRECTION = ((-1, -1), (-1, 0), (-1, 1), (0, -1), (0, 1), (1, -1), (1, 0), (1, 1))

    def game_init(self):
        # initialize
        self.put_stone([4, 4], self.OPPONENT)
        self.put_stone([4, 5], self.PLAYER)
        self.put_stone([5, 4], self.PLAYER)
        self.put_stone([5, 5], self.OPPONENT)

        self.turn = self.PLAYER

        self.putlist = self.make_putlist(self.turn)


    def make_putlist(self, player):
        """
        turn_playerが今置ける場所を返す。
        """

        result = []

        for y in range(1, self.BOARD_HEIGHT+1):
            for x in range(1, self.BOARD_WIDTH+1):

                if not(self.board[y][x].stone is None):
                    continue

                for d in self.DIRECTION:
                    dx, dy = x + d[1], y + d[0]

                    if self.board[dy][dx] is None:
                        continue
                    
                    if self.board[dy][dx].stone != player.to_color() and not(self.board[dy][dx].stone is None):
                        # 相手の石を見つけた→ひっくり返せるか調査
                        # 自分の石の色に or Noneがくるまで
                        while not(self.board[dy][dx] is None) and not(self.board[dy][dx].stone is None) and (self.board[dy][dx].stone != player.to_color()):
                            dx += d[1]
                            dy += d[0]

                        if self.board[dy][dx] is None:
                            continue

                        if self.board[dy][dx].stone == player.to_color():
                            # ひっくり返せる
                            # resultにappendして、for d in self.DIRECTIONを抜ける
                            result.append((y, x))
                            break

        return result



    def put_stone(self, position, player, test=False):
        
        board = self.board[:]

        board[position[0]][position[1]].stone = player.to_color()

        for d in self.DIRECTION:
            dx, dy = position[1] + d[1], position[0] + d[0]

            if board[dy][dx] is None:
                continue

            if board[dy][dx].stone != player.to_color() and not(board[dy][dx].stone is None):
                # 相手の石
                # 自分の石の色になるまで
                while not(board[dy][dx] is None) and not(board[dy][dx].stone is None) and (board[dy][dx].stone != player.to_color()):
                    dx += d[1]
                    dy += d[0]


                if board[dy][dx] is None:
                    continue
                
                if board[dy][dx].stone == player.to_color():
                    # ひっくり返せる
                    dx -= d[1]
                    dy -= d[0]
                    while not(board[dy][dx].stone is None) and (board[dy][dx].stone != player.to_color()):
                        board[dy][dx].stone = player.to_color()
                        dx -= d[1]
                        dy -= d[0]

        if not test:
            self.board = board[:]


    def nextturn(self):
        self.order_index = (self.order_index + 1) % len(self.ORDER)
        self.turn = self.ORDER[self.order_index]


    def end(self):
        
        for p in self.ORDER:
            for y in range(1, self.BOARD_WIDTH+1):
                for x in range(1, self.BOARD_HEIGHT+1):
                    if self.board[y][x].stone == p.to_color():
                        p.point += 1

        draw_flag = False
        winner = self.ORDER[0]
        for p in self.ORDER[1:]:
            if winner.point < p.point:
                winner = p
                draw_flag = False
            elif winner.point == p.point:
                draw_flag = True

        if draw_flag:
            printf('\nGAME SET\nDRAW')

        else:
            print('\nGAME SET\nPLAYER {}, WIN!'.format(self.ORDER.index(winner)+1))

    @classmethod
    def mini_max(cls, game):
        """
        ミニマックス法。
        2手先まで読んで、最善の手を選択。
        """

        evaluation = [ \
            [ 30, -12,   0,  -1,  -1,   0, -12,  30], \
            [-12, -15,  -3,  -3,  -3,  -3, -15, -12], \
            [  0,  -3,   0,  -1,  -1,   0,  -3,   0], \
            [ -1,  -3,  -1,  -1,  -1,  -1,  -3,  -1], \
            [ -1,  -3,  -1,  -1,  -1,  -1,  -3,  -1], \
            [  0,  -3,   0,  -1,  -1,   0,  -3,   0], \
            [-12, -15,  -3,  -3,  -3,  -3, -15, -12], \
            [ 30, -12,   0,  -1,  -1,   0, -12,  30] \
        ]

        score = 0


        # print("press ({}, {})".format(game.putlist[0][0], game.putlist[0][1]))
        return game.putlist[0]
        




def main():
    # reversi program
    
    ## game initialize
    game = Reversi()

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
            
            # print('GAME INFO\nPLAYER {}, PASS'.format(game.ORDER.index(game.turn)+1))
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