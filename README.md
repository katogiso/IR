# M5Stick の赤外線リモコン化
## 目次

1. プロトコルの理解 [参考 : ](http://elm-chan.org/docs/ir_format.html)
   1. 送信波の周波数
   2. 日本で多いプロトコルから検討
      - 対象も日本メーカー
      - 長い期間の High でデータの送信の開始、終了
2. HW の理解
   1. M5Stick の GPIP はどうやって 38kHzで動かすのか
      - そもそも動かせるのか

         ESP32(CPU)のクロックが240MHz [SwitchScience 商品情報](https://www.switch-science.com/products/9350)

      - 既存のライブラリ[IRsend](https://github.com/Arduino-IRremote/Arduino-IRremote/blob/master/src/IRSend.hpp)

         こちらも GPIO を直接動かしているので、プログラムから直接ドライブ可能だと考えられる。
      
      - オシロスコープで確認

      ![波形](resources/LabNation_Screenshot3.png)

   2. M5 の IR Remote 回路図
      - [公式ドキュメント](https://docs.m5stack.com/en/unit/ir)
      - 回路図
        ![回路図 : 公式ドキュメントより](https://static-cdn.m5stack.com/resource/docs/products/unit/ir/ir_sch_01.webp)
      - 受信 IC

         [IRM-3638T](https://www.mouser.jp/datasheet/2/143/EVER_S_A0007513414_1-2548705.pdf)

         ブロックダイヤグラムから 38kHz の搬送波はフィルタリング（検波）されて信号があれば Low, 信号が無ければ High となって INPUT に出てくるので、結局 570uSec程度のトグルをCPUで検出できれば受信ができる
      - 出力
         GPIO を 38kHzでトグルさせる必要がある

3. 正規のリモコンからデータを取得する
   1. IR Remote の回路図から INPUT Port (G33) をloop()で読み取る 

      setup() は起動時に実行する内容を記述

      loop() は起動中繰り返し実行する内容を記述

   2. サンプリング定理とリモコンのプロトコルから 1/(570x10^-6[sec]) x 2 よりもGPIOのデータを読み取れれば良い
   
      受信結果は Serial で VS code の端末へ出力する
   
   3. 受信波形
      ![波形](resources/LabNation_Screenshot5.png)

   4. データのフォーマット解析

      人力で解析する。今回は NEC フォーマットだった。 

4. データを送信する
   1. フォーマット通りに送信する 
   2. データを送信
      ![波形](resources/LabNation_Screenshot8.png)
