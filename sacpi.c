/*
 * sacpi - simple tool to display battery and acpi info
 * Copyright (C) 2025 Topias Silfverhuth
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.  
 */


#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>

/* function declarations  */
static void bat(const char *bdir);
static void read_ac();
static int batscan(const struct dirent *bat);
static void read_thermal(const char *tdir);


static const char *adir = "/sys/class/power_supply";
static const char *tdir = "/sys/class/thermal";

/* Scans for all batteries in bdir(for scandir)  */
int
batscan(const struct dirent *bat)
{
  if(strstr(bat->d_name, "BAT"))
    return 1;
  else if(strstr(bat->d_name, "bat"))
    return 1;
  else
    return 0;
}

/* Displays battery information */
void
bat(const char *bdir)
{
  int i;
  struct dirent **batteries;
  int c;
  c = scandir(bdir, &batteries, batscan, alphasort);  

 
  for(i=0; i<c; i++){
    /** Read Capacity **/
    char capn[266];
    snprintf(capn, 266, "%s/%s/capacity", bdir, batteries[i]->d_name);
    int capf = open(capn , O_RDONLY);
    
    ssize_t capsize = 0;
    char capbuf[10];
    if(capf > 0)
      capsize = read(capf, &capbuf, 9);
    close(capf);
    
    char capacity[capsize];
    snprintf(capacity, capsize, "%s", capbuf);
    
    /** Read status **/
    char stn[265];
    snprintf(stn, 265, "%s/%s/status", bdir, batteries[i]->d_name);
    /* open returns non-zero if successful, -1 if not */
    int st = open(stn, O_RDONLY);
    
    char stbuf[20];
    ssize_t stsize = 0;
    if(st>0)
      stsize = read(st, &stbuf, 20);
    close(st);
    
    char state[stsize];
    snprintf(state, stsize, "%s", stbuf);

    printf("%s: %s%%, %s\n", batteries[i]->d_name, capsize ? capacity : "0", stsize ? state : "");
}
}

/*  AC adapter info  */
void
read_ac(){
  int ac = open("/sys/class/power_supply/AC/online", O_RDONLY);
  char buf[2];
  if(ac) read(ac, &buf, 1);
  close(ac);
  short online = atoi(buf);
  
  printf("Adapter: %s\n", online ? "on-line" : "off-line");
}

/* scandir filter function  */
int
thermscan(const struct dirent *dir)
{
  if(strstr(dir->d_name, "thermal_zone"))
     return 1;
  else
     return 0;
}

/*  Thermals  */
void
read_thermal(const char *tdir)
{
  struct dirent **thermals;
  int d;
  int i;
  ssize_t ts = 0;
  char thermbuf[5];
  int therm;
  
  d = scandir(tdir, &thermals, thermscan, alphasort);

  for(i=0; i<d; i++){
    char zn[265];
    snprintf(zn, 265, "%s/%s/temp", tdir, thermals[i]->d_name);
    int tf = open(zn, O_RDONLY);

    if(tf>0)
      ts = read(tf, &thermbuf, 5);
    close(tf);
    therm = atoi(thermbuf);
    therm /= 1000;

    if(!ts)
      fprintf(stderr, "Thermal %d: Not Found\n", i);
    printf("Thermal %d: %d C\n", i, therm);
  }
}

void
help(){
  puts("Usage: sacpi [option]\noptions are: -b for battery info, -a for AC adapter info");
}


int
main(int argc, char *argv[]){

  int c;
  unsigned short bflag = 0;
  unsigned short acflag = 0;
  unsigned short tflag = 0;
  
  while((c=getopt(argc, argv, "abht")) != -1)
    switch(c){
    case 'b':
      bflag=1;
      break;
    case 'a':
      acflag=1;
      break;
    case 't':
      tflag=1;
      break;
    case 'h':
      help();
      break;
    }
 
  if(acflag)
    read_ac();
  if(bflag)
    bat(adir);
  if(tflag)
    read_thermal(tdir);
  if(argc==1)
    bat(adir);
  
  return EXIT_SUCCESS;
}
