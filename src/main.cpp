#include <Arduino.h>
#include <M5GFX.h>
#include <M5Unified.h>

#define LENGTH 66 

enum SIG_VALUE {
  SIG_LOW=1,
  SIG_HIGH=0
};

enum IR_STATE {
  IDLE=0,
  LEADER,
  END,
  DATA
};

struct packet_t {
  u_int16_t leader; 
  u_int8_t cur_pos = 0;
  u_int16_t data[LENGTH];
  u_int8_t data_value[LENGTH];

} packet;

// TOSHIBA NLER007-LC : Turned off : NEC format 
u_int16_t tmp_data[] = {
   // Leader
   562*16, 562*8,
   // customer code ( ID ) 
   562*1,562*3, 562*1,562*3, 562*1,562*3, 562*1,562*1,
   562*1,562*1, 562*1,562*3, 562*1,562*3, 562*1,562*3,
   562*1,562*1, 562*1,562*1, 562*1,562*3, 562*1,562*3,
   562*1,562*1, 562*1,562*1, 562*1,562*1, 562*1,562*1,
   // Data
   562*1,562*3, 562*1,562*3, 562*1,562*1, 562*1,562*3,
   562*1,562*1, 562*1,562*1, 562*1,562*1, 562*1,562*3,
   562*1,562*1, 562*1,562*1, 562*1,562*3, 562*1,562*1,
   562*1,562*3, 562*1,562*3, 562*1,562*3, 562*1,562*1,
   // END
   562*16, 562*8
};

u_int16_t start_sig_time   = 0;
u_int16_t duration = 0;
u_int32_t threathold_leader = 2400;
u_int32_t threathold_data = 350;

SIG_VALUE pre_value = SIG_LOW;
IR_STATE  state=IDLE;

void send_ir_by_nec_format(u_int16_t data[], u_int16_t length);
void update_state();
void update_values();

void setup(void) {
  auto cfg = M5.config();
  M5.begin(cfg);
  pinMode(33, INPUT); // IR Input
  pinMode(32, OUTPUT); // IR Output
}

void loop(void) {
  SIG_VALUE cur_sig_value; 

  M5.update();
  cur_sig_value = (SIG_VALUE) digitalRead(33);

  if(M5.BtnA.wasPressed()){
    send_ir_by_nec_format(tmp_data, 68);
  } else if( cur_sig_value != pre_value )  {
    update_state();
    update_values();
    pre_value = cur_sig_value;
  }
}

void send_ir_by_nec_format(u_int16_t data[], u_int16_t length)
{
  unsigned long cur_time = micros();
  M5.Display.startWrite();
  M5.Display.fillScreen(BLACK);
  M5.Display.setCursor(5, 0);
  M5.Display.printf("Send IR > ");
  M5.Display.endWrite();
  for( int i=0; i < length / 2 ; i++){ 
    cur_time = micros();
    while(tmp_data[i*2]>micros()-cur_time){
      digitalWrite(32,1);
      delayMicroseconds(13); // 1/38(khz) / 2
      digitalWrite(32,0);
      delayMicroseconds(13); // 1/38(khz) / 2
    }
    digitalWrite(32,0);
    delayMicroseconds(tmp_data[i*2+1]);
  }
  M5.Display.startWrite();
  M5.Display.setCursor(5, 5*10);
  M5.Display.printf("Done.");
  M5.Display.endWrite();
}
void update_state(){
  unsigned long cur_time;
  cur_time = micros();
  duration = cur_time - start_sig_time;
  if( state==IDLE ) {
    if( pre_value == SIG_HIGH && duration > threathold_leader ) {
      state = LEADER;
    } else {
      state = IDLE;
    }
  } else if (state == LEADER) {
    state = DATA;
  } else if (state == DATA ) { 
    if( pre_value == SIG_LOW && duration > threathold_leader ) {
      state = END;
    } else if (duration > threathold_data) {
      state = DATA;
    }
  } else if (state == END) {
    state = IDLE;
  }
  start_sig_time = cur_time;
}

void update_values(){
  if(state == END){
    M5.Display.startWrite();
    M5.Display.fillScreen(BLACK);
    M5.Display.setCursor(5, 0);
    M5.Display.printf("leader : %010d", packet.leader);
    M5.Display.endWrite();
    Serial.printf("Start : [%d", packet.leader);
    for(int i=0; i < packet.cur_pos; i++){
      Serial.printf(",%d", packet.data[i]);
    }
    Serial.printf("]");
  } else if (state==LEADER) {
    packet.leader = duration;
    packet.cur_pos = 0;
  } else if ( state == DATA) {
    if( packet.cur_pos < LENGTH){
      packet.data[packet.cur_pos] = duration;
      packet.data_value[packet.cur_pos] = ( pre_value == SIG_HIGH )? 1 : 0;
      packet.cur_pos++;
    }
  } else if (state==IDLE){
    //NOP
  }
}