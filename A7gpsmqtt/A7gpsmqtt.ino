#include <SoftwareSerial.h>
#include <stdlib.h>
#include <string.h>
#include <DHT22.h>
#define DHT22_PIN 7//7号脚不接任何电阻
DHT22 myDHT22(DHT22_PIN);
char atCommand[50];
float hum,tum;
unsigned char mqttMessage[127];
int mqttMessageLength = 0;
int baseIndex;
int p=0;
SoftwareSerial gprs(2, 3);//用于发送AT指令
//SoftwareSerial gps(4, 5);//实际只用4号引脚接收GPS信号
String lng;//经度
String lat;//纬度
//int count=0;


void setup() {
  gprs.begin(115200);
  Serial.begin(9600);
  //gps.begin(9600);  
  s();
 // delay(5000);
}

void loop() {
//  Serial.println(count); 
  //count++;
     delay(1000);
       DHT22_ERROR_t errorCode;
  if( myDHT22.readData()== DHT_ERROR_NONE){
    tum=myDHT22.getTemperatureC(); 
    hum=myDHT22.getHumidity();
    }  //读取DHT22的数据判断并赋予h,t.
   String c;//存放接收到的所有GPS包
   String aftersplit;//存放符合要求的GPS包
   c=command();
    Serial.println(c);
     Serial.println(c.length());
     aftersplit=split(c);//筛选符合要求的GPS包
       //Serial.println(aftersplit);  
       Serial.println(lng);//经度
        Serial.println(lat);//纬度
String json= buildJson();
  char jsonStr[300];
  json.toCharArray(jsonStr,300);
  sendMQTTMessage("chenyu", "124.160.104.198", "1883", "zuccmqtt",jsonStr,"chenyu","KDANAA921");

}

void sendMQTTMessage(char* clientId, char* brokerUrl, char* brokerPort, char* topic, char* message ,char* username, char* password){
 delay(1000); // Wait a second
 String c;
 strcpy(atCommand, "AT+CIPSTART=\"TCP\",\"");
 strcat(atCommand, brokerUrl);
 strcat(atCommand, "\",");
 strcat(atCommand, brokerPort);
 gprs.println(atCommand);
 mqttMessageLength= mqtt_connect_message(mqttMessage, clientId,username,password);
 delay(7000);
  gprs.println("AT+CIPSEND="+String(mqttMessageLength));
  c=command2();
   Serial.println(c);  
 for (int j = 0; j < mqttMessageLength; j++) {
  gprs.write(mqttMessage[j]); // Message contents
 }
 mqttMessageLength = mqtt_publish_message(mqttMessage, topic, message);
 delay(2000);
  gprs.println("AT+CIPSEND="+String(mqttMessageLength));
  c=command2();
   Serial.println(c);  
 for (int k = 0; k < mqttMessageLength; k++) {
  gprs.write(mqttMessage[k]);
 }
  c=command2();
   Serial.println(c);  
  gprs.println("AT+CIPCLOSE");
  c=command2();
   Serial.println(c);  
}
int mqtt_connect_message(unsigned char *mqttMessage,char* client_id,char* username,char* password)  
{   
    char i = 0;    
    char client_id_length = strlen(client_id);    
    int username_length = strlen(username);    
    int password_length = strlen(password);    
    int packetLen = 12 + 2 + client_id_length + 2 + username_length + 2 + password_length;    
        
    mqttMessage[0] = 16;                      // MQTT Message Type CONNECT    
    mqttMessage[1] = packetLen%256;    
    baseIndex = 2;    
    if( packetLen >127 ){//    
        mqttMessage[2] = 1;//packetLen/127;    
        baseIndex = 3;    
    }    
        
        
    mqttMessage[baseIndex] = 0;                       // Protocol Name Length MSB    
    mqttMessage[baseIndex+1] = 6;                       // Protocol Name Length LSB    
    mqttMessage[baseIndex+2] = 77;                      // ASCII Code for M    
    mqttMessage[baseIndex+3] = 81;                      // ASCII Code for Q    
    mqttMessage[baseIndex+4] = 73;                      // ASCII Code for I    
    mqttMessage[baseIndex+5] = 115;                     // ASCII Code for s    
    mqttMessage[baseIndex+6] = 100;                     // ASCII Code for d    
    mqttMessage[baseIndex+7] = 112;                     // ASCII Code for p    
    mqttMessage[baseIndex+8] = 3;                      // MQTT Protocol version = 3    
    mqttMessage[baseIndex+9] = 194;                   // conn flags    
    mqttMessage[baseIndex+10] = 0;                      // Keep-alive Time Length MSB    
    mqttMessage[baseIndex+11] = 60;                     // Keep-alive Time Length LSB    
        
    mqttMessage[baseIndex+12] = 0;                      // Client ID length MSB    
    mqttMessage[baseIndex+13] = client_id_length;       // Client ID length LSB    
        
    baseIndex += 14;    
    // Client ID    
    for(i = 0; i < client_id_length; i++){    
        mqttMessage[baseIndex + i] = client_id[i];    
    }    
        
    baseIndex = baseIndex+client_id_length;    
        
    //username    
    mqttMessage[baseIndex] = 0;                      //username length MSB    
    mqttMessage[baseIndex+1] = username_length;       //username length LSB    
    baseIndex = baseIndex+2;    
    for(i = 0; i < username_length ; i++){    
        mqttMessage[baseIndex + i] = username[i];    
    }    
        
    baseIndex = baseIndex + username_length;    
        
    //password    
    mqttMessage[baseIndex] = 0;                      //password length MSB    
    mqttMessage[baseIndex+1] = password_length;       //password length LSB    
    baseIndex = baseIndex + 2;    
    for(i = 0; i < password_length ; i++){    
        mqttMessage[baseIndex + i] = password[i];    
    }    
        
    baseIndex += password_length;    
    
    return baseIndex;    
}  
  
  
int mqtt_publish_message(unsigned char *mqttMessage, char  *topic, char  *message) {    
        
    //mqttMessage = 0;    
        
    unsigned char i = 0;    
    unsigned char topic_length = strlen(topic);    
    unsigned char message_length = strlen(message);    
        
    mqttMessage[0] = 48;                                  // MQTT Message Type CONNECT    
    mqttMessage[1] = 2 + topic_length + message_length;   // Remaining length    
    mqttMessage[2] = 0;                                   // MQTT Message Type CONNECT    
    mqttMessage[3] = topic_length;                        // MQTT Message Type CONNECT    
        
    // Topic    
    for(i = 0; i < topic_length; i++){    
        mqttMessage[4 + i] = topic[i];    
    }    
        
    // Message    
    for(i = 0; i < message_length; i++){    
        mqttMessage[4 + topic_length + i] = message[i];    
    }    
        
    return 4 + topic_length + message_length;    
}
int strlen(char *str)  
{  
    int len = 0;  
    while (*str != '\0') {  
        len++;  
        str++;  
    }  
    return len;  
}

