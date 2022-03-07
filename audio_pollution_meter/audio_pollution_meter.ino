
#include <SD.h>
#include <SPI.h>

File dataFile;

const int chipSelect = 4;
const int ANALOGPIN = 19;
bool status;
int audio;

void setup() {

  Serial.begin(9600);
  while (!Serial);
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

}

void loop() {


  Serial.printf("About to collect audio pullution \n");   //test print to screen
  delay(2000);

  audio = analogRead(ANALOGPIN);                    //interger for audio
  if (audio >= 795) {
    Serial.printf(" %i\n", audio);
    writeToSD(audio);                             //pulls up void writeToSD when above a threshold
  }
  
      delay(2000);
      readFromSD();
  }

void writeToSD(int print_audio) {
  dataFile = SD.open("AudioPollution.csv", FILE_WRITE);   //writes int audio to SD

  if (dataFile) {
    dataFile.printf("%i\n", print_audio);
    dataFile.close();
    Serial.printf("%i \n", print_audio);
  }
  else {
    Serial.printf("error opening Audio_Pollution.csv \n");          // if the file is available, write to it:
  }
  return;
}

void readFromSD() {


  dataFile = SD.open("Audio_Pollution.csv");
  if (dataFile) {
    Serial.printf("Audio_Pollution.csv: \n");                       // re-open the file for reading:

    // read from the file until there's nothing else in it:
    while (dataFile.available()) {
      Serial.write(dataFile.read());
    }
    dataFile.close();
  } else {
    Serial.printf("error opening Audio_Pollution.csv \n");
  }

  return;
}
