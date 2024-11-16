#include <SPI.h>
#include <Ethernet.h>
#include <L293.h>

// Definição dos pinos do motor
const int speedPin = 8;
const int forwardPin = 8;
const int reversePin = 4;
const int ledPin = 5;

// Inicialização do motor usando a biblioteca L293
L293 motor(speedPin, forwardPin, reversePin);

// Variável para armazenar o estado do motor
int motorState = LOW;
int ledState =  LOW;

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
//IPAddress ip(192, 168, 1, 177);

IPAddress ip(192, 168, 100, 6);

// Inicializa a biblioteca do servidor Ethernet
EthernetServer server(80);

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Ethernet WebServer Example");

  // Inicia a conexão Ethernet e o servidor:
  Ethernet.begin(mac, ip);

  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }

  // Inicia o servidor
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}

void loop() {
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // a solicitação HTTP termina com uma linha em branco
    bool currentLineIsBlank = true;
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        currentLine += c;

        // Se a linha atual contém uma solicitação GET
        if (currentLine.endsWith("\n") && currentLine.length() > 1) {
          // Verifica se a solicitação é para o motor
          if (currentLine.indexOf("?m1=1") != -1) {
            ledState = HIGH;
            motorState = HIGH; // Liga o motor
            motor.forward(255); // Define a direção e a velocidade do motor
          } else if (currentLine.indexOf("?m1=0") != -1) {
            ledState =  LOW;
            motorState = LOW; // Desliga o motor
            motor.stop(); // Para o motor
          }
          // Limpa a linha atual
          currentLine = "";
        }
        // Se você chegou ao final da linha (recebeu um caractere de nova linha)
        // e a linha está em branco, a solicitação HTTP terminou
        if (c == '\n' && currentLineIsBlank) {
          // Envia um cabeçalho de resposta HTTP padrão
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // a conexão será fechada após a resposta ser enviada
          client.println("Refresh: 5");         // atualiza a página automaticamente a cada 5 segundos
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          // Output the value of each analog input pin
          for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
            int sensorReading = analogRead(analogChannel);
            client.print("analog input ");
            client.print(analogChannel);
            client.print(" is ");
            client.print(sensorReading);
            client.println("<br />");
          }
          client.println("<p>Estado do motor: ");
          client.print((motorState == HIGH) ? "Ligado" : "Desligado");
          client.println("</p>");
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you are starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you have gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}
