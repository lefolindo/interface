void setup(){
Serial.begin(9600);
}
void loop(){

int Yaxis=analogRead(A0);

int color=map(Yaxis,170,515,0,255);

Serial.write(color);

delay(100);
}
