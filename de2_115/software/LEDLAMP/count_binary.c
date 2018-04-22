#include "count_binary.h"
#include "altera_avalon_timer_regs.h"
#include "alt_types.h"


/* Seven Segment Display PIO Functions 
 * sevenseg_set_hex() --  implements a hex digit map.
 */
#ifdef SEVEN_SEG_PIO_BASE
static void sevenseg_set_hex(int hex)
{
    static alt_u8 segments[16] = {
        0x81, 0xCF, 0x92, 0x86, 0xCC, 0xA4, 0xA0, 0x8F, 0x80, 0x84, /* 0-9 */
        0x88, 0xE0, 0xF2, 0xC2, 0xB0, 0xB8 };                       /* a-f */

    unsigned int data = segments[hex & 15] | (segments[(hex >> 4) & 15] << 8);

    IOWR_ALTERA_AVALON_PIO_DATA(SEVEN_SEG_PIO_BASE, data);
}
#endif


alt_u8 key_flag = 0;


void ISR_handle_button(void* context)
{
	key_flag = ~key_flag;
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(PIO_KEY_BASE, 0x0);//CLEAR INTERRUPT
}

void init_button_pio(void)
{
	IOWR_ALTERA_AVALON_PIO_IRQ_MASK(PIO_KEY_BASE, 0x1);//ENABLE KEY_INTERRUPT
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(PIO_KEY_BASE, 0x0);//DISABLE KEY_EDGE_CAPTURE
	alt_ic_isr_register(PIO_KEY_IRQ_INTERRUPT_CONTROLLER_ID, PIO_KEY_IRQ, ISR_handle_button,NULL, 0x0);

}


static void lcd_init( FILE *lcd )
{
    /* If the LCD Display exists, write a simple message on the first line. */
    LCD_PRINTF(lcd, "%c%s SC17023010 JqZHANG", ESC,
               ESC_TOP_LEFT);
}

static void initial_message()
{
    printf("\n\n**************************\n");
    printf("* Hello from Nios II!    *\n");
    printf("* SC17023010 JqZHANG     *\n");
    printf("**************************\n");
}

int led7(int j)
{
	switch (j)
	{
		case 0:
			return 0x81;

		case 1:
			return 0xCF;

		case 2:
			return 0x92;

		case 3:
			return 0x86;

		case 4:
			return 0xCC;

		case 5:
			return 0xA4;

		case 6:
			return 0xA0;

		case 7:
			return 0x8F;

		default:
			return 0x80;

	}
}



int main()
{
	int i;
    FILE * lcd;
	initial_message();
    /* Initialize the LCD, if there is one.
     */
    lcd = LCD_OPEN();
    if(lcd != NULL) {lcd_init( lcd );}

    init_button_pio();

	while(1)
	{

		if(key_flag)
		{

			LCD_PRINTF(lcd, "%c%s %c%s  left \n", ESC, ESC_TOP_LEFT, ESC, ESC_CLEAR, ESC, ESC_COL1_INDENT5);
			for (i = 0; i < 8; i++)
			{
				sevenseg_set_hex(led7(i));
				IOWR_ALTERA_AVALON_PIO_DATA(PIO_LED_BASE, 1<<i);
				usleep(300000);
			}

		}

		else
		{

			LCD_PRINTF(lcd, "%c%s %c%s  right \n", ESC, ESC_TOP_LEFT, ESC, ESC_CLEAR, ESC, ESC_COL1_INDENT5);
			for (i = 7; i >= 0; i--)
			{
				sevenseg_set_hex(led7(i));
				IOWR_ALTERA_AVALON_PIO_DATA(PIO_LED_BASE, 1<<i);
				usleep(300000);
			}
		}



	}
	return 0;
}


