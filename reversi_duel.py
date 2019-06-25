# -*- Coding: utf-8 -*-

"""
2人用に特化したシミュ用リバーシ
"""

import copy
import time
import random


class Player:
    def __init__(self, color, group='human', rogic=None, name=None, **kwargs):
        self.color = color
        self.point = 0
        self.group = group
        self.rogic = rogic
        self.name = name

        self.kwargs = kwargs

        """
        self.rogicについて
        rogicはAIのときに使う変数。
        引数は(board, putlist)
        AIの手番のときに呼ばれ、(y,x)の形で返す。
        """


    def to_color(self):
        return self.color

    def to_name(self):
        if self.group == 'AI':
            return self.rogic.__name__
        else:
            return self.name


class Reversi:
    def __init__(self, order, mode = 'cui', area = 8):
        self.BOARD_WIDTH = self.BOARD_HEIGHT = area
        self.board = [[-1 for _ in range(self.BOARD_WIDTH + 2)] for _ in range(self.BOARD_HEIGHT + 2)] # outer-side

        self.display = mode
        
        self.ORDER = order
        self.ORDER[0].color = 0
        self.ORDER[1].color = 1
        self.putlist = []
        self.order_index = 0
        self.DIRECTION = ((-1, -1), (-1, 0), (-1, 1), (0, -1), (0, 1), (1, -1), (1, 0), (1, 1))

        self.stone_num = 0

    def opponent(self, player):
        """
        指定したプレイヤーの相手を返す。
        """
        return self.ORDER[1] if player == self.ORDER[0] else self.ORDER[0]

    def game_init(self):
        # initialize
        print(self.ORDER[0].to_color(), self.ORDER[1].to_color())
        self.order_index = random.randint(0, 1) # 0 or 1
        self.turn = self.ORDER[self.order_index]
        self.put_stone([4, 4], self.opponent(self.turn))
        self.put_stone([4, 5], self.turn)
        self.put_stone([5, 4], self.turn)
        self.put_stone([5, 5], self.opponent(self.turn))

        self.putlist = self.make_putlist(self.turn)

        print('{} からスタート'.format(self.turn.to_name()))

    def make_putlist(self, player, board = None):
        """
        turn_playerが今置ける場所を返す。
        """

        if board is None:
            board = self.board[:]

        result = []

        for y in range(1, self.BOARD_HEIGHT+1):
            for x in range(1, self.BOARD_WIDTH+1):

                if board[y][x] != -1:
                    continue

                for d in self.DIRECTION:
                    dx, dy = x + d[1], y + d[0]
                    
                    if board[dy][dx] == player.to_color() ^ 1:
                        # 相手の石を見つけた→ひっくり返せるか調査
                        # 自分の石の色に or Noneがくるまで
                        while board[dy][dx] == player.to_color() ^ 1:
                            dx += d[1]
                            dy += d[0]

                        if board[dy][dx] == player.to_color():
                            # ひっくり返せる
                            # resultにappendして、for d in self.DIRECTIONを抜ける
                            result.append((y, x))
                            break

        return result



    def put_stone(self, position, player, test = False, test_board = None):
        '''
        positionに応じて石を置く。
        testがTrueのときは、実際には反映させず、結果だけ返す。
        test_boardがあった場合
        '''
        board = None
        if test == True and test_board is not None:
            board = test_board[:]
        else:
            board = [[-1 for _ in range(self.BOARD_WIDTH+2)] for _ in range(self.BOARD_HEIGHT+2)]
            for y in range(1, self.BOARD_HEIGHT+1):
                for x in range(1, self.BOARD_WIDTH+1):
                    board[y][x] = self.board[y][x]


        board[position[0]][position[1]] = player.to_color()
        if test == False:
            self.stone_num += 1

        for d in self.DIRECTION:
            dx, dy = position[1] + d[1], position[0] + d[0]

            if board[dy][dx] == player.to_color() ^ 1:
                # 相手の石
                # 自分の石の色になるまで
                while board[dy][dx] == player.to_color() ^ 1:
                    dx += d[1]
                    dy += d[0]
                
                if board[dy][dx] == player.to_color():
                    # ひっくり返せる
                    dx -= d[1]
                    dy -= d[0]
                    while board[dy][dx] == player.to_color() ^ 1:
                        board[dy][dx] ^= 1
                        dx -= d[1]
                        dy -= d[0]

        if test == False:
            for y in range(1, self.BOARD_HEIGHT+1):
                for x in range(1, self.BOARD_WIDTH+1):
                    self.board[y][x] = board[y][x]

        return board


    def nextturn(self):
        self.order_index ^= 1
        self.turn = self.ORDER[self.order_index]


    def end(self):

        print('\ngame end!')
        
        point = 0
        log_str = None

        for y in range(1, self.BOARD_WIDTH+1):
            for x in range(1, self.BOARD_HEIGHT+1):
                if self.board[y][x] == 0:
                    point += 1
                elif self.board[y][x] == 1:
                    point -= 1

        if point == 0:
            log_str = 'DRAW'
            print('GAME SET\nDRAW')

        else:
            log_str = self.ORDER[0].to_name() if point > 0 \
                      else self.ORDER[1].to_name()
            print('GAME SET\n{}, WIN!'.format(log_str))

        # 対戦ログ記入
        from datetime import datetime
        ## YYYY-MM-DD HH:MM:SS : xx vs xx -> xx
        with open('matchlog.txt', 'a') as f:
            f.write('\n{} : {}({}) vs {}({}) -> {}'.format(
                datetime.now().strftime('%Y-%m-%d %H:%M:%S'),
                self.ORDER[0].to_name(), self.ORDER[0].point,
                self.ORDER[1].to_name(), self.ORDER[1].point,
                log_str
            ))



    def pass_check(self):
        '''
        パスをする人を返す。
        空リストが返ればパスはなし。
        '''

        result = []

        if self.stone_num >= self.BOARD_HEIGHT * self.BOARD_WIDTH:
            # 置ききって終了→全員パス
            return self.ORDER

        start_player = self.turn

        while len(self.putlist) == 0:
            result.append(self.turn)

            self.nextturn()
            self.putlist = self.make_putlist(self.turn)

            if self.turn == start_player:
                break

        return result