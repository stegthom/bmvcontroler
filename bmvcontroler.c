#include <getopt.h>
#include <stdio.h>
#include <stdint.h>
#ifndef _MSC_VER
#include <unistd.h>
#endif
#include <stdlib.h>
#include <string>
#include <fcntl.h>  /* File Control Definitions          */
#include <termios.h>/* POSIX Terminal Control Definitions*/
#include <errno.h>  /* ERROR Number Definitions          */
#include <sys/ioctl.h>
#include <linux/serial.h>
#include <sys/types.h>
#include <cstring>

bool bmv_connected = false;
struct termios oldSettings;
struct termios Settings;

int connectbmv(std::string bmvdev);
void disconnectbmv(int ctx);
int writebmv(int bmvctx, char *buffer, int lenght);
int readbmv (int bmvctx, uint8_t *buffer, int lenght);
int main( int argc, char* argv[] )
{

std::string tracerdev;
   std::string bmvdev;
   std::string get;
   bool displayhelp = false;
   int bmvctx;
   
   char GetSoc[] =        ":7FF0F0040\n";
   char GetVoltage[] =    ":78DED00D4\n";
   char GetCurrent[] =    ":78FED00D2\n"; //Unit 0,1A
   //char GetCurrent[] =    ":78CED00D5\n";  //Unit 0,001A
   char GetPower[] =      ":78EED00D3\n";
   char GetConsumedAh[] = ":7FFEE0061\n";
   char GetTtg[] =        ":7FE0F0041\n";
   char GetA[] =          ":70003004B\n"; //Depth of the deepest discharge
   char GetB[] =          ":70103004A\n"; //Depth of the last discharge
   char GetC[] =          ":702030049\n"; //Depth of the average discharge
   char GetD[] =          ":703030048\n"; //Number of cycles
   char GetE[] =          ":704030047\n"; //Number of full discharges
   char GetF[] =          ":705030046\n"; //Cumulative Amp Hours
   char GetG[] =          ":706030045\n"; //Minimum Voltage
   char GetH[] =          ":707030044\n"; //Maximum Voltage
   char GetI[] =          ":708030043\n"; //Seconds since full charge
   char GetJ[] =          ":709030042\n"; //Number of automatic synchronizations
   char GetL[] =          ":70A030041\n"; //Number of Low Voltage Alarms
   char GetM[] =          ":70B030040\n"; //Number of High Voltage Alarms
   char GetR[] =          ":71003003B\n"; //Amount of discharged energy
   char GetS[] =          ":71103003A\n"; //Amount of charged energy
   
static struct option long_options[] = {
      { "device", 		required_argument, NULL, 'd' },
	  { "get",     	required_argument, NULL, 'g' },
      { "help",       		no_argument,       NULL, 'h' },
      { NULL }
      };

int c;
while ((c = getopt_long(argc, argv, "d:g:h", long_options, NULL)) != -1)
     {
	switch (c)
	  {
	   case 'd': bmvdev = optarg;
	             break;
	   case 'g': get = optarg;
	             break;
	   case 'h': displayhelp = true; break;
	   default: exit(0);
	  }
     }
	 
	 if (displayhelp)
	  {
	     printf("Usage: bmvcontroler [OPTIONS]\n\n"
		    "-d PATH,     --device=PATH          Device Path of the victron bmv Monitor\n"
			"-g Get Command, --get=Value to Get	 Value Could be one of these:\n"
			"	soc  Get State of Charge (0,01%)\n"
			"	voltage  Get Battery Voltage (0,01V)\n"
			"	current  Get Battery current (0,1A)\n"
			"	power  Get Battery Power (W)\n"
			"	consumedah  Get Consumed AH (0,1AH)\n"
			"	ttg  Get Time to Go (min)\n"
			"	a  Get Depth of the deepest discharge (0,1AH)\n"
			"	b  Get Depth of the last discharge (0,1AH)\n"
			"	c  Get Depth of the average discharge(0,1AH)\n"
			"	d  Get Number of cycles\n"
			"	e  Get Number of full discharges\n"
			"	f  Get Cumulative Amp Hours (0,1AH)\n"
			"	g  Get Minimum Voltage (0,01V)\n"
			"	h  Get Maximum Voltage (0,01V)\n"
			"	i  Get Seconds since full charge (sec)\n"
			"	j  Get Number of automatic synchronizations\n"
			"	l  Get Number of Low Voltage Alarms\n"
			"	m  Get Number of High Voltage Alarms\n"
			"	r  Get Amount of discharged energy(0,01KWh)\n"
			"	s  Get Amount of charged energy (0,01KWh)\n"
		    "-h,  --help  Show Usage\n"
		    "\n");
			exit(0);
	  } 
	  if (!bmv_connected)
	  {
		  bmvctx = connectbmv(bmvdev);
		  if (bmvctx == -1)
		  {
			  printf("bmvcontroler: Error Connect to BMV\n");
			  exit(-1);
		  }
	  }
	  
	  if (get.compare("soc") == 0)
	  {
		if (writebmv(bmvctx, GetSoc, 11) != 1)
		{
			printf("Error write to bmv\n");
			exit(-1);
		}
		uint8_t buffer[100];
		int buffersize=0;
		buffersize = readbmv(bmvctx, buffer, 100);
		if (buffersize <= 0)
		{
			printf("error Read BMV\n");
			exit(-1);
		}
		if (buffer[0] == 0x07 && buffer[1] == 0xFF && buffer[2] == 0x0F && buffer[3] == 0x00)
		{
			uint16_t soc;
			//(tab_int8[(index)] << 8) | tab_int8[(index) + 1]
			soc = buffer[4] | (buffer[5] << 8);
			printf("%d\n", soc);
			exit(1);
		}
		else
		{
			printf("Error in BMV Response\n");
			exit(-1);
		}
		
	  }
	  
	  else if (get.compare("voltage") == 0)
	  {
		  if (writebmv(bmvctx, GetVoltage, 11) != 1)
		{
			printf("Error write to bmv\n");
			exit(-1);
		}
		uint8_t buffer[100];
		int buffersize=0;
		buffersize = readbmv(bmvctx, buffer, 100);
		if (buffersize <= 0)
		{
			printf("error Read BMV\n");
			exit(-1);
		}
		if (buffer[0] == 0x07 && buffer[1] == 0x8D && buffer[2] == 0xED && buffer[3] == 0x00)
		{
			int16_t voltage;
			//(tab_int8[(index)] << 8) | tab_int8[(index) + 1]
			voltage = buffer[4] | (buffer[5] << 8);
			printf("%d\n", voltage);
			exit(1);
		}
		else
		{
			printf("Error in BMV Response\n");
			exit(-1);
		}
	  }
	  
	  else if (get.compare("current") == 0)
	  {
		 if (writebmv(bmvctx, GetCurrent, 11) != 1)
		{
			printf("Error write to bmv\n");
			exit(-1);
		}
		uint8_t buffer[100];
		int buffersize=0;
		buffersize = readbmv(bmvctx, buffer, 100);
		if (buffersize <= 0)
		{
			printf("error Read BMV\n");
			exit(-1);
		}
		if (buffer[0] == 0x07 && buffer[1] == 0x8F && buffer[2] == 0xED && buffer[3] == 0x00)
		{
			int16_t current;
			//(tab_int8[(index)] << 8) | tab_int8[(index) + 1]
			current = buffer[4] | (buffer[5] << 8);
			printf("%d\n", current);
			exit(1);
		}
		else
		{
			printf("Error in BMV Response\n");
			exit(-1);
		} 
	  }
	  
	  else if (get.compare("power") == 0)
	  {
		 if (writebmv(bmvctx, GetPower, 11) != 1)
		{
			printf("Error write to bmv\n");
			exit(-1);
		}
		uint8_t buffer[100];
		int buffersize=0;
		buffersize = readbmv(bmvctx, buffer, 100);
		if (buffersize <= 0)
		{
			printf("error Read BMV\n");
			exit(-1);
		}
		if (buffer[0] == 0x07 && buffer[1] == 0x8E && buffer[2] == 0xED && buffer[3] == 0x00)
		{
			int16_t power;
			power = buffer[4] | (buffer[5] << 8);
			printf("%d\n", power);
			exit(1);
		}
		else
		{
			printf("Error in BMV Response\n");
			exit(-1);
		}  
	  }
	  
	  else if (get.compare("consumedah") == 0)
	  {
		 if (writebmv(bmvctx, GetConsumedAh, 11) != 1)
		{
			printf("Error write to bmv\n");
			exit(-1);
		}
		uint8_t buffer[100];
		int buffersize=0;
		buffersize = readbmv(bmvctx, buffer, 100);
		if (buffersize <= 0)
		{
			printf("error Read BMV\n");
			exit(-1);
		}
		if (buffer[0] == 0x07 && buffer[1] == 0xFF && buffer[2] == 0xEE && buffer[3] == 0x00)
		{
			int32_t consumedah;
			consumedah = buffer[4] | (buffer[5] << 8) | (buffer[6] << 16) | (buffer[7] << 24);
			printf("%d\n", consumedah);
			exit(1);
		}
		else
		{
			printf("Error in BMV Response\n");
			exit(-1);
		}  
	  }
	  
	  else if (get.compare("ttg") == 0)
	  {
		  if (writebmv(bmvctx, GetTtg, 11) != 1)
		{
			printf("Error write to bmv\n");
			exit(-1);
		}
		uint8_t buffer[100];
		int buffersize=0;
		buffersize = readbmv(bmvctx, buffer, 100);
		if (buffersize <= 0)
		{
			printf("error Read BMV\n");
			exit(-1);
		}
		if (buffer[0] == 0x07 && buffer[1] == 0xFE && buffer[2] == 0x0F && buffer[3] == 0x00)
		{
			uint16_t ttg;
			ttg = buffer[4] | (buffer[5] << 8);
			printf("%d\n", ttg);
			exit(1);
		}
		else
		{
			printf("Error in BMV Response\n");
			exit(-1);
		}  
	  }
	  
	  else if (get.compare("a") == 0)
	  {
		   if (writebmv(bmvctx, GetA, 11) != 1)
		{
			printf("Error write to bmv\n");
			exit(-1);
		}
		uint8_t buffer[100];
		int buffersize=0;
		buffersize = readbmv(bmvctx, buffer, 100);
		if (buffersize <= 0)
		{
			printf("error Read BMV\n");
			exit(-1);
		}
		if (buffer[0] == 0x07 && buffer[1] == 0x00 && buffer[2] == 0x03 && buffer[3] == 0x00)
		{
			int32_t a;
			a = buffer[4] | (buffer[5] << 8) | (buffer[6] << 16) | (buffer[7] << 24);
			printf("%d\n", a);
			exit(1);
		}
		else
		{
			printf("Error in BMV Response\n");
			exit(-1);
		}
	  }
	  
	  else if (get.compare("b") == 0)
	  {
		     if (writebmv(bmvctx, GetB, 11) != 1)
		{
			printf("Error write to bmv\n");
			exit(-1);
		}
		uint8_t buffer[100];
		int buffersize=0;
		buffersize = readbmv(bmvctx, buffer, 100);
		if (buffersize <= 0)
		{
			printf("error Read BMV\n");
			exit(-1);
		}
		if (buffer[0] == 0x07 && buffer[1] == 0x01 && buffer[2] == 0x03 && buffer[3] == 0x00)
		{
			int32_t b;
			b = buffer[4] | (buffer[5] << 8) | (buffer[6] << 16) | (buffer[7] << 24);
			printf("%d\n", b);
			exit(1);
		}
		else
		{
			printf("Error in BMV Response\n");
			exit(-1);
		}
	  }
	  
	  else if (get.compare("c") == 0)
	  {
		     if (writebmv(bmvctx, GetC, 11) != 1)
		{
			printf("Error write to bmv\n");
			exit(-1);
		}
		uint8_t buffer[100];
		int buffersize=0;
		buffersize = readbmv(bmvctx, buffer, 100);
		if (buffersize <= 0)
		{
			printf("error Read BMV\n");
			exit(-1);
		}
		if (buffer[0] == 0x07 && buffer[1] == 0x02 && buffer[2] == 0x03 && buffer[3] == 0x00)
		{
			int32_t c;
			c = buffer[4] | (buffer[5] << 8) | (buffer[6] << 16) | (buffer[7] << 24);
			printf("%d\n", c);
			exit(1);
		}
		else
		{
			printf("Error in BMV Response\n");
			exit(-1);
		}  
	  }
	  
	  else if (get.compare("d") == 0)
	  {
		      if (writebmv(bmvctx, GetD, 11) != 1)
		{
			printf("Error write to bmv\n");
			exit(-1);
		}
		uint8_t buffer[100];
		int buffersize=0;
		buffersize = readbmv(bmvctx, buffer, 100);
		if (buffersize <= 0)
		{
			printf("error Read BMV\n");
			exit(-1);
		}
		if (buffer[0] == 0x07 && buffer[1] == 0x03 && buffer[2] == 0x03 && buffer[3] == 0x00)
		{
			uint32_t d;
			d = buffer[4] | (buffer[5] << 8) | (buffer[6] << 16) | (buffer[7] << 24);
			printf("%d\n", d);
			exit(1);
		}
		else
		{
			printf("Error in BMV Response\n");
			exit(-1);
		}
	  }
	  
	  else if (get.compare("e") == 0)
	  {
		     if (writebmv(bmvctx, GetE, 11) != 1)
		{
			printf("Error write to bmv\n");
			exit(-1);
		}
		uint8_t buffer[100];
		int buffersize=0;
		buffersize = readbmv(bmvctx, buffer, 100);
		if (buffersize <= 0)
		{
			printf("error Read BMV\n");
			exit(-1);
		}
		if (buffer[0] == 0x07 && buffer[1] == 0x04 && buffer[2] == 0x03 && buffer[3] == 0x00)
		{
			uint32_t e;
			e = buffer[4] | (buffer[5] << 8) | (buffer[6] << 16) | (buffer[7] << 24);
			printf("%d\n", e);
			exit(1);
		}
		else
		{
			printf("Error in BMV Response\n");
			exit(-1);
		}
	  }
	  
	  else if (get.compare("f") == 0)
	  {
		    if (writebmv(bmvctx, GetF, 11) != 1)
		{
			printf("Error write to bmv\n");
			exit(-1);
		}
		uint8_t buffer[100];
		int buffersize=0;
		buffersize = readbmv(bmvctx, buffer, 100);
		if (buffersize <= 0)
		{
			printf("error Read BMV\n");
			exit(-1);
		}
		if (buffer[0] == 0x07 && buffer[1] == 0x05 && buffer[2] == 0x03 && buffer[3] == 0x00)
		{
			int32_t f;
			f = buffer[4] | (buffer[5] << 8) | (buffer[6] << 16) | (buffer[7] << 24);
			printf("%d\n", f);
			exit(1);
		}
		else
		{
			printf("Error in BMV Response\n");
			exit(-1);
		} 
	  }
	  
	  else if (get.compare("g") == 0)
	  {
		      if (writebmv(bmvctx, GetG, 11) != 1)
		{
			printf("Error write to bmv\n");
			exit(-1);
		}
		uint8_t buffer[100];
		int buffersize=0;
		buffersize = readbmv(bmvctx, buffer, 100);
		if (buffersize <= 0)
		{
			printf("error Read BMV\n");
			exit(-1);
		}
		if (buffer[0] == 0x07 && buffer[1] == 0x06 && buffer[2] == 0x03 && buffer[3] == 0x00)
		{
			int32_t g;
			g = buffer[4] | (buffer[5] << 8) | (buffer[6] << 16) | (buffer[7] << 24);
			printf("%d\n", g);
			exit(1);
		}
		else
		{
			printf("Error in BMV Response\n");
			exit(-1);
		}
	  }
	  
	  else if (get.compare("h") == 0)
	  {
		      if (writebmv(bmvctx, GetH, 11) != 1)
		{
			printf("Error write to bmv\n");
			exit(-1);
		}
		uint8_t buffer[100];
		int buffersize=0;
		buffersize = readbmv(bmvctx, buffer, 100);
		if (buffersize <= 0)
		{
			printf("error Read BMV\n");
			exit(-1);
		}
		if (buffer[0] == 0x07 && buffer[1] == 0x07 && buffer[2] == 0x03 && buffer[3] == 0x00)
		{
			int32_t h;
			h = buffer[4] | (buffer[5] << 8) | (buffer[6] << 16) | (buffer[7] << 24);
			printf("%d\n", h);
			exit(1);
		}
		else
		{
			printf("Error in BMV Response\n");
			exit(-1);
		}
	  }
	  
	  else if (get.compare("i") == 0)
	  {
		      if (writebmv(bmvctx, GetI, 11) != 1)
		{
			printf("Error write to bmv\n");
			exit(-1);
		}
		uint8_t buffer[100];
		int buffersize=0;
		buffersize = readbmv(bmvctx, buffer, 100);
		if (buffersize <= 0)
		{
			printf("error Read BMV\n");
			exit(-1);
		}
		if (buffer[0] == 0x07 && buffer[1] == 0x08 && buffer[2] == 0x03 && buffer[3] == 0x00)
		{
			uint32_t i;
			i = buffer[4] | (buffer[5] << 8) | (buffer[6] << 16) | (buffer[7] << 24);
			printf("%d\n", i);
			exit(1);
		}
		else
		{
			printf("Error in BMV Response\n");
			exit(-1);
		}
	  }
	  
	  else if (get.compare("j") == 0)
	  {
		    if (writebmv(bmvctx, GetJ, 11) != 1)
		{
			printf("Error write to bmv\n");
			exit(-1);
		}
		uint8_t buffer[100];
		int buffersize=0;
		buffersize = readbmv(bmvctx, buffer, 100);
		if (buffersize <= 0)
		{
			printf("error Read BMV\n");
			exit(-1);
		}
		if (buffer[0] == 0x07 && buffer[1] == 0x09 && buffer[2] == 0x03 && buffer[3] == 0x00)
		{
			uint32_t j;
			j = buffer[4] | (buffer[5] << 8) | (buffer[6] << 16) | (buffer[7] << 24);
			printf("%d\n", j);
			exit(1);
		}
		else
		{
			printf("Error in BMV Response\n");
			exit(-1);
		} 
	  }
	  
	  else if (get.compare("l") == 0)
	  {
		      if (writebmv(bmvctx, GetL, 11) != 1)
		{
			printf("Error write to bmv\n");
			exit(-1);
		}
		uint8_t buffer[100];
		int buffersize=0;
		buffersize = readbmv(bmvctx, buffer, 100);
		if (buffersize <= 0)
		{
			printf("error Read BMV\n");
			exit(-1);
		}
		if (buffer[0] == 0x07 && buffer[1] == 0x0A && buffer[2] == 0x03 && buffer[3] == 0x00)
		{
			uint32_t l;
			l = buffer[4] | (buffer[5] << 8) | (buffer[6] << 16) | (buffer[7] << 24);
			printf("%d\n", l);
			exit(1);
		}
		else
		{
			printf("Error in BMV Response\n");
			exit(-1);
		} 
	  }
	  
	  else if (get.compare("m") == 0)
	  {
		        if (writebmv(bmvctx, GetM, 11) != 1)
		{
			printf("Error write to bmv\n");
			exit(-1);
		}
		uint8_t buffer[100];
		int buffersize=0;
		buffersize = readbmv(bmvctx, buffer, 100);
		if (buffersize <= 0)
		{
			printf("error Read BMV\n");
			exit(-1);
		}
		if (buffer[0] == 0x07 && buffer[1] == 0x0B && buffer[2] == 0x03 && buffer[3] == 0x00)
		{
			uint32_t m;
			m = buffer[4] | (buffer[5] << 8) | (buffer[6] << 16) | (buffer[7] << 24);
			printf("%d\n", m);
			exit(1);
		}
		else
		{
			printf("Error in BMV Response\n");
			exit(-1);
		} 
	  }
	  
	  else if (get.compare("r") == 0)
	  {
		       if (writebmv(bmvctx, GetR, 11) != 1)
		{
			printf("Error write to bmv\n");
			exit(-1);
		}
		uint8_t buffer[100];
		int buffersize=0;
		buffersize = readbmv(bmvctx, buffer, 100);
		if (buffersize <= 0)
		{
			printf("error Read BMV\n");
			exit(-1);
		}
		if (buffer[0] == 0x07 && buffer[1] == 0x10 && buffer[2] == 0x03 && buffer[3] == 0x00)
		{
			uint32_t r;
			r = buffer[4] | (buffer[5] << 8) | (buffer[6] << 16) | (buffer[7] << 24);
			printf("%d\n", r);
			exit(1);
		}
		else
		{
			printf("Error in BMV Response\n");
			exit(-1);
		} 
	  }
	  
	  else if (get.compare("s") == 0)
	  {
		      if (writebmv(bmvctx, GetS, 11) != 1)
		{
			printf("Error write to bmv\n");
			exit(-1);
		}
		uint8_t buffer[100];
		int buffersize=0;
		buffersize = readbmv(bmvctx, buffer, 100);
		if (buffersize <= 0)
		{
			printf("error Read BMV\n");
			exit(-1);
		}
		if (buffer[0] == 0x07 && buffer[1] == 0x11 && buffer[2] == 0x03 && buffer[3] == 0x00)
		{
			uint32_t s;
			s = buffer[4] | (buffer[5] << 8) | (buffer[6] << 16) | (buffer[7] << 24);
			printf("%d\n", s);
			exit(1);
		}
		else
		{
			printf("Error in BMV Response\n");
			exit(-1);
		} 
	  }
	  
	  else
	  {
		  printf("Error unknwon Command\n");
		  exit(-1);
	  }
	  
	
	  
	  
	  disconnectbmv(bmvctx);
	  exit(0);
}

