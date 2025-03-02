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

/* function declarations */
int asprintf(char **, const char*, ...);
int alphasort();
int scandir();
char* strcasestr();

static void bat();
static void read_ac();
static int batscan(const struct dirent *bat);
static void msg(char*, char*, char*);

/* variables */
const char *bdir = "/sys/class/power_supply/";


void
msg(char *bat, char *percent, char *state)
{
  printf("%s: %s%%, %s\n", bat, percent, state);
}

/* Scans for all batteries in power_supply directory */
int
batscan(const struct dirent *bat)
{
  if(strcasestr(bat->d_name, "BAT"))
    return 1;
  else
    return 0;
}

void
bat()
{
  struct dirent **batteries;
  int c;
  c = scandir(bdir, &batteries, batscan, alphasort);  

  int i;
  for(i=0; i<c; i++){
    /** Read Capacity **/
    char *capn;
    asprintf(&capn, "%s%s/capacity", bdir, batteries[i]->d_name);
    int capf = capn ? open(capn , O_RDONLY) : 0;
    free(capn);
    
    ssize_t capsize = 0;
    char capbuf[10];
    if(capf > 0)
      capsize = read(capf, &capbuf, 9);
    close(capf);
    
    char capacity[capsize];
    snprintf(capacity, capsize, "%s", capbuf);
    
    /** Read status **/
    char *stn;
    asprintf(&stn, "%s%s/status", bdir, batteries[i]->d_name);
    /* open returns non-zero if successful, -1 if not */
    int st = open(stn, O_RDONLY);
    free(stn);
    
    char stbuf[20];
    ssize_t bts = 0;
    if(st>0)
      bts = read(st, &stbuf, 20);
    close(st);
    
    
    char state[bts];
    snprintf(state, bts, "%s", stbuf);
    
    msg(batteries[i]->d_name, capsize ? capacity : "0", bts ? state : "");
  }
  return;
}

/* Adapter info */
void
read_ac(){
  int ac = open("/sys/class/power_supply/AC/online", O_RDONLY);
  char buf[2];
  if(ac) read(ac, &buf, 1);
  close(ac);
  short online = atoi(buf);
  printf("Adapter: %s\n", online ? "on-line" : "off-line");
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
  
  while((c=getopt(argc, argv, "abh")) != -1)
    switch(c){
    case 'b':
      bflag=1;
      break;
    case 'a':
      acflag=1;
      break;
    case 'h':
      help();
      break;
    }

  if(acflag)
      read_ac();
  if(bflag)
      bat();
  if(argc==1)
      bat();
}
