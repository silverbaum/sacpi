/*
 * sacpi - simple tool to display battery and acpi info
 * Copyright (C) 2025 Topias Silfverhuth

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




static void bat();
static void read_ac();





void
bat(){
  int cap = open("/sys/class/power_supply/BAT0/capacity", O_RDONLY);
  int st = open("/sys/class/power_supply/BAT0/status", O_RDONLY);
  
  
  char capacity[5];
  read(cap, &capacity, 4);

  char state[13];
  read(st, &state, 12);
  
  int percent = atoi(capacity);
  
  printf("BAT0: %s, %d%\n", state, percent);
}

/* Adapter info */
void
read_ac(){
  int ac = open("/sys/class/power_supply/AC/online", O_RDONLY);
  char buf[3];
  read(ac, &buf, 2);
  unsigned int online = atoi(buf);
  printf("Adapter: %s\n", online ? "on-line" : "off-line");
}

void
help(){
  puts("options are: -b, -a");
}


int
main(int argc, char *argv[]){
  int c;
  unsigned short bflag, acflag = 0;
  
  while((c=getopt(argc, argv, "abh")) != -1)
    switch(c){
    case 'b':
      bflag=1;
    case 'a':
      acflag=1;
    case 'h':
      help();
    default:
      
    }

  if(acflag){
      read_ac();
  }if(bflag){
      bat();
  }
}
