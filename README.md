# M5Stick の赤外線リモコン化
## 目次

1. プロトコルの理解 [参考 : ](http://elm-chan.org/docs/ir_format.html)
   1. 送信波の周波数
   2. 日本で多いプロトコルから検討
      - 対象も日本メーカー
      - 長い期間の High でデータの送信の開始、終了
2. HW の理解
   1. M5Stick の GPU はどうやって 38kHzで動かすのか
      - そもそも動かせるのか
      - 既存のライブラリ[IRsend](https://github.com/Arduino-IRremote/Arduino-IRremote/blob/master/src/IRSend.hpp)

    2. M5 の IR Remote 回路図
      - [公式ドキュメント](https://docs.m5stack.com/en/unit/ir)
      - 回路図
        ![回路図 : 公式ドキュメントより](https://static-cdn.m5stack.com/resource/docs/products/unit/ir/ir_sch_01.webp)