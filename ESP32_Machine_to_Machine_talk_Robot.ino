#include<WiFi.h>
#include<ESP32Servo.h>

const char* ssid = "Kunal Kr.";
const char* password = "Rajesh Press";

WiFiServer server(5000);

int rangen = 0;

Servo myservo;
int servoPin = 16;
int i=0;

#define echoPin 35
#define trigPin 32

#define enA 17
#define in1 18
#define in2 19
#define in3 21
#define in4 22
#define enB 23

String myStr,myStrR,myStrL;

const int freq = 500;
const int channel1 = 14;
const int channel2 = 15;
const int resolution = 10;

int speedRobot  = 800;
String cmnd = "" ;
bool obsFlag = false;
bool pathBlocked = false; 
bool rightCleared = false;

float duration,distance,Distance;

float ultrasonic()
{ digitalWrite(trigPin,LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin,HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin,LOW);

  duration = pulseIn(echoPin, HIGH);
  distance= (duration/2)*0.0343;

  return distance;
}
void setup() {
  pinMode(echoPin,INPUT);
  pinMode(trigPin,OUTPUT);

  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  myservo.setPeriodHertz(50);    // standard 50 hz servo
  myservo.attach(servoPin, 500, 2400);

  myservo.write(90);

  pinMode(enA,OUTPUT);
  pinMode(enB,OUTPUT);
  pinMode(in1,OUTPUT);
  pinMode(in2,OUTPUT);
  pinMode(in3,OUTPUT);
  pinMode(in4,OUTPUT);

  ledcSetup(  channel1 , freq , resolution );
  ledcSetup(  channel2 , freq , resolution );
  ledcAttachPin( enA , channel1 );
  ledcAttachPin( enB , channel2 );
  
Serial.begin(115200);

Serial.print("Connecting to ");
Serial.println(ssid);

WiFi.mode(WIFI_STA);
WiFi.begin(ssid,password);

while(WiFi.status()!=WL_CONNECTED)
{ delay(100);
  Serial.print(".");
}

Serial.println();
Serial.println("WiFi connected");
Serial.println(WiFi.localIP());

server.begin();
Serial.println("Server started.");

}

void loop() {
 
  WiFiClient client = server.available();
  
  
  if(!client)
  { return;
  }

  Serial.println("Got a client !");
  client.println("Connected.");

  for(;;) 
  {  
   Distance = ultrasonic();
   myStr = String(Distance);
   
   if(Distance <= 23.00)
   {
     client.println("Path blocked : "+ myStr + " cm");
     pathBlocked = true;
     stopRobot();
    
     delay(300);
     backward();
     delay(400);
     stopRobot();
     delay(300);
     float distanceR = lookRight();
     delay(300);
     float distanceL = lookLeft();
     delay(300);
     myStrR = String(distanceR);
     myStrL = String(distanceL);
   

      if(distanceR >= distanceL)
      {
        client.println("Right is clear : "+ myStrR + " cm");
        rightCleared = true;
      }
      else
      {
        client.println("Left is clear : "+ myStrL + " cm");
        rightCleared = false;
      }
      
    }
   else
   {
     pathBlocked = false;
   }
   
    if(client.available())
   { cmnd = client.readStringUntil('\r');
     Serial.println(cmnd);
   }
  if( cmnd == "u") 
     {
      forward();
      obsFlag = false;
    
      client.println("[ Forward ]");
     }
  else if( cmnd == "d") 
     {
      backward();
      obsFlag = false;
      
      client.println("[ Backward ]");
     }
  else if( cmnd == "l") 
     {
      left();
      obsFlag = false;
      
     
      client.println("[ Left ]");
     }
  else if( cmnd == "r") 
     {
      right();
      obsFlag = false;
      
      
      client.println("[ Right ]");
     }
  else if( cmnd == "rr") 
     {
      rollLeft();
      obsFlag = false;

     
      client.println("[ I am rolling(left) Ya Stoopid Hooman !! ]");
     }
  else if( cmnd == "rl") 
     {
      rollRight();
      obsFlag = false;

     
      client.println("[ I am rolling(right) Ya Stoopid Hooman !! ]");
     }
  else if( cmnd == "s") 
     {
      stopRobot();
      obsFlag = false;

     
      client.println("[ Stopped ]");
     }
  else if( cmnd == "a")
      {  
        obsFlag = false;

    
        speedRobot = 600;
        stopRobot();
        client.println("Adjusted Speed : 600");
      }
  else if( cmnd == "b") 
      {  
        obsFlag = false;

       
        speedRobot = 800;
        stopRobot();
        client.println("Adjusted Speed : 800");
      }
  else if( cmnd == "c")
     { 
      obsFlag = false; 

   
      speedRobot = 1023;
      stopRobot();
      client.println("Adjusted Speed : 1023");
     }

   else if( cmnd == "ran")
     { 
      obsFlag = false; 
      client.print(" Random Task triggered : ");

      rangen = random(1,6);

      switch(rangen)
      { 
        case 1 : client.println("loopRobot function");
                 loopRobot();
                 break;
                 
        case 2 : client.println("robotWalk function");
                 robotWalk(); 
                 break;
                 
        case 3 : client.println("stayBack function");
                 stayBack();
                 break;    
                 
        case 4 : client.println("roll2 function");
                 roll2();
                 break;
                 
        case 5 : client.println("MADNESS INCOMING !!");
                 madness();
                 break;  

        case 0 : break;                                       
      }
      rangen = 0;
     }

   else if( cmnd == "sp1") 
     {
      obsFlag = false;
      client.println("[ Robot makes a loop. ]");
      loopRobot();
     }
     
    else if( cmnd == "sp2") 
     {
      obsFlag = false;
      client.println("[ Robot takes a walk. ]");
      robotWalk();
     }
     
    else if( cmnd == "sp3") 
     {
      obsFlag = false;
      client.println("[ Robot stays back. ]");
      stayBack();
     }

    else if( cmnd == "sp4") 
     {
      obsFlag = false;
      client.println("[ Robot makes a special roll. ]");
      roll2();
     }

    else if( cmnd == "sp5") 
     {
      obsFlag = false;
      client.println("[ MADNESSSSSSS !!!!! ]");
      madness();
     }

          
   else if( cmnd == "o")
     { 
      stopRobot();
      obsFlag = true;

      client.println(":: Obstacle Avoiding Mode ::");  
     }

  
  cmnd = "";
  delay(10);

  if(obsFlag)
  {   if(pathBlocked)
    { 
      stopRobot();
      
       if(rightCleared)
      { 
       right();
       delay(10);
      }
      else
      {
      left();
      delay(10);
      }
    }
    else
    {
      forward();
      delay(10);
    }
  }
 }
}
float lookRight()
{  for( i=90 ; i>=0 ; i--)
   {myservo.write(i);
    delay(4);
   }
  delay(50);
  Distance = ultrasonic();
  delay(50);
 for( i=0 ; i<=90 ; i++)
   {myservo.write(i);
    delay(4);
   }
  return Distance;
}

