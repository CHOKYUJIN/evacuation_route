// WIFI
#include <ESP8266WiFi.h>

// db control
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>

// ds18b20
#include <OneWire.h>  
#include <DallasTemperature.h>  

// dot matrix
#include "LedControl.h"

// WIFI information
const char* ssid = "mipil";
const char* password = "qwer1234";

// DB connection information
IPAddress server_addr(13,125,216,58);   // IP of the MySQL server
const char* hostGet = "ec2-13-125-216-58.ap-northeast-2.compute.amazonaws.com"; 
char mysqluser[] = "root";         // MySQL user login username
char mysqlpassword[] = "capstone!";           // MySQL user login password
WiFiClient SQLclient;
MySQL_Connection conn((Client *)&SQLclient);

// IFTTT information
#define HOSTIFTTT "maker.ifttt.com"
#define EVENT "IOT_fire_alarm"
#define IFTTTKEY "bu_WF_erv9J8YUFbCPS2et"
WiFiClientSecure client;

// ds18b20 global variable   
#define ONE_WIRE_BUS D3   
OneWire oneWire(ONE_WIRE_BUS);  //1-wire 디바이스와 통신하기 위한 준비  
DallasTemperature sensors(&oneWire);  // oneWire선언한 것을 sensors 선언시 참조함.    

// dot matrix global variable
LedControl lc = LedControl(D12,D11,D10,4);
int LedSet = 0;
int direction = -1;  

// id of this alarm
String id = "6";

int WiFiCon() {
    // Check if we have a WiFi connection, if we don't, connect.
  int xCnt = 0;

  if (WiFi.status() != WL_CONNECTED){

        Serial.println();
        Serial.println();
        Serial.print("Connecting to ");
        Serial.println(ssid);

        WiFi.mode(WIFI_STA);
        
        WiFi.begin(ssid, password);
        
        while (WiFi.status() != WL_CONNECTED  && xCnt < 50) {
          delay(500);
          Serial.print(".");
          xCnt ++;
        }

        if (WiFi.status() != WL_CONNECTED){
          Serial.println("WiFiCon : 0 (never connected)");
          return 0; //never connected
        } 
        else {
          Serial.println("WiFiCon : 1 (initial connection)");
          Serial.println("");
          Serial.println("WiFi connected");  
          Serial.println("IP address: ");
          Serial.println(WiFi.localIP());
          return 1; //1 is initial connection
        }

  } else {
    Serial.println("WiFiCon : 2 (already connected)");
    return 2; //2 is already connected
  
  }
}

void DBcon(){
  if (conn.connect(server_addr, 3306, mysqluser, mysqlpassword)) {
     delay(1000);
     Serial.println("DB-Connection Success.");
  }
  else
    Serial.println("DB-Connection failed.");
}

int mysqlSelect(){
  row_values *row = NULL;
  long head_count = 0;
  char query[] = "SELECT dir FROM fire_alarm.direction1"; // 불 붙은 보드 번호 가져오는 거로 수정
  bool ret_cnt;
  MySQL_Cursor *cur_mem;

  Serial.println(" Selecting with a cursor dynamically allocated.");
  // Initiate the query class instance
  cur_mem = new MySQL_Cursor(&conn);
  // Execute the query
  ret_cnt = cur_mem->execute(query);
  Serial.println(ret_cnt);
  // Fetch the columns (required) but we don't use them.
  if(ret_cnt){
    column_names *columns = cur_mem->get_columns();
    delay(100);
    // Read the row (we are only expecting the one)
    direction = -1;
    do {
      row = cur_mem->get_next_row();
      if (row != NULL) {
        head_count = atol(row->values[0]);
        direction = head_count;
        Serial.print("direction : ");
        Serial.println(head_count);
        break;
      }
    } while (row != NULL);
    columns = NULL;
  }
  // Deleting the cursor also frees up memory used
  cur_mem->close();
  delete cur_mem;

  // Show the result
  Serial.println("Selection Complete");
  delay(1000);
}

// Insert data on mysql db : 현재 id가 primary key라서 중복해서 안들어감
void postData(int temp, int smoke) {
  
   WiFiClient clientGet;
   const int httpGetPort = 80;

   //the path and file to send the data to:
   String urlGet = "/write.php";
   
      urlGet += "?temp=" + String(temp) + "&smoke=" + String(smoke);
      Serial.print(">>> Connecting to host: ");
      Serial.println(hostGet);
      Serial.println(urlGet);
       
       if (!clientGet.connect(hostGet, httpGetPort)) {
        Serial.print("Connection failed: ");
        Serial.print(hostGet);
      } else {
          Serial.print("success connection");

          clientGet.println("GET " + urlGet + " HTTP/1.1");
          clientGet.print("Host: ");
          clientGet.println(hostGet);
          clientGet.println("User-Agent: ESP8266/1.0");
          clientGet.println("Connection: close\r\n\r\n");
     
          unsigned long timeoutP = millis();
          while (clientGet.available() == 0) {
            
            if (millis() - timeoutP > 10000) {
              Serial.print(">>> Client Timeout: ");
              Serial.println(hostGet);
              clientGet.stop();
              return;
            }
          }

          //just checks the 1st line of the server response. Could be expanded if needed.
          while(clientGet.available()){ 
            String retLine = clientGet.readStringUntil('\r');
            Serial.println(retLine);
            break; 
          }

      } //end client connection if else
            
      Serial.print(">>> Closing host: ");
      Serial.println(hostGet);
          
      clientGet.stop();
      delay(3000);
}

