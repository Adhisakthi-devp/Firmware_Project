#define MODEM_TX 17
#define MODEM_RX 16

#define en_modem 32
#define pwr_key 33
#define rst_modem 25

String modemResponse = "";

void iopin()
{
  pinMode(en_modem, OUTPUT);
  pinMode(pwr_key, OUTPUT);
  pinMode(rst_modem, OUTPUT);
}

void setpin()
{
  digitalWrite(en_modem, HIGH);
  delay(2000);

  digitalWrite(en_modem, LOW);
  delay(2000);

  digitalWrite(pwr_key, LOW);
  delay(2000);

  digitalWrite(pwr_key, HIGH);
  delay(10000);

  Serial.println("Power ON complete");
}

void checking1()
{
  int R = -1;
  int B = -1;

  int pos = modemResponse.indexOf("+CSQ:");

  if (pos != -1)
  {
    sscanf(modemResponse.c_str() + pos, "+CSQ: %d,%d", &R, &B);

    Serial.print("Signal Strength Value : ");
    Serial.println(R);

    Serial.print("BER Value : ");
    Serial.println(B);
  }
  int dbm = -113 + (R * 2); 
  Serial.print(dbm);


  if (R == 31)
  {
    Serial.println("Signal Strength : Excellent");
  }
  else if (R >= 20)
  {
    Serial.println("Signal Strength : Very Good");
  }
  else if (R >= 10)
  {
    Serial.println("Signal Strength : Good");
  }
  else if (R >= 2)
  {
    Serial.println("Signal Strength : Poor");
  }
  else if (R == 1)
  {
    Serial.println("Signal Strength : Very Poor");
  }
  else if (R == 0)
  {
    Serial.println("Signal Strength : No Signal");
  }
  else if (R == 99)
  {
    Serial.println("Signal Strength : Unknown");
  }
  else
  {
    Serial.println("Invalid Value");
  }

  Serial.print("BER Quality : ");

  if (B == 99)
  {
    Serial.println("Unknown");
  }
  else if (B <= 1)
  {
    Serial.println("Excellent");
  }
  else if (B <= 3)
  {
    Serial.println("Good");
  }
  else if (B <= 7)
  {
    Serial.println("Poor");
  }
  else
  {
    Serial.println("Invalid");
  }
}

void checking2()
{
  int A = -1;
  int Z = -1;

  int pos = modemResponse.indexOf("+CREG:");

  if (pos != -1)
  {
    sscanf(modemResponse.c_str() + pos, "+CREG: %d,%d", &A, &Z);

    Serial.print("State : ");
    Serial.println(A);

    Serial.print("Network Registration Status : ");
    Serial.println(Z);
  }

  if (A == 1 && Z == 1)
  {
    Serial.println("Successfully Connected to Home Network");
  }
  else if (A == 1 && Z == 2)
  {
    Serial.println("Currently Searching for Signal");
  }
  else if (A == 1 && Z == 5)
  {
    Serial.println("Connected to Roaming Network");
  }
  else if (A == 1 && Z == 3)
  {
    Serial.println("Registration Denied");
  }
  else
  {
    Serial.println("Network Status Unknown");
  }
}

void sendAT(String cmd, uint32_t waitTime = 3000)
{
  Serial.println();
  Serial.print("CMD : ");
  Serial.println(cmd);

  modemResponse = "";

  Serial2.println(cmd);

  unsigned long start = millis();

  while (millis() - start < waitTime)
  {
    while (Serial2.available())
    {
      char c = Serial2.read();
      modemResponse += c;
      Serial.write(c);
    }
  }
  if (cmd == "AT+CSQ")
    checking1();

  if (cmd == "AT+CREG?")
    checking2();

  Serial.println();
}

void calling()
{
  Serial2.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);

  delay(3000);

  sendAT("AT");
  sendAT("AT+CPIN?");
  sendAT("AT+CGDCONT=1,\"IP\",\"www\"");
  sendAT("AT+CGACT=1,1");
  sendAT("AT+CNMP=2");
  sendAT("AT+CREG=1");
  sendAT("AT+CGREG=1");
  sendAT("AT+CEREG=1");

  sendAT("AT+CSQ");
  sendAT("AT+CREG?");
  sendAT("AT+CGREG?");
  sendAT("AT+CEREG?");
  sendAT("AT+COPS?");

  

  // Optional: Check IP Address
  sendAT("AT+CGPADDR=1");
}

void setup()
{
  Serial.begin(115200);

  iopin();

  Serial.println("Powering Modem...");

  setpin();

  calling();
}

void loop()
{
}
