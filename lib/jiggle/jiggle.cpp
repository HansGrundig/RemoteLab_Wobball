#include "Jiggle.h"

// Konstanten & Servos (Nur hier in der Lib sichtbar)
const int angOrig = 89; 
Servo servo0;
Servo servo1;
Servo servo2;

void initJiggle(uint8_t pin0, uint8_t pin1, uint8_t pin2) {
    servo0.write(angOrig);
    servo1.write(angOrig);
    servo2.write(angOrig);
    delay(200);
    servo0.attach(pin0);
    servo1.attach(pin1);
    servo2.attach(pin2);
}

double theta(int leg, double hz, double nx, double ny) {
    // Table Dimensions for Inverse Kinematic
    double d = 7.5;  //distance from the center of the base to any of its corners
    double e = 7.5;  //distance from the center of the platform to any of its corners
    double f = 2.5;  //length of link #1
    double g = 6.0;  //length of link #2

    //Calculation Variables
    double nmag, nz;  //magnitude and z component of the normal vector
    double x, y, z;   //generic variables for the components of legs A, B, and C
    double mag;       //generic magnitude of the leg vector
    double angle;     //generic angle for legs A, B, and C

    //create unit normal vector
    nmag = sqrt(pow(nx, 2) + pow(ny, 2) + 1);  //magnitude of the normal vector
    nx /= nmag;
    ny /= nmag;
    nz = 1 / nmag;
    //calculates angle A, B, or C
    switch (leg) {
        case 0:  //Leg A
        y = d + (e / 2) * (1 - (pow(nx, 2) + 3 * pow(nz, 2) + 3 * nz) / (nz + 1 - pow(nx, 2) + (pow(nx, 4) - 3 * pow(nx, 2) * pow(ny, 2)) / ((nz + 1) * (nz + 1 - pow(nx, 2)))));
        z = hz + e * ny;
        mag = sqrt(pow(y, 2) + pow(z, 2));
        angle = acos(y / mag) + acos((pow(mag, 2) + pow(f, 2) - pow(g, 2)) / (2 * mag * f));
        break;
        case 1:  //Leg B
        x = (sqrt(3) / 2) * (e * (1 - (pow(nx, 2) + sqrt(3) * nx * ny) / (nz + 1)) - d);
        y = x / sqrt(3);
        z = hz - (e / 2) * (sqrt(3) * nx + ny);
        mag = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
        angle = acos((sqrt(3) * x + y) / (-2 * mag)) + acos((pow(mag, 2) + pow(f, 2) - pow(g, 2)) / (2 * mag * f));
        break;
        case 2:  //Leg C
        x = (sqrt(3) / 2) * (d - e * (1 - (pow(nx, 2) - sqrt(3) * nx * ny) / (nz + 1)));
        y = -x / sqrt(3);
        z = hz + (e / 2) * (sqrt(3) * nx - ny);
        mag = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
        angle = acos((sqrt(3) * x - y) / (2 * mag)) + acos((pow(mag, 2) + pow(f, 2) - pow(g, 2)) / (2 * mag * f));
        break;
    }
    return (angle * (180 / PI));  //converts angle to degrees and returns the value
}

//Let platform wobble a bit
void jiggle(){
    // WARNING:
    // angle to servo only between 80 - 98 
    // pitch / roll for platform (input to theta() +-0.03
    int pos[3];
    
    //Constants
    double hz = 8.0; // height of the platform
    double angOffset = 32; // angOrig = 90 (irl 90 ^= 90 * 1.5 = 135) theta(i,8.0, 0.01, 0 ) --> 121+-2 | theta - angoffset ~= 90+-2
    //Paramters (Be careful not to overstep values noted in WARNING)
    static double step = 0.005;
    double nx = 0.0;
    double ny = 0.0;
    const int range = 3; // range * 4 = maximum amount of steps from lowest to highest (pro + 1 range 1-2 max degree) MAX. 4
    // change pitch und yaw of table slowly between 0-4*step and 0+4*step (start 0 -> 4x nx,ny - step -> 8x nx,ny + step -> 4x nx,ny - step)
    for (int t = 0; t < range*4; t++){
      if ((t < range) || ( t >= (range*3)) ){ // first and ,last quarters of steps, nx += step and ny -=step
        nx += step;
        ny -= step;
      }
      if ((t >= range) && (t < range * 3) ){ // middle range of steps nx -= step und ny += step
        nx -= step;
        ny += step;
      }
      Serial.print("nx*10:");
      Serial.println(nx*10);
      Serial.print("ny*10:");
      Serial.println(ny*10); 
      for (int i = 0; i < 3; i++) {
           pos[i] = round((theta(i, hz, nx, ny)) - angOffset); // Calculate servo angles using inversekinematics 
           //Serial.println(pos[i]);}
      //Serial.println("-----");
      servo0.write(pos[0]);
      servo1.write(pos[1]);
      servo2.write(pos[2]);
      delay(50);
    }
    }
}
