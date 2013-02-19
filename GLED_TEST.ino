//モノクログラフィック液晶動作テストプログラム
//型番：LM523 HD44104チップ使用
//64*240dot

//ピン定義
#define D1 0   //上半分シリアルデータ
#define FLM 1  //First Line Maker　先頭ライン識別信号
#define M 2    //液晶の表示をコントロールする信号。1フレームごとにH/Lを切り替えて送信
#define CL1 3  //シフトレジスタラッチ信号。1ライン描画データを送信毎にHigh
#define CL2 4  //シリアルデータクロック
#define D2 5   //下半分シリアルデータ。VRAMの関係で未使用

//VRAM(Y,X)
uint8_t vram [32][30];  //本当は[64][30]としたいが、メモリが足りないので高さ半分

//変数宣言
//高速化のため、直接制御する
uint8_t bit_D1;
volatile uint8_t *out_D1;
uint8_t bit_FLM ;
volatile uint8_t *out_FLM ;
uint8_t bit_M ;
volatile uint8_t *out_M ;
uint8_t bit_CL1 ;
volatile uint8_t *out_CL1; 
uint8_t bit_CL2 ;
volatile uint8_t *out_CL2 ;
uint8_t bit_D2 ;
volatile uint8_t *out_D2 ;

void setup(){
  pinMode(D1,OUTPUT);
  pinMode(FLM,OUTPUT);
  pinMode(M,OUTPUT);
  pinMode(CL1,OUTPUT);
  pinMode(CL2,OUTPUT);
  pinMode(D2,OUTPUT);
  //各ポートセット
  bit_D1 = digitalPinToBitMask(D1);
  out_D1 = portOutputRegister(digitalPinToPort(D1));
  bit_FLM = digitalPinToBitMask(FLM);
  out_FLM = portOutputRegister(digitalPinToPort(FLM));
  bit_M = digitalPinToBitMask(M);
  out_M = portOutputRegister(digitalPinToPort(M));
  bit_CL1 = digitalPinToBitMask(CL1);
  out_CL1 = portOutputRegister(digitalPinToPort(CL1));
  bit_CL2 = digitalPinToBitMask(CL2);
  out_CL2 = portOutputRegister(digitalPinToPort(CL2));
  bit_D2 = digitalPinToBitMask(D2);
  out_D2 = portOutputRegister(digitalPinToPort(D2));

  //init
  //初期クロックとFLM信号を送信し、カーソル？を0,0に戻す
  //クロックは空クロックを送信しても特に問題ななさそう。
  //動かないときは空クロックを送ると動いたりする。（Wait??)
  /*
  digitalWrite(D1,LOW);
   digitalWrite(D2,LOW);
   digitalWrite(CL1,LOW);
   digitalWrite(CL2,HIGH);
   digitalWrite(CL1,HIGH);
   digitalWrite(CL2,LOW);
   digitalWrite(CL1,LOW);
   digitalWrite(CL2,HIGH);
   */
  digitalWrite(FLM,HIGH);  //FLMは一番最初の1ラインだけはHighにしないといけないみたい。
  digitalWrite(M,LOW);
  //今回はVRAMの関係で下半分の描画はなし
  digitalWrite(D2,LOW);
  //VRAMクリア
  clear_vram();
  //表示
  output_lcd();
}
void clear_vram(){
  uint8_t x;
  uint8_t y;
  for (y=0;y<32;++y){
    for(x=0;x<30;++x){
      vram[y][x] = 0x00;
    }
  }
  //表示テストデータ
  //vram[0][0]=0x80;
}
void output_lcd(){
  uint8_t tmp ;
  uint8_t state = 0;//Mのフラグ管理
  //uint8_t tmp2;
  for(uint8_t y=0;y<32;++y){
    //縦ループ
    for(uint8_t x=0;x<30;++x){
      //横ループ
      tmp = vram[y][x];  //上半分の領域用1Byte
      //tmp2 = vram[y+32][x];  //下半分の領域用1Byte     
      for(uint8_t i=0;i<8;++i){
        //VRAMの1Byteを1bitずつシフトしながら液晶のシフトレジスタへ転送する。
        //D1とD2を両方セットした後確定クロックを送信しないといけないので、ShiftOut関数が使えない。
        //(今回はD1だけなので使えるけど）

        //指定したビットだけ立てる
        // 例）2 And 0x80 
        // 00000010 And 10000000 = 00000000
        if (tmp & 0x80){
          *out_D1 |= bit_D1;//Hi
          //digitalWrite(D1,HIGH);
        }
        else{
          *out_D1 &= ~bit_D1;//Lo
          //digitalWrite(D1,LOW);
        }
        /*
        if (tmp2 & 0x80){
         *out_D2 |= bit_D2;
         }
         else{
         *out_D2 &= ~bit_D2;
         }
         */
        //送信クロック
        //digitalWrite(CL2,LOW);
        *out_CL2 &= ~bit_CL2; //Lo
        //digitalWrite(CL2,HIGH);
        *out_CL2 |= bit_CL2;//Hi
        //Shift
        tmp <<=1;
        //tmp2 <<=1;
      }
    }
    //液晶シフトレジスタ確定
    //念のためCL2も送信していたが、なくても動作する？
    //digitalWrite(CL2,LOW);
    //*out_CL2 &= ~bit_CL2;
    //digitalWrite(CL1,HIGH);
    *out_CL1 |= bit_CL1;
    //digitalWrite(CL2,HIGH);
    //*out_CL2 |= bit_CL2;
    //digitalWrite(CL1,LOW);
    *out_CL1 &= ~bit_CL1;
    //digitalWrite(FLM,LOW);
    //1ライン転送したので、Loにする
    *out_FLM &= ~bit_FLM;
    //Mを操作(1フレーム転送ごとにH/L切り替え
    state = 1- state;
    if (state ==1){
      *out_M |= bit_M;
    }
    else{
      *out_M &= ~bit_M;
    }
  }
  //1ライン転送完了
  //digitalWrite(FLM,HIGH);
  *out_FLM |= bit_FLM;
}
void loop(){
  // delay(1000);
  //output_lcd();
  test_write();
}
void test_write(){
  //動作チェック用
  uint8_t tmp;
  for (uint8_t y=0;y<32;++y){
    for(uint8_t x=0;x<30;++x){
      tmp=0x80;
      for(uint8_t i=0;i<8;++i){
        vram[y][x] = vram[y][x]+tmp;
        tmp >>=1;
        output_lcd();
        //clear_vram();
      }
    }
  }
  clear_vram();
}






