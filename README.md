GLCD_TEST
===================
Arduino UNOで正体不明グラフィック液晶を動かしてみました。  
使用チップはHD44104です。（例によってデータシートは見つかりませんでした。）  
入手した型番：LM523という液晶側のピンアサインは以下のとおりです。  
[hardwareluxx](http://www.hardwareluxx.de/community/f141/serielles-lcd-ansteuern-hitachi-lm200-72514-print.html "hardwareluxx")
より  
  
1:D1 上位32ライン分表示用シリアルデータ  
2:FLM First Line Marker スタートラインを示す信号  
3:M 液晶の表示をコントロールする信号。1フレームごとにH/Lを切り替えて送信  
4:CL1 水平割り込み。1ラインシフトレジスタラッチ信号  
5:CL2 D1,D2用シリアルデータ転送用クロック  
6:D2 下位32ライン分表示用シリアルデータ  
7:Vdd 5V  
8:GNC  
9:Vee -5V 液晶パネル駆動用電源。  
10:コントラスト調整用。5VとGND間にVRを入れて、電圧を調整することにより、液晶のコントラストを調整できる。  


