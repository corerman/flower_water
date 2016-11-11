/*    花卉自动浇水系统
 * 1.基础的土壤湿度检测与浇水
 * 2.根据土壤湿度进行分级分量浇水
 * 3.根据土壤渗透速度智能等待时间
 */
#define WetPoint A1   //湿度传感器连接口
#define jidianqi 13  //继电器连接口


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

 int qian,bai,shi,ge;  //定义湿度的四位数字
 int num;  //存储湿度变量
 
//定义湿度范围界限值
int wetTopLevel=850;  //湿度的上限，这里的上限代表很干燥【不符合生存条件】
int wetLowLevel=650; //湿度的下限，这里的下限代表很潮湿【不符合生存条件】
int waterTime=5000; //浇水的时间的最大值
int wetTimeLevel1,wetTimeLevel2,wetTimeLevel3,wetTimeLevel4,wetTimeLevel5;

/* 设计loopCheckTime everyLoopTime commonDelay 和 showNumTime是考虑避免使用多线程，但是构建多任务处理环境
*  原理是 总的检测时间除以一次LOOP的大概总时间 赋值为count_top 
*  每次LOOP循环则把currentCount+1，如果currentCount大于count_top 则执行相关活动
*/
unsigned long loopCheckTime=1*60;//这是1分钟的土壤湿度检测时间  【初始化渗透等待时间 】
long everyLoopTime=0; //Loop函数的大概总时间 在loop函数开始出初始计算获得
int commonDelay=0; //公共等待时间 用于调整总等待毫秒数
int showNumTime=5; //四位数码管显示时间【人类视觉停留】
long count_top=0; //一个检测时间所计算出的计数量
long countTop_bak=0; //count_top的备份变量，用来还原检测时间长度
long currentCount=0; //定义当前LOOP所产生的计数量

int firstFlag=1; //设置firstFlag 变量 是为了标志Arduino的第一次LOOP

long stepOfWaterDelay=1; //渗水智能等待递加时间，每次递加为1分钟【单位分钟】
long stepAddWaterDelayCount=0; //渗水等待递加次数统计，最大为10次，十次后归为

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




void loop() {
    num=analogRead(WetPoint); //读取湿度的数值
    divNumAndLight(num,showNumTime); //拆分数字的每一位，并显示湿度数值到四位数码管
    
    if(firstFlag==1)  //如果当前是arduino首次启动的状态 
    {
        firstFlag=2; 
        loopCheckTime=loopCheckTime*1000; //把检测等待时间转换为毫秒单位 只需要一次计算即可 所以放在此处
        digitalWrite(jidianqi,HIGH);//停止抽水
        
        if(num<=wetLowLevel)  //如果湿度已经低于湿度下限 就立即停止抽水
        {
            //停止抽水
            digitalWrite(jidianqi,HIGH);  
        }
        if(num>=wetTopLevel) //如果湿度数值高于湿度上限 而且是第一次 就立即抽5秒水
        {
            digitalWrite(jidianqi,LOW); //抽水
            giveWaterDelay(num,wetTopLevel,wetLowLevel);
            digitalWrite(jidianqi,HIGH);//停止抽水
        }       
         
       everyLoopTime=commonDelay+showNumTime*4; //算出每次系统循环所消耗的总时间 单位为毫秒
       count_top=loopCheckTime/everyLoopTime; //算出循环计数的上限值   
       countTop_bak=count_top;  //count_top初始准确值备份
       
        wetTimeLevel1=waterTime/5;   //第一等级浇水时间   最短浇水时间
        wetTimeLevel2=waterTime*2/5; //第二等级浇水时间
        wetTimeLevel3=waterTime*3/5; //第三等级浇水时间
        wetTimeLevel4=waterTime*4/5; //第四等级浇水时间
        wetTimeLevel5=waterTime; //第三等级浇水时间
    }
    
   currentCount++; //每次LOOP 当前计数器自加1
   if(currentCount>=count_top) //现在系统已经循环超过了浇水监测时间【默认是1分钟】
   {
      if(num<=wetLowLevel)  //如果湿度已经低于湿度下限 就立即停止抽水
      {
          //停止抽水
          digitalWrite(jidianqi,HIGH);  
          //重置操作
          currentCount=2;//重置当前计数器
          stepAddWaterDelayCount=0; //重置每次渗透延迟时间
          count_top=countTop_bak; //重置计数器上限值
      }else if(num>=wetTopLevel) //如果湿度已经超过湿度上限
       {
          digitalWrite(jidianqi,LOW); //抽水
          giveWaterDelay(num,wetTopLevel,wetLowLevel); //智能分级浇水
          digitalWrite(jidianqi,HIGH);//停止抽水
          //重置操作
          currentCount=2; //为了避免BUG 设置为2
          stepAddWaterDelayCount=0;
          count_top=countTop_bak;
        }
        else //湿度在合适的区间(wetLowLevel,wetTopLevel),延长土壤渗水等待时间等待时间
        {
            //停止抽水
            stepAddWaterDelayCount++;  //渗透时间延长等待次数递加
            if(stepAddWaterDelayCount<=10)
            {
               digitalWrite(jidianqi,HIGH);  //禁止浇水      
               currentCount=2;  //重置当前计数器
               count_top=count_top+((stepOfWaterDelay*60*1000)/everyLoopTime);   //修改count_top值，每次递加一分钟   修复BUG       
            }
            else
            {
              //重置操作
                stepAddWaterDelayCount=0; 
                count_top=countTop_bak; //重置count_top上限值
                currentCount=2;
             }
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

void giveWaterDelay(int wetNum,int wetTopLevel,int wetLowLevel){
  //浇水延时函数 此函数默认代表要执行浇水 不同等级的湿度进行不同时间的浇水
  if(wetNum>wetTopLevel)
  {
    int wetSub= wetNum-wetTopLevel;
    if(wetSub<=50)
    {
     // Serial.println("water 1");
      delay(wetTimeLevel1); 
     }else if(wetSub<=100)
     {
      //Serial.println("water 2");
      delay(wetTimeLevel2);
      }else if(wetSub<=150)
      {
      //  Serial.println("water 3");
        delay(wetTimeLevel3);
       }
        else{
         // Serial.println("water 4");
          delay(wetTimeLevel5);
        }
   }
}
