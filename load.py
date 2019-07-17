import json
import reversi
import bot_methods as bot
import static_scores

def parse_config(filename):
    """
    configファイルを読み込んでPlayerオブジェクトを返す
    """

    with open(filename, 'r') as f:
        config = json.load(f)

        obj = []

        for k, v in config.items():
            if k == 'player' or k == 'player1' or k == 'player2':
                name = 'PLAYER'
                if 'name' in v:
                    name = v['name']
                
                obj.append(
                    reversi.Player(
                        'Player',
                        name = name
                    )
                )
            else:
                method = bot.alpha_beta
                if 'method' in v:
                    method = getattr(bot, v['method'])

                max_depth = 2
                if 'max_depth' in v:
                    max_depth = v['max_depth']

                playout_count = 50
                if 'playout_count' in v:
                    playout_count = v['playout_count']

                evaluation = static_scores.static_board_score
                if 'evaluation' in v:
                    if v['evaluation'] == 'mixed':
                        evaluation = 'mixed'
                    else:
                        evaluation = getattr(static_scores, v['evaluation'])

                obj.append(
                    reversi.Player(
                        'AI',
                        method,
                        name='BOT',
                        max_depth=max_depth,
                        playout_count=playout_count,
                        evaluation=evaluation
                    )
                )

    return obj
