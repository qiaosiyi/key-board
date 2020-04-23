#include "key.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "usart3.h"
#include "stm32f10x.h"
#include "string.h"

#include "append.h"

void KEY_Init(void){
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB| RCC_APB2Periph_GPIOC,ENABLE);

  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_11 | GPIO_Pin_12 ;
  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;
  GPIO_Init(GPIOA,&GPIO_InitStructure);


  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_3 | GPIO_Pin_4|
          GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8|
          GPIO_Pin_9;         //
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;     //
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_Init(GPIOB,&GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;         //14AT 15CLEAR
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;     //
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOC,&GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;         //13LINK STATUS
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;     //
  GPIO_Init(GPIOC,&GPIO_InitStructure);
}


u8* check_cmd(u8 *str)
{
  char *strx=0;
  if(USART_RX_STA&0X8000)  
  { 
  USART_RX_BUF[USART_RX_STA&0X7FFF]=0;
  strx=strstr((const char*)USART_RX_BUF,(const char*)str);
  } 
  return (u8*)strx;
}



u8 send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
  u8 res=0; 
  USART_RX_STA=0;
  if((u32)cmd<=0XFF)
  {
  while((USART1->SR&0X40)==0);//
  USART1->DR=(u32)cmd;
  }else printf("%s\r\n",cmd);// 
  if(ack&&waittime)  // 
  {
  while(--waittime)  //µÈ´ýµ¹¼Æ•J
  {
    delay_ms(10);
    if(USART_RX_STA&0X8000)//½ÓÊÕµ½ÆÚ´ýµÄÓ¦´ð½á¹û
    {
    if(check_cmd(ack))break;//µÃµ½ÓÐÐ§Êý¾Ý 
    USART_RX_STA=0;
    } 
  }
  if(waittime==0)res=1; 
  }
  return res;
} 

void setrow0_0(void){ROW0_0;ROW1_0;ROW2_0;ROW3_0;ROW4_0;ROW5_0;ROW6_0;ROW7_0;ROW8_0;}
void setrow0_1(void){ROW0_1;ROW1_0;ROW2_0;ROW3_0;ROW4_0;ROW5_0;ROW6_0;ROW7_0;ROW8_0;}
void setrow1_1(void){ROW0_0;ROW1_1;}
void setrow2_1(void){ROW1_0;ROW2_1;}
void setrow3_1(void){ROW2_0;ROW3_1;}
void setrow4_1(void){ROW3_0;ROW4_1;}
void setrow5_1(void){ROW4_0;ROW5_1;}
void setrow6_1(void){ROW5_0;ROW6_1;}
void setrow7_1(void){ROW6_0;ROW7_1;}
void setrow8_1(void){ROW7_0;ROW8_1;}
void setrow9_1(void){ROW0_1;ROW1_1;ROW2_1;ROW3_1;ROW4_1;ROW5_1;ROW6_1;ROW7_1;ROW8_1;}

// if any col has valid data
// for example
// col0 col1 col2 col3 col4 col5 col6 col7
//  0   1  0   1  0   0  0   0
u8 read_inputdata(void)
{
  static u8 byte[2] = {0, 0};  
  byte[0] = 0;          
  if(COL0){          
  byte[0] |= 0x80;//bit 1000 0000
  byte[1]++;
  }
  if(COL1){
  byte[0] |= 0x40;//bit 0100 0000
  byte[1]++;
  }  
  if(COL2){
  byte[0] |= 0x20;//bit 0010 0000
  byte[1]++;
  }  
  if(COL3){
  byte[0] |= 0x10;//bit 0001 0000
  byte[1]++; 
  }  
  if(COL4){
  byte[0] |= 0x08;//bit 0000 1000
  byte[1]++;
  }  
  if(COL5){
  byte[0] |= 0x04;//bit 0000 0100
  byte[1]++;
  }  
  if(COL6){
  byte[0] |= 0x02;//bit 0000 0010
  byte[1]++;
  }  
  if(COL7){
  byte[0] |= 0x01;//bit 0000 0001
  byte[1]++;
  }  
  //u3_printf("read_inputdata:%x\r\n",byte[0]);
  return byte[0];
}

