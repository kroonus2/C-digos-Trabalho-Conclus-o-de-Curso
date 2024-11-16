#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

// Configurações de Wi-Fi
const char* ssid = "Valeria 27";
const char* password = "1099251000";

// Certificado e chave privada
const char* cert = R"EOF(
-----BEGIN CERTIFICATE-----
MIIC+zCCAeOgAwIBAgIUdH7A+F28+f4Wk/HoNGtfr0H+lOgwDQYJKoZIhvcNAQEL
BQAwgYkxCzAJBgNVBAYTAlVTMQswCQYDVQQIDAJDQTERMA8GA1UEBwwIU2FudG9z
ZTEPMA0GA1UECgwGQ29tcGFueTEPMA0GA1UECwwGQ29tcGFueTEUMBIGA1UEAwwL
bG9jYWxob3N0LmNvbTElMCMGCSqGSIb3DQEJARYWcm9vdEBsb2NhbGhvc3QuY29t
MB4XDTIxMTAyMzEyNTkwOFoXDTMxMTAyMzEyNTkwOFowgYkxCzAJBgNVBAYTAlVT
MQswCQYDVQQIDAJDQTERMA8GA1UEBwwIU2FudG9zZTEPMA0GA1UECgwGQ29tcGFu
eTEPMA0GA1UECwwGQ29tcGFueTEUMBIGA1UEAwwLbG9jYWxob3N0LmNvbTElMCMG
CSqGSIb3DQEJARYWcm9vdEBsb2NhbGhvc3QuY29tMIIBIjANBgkqhkiG9w0BAQEF
AAOCAQ8AMIIBCgKCAQEA7OTVYUgInGIttoh5xDSB3t7Un1uZG5XjWftbGmz8rNhT
O2R/YaWcK9QXYl+PLowmn2J7XAJKgWyXUztCDExM1ct4w2jiyBN4iWg6XTlC7mZc
AmIDCFujPSXLP8LhLpy+Zy15H1HMLMPtM2YfDkHlm16Ofo9IFXed2KZt+16uLT9o
lD96hQFVx8gBWYJVEq0b5AvrT4+vX3gzyb+IwTlYUy/mr+UijR48bq8MwrcngYh1
FkjgfUdu5U5q1SA5bzEqaDk9mA0Nq+MDiNnmEP5hEgH9jlhZBQaZfO4Or0HUVaZZ
TVuHRzvC2Wz1F25mF5aDZcJkzv5KJ8hvngtknb2vYQIDAQABo1MwUTAdBgNVHQ4E
FgQUMyUanI68j2yzIH6EvFhStA/WcC8wHwYDVR0jBBgwFoAUMyUanI68j2yzIH6E
vFhStA/WcC8wDwYDVR0TAQH/BAUwAwEB/zANBgkqhkiG9w0BAQsFAAOCAQEAhG8h
tHFczOe0gZ8frwwpLzI/nRINeTOzOx1cUKOoZRb5Uu/YQ2u3ROH5ggIcTOOab95b
50N3zE9Pl82tr7BMSkslZmZKplSnql3tnZSiZcCmT+f9z0z1VC4WOG+bA+7Y/Yyd
aj3QfuWSJjdiMo74dA7N8kB4ZJ84INWa/lc7eqnJ5swPeD7NNcmzeFTqMF1dvFSe
fxh+a0p7EEFsSghJmHBfe0Qi9xSnPCE0G9ojKmF5LGUCBh6uYZXXfmrYtSBWTUeb
BGGuWGc6rBf36ZwzKKBRoFTKpTkWZ47n7t++4nTo6GiGTR9SbWn14qRU9/UuMCQ1
aGNAit4KJc72/WN4ew==
-----END CERTIFICATE-----
)EOF";

