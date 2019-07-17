# リバーシゲーム
python3環境で動作するリバーシゲーム

## 動作環境
- python3.7
- numpyが入っている
- python3-tkが入っている

### virtualenv
virtualenvを用いて、動作環境を構築できる
<details>
  <summary>bash,zsh</summary>
  ``` shell
  $ virtualenv .
  $ source bin/activate
  $ pip install -r freeze.txt
  ```
</details>
<details>
  <summary>fish</summary>
  ``` shell
  $ virtualenv .
  $ source bin/activate.fish
  $ pip install -r freeze.txt
  ```
</details>

## あそびかた
**vsヒトをする場合、simulate.pyは使用しないでください。**
``` shell
python play.py
```
play_config.jsonに設定を書き込んで、GUI上で対戦ができる。
ヒトvsヒトの対戦も可能。

``` shell
python simulate.py
```
simulate.configに設定を書き込んで、BOT同士の対戦させる。
試合後は棋譜が出力され、[このサイト](http://do2.rojo.jp/shogi/javascript/mania.html?f=1)で確認ができる。

## *_config.jsonについて
playerかbotを2種類用意して、jsonファイルに書き込む。
それぞれの書き方は以下の通り。
### player
自分たちが操作したい場合はこのオブジェクトを用意する。
- name: 自分の名前などを入力。デフォルトはPLAYER。
### bot
botに操作してもらう場合はこのオブジェクトを用意する。
- method: 下にある[実装BOT一覧](https://github.com/Dozi0116/reversi#%E5%AE%9F%E8%A3%85bot%E4%B8%80%E8%A6%A7)から、一つ選ぶ。デフォルトはalpha_beta。
- evaluation: 下にある[実装盤面評価一覧](https://github.com/Dozi0116/reversi#%E5%AE%9F%E8%A3%85%E7%9B%A4%E9%9D%A2%E8%A9%95%E4%BE%A1%E4%B8%80%E8%A6%A7)から、一つ選ぶ。デフォルトはstatic_board_score。
- max_depth: max_min、alpha_beta、mini_maxで必要になる値。何手まで先読みを行うか。デフォルトは2。(2〜4)
- playout_count: montecarlo、soft_maxで必要になる値。プレイアウト(仮想の決着)を何回行うか。デフォルトは100。(10〜500)

---

以下テンプレート

### 対人戦
``` json
{
  "player1": {
    "name": "player 1"
  },
    
  "player2": {
    "name": "player 2"
  }
}
```

### ヒトvsBOT
``` json
{
  "player": {
    "name": "player 1"
  },
    
  "bot": {
    "method": "alpha_beta",
    "max_depth": 4,
    "evaluation": "static_board_score"   
  }
}
```

### BOTvsBOT
``` json
{
  "bot1": {
    "method": "alpha_beta",
    "max_depth": 4,
    "evaluation": "static_board_score"   
  },
    
  "bot2": {
    "method": "soft_max",
    "playout_count": 300,
    "evaluation": "static_board_score"   
  }
}
```

## 実装BOT一覧
- max_min
- random_play
- max_only
- montecarlo
- soft_max
- fixed_mini_max
- alpha_beta
- mini_max

今後も実装予定

## 実装盤面評価一覧
- static_corner_score
- static_board_score

今後も実装予定(BOTより優先的に作りたい)

---

対戦結果はmatchlog.txtに保存され、どの相手が強いかがわかる。

## 更新履歴
2019-07-17 configの書き方を更新