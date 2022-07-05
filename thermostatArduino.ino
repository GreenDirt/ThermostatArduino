#include <OneWire.h>

/* 
max de chauffe : 60 sec
plus de 50° d'ecart : 100%(60 sec de chauffe)
25° d'ecart : 50%(soit 30 sec)


50*2 = 100
25*2 = 50
10*2 = 20
5*2 = 10
1*2 = 2


 informations transmises sous la forme : TEMP ACTUELLE, TEMP DESIREE, TEMPS DE CHAUFFE DEFINI, CHAUFFE ACTIVEE
 */

const int DS18B20_ID = 0x28;
OneWire ds(10);

int pinChauffe = 12;
int tempObjectif = 90;   
int temp = 0;
int intensiteChauffe = 0; //0 = chauffe desactivee
int tempsChauffe = 0; //Le temps depuis lequel on a activé la chauffe
int tempsMaxChauffe = 60; //Le temps max pendant lequel on chauffe
int ecartMaxTemperature; //L'ecart auquel on applique 100% de la chauffe
int ecartTemperature = 0;
int state = 1; //0 = chauffe pas; 1 = chauffe
int indexUpdate = 0;

String inputString = "";         // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete

void setup() 
{ 
  Serial.begin(9600);
  inputString.reserve(200);
  pinMode(pinChauffe, OUTPUT);
} 
 
void loop() 
{ 
  if (stringComplete) {
    
    if(0 < inputString.toInt() < 90){
      tempObjectif = inputString.toInt();
      //temp = inputString.toInt(); //Developpement seulement
    }
    
    // clear the string:
    
    inputString = "";
    stringComplete = false;
  }

  temp = getTemperatureDS18b20();
  tempsChauffe += 1;
  regulation();
    
  
  Serial.print(temp);
  Serial.print(",");
  Serial.print(tempObjectif);
  Serial.print(",");
  Serial.print(intensiteChauffe);
  Serial.print(",");
  Serial.println(state);
  delay(500);

  indexUpdate += 1;
} 

void regulation() {
  ecartTemperature = tempObjectif - temp;

  if(ecartTemperature > 30){
    activeChauffe();
  }
  else if(ecartTemperature <= 0) {
      coupeChauffe();
      intensiteChauffe = 0;
  }
  else{
    intensiteChauffe = ecartTemperature*2*tempsMaxChauffe/100*2;
  
    if(tempsChauffe >= intensiteChauffe and state == 1) {
      coupeChauffe();
      tempsChauffe = 0;
    }
  
     if(tempsChauffe >= intensiteChauffe and state == 0) {
      activeChauffe();
      tempsChauffe = 0;
    } 
  }
}

void activeChauffe(){
  digitalWrite(pinChauffe, HIGH);
  state = 1;
}

void coupeChauffe(){
  digitalWrite(pinChauffe, LOW);
  state = 0;
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}

float getTemperatureDS18b20(){
  byte i;
  byte data[12];
  byte addr[8];
  float temp =0.0;
  
  //Il n'y a qu'un seul capteur, donc on charge l'unique adresse.
  ds.search(addr);
  
  /*// Cette fonction sert à surveiller si la transmission s'est bien passée
  if (OneWire::crc8( addr, 7) != addr[7]) {
    Serial.println("getTemperatureDS18b20 : <!> CRC is not valid! <!>");
    return false;
  }*/
  
  /*// On vérifie que l'élément trouvé est bien un DS18B20
  if (addr[0] != DS18B20_ID) {
    Serial.println("L'équipement trouvé n'est pas un DS18B20");
    return false;
  }*/
  
  // Demander au capteur de mémoriser la température et lui laisser 850ms pour le faire (voir datasheet)
  ds.reset();
  ds.select(addr);
  ds.write(0x44);
  delay(850);
  // Demander au capteur de nous envoyer la température mémorisée
  ds.reset();
  ds.select(addr);
  ds.write(0xBE);
  
  // Le MOT reçu du capteur fait 9 octets, on les charge donc un par un dans le tableau data[]
  for ( i = 0; i < 9; i++) {
    data[i] = ds.read();
  }
  // Puis on converti la température (*0.0625 car la température est stockée sur 12 bits)
  temp = ( (data[1] << 8) + data[0] )*0.0625;
  
  return temp;
}
