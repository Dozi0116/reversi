import tkinter as tk
from tkinter import messagebox
import copy
import random

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
    def __init__(self, y, x):
        self.y = y
        self.x = x
        self.stone = None
        


class Reversi:
    """
    2人用リバーシゲーム
    """

    def __init__(self):
        self.BOARD_WIDTH = self.BOARD_HEIGHT = 8
        self.board = [[None for _ in range(self.BOARD_WIDTH + 2)] for _ in range(self.BOARD_HEIGHT + 2)] # outer-side

        # cell配置
        for y in range(1, self.BOARD_HEIGHT+1):
            for x in range(1, self.BOARD_WIDTH+1):
                self.board[y][x] = Cell(y, x)
        
        self.ORDER = [Player('gray1', 'AI', Reversi.mini_max), Player('gray99', 'AI', Reversi.mini_max)]
        self.putlist = []
        self.order_index = None
        self.DIRECTION = ((-1, -1), (-1, 0), (-1, 1), (0, -1), (0, 1), (1, -1), (1, 0), (1, 1))

    def opponent(self, player):
        """
        指定したプレイヤーの相手を返す。
        """
        return self.ORDER[1] if player == self.ORDER[0] else self.ORDER[0]


    def game_init(self):
        # initialize
        self.order_index = random.randint(0, 1)
        self.turn = self.ORDER[self.order_index]
        self.put_stone([4, 4], self.opponent(self.turn))
        self.put_stone([4, 5], self.turn)
        self.put_stone([5, 4], self.turn)
        self.put_stone([5, 5], self.opponent(self.turn))

        self.putlist = self.make_putlist(self.turn)


    def make_putlist(self, player, board = None):
        """
        turn_playerが今置ける場所を返す。
        盤面の指定があれば指定された盤面を、なければ、現在のゲームの状態を参照する。
        探索計算量はO(マス目の数 * 8)
        """

        # マス情報を移す
        if board is None:
            board = [[None for _ in range(self.BOARD_WIDTH+2)] for _ in range(self.BOARD_HEIGHT+2)]
            for y in range(1, self.BOARD_HEIGHT+1):
                for x in range(1, self.BOARD_WIDTH+1):
                    board[y][x] = self.board[y][x].stone

        result = []

        for y in range(1, self.BOARD_HEIGHT+1):
            for x in range(1, self.BOARD_WIDTH+1):

                if board[y][x] is not None:
                    continue

                for d in self.DIRECTION:
                    dx, dy = x + d[1], y + d[0]

                    if board[dy][dx] is None:
                        continue
                    
                    if board[dy][dx] != player.to_color() and board[dy][dx] is not None:
                        # 相手の石を見つけた→ひっくり返せるか調査
                        # 自分の石の色に or Noneがくるまで
                        while board[dy][dx] is not None and (board[dy][dx] != player.to_color()):
                            dx += d[1]
                            dy += d[0]

                        if board[dy][dx] is None:
                            continue

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
        if test and test_board is not None:
            board = test_board[:]
        else:
            board = [[None for _ in range(self.BOARD_WIDTH+2)] for _ in range(self.BOARD_HEIGHT+2)]
            for y in range(1, self.BOARD_HEIGHT+1):
                for x in range(1, self.BOARD_WIDTH+1):
                    board[y][x] = self.board[y][x].stone


        board[position[0]][position[1]] = player.to_color()

        for d in self.DIRECTION:
            dx, dy = position[1] + d[1], position[0] + d[0]

            if board[dy][dx] is None:
                continue

            if board[dy][dx] != player.to_color() and board[dy][dx] is not None:
                # 相手の石
                # 自分の石の色になるまで
                while board[dy][dx] is not None and board[dy][dx] != player.to_color():
                    dx += d[1]
                    dy += d[0]


                if board[dy][dx] is None:
                    continue
                
                if board[dy][dx] == player.to_color():
                    # ひっくり返せる
                    dx -= d[1]
                    dy -= d[0]
                    while board[dy][dx] is not None and board[dy][dx] != player.to_color():
                        board[dy][dx] = player.to_color()
                        dx -= d[1]
                        dy -= d[0]

        if not test:
            for y in range(1, self.BOARD_HEIGHT+1):
                for x in range(1,self.BOARD_WIDTH+1):
                    self.board[y][x].stone = board[y][x]

        return board


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
            print('\nGAME SET\nDRAW')

        else:
            print('\nGAME SET\nPLAYER {}, WIN!'.format(self.ORDER.index(winner)+1))

    @classmethod
    def mini_max(cls, game):
        """
        ミニマックス法。
        2手先まで読んで、最善の手を選択。
        """

        def calc_score(game, board):
            """
            boardを受け取ったら、評価値を返す
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

            for y in range(1, game.BOARD_WIDTH+1):
                for x in range(1, game.BOARD_HEIGHT+1):
                    if board[y][x] is None:
                        continue
                    elif board[y][x] == game.turn.to_color():
                        score += evaluation[y-1][x-1] # game.boardは外周が含まれているため。
                    else:
                        score -= evaluation[y-1][x-1]

            return score


        # 1手先読みしてみる
        max_score = -9999
        max_pos = game.putlist[0]
        for pos in game.putlist:
            board = game.put_stone(pos, game.turn, test=True) # 1手先置きした盤面
            score = calc_score(game, board)
            print('if put ({}, {}) -> {} points'.format(pos[0], pos[1], score))
            if score > max_score:
                max_score = score
                max_pos = pos


        print("press ({}, {})".format(max_pos[0], max_pos[1]))
        return max_pos
        




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
            
            print('\n\nGAME INFO\nPLAYER {}, PASS\n'.format(game.ORDER.index(game.turn)+1))
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