float lookLeft()
{ for( i=90 ; i<=180 ; i++)
   {myservo.write(i);
    delay(4);
   }
  delay(50);
  Distance = ultrasonic();
  delay(50);
  for( i=180 ; i>=90 ; i--)
   {myservo.write(i);
    delay(4);
   }
  return Distance;
  delay(100);
}

void forward()
{ digitalWrite(in1 , LOW);
  digitalWrite(in2 , HIGH);
  ledcWrite(channel1 , speedRobot);

  digitalWrite(in3 , LOW);
  digitalWrite(in4 , HIGH);
  ledcWrite(channel2 , speedRobot);
}

void backward()
{ digitalWrite(in1 , HIGH);
  digitalWrite(in2 , LOW);
  ledcWrite(channel1 , speedRobot);

  digitalWrite(in3 , HIGH);
  digitalWrite(in4 , LOW);
  ledcWrite(channel2 , speedRobot);
}


void left()
{ digitalWrite(in1 , HIGH);
  digitalWrite(in2 , LOW);
  ledcWrite(channel1 , speedRobot);

  digitalWrite(in3 , LOW);
  digitalWrite(in4 , HIGH);
  ledcWrite(channel2 , speedRobot);

  delay(500);
  forward();
  
}

void rollLeft()
{
  digitalWrite(in1 , HIGH);
  digitalWrite(in2 , LOW);
  ledcWrite(channel1 , speedRobot);

  digitalWrite(in3 , LOW);
  digitalWrite(in4 , HIGH);
  ledcWrite(channel2 , speedRobot);
}

void right()
{ digitalWrite(in1 , LOW);
  digitalWrite(in2 , HIGH);
  ledcWrite(channel1 , speedRobot);

  digitalWrite(in3 , HIGH);
  digitalWrite(in4 , LOW);
  ledcWrite(channel2 , speedRobot);

  delay(500);
  forward();

}

void rollRight()
{
  digitalWrite(in1 , LOW);
  digitalWrite(in2 , HIGH);
  ledcWrite(channel1 , speedRobot);

  digitalWrite(in3 , HIGH);
  digitalWrite(in4 , LOW);
  ledcWrite(channel2 , speedRobot);

}

void stopRobot()
{ digitalWrite(in1 , LOW);
  digitalWrite(in2 , LOW);
  
  digitalWrite(in3 , LOW);
  digitalWrite(in4 , LOW);

}


void loopRobot()
{ stopRobot();
  delay(100);
  forward();
  delay(1500);
  stopRobot();
  delay(100);
  right();
  delay(1500);
  stopRobot();
  delay(100);
  right();
  delay(1500);
  stopRobot();
  delay(100);
  right();
  delay(1500);
  stopRobot();
  delay(100);
}

void robotWalk()
{ stopRobot();
  delay(100);
  forward();
  delay(2000);
  stopRobot();
  delay(100);
  right();
  delay(50);
  stopRobot();
  delay(100);
  right();
  delay(2000);
  stopRobot();
  delay(100);
}

void stayBack()
{ stopRobot();
  delay(100);
  backward();
  delay(2000);
  stopRobot();
  delay(100);
}

void roll2()
{ stopRobot();
  delay(100);
  rollLeft();
  delay(5000);
  stopRobot();
  delay(300);
  rollRight();
  delay(5000);
  stopRobot();
  delay(300);
}

void madness()
{ stopRobot();
  delay(100);
  forward();
  delay(1500);
  stopRobot();
  delay(100);
  rollLeft();
  delay(2000);
  stopRobot();
  delay(100);
  right();
  delay(1500);
  stopRobot();
  delay(100);
  rollRight();
  delay(1500);
  stopRobot();
  delay(100);
  left();
  delay(1500);
  stopRobot();
  delay(100);
  left();
  delay(1500);
  stopRobot();
  delay(100);
  rollRight();
  delay(5000);
  stopRobot();
  delay(100);
}
