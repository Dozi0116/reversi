import tkinter as tk


class Player:
    def __init__(self, color):
        self.color = color
        self.point = 0

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
        print('clicked! ({}, {})'.format(self.y, self.x))
        Reversi.progress(self.board, self.y, self.x)
        


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
        
        self.PLAYER = Player('gray1')
        self.OPPONENT = Player('gray99')
        self.ORDER = [self.PLAYER, self.OPPONENT]
        self.putlist = []
        self.order_index = 0
        self.DIRECTION = ((-1, -1), (-1, 0), (-1, 1), (0, -1), (0, 1), (1, -1), (1, 0), (1, 1))

    def game_init(self):
        # initialize
        self.put_stone([4, 4], self.PLAYER)
        self.put_stone([4, 5], self.OPPONENT)
        self.put_stone([5, 4], self.OPPONENT)
        self.put_stone([5, 5], self.PLAYER)

        self.turn = self.PLAYER

        self.make_putlist(self.turn)

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
        turn_playerが今置ける場所をself.putlistに格納。
        """

        self.putlist = []

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
                            self.putlist.append((y, x))
                            break



    def put_stone(self, position, player):
        self.board[position[0]][position[1]].stone = player.to_color()

        for d in self.DIRECTION:
            dx, dy = position[1] + d[1], position[0] + d[0]

            if self.board[dy][dx] is None:
                continue

            if self.board[dy][dx].stone != player.to_color() and not(self.board[dy][dx].stone is None):
                # 相手の石
                # 自分の石の色になるまで
                while not(self.board[dy][dx] is None) and not(self.board[dy][dx].stone is None) and (self.board[dy][dx].stone != player.to_color()):
                    dx += d[1]
                    dy += d[0]


                if self.board[dy][dx] is None:
                    continue
                
                if self.board[dy][dx].stone == player.to_color():
                    # ひっくり返せる
                    dx -= d[1]
                    dy -= d[0]
                    while not(self.board[dy][dx].stone is None) and (self.board[dy][dx].stone != player.to_color()):
                        self.board[dy][dx].stone = player.to_color()
                        dx -= d[1]
                        dy -= d[0]


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
            print('''
            ==========
             GAME SET
               DRAW
            ==========
            ''')

        else:
            print('''
            ============
              GAME SET
            PLAYER {} WIN
            ============
            '''.format(self.ORDER.index(winner)+1))

    @classmethod
    def progress(cls, board, y, x):

        if not((y,x) in board.putlist):
            print('position error')
            return

        board.put_stone((y,x), board.turn)

        board.nextturn()

        board.make_putlist(board.turn)

        start_player = board.turn
        # パス判定
        while len(board.putlist) == 0:
            print('=====PASS=====')
            board.nextturn()

            if start_player == board.turn:
                break

        if len(board.putlist) == 0:
            # 終了
            board.window.destroy()
            board.end()
            return

        

        board.show_board_gui()

        




def main():
    # reversi program
    
    ## game initialize
    game = Reversi()

    game.game_init()

    game.window.mainloop()

    return



if __name__ == '__main__':
    main()