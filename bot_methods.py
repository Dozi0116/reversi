# -*- Coding: utf-8 -*-

import copy
import random

def changeable_mini_max(game):
    """
    ミニマックス法を可変的にできるようにした。

    奇数手先読み…最高の手を持ってくる
    偶数手先読み…最悪の手を持ってくる

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
    

    def min_calc(game, root_board, depth_limit, stone_num):
        board = root_board[:]
        if depth_limit <= 0 or stone_num  > game.BOARD_HEIGHT * game.BOARD_WIDTH:
            return calc_score(game, board)

        min_score = 9999
        putlist = game.make_putlist(game.opponent(game.turn), board)
        if len(putlist) == 0:
            # 敵が置けない -> OK -> 最高評価
            # print('min_score -> {}'.format(min_score))
            return min_score

        for pos in putlist:
            # print((' ' * depth_limit) + 'O press ({}, {})'.format(pos[0], pos[1]))
            board = copy.deepcopy(root_board)

            board = game.put_stone(pos, game.opponent(game.turn), test=True, test_board=board)
            score = max_calc(game, board, depth_limit - 1, stone_num + 1)

            if score < min_score:
                min_score = score

        # print('min_score -> {}'.format(min_score))
        return min_score


    def max_calc(game, root_board, depth_limit, stone_num):
        board = root_board[:]
        if depth_limit <= 0 or stone_num  > game.BOARD_HEIGHT * game.BOARD_WIDTH:
            return calc_score(game, board)

        max_score = -9999
        putlist = game.make_putlist(game.turn, board)
        if len(putlist) == 0:
            # 自分が置けない -> NG -> 最低評価
            # print('max_score -> {}'.format(max_score))
            return max_score

        for pos in putlist:
            # print((' ' * depth_limit) + 'P press ({}, {})'.format(pos[0], pos[1]))
            board = copy.deepcopy(root_board)

            board = game.put_stone(pos, game.turn, test=True, test_board=board)
            score = min_calc(game, board, depth_limit - 1, stone_num + 1)

            if score > max_score:
                max_score = score

        # print('max_score -> {}'.format(max_score))
        return max_score


    MAX_DEPTH = 2
    if 'max_depth' in game.turn.kwargs:
        MAX_DEPTH = game.turn.kwargs['max_depth']

    max_score = -9999
    max_pos = game.putlist[0]
    for pos in game.putlist:
        # print('PLAYER press ({}, {})'.format(pos[0], pos[1]))
        board = game.put_stone(pos, game.turn, test=True)
        score = min_calc(game, board, MAX_DEPTH-1, game.stone_num)

        if score > max_score:
            max_score = score
            max_pos = pos

    # print('press ({}, {})'.format(max_pos[0], max_pos[1]))

    return max_pos


    

def mini_max(game):
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
        # print('if put ({}, {})'.format(pos[0], pos[1]))

        # 2手先読み
        next_poslist = game.make_putlist(game.opponent(game.turn), board)
        min_score = 9999
        if len(next_poslist) == 0:
            # 連続で打てる -> 最強
            return pos
            
        min_pos = next_poslist[0]
        for next_pos in next_poslist:
            next_board = game.put_stone(next_pos, game.opponent(game.turn), test=True, test_board=board)
            next_score = calc_score(game, next_board)
            # print('    next put ({}, {}) -> {} points'.format(next_pos[0], next_pos[1], next_score))

            if next_score < min_score:
                min_score = next_score
                min_pos = next_pos

        # print('        min score is {} points'.format(min_score))

        if min_score > max_score:
            max_score = min_score
            max_pos = pos


    # print("press ({}, {})".format(max_pos[0], max_pos[1]))
    return max_pos



def max_only(game):
    """
    マックス法。
    1手先まで読んで、最善の手を選択。
    """

    def calc_score(game, board):
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
        # print('if put ({}, {}) -> {} points'.format(pos[0], pos[1], score))
        if score > max_score:
            max_score = score
            max_pos = pos


    # print("press ({}, {})".format(max_pos[0], max_pos[1]))
    return max_pos



def random_play(game):
    """
    ランダムプレイアウト。
    おける手から適当な手を返す
    """
    put_pos = random.choice(game.putlist)

    # print('press ({}, {})'.format(put_pos[0], put_pos[1]))

    return put_pos



def montecalro(game):
    """
    モンテカルロ法。
    おける手をランダムプレイアウトする
    """

    def playout(game, board):
        """
        ランダムプレイアウトの関数
        """

        player = game.opponent(game.turn)

        putlist = game.make_putlist(player, board)

        is_game_end = False

        if len(putlist) == 0:
            player = game.opponent(player)
            putlist = game.make_putlist(player, board)

            if len(putlist) == 0:
                is_game_end = True

        while is_game_end == False:
            pos = random.choice(putlist) # 変更あり！→盤面評価に基づくなど
            board = game.put_stone(pos, player, test=True, test_board=board)
            player = game.opponent(player)
            putlist = game.make_putlist(player, board)

            if len(putlist) == 0:
                player = game.opponent(player)
                putlist = game.make_putlist(player, board)

                if len(putlist) == 0:
                    is_game_end = True

        score = 0
        for col in board:
            for cell in col:
                if cell == game.turn.to_color():
                    score += 1
                elif cell == game.opponent(game.turn).to_color():
                    score -= 1


        if score > 0:
            return 1
        elif score == 0:
            return 0
        else:
            return -1
            

    # 何回プレイアウトを行うか。デフォは100回
    playout_count = 100
    if 'playout_count' in game.turn.kwargs:
        playout_count = game.turn.kwargs['playout_count']

    
    max_win_points = -playout_count # 全負けでこの数字
    max_win_pos = [game.putlist[0]] # リスト管理する。

    for pos in game.putlist:
        win_points = 0
        root_board = game.put_stone(pos, game.turn, test=True)

        for i in range(playout_count):
            board = copy.deepcopy(root_board)
            result = playout(game, board)
            win_points += result # 勝利 -> +1, 引き分け -> +0, 負け -> -1

        print('if put ({}, {}) -> {}point(s)'.format(pos[0], pos[1], win_points))
        
        if win_points > max_win_points:
            max_win_points = win_points
            max_win_pos = [pos]
        elif win_points == max_win_points:
            max_win_pos.append(pos)

    put_pos = random.choice(max_win_pos)
    # print('press ({}, {})'.format(put_pos[0], put_pos[1]))
        
    return put_pos



def changeable_alpha_beta(game):
    """
    アルファベータ法。

    max_calc -> 現時点の最高値をmin_calcに送る。
    min_calcは最高値を下回ったら探索を打ち切る(どうあがいても送られてきた最高値に勝てないため)。
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
    

    def min_calc(game, root_board, depth_limit, stone_num, beta):
        board = root_board[:]
        if depth_limit <= 0 or stone_num  > game.BOARD_HEIGHT * game.BOARD_WIDTH:
            return calc_score(game, board)

        min_score = 9999
        putlist = game.make_putlist(game.opponent(game.turn), board)
        if len(putlist) == 0:
            # 敵が置けない -> OK -> 最高評価
            # print('min_score -> {}'.format(min_score))
            return min_score

        for pos in putlist:
            # print((' ' * depth_limit) + 'O press ({}, {})'.format(pos[0], pos[1]))
            board = copy.deepcopy(root_board)

            board = game.put_stone(pos, game.opponent(game.turn), test=True, test_board=board)
            score = max_calc(game, board, depth_limit - 1, stone_num + 1, min_score)

            if score < min_score:
                min_score = score

            if min_score < beta:
                break

        # print('min_score -> {}'.format(min_score))
        return min_score


    def max_calc(game, root_board, depth_limit, stone_num, alpha):
        board = root_board[:]
        if depth_limit <= 0 or stone_num  > game.BOARD_HEIGHT * game.BOARD_WIDTH:
            return calc_score(game, board)

        max_score = -9999
        putlist = game.make_putlist(game.turn, board)
        if len(putlist) == 0:
            # 自分が置けない -> NG -> 最低評価
            # print('max_score -> {}'.format(max_score))
            return max_score

        for pos in putlist:
            # print((' ' * depth_limit) + 'P press ({}, {})'.format(pos[0], pos[1]))
            board = copy.deepcopy(root_board)

            board = game.put_stone(pos, game.turn, test=True, test_board=board)
            score = min_calc(game, board, depth_limit - 1, stone_num + 1, max_score)

            if score > max_score:
                max_score = score

            if max_score > beta:
                break

        # print('max_score -> {}'.format(max_score))
        return max_score


    MAX_DEPTH = 2
    if 'max_depth' in game.turn.kwargs:
        MAX_DEPTH = game.turn.kwargs['max_depth']

    max_score = -9999
    max_pos = game.putlist[0]
    for pos in game.putlist:
        # print('PLAYER press ({}, {})'.format(pos[0], pos[1]))
        board = game.put_stone(pos, game.turn, test=True)
        score = min_calc(game, board, MAX_DEPTH-1, game.stone_num, -9999) # alpha -> min_calcで使う。

        if score > max_score:
            max_score = score
            max_pos = pos

    # print('press ({}, {})'.format(max_pos[0], max_pos[1]))

    return max_pos