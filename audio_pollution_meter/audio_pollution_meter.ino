/*
   Audio cancellation
   Thomas Abeyta
   March 7, 2022
*/


#include <TimeLib.h>        //time header
#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define TIME_HEADER  "T"   // Header tag for serial time sync message

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET  4// reset pin #
#define SCREEN_ADDRESS 0x3C /// See datasheet for Address; for 128x64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

File dataFile;

const int chipSelect = 4;
const int ANALOGPIN = 19;
bool status;
int audio;
int dbLevel;

void setup() {

  Serial.begin(9600);
  while (!Serial);  // Wait for Arduino Serial Monitor to open
  delay(100);

  setSyncProvider(getTeensy3Time);                 // set the Time library to use Teensy 3.0's RTC to keep time

  if (timeStatus() != timeSet) {
    Serial.println("Unable to sync with the RTC");
  }
  else {
    Serial.println("RTC has set the system time");  //initializing time
  }
  Serial.printf("Initializing SD card...");         //monitor feedback/initializing SD

  pinMode(chipSelect, OUTPUT);                      //sets pin ready to write to SD
  pinMode(ANALOGPIN, INPUT);                        //gets pin ready to read MIC
  digitalWrite(chipSelect, HIGH);                   //sets the pin to high as on

  status = SD.begin(chipSelect);
  if (!status) {  // if status is false
    Serial.printf("Card failed, or not present\n");
    while (true); // pause the code indefinately
  }
  else {
    Serial.printf("card initialized.\n");           //checks the SD status and initialize
  }
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
  }


  display.display();
  delay(4000); //pause for 4 seconds

  //clear the buffer
  display.clearDisplay();
  display.setRotation(2);

}

void loop() {



  audio = analogRead(ANALOGPIN);                    //interger for audio
  if(audio >=900){
  if (Serial.available()) {
    time_t t = processSyncMessage();
    if (t != 0) {
      Teensy3Clock.set(t); // set the RTC
      setTime(t);
    }
  }
  
    digitalClockDisplay();
    text();
    Serial.printf(" above 45db: %i\n", audio);
    writeToSD(audio);                             //pulls up void writeToSD when above a threshold
}
}

void text(void) {

  dbLevel = (audio / 20);
  display.clearDisplay();
  display.setTextSize(1);  //draws 2x scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 3);
  display.printf("DB level: %i", dbLevel);
  display.display(); //shows the initial text

}


void writeToSD(int print_audio) {
  dataFile = SD.open("loud.csv", FILE_WRITE);   //writes int audio to SD

  if (dataFile) {
    dataFile.printf("%i\n", print_audio);
    dataFile.close();
    Serial.printf("write to sd: %i \n", print_audio);
  }
  else {
    Serial.printf("loud.csv \n");          // if the file is available, write to it:
  }
  return;
}

void readFromSD() {


  dataFile = SD.open("loud.csv");
  if (dataFile) {
    Serial.printf("loud.csv: \n");                       // re-open the file for reading:

    // read from the file until there's nothing else in it:
    while (dataFile.available()) {
      Serial.write(dataFile.read());
    }
    dataFile.close();
  } else {
    Serial.printf("error opening audioPol.csvm \n");
  }

  return;
}

void digitalClockDisplay() {
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year());
  Serial.println();
}

time_t getTeensy3Time() {
  return Teensy3Clock.get();
}

/*  code to process time sync messages from the serial port   */


unsigned long processSyncMessage() {
  unsigned long pctime = 0L;
  const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013

  if (Serial.find(TIME_HEADER)) {
    pctime = Serial.parseInt();
    return pctime;
    if ( pctime < DEFAULT_TIME) { // check the value is a valid time (greater than Jan 1 2013)
      pctime = 0L; // return 0 to indicate that the time is not valid
    }
  }
  return pctime;
}

void printDigits(int digits) {
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}
