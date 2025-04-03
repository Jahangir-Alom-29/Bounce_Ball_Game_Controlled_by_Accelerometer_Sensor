#include <Wire.h>
#include <LiquidCrystal.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

// LCD Setup
LiquidCrystal lcd(2, 3, 4, 5, 6, 7); // Initialize the LCD library with the numbers of the interface pins

// ADXL345 Setup
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(0); // Define the sensor object

// Game Variables
int paddlecol = 7;   // Initial position of the paddle (starting in the middle of row 1)
int paddlerow = 1;   // Row where paddle is displayed
float ballX = 8.0;   // Ball X position (start in the middle horizontally)
float ballY = 0.0;   // Ball Y position (start just above the paddle)
float ballSpeed = 0.01;  // Ball speed (adjust for desired movement speed)
float ballDirectionX = 1.0;  // Ball X direction (1 = right, -1 = left)
float ballDirectionY = 1.0;  // Ball Y direction (1 = down, -1 = up)

void setup() {
  // Initialize Serial for debugging
  Serial.begin(9600);

  // Initialize LCD
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("Ping Pong Game");
  delay(1000);
  lcd.clear();

  // Initialize ADXL345
  if (!accel.begin()) {
    lcd.clear();
    lcd.print("ADXL345 not found!");
    while (1);
  }
  accel.setRange(ADXL345_RANGE_4_G);

  // Draw initial paddle (three underscores)
  lcd.setCursor(paddlecol - 1, paddlerow); // Move cursor to the left of the paddle start position
  lcd.print("___"); // Display three underscores for the paddle
  delay(50);
}

void loop() {
  // Read accelerometer data
  sensors_event_t event;
  accel.getEvent(&event);

  // Update paddle position based on accelerometer data
  int mapX = map(event.acceleration.x, -7, 7, -4, 4); // Adjust sensitivity for horizontal movement
  if(mapX>=0 && mapX < 1 || mapX <= 0 && mapX > -1){
    lcd.setCursor(paddlecol, paddlerow);
  }
  if(mapX > 1 && mapX <= 4){
    for(int i=0; i<=8; i++){
  lcd.setCursor(paddlecol + i, paddlerow);
  lcd.print("___");
  lcd.setCursor(paddlecol-i, paddlerow);
  lcd.print(" ");
  }
  }
  paddlecol += accelX;

  // Limit paddle position to stay within LCD width
  if (paddlecol < 1) {
    paddlecol = 1;
  } else if (paddlecol > 14) {
    paddlecol = 14;
  }

  // Clear previous paddle position
  lcd.setCursor(paddlecol - 2, paddlerow); // Move cursor to the previous paddle position
  lcd.print("   "); // Clear the three columns space

  // Draw paddle at new position
  lcd.setCursor(paddlecol - 1, paddlerow);
  lcd.print("___"); // Display three underscores for the paddle

  // Clear previous ball position
  lcd.setCursor(int(ballX), int(ballY));
  lcd.print(" "); // Clear the ball area

  // Move the ball
  ballX += ballDirectionX * ballSpeed;
  ballY += ballDirectionY * ballSpeed;

  // Check ball collision with walls
  if (ballX <= 0 || ballX >= 15) {
    ballDirectionX = -ballDirectionX; // Reverse X direction on side wall collision
  }
  if (ballY <= 0) {
    ballDirectionY = -ballDirectionY; // Reverse Y direction on ceiling collision
  }

  // Check ball collision with paddle
  if (ballY >= 0 && ballY <= 1 && ballX >= paddlecol - 1 && ballX <= paddlecol + 1) {
    // Calculate angle of ball bounce based on paddle position
    float relativeIntersectX = paddlecol - ballX;
    float normalizedIntersectX = relativeIntersectX / 1.5; // Adjusted for three columns width
    float bounceAngle = normalizedIntersectX * (PI / 3.0); // Maximum bounce angle PI/3 (60 degrees)

    // Update ball direction based on bounce angle
    ballDirectionX = cos(bounceAngle);
    ballDirectionY = -sin(bounceAngle);
  }

  // Check game over condition (ball crosses the lower boundary)
  if (ballY >= 2) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Game Over!"); // Display game over message
    delay(2000);
    lcd.clear();
    ballX = 8.0;  // Reset ball position
    ballY = 0.0;
    ballDirectionY = 1.0; // Reset ball direction
  }
}

 
