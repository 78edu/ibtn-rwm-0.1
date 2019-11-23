//ibtn-rwm 0.1
//����� ����������� �� ������ ��������� ����� � ������ ������.
//������� ����� ������� ��� ���������� �������������� � ������ ������������� ���.
//���������� writeByte ��� ������� ���� � �������� �����������.


#include <OneWireSTM.h> //����� ������������ OneWire.h ��� ������� avr arduino
int pin = 10;
int counter = 0; //��� ������ ����� ����� � ����������.
OneWire iButton(pin); 
byte key_to_write[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
byte tempkey[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

int serialByte ;
int workmode = 0; //0 - idle 1 - read 2 - write 3 - modify

void setup (void)
{
  Serial.begin(9600);
}
///////////////
void serialFlush()
{
  while (Serial.available() > 0)
  {
    char t = Serial.read();
  }
}

void commandRead(int inByte, int outMode)
{
  if (Serial.available() > 0)
  {
    serialByte = Serial.read(); // read the incoming byte:

    if (serialByte == inByte)
    {
      workmode = outMode;
      serialFlush();
    }
  }
}

int writeByte(byte data) {
  int data_bit;
  for (data_bit = 0; data_bit < 8; data_bit++) 
  {
			if (data & 1) {
			digitalWrite(pin, LOW);
			pinMode(pin, OUTPUT);
			delayMicroseconds(60);
			pinMode(pin, INPUT);
			digitalWrite(pin, HIGH);
			delay(10);
			} else {
			digitalWrite(pin, LOW);
			pinMode(pin, OUTPUT);
			pinMode(pin, INPUT);
			digitalWrite(pin, HIGH);
			delay(10);
			}	
    data = data >> 1;
  }
  return 0;
}
///////////////
void loop(void) {

  switch (workmode) {
    case 0:
	//Idle
      Serial.println("Send R for Read, M for Modify and W for Write");
      Serial.print("Key to write:");
	  
      for (int i = 0; i < 8; i++) 
	  {
        Serial.print(key_to_write[i], HEX);
        Serial.print(" "); 
	  }
	  
      Serial.println("");
      delay(2000);
      if (Serial.available() > 0) 
	  {
			serialByte = Serial.read(); // read the incoming byte:

			if (serialByte == 0x52) //R
			{
			workmode = 1;
			serialFlush();
			Serial.println("Read key mode - E for End");
			}
			if (serialByte == 0x57)
			{
			workmode = 2; //W
			serialFlush();
			Serial.println("Write key mode - E for End");
			}
			if (serialByte == 0x4D) //M
			{
			workmode = 3; //modify mode
			serialFlush();
			Serial.println("Modify key mode - E for End");
			Serial.println("Input key in decimal format like: 255.255... for FF FF");
			}

      }
      break;

	  
    case 1:
	//Read
      pinMode(pin, INPUT);
      byte addr[8]; // ������ ��� �������� ������ �����

			if ( !iButton.search(addr) ) 
			{ // ���� ���� �� ��������
			commandRead(0x45, 0); // ���� E ��� ������
			delay(1000);
			return; // � ��������� �����
			}
	
      Serial.print("Key : ");
	  
			for (int i = 0; i < 8; i++) 
			{
			Serial.print(addr[i], HEX); // ������� �������� ������ �����
			key_to_write[i] = addr[i]; //����� ���� ����, ������ ��� ����� ����� ��������
			Serial.print(" ");
			//delay(1000);
			}
			
      Serial.println();
      iButton.reset(); // ����� �����
      
	  Serial.println("Key in Decimal:"); //�� ���������� ��� ������� � HEX, 
										//������� ���������� �������� ����� �� ����� ���� ����� ����� �������
			for (int i = 0; i < 8; i++) 
			{
			Serial.print(key_to_write[i], DEC); // ������� �������� ������ ����� � ���������� ����
			Serial.print(".");
			//delay(1000);
			}
			
      Serial.println();
      commandRead(0x45, 0); //���� E ��� ������
      break;

    case 2:
	//Write
      //Serial.println("Write key mode - E for End");
      commandRead(0x45, 0);
      pinMode(pin, OUTPUT);
      delay(1000);
      //////////
      delay (200);
      iButton.skip();
      iButton.reset();
      iButton.write(0x33); // ������ �������� ������ �����

      byte data[8]; // ������ ��� �������� ������ �����
      iButton.read_bytes(data, 8); // ��������� ������ ������������ �����, 8�8=64 ����

      if (data[0] & data[1] & data[2] & data[3] & data[4] & data[5] & data[6] & data[7] == 0xFF) {
        return; // ���� ���� �� �������� � �����������, ��������� ��������� � ���, ���� ����� ��������
		// ��������� ������, �.�. ����� ��������� �������� �������, �� ������ ������.
      }

      iButton.skip();
      iButton.reset();
      iButton.write(0xD1); // ������� ���������� ������
	  
      digitalWrite(pin, LOW);
      pinMode(pin, OUTPUT);
      delayMicroseconds(60);
      pinMode(pin, INPUT);
      digitalWrite(pin, HIGH);
      delay(10);

	  
      // ������� ����, ������� ���������� ��������:
      Serial.print("Writing iButton ID: ");
      for (byte i = 0; i < 8; i++) 
	  {
      Serial.print(key_to_write[i], HEX);
      Serial.print(" ");
      }
      Serial.print("\n");

	  
      iButton.skip();
      iButton.reset();
      iButton.write(0xD5); // ������� ������
	  
			for (byte i = 0; i < 8; i++) 
			{
			writeByte(key_to_write[i]);
			Serial.print("*");
			}
			Serial.print("\n");

      iButton.reset();
      iButton.write(0xD1); // ������� ������ �� ������ ������
      digitalWrite(pin, LOW);
      pinMode(pin, OUTPUT);
      delayMicroseconds(10);
      pinMode(pin, INPUT);
      digitalWrite(pin, HIGH);
      delay(10);

      Serial.println("Success!");
      Serial.println("One more key or E for End");
      //workmode=1;
      commandRead(0x45, 0);
      break;

    case 3:
	//Modify
      delay(200);

      if (Serial.available() > 0) 
	  {
      serialByte = Serial.parseInt(); // read the incoming byte:
      Serial.print("Received:");
      Serial.println(serialByte, HEX);
      tempkey[counter] = serialByte;
      counter = counter + 1;
      }
	  
      if (counter == 8) 
	  {
      Serial.println("Key changed");
			for (int i = 0; i < 8; i++) 
			{
			Serial.print(tempkey[i], HEX);
			Serial.print(" ");
			}
        //workmode=0;
      counter = 0;
			for (int i = 0; i < 8; i++) 
			{
			key_to_write[i] = tempkey[i];
			}

        serialFlush();
        workmode = 0;
        Serial.println("");
      }
      //commandRead(0x45, 0);

      break;

  }
}