int readbmv(int bmvctx, uint8_t *buffer, int lenght)
{
	
	int rv;
		fd_set set;
		struct timeval timeout;
		int x=0;
		bool startfound = false;
		bool stopfound = false;
		char byte;
		char temp[20];
		int num = 0;
		for (x = 0; x<30; x++)
		{
			FD_ZERO(&set);
			FD_SET(bmvctx, &set);
			timeout.tv_sec = 3;
			timeout.tv_usec = 0;
			rv = select(bmvctx + 1, &set, NULL, NULL, &timeout);
			if(rv == -1)
			{
				printf("Error on Select\n"); /* an error accured */
				return -1;
			}
			else if(rv == 0)
			{
				printf("Timout on Select\n");
				return -1; //Timeout
			}
		//	else 
				//printf("select returns for reading\n");
		
			int bytesread = 0;
			bytesread = 0;
			bytesread = read(bmvctx, &byte, 1);
			if (bytesread < 1)
			{
				printf("bmvcontroler Error on read bmv");
				exit(-1);
			}
			//printf("%d bytes read from bmv: %c\n", bytesread, byte);
			//int n;
			//for(n=0; n<bytesread; n++)
			//{
			//	printf("%c", read_buffer[n]);
			//}
			if (startfound == false)
			{
				if (byte == ':')
				{
					startfound = true;
					continue;
				}
				else 
					continue;
			}
			else if (startfound == true && stopfound == false)
			{
				if (byte == '\n')
				{
					stopfound = true;
					break;
				}
				else
				{
					if (num >= 20)
					{
						printf("Error buffer overun\n");
						exit (-1);
					}
					temp[num] = byte;
					num++;
					continue;
				}
			}
		}
		if (!stopfound || !startfound)
		{
			printf("Error Receive end of message");
			exit(-1);
		}
		//int y = 0;
		//printf("%i bytes in buffer: ", num);
	//	for (y=0;y<num;y++)
	//	{
	//		printf("%c", temp[y]);
	//	}
	//	printf("\n");
		std::string message;
		message.assign(temp, num);
	//	printf ("Message: %s\n", message.c_str());
		//uint8_t hexbuffer[100];
		buffer[0] = std::stoi(message.substr(0, 1), NULL, 16);
	//	printf("buffer[0]= %02X\n", buffer[0]);
		int n=1;
		int buffersize=1;
		for (n=1 ; n<num; n++)
		{
			buffer[buffersize] = std::stoi(message.substr(n, 2), NULL, 16);
		//	printf("buffer[m]= %02X\n", buffer[buffersize]);
			n++;
			buffersize++;
			
		}
		int z = 0;
		uint8_t check = 0x55;
		for (z=0; z<buffersize; z++)
		{
			check -= buffer[z];
		}
	//	printf ("check = %02X\n", check);
		if (check == 0)
		{
			return buffersize;
		}
		else
		{
			printf("Invalid Checksum");
			exit (-1);
		}
		
}
 int writebmv(int bmvctx, char *buffer, int lenght)
	{	
	  //char buffer[] = ":70010003E\n"; //11
	  int rc=0;
	  rc = write(bmvctx, &buffer[0], lenght);
	//  printf ("%d bytes writen to bmv\n", rc);
	  if (rc <= 0)
		{
			printf("bmvcontroler Error on send\n");
			return -1;
		}
		return 1;
	}
	
