// 增加按钮口
const int UP_BUTTON_PIN = 12;
// 减小按钮口
const int DOWN_BUTTON_PIN = 11;
// 马达连接到数字输出口
const int MOTOR_PIN = 10;
// 周期时间，单位为微秒
const int CYCLE_TIME = 20000;
// PWM输出的时间，单位为微秒
const int CYCLE_PWM_OUTPUT_TIME = 3000;
// 应用逻辑执行时间，单位为微秒
const int CYCLE_APPLICATION_TIME = (CYCLE_TIME - CYCLE_PWM_OUTPUT_TIME);
// 最小有效PWM宽度，单位为微秒
const int MIN_PWM_WIDTH = 900;
// 最大有效PWM宽度，单位为微秒
const int MAX_PWM_WIDTH = 2100;
// 从0开始的最大控制力
const int MAX_CONTROL_POWER = 200;

void setup() 
{
  Serial.begin( 9600 );
  pinMode( MOTOR_PIN, OUTPUT ); 
}

void application(int& ctrlPower)
{
  static int s_ctrlPower = 0;
  
  /*
  while( Serial.available() )
  {
    // 这里是获取控制指令
    s_ctrlPower = Serial.parseInt();
    //Serial.print( "received number is: " );
    //Serial.println( ctrlPower, DEC );
  }
  */

  int up = digitalRead( UP_BUTTON_PIN );
  int down = digitalRead( DOWN_BUTTON_PIN );
  //Serial.print( "up = " );
  //Serial.print( up, DEC );
  //Serial.print( ", down = " );
  //Serial.println( down, DEC );
  static int s_lastUp = 0;
  static int s_lastDown = 0;
  if( !s_lastUp && up )
  {
      s_ctrlPower ++;
  }  
  if( !s_lastDown && down )
  {
    s_ctrlPower --;
  }
  s_lastUp = up;
  s_lastDown = down;
  
  ctrlPower = s_ctrlPower;
  Serial.println( ctrlPower, DEC );
}

void loop()
{
  /*
   * 每周期20ms，其中用于应用逻辑计算的时间为17ms，其他3ms用于输出PWM高电平。
   * 即：PWM高电平从第17ms开始输出
   */

  long t0 = micros(); //记录进入loop的时间

  // 应用逻辑
  int ctrlPower = 0;
  application( ctrlPower );
  if( ctrlPower <= 0 )
  {
    // TODO: 异常处理
    ctrlPower = 0;
  }
  if( ctrlPower >= MAX_CONTROL_POWER )
  {
    // TODO: 异常处理
    ctrlPower = MAX_CONTROL_POWER;
  }

  // 0～200分别对应高电平脉宽0.9ms～2.1ms
  int len = (MIN_PWM_WIDTH + ctrlPower * (MAX_PWM_WIDTH - MIN_PWM_WIDTH) / MAX_CONTROL_POWER);
  if( len > MAX_PWM_WIDTH )
  {
    // TODO: 处理异常
    len = MAX_PWM_WIDTH;
  }

  //Serial.println( ctrlPower, DEC );
  Serial.println( len, DEC );

  long t1 = micros();
  int wait_time = 0;
  if( (t1 - t0) > CYCLE_APPLICATION_TIME )
  {
    // 应用逻辑计算超时
    // TODO: 处理异常
    Serial.println( "too long" );

    wait_time = 0;
  }
  else
  {
    wait_time = CYCLE_APPLICATION_TIME - (t1-t0);
  }  

  // 等待直到周期的第17ms
  delayMicroseconds( wait_time );
  
  // 输出高电平
  digitalWrite( MOTOR_PIN, HIGH );
  // 延时len微秒
  delayMicroseconds( len );
  // 输出低电平
  digitalWrite( MOTOR_PIN, LOW );
}