String buildJson() {
  String data = "{";
  data+="\"HTU_T\":\"";
  data+=String(tum);
  data+= "\",";
  
  data+="\"HTU_H\":\"";
  data+=String(hum);
  data+= "\",";
 
  data+="\"firstspeed\":\"";
  data+=String(p);
  data+= "\",";

  data+="\"lastspeed\":\"";
  data+=String(p);
  data+= "\",";

  data+="\"gps\":\"";
  data+=String(lat);
  data+= ",";
  data+=String(lng);
  data+="\"";
  data+="}";
  
  return data;
}

String  split (String c){//处理所有GPS包，解析出经纬度。
  int q1=0;
  int q2=0;
  String ans="";
  int i,j=0;
  int op[100];
  if(c.length()>62&&c.length()<130&&c.startsWith("$GPGGA,")){
    for(i=0;i<=c.length();i++){
     if( c[i]==','){ op[j]=i;j++;}
    }
       if(j!=12)return ans;
       lng="";
       lat="";
for(int k=op[1]+1;k<op[2]&&q1<50;k++){
  lat+=char(c[k]);
  q1++;
  }
 for(int k=op[3]+1;k<op[4]&&q2<50;k++){
  lng+=char(c[k]);
  q2++;
  }
    ans=c;
    return ans;
    }
    else return ans;
  }

void s(){//AT指令
   String c;
   gprs.println("AT");
   c=command2();
   Serial.println(c); 
   gprs.println("ATE1");
    c=command2();
   Serial.println(c); 
   gprs.println("AT+CGATT=1");
    c=command2();
   Serial.println(c); 
   gprs.println("AT+CGACT=1,1");
   c=command2();
   Serial.println(c); 
   gprs.println("AT+GPS=1");
   c=command2();
   Serial.println(c);  
    gprs.println("AT+CGATT=1");
   c=command2();
   Serial.println(c);
   gprs.println("AT+CGACT=1,1");
   c=command2();
   Serial.println(c);
   gprs.println("AT+AGPS=0");
   c=command2();
   Serial.println(c); 
}

String command(){//接收GPS字符包
  int xiabiao;
  String hu;
while(Serial.available()) {
  hu+=char(Serial.read()); 
  }
  return hu;
}

String command2(){//接收AT指令反馈
  int xiabiao;
  String hu;
long last2 = millis();
  long now2 = millis();//记录当前时间
   while (last2-now2< 500) {//运行0.5秒
    
         while(Serial.available()) {
              hu+=char(Serial.read()); 
       }
         last2 = millis(); 
   }
  return hu;
}