u8 *Read_KeyValue(void)
{
  u8 i = 0;
  u8 temp_byte[16];
  static u8 byte[16];
  
  for(i = 0; i < 16; i++)
  byte[i] = 0x00;

  setrow0_1();delay_us(50);temp_byte[0] |= read_inputdata();//get input col value  ·À¶¶ÊÆ
  setrow1_1();delay_us(50);temp_byte[1] |= read_inputdata();//get input col value  
  setrow2_1();delay_us(50);temp_byte[2] |= read_inputdata();//get input col value  
  setrow3_1();delay_us(50);temp_byte[3] |= read_inputdata();//get input col value  
  setrow4_1();delay_us(50);temp_byte[4] |= read_inputdata();//get input col value  
  setrow5_1();delay_us(50);temp_byte[5] |= read_inputdata();//get input col value  
  setrow6_1();delay_us(50);temp_byte[6] |= read_inputdata();//get input col value  
  setrow7_1();delay_us(50);temp_byte[7] |= read_inputdata();//get input col value  
  setrow8_1();delay_us(50);temp_byte[8] |= read_inputdata();//get input col value  
  setrow9_1();
  setrow0_1();delay_us(50);byte[0] |= read_inputdata();//get input col value  
  setrow1_1();delay_us(50);byte[1] |= read_inputdata();//get input col value  
  setrow2_1();delay_us(50);byte[2] |= read_inputdata();//get input col value  
  setrow3_1();delay_us(50);byte[3] |= read_inputdata();//get input col value  
  setrow4_1();delay_us(50);byte[4] |= read_inputdata();//get input col value  
  setrow5_1();delay_us(50);byte[5] |= read_inputdata();//get input col value  
  setrow6_1();delay_us(50);byte[6] |= read_inputdata();//get input col value  
  setrow7_1();delay_us(50);byte[7] |= read_inputdata();//get input col value  
  setrow8_1();delay_us(50);byte[8] |= read_inputdata();//get input col value  
  setrow9_1();

  while(temp_byte[0] != byte[0] | temp_byte[1] != byte[1] | temp_byte[2] != byte[2] | temp_byte[3] != byte[3] | 
  temp_byte[4] != byte[4] | temp_byte[5] != byte[5] | temp_byte[6] != byte[6] | temp_byte[7] != byte[7] | 
  temp_byte[8] != byte[8])
  {
  for(i = 0; i < 9; i++)
    temp_byte[i] = byte[i];
  
  setrow0_1();delay_us(50);byte[0] |= read_inputdata();//get input col value  
  setrow1_1();delay_us(50);byte[1] |= read_inputdata();//get input col value  
  setrow2_1();delay_us(50);byte[2] |= read_inputdata();//get input col value  
  setrow3_1();delay_us(50);byte[3] |= read_inputdata();//get input col value  
  setrow4_1();delay_us(50);byte[4] |= read_inputdata();//get input col value  
  setrow5_1();delay_us(50);byte[5] |= read_inputdata();//get input col value  
  setrow6_1();delay_us(50);byte[6] |= read_inputdata();//get input col value  
  setrow7_1();delay_us(50);byte[7] |= read_inputdata();//get input col value  
  setrow8_1();delay_us(50);byte[8] |= read_inputdata();//get input col value  
  setrow9_1();
  }
  
  return byte;
}