const char* private_key = R"EOF(
-----BEGIN PRIVATE KEY-----
MIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKkwggSlAgEAAoIBAQD8YeBPOuwxzwg5
W0ReZaFYYvGZKsfZz/wB1KrTP6W8Onf2mIStzL+uwSYBbT2+/jL+kGp4eJxNInXP
Q3UULcwB/RfIEM0sITtQ1kpUMVYw0EHeVjPPp1lsC0I5eqn61OkfU4RPxAAbBRJR
MK1jAFD8u7lk81kpOWq+Peay1bXNRhg6FUB5TDsb/YccvFe1oQzUobfl90yFFS/n
gfAzW4N36wJ68Yexfh0Rh/zTb5vjOnzTnU1KdrBYJkCeHSqnGUp0I6jVzKeXgV4o
v72WUXiFVJlyPHfrFlFFpbM0U3uVZ12ZLkDbotR5aA7Eb34P5UwYAxgNDsCZ9NvD
j2l1fY5ZAgMBAAECggEAY40Gh9quO+8qlJ8yxLO7EOXvj9rFAeR+aOhFxXbLf/vA
Vj+9Mbz/k1Y+dDCHZPyktW3h5m5AL89DUc35qOdvQCK4wTIlXPLikKTMPyOa8J2C
5rArptmTOsGVJ4CoBZyhm5biVdKwI/OZlFIMMbKIMl7n/bhzQz3QQezmrdKcD1SC
2EFJmJdlRGEc+JIXEZhYoJ4Ve3YAYfNO1uXZ6rWNHUKHr3S3BaHyvQ5r1qIBhZ4g
kqTO0g1+Fk+16Mf8lxLOH6dyh7hgtvODZkJm+MC7FxbM1PBm9ESsB17e96AqFX/k
K9vob2AGuAZE4mhIC1krmYgpM0nRMhQ5jtPaVqHbAQKBgQD8OtO/WvlL6pX8LnwV
AcJHeCdHp0d8FY5Q+tbmjsWj8wDr4QGQbn0wTLyd6TSyIoPhQzZ6wBeCEuC8gxTu
WyPSI4S9JXcFXLCI7ynUSFq4YpZgAkaRm0pSMR9mEZq8ePqKTA6HZvJZxY2h+n+I
bZRLSL60mtrvX0/cLJ1plflxtQKBgQD+1+XPnhTxwhlfmTW9pEKODh06khBSpqxX
7HWGoxK+cghQxGiWGNlADjU9DdsUNzkfd7fxaonOUEHbmXYD6AXtTIsD+b+T6iLx
nbEKGzDiyOSyyJkExWip4byREuNd1Bp7rbzEJphbh8NxB1oz8bcaahB9g5hwztDS
EJhDgzO3twKBgFIlW2LlUwv5nl6XJlLNhJ6Su25XTRE0eHLwykqKn9GrWzLnH1Hz
8GBS2MvGQF4mlV7u7/OwYpQnVUNzfrpTXDdIsq2G3c9Eu93LbhV5STHNGgCU6kcl
/sKTuVGoOfFT6vI8PlwrJHC6u1MoyDOUO7FEWXyZYQYCl9vBx8YwqPBFAoGAa5l+
RsW+GyH68HbGo8KRQKKQIm1KLKLZwryoV6iK3EsS3BdKK6g+FGeaZNd3Hn9K4WGo
9AAp8fZc2QzRDyXB5RPQAbD8XoAVHCR6dfadZdIMfZcQ/AeD8RafgeuOwR7lMH9Q
EZ6QgRY7RJlJINl1S9T4OQPt1/x9ekb9NgR3g5UCgYEA94nrrY62oQoXynOi5rgV
cSTUKlZWthoqktlnFOKy/GjGlPBT5S4y4eaCzT5Imn5QCyblzTOS3cNxUPRIld+k
hZRNeM/BGpa26gDOjlLhuW8dBCf+mh/0ohfrToGnxM8qxG8gKmrpe++QOECwzKgy
0+o9nml3BCRbSsfQ4buMl3w=
-----END PRIVATE KEY-----
)EOF";

// Cria o servidor na porta 443 (HTTPS)
AsyncWebServer server(443);

void setup() {
  Serial.begin(115200);

  // Configura o pino LED
  pinMode(2, OUTPUT);

  // Conecta-se ao Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Inicia o sistema de arquivos para armazenar os certificados
  if(!SPIFFS.begin(true)){
    Serial.println("Erro ao montar o SPIFFS");
    return;
  }

  // Configura HTTPS
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", "Servidor HTTPS funcionando!");
  });

  server.on("/H", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(2, HIGH);  // Liga o LED
    request->send(200, "text/html", "LED Ligado");
  });

  server.on("/L", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(2, LOW);  // Desliga o LED
    request->send(200, "text/html", "LED Desligado");
  });

  // Inicia o servidor HTTPS
  server.begin();
}

void loop() {
  // Código do loop (se necessário)
}
