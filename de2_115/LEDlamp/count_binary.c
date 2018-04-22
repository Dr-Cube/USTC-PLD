#include "system.h"
#include "altera_avalon_pio_regs.h"

int main(void)
{

	while(1)
	{
		for (int i = 0; i < 8; i++)
		{
			IOWR_ALTERA_AVALON_PIO_DATA(LED_PIO_BASE, 1<<i);
			usleep(100000);
		}
	}
	return 0;
}
