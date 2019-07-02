# リバーシゲーム
python3環境で動作するリバーシゲーム

## あそびかた
python play.py
ファイル内のbot.xxx部分をbot_methods.pyの中にある関数たちに変えれば様々な強さの相手と戦える。

python simurate.py
BOT同士の対戦が見られる。試合後は棋譜が出力され、[このサイト](http://do2.rojo.jp/shogi/javascript/mania.html?f=1)で確認ができる。

## 実装BOT一覧
- max_min
- random_play
- max_only
- soft_max
- fixed_mini_max
- alpha_beta
- mini_max

今後も実装予定

## 実装盤面評価一覧
- static_board_calc

今後も実装予定(BOTより優先的に作りたい)

---

対戦結果はmatchlog.txtに保存され、どの相手が強いかがわかる。