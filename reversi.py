# -*- Coding: utf-8 -*-

import tkinter as tk
from tkinter import messagebox
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

class Cell:
    def __init__(self, game, parent, y, x):
        self.y = y
        self.x = x
        self.frame = tk.Frame(parent, relief=tk.FLAT, bd=2, width=50, height=50)
        self.canvas = tk.Canvas(self.frame, bg='#459b5f', width=50, height=50)
        self.canvas.bind('<Button-1>', self.on_click)
        self.frame.grid(row=y, column=x)
        self.canvas.place(x = 0, y = 0)
        self.game = game

    def on_click(self, event):
        print('clicked! ({}, {})'.format(self.y, self.x))
        Reversi.progress(self.game, (self.y, self.x))
        


class Reversi:
    def __init__(self, order, mode = 'cui', area = 8):
        self.BOARD_WIDTH = self.BOARD_HEIGHT = area
        self.board = [[None for _ in range(self.BOARD_WIDTH + 2)] for _ in range(self.BOARD_HEIGHT + 2)] # outer-side

        self.display = mode

        if mode == 'gui':
            self.window = tk.Tk()
            self.window.title = 'リバーシゲーム'
            self.window.geometry(
                '{}x{}'.format(self.BOARD_WIDTH*50+10, self.BOARD_HEIGHT*50+10 + 200)
            )
            self.board_frame = tk.Frame(
                self.window, 
                relief=tk.RIDGE, 
                bd=5, 
                width=self.BOARD_WIDTH*50, 
                height=self.BOARD_HEIGHT*50
            )

            # cell配置
            self.cells = [[None for _ in range(self.BOARD_WIDTH+2)] for _ in range(self.BOARD_HEIGHT+2)]
            for y in range(1, self.BOARD_HEIGHT+1):
                for x in range(1, self.BOARD_WIDTH+1):
                    self.cells[y][x] = Cell(self, self.board_frame, y, x)

            self.board_frame.pack()

            self.info_frame = tk.Frame(
                self.window, 
                bg='gray80', 
                width=self.board_frame['width'], 
                height=200
            )
        
            # 誰番か、パスがあったかを表示
            self.info_frame.pack()
        
        self.ORDER = order
        self.putlist = []
        self.order_index = 0
        self.DIRECTION = ((-1, -1), (-1, 0), (-1, 1), (0, -1), (0, 1), (1, -1), (1, 0), (1, 1))

        self.stone_num = 0
        self.score = '' # 棋譜

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

        if self.display == 'gui':
            while self.turn.group == 'AI':
                # 先に置く
                self.put_stone(self.turn.rogic(self), self.turn)
                self.nextturn()
                self.putlist = self.make_putlist(self.turn)

                start_player = self.turn
                # パス判定
                while len(self.putlist) == 0:
                    self.nextturn()
                    self.putlist = self.make_putlist(self.turn)

                if start_player == self.turn:
                    break

                if len(self.putlist) == 0:
                    print('error.')
            
            self.show_board_gui()

    def show_board_gui(self):
        # 盤面
        for y in range(1, self.BOARD_HEIGHT+1):
            for x in range(1, self.BOARD_WIDTH+1):
                # cellの中身をきれいに
                self.cells[y][x].canvas.delete('all')
                if (y, x) in self.putlist:
                    self.cells[y][x].canvas.create_oval(10, 10, 40, 40, fill='gray50', tag='stone')
                elif self.board[y][x] is not None:
                    self.cells[y][x].canvas.create_oval(10, 10, 40, 40, fill=self.board[y][x], tag='stone')

        # 手番
        print('{} の ターン'.format(self.turn.to_name()))


    def make_putlist(self, player, board = None):
        """
        turn_playerが今置ける場所を返す。
        """

        if board is None:
            board = self.board[:]

        result = []

        for y in range(1, self.BOARD_HEIGHT+1):
            for x in range(1, self.BOARD_WIDTH+1):

                if board[y][x] is not None:
                    continue

                for d in self.DIRECTION:
                    dx, dy = x + d[1], y + d[0]

                    if board[dy][dx] is None:
                        continue
                    
                    if board[dy][dx] != player.to_color():
                        # 相手の石を見つけた→ひっくり返せるか調査
                        # 自分の石の色に or Noneがくるまで
                        while board[dy][dx] is not None and board[dy][dx] != player.to_color():
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
        if test == True and test_board is not None:
            board = test_board[:]
        else:
            board = [[None for _ in range(self.BOARD_WIDTH+2)] for _ in range(self.BOARD_HEIGHT+2)]
            for y in range(1, self.BOARD_HEIGHT+1):
                for x in range(1, self.BOARD_WIDTH+1):
                    board[y][x] = self.board[y][x]


        board[position[0]][position[1]] = player.to_color()
        if test == False:
            self.stone_num += 1
            self.score += chr(ord('A') + (position[0] - 1)) +  str(position[1])

        for d in self.DIRECTION:
            dx, dy = position[1] + d[1], position[0] + d[0]

            if board[dy][dx] is None:
                continue

            if board[dy][dx] != player.to_color() and board[dy][dx] is not None:
                # 相手の石
                # 自分の石の色になるまで
                while board[dy][dx] is not None and (board[dy][dx] != player.to_color()):
                    dx += d[1]
                    dy += d[0]


                if board[dy][dx] is None:
                    continue
                
                if board[dy][dx] == player.to_color():
                    # ひっくり返せる
                    dx -= d[1]
                    dy -= d[0]
                    while board[dy][dx] is not None and (board[dy][dx] != player.to_color()):
                        board[dy][dx] = player.to_color()
                        dx -= d[1]
                        dy -= d[0]

        if test == False:
            for y in range(1, self.BOARD_HEIGHT+1):
                for x in range(1, self.BOARD_WIDTH+1):
                    self.board[y][x] = board[y][x]

        return board


    def nextturn(self):
        self.order_index = (self.order_index + 1) % len(self.ORDER)
        self.turn = self.ORDER[self.order_index]


    def end(self):

        print('\ngame end!')
        
        for p in self.ORDER:
            for y in range(1, self.BOARD_WIDTH+1):
                for x in range(1, self.BOARD_HEIGHT+1):
                    if self.board[y][x] == p.to_color():
                        p.point += 1

        for p in self.ORDER:
            print('{} points -> {}'.format(p.to_name(), p.point))

        draw_flag = False
        winner = self.ORDER[0]
        log_str = self.ORDER[0].to_name()
        for p in self.ORDER[1:]:
            if winner.point < p.point:
                winner = p
                draw_flag = False
            elif winner.point == p.point:
                draw_flag = True

        if draw_flag:
            log_str = 'DRAW'
            if self.display == 'gui':
                tk.messagebox.showinfo('GAME SET', 'DRAW')
            else:
                print('GAME SET\nDRAW')

        else:
            log_str = winner.to_name()
            if self.display == 'gui':
                tk.messagebox.showinfo('GAME SET', '{}, WIN!'.format(winner.to_name()))
            else:
                print('GAME SET\n{}, WIN!'.format(winner.to_name()))

        # 対戦ログ記入
        # 多人数戦未対応
        from datetime import datetime
        ## YYYY-MM-DD HH:MM:SS : xx vs xx -> xx
        with open('matchlog.txt', 'a') as f:
            f.write('\n{} : {}({}) vs {}({}) -> {}\nscore -> {}'.format(
                datetime.now().strftime('%Y-%m-%d %H:%M:%S'),
                self.ORDER[0].to_name(), self.ORDER[0].point,
                self.ORDER[1].to_name(), self.ORDER[1].point,
                log_str,
                self.score
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

        

    @classmethod
    def progress(cls, game, pos):
        '''
        プレイヤーが手番のとき、おけるマスをクリックしたらゲームが進行する
        '''

        (y, x) = pos

        if not((y,x) in game.putlist):
            print('position error')
            print('game.putlist -> {}'.format(game.putlist))
            return

        game.put_stone((y,x), game.turn)
        game.nextturn()
        game.putlist = game.make_putlist(game.turn)
        game.show_board_gui()

        pass_player = game.pass_check()
        if len(pass_player) == len(game.ORDER):
            # ゲーム終了
            game.end()
            game.window.destroy()
            return
        else:
            for p in pass_player:
                tk.messagebox.showinfo('GAME INFO', '{}, PASS'.format(p.to_name()))
                game.show_board_gui()

        if game.turn.group == 'AI':
            # AIに手番を投げる
            Reversi.progress(game, game.turn.rogic(game))