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
#include <errno.h>

#define VERSION "1.1.1"
#define _POSIX_C_SOURCE 200809L

/* function declarations  */

static inline void bat(const char *bdir);
static inline void read_ac(const char *acdir);
static int batscan(const struct dirent *bat);
static int thermscan(const struct dirent *dir);
static inline void read_thermal(const char *tdir);

/* Directories for battery & ac, thermal info respectively. */
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

/** scans adir for batteries using scandir, iterates through the found batteries
 *  (directories containing BAT or bat in their name). */
void
bat(const char *adir)
{
    int i;
    struct dirent **batteries;
    int c;
    c = scandir(adir, &batteries, batscan, alphasort);
    
    if(c<=0){
      perror("No batteries found");
      return;
    }
  
    for(i=0; i<c; i++){
            /* Read Capacity */
            char *capn = 0;
            int capf = 0;

            asprintf(&capn, "%s/%s/capacity", adir, batteries[i]->d_name);
            if(capn)
              capf = open(capn , O_RDONLY);
            free(capn);

            ssize_t capsize = 0;
            char capbuf[10];
            if(capf > 0)
              capsize = read(capf, &capbuf, 9);
            close(capf);
            
            char capacity[capsize];
            snprintf(capacity, capsize, "%s", capbuf);
            
            /* Read status */
            char *stn = 0;
            int st = 0;
            char stbuf[30];
            ssize_t stsize = 0;

            asprintf(&stn, "%s/%s/status", adir, batteries[i]->d_name);
          
            if(stn){
          st = open(stn, O_RDONLY);
              free(stn);
            }

            if(st>0){
              stsize = read(st, &stbuf, 30);
              close(st);
            }
            
            char state[stsize+1];
            snprintf(state, stsize, "%s", stbuf);

            printf("%s: %s%%, %s\n", batteries[i]->d_name, capsize ? capacity : "0", stsize ? state : "");
            free(batteries[i]);
    }
    free(batteries);
}

/*  AC adapter info,
 *  looks for acdir/AC/ and reads acdir/AC/online  */
void
read_ac(const char *acdir)
{
    char *acn = 0;
    int acf = 0;
    
    struct dirent *entry;
    DIR *d = opendir(acdir);
    errno = 0;

    if(!d){
      perror("AC not found");
      return;
    }

  	while((entry = readdir(d)) != NULL){
    		if(errno == EBADF)
        		perror("AC not found");

    		if(!strncmp(entry->d_name, "AC", 2))
        		asprintf(&acn, "%s/%s/online", acdir, "AC");
  	}
	
 	  closedir(d);
	
  	if(!acn){
		fputs("No AC adapter found\n", stderr);
    		return;
	}
	
    acf = open(acn , O_RDONLY);
  	free(acn);
	
	
	  char buf[2] = {0};
    read(acf, &buf, 1);
	
	  short online = atoi(buf);
	
	  printf("Adapter: %s\n", online ? "on-line" : "off-line");
  	close(acf);
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

/**  Thermals  **/
void
read_thermal(const char *tdir)
{
    struct dirent **thermals;
    int d = 0;
    int i;
    ssize_t ts = 0;
    char thermbuf[6];
    int therm;
    int tf = 0;
    
    d = scandir(tdir, &thermals, thermscan, alphasort);

    if(!d){
        fputs("No thermals found\n", stderr);
        return;
    }

    for(i=0; i<d; i++){
            char *zn;
            asprintf(&zn, "%s/%s/temp", tdir, thermals[i]->d_name);
            if(zn)
              tf = open(zn, O_RDONLY);
            free(zn);	

            if(tf>0)
              ts = read(tf, &thermbuf, 5);
            close(tf);
            therm = atoi(thermbuf);
            therm /= 1000;

            if(!ts)
              fprintf(stderr, "Thermal %d: Not Found\n", i);
            printf("Thermal %d: %d C\n", i, therm);

            free(thermals[i]);
    }
    free(thermals);
}

static void
help(){
  puts("Usage: sacpi [option]\noptions are: \n\
-b, --battery      for battery info \n\
-a, --ac           for AC adapter info\n\
-t, --thermal      for thermal info\n\
-A, --all          prints all the options\n\
-h, --help         prints this help\n\
-v, --version	   displays the version and license information");
}

static void
version(){
  printf("sacpi %s\n", VERSION);
  puts("a simple tool to display battery, AC, and thermal info.\n\
Copyright (C) 2025 Topias Silfverhuth\n\
License GPLv2+: GNU GPL version 2 or later <https://www.gnu.org/licenses/old-licenses/gpl-2.0.html>\n\
This is free software: you are free to change and redistribute it.\n\
There is NO WARRANTY, to the extent permitted by law.\n");
}


int
main(int argc, char *argv[]){

    int c;
    unsigned short bflag = 0;
    unsigned short acflag = 0;
    unsigned short tflag = 0;

    static struct option longopts[] =
    {
      {"version", no_argument, 0, 'v'},
      {"help", no_argument, 0, 'h'},
      {"battery", no_argument, 0, 'b'},
      {"thermal", no_argument, 0, 't'},
      {"ac", no_argument, 0, 'a'},
      {"all", no_argument, 0, 'A'}
    };
    int optindex = 0;
  
    while((c=getopt_long(argc, argv, "Aabhtv", longopts, &optindex)) != -1)
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
      case 'A':
        bflag=1;
        acflag=1;
        tflag=1;
        break;
      case 'h':
        help();
        break;
      case 'v':
        version();
        return 0;
        break;
      case '?':
        help();
        break;
      }

 
  
    if(bflag)
      bat(adir);
    if(acflag)
      read_ac(adir);
    if(tflag)
      read_thermal(tdir);
    if(argc==1)
      bat(adir);
    
    return EXIT_SUCCESS;
}
