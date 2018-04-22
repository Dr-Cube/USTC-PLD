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


//多按键中断
/* A variable to hold the value of the button pio edge capture register. */

volatile int edge_capture;

#ifdef BUTTON_PIO_BASE

#ifdef ALT_ENHANCED_INTERRUPT_API_PRESENT
static void handle_button_interrupts(void* context)
#else
static void handle_button_interrupts(void* context, alt_u32 id)
#endif
{
    // Cast context to edge_capture's type. It is important that this be 
    //  declared volatile to avoid unwanted compiler optimization.
    
    volatile int* edge_capture_ptr = (volatile int*) context;
    // Store the value in the Button's edge capture register in *context. 
    *edge_capture_ptr = IORD_ALTERA_AVALON_PIO_EDGE_CAP(BUTTON_PIO_BASE);
    // Reset the Button's edge capture register. 
    IOWR_ALTERA_AVALON_PIO_EDGE_CAP(BUTTON_PIO_BASE, 0);
    
    //* 
    // * Read the PIO to delay ISR exit. This is done to prevent a spurious
    // * interrupt in systems with high processor -> pio latency and fast
    // * interrupts.
    
    IORD_ALTERA_AVALON_PIO_EDGE_CAP(BUTTON_PIO_BASE); 
}

//* Initialize the button_pio. 

static void init_button_pio()
{
    //* Recast the edge_capture pointer to match the alt_irq_register() function
    // * prototype. 
    void* edge_capture_ptr = (void*) &edge_capture;
    //* Enable all 4 button interrupts. 
    IOWR_ALTERA_AVALON_PIO_IRQ_MASK(BUTTON_PIO_BASE, 0xf);
    //* Reset the edge capture register. 
    IOWR_ALTERA_AVALON_PIO_EDGE_CAP(BUTTON_PIO_BASE, 0x0);
    //* Register the interrupt handler. 
#ifdef ALT_ENHANCED_INTERRUPT_API_PRESENT
    alt_ic_isr_register(BUTTON_PIO_IRQ_INTERRUPT_CONTROLLER_ID, BUTTON_PIO_IRQ, 
      handle_button_interrupts, edge_capture_ptr, 0x0);
#else
    alt_irq_register( BUTTON_PIO_IRQ, edge_capture_ptr, handle_button_interrupts);
#endif
}
#endif //* BUTTON_PIO_BASE 

static void handle_button_press(alt_u8 type, FILE *lcd)
{
    //* Button press actions while counting.
    if (type == 'c')
    {
        switch (edge_capture) 
        {
            //* Button 1:  Output counting to LED only.
        case 0x1:
            
            break;
            //* Button 2:  Output counting to SEVEN SEG only.
        case 0x2:
            count_sevenseg();
            break;
            //* Button 3:  Output counting to D only.
        case 0x4:
            count_lcd( lcd );
            break;
            //* Button 4:  Output counting to LED, SEVEN_SEG, and D.
        case 0x8:
            count_all( lcd );
            break;
            //* If value ends up being something different (shouldn't) do same as 8. 
        default:
            count_all( lcd );
            break;
        }
    }
    // If 'type' is anything else, assume we're "waiting"...
    else
    {
        switch (edge_capture)
        {
        case 0x1:
            printf( "Button 1\n");
            edge_capture = 0;
            break;
        case 0x2:
            printf( "Button 2\n");
            edge_capture = 0;
            break;
        case 0x4:
            printf( "Button 3\n");
            edge_capture = 0;
            break;
        case 0x8:
            printf( "Button 4\n");
            edge_capture = 0;
            break;
        default:
            printf( "Button press UNKNOWN!!\n");
        }
    }
}

/*
//单按键中断
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
*/

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
			//break;
		case 1:
			return 0xCF;
			//break;
		case 2:
			return 0x92;
			//break;
		case 3:
			return 0x86;
			//break;
		case 4:
			return 0xCC;
			//break;
		case 5:
			return 0xA4;
			//break;
		case 6:
			return 0xA0;
			//break;
		case 7:
			return 0x8F;
			//break;
		default:
			return 0x80;
			//break;
	}
}

static void ledlamp()
{
	LCD_PRINTF(lcd, "%c%s %c%s  left \n", ESC, ESC_TOP_LEFT, ESC, ESC_CLEAR, ESC, ESC_COL1_INDENT5);
	for (i = 0; i < 8; i++)
	{
		sevenseg_set_hex(led7(i));
		IOWR_ALTERA_AVALON_PIO_DATA(PIO_LED_BASE, 1<<i);
		usleep(100000);
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

	//#ifdef BUTTON_PIO_BASE //#IFDEF是否要去掉，pdf中没有
    init_button_pio();
	//#endif

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

/*
//流水灯
int main(void)
{
	int i;
	while(1)
	{
		for (i = 0; i < 8; i++)
		{
			IOWR_ALTERA_AVALON_PIO_DATA(PIO_LED_BASE, 1<<i);
			usleep(100000);
		}
	}
	return 0;
}
*/
