#include <DHT22.h>
#define DHT22_PIN 7//7号脚不接任何电阻
DHT22 myDHT22(DHT22_PIN);
float h,t;
void setup(void)
{
  h=0,t=0;
  Serial.begin(9600);
}

void loop(void)
{ 
DHT22_ERROR_t errorCode;if( myDHT22.readData()== DHT_ERROR_NONE){t=myDHT22.getTemperatureC(); h=myDHT22.getHumidity();}  //HZY温馨提示：读取DHT22的数据判断并赋予h,t.
if(h>0){
  Serial.print(t);
  Serial.println("C ");
  Serial.print(h);
  Serial.println("%"); 
  delay(1000);
}
}
