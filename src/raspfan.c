#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <getopt.h>
#include <libgen.h>

#define OPTSTR "hvg:t:f:s:"
#define USAGE_FMT  "%s [-v] [-g gpio] [-t temp-on-threshold] [-f temp-off-threshold] [-s sleep-interval] [-h]\n"


typedef struct {
  int           verbose;
  int           gpioPin;                 // Which GPIO pin you're using to control the fan. 
  float         tempOnThreshold;         // (degrees Celsius) Fan kicks on at this temperature.
  float         tempOffThreshold;        // (degress Celsius) Fan shuts off at this temperature.
  int           sleepInterval;           // (seconds) How often we check the core temperature.
} TypeOptions;

TypeOptions options = {0, 0, 55.0, 50.0, 5};

float getTemp(){
    FILE *thermal = fopen("/sys/class/thermal/thermal_zone0/temp","r");
    float millideg;
    int n = fscanf(thermal,"%f",&millideg);
    if (n==0) return -1;
    fclose(thermal);
    return millideg / 1000;
}

char isFanActive=0;
void controlFan(TypeOptions options){
  if (options.verbose > 0){
     printf("Fan control\n");
     printf("Temp ON threshold:  %.2fC\n", options.tempOnThreshold);
     printf("Temp OFF threshold: %.2fC\n", options.tempOffThreshold);
     printf("Sleep interval: %d\n", options.sleepInterval);
  }
  if (wiringPiSetup () == -1) return;
  pinMode (options.gpioPin, OUTPUT);
  for(;;){
    float temp = getTemp();
    if (temp >= options.tempOnThreshold){
       isFanActive = 1;
    } else if (temp < options.tempOffThreshold){
       isFanActive = 0;
    }

    if (options.verbose > 0){
      printf("Current temp: %.2f, On: %.2f, Off: %.2f %s\n", temp, options.tempOnThreshold, options.tempOffThreshold, (isFanActive==1)?"(Active)":"");
    }
    digitalWrite(options.gpioPin, isFanActive);
    delay(options.sleepInterval * 1000);
  }
}

void usage(char *progname, int opt) {
  fprintf(stderr, USAGE_FMT, progname);
  exit(1);
}

int main (int argc, char *argv[]){
  int opt;
  while ((opt = getopt(argc, argv, OPTSTR)) != EOF){
    switch(opt) {
       // GPIO
       case 'g':
           options.gpioPin = atoi(optarg);
           break;
       // Temp On Threshold
       case 't':
           options.tempOnThreshold = atof(optarg);
           break;
       // Temp Off Threshold
       case 'f':
           options.tempOffThreshold = atof(optarg);
           break;
       // Sleep interval
       case 's':
           options.sleepInterval = atoi(optarg);
           break;
       case 'v':
           options.verbose = 1;
           break;
       case 'h':
       default:
           usage(basename(argv[0]), opt);
           break;
    }
  }
  controlFan(options);
  return 0 ;
}
