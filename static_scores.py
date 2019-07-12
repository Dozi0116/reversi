# -*- Coding: utf-8 -*-

"""
盤面評価関数系
自分が有利になればなるほど正の値を返す。
"""
def static_board_score(game, board):
    """
    boardを受け取ったら、評価値を返す
    """

    evaluation = [
        [ 30, -12,   0,  -1,  -1,   0, -12,  30],
        [-12, -15,  -3,  -3,  -3,  -3, -15, -12],
        [  0,  -3,   0,  -1,  -1,   0,  -3,   0],
        [ -1,  -3,  -1,  -1,  -1,  -1,  -3,  -1],
        [ -1,  -3,  -1,  -1,  -1,  -1,  -3,  -1],
        [  0,  -3,   0,  -1,  -1,   0,  -3,   0],
        [-12, -15,  -3,  -3,  -3,  -3, -15, -12],
        [ 30, -12,   0,  -1,  -1,   0, -12,  30]
    ]

    score = 0

    for y in range(1, game.BOARD_WIDTH+1):
        for x in range(1, game.BOARD_HEIGHT+1):
            if board[y][x] == game.turn.to_color():
                score += evaluation[y-1][x-1] # game.boardは外周が含まれているため。
            elif board[y][x] == game.opponent(game.turn).to_color():
                score -= evaluation[y-1][x-1] 

    return score


def static_corner_score(game, board):
    """
    boardを受け取り、評価値を返す。
    角の形を見て点数を算出。
    """

    # -1 -> 何もない, 0 -> 相手の石, 1 -> 自分の石
    # 27通りの評価をする
    # ([盤面], 点数)
    # 盤面評価Ver2: 大きい知識もなく、ひたすら適当に得点付け
    # 対称的な盤面はスコアが正負で一致しているはず
    # 角[石, 石, 石]中央
    evaluation = [
        ([-1, -1, -1],   0),

        ([-1, -1,  1],   1),
        ([-1, -1,  0],  -1),

        ([-1,  0, -1],   2),
        ([-1,  1, -1],  -2),

        ([-1,  0,  0],   8),
        ([-1,  1,  1],  -8),

        ([-1,  0,  1],  12),
        ([-1,  1,  0], -12),

        ([ 1, -1, -1],  10),
        ([ 0, -1, -1], -10),

        ([ 1, -1,  1],   9),
        ([ 0, -1,  0],  -9),

        ([ 1, -1,  0],  10), 
        ([ 0, -1,  1], -10),

        ([ 1,  1, -1],  12),
        ([ 0,  0, -1], -12),

        ([ 1,  1,  1],  15),
        ([ 0,  0,  0], -15),

        ([ 1,  1,  0],  14),
        ([ 0,  0,  1], -14),

        ([ 1,  0, -1],  13),
        ([ 0,  1, -1], -13),

        ([ 1,  0,  1],   6),
        ([ 0,  1,  0],  -6),

        ([ 1,  0,  0],   6),
        ([ 0,  1,  1],  -6)
    ]

    # 四隅を縦横斜めの3方向で見るホーシー
    # テンキーを基準に自分のマス位置を5としたとき、32164987の順で見るべき方向に1を格納
    LU_corner = 0b11010000 # 左上
    RU_corner = 0b01101000 # 右上
    LD_corner = 0b00010110 # 左下
    RD_corner = 0b00001011 # 右下

    # 78946123の順
    direction = [
        (-1, -1), (-1,  0), (-1,  1),
        ( 0, -1),           ( 0,  1),
        ( 1, -1), ( 1,  0), ( 1,  1)
    ]

    corner_order = (LU_corner, RU_corner, LD_corner, RD_corner)
    pos_order = ((1, 1), (1, game.BOARD_WIDTH), (game.BOARD_HEIGHT, 1), (game.BOARD_HEIGHT, game.BOARD_WIDTH)) # board[pos[0]][pos[1]]

    score = 0

    for flg, pos in zip(corner_order, pos_order):
        for i in range(len(direction)):
            if flg & (1 << i) >= 1:
                # 78946123の順でチェック
                condition = [-1 for _ in range(3)]
                
                for j in range(3):
                    # 自分の石→1、相手の石→0、何もない→-1
                    if board[pos[0]+direction[i][0]*j][pos[1]+direction[i][1]*j] == game.turn.to_color():
                        condition[j] = 1
                    elif board[pos[0]+direction[i][0]*j][pos[1]+direction[i][1]*j] == game.opponent(game.turn).to_color():
                        condition[j] = 0
                    else:
                        condition[j] = -1
                
                # 比較
                for e in evaluation:
                    if condition == e[0]:
                        score += e[1]
                        break

    return score


def calc_score(game, board):
        if 'evaluation' in game.turn.kwargs:
            if type(game.turn.kwargs['evaluation']) == type('') and \
               game.turn.kwargs['evaluation'] == 'mixed':
               return static_board_score(game, board) + (static_corner_score(game, board) // 1.5)
            else:
                return game.turn.kwargs['evaluation'](game, board)
        else:
            return static_board_score(game, board)