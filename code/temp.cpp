/* rfwakes
 * g++ rfwakes.c rfm69bios.o -o rfwakes -lwiringPi
 Copyright (C) 2020  Ray, Chang

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <vector>


#ifndef RFM69BIOS_H
#include "rfm69bios.h"
#endif

struct Target {
	std::string remrfid;
};

void readConfig(char const *fileName, char clist[2][30])
{
	FILE *file = fopen(fileName, "r");
	int c;

	if (file == NULL) {
		fprintf(stderr, "Config file cannot find in /home/pi/myConfig.");
		fprintf(stderr, "Usage: rfwake [calledRFID] opt.[localRFID] [GPIO]\n");
		exit(EXIT_FAILURE);
	} //could not open file

	char item[30] = {};

	size_t n = 0;
	while ((c = fgetc(file)) != EOF)
	{
		if (c == ' ') {
			item[n] = '\0';
			strcpy(clist[0], item);
			memset(&item[0], 0, sizeof(item));
			n = 0;
		}
		else
			item[n++] = (char) c;
	}

	// terminate with the null character
	item[n] = '\0';
	strcpy(clist[1], item); 
	fclose(file);
}

std::vector <Target> readTargets(){
	std::ifstream infile("/home/pi/targetlist");
	std::string temp;
	std::vector <Target> tlist;
	while (infile >> temp) {
		Target t;
		t.remrfid = temp;
		tlist.push_back(t);
	}
	//std::cout << tlist[0].remrfid;
	for(auto it = tlist.begin(); it != tlist.end(); it++) {
		std::cout << it->remrfid;
		fprintf(stdout, "Wake Remote RFID:%s\n", it->remrfid.c_str());
	}
}

int main(int argc, char* argv[]) {
	int fd, gpio, i, mode, res, nbr=1, gotyou=0;
	char *ap;
	unsigned char locrfid[IDSIZE], remrfid2[IDSIZE], recrfid[IDSIZE];
	std::vector <Target> Targetlist;

	// *** Config ***
	char config[2][30];

	if (argc == 1) {	// case: rfwakes
		readConfig("/home/pi/myConfig", config); 
		Targetlist = readTargets();
	} else {  		// other case
		fprintf(stderr, "Usage: rfwakes\n");
		exit(EXIT_FAILURE);
	}

	for (i = 0, ap = config[0]; i < IDSIZE; i++, ap++) {
		locrfid[i] = strtoul(ap,&ap,16);
	}
	gpio = atoi(config[1]);
	
	fprintf(stdout, "Local RFID:%s, GPIO:%d\n",config[0] , gpio); 
	fprintf(stdout, "Wake Remote RFID:%s\n", Targetlist[0]);
	
	//for(auto it = Targetlist.begin(); it != Targetlist.end(); it++) {
	//	fprintf(stdout, "Wake Remote RFID:%s\n", *it->remrfid);
	//}
/*
	// *** Setup ***
	if (wiringPiSetupSys() < 0) {
		fprintf(stderr, "Failed to setup wiringPi\n");
		exit(EXIT_FAILURE);
	}
	fd = wiringPiSPISetup(SPI_DEVICE, SPI_SPEED);
	if (fd < 0) {
		fprintf(stderr, "Failed to open SPI device\n");
		exit(EXIT_FAILURE);
	}

	for (;;) {
		// *** Transmission ***
		// prepare for TX
		if (rfm69startTxMode(remrfid)) {
			fprintf(stderr, "Failed to enter TX Mode\n");
			exit(EXIT_FAILURE);
		}
		// write Tx data
		rfm69txdata(&remrfid[IDSIZE-1],1);
		rfm69txdata(locrfid,IDSIZE);
		// wait for HW interrupt(s) and check for TX_Sent state, takes approx. 853.3�s
		do {
			if(waitForInterrupt(gpio, 1) <= 0) { // wait for GPIO_xx
				fprintf(stderr, "Failed to wait on TX interrupt\n");
				exit(EXIT_FAILURE);
			}
			mode = rfm69getState();
			if (mode < 0) {
				fprintf(stderr, "Failed to read RFM69 Status\n");
				exit(EXIT_FAILURE);
			}
		} while ((mode & 0x08) == 0);
		fprintf(stdout, "%d. Wake-Telegram sent.\n", nbr++);

		// switch back to STDBY Mode
		if (rfm69STDBYMode()) {
			fprintf(stderr, "Failed to enter STDBY Mode\n");
			exit(EXIT_FAILURE);
		}

		// *** Reception ***
		// prepare for RX
		if (rfm69startRxMode(locrfid)) {
			fprintf(stderr, "Failed to enter RX Mode\n");
			exit(EXIT_FAILURE);
		}
		// wait for HW interrupt(s) and check for CRC_Ok state
		res = waitForInterrupt(gpio, 84); // wait for GPIO_xx
		if (res < 0) {
			fprintf(stderr, "Failed to wait on RX interrupt\n");
			exit(EXIT_FAILURE);
		}
		else if (res > 0) { // in case of reception ...
			mode = rfm69getState();
			if (mode < 0) {
				fprintf(stderr, "Failed to read RFM69 Status\n");
				exit(EXIT_FAILURE);
			}
			if ((mode & 0x02) == 0x02) { // ... and CrcOk ...
				// read remote RF ID from FIFO
				rfm69rxdata(recrfid, 1); // skip last byte of called RF ID
				rfm69rxdata(recrfid, IDSIZE); // read complete remote RF ID
				// check received vs. called remote RF ID
				for (i = 0, gotyou = 1; i < IDSIZE; i++) // ... and RF ID equal ...
					if (remrfid[i] != recrfid[i]) gotyou = 0; // ... then done
				if (!gotyou) delay(85); // wait long enough if wrong RF ID received
			}
		}
		// switch back to STDBY Mode
		if (rfm69STDBYMode()) {
			fprintf(stderr, "Failed to enter STDBY Mode\n");
			exit(EXIT_FAILURE);
		}
		if (gotyou)
			break;
	} 

	// output of remote RF ID
	fprintf(stdout, "ACK received from called Station RF ID ");
	for (i = 0; i < IDSIZE; i++) {
		if(i != 0) fprintf(stdout,":");
		fprintf(stdout, "%02x", recrfid[i]);
	}
	fprintf(stdout,"\n");

	close(fd);
*/
	exit(EXIT_SUCCESS);
}