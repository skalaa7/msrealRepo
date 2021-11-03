#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
	FILE *fp;
	char *str;
	char tval1;
	size_t num_of_bytes = 6;
	float percentage;
	long int period = 20000L;
	char taster,stanje;
	
	while(1)
	{
		fp = fopen ("/dev/button", "r");
		if(fp==NULL)
		{
			puts("Problem pri otvaranju /dev/button");
			return -1;
		}
		str = (char *)malloc(num_of_bytes+1); 
		getline(&str, &num_of_bytes, fp); 

		if(fclose(fp))
		{
			puts("Problem pri zatvaranju /dev/button");
			return -1;
		}
		
		tval1 = str[2] - 48;
		
		if(tval1==1)
		{
			if(stanje<21)
				stanje++;
			
		}
		else
			stanje=0;
		
		if(stanje==20)
			taster=1;
		
		
		if(taster==1)
		{
			percentage=percentage+0.1;
			if (percentage>1)
				percentage=1;
			else if(percentage<0)
				percentage=0;
			
			taster=0;
		}
		fp = fopen("/dev/led", "w");
		if(fp == NULL)
		{
			printf("Problem pri otvaranju /dev/led\n");
			return -1;
		}
		fputs("0x0F\n", fp);
		if(fclose(fp))
		{
			printf("Problem pri zatvaranju /dev/led\n");
			return -1;
		}
		usleep(percentage*period);

		// Ugasi diode
		fp = fopen("/dev/led", "w");
		if(fp == NULL)
		{
			printf("Problem pri otvaranju /dev/led\n");
			return -1;
		}
		fputs("0x00\n", fp);
		if(fclose(fp))
		{
			printf("Problem pri zatvaranju /dev/led\n");
			return -1;
		}
		usleep((1-percentage)*period);
		
		
	}

	return 0;
}
