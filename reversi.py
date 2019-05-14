import tkinter as tk
from tkinter import messagebox
import copy
import time


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
    def __init__(self, board, parent, y, x):
        self.y = y
        self.x = x
        self.stone = None
        self.frame = tk.Frame(parent, relief=tk.FLAT, bd=2, width=50, height=50)
        self.canvas = tk.Canvas(self.frame, bg='#459b5f', width=50, height=50)
        self.canvas.bind('<Button-1>', self.on_click)
        self.frame.grid(row=y, column=x)
        self.canvas.place(x = 0, y = 0)
        self.board = board

    def on_click(self, event):
        print('clicked! ({}, {}) -> {}'.format(self.y, self.x, self.stone))
        Reversi.progress(self.board, (self.y, self.x))
        


class Reversi:
    def __init__(self):
        self.BOARD_WIDTH = self.BOARD_HEIGHT = 8
        self.board = [[None for _ in range(self.BOARD_WIDTH + 2)] for _ in range(self.BOARD_HEIGHT + 2)] # outer-side

        self.window = tk.Tk()
        self.window.title = 'リバーシゲーム'
        self.window.geometry('{}x{}'.format(self.BOARD_WIDTH*50+10, self.BOARD_HEIGHT*50+10 + 200))
        self.board_frame = tk.Frame(self.window, relief=tk.RIDGE, bd=5, width=self.BOARD_WIDTH*50, height=self.BOARD_HEIGHT*50)

        # cell配置
        for y in range(1, self.BOARD_HEIGHT+1):
            for x in range(1, self.BOARD_WIDTH+1):
                self.board[y][x] = Cell(self, self.board_frame, y, x)

        self.board_frame.pack()

        self.info_frame = tk.Frame(self.window, bg='gray80', width=self.board_frame['width'], height=200)
        # 誰番か、パスがあったかを表示
        self.info_frame.pack()
        
        self.PLAYER = Player('gray1', 'human')
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

        self.show_board_gui()

    def show_board_gui(self):
        # 盤面
        for y in range(1, self.BOARD_HEIGHT+1):
            for x in range(1, self.BOARD_WIDTH+1):
                # cellの中身をきれいに
                self.board[y][x].canvas.delete('all')
                if (y, x) in self.putlist:
                    self.board[y][x].canvas.create_oval(10, 10, 40, 40, fill='gray50', tag='stone')
                elif not(self.board[y][x].stone is None):
                    self.board[y][x].canvas.create_oval(10, 10, 40, 40, fill=self.board[y][x].stone, tag='stone')

        # 手番
        print('player{} の ターン'.format(self.order_index + 1))


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
        '''
        positionに応じて石を置く。
        testがTrueのときは、実際には反映させず、結果だけ返す。
        '''
        
        board = [[None for _ in range(self.BOARD_WIDTH+2)] for _ in range(self.BOARD_HEIGHT+2)]
        for y in range(1, self.BOARD_HEIGHT+1):
            for x in range(1, self.BOARD_WIDTH+1):
                board[y][x] = self.board[y][x].stone

        board[position[0]][position[1]] = player.to_color()

        for d in self.DIRECTION:
            dx, dy = position[1] + d[1], position[0] + d[0]

            if board[dy][dx] is None:
                continue

            if board[dy][dx] != player.to_color() and not(board[dy][dx] is None):
                # 相手の石
                # 自分の石の色になるまで
                while not(board[dy][dx] is None) and (board[dy][dx] != player.to_color()):
                    dx += d[1]
                    dy += d[0]


                if board[dy][dx] is None:
                    continue
                
                if board[dy][dx] == player.to_color():
                    # ひっくり返せる
                    dx -= d[1]
                    dy -= d[0]
                    while not(board[dy][dx] is None) and (board[dy][dx] != player.to_color()):
                        board[dy][dx] = player.to_color()
                        dx -= d[1]
                        dy -= d[0]

        if not test:
            print('test = False')
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
            tk.messagebox.showinfo('GAME SET', 'DRAW')

        else:
            tk.messagebox.showinfo('GAME SET', 'PLAYER {}, WIN!'.format(self.ORDER.index(winner)+1))

    @classmethod
    def progress(cls, game, pos):
        '''
        プレイヤーが手番のとき、おけるマスをクリックしたらゲームが進行する
        '''

        (y, x) = pos

        if not((y,x) in game.putlist):
            print('position error')
            return

        game.put_stone((y,x), game.turn)

        game.nextturn()

        game.putlist = game.make_putlist(game.turn)

        game.show_board_gui()

        start_player = game.turn
        # パス判定
        while len(game.putlist) == 0:
            
            tk.messagebox.showinfo('GAME INFO', 'PLAYER {}, PASS'.format(game.ORDER.index(game.turn)+1))
            game.nextturn()

            game.putlist = game.make_putlist(game.turn)
            game.show_board_gui()
            

            if start_player == game.turn:
                break

        if len(game.putlist) == 0:
            # 終了
            game.end()
            game.window.destroy()
            return

        if game.turn.type == 'AI':
            # AIに手番を投げる
            Reversi.progress(game, game.turn.rogic(game))
            

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

                    print('(y, x) = ({}, {}) => {} : {} points'.format())

            return score


        # 1手先読みしてみる
        score = []
        for pos in game.putlist:
            board = game.put_stone(pos, game.turn, test=True)
            score.append(calc_score(game, board))

        print(score)


        # print("press ({}, {})".format(game.putlist[0][0], game.putlist[0][1]))
        return game.putlist[0]
        




def main():
    # reversi program
    
    ## game initialize
    game = Reversi()

    game.game_init()

    game.window.mainloop()

    return



if __name__ == '__main__':
    main()