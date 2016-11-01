#define WetPoint A1
#define jidianqi 13


//设置每个数码管的灯管控制
int a = 1;
int b = 2;
int c = 3;
int d = 4;
int e = 5;
int f = 6;
int g = 7;
//设置数码管控制接口
int d4 = 9;
int d3 = 10;
int d2 = 11;
int d1 = 12;

 int qian,bai,shi,ge;

//定义湿度上限
int wetTopLevel=850;  //湿度的上限，这里的上限代表很干燥
int wetLowLevel=650; //湿度的下限
int waterTime=5000; //浇水的时间
int num;  //实时湿度读取变量

//设计loopCheckTime everyLoopTime commonDelay 和 showNumTime是考虑避免多线程 ，原理是 总的监测时间除以一次LOOP的大概总时间 赋值为count_top ，每次LOOP循环则把currentCount+1，如果currentCount大于count_top 则执行湿度监测
unsigned long loopCheckTime=1*60;//这是1分钟的监测时间  
long everyLoopTime=0; //Loop函数的大概总时间 在loop函数开始出初始计算获得
int commonDelay=0; //公共等待时间 用于调整总等待毫秒数
int showNumTime=5;
long count_top=0;
long currentCount=0;
void setup() {
  //初始化引脚
  //Serial.begin(9600);
  pinMode(d1, OUTPUT);
  pinMode(d2, OUTPUT);
  pinMode(d3, OUTPUT);
  pinMode(d4, OUTPUT);
  pinMode(a, OUTPUT);
  pinMode(b, OUTPUT);
  pinMode(c, OUTPUT);
  pinMode(d, OUTPUT);
  pinMode(e, OUTPUT);
  pinMode(f, OUTPUT);
  pinMode(g, OUTPUT);
  pinMode(jidianqi, OUTPUT);
}


int firstFlag=1; //设置firstFlag 变量 是为了标志系统的的初始化 
void loop() {
  if(firstFlag==1)  //如果当前是arduino首次启动的状态 
  {
    if(num<=wetLowLevel)  //如果湿度已经低于湿度下限 就立即停止抽水
    {
        //停止抽水
        digitalWrite(jidianqi,HIGH);  
        //响蜂鸣器
    }
   loopCheckTime=loopCheckTime*1000; //把检测等待时间转换为毫秒单位 只需要一次计算即可 所以放在此处
   firstFlag++; 
  }
   everyLoopTime=commonDelay+showNumTime*4; //算出每次系统循环所消耗的总时间 单位为毫秒
   count_top=loopCheckTime/everyLoopTime; //算出循环计数的上限值
   num=analogRead(WetPoint); //读取湿度的数值
   divNumAndLight(num,showNumTime); //拆分数字的每一位，并显示湿度数值到四位数码管
 
   currentCount=currentCount+1;
    if(num>=wetTopLevel && currentCount==1) //如果湿度数值高于湿度上限 而且是第一次 就立即抽5秒水
    {
        digitalWrite(jidianqi,LOW); //抽水
        delay(waterTime);
        digitalWrite(jidianqi,HIGH);//停止抽水
    }
    if(currentCount>=count_top) //现在系统已经循环超过了浇水监测时间【默认是30分钟】
    {
      if(num<=wetLowLevel)  //如果湿度已经低于湿度下限 就立即停止抽水
      {
          //停止抽水
          digitalWrite(jidianqi,HIGH);  
          //响蜂鸣器
          currentCount=count_top+10;//此处的计算是为了避免【高速的loop循环导致无休止的递加从而可能引发的溢出异常】
      }else if(num>=wetTopLevel) //如果湿度已经超过湿度上限
       {
          digitalWrite(jidianqi,LOW); //抽水
          delay(waterTime);
          digitalWrite(jidianqi,HIGH);//停止抽水
          currentCount=2; //赋值为1就避免了第二个if 避免两次浇水
        }   
    }
  //delay(commonDelay);  //公共等待时间，目的是用于凑整调优系统运行时间，但是如果开启此处使用会导致数码管显示的视觉异常【数码管会不停闪烁】
}

void pickDigit(int x)  //定义pickDigit(x),其作用是指定使用哪个数码管
{
  digitalWrite(d1, HIGH);
  digitalWrite(d2, HIGH);
  digitalWrite(d3, HIGH);
  digitalWrite(d4, HIGH);
  switch(x)
  {
  case 1: 
    digitalWrite(d1, LOW); 
    break;
  case 2: 
    digitalWrite(d2, LOW); 
    break;
  case 3: 
    digitalWrite(d3, LOW); 
    break;
  default: 
    digitalWrite(d4, LOW); 
    break;
  }
}