u8 *map_2_key(u8 *read_byte)
{
  static u8 byte[7] = {0,0,0,0,0,0,0};
  u8 i = 0;
  u8 fn = 0;
  u8 app = 0;
  for(i = 0; i < 7; i++)
  byte[i] = 0x00;

  i = 1;
  if(read_byte[0])
  {
  if((read_byte[0] & 0x80) == 0x80){byte[i] = key_esc;i++;if(i == 7){return byte;}}
  if((read_byte[0] & 0x40) == 0x40){byte[i] = key_1;i++;  if(i == 7){return byte;}}
  if((read_byte[0] & 0x20) == 0x20){byte[i] = key_2;i++;  if(i == 7){return byte;}}
  if((read_byte[0] & 0x10) == 0x10){byte[i] = key_3;i++;  if(i == 7){return byte;}}
  if((read_byte[0] & 0x08) == 0x08){byte[i] = key_4;i++;  if(i == 7){return byte;}}
  if((read_byte[0] & 0x04) == 0x04){byte[i] = key_5;i++;  if(i == 7){return byte;}}
  if((read_byte[0] & 0x02) == 0x02){byte[i] = key_6;i++;  if(i == 7){return byte;}}
  if((read_byte[0] & 0x01) == 0x01){byte[i] = key_7;i++;  if(i == 7){return byte;}}
  }

  if(read_byte[1])
  {
  if((read_byte[1] & 0x80) == 0x80){byte[i] = key_tab;i++;if(i == 7){return byte;}}
  if((read_byte[1] & 0x40) == 0x40){byte[i] = key_q;i++;  if(i == 7){return byte;}}
  if((read_byte[1] & 0x20) == 0x20){byte[i] = key_w;i++;  if(i == 7){return byte;}}
  if((read_byte[1] & 0x10) == 0x10){byte[i] = key_e;i++;  if(i == 7){return byte;}}
  if((read_byte[1] & 0x08) == 0x08){byte[i] = key_r;i++;  if(i == 7){return byte;}}
  if((read_byte[1] & 0x04) == 0x04){byte[i] = key_T;i++;  if(i == 7){return byte;}}
  if((read_byte[1] & 0x02) == 0x02){byte[i] = key_y;i++;  if(i == 7){return byte;}}
  if((read_byte[1] & 0x01) == 0x01){byte[i] = key_u;i++;  if(i == 7){return byte;}}
  }

  if(read_byte[2])
  {
  if((read_byte[2] & 0x80) == 0x80){byte[i] = key_caps_lock;i++;if(i == 7){return byte;}}
  if((read_byte[2] & 0x40) == 0x40){byte[i] = key_a;i++;  if(i == 7){return byte;}}
  if((read_byte[2] & 0x20) == 0x20){byte[i] = key_s;i++;  if(i == 7){return byte;}}
  if((read_byte[2] & 0x10) == 0x10){byte[i] = key_d;i++;  if(i == 7){return byte;}}
  if((read_byte[2] & 0x08) == 0x08){byte[i] = key_f;i++;  if(i == 7){return byte;}}
  if((read_byte[2] & 0x04) == 0x04){byte[i] = key_g;i++;  if(i == 7){return byte;}}
  if((read_byte[2] & 0x02) == 0x02){byte[i] = key_h;i++;  if(i == 7){return byte;}}
  if((read_byte[2] & 0x01) == 0x01){byte[i] = key_j;i++;  if(i == 7){return byte;}}
  }

  if(read_byte[3])
  {
  if((read_byte[3] & 0x80) == 0x80){byte[0] |= L_Shi;}
  if((read_byte[3] & 0x40) == 0x40){byte[i] = key_insert;i++;  if(i == 7){return byte;}}
  if((read_byte[3] & 0x20) == 0x20){byte[i] = key_z;i++;  if(i == 7){return byte;}}
  if((read_byte[3] & 0x10) == 0x10){byte[i] = key_x;i++;  if(i == 7){return byte;}}
  if((read_byte[3] & 0x08) == 0x08){byte[i] = key_c;i++;  if(i == 7){return byte;}}
  if((read_byte[3] & 0x04) == 0x04){byte[i] = key_v;i++;  if(i == 7){return byte;}}
  if((read_byte[3] & 0x02) == 0x02){byte[i] = key_b;i++;  if(i == 7){return byte;}}
  if((read_byte[3] & 0x01) == 0x01){byte[i] = key_n;i++;  if(i == 7){return byte;}}
  }

  if(read_byte[4])
  {
  if((read_byte[4] & 0x80) == 0x80){byte[0] |= R_Ctl;}
  if((read_byte[4] & 0x40) == 0x40){byte[0] |= L_Gui;}
  if((read_byte[4] & 0x10) == 0x10){byte[0] |= L_Alt;}
  if((read_byte[4] & 0x20) == 0x20){byte[i] = L_APP;i++;  if(i == 7){return byte;}}
  if((read_byte[4] & 0x08) == 0x08){byte[i] = L_FN;i++;  if(i == 7){return byte;}}
  if((read_byte[4] & 0x04) == 0x04){byte[i] = key_spacebar;i++;  if(i == 7){return byte;}}
  if((read_byte[4] & 0x02) == 0x02){byte[i] = key_home;i++;  if(i == 7){return byte;}}
  if((read_byte[4] & 0x01) == 0x01){byte[i] = key_end;i++;  if(i == 7){return byte;}}
  }

  if(read_byte[5])
  {
  if((read_byte[5] & 0x80) == 0x80){byte[i] = key_8;i++;if(i == 7){return byte;}}
  if((read_byte[5] & 0x40) == 0x40){byte[i] = key_9;i++;  if(i == 7){return byte;}}
  if((read_byte[5] & 0x20) == 0x20){byte[i] = key_0;i++;  if(i == 7){return byte;}}
  if((read_byte[5] & 0x10) == 0x10){byte[i] = key_minus;i++;  if(i == 7){return byte;}}
  if((read_byte[5] & 0x08) == 0x08){byte[i] = key_equal;i++;  if(i == 7){return byte;}}
  if((read_byte[5] & 0x04) == 0x04){byte[i] = key_backslash;i++;  if(i == 7){return byte;}}
  if((read_byte[5] & 0x02) == 0x02){byte[i] = key_backspace;i++;  if(i == 7){return byte;}}
  if((read_byte[5] & 0x01) == 0x01){byte[i] = key_grave;i++;  if(i == 7){return byte;}}
  }

  if(read_byte[6])
  {
  if((read_byte[6] & 0x80) == 0x80){byte[i] = key_i;i++;if(i == 7){return byte;}}
  if((read_byte[6] & 0x40) == 0x40){byte[i] = key_k;i++;  if(i == 7){return byte;}}
  if((read_byte[6] & 0x20) == 0x20){byte[i] = key_l;i++;  if(i == 7){return byte;}}
  if((read_byte[6] & 0x10) == 0x10){byte[i] = key_semicolon;i++;  if(i == 7){return byte;}}
  if((read_byte[6] & 0x08) == 0x08){byte[i] = key_o;i++;  if(i == 7){return byte;}}
  if((read_byte[6] & 0x04) == 0x04){byte[i] = key_p;i++;  if(i == 7){return byte;}}
  if((read_byte[6] & 0x02) == 0x02){byte[i] = key_brace1;i++;  if(i == 7){return byte;}}
  if((read_byte[6] & 0x01) == 0x01){byte[i] = key_brace2;i++;  if(i == 7){return byte;}}
  }

  if(read_byte[7])
  {
  if((read_byte[7] & 0x80) == 0x80){byte[i] = key_m;i++;if(i == 7){return byte;}}
  if((read_byte[7] & 0x40) == 0x40){byte[i] = key_comma;i++;  if(i == 7){return byte;}}
  if((read_byte[7] & 0x20) == 0x20){byte[i] = key_full_stop;i++;  if(i == 7){return byte;}}
  if((read_byte[7] & 0x10) == 0x10){byte[i] = key_slash;i++;  if(i == 7){return byte;}}
  if((read_byte[7] & 0x08) == 0x08){byte[0] |= R_Shi;}
  if((read_byte[7] & 0x04) == 0x04){byte[i] = key_quotation_mark;i++;  if(i == 7){return byte;}}
  if((read_byte[7] & 0x02) == 0x02){byte[i] = key_enter;i++;  if(i == 7){return byte;}}
  if((read_byte[7] & 0x01) == 0x01){byte[i] = key_delete;i++;  if(i == 7){return byte;}}
  }

  if(read_byte[8])
  {
  if((read_byte[8] & 0x10) == 0x10){byte[i] = key_app;i++;  if(i == 7){return byte;}}
  if((read_byte[8] & 0x08) == 0x08){byte[i] = key_left;i++;  if(i == 7){return byte;}}
  if((read_byte[8] & 0x04) == 0x04){byte[i] = key_down;i++;  if(i == 7){return byte;}}
  if((read_byte[8] & 0x02) == 0x02){byte[i] = key_up;i++;    if(i == 7){return byte;}}
  if((read_byte[8] & 0x01) == 0x01){byte[i] = key_right;i++;  if(i == 7){return byte;}}
  }

  for(i = 1; i < 9; i++)
  {
  if(byte[i] == R_FN | byte[i] == L_FN)
  {
    byte[i] = 0;
    fn = 1;
  }
  if(byte[i] == R_APP | byte[i] == L_APP)
  {
    byte[i] = 0;
    app = 1;
  }
  }

  if(byte[0] == R_Alt || byte[0] == L_Alt)
  {
  if(byte[1] == key_4 && byte[2] == 0x00 && byte[3] == 0x00 && byte[4] == 0x00 && byte[5] == 0x00 && byte[6] == 0x00)
    byte[1] = key_F4;
  
  //if(byte[1] == 0x00 && byte[2] == 0x00 && byte[3] == 0x00 && byte[4] == 0x00 && byte[5] == 0x00 && byte[6] == 0x00){
  //  byte[0] = 0x00;}//Èç¹ûÖ»ÓÐalt°´ÏÂ£¬Ôò²»¼ì²âalt¡£ÒòÎªalt»¹ÓÐ×ªÒë£ºcommand¡£
  }

  if(fn | app)
  {
  //fn and app process
  if(fn)
  {
    for(i = 1; i < 7; i++)
    {
    //ÕÒµ½µÚÒ»¸öÆäËû°´¼ü¼üÖµ£¬only response one fn function.·µ»Ø¸øÖ÷·¢ËÍ³ÌÐò?
    if(byte[i])
    {
      byte[1] = fn_fnc(byte[i]);
      byte[2] = byte[3] = byte[4] = byte[5] = byte[6] = 0;
      break;
    }
    }
  }

  if(app)
  {
    for(i = 1; i < 7; i++)
    {
    //ÕÒµ½µÚÒ»¸öÆäËû°´¼ü¼üÖµ£¬²»ÈÃÖ÷·¢ËÍ³ÌÐòÏìÓ¦£¬ÒòÎªÖ÷³ÌÐòÖ»ÄÜ·¢ËÍÒ»´Î°´¼ü£¬appÐèÒªÖ§³Ö¸´ÔÓ×Ö´®·¢ßT
    if(byte[i])
    {
      byte[1] = app_fnc(byte[i]);
      byte[2] = byte[3] = byte[4] = byte[5] = byte[6] = 0;
      break;
    }
    }
  }
  }
  return byte;
}
u8 if_key_same(u8* byte_old,u8*byte)
{
  u8 i=0, j=0;
  u8 downup=0, updown=0;
  
  if(byte[0] == byte_old[0])
  {
  for(i = 1; i < 7; i++)
  {
    for(j = 1; j < 7; j++)
    {
    if(byte[i] == byte_old[j])
    {
      downup++;
      break;
    }
    }
  }

  for(i = 1; i < 7; i++)
  {
    for(j = 1; j < 7; j++)
    {
    if(byte_old[i] == byte[j])
    {
      updown++;
      break;
    }
    }
  }

  if(downup == 6 && updown == 6)
    return 1;
  }

  return 0;
}

