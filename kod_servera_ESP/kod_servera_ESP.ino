#include  <ESP8266WiFi.h>  
 
const char* ssid     ="Orange_Swiatlowod_0960"; // Tu wpisz nazwę swojego wifi
const char* password = "JL27QYVJRQR6"; // Tu wpisz hasło do swojego wifi

WiFiServer server(80);
 
void setup() {
Serial.begin(115200);
delay(10);
 
// Connect to WiFi network
Serial.println();
Serial.println();
Serial.print("Connecting to ");
Serial.println(ssid);
 
WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}
 
Serial.println("");
Serial.println("WiFi connected");
 
// Start the server
server.begin();
Serial.println("Server started");
 
// Print the IP address
Serial.print("Use this URL to connect: ");
Serial.print("http://");
Serial.print(WiFi.localIP());
Serial.println("/"); 
}

void loop() {
// Check if a client has connected
WiFiClient client = server.available();
if (!client) {
  return;
}
 
            // Wait until the client sends some data

int timewate = 0;
while(!client.available()){
  delay(1);
  timewate = timewate +1;
  if(timewate>1800)
  {
    Serial.println(">>> Client Timeout !");
    client.stop();
    return;
  }
}
 
// Read the first line of the request
String request = client.readStringUntil('\r');

client.flush();
 
// Match the request 
int value = LOW;
if (request.indexOf("/b0") != -1)  {
  Serial.println("n");
  value = HIGH;
}
 
if (request.indexOf("/b1") != -1)  {
  Serial.println("w");
  value = LOW;
}
if (request.indexOf("/b2") != -1)  {
  Serial.println("s");
  value = HIGH;
}
if (request.indexOf("/b3") != -1)  {
  Serial.println("e");
  value = LOW;
}
 
// Return the response
client.println("");
client.println("");
client.println("<!DOCTYPE HTML><html lang='pl'><head><meta charset='utf-8'>"); //  do not forget this one
client.println("<title>Sterowanie robotem z pomocą modułu ESP</title><script type='text/javascript'>");
client.println("");
client.println("function sendChar(wiadomosc){Serial.println('wiadomosc')}");
client.println("</script><style>body{margin:0;height: 70%;background-color: darkgrey;font-family: 'Times New Roman',serif;font-size: 22px;color: white;");  
client.println("-webkit-user-select: none;-moz-user-select: none;-ms-user-select: none;user-select: none;}html{height:70%;}h1{font-size: 30px;");
client.println("font-weight: 400;text-align: center;letter-spacing: 5px;margin-top: 20px;margin-bottom: 0px;}.przyciski{width: 100%;height: 70%;text-align: center;vertical-align:bottom;}");
client.println(".btn{width: 20%;height: 40%;display: inline-block;margin: 3px;border: 5px solid black;border-radius: 6px;cursor: pointer;");
client.println("filter: brightness(80%);transition: all .3s ease-in-out;background-color: dimgray;vertical-align: middle;text-align: center;line-height: 60px;font-size:30px;color: white;}.notbtn{width: 20%;");
client.println("height: 35%;display: inline-block;margin: 3px;}.btn:hover{border: 4px solid purple;filter: brightness(100%);}</style></head><body>");
client.println("<center><h1>Połączenie z Arduino</h1></center><br><center>Przyciski pozwalają na sterowanie robotem</center><br><div class='przyciski'>");
client.println("<div class='notbtn' id='a0'></div><a href='/b0'><div class='btn' id='b0'>Up</div></a><div class='notbtn' id='a1'></div><br>");
client.println("<a href='/b1'><div class='btn' id='b1'>Left</div></a><a href='/b2'><div class='btn' id='b2'>Down</div></a><a href='/b3'><div class='btn' id='b3'>Right</div></a></div></body></html>");
delay(1);
}
