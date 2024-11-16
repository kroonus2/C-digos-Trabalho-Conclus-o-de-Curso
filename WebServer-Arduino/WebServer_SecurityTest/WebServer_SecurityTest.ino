#include <SPI.h>
#include <Ethernet.h>
#include <L293.h>
#include <Crypto.h>
#include <AES.h>
#include <string.h>

//limitação de requisições
unsigned long requestInterval = 5000; // 5 segundos

// Definição dos pinos do motor
const int speedPin = 8;
const int forwardPin = 8;
const int reversePin = 4;

// Inicialização do motor usando a biblioteca L293
L293 motor(speedPin, forwardPin, reversePin);

// Variável para armazenar o estado do motor
int motorState = LOW;

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 100, 6);

// Inicializa a biblioteca do servidor Ethernet
EthernetServer server(80);

// Definindo a chave AES
byte key[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
AES128 aes128;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Ethernet WebServer with AES Encryption");

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

  // Inicializa a chave de criptografia AES
  aes128.setKey(key, sizeof(key));
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
            motorState = HIGH; // Liga o motor
            motor.forward(255); // Define a direção e a velocidade do motor
          } else if (currentLine.indexOf("?m1=0") != -1) {
            motorState = LOW; // Desliga o motor
            motor.stop(); // Para o motor
          }
          // Limpa a linha atual
          currentLine = "";
        }
        // Se você chegou ao final da linha (recebeu um caractere de nova linha)
        // e a linha está em branco, a solicitação HTTP terminou
        if (c == '\n' && currentLineIsBlank) {
          // Cria o texto simples com o estado do motor
          byte plaintext[16] = {0}; // Inicializa um bloco de 16 bytes
          if (motorState == HIGH) {
            strcpy((char*)plaintext, "Motor: ON"); // Copia o texto "Motor: ON" para o bloco de texto
          } else {
            strcpy((char*)plaintext, "Motor: OFF"); // Copia o texto "Motor: OFF" para o bloco de texto
          }

          // Criptografa o texto simples
          byte cypher[16];
          aes128.encryptBlock(cypher, plaintext);

          // Envia um cabeçalho de resposta HTTP padrão
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // a conexão será fechada após a resposta ser enviada
          client.println("Refresh: 50");         // atualiza a página automaticamente a cada 5 segundos
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("<p>Estado do motor criptografado:</p>");
          client.println("<p>");
          for (int i = 0; i < sizeof(cypher); i++) {
            client.print(cypher[i], HEX); // Imprime cada byte criptografado em formato hexadecimal
            client.print(" ");
          }
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