u8 send_key(void)
{
  static u8 byte_old[7] = {0,0,0,0,0,0,0};  //old values
  u8* byte;//read new key map
  u8 i = 0, j = 0, k = 0, less = 0, more = 0;
  u8 lesss = 0, moree = 0;
  byte = map_2_key(Read_KeyValue());//read new key map

  if(if_key_same(byte_old,byte))
  {
  return 0;
  }
  else
  {
  //if key changed
  // u3_printf("before_byte:%x %x %x %x %x %x %x \r\n",byte_old[0],byte_old[1],byte_old[2],byte_old[3],byte_old[4],byte_old[5],byte_old[6]);
  // u3_printf("byte:   %x %x %x %x %x %x %x \r\n",byte[0],byte[1],byte[2],byte[3],byte[4],byte[5],byte[6]);
  if(byte[1]>APP_START && byte[1]<APP_END)
  {
    switch(byte[1])
    {
    case APP_A:   ctrl_a(); return 0;
    case APP_Z:   ctrl_z(); return 0;
    case APP_X:   ctrl_x(); return 0;
    case APP_C:   ctrl_c(); return 0;
    case APP_V:   ctrl_v(); return 0;
    case APP_D:   app_page_down(); return 0;
    case APP_U:   app_page_up(); return 0;
    case APP_J:   app_down(); return 0;
    case APP_K:   app_up(); return 0;
    case APP_H:   app_left(); return 0;
    case APP_L:   app_right(); return 0;
    case APP_2:   print_pass(); return 0;
    case APP_1:   print_mail(); return 0;//qsy2011815@163.com
    
    case APP_S:   ctrl_s(); return 0;
    case APP_F:   ctrl_f(); return 0;
    case APP_T:   ctrl_h(); return 0;
    case APP_TAB:   alt_tab(); return 0;
    case APP_B:   ctrl_b(); return 0;
    

    case APP_8:   change_mode();  return 0;
    case APP_9:   clear(); return 0;
    case APP_0:  pair(); return 0;

    default: return 0;
    }
    
 
    
    
    
    
  }
    
  for(i = 1; i < 7; i++)
  {
    //find if less ,if less, set byte_old to 0x00
    less = 1;
    for(j = 1; j < 7; j++)
    {
    if(byte_old[i] == byte[j])
    {
      less = 0;
      break;
    }
    }
    if(less)
    {
    byte_old[i] = 0x00;
    lesss = 1;
    }
  }

  if(lesss == 1)
  {
    u3_printf("%x%x%x%x%x%x%x\r\n",byte_old[0],byte_old[1],byte_old[2],byte_old[3],byte_old[4],byte_old[5],byte_old[6]);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c", 0x0c,0x00,0xa1,0x01,byte_old[0],0x00, byte_old[1],byte_old[2],byte_old[3],byte_old[4],byte_old[5],byte_old[6]);
  }
  
  for(i = 1; i < 7; i++)
  {
    //find if more , set byte to before_byte 0x00 position
    more = 1;
    for(j = 1; j < 7; j++)
    {
    if(byte[i] == byte_old[j])
    {
      more = 0;
      break;
    }
    }

    if(more)
    {
    //byte[i] is the more key down
    moree = 1;
    for(k = 1; k < 7; k++)
    {
      if(byte_old[k] == 0x00)
      {
      byte_old[k] = byte[i];
      break;
      }
    }
    }
  }

  if(moree == 1)
  {
    u3_printf("%x%x%x%x%x%x%x \r\n",byte_old[0],byte_old[1],byte_old[2],byte_old[3],byte_old[4],byte_old[5],byte_old[6]);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c", 0x0c,0x00,0xa1,0x01,byte_old[0],0x00, byte_old[1],byte_old[2],byte_old[3],byte_old[4],byte_old[5],byte_old[6]);
  }
  
  if(moree == 0 && lesss ==0){//only ctl key changed
    byte_old[0] = byte[0];
    u3_printf("%x%x%x%x%x%x%x \r\n",byte_old[0],byte_old[1],byte_old[2],byte_old[3],byte_old[4],byte_old[5],byte_old[6]);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c",  0x0c,0x00,0xa1,0x01,byte_old[0],0x00, byte_old[1],byte_old[2],byte_old[3],byte_old[4],byte_old[5],byte_old[6]);
  }
  }
  return 0;
}

