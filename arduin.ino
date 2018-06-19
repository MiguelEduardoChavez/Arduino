#include <SPI.h> 
#include <Ethernet.h> 
 
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,1,100);
EthernetServer server(80);
 

String estado="OFF";
int pinSonido = 4;
int relay = 5;
int aplausos = 0;
long rangoInicio = 0;
long rangoDetectado = 0;
boolean estadoLuz = true;
 
void setup()
{
  Serial.begin(9600);
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
 
 
}
 
void loop()
{
  EthernetClient client = server.available(); 
  conServidor(client);
  Serial.println(aplausos);
  contarAplauso();
  if (millis()-rangoInicio >= 40)
  {
    //Serial.println(aplausos);
    if (aplausos == 2)
    {
      if (!estadoLuz)
        {
          
          estadoLuz = true;
          pinMode(relay,LOW);
          Serial.println(estadoLuz);
          enviarRespuesta(client);
          Serial.println("Apagado");

        }
    }
    if(aplausos ==1){
        if (estadoLuz)
         {
          estadoLuz = false;
          pinMode(relay,HIGH);
          Serial.println(estadoLuz);
          enviarRespuesta(client);

        }
      }
    aplausos = 0;
  }
}

void conServidor(EthernetClient client){
    if (client) {
    Serial.println("new client");
    boolean currentLineIsBlank = true; 
    String cadena=""; 
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        cadena.concat(c);
         int posicion=cadena.indexOf("LED="); 
          if(cadena.substring(posicion)=="LED=ON")
          {
           pinMode(relay,HIGH);
                      estadoLuz = false;
                      estado="ON";
          }
          if(cadena.substring(posicion)=="LED=OFF")
          {
            pinMode(relay,LOW);
            estado="OFF";
            estadoLuz = true;
            Serial.println(estadoLuz);
          }
 
      if (c == '\n' && currentLineIsBlank) {
        enviarRespuesta(client);
        break;
        }
        if (c == '\n') {
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }
    delay(100);
    client.stop();// Cierra la conexión
  }
  }
  
  void contarAplauso(){
    Serial.println(aplausos);
    int status_sensor = digitalRead(pinSonido);
  if (status_sensor == 0)
  {
    if (aplausos == 0)
    {
      rangoInicio = rangoDetectado = millis();
      aplausos++;
    }
    else if (aplausos > 0 && millis()-rangoDetectado >= 50)
    {
      rangoDetectado = millis();
      aplausos++;
    }
  }
}

void enviarRespuesta(EthernetClient client){
             // Enviamos al cliente una respuesta HTTP
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println();
            client.println("<html>");
            client.println("<head>");
            client.println("</head>");
            client.println("<body>");
            client.println("<h1 align='center'>Practica 6</h1>");            
            client.println("<div style='text-align:center;'>");
            client.println("<button onClick=location.href='./?LED=ON\' style='margin:auto;background-color: #DAF7A6;color: snow;padding: 10px;border: 1px solid #3F7CFF;width:65px;'>");
            client.println("ON");
            client.println("</button>");
            client.println("<button onClick=location.href='./?LED=OFF\' style='margin:auto;background-color: #DAF7A6;color: snow;padding: 10px;border: 1px solid #3F7CFF;width:65px;'>");
            client.println("OFF");
            client.println("</button>");
            client.println("<br /><br />");
            client.println("<h2>Estado del Foco: ");
            if(estadoLuz){
              client.print("Apagado");
              }else {
                client.print("Encendido");
              }
            client.print("</h2>");
            client.println("</b></body>");
            client.println("</html>");
}

