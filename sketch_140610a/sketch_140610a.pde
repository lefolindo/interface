
import processing.serial.*;

Serial puerto;
int colorp ;
void setup(){

size(800,600);

print(Serial.list());

colorp=0;
puerto=new Serial(this,Serial.list()[0],9600);
}
void draw(){

if (puerto.available()>0){
colorp=puerto.read();

}

background(0,0,colorp);
}