void change_mode(void)
{
  AT_1;
  delay_ms(300);
  AT_0;
  send_cmd("AT+EXIT", "OK", 200);
  delay_ms(300);
}

void clear(void)
{
  u8 text[64] = {0};
  AT_1;
  delay_ms(300);
  AT_0;
  send_cmd("AT+DMODE=1","OK",200);
  sprintf((char*)text, "AT+NAME=%s", BLT_NAME);
  send_cmd(text,"OK",200);
  send_cmd("AT+UNPLUG","OK",200);
  send_cmd("AT+EXIT","OK",200);
  delay_ms(300);
}

void pair(void)
{
  u8 times = 0;
  AT_1;delay_ms(300);AT_0;
  send_cmd("AT+PAIR","OK",200);
  while(!send_cmd("AT+STATE?","4",200))
  {
  delay_ms(1000);
  times++;
  if(times == 20)
  {
    send_cmd("AT+EXIT","OK",200);   
    break;
  }
  }
  send_cmd("AT+EXIT","OK",200);
  delay_ms(300);
}

void low_power(void){
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;

  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;
  GPIO_Init(GPIOA,&GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3;
  GPIO_Init(GPIOA,&GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5;
  GPIO_Init(GPIOA,&GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;
  GPIO_Init(GPIOA,&GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_7;
  GPIO_Init(GPIOA,&GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8;
  GPIO_Init(GPIOA,&GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_11;
  GPIO_Init(GPIOA,&GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_12;
  GPIO_Init(GPIOA,&GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13;
  GPIO_Init(GPIOA,&GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_14;
  GPIO_Init(GPIOA,&GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_15;
  GPIO_Init(GPIOA,&GPIO_InitStructure);
/////////////////////////////////////////////////

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;
  GPIO_Init(GPIOB,&GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1;
  GPIO_Init(GPIOB,&GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3;
  GPIO_Init(GPIOB,&GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4;
  GPIO_Init(GPIOB,&GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5;
  GPIO_Init(GPIOB,&GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;
  GPIO_Init(GPIOB,&GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_7;
  GPIO_Init(GPIOB,&GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8;
  GPIO_Init(GPIOB,&GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;
  GPIO_Init(GPIOB,&GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_12;
  GPIO_Init(GPIOB,&GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13;
  GPIO_Init(GPIOB,&GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_14;
  GPIO_Init(GPIOB,&GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_15;
  GPIO_Init(GPIOB,&GPIO_InitStructure);
//////////////////////////////////////////////

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);

  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_14;
  GPIO_Init(GPIOB,&GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_15;
  GPIO_Init(GPIOB,&GPIO_InitStructure);
}

void a(void)
{
  printf("%c%c%c%c%c%c%c%c%c%c%c%c",  0x0c,0x00,0xa1,0x01,0x00,0x00,key_a,0x00,0x00,0x00,0x00,0x00);
  printf("%c%c%c%c%c%c%c%c%c%c%c%c",  0x0c,0x00,0xa1,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
}

u8 fn_fnc(u8 key)
{
  switch(key)
  {
  case key_1: return key_F1;
  case key_2: return key_F2;
  case key_3: return key_F3;
  case key_4: return key_F4;
  case key_5: return key_F5;
  case key_6: return key_F6;
  case key_7: return key_F7;
  case key_8: return key_F8;
  case key_9: return key_F9;
  case key_0: return key_F10;
  case key_minus: return key_F11;
  case key_equal: return key_F12;
  case key_w: return vol_up;
  case key_s: return mute;
  case key_x: return vol_down;
  case key_home: return key_page_up;
  case key_end: return key_page_down;
  case key_d:   return key_page_down;
  case key_u:   return key_page_up;
  case key_j:   return key_down;
  case key_k:   return key_up;
  case key_h:   return key_left;
  case key_l:   return key_right;
  default: return 0;
  }
}



u8 app_fnc(u8 key)
{
  switch(key)
  {
  case key_q: return APP_1; //mail()
  case key_2: return APP_2;
  // case key_3: return APP_3;
  // case key_4: return APP_4;
  // case key_5: return APP_5;
  // case key_6: return APP_6;
  // case key_7: return APP_7;
  case key_backspace: return APP_8;  // change mode
  case key_backslash: return APP_9;  // clear
  case key_grave: return APP_0;  // pair
  
  case key_a: return APP_A;  // ctrl + a
  case key_z: return APP_Z;  // 
  case key_x: return APP_X;  // 
  case key_c: return APP_C;  // 
  case key_v: return APP_V;  // 
  
  case key_d: return APP_D;  // 
  case key_u: return APP_U;  // 
  case key_j: return APP_J;  // 
  case key_k: return APP_K;  // 
  case key_l: return APP_L;  // 
  case key_h: return APP_H;  // 
  
  case key_s: return APP_S;  // 
  case key_f: return APP_F;  // 
  case key_T: return APP_T;  // 
  case key_tab: return APP_TAB;  // 
  case key_b: return APP_B;  // 
  
  
  
  // case key_n: return APP_N;  // 
  default: return 0;
  }
}