void IFTTT(String id){
  if (client.connected())
  {
    client.stop();
  }
  client.flush();
  
  if (client.connect(HOSTIFTTT,443)) {
    Serial.println("Connected");
    
    String toSend = "GET /trigger/";
    toSend += EVENT;
    toSend += "/with/key/";
    toSend += IFTTTKEY;
    toSend += "?value1=";
    toSend += id;
    toSend += " HTTP/1.1\r\n";
    toSend += "Host: ";
    toSend += HOSTIFTTT;
    toSend += "\r\n";
    toSend += "Connection: close\r\n\r\n";

    client.print(toSend);
  } 
  client.flush();
  client.stop();
  delay(1000);
}

float ds18b20(){
  float temp;
  sensors.begin();
  Serial.print("Requesting temperatures..."); 
  sensors.requestTemperatures(); // Send the command to get temperature readings 
  Serial.println("DONE"); 
  Serial.print("Temperature : "); 
  temp = sensors.getTempCByIndex(0);
  Serial.println(temp); // Why "byIndex"?  
  // You can have more than one DS18B20 on the same bus.  
  // 0 refers to the first IC on the wire 
  delay(1000); 
  return temp;
}

int mq135(){
  int sensorValue;
  int digitalValue;
  Serial.print("Requesting CO2...");
  sensorValue = analogRead(0); // read analog input pin 0
  
  digitalValue = digitalRead(2); 
  if(sensorValue>400)
  {
    digitalWrite(13, HIGH);
  }
  else
    digitalWrite(13, LOW);
    
  Serial.print("CO2 ppm : ");
  Serial.println(sensorValue, DEC); // prints the value read
  //Serial.println(digitalValue, DEC);
 
  delay(1000); // wait 100ms for next reading
  return sensorValue;
}

void cleanLed(){
  for(int i = 0; i < 4; i++){     // 총 4번 반복
       lc.clearDisplay(i);
       delay(500);
    }
    Serial.println("Initialize LED");
    delay(1000);
}

// 1 : 좌, 2 : 우, 3 : 상, 4 : 하
void LedTurnOn(int dir){
  int devices=lc.getDeviceCount();
  unsigned long delaytime=30;
  int i;
  if(dir == 1){ // left
    for(i = 0; i < 4; i++){     // 총 4번 반복
       for(int col = 0; col <= 7; col++){
          if(col < 4){
            for(int row = 3 - col; row <= 4 + col; row++) 
              lc.setLed(i, row, col, true);
          }
          else {
            for(int row = 2; row <= 5; row++) 
              lc.setLed(i, row, col, true);
          }
       }
      delay(500);     // 딜레이 500ms
    }
  }
  else if(dir == 2){ // right
      for(i = 0; i < 4; i++){     // 총 4번 반복
         for(int col = 0; col <= 7; col++){
            if(col < 4){
              for(int row = 2; row <= 5; row++) 
                lc.setLed(i, row, col, true);
            }
            else {
              for(int row = col - 4; row <= 11 - col; row++) 
                lc.setLed(i, row, col, true);
            }
         }
        delay(500);     // 딜레이 500ms
      }
      
    }
    else if(dir == 3){ // up
        for(i = 0; i < 4; i++){     // 총 4번 반복
         for(int row = 0; row <= 7; row++){
            if(row < 4){
              for(int col = 3 - row; col <= 4 + row; col++) 
                lc.setLed(i, row, col, true);
            }
            else {
              for(int col = 2; col <= 5; col++) 
                lc.setLed(i, row, col, true);
            }
         }
        delay(500);     // 딜레이 500ms
      }
    }
    else if(dir == 4){
      for(i = 0; i < 4; i++){     // 총 4번 반복
         for(int row = 0; row <= 7; row++){
            if(row < 4){
              for(int col = 2; col <= 5; col++) 
                lc.setLed(i, row, col, true);
            }
            else {
              for(int col = row - 4; col <= 11 - row; col++) 
                lc.setLed(i, row, col, true);
            }
         }
        delay(500);     // 딜레이 500ms
      }
    }
    Serial.println("LED Set complete");
    delay(500);
}

void setup() {
  Serial.begin(9600);
  delay(1000);

  // WIFI connection : success
  Serial.println("WIFI CONNECTION START");
  WiFiCon();
  delay(1000);

  // DB connection : success
  Serial.println("DB CONNECTION START");
  DBcon();
  delay(1000);

  // 온도 센서 init
  sensors.begin();

  // 연기 센서 init
  pinMode(13, OUTPUT);
  pinMode( 3, INPUT);

  //dot matrix init
  int devices=lc.getDeviceCount();
    //we have to init all devices in a loop
    for(int address=0;address<devices;address++) {
      /*The MAX72XX is in power-saving mode on startup*/
      lc.shutdown(address,false);
      /* Set the brightness to a medium values */
      lc.setIntensity(address,8);
      /* and clear the display */
      lc.clearDisplay(address);
  }
  LedSet = 0;
}

void loop() {
  // put your main code here, to run repeatedly:
  int ds18b20_ret = 0, mq135_ret = 0;
    if(LedSet == 0){
      Serial.println("0000000000");
      delay(500);
     // get temperature : success
     ds18b20_ret = ds18b20();
      
     //get CO2 : success 
     mq135_ret = mq135();
    
     // store datas on mysql data base : success
     postData(ds18b20_ret, mq135_ret);
    
     // read data from mysql : success
     mysqlSelect();
  Serial.println("0 :" + (String)direction);
      delay(500);
     // turn on dot matrix
     if(direction > 0){
        LedSet = 1;
        LedTurnOn(direction);
      }
      delay(1000);
  }
  /*
  else if (LedSet == 1) {
      // read data from mysql : success
      mysqlSelect();
      Serial.println("1 :" + (String)direction);
      delay(500);
      //initialization
      if(direction <= 0){
        LedSet = 0;
        cleanLed();
      }
      
     // send Line message : success
     //IFTTT(id);
     delay(1000);
    }*/
}