void divNumAndLight(int num,int delaytime){

    qian=num/1000;
    bai=(num/100)%10;
    shi=(num%100)/10;
    ge=num%10;
    ge=0;
      clearLEDs();
      pickDigit(1);
      showNum(qian);
      delayMicroseconds(delaytime*1000); //此处的单位是微秒 时间很短产生视觉停留

      clearLEDs();
      pickDigit(2);
      showNum(bai);
      delayMicroseconds(delaytime*1000);

      clearLEDs();
      pickDigit(3);
      showNum(shi);
      delayMicroseconds(delaytime*1000);

      clearLEDs();
      pickDigit(4);
      showNum(ge);
      delayMicroseconds(delaytime*1000);
  }


  void showNum(int num)
  {
    switch(num)
    {
      case 0:
          digitalWrite(a,HIGH);  
          digitalWrite(b,HIGH);  
          digitalWrite(c,HIGH);  
          digitalWrite(d,HIGH);  
          digitalWrite(e,HIGH);  
          digitalWrite(f,HIGH);  
          digitalWrite(g,LOW);
          break;
      case 1:
          digitalWrite(a,LOW);  
          digitalWrite(b,HIGH);  
          digitalWrite(c,HIGH);  
          digitalWrite(d,LOW);  
          digitalWrite(e,LOW);  
          digitalWrite(f,LOW);  
          digitalWrite(g,LOW);  
          break;  
      case 2:
          digitalWrite(a,HIGH);  
          digitalWrite(b,HIGH);  
          digitalWrite(c,LOW);  
          digitalWrite(d,HIGH);  
          digitalWrite(e,HIGH);  
          digitalWrite(f,LOW);  
          digitalWrite(g,HIGH);
          break;
      case 3:
          digitalWrite(a,HIGH);  
          digitalWrite(b,HIGH);  
          digitalWrite(c,HIGH);  
          digitalWrite(d,HIGH);  
          digitalWrite(e,LOW);  
          digitalWrite(f,LOW);  
          digitalWrite(g,HIGH);
          break;
      case 4:
          digitalWrite(a,LOW);  
          digitalWrite(b,HIGH);  
          digitalWrite(c,HIGH);  
          digitalWrite(d,LOW);  
          digitalWrite(e,LOW);  
          digitalWrite(f,HIGH);  
          digitalWrite(g,HIGH);
          break;
      case 5:
          digitalWrite(a,HIGH);  
          digitalWrite(b,LOW);  
          digitalWrite(c,HIGH);  
          digitalWrite(d,HIGH);  
          digitalWrite(e,LOW);  
          digitalWrite(f,HIGH);  
          digitalWrite(g,HIGH);
          break;
      case 6:
          digitalWrite(a,HIGH);  
          digitalWrite(b,LOW);  
          digitalWrite(c,HIGH);  
          digitalWrite(d,HIGH);  
          digitalWrite(e,HIGH);  
          digitalWrite(f,HIGH);  
          digitalWrite(g,HIGH);
          break;
      case 7:
          digitalWrite(a,HIGH);  
          digitalWrite(b,HIGH);  
          digitalWrite(c,HIGH);  
          digitalWrite(d,LOW);  
          digitalWrite(e,LOW);  
          digitalWrite(f,LOW);  
          digitalWrite(g,LOW);
          break;
      case 8:
          digitalWrite(a,HIGH);  
          digitalWrite(b,HIGH);  
          digitalWrite(c,HIGH);  
          digitalWrite(d,HIGH);  
          digitalWrite(e,HIGH);  
          digitalWrite(f,HIGH);  
          digitalWrite(g,HIGH);
          break;
      case 9:
          digitalWrite(a,HIGH);  
          digitalWrite(b,HIGH);  
          digitalWrite(c,HIGH);  
          digitalWrite(d,HIGH);  
          digitalWrite(e,LOW);  
          digitalWrite(f,HIGH);  
          digitalWrite(g,HIGH);
          break;
    }   
  }


  void clearLEDs()  //清屏
{
  digitalWrite(a, LOW);
  digitalWrite(b, HIGH);
  digitalWrite(c, HIGH);
  digitalWrite(d, HIGH);
  digitalWrite(e, HIGH);
  digitalWrite(f, HIGH);
  digitalWrite(g, HIGH);
}