int connectbmv(std::string bmvdev)
{
	std::string device;
	speed_t speed = B19200;
	int parity;
	int databit;
	int stopbit;
	int ctx;
	
	ctx = open(bmvdev.c_str(),O_RDWR);
	//ctx = open(bmvdev.c_str(),O_RDWR | O_NOCTTY);
  if(ctx<0)
  {
    printf("bmvcontroler:Open device failure\n") ;
    return -1;
  }

  /* save old Terminal Settings */
  if (tcgetattr (ctx, &oldSettings) < 0)
  {
    printf("bmvcontroler:Can't get terminal parameters");
    return -1 ;
  }

  memset(&Settings, 0, sizeof(struct termios));
  if ((cfsetispeed(&Settings, speed) < 0) ||
        (cfsetospeed(&Settings, speed) < 0)) 
		{
			disconnectbmv(ctx);
			ctx = -1;
			printf("bmvcontroler: Error setting Baudrate");
			return -1;
		}
	
	Settings.c_cflag = Settings.c_cflag | CS8;
	//Settings.c_iflag = IGNPAR;
	Settings.c_cflag &=~ PARENB;
	//Settings.c_iflag &= ~INPCK;
	
	/* Software flow control is disabled */
	Settings.c_iflag &= ~(IXON | IXOFF | IXANY);
   Settings.c_cc[VTIME] = 10;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
Settings.c_cc[VMIN] = 0;
   
	Settings.c_cflag = Settings.c_cflag | CLOCAL | CREAD;
	//Settings.c_oflag = 0;
	Settings.c_lflag = 0; /*ICANON;*/
	 /* clean port */
	//tcflush(ctx, TCIFLUSH);
	
	 /* activate the settings port */
  if (tcsetattr(ctx,TCSANOW,&Settings) <0)
  {
    printf("bmvcontroler: Can't set terminal parameters ");
    return -1 ;
  }
  
			
	
	// Disable RTS Mode
	
	int flags;
	
	//ioctl(ctx, TIOCMGET, &flags);
   
      //  flags &= ~TIOCM_RTS;
    
	//ioctl(ctx, TIOCMSET, &flags);
				
  
  /* clean I & O device */
  //tcflush(ctx,TCIOFLUSH);
  bmv_connected = true;
  
 
   return ctx;
} 

void disconnectbmv(int ctx)
{
	if (ctx >= 0)
	{
		tcsetattr(ctx,TCSANOW,&oldSettings);
		close(ctx);
	}
	ctx = -1;
	bmv_connected = false;
}

