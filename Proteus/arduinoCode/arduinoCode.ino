byte camData [10][640] {};

void setup(){
  for (byte i = 0; i < 10; ++i){
    for (int j = 0; j < 640; ++j){
      camData [i][j] = j / float(i + 1);
    }
  }
  Serial.begin(9600);
}

void loop(){
  for (byte i = 0; i < 10; ++i){
    for (int j = 0; j < 640; ++j){
      Serial.print(camData [i][j]);
      Serial.print("  ");
    }
    Serial.println();
  }

  while(1);
}
