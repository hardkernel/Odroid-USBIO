/********************************************************************
 FileName:      main.c
 Dependencies:  See INCLUDES section
 Processor:     PIC18, PIC24, dsPIC, and PIC32 USB Microcontrollers
 Hardware:      This demo is natively intended to be used on Microchip USB demo
                boards supported by the MCHPFSUSB stack.  See release notes for
                support matrix.  This demo can be modified for use on other 
                hardware platforms.
 Complier:      Microchip C18 (for PIC18), XC16 (for PIC24/dsPIC), XC32 (for PIC32)
 Company:       Microchip Technology, Inc.

 Software License Agreement:

 The software supplied herewith by Microchip Technology Incorporated
 (the "Company") for its PIC® Microcontroller is intended and
 supplied to you, the Company's customer, for use solely and
 exclusively on Microchip PIC Microcontroller products. The
 software is owned by the Company and/or its supplier, and is
 protected under applicable copyright laws. All rights are reserved.
 Any use in violation of the foregoing restrictions may subject the
 user to criminal sanctions under applicable laws, as well as to
 civil liability for the breach of the terms and conditions of this
 license.

 THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES,
 WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.

********************************************************************
 File Description:

 Change History:
  Rev   Description
  ----  -----------------------------------------
  1.0   Initial release
  2.1   Updated for simplicity and to use common
                     coding style
  2.7b  Improvements to USBCBSendResume(), to make it easier to use.
  2.9f  Adding new part support
********************************************************************/
#include <usart.h>      // hk_usb_io
#include <delays.h>
#ifndef MAIN_C
#define MAIN_C
#define USE_INTERNAL_OSC
/** INCLUDES *******************************************************/
#include "./USB/usb.h"
#include "HardwareProfile.h"
#include "./USB/usb_function_hid.h"

/** CONFIGURATION **************************************************/
#if defined(PICDEM_FS_USB)      // Configuration bits for PICDEM FS USB Demo Board (based on PIC18F4550)
        #pragma config PLLDIV   = 5         // (20 MHz crystal on PICDEM FS USB board)
        #pragma config CPUDIV   = OSC1_PLL2   
        #pragma config USBDIV   = 2         // Clock source from 96MHz PLL/2
        #pragma config FOSC     = HSPLL_HS
        #pragma config FCMEN    = OFF
        #pragma config IESO     = OFF
        #pragma config PWRT     = OFF
        #pragma config BOR      = ON
        #pragma config BORV     = 3
        #pragma config VREGEN   = ON      //USB Voltage Regulator
        #pragma config WDT      = OFF
        #pragma config WDTPS    = 32768
        #pragma config MCLRE    = ON
        #pragma config LPT1OSC  = OFF
        #pragma config PBADEN   = OFF
//      #pragma config CCP2MX   = ON
        #pragma config STVREN   = ON
        #pragma config LVP      = OFF
//      #pragma config ICPRT    = OFF       // Dedicated In-Circuit Debug/Programming
        #pragma config XINST    = OFF       // Extended Instruction Set
        #pragma config CP0      = OFF
        #pragma config CP1      = OFF
//      #pragma config CP2      = OFF
//      #pragma config CP3      = OFF
        #pragma config CPB      = OFF
//      #pragma config CPD      = OFF
        #pragma config WRT0     = OFF
        #pragma config WRT1     = OFF
//      #pragma config WRT2     = OFF
//      #pragma config WRT3     = OFF
        #pragma config WRTB     = OFF       // Boot Block Write Protection
        #pragma config WRTC     = OFF
//      #pragma config WRTD     = OFF
        #pragma config EBTR0    = OFF
        #pragma config EBTR1    = OFF
//      #pragma config EBTR2    = OFF
//      #pragma config EBTR3    = OFF
        #pragma config EBTRB    = OFF

#elif defined(PICDEM_FS_USB_K50)
        #pragma config PLLSEL   = PLL3X     // 3X PLL multiplier selected
        #pragma config CFGPLLEN = OFF       // PLL turned on during execution
        #pragma config CPUDIV   = NOCLKDIV  // 1:1 mode (for 48MHz CPU)
        #pragma config LS48MHZ  = SYS48X8   // Clock div / 8 in Low Speed USB mode
        #pragma config FOSC     = INTOSCIO  // HFINTOSC selected at powerup, no clock out
        #pragma config PCLKEN   = OFF       // Primary oscillator driver
        #pragma config FCMEN    = OFF       // Fail safe clock monitor
        #pragma config IESO     = OFF       // Internal/external switchover (two speed startup)
        #pragma config nPWRTEN  = OFF       // Power up timer
        #pragma config BOREN    = SBORDIS   // BOR enabled
        #pragma config nLPBOR   = ON        // Low Power BOR
        #pragma config WDTEN    = SWON      // Watchdog Timer controlled by SWDTEN
        #pragma config WDTPS    = 32768     // WDT postscalar
        #pragma config PBADEN   = OFF       // Port B Digital/Analog Powerup Behavior
        #pragma config SDOMX    = RC7       // SDO function location
        #pragma config LVP      = OFF       // Low voltage programming
        #pragma config MCLRE    = ON        // MCLR function enabled (RE3 disabled)
        #pragma config STVREN   = ON        // Stack overflow reset
        //#pragma config ICPRT  = OFF       // Dedicated ICPORT program/debug pins enable
        #pragma config XINST    = OFF       // Extended instruction set

#elif defined(PIC18F87J50_PIM)				// Configuration bits for PIC18F87J50 FS USB Plug-In Module board
        #pragma config XINST    = OFF   	// Extended instruction set
        #pragma config STVREN   = ON      	// Stack overflow reset
        #pragma config PLLDIV   = 3         // (12 MHz crystal used on this board)
        #pragma config WDTEN    = OFF      	// Watch Dog Timer (WDT)
        #pragma config CP0      = OFF      	// Code protect
        #pragma config CPUDIV   = OSC1      // OSC1 = divide by 1 mode
        #pragma config IESO     = OFF      	// Internal External (clock) Switchover
        #pragma config FCMEN    = OFF      	// Fail Safe Clock Monitor
        #pragma config FOSC     = HSPLL     // Firmware must also set OSCTUNE<PLLEN> to start PLL!
        #pragma config WDTPS    = 32768
//      #pragma config WAIT     = OFF      	// Commented choices are
//      #pragma config BW       = 16      	// only available on the
//      #pragma config MODE     = MM      	// 80 pin devices in the 
//      #pragma config EASHFT   = OFF      	// family.
        #pragma config MSSPMSK  = MSK5
//      #pragma config PMPMX    = DEFAULT
//      #pragma config ECCPMX   = DEFAULT
        #pragma config CCP2MX   = DEFAULT   
        
// Configuration bits for PIC18F97J94 PIM and PIC18F87J94 PIM
#elif defined(PIC18F97J94_PIM) || defined(PIC18F87J94_PIM)
        #pragma config STVREN   = ON      	// Stack overflow reset
        #pragma config XINST    = OFF   	// Extended instruction set
        #pragma config BOREN    = ON        // BOR Enabled
        #pragma config BORV     = 0         // BOR Set to "2.0V" nominal setting
        #pragma config CP0      = OFF      	// Code protect disabled
        #pragma config FOSC     = FRCPLL    // Firmware should also enable active clock tuning for this setting
        #pragma config SOSCSEL  = LOW       // SOSC circuit configured for crystal driver mode
        #pragma config CLKOEN   = OFF       // Disable clock output on RA6
        #pragma config IESO     = OFF      	// Internal External (clock) Switchover
        #pragma config PLLDIV   = NODIV     // 4 MHz input (from 8MHz FRC / 2) provided to PLL circuit
        #pragma config POSCMD   = NONE      // Primary osc disabled, using FRC
        #pragma config FSCM     = CSECMD    // Clock switching enabled, fail safe clock monitor disabled
        #pragma config WPDIS    = WPDIS     // Program memory not write protected
        #pragma config WPCFG    = WPCFGDIS  // Config word page of program memory not write protected
        #pragma config IOL1WAY  = OFF       // IOLOCK can be set/cleared as needed with unlock sequence
        #pragma config LS48MHZ  = SYSX2     // Low Speed USB clock divider
        #pragma config WDTCLK   = LPRC      // WDT always uses INTOSC/LPRC oscillator
        #pragma config WDTEN    = ON        // WDT disabled; SWDTEN can control WDT
        #pragma config WINDIS   = WDTSTD    // Normal non-window mode WDT.
        #pragma config VBTBOR   = OFF       // VBAT BOR disabled
      
#elif defined(PIC18F46J50_PIM) || defined(PIC18F_STARTER_KIT_1) || defined(PIC18F47J53_PIM)
     #pragma config WDTEN = OFF          //WDT disabled (enabled by SWDTEN bit)
     #pragma config PLLDIV = 3           //Divide by 3 (12 MHz oscillator input)
     #pragma config STVREN = ON          //stack overflow/underflow reset enabled
     #pragma config XINST = OFF          //Extended instruction set disabled
     #pragma config CPUDIV = OSC1        //No CPU system clock divide
     #pragma config CP0 = OFF            //Program memory is not code-protected
     #pragma config OSC = HSPLL          //HS oscillator, PLL enabled, HSPLL used by USB
     #pragma config FCMEN = OFF          //Fail-Safe Clock Monitor disabled
     #pragma config IESO = OFF           //Two-Speed Start-up disabled
     #pragma config WDTPS = 32768        //1:32768
     #pragma config DSWDTOSC = INTOSCREF //DSWDT uses INTOSC/INTRC as clock
     #pragma config RTCOSC = T1OSCREF    //RTCC uses T1OSC/T1CKI as clock
     #pragma config DSBOREN = OFF        //Zero-Power BOR disabled in Deep Sleep
     #pragma config DSWDTEN = OFF        //Disabled
     #pragma config DSWDTPS = 8192       //1:8,192 (8.5 seconds)
     #pragma config IOL1WAY = OFF        //IOLOCK bit can be set and cleared
     #pragma config MSSP7B_EN = MSK7     //7 Bit address masking
     #pragma config WPFP = PAGE_1        //Write Protect Program Flash Page 0
     #pragma config WPEND = PAGE_0       //Start protection at page 0
     #pragma config WPCFG = OFF          //Write/Erase last page protect Disabled
     #pragma config WPDIS = OFF          //WPFP[5:0], WPEND, and WPCFG bits ignored 
     #if defined(PIC18F47J53_PIM)
        #pragma config CFGPLLEN = OFF
     #else
        #pragma config T1DIG = ON           //Sec Osc clock source may be selected
        #pragma config LPT1OSC = OFF        //high power Timer1 mode
     #endif
#elif defined(LOW_PIN_COUNT_USB_DEVELOPMENT_KIT)
        #pragma config CPUDIV = NOCLKDIV
        #pragma config USBDIV = OFF
        #pragma config FOSC   = HS
        #pragma config PLLEN  = ON
        #pragma config FCMEN  = OFF
        #pragma config IESO   = OFF
        #pragma config PWRTEN = OFF
        #pragma config BOREN  = OFF
        #pragma config BORV   = 30
        #pragma config WDTEN  = OFF
        #pragma config WDTPS  = 32768
        #pragma config MCLRE  = OFF
        #pragma config HFOFST = OFF
        #pragma config STVREN = ON
        #pragma config LVP    = OFF
        #pragma config XINST  = OFF
        #pragma config BBSIZ  = OFF
        #pragma config CP0    = OFF
        #pragma config CP1    = OFF
        #pragma config CPB    = OFF
        #pragma config WRT0   = OFF
        #pragma config WRT1   = OFF
        #pragma config WRTB   = OFF
        #pragma config WRTC   = OFF
        #pragma config EBTR0  = OFF
        #pragma config EBTR1  = OFF
        #pragma config EBTRB  = OFF                                                  // CONFIG7H

#elif	defined(PIC16F1_LPC_USB_DEVELOPMENT_KIT)
    // PIC 16F1459 fuse configuration:
    // Config word 1 (Oscillator configuration)
    // 20Mhz crystal input scaled to 48Mhz and configured for USB operation
    #if defined (USE_INTERNAL_OSC)
#warning Using Internal Oscillator
        __CONFIG(FOSC_INTOSC & WDTE_OFF & PWRTE_OFF & MCLRE_OFF & CP_OFF & BOREN_OFF & CLKOUTEN_ON & IESO_OFF & FCMEN_OFF);
        __CONFIG(WRT_OFF & CPUDIV_NOCLKDIV & USBLSCLK_48MHz & PLLMULT_3x & PLLEN_ENABLED & STVREN_ON &  BORV_LO & LPBOR_OFF & LVP_OFF);
    #else
#warning Using Crystal Oscillator
        __CONFIG(FOSC_HS & WDTE_OFF & PWRTE_OFF & MCLRE_OFF & CP_OFF & BOREN_OFF & CLKOUTEN_ON & IESO_OFF & FCMEN_OFF);
        __CONFIG(WRT_OFF & CPUDIV_NOCLKDIV & USBLSCLK_48MHz & PLLMULT_4x & PLLEN_ENABLED & STVREN_ON &  BORV_LO & LPBOR_OFF & LVP_OFF);
    #endif

#elif defined(EXPLORER_16)
    #if defined(__PIC24FJ256GB110__)
        _CONFIG1( JTAGEN_OFF & GCP_OFF & GWRP_OFF & FWDTEN_OFF & ICS_PGx2) 
        _CONFIG2( PLL_96MHZ_ON & IESO_OFF & FCKSM_CSDCMD & OSCIOFNC_ON & POSCMOD_HS & FNOSC_PRIPLL & PLLDIV_DIV2 & IOL1WAY_ON)
    #elif defined(PIC24FJ256GB210_PIM)
        _CONFIG1(FWDTEN_OFF & ICS_PGx2 & GWRP_OFF & GCP_OFF & JTAGEN_OFF)
        _CONFIG2(POSCMOD_HS & IOL1WAY_ON & OSCIOFNC_ON & FCKSM_CSDCMD & FNOSC_PRIPLL & PLL96MHZ_ON & PLLDIV_DIV2 & IESO_OFF)
    #elif defined(__PIC24FJ64GB004__)
        _CONFIG1(WDTPS_PS1 & FWPSA_PR32 & WINDIS_OFF & FWDTEN_OFF & ICS_PGx1 & GWRP_OFF & GCP_OFF & JTAGEN_OFF)
        _CONFIG2(POSCMOD_HS & I2C1SEL_PRI & IOL1WAY_OFF & OSCIOFNC_ON & FCKSM_CSDCMD & FNOSC_PRIPLL & PLL96MHZ_ON & PLLDIV_DIV2 & IESO_OFF)
        _CONFIG3(WPFP_WPFP0 & SOSCSEL_SOSC & WUTSEL_LEG & WPDIS_WPDIS & WPCFG_WPCFGDIS & WPEND_WPENDMEM)
        _CONFIG4(DSWDTPS_DSWDTPS3 & DSWDTOSC_LPRC & RTCOSC_SOSC & DSBOREN_OFF & DSWDTEN_OFF)
    #elif defined(__32MX460F512L__) || defined(__32MX795F512L__)
        #pragma config UPLLEN   = ON        // USB PLL Enabled
        #pragma config FPLLMUL  = MUL_15        // PLL Multiplier
        #pragma config UPLLIDIV = DIV_2         // USB PLL Input Divider
        #pragma config FPLLIDIV = DIV_2         // PLL Input Divider
        #pragma config FPLLODIV = DIV_1         // PLL Output Divider
        #pragma config FPBDIV   = DIV_1         // Peripheral Clock divisor
        #pragma config FWDTEN   = OFF           // Watchdog Timer
        #pragma config WDTPS    = PS1           // Watchdog Timer Postscale
        #pragma config FCKSM    = CSDCMD        // Clock Switching & Fail Safe Clock Monitor
        #pragma config OSCIOFNC = OFF           // CLKO Enable
        #pragma config POSCMOD  = HS            // Primary Oscillator
        #pragma config IESO     = OFF           // Internal/External Switch-over
        #pragma config FSOSCEN  = OFF           // Secondary Oscillator Enable (KLO was off)
        #pragma config FNOSC    = PRIPLL        // Oscillator Selection
        #pragma config CP       = OFF           // Code Protect
        #pragma config BWP      = OFF           // Boot Flash Write Protect
        #pragma config PWP      = OFF           // Program Flash Write Protect
        #pragma config ICESEL   = ICS_PGx2      // ICE/ICD Comm Channel Select
    #elif defined(__dsPIC33EP512MU810__) || defined (__PIC24EP512GU810__)
        _FOSCSEL(FNOSC_FRC);
        _FOSC(FCKSM_CSECMD & OSCIOFNC_OFF & POSCMD_XT);
        _FWDT(FWDTEN_OFF);
    
    #else
        #error No hardware board defined, see "HardwareProfile.h" and __FILE__
    #endif
#elif defined(PIC24F_STARTER_KIT)
    _CONFIG1( JTAGEN_OFF & GCP_OFF & GWRP_OFF & FWDTEN_OFF & ICS_PGx2) 
    _CONFIG2( PLL_96MHZ_ON & IESO_OFF & FCKSM_CSDCMD & OSCIOFNC_OFF & POSCMOD_HS & FNOSC_PRIPLL & PLLDIV_DIV3 & IOL1WAY_ON)
#elif defined(PIC24FJ256DA210_DEV_BOARD)
    _CONFIG1(FWDTEN_OFF & ICS_PGx2 & GWRP_OFF & GCP_OFF & JTAGEN_OFF)
    _CONFIG2(POSCMOD_HS & IOL1WAY_ON & OSCIOFNC_ON & FCKSM_CSDCMD & FNOSC_PRIPLL & PLL96MHZ_ON & PLLDIV_DIV2 & IESO_OFF)
#elif defined(PIC32_USB_STARTER_KIT)
    #pragma config UPLLEN   = ON        // USB PLL Enabled
    #pragma config FPLLMUL  = MUL_15        // PLL Multiplier
    #pragma config UPLLIDIV = DIV_2         // USB PLL Input Divider
    #pragma config FPLLIDIV = DIV_2         // PLL Input Divider
    #pragma config FPLLODIV = DIV_1         // PLL Output Divider
    #pragma config FPBDIV   = DIV_1         // Peripheral Clock divisor
    #pragma config FWDTEN   = OFF           // Watchdog Timer
    #pragma config WDTPS    = PS1           // Watchdog Timer Postscale
    #pragma config FCKSM    = CSDCMD        // Clock Switching & Fail Safe Clock Monitor
    #pragma config OSCIOFNC = OFF           // CLKO Enable
    #pragma config POSCMOD  = HS            // Primary Oscillator
    #pragma config IESO     = OFF           // Internal/External Switch-over
    #pragma config FSOSCEN  = OFF           // Secondary Oscillator Enable (KLO was off)
    #pragma config FNOSC    = PRIPLL        // Oscillator Selection
    #pragma config CP       = OFF           // Code Protect
    #pragma config BWP      = OFF           // Boot Flash Write Protect
    #pragma config PWP      = OFF           // Program Flash Write Protect
    #pragma config ICESEL   = ICS_PGx2      // ICE/ICD Comm Channel Select
#elif defined(DSPIC33E_USB_STARTER_KIT)
        _FOSCSEL(FNOSC_FRC);
        _FOSC(FCKSM_CSECMD & OSCIOFNC_OFF & POSCMD_XT);
        _FWDT(FWDTEN_OFF);
#elif defined(PIC24FJ64GB502_MICROSTICK)
    _CONFIG1(WDTPS_PS1 & FWPSA_PR32 & WINDIS_OFF & FWDTEN_OFF & ICS_PGx1 & GWRP_OFF & GCP_OFF & JTAGEN_OFF)
    _CONFIG2(I2C1SEL_PRI & IOL1WAY_OFF & FCKSM_CSDCMD & FNOSC_PRIPLL & PLL96MHZ_ON & PLLDIV_DIV2 & IESO_OFF)
    _CONFIG3(WPFP_WPFP0 & SOSCSEL_SOSC & WUTSEL_LEG & WPDIS_WPDIS & WPCFG_WPCFGDIS & WPEND_WPENDMEM)
    _CONFIG4(DSWDTPS_DSWDTPS3 & DSWDTOSC_LPRC & RTCOSC_SOSC & DSBOREN_OFF & DSWDTEN_OFF)
#else
    #error No hardware board defined, see "HardwareProfile.h" and __FILE__
#endif

/** VARIABLES ******************************************************/
#if defined(__18CXX)
    #pragma udata

    //The ReceivedDataBuffer[] and ToSendDataBuffer[] arrays are used as
    //USB packet buffers in this firmware.  Therefore, they must be located in
    //a USB module accessible portion of microcontroller RAM.
    #if defined(__18F14K50) || defined(__18F13K50) || defined(__18LF14K50) || defined(__18LF13K50) 
        #pragma udata USB_VARIABLES=0x260
    #elif defined(__18F2455) || defined(__18F2550) || defined(__18F4455) || defined(__18F4550)\
        || defined(__18F2458) || defined(__18F2553) || defined(__18F4458) || defined(__18F4553)\
        || defined(__18LF24K50) || defined(__18F24K50) || defined(__18LF25K50)\
        || defined(__18F25K50) || defined(__18LF45K50) || defined(__18F45K50)
        #pragma udata USB_VARIABLES=0x500
    #elif defined(__18F4450) || defined(__18F2450)
        #pragma udata USB_VARIABLES=0x480
    #else
        #pragma udata
    #endif
#endif

#if defined(__XC8)
    #if defined(_18F14K50) || defined(_18F13K50) || defined(_18LF14K50) || defined(_18LF13K50)
        #define RX_DATA_BUFFER_ADDRESS @0x260
        #define TX_DATA_BUFFER_ADDRESS @0x2A0
    #elif  defined(_18F2455)   || defined(_18F2550)   || defined(_18F4455)  || defined(_18F4550)\
        || defined(_18F2458)   || defined(_18F2453)   || defined(_18F4558)  || defined(_18F4553)\
        || defined(_18LF24K50) || defined(_18F24K50)  || defined(_18LF25K50)\
        || defined(_18F25K50)  || defined(_18LF45K50) || defined(_18F45K50)
        #define RX_DATA_BUFFER_ADDRESS @0x500
        #define TX_DATA_BUFFER_ADDRESS @0x540
    #elif defined(_18F4450) || defined(_18F2450)
        #define RX_DATA_BUFFER_ADDRESS @0x480
        #define TX_DATA_BUFFER_ADDRESS @0x4C0
    #elif defined(_16F1459)
        #define RX_DATA_BUFFER_ADDRESS @0x2050
        #define TX_DATA_BUFFER_ADDRESS @0x20A0
    #else
        #define RX_DATA_BUFFER_ADDRESS
        #define RX_DATA_BUFFER_ADDRESS
    #endif
#else
    #define RX_DATA_BUFFER_ADDRESS
    #define TX_DATA_BUFFER_ADDRESS
#endif

#define BSIZE  80      // hk_usb_io
unsigned char ReceivedDataBuffer[BSIZE] RX_DATA_BUFFER_ADDRESS;
unsigned char ToSendDataBuffer[BSIZE] TX_DATA_BUFFER_ADDRESS;
// HK_USB_IO demo
char version[] = "050";     // version major.minor.fix
// revisions:  031   first release:  gpio, adc, serial
// 040:  i2c
// 041:  i2c bumped to 400khz
// 050:  SPI support added

#if defined(__18CXX)
#pragma udata
#endif

USB_HANDLE USBOutHandle = 0;    //USB handle.  Must be initialized to 0 at startup.
USB_HANDLE USBInHandle = 0;     //USB handle.  Must be initialized to 0 at startup.

BOOL blinkStatusValid = TRUE;

/** PRIVATE PROTOTYPES *********************************************/
void BlinkUSBStatus(void);
BOOL Switch2IsPressed(void);
BOOL Switch3IsPressed(void);
static void InitializeSystem(void);
void ProcessIO(void);
void UserInit(void);
void YourHighPriorityISRCode();
void YourLowPriorityISRCode();
void USBCBSendResume(void);
WORD_VAL ReadPOT(void);
WORD_VAL ReadPOT1(void);    // HK USB_IO demo
void i2c_init(void);
void i2c_idle(void);
void i2c_start(unsigned char);
void i2c_stop(void);
unsigned char i2c_slave_ack(void);
void i2c_write(unsigned char);
void i2c_master_ack(unsigned char);
unsigned char i2c_read(void);
unsigned char i2c_isdatardy(void);
void spi_init(unsigned char, unsigned char, unsigned char);
unsigned char spi_transfer(unsigned char);
void spi_cs(unsigned char);

/** VECTOR REMAPPING ***********************************************/
#if defined(__18CXX)
	//On PIC18 devices, addresses 0x00, 0x08, and 0x18 are used for
	//the reset, high priority interrupt, and low priority interrupt
	//vectors.  However, the current Microchip USB bootloader 
	//examples are intended to occupy addresses 0x00-0x7FF or
	//0x00-0xFFF depending on which bootloader is used.  Therefore,
	//the bootloader code remaps these vectors to new locations
	//as indicated below.  This remapping is only necessary if you
	//wish to program the hex file generated from this project with
	//the USB bootloader.  If no bootloader is used, edit the
	//usb_config.h file and comment out the following defines:
	//#define PROGRAMMABLE_WITH_USB_HID_BOOTLOADER
	//#define PROGRAMMABLE_WITH_USB_LEGACY_CUSTOM_CLASS_BOOTLOADER
	
	#if defined(PROGRAMMABLE_WITH_USB_HID_BOOTLOADER)
		#define REMAPPED_RESET_VECTOR_ADDRESS			0x1000
		#define REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS	0x1008
		#define REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS	0x1018
	#elif defined(PROGRAMMABLE_WITH_USB_MCHPUSB_BOOTLOADER)	
		#define REMAPPED_RESET_VECTOR_ADDRESS			0x800
		#define REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS	0x808
		#define REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS	0x818
	#else	
		#define REMAPPED_RESET_VECTOR_ADDRESS			0x00
		#define REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS	0x08
		#define REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS	0x18
	#endif
	
	#if defined(PROGRAMMABLE_WITH_USB_HID_BOOTLOADER)||defined(PROGRAMMABLE_WITH_USB_MCHPUSB_BOOTLOADER)
	extern void _startup (void);        // See c018i.c in your C18 compiler dir
	#pragma code REMAPPED_RESET_VECTOR = REMAPPED_RESET_VECTOR_ADDRESS
	void _reset (void)
	{
	     _asm goto _startup _endasm
	     //asm ("goto _startup");
	}
	#endif
	#pragma code REMAPPED_HIGH_INTERRUPT_VECTOR = REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS
	void Remapped_High_ISR (void)
	{
	     _asm goto YourHighPriorityISRCode _endasm
	     //asm ("goto YourHighPriorityISRCode");
	}
	#pragma code REMAPPED_LOW_INTERRUPT_VECTOR = REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS
	void Remapped_Low_ISR (void)
	{
	     _asm goto YourLowPriorityISRCode _endasm
	     //asm ("goto YourLowPriorityISRCode");
	}
	
	#if defined(PROGRAMMABLE_WITH_USB_HID_BOOTLOADER)||defined(PROGRAMMABLE_WITH_USB_MCHPUSB_BOOTLOADER)
	//Note: If this project is built while one of the bootloaders has
	//been defined, but then the output hex file is not programmed with
	//the bootloader, addresses 0x08 and 0x18 would end up programmed with 0xFFFF.
	//As a result, if an actual interrupt was enabled and occured, the PC would jump
	//to 0x08 (or 0x18) and would begin executing "0xFFFF" (unprogrammed space).  This
	//executes as nop instructions, but the PC would eventually reach the REMAPPED_RESET_VECTOR_ADDRESS
	//(0x1000 or 0x800, depending upon bootloader), and would execute the "goto _startup".  This
	//would effective reset the application.
	
	//To fix this situation, we should always deliberately place a 
	//"goto REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS" at address 0x08, and a
	//"goto REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS" at address 0x18.  When the output
	//hex file of this project is programmed with the bootloader, these sections do not
	//get bootloaded (as they overlap the bootloader space).  If the output hex file is not
	//programmed using the bootloader, then the below goto instructions do get programmed,
	//and the hex file still works like normal.  The below section is only required to fix this
	//scenario.
	#pragma code HIGH_INTERRUPT_VECTOR = 0x08
	void High_ISR (void)
	{
	     _asm goto REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS _endasm
	     //asm ("goto REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS");
	}
	#pragma code LOW_INTERRUPT_VECTOR = 0x18
	void Low_ISR (void)
	{
	     _asm goto REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS _endasm
	     //asm ("goto REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS");
	}
	#endif	//end of "#if defined(PROGRAMMABLE_WITH_USB_HID_BOOTLOADER)||defined(PROGRAMMABLE_WITH_USB_LEGACY_CUSTOM_CLASS_BOOTLOADER)"

	#pragma code
	
	
	//These are your actual interrupt handling routines.
	#pragma interrupt YourHighPriorityISRCode
	void YourHighPriorityISRCode()
	{
		//Check which interrupt flag caused the interrupt.
		//Service the interrupt
		//Clear the interrupt flag
		//Etc.
        #if defined(USB_INTERRUPT)
	        USBDeviceTasks();
        #endif
	
	}	//This return will be a "retfie fast", since this is in a #pragma interrupt section 
	#pragma interruptlow YourLowPriorityISRCode
	void YourLowPriorityISRCode()
	{
		//Check which interrupt flag caused the interrupt.
		//Service the interrupt
		//Clear the interrupt flag
		//Etc.
	
	}	//This return will be a "retfie", since this is in a #pragma interruptlow section 
#elif defined(_PIC14E)
    	//These are your actual interrupt handling routines.
	void interrupt ISRCode()
	{
		//Check which interrupt flag caused the interrupt.
		//Service the interrupt
		//Clear the interrupt flag
		//Etc.
        #if defined(USB_INTERRUPT)

	        USBDeviceTasks();
        #endif
	}
#endif

#if defined(HARDKERNEL_PIC18F45K50)
void ie_pwm(void) {
		ANSELC &= 0xFB;
		TRISC  |= 0x04; //- RC2 input

		//- PWM Period = [PR2 + 1] * 4 * TOSC * TMR2 Prescale Value
		//- 3,750 Hz   = (199 + 1) * 4 * 1/48,000,000 * 16
		PR2 = 199;
		CCP1CON = 0x0C; //- PWM mode
		
		//- Duty Cycle Ratio = CCPR1L:CCP1CON<5:4> / 4 * (PR2 + 1)
		//- 50 %   =        400          * 1/48,000,000 * 16
		CCPR1L = 0x64; //- 50%
		//- CCPR1L = 0x0;
		T2CON |= 0x06;

		TRISC  &= 0xFB; //- RC2 output
}

void ie_btnLed(void) {
		TRISDbits.TRISD3=1;
		TRISCbits.TRISC2=0;
		if(!(PORTDbits.RD3))
			LATC = 0x04;
		else
			LATC &= 0xFB;
}

#define REG_ADDR(x) (*(volatile unsigned char*)(0xF00 + x))

unsigned char rw_Register(unsigned char addr, char rw, unsigned char val) {
		
		unsigned char * reg;

		reg = &(REG_ADDR(addr));
			
		if(rw)
			*reg = val;

		return *reg;
}

unsigned char rw_Register_bit(unsigned char addr, char bit, char rw, unsigned char val) {
		
		unsigned char * reg;

		reg = &(REG_ADDR(addr));
			
		if(rw) {
			if(val)
				*reg |= val<<bit;
			else
				*reg &= ~(1<<bit);
		}

		return (*reg&(1<<bit))?1:0;
}
#endif

/** DECLARATIONS ***************************************************/
#if defined(__18CXX)
#pragma code
#endif

/********************************************************************
 * Function:        void main(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Main program entry point.
 *
 * Note:            None
 *******************************************************************/
#if defined(__18CXX)
void main(void)
#else
int main(void)
#endif
{   
    InitializeSystem();

    #if defined(USB_INTERRUPT)
        USBDeviceAttach();
    #endif

    while(1)
    {
        #if defined(USB_POLLING)
		// Check bus status and service USB interrupts.
        USBDeviceTasks(); // Interrupt or polling method.  If using polling, must call
        				  // this function periodically.  This function will take care
        				  // of processing and responding to SETUP transactions 
        				  // (such as during the enumeration process when you first
        				  // plug in).  USB hosts require that USB devices should accept
        				  // and process SETUP packets in a timely fashion.  Therefore,
        				  // when using polling, this function should be called 
        				  // regularly (such as once every 1.8ms or faster** [see 
        				  // inline code comments in usb_device.c for explanation when
        				  // "or faster" applies])  In most cases, the USBDeviceTasks() 
        				  // function does not take very long to execute (ex: <100 
        				  // instruction cycles) before it returns.
        #endif
		// Application-specific tasks.
		// Application related code may be added here, or in the ProcessIO() function.
        ProcessIO();
    }//end while
}//end main


/********************************************************************
 * Function:        static void InitializeSystem(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        InitializeSystem is a centralize initialization
 *                  routine. All required USB initialization routines
 *                  are called from here.
 *
 *                  User application initialization routine should
 *                  also be called from here.                  
 *
 * Note:            None
 *******************************************************************/
static void InitializeSystem(void)
{
    #if defined(_PIC14E)
        ANSELA = 0x00;
        ANSELB = 0x00;
        ANSELC = 0x00;
        TRISA  = 0x00;
        TRISB  = 0x00;
        TRISC  = 0x00;
        OSCTUNE = 0;
        #if defined (USE_INTERNAL_OSC)
            OSCCON = 0x7C;   // PLL enabled, 3x, 16MHz internal osc, SCS external
            OSCCONbits.SPLLMULT = 1;   // 1=3x, 0=4x
            ACTCON = 0x90;   // Clock recovery on, Clock Recovery enabled; SOF packet
        #else
            OSCCON = 0x3C;   // PLL enabled, 3x, 16MHz internal osc, SCS external
            OSCCONbits.SPLLMULT = 0;   // 1=3x, 0=4x
            ACTCON = 0x00;   // Clock recovery off, Clock Recovery enabled; SOF packet
        #endif
    #endif

    #if (defined(__18CXX) & !defined(PIC18F87J50_PIM) & !defined(PIC18F97J94_FAMILY))
        ADCON1 |= 0x0F;                 // Default all pins to digital
    #elif defined(__C30__) || defined __XC16__
    	#if defined(__PIC24FJ256DA210__) || defined(__PIC24FJ256GB210__)
    		ANSA = 0x0000;
    		ANSB = 0x0000;
    		ANSC = 0x0000;
    		ANSD = 0x0000;
    		ANSE = 0x0000;
    		ANSF = 0x0000;
    		ANSG = 0x0000;
     #elif defined(__dsPIC33EP512MU810__) || defined (__PIC24EP512GU810__)
        	ANSELA = 0x0000;
    		ANSELB = 0x0000;
    		ANSELC = 0x0000;
    		ANSELD = 0x0000;
    		ANSELE = 0x0000;
    		ANSELG = 0x0000;
            
            // The dsPIC33EP512MU810 features Peripheral Pin
            // select. The following statements map UART2 to 
            // device pins which would connect to the the 
            // RX232 transciever on the Explorer 16 board.

             RPINR19 = 0;
             RPINR19 = 0x64;
             RPOR9bits.RP101R = 0x3;

        #else
        	AD1PCFGL = 0xFFFF;
        #endif        
    #elif defined(__C32__)
        AD1PCFG = 0xFFFF;
    #endif

    #if defined(PIC18F87J50_PIM) || defined(PIC18F46J50_PIM) || defined(PIC18F_STARTER_KIT_1) || defined(PIC18F47J53_PIM)
    	//On the PIC18F87J50 Family of USB microcontrollers, the PLL will not power up and be enabled
    	//by default, even if a PLL enabled oscillator configuration is selected (such as HS+PLL).
    	//This allows the device to power up at a lower initial operating frequency, which can be
    	//advantageous when powered from a source which is not gauranteed to be adequate for 48MHz
    	//operation.  On these devices, user firmware needs to manually set the OSCTUNE<PLLEN> bit to
    	//power up the PLL.
        {
            unsigned int pll_startup_counter = 600;
            OSCTUNEbits.PLLEN = 1;  //Enable the PLL and wait 2+ms until the PLL locks before enabling USB module
            while(pll_startup_counter--);
        }
        //Device switches over automatically to PLL output after PLL is locked and ready.
    #endif

    #if defined(PIC18F87J50_PIM)
    	//Configure all I/O pins to use digital input buffers.  The PIC18F87J50 Family devices
    	//use the ANCONx registers to control this, which is different from other devices which
    	//use the ADCON1 register for this purpose.
        WDTCONbits.ADSHR = 1;			// Select alternate SFR location to access ANCONx registers
        ANCON0 = 0xFF;                  // Default all pins to digital
        ANCON1 = 0xFF;                  // Default all pins to digital
        WDTCONbits.ADSHR = 0;			// Select normal SFR locations
    #endif

    #if defined(PIC18F97J94_FAMILY)
        //Configure I/O pins for digital input mode.
        ANCON1 = 0xFF;
        ANCON2 = 0xFF;
        ANCON3 = 0xFF;
        #if(USB_SPEED_OPTION == USB_FULL_SPEED)
            //Enable INTOSC active clock tuning if full speed
            ACTCON = 0x90; //Enable active clock self tuning for USB operation
            while(OSCCON2bits.LOCK == 0);   //Make sure PLL is locked/frequency is compatible
                                            //with USB operation (ex: if using two speed 
                                            //startup or otherwise performing clock switching)
        #endif
    #endif
    
    #if defined(PIC18F45K50_FAMILY)
        //Configure oscillator settings for clock settings compatible with USB 
        //operation.  Note: Proper settings depends on USB speed (full or low).
        #if(USB_SPEED_OPTION == USB_FULL_SPEED)
            OSCTUNE = 0x80; //3X PLL ratio mode selected
            OSCCON = 0x70;  //Switch to 16MHz HFINTOSC
            OSCCON2 = 0x10; //Enable PLL, SOSC, PRI OSC drivers turned off
            while(OSCCON2bits.PLLRDY != 1);   //Wait for PLL lock
            *((unsigned char*)0xFB5) = 0x90;  //Enable active clock tuning for USB operation
        #endif
        //Configure all I/O pins for digital mode (except RA0/AN0 which has POT on demo board)
        ANSELA = 0x03;  // HK USB_IO demo  ra0 and ra1 are ADC
        ANSELB = 0x00;
        ANSELC = 0x00;
        ANSELD = 0x00;
        ANSELE = 0x00;
    #endif
    
    #if defined(__32MX460F512L__)|| defined(__32MX795F512L__)
    // Configure the PIC32 core for the best performance
    // at the operating frequency. The operating frequency is already set to 
    // 60MHz through Device Config Registers
    SYSTEMConfigPerformance(60000000);
	#endif

  #if defined(__dsPIC33EP512MU810__) || defined (__PIC24EP512GU810__)

    // Configure the device PLL to obtain 60 MIPS operation. The crystal
    // frequency is 8MHz. Divide 8MHz by 2, multiply by 60 and divide by
    // 2. This results in Fosc of 120MHz. The CPU clock frequency is
    // Fcy = Fosc/2 = 60MHz. Wait for the Primary PLL to lock and then
    // configure the auxilliary PLL to provide 48MHz needed for USB 
    // Operation.

	PLLFBD = 38;				/* M  = 60	*/
	CLKDIVbits.PLLPOST = 0;		/* N1 = 2	*/
	CLKDIVbits.PLLPRE = 0;		/* N2 = 2	*/
	OSCTUN = 0;			

    /*	Initiate Clock Switch to Primary
     *	Oscillator with PLL (NOSC= 0x3)*/
	
    __builtin_write_OSCCONH(0x03);		
	__builtin_write_OSCCONL(0x01);
	while (OSCCONbits.COSC != 0x3);       

    // Configuring the auxiliary PLL, since the primary
    // oscillator provides the source clock to the auxiliary
    // PLL, the auxiliary oscillator is disabled. Note that
    // the AUX PLL is enabled. The input 8MHz clock is divided
    // by 2, multiplied by 24 and then divided by 2. Wait till 
    // the AUX PLL locks.

    ACLKCON3 = 0x24C1;   
    ACLKDIV3 = 0x7;
    ACLKCON3bits.ENAPLL = 1;
    while(ACLKCON3bits.APLLCK != 1); 

    #endif

    #if defined(PIC18F46J50_PIM) || defined(PIC18F_STARTER_KIT_1) || defined(PIC18F47J53_PIM)
	//Configure all I/O pins to use digital input buffers.  The PIC18F87J50 Family devices
	//use the ANCONx registers to control this, which is different from other devices which
	//use the ADCON1 register for this purpose.
    ANCON0 = 0x7F;                  // All pins to digital (except AN7: temp sensor)
    ANCON1 = 0xBF;                  // Default all pins to digital.  Bandgap on.

    #endif
    
   #if defined(PIC24FJ64GB004_PIM) || defined(PIC24FJ256DA210_DEV_BOARD)
	//On the PIC24FJ64GB004 Family of USB microcontrollers, the PLL will not power up and be enabled
	//by default, even if a PLL enabled oscillator configuration is selected (such as HS+PLL).
	//This allows the device to power up at a lower initial operating frequency, which can be
	//advantageous when powered from a source which is not gauranteed to be adequate for 32MHz
	//operation.  On these devices, user firmware needs to manually set the CLKDIV<PLLEN> bit to
	//power up the PLL.
    {
        unsigned int pll_startup_counter = 600;
        CLKDIVbits.PLLEN = 1;
        while(pll_startup_counter--);
    }

    //Device switches over automatically to PLL output after PLL is locked and ready.
    #endif


//	The USB specifications require that USB peripheral devices must never source
//	current onto the Vbus pin.  Additionally, USB peripherals should not source
//	current on D+ or D- when the host/hub is not actively powering the Vbus line.
//	When designing a self powered (as opposed to bus powered) USB peripheral
//	device, the firmware should make sure not to turn on the USB module and D+
//	or D- pull up resistor unless Vbus is actively powered.  Therefore, the
//	firmware needs some means to detect when Vbus is being powered by the host.
//	A 5V tolerant I/O pin can be connected to Vbus (through a resistor), and
// 	can be used to detect when Vbus is high (host actively powering), or low
//	(host is shut down or otherwise not supplying power).  The USB firmware
// 	can then periodically poll this I/O pin to know when it is okay to turn on
//	the USB module/D+/D- pull up resistor.  When designing a purely bus powered
//	peripheral device, it is not possible to source current on D+ or D- when the
//	host is not actively providing power on Vbus. Therefore, implementing this
//	bus sense feature is optional.  This firmware can be made to use this bus
//	sense feature by making sure "USE_USB_BUS_SENSE_IO" has been defined in the
//	HardwareProfile.h file.    
    #if defined(USE_USB_BUS_SENSE_IO)
    tris_usb_bus_sense = INPUT_PIN; // See HardwareProfile.h
    #endif
    
//	If the host PC sends a GetStatus (device) request, the firmware must respond
//	and let the host know if the USB peripheral device is currently bus powered
//	or self powered.  See chapter 9 in the official USB specifications for details
//	regarding this request.  If the peripheral device is capable of being both
//	self and bus powered, it should not return a hard coded value for this request.
//	Instead, firmware should check if it is currently self or bus powered, and
//	respond accordingly.  If the hardware has been configured like demonstrated
//	on the PICDEM FS USB Demo Board, an I/O pin can be polled to determine the
//	currently selected power source.  On the PICDEM FS USB Demo Board, "RA2" 
//	is used for	this purpose.  If using this feature, make sure "USE_SELF_POWER_SENSE_IO"
//	has been defined in HardwareProfile - (platform).h, and that an appropriate I/O pin 
//  has been mapped	to it.
    #if defined(USE_SELF_POWER_SENSE_IO)
    tris_self_power = INPUT_PIN;	// See HardwareProfile.h
    #endif

    UserInit();
    
    USBDeviceInit();	//usb_device.c.  Initializes USB module SFRs and firmware
    					//variables to known states.
}//end InitializeSystem



/******************************************************************************
 * Function:        void UserInit(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This routine should take care of all of the demo code
 *                  initialization that is required.
 *
 * Note:            
 *
 *****************************************************************************/
void UserInit(void)
{
    //Initialize all of the LED pins
    mInitAllLEDs();
    
    //Initialize all of the push buttons
    mInitAllSwitches();

    // HK USB_IO demo
    // HK_GPIO default RD4,RD5=input, RD6,RD7=output
    TRISDbits.TRISD4=1;       //HK set for input RD4
    TRISDbits.TRISD5=1;       //HK set for input RD5
    TRISDbits.TRISD6=0;       //HK set for output RD6
    TRISDbits.TRISD7=0;       //HK set for output RD7
#define gprd4   PORTDbits.RD4
#define gprd5   PORTDbits.RD5
#define gprd6   PORTDbits.RD6
#define gprd7   PORTDbits.RD7

Open1USART( USART_TX_INT_OFF &
    USART_RX_INT_OFF &
    USART_ASYNCH_MODE &
    USART_EIGHT_BIT &
    USART_CONT_RX &
    USART_BRGH_HIGH &
    USART_ADDEN_OFF,
    77 );  // 48M / (64*9600) = spbrg+1 = 78.125, so spbrg = 77

    //Initialize I/O pin and ADC settings to collect potentiometer measurements
    mInitPOT();   // adc on RDA0
    mInitPOT1();  // HK USB_IO demo, adc on RDA1
    
    //initialize the variable holding the handle for the last
    // transmission
    USBOutHandle = 0;
    USBInHandle = 0;

#if defined(HARDKERNEL_PIC18F45K50)
    blinkStatusValid = FALSE;
#else
    blinkStatusValid = TRUE;
#endif
}//end UserInit

/********************************************************************
 * Function:        void ProcessIO(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is a place holder for other user
 *                  routines. It is a mixture of both USB and
 *                  non-USB tasks.
 *
 * Note:            None
 *******************************************************************/
void ProcessIO(void)
{   
    char gpin;
    unsigned char *tbuf;
    int n;
    //Blink the LEDs according to the USB device status
    if(blinkStatusValid)
    {
        BlinkUSBStatus();
    }

    // User Application USB tasks
    if((USBDeviceState < CONFIGURED_STATE)||(USBSuspendControl==1)) return;
    
    //Check if we have received an OUT data packet from the host
    if(!HIDRxHandleBusy(USBOutHandle))				
    {   
        //We just received a packet of data from the USB host.
        //Check the first byte of the packet to see what command the host 
        //application software wants us to fulfill.
        switch(ReceivedDataBuffer[0])				//Look at the data the host sent, to see what kind of application specific command it sent.
        {
            case 0x40:  // void i2c_init(void)
                i2c_init();
                break;
            case 0x41:  // void i2c_idle(void)
                i2c_idle();
                break;
            case 0x42:  // void i2c_start(uchar)
                i2c_start(ReceivedDataBuffer[1]);   // I2C_START_CMD or other for restart_i2c
                break;
            case 0x43:  // void i2c_stop(void)
                i2c_stop();
                break;
            case 0x44:  // unsigned char  i2c_slave_ack(void)
                ToSendDataBuffer[1] = i2c_slave_ack(); // 1=no ack, 0=ack
                ToSendDataBuffer[0] = 0x44;     // echo back cmd
                if(!HIDTxHandleBusy(USBInHandle)) {
                    USBInHandle = HIDTxPacket(HID_EP,(BYTE*)&ToSendDataBuffer[0],64);
                }
                break;
            case 0x45:  // void i2c_write(unsigned char data)
                i2c_write(ReceivedDataBuffer[1]);
                break;
            case 0x46:  // void i2c_master_ack(unsigned char ack_type)
                i2c_master_ack(ReceivedDataBuffer[1]); // 1=nack,0=ack
                break;
            case 0x47:  // unsigned char i2c_read(void)
                ToSendDataBuffer[1] = i2c_read();
                ToSendDataBuffer[0] = 0x47;
                if(!HIDTxHandleBusy(USBInHandle)) {
                    USBInHandle = HIDTxPacket(HID_EP,(BYTE*)&ToSendDataBuffer[0],64);
                }
                break;
            case 0x48:  // unsigned char i2c_isdatardy(void)
                ToSendDataBuffer[1] = i2c_isdatardy();
                ToSendDataBuffer[0] = 0x48;
                if(!HIDTxHandleBusy(USBInHandle)) {
                    USBInHandle = HIDTxPacket(HID_EP,(BYTE*)&ToSendDataBuffer[0],64);
                }
                break;
                // reserve space for I2C block functions
            case 0x50:  // void spi_init(mode, baud, sample)
                spi_init(ReceivedDataBuffer[1],ReceivedDataBuffer[2], ReceivedDataBuffer[3]);
                break;
            case 0x51:  // unsigned char spi_transfer(unsigned char addr)
                ToSendDataBuffer[1] = spi_transfer(ReceivedDataBuffer[1]);
                ToSendDataBuffer[0] = 0x51;
                if(!HIDTxHandleBusy(USBInHandle)) {
                    USBInHandle = HIDTxPacket(HID_EP,(BYTE*)&ToSendDataBuffer[0],64);
                }
                break;
            case 0x52:  // chip select:  void spi_cs(1 or 0)  0-enables
                spi_cs(ReceivedDataBuffer[1]);
                break;
            case 0x80:  //Toggle LEDs command
		        blinkStatusValid = FALSE;			//Stop blinking the LEDs automatically, going to manually control them now.
                if(mGetLED_1() == mGetLED_2())
                {
                    mLED_1_Toggle();
                    mLED_2_Toggle();
                }
                else
                {
                    if(mGetLED_1())
                    {
                        mLED_2_On();
                    }
                    else
                    {
                        mLED_2_Off();
                    }
                }
                break;
            case 0x81:  //Get push button state
                //Check to make sure the endpoint/buffer is free before we modify the contents
                if(!HIDTxHandleBusy(USBInHandle))
                {
                    ToSendDataBuffer[0] = 0x81;				//Echo back to the host PC the command we are fulfilling in the first byte.  In this case, the Get Pushbutton State command.
    				if(sw3 == 1)							//pushbutton not pressed, pull up resistor on circuit board is pulling the PORT pin high
    				{
    					ToSendDataBuffer[1] = 0x01;			
    				}
    				else									//sw3 must be == 0, pushbutton is pressed and overpowering the pull up resistor
    				{
    					ToSendDataBuffer[1] = 0x00;
    				}
    				//Prepare the USB module to send the data packet to the host
                    USBInHandle = HIDTxPacket(HID_EP,(BYTE*)&ToSendDataBuffer[0],64);
                }
                break;
            case 0x82:  // Get input value for supplied pin#
                // where [1] is pin:  1=gprd4,2=gprd5,3=gprd6,4=gprd7
                if (!HIDTxHandleBusy(USBInHandle))
                {
                    ToSendDataBuffer[0] = 0x82; // echo cmd
                    if (ReceivedDataBuffer[1] == 1) { gpin = gprd4; }
                    if (ReceivedDataBuffer[1] == 2) { gpin = gprd5; }
                    if (ReceivedDataBuffer[1] == 3) { gpin = gprd6; }
                    if (ReceivedDataBuffer[1] == 4) { gpin = gprd7; }
                    if (gpin == 1) { ToSendDataBuffer[1] = 0x01;}
                    else           { ToSendDataBuffer[1] = 0;}
                    ToSendDataBuffer[2] = 0;    // msb
                    //Prepare the USB module to send the data packet to the host
	            USBInHandle = HIDTxPacket(HID_EP,(BYTE*)&ToSendDataBuffer[0],64);
                }
                break;
            case 0x83:  // ouptut a value on supplied pin#
                if (ReceivedDataBuffer[2] == 1) { gpin = 1;}
                else
                if (ReceivedDataBuffer[2] == 0) { gpin = 0;}
                else break;
                switch (ReceivedDataBuffer[1]) {
                    case 1: LATDbits.LATD4 = gpin;
                    break;
                    case 2: LATDbits.LATD5 = gpin;
                    break;
                    case 3: LATDbits.LATD6 = gpin;
                    break;
                    case 4: LATDbits.LATD7 = gpin;
                    break;
                    default: break;
                }
                break;
            case 0x84:  // configure GPIO port direction, 1=input, 0=output
                if (ReceivedDataBuffer[2] == 1) { gpin = 1;} // GPIO is input
                else
                if (ReceivedDataBuffer[2] == 0) { gpin = 0;} // GPIO is output
                else break;
                switch (ReceivedDataBuffer[1]) {
                    case 1: TRISDbits.TRISD4 = gpin;
                    break;
                    case 2: TRISDbits.TRISD5 = gpin;
                    break;
                    case 3: TRISDbits.TRISD6 = gpin;
                    break;
                    case 4: TRISDbits.TRISD7 = gpin;
                    break;
                    default: break;
                }
                break;
            case 0x85:  // return ROM version string to caller
      	                if(!HIDTxHandleBusy(USBInHandle))
	                {
                            ToSendDataBuffer[0] = 0x85;   // echo back cmd
                            ToSendDataBuffer[1] = version[0];
                            ToSendDataBuffer[2] = version[1];
                            ToSendDataBuffer[3] = version[2];
                            USBInHandle = HIDTxPacket(HID_EP,(BYTE*)&ToSendDataBuffer[0],64);
                        }
                break;
            case 0x86: // send char string out UART
            //    while (Busy1USART());  // Odd, puts adds extra chars
            //    puts1USART((char *) &ReceivedDataBuffer[1]);
                n=1;
                do { while(Busy1USART());putc1USART(ReceivedDataBuffer[n]);n++;}
                while (ReceivedDataBuffer[n]);
                break;
            case 0x87: // test if UART has char available to read
                if(!HIDTxHandleBusy(USBInHandle))
                {
                    ToSendDataBuffer[0] = 0x87;  // echo back cmd
                    if (DataRdy1USART()) { ToSendDataBuffer[1] = 1;}
                    else { ToSendDataBuffer[1] = 0;}
                    USBInHandle = HIDTxPacket(HID_EP,(BYTE*)&ToSendDataBuffer[0],64);
                }
                break;
            case 0x88: // read a single char from UART
                if(!HIDTxHandleBusy(USBInHandle))
                {
                    ToSendDataBuffer[0] = 0x88;  // echo back cmd
                    if (DataRdy1USART()) {
                        ToSendDataBuffer[1] = 1;
                        ToSendDataBuffer[2] = Read1USART();
                    } else {
                        ToSendDataBuffer[1] = 0;  // no char was available
                    }
                    USBInHandle = HIDTxPacket(HID_EP,(BYTE*)&ToSendDataBuffer[0],64);
                }
                break;
            case 0x89:  // send a single char to UART
                 // while (Busy1USART());
                 Write1USART(ReceivedDataBuffer[1]);
                break;
            case 0x37:	//Read POT command.  Uses ADC to measure an analog voltage on one of the ANxx I/O pins, and returns the result to the host
                {
                    WORD_VAL w;
			ADCON0=0x01;  // 0000 in CHS bits select A0 pin for ADC input channel [0000 01]
                    //Check to make sure the endpoint/buffer is free before we modify the contents
	                if(!HIDTxHandleBusy(USBInHandle))
	                {
	                    w = ReadPOT();					//Use ADC to read the I/O pin voltage.  See the relevant HardwareProfile - xxxxx.h file for the I/O pin that it will measure.
														//Some demo boards, like the PIC18F87J50 FS USB Plug-In Module board, do not have a potentiometer (when used stand alone).
														//This function call will still measure the analog voltage on the I/O pin however.  To make the demo more interesting, it
														//is suggested that an external adjustable analog voltage should be applied to this pin.
						ToSendDataBuffer[0] = 0x37;  	//Echo back to the host the command we are fulfilling in the first byte.  In this case, the Read POT (analog voltage) command.
						ToSendDataBuffer[1] = w.v[0];  	//Measured analog voltage LSB
						ToSendDataBuffer[2] = w.v[1];  	//Measured analog voltage MSB

                        //Prepare the USB module to send the data packet to the host
	                    USBInHandle = HIDTxPacket(HID_EP,(BYTE*)&ToSendDataBuffer[0],64);
	                }					
                }
                break;
            case 0x38:	//Read RDA1 ADC value
                {
                    WORD_VAL wv;
                        ADCON0=0x05;  // 0001 in CHS bits select A1 pin for ADC input channel [0001 01]
                        if(!HIDTxHandleBusy(USBInHandle))
	                {
                            wv = ReadPOT1();                 //Use ADC to read the I/O pin voltage.  See the relevant HardwareProfile - xxxxx.h file for the I/O pin that it will measure.
                            ToSendDataBuffer[0] = 0x38;      //Echo back to the host the command we are fulfilling in the first byte.  In this case, the Read POT (analog voltage) command.
                            ToSendDataBuffer[1] = wv.v[0];   //Measured analog voltage LSB
                            ToSendDataBuffer[2] = wv.v[1];   //Measured analog voltage MSB
                            //Prepare the USB module to send the data packet to the host
                            USBInHandle = HIDTxPacket(HID_EP,(BYTE*)&ToSendDataBuffer[0],64);
	                }
                }
                break;
#if defined(HARDKERNEL_PIC18F45K50)
            case 0x98: //- Get Register
                {
					unsigned char R_addr, re_val;

					R_addr = ReceivedDataBuffer[10];
					
					re_val = rw_Register(R_addr, 0, 0);

	                if(!HIDTxHandleBusy(USBInHandle))
	                {
						ToSendDataBuffer[0] = 0x98;
						ToSendDataBuffer[1] = re_val;
                                                
	                    USBInHandle = HIDTxPacket(HID_EP,(BYTE*)&ToSendDataBuffer[0],64);
	                }					
				}
				break;

			case 0x99: //- Set Register
				{
					unsigned char R_addr, val, re_val;

					R_addr = ReceivedDataBuffer[10];
					val = ReceivedDataBuffer[11];

					re_val = rw_Register(R_addr, 1, val);

	                if(!HIDTxHandleBusy(USBInHandle))
	                {
						ToSendDataBuffer[0] = 0x99;
						ToSendDataBuffer[1] = re_val;

	                    USBInHandle = HIDTxPacket(HID_EP,(BYTE*)&ToSendDataBuffer[0],64);
	                }					
				}
				break;

			case 0x9A: //- Get Register Bit
                {
					unsigned char R_addr, R_bit, re_val;

					R_addr = ReceivedDataBuffer[10];
					R_bit = ReceivedDataBuffer[11];
					
					re_val = rw_Register_bit(R_addr, R_bit, 0, 0);

	                if(!HIDTxHandleBusy(USBInHandle))
	                {
						ToSendDataBuffer[0] = 0x9A;
						ToSendDataBuffer[1] = re_val;

	                    USBInHandle = HIDTxPacket(HID_EP,(BYTE*)&ToSendDataBuffer[0],64);
	                }					
				}
				break;

			case 0x9B: //- Set Register Bit
				{
					unsigned char R_addr, R_bit, val, re_val;

					R_addr = ReceivedDataBuffer[10];
					R_bit = ReceivedDataBuffer[11];
					val = ReceivedDataBuffer[12];

					re_val = rw_Register_bit(R_addr, R_bit, 1, val);

	                if(!HIDTxHandleBusy(USBInHandle))
	                {
						ToSendDataBuffer[0] = 0x9B;
						ToSendDataBuffer[1] = re_val;

	                    USBInHandle = HIDTxPacket(HID_EP,(BYTE*)&ToSendDataBuffer[0],64);
	                }					
				}
				break;
#endif
        }
        //Re-arm the OUT endpoint, so we can receive the next OUT data packet 
        //that the host may try to send us.
        USBOutHandle = HIDRxPacket(HID_EP, (BYTE*)&ReceivedDataBuffer, 64);
    }

    
}//end ProcessIO

/******************************************************************************
 * Function:        WORD_VAL ReadPOT(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          WORD_VAL - the 10-bit right justified POT value
 *
 * Side Effects:    ADC buffer value updated
 *
 * Overview:        This function reads the POT and leaves the value in the 
 *                  ADC buffer register
 *
 * Note:            None
 *****************************************************************************/
// HK USB_IO demo, new routine to read RA1  ADC
WORD_VAL ReadPOT1(void)  // adds another ADC channel on RA1
{
    WORD_VAL wv;
    wv.Val = 0;
        ADCON0bits.GO = 1;              // Start AD conversion
            while(ADCON0bits.GO);     // Wait for conversion
            wv.v[0] = ADRESL;
            wv.v[1] = ADRESH;
    return wv;
} // end ReadPOT1

WORD_VAL ReadPOT(void)
{
    WORD_VAL w;

    w.Val = 0;

    #if defined(__18CXX) || defined(__XC8)
        #if defined(PIC18F97J94_PIM) 
            ADCON1Lbits.SAMP = 1;           // Start AD sampling/convert sequence
            while(ADCON1Lbits.DONE == 0);   // Wait for result complete
            w.v[0] = ADCBUF3L;
            w.v[1] = ADCBUF3H;       
        #elif defined(PIC18F87J94_PIM)
            ADCON1Lbits.SAMP = 1;           // Start AD sampling/convert sequence
            while(ADCON1Lbits.DONE == 0);   // Wait for result complete
            w.v[0] = ADCBUF0L;
            w.v[1] = ADCBUF0H;       
        #else
            ADCON0bits.GO = 1;              // Start AD conversion
            while(ADCON0bits.GO);     // Wait for conversion
            w.v[0] = ADRESL;
            w.v[1] = ADRESH;
        #endif

    #elif defined(__C30__) || defined(__C32__) || defined __XC16__
        #if defined(PIC24FJ256GB110_PIM) || \
            defined(PIC24FJ256DA210_DEV_BOARD) || \
            defined(PIC24FJ256GB210_PIM)
            AD1CHS = 0x5;           //MUXA uses AN5

            // Get an ADC sample
            AD1CON1bits.SAMP = 1;           //Start sampling
            for(w.Val=0;w.Val<1000;w.Val++); //Sample delay, conversion start automatically
            AD1CON1bits.SAMP = 0;           //Start sampling
            for(w.Val=0;w.Val<1000;w.Val++); //Sample delay, conversion start automatically
            while(!AD1CON1bits.DONE);       //Wait for conversion to complete

        #elif defined(DSPIC33EP512MU810_PIM) || defined (PIC24EP512GU810_PIM)
            
            // Routine to read the Explorer 16 potentiometer.
              // Get an ADC sample
            AD1CHS0bits.CH0SA = 5;
            AD1CON1bits.SAMP = 1;           //Start sampling
            for(w.Val=0;w.Val<1000;w.Val++); //Sample delay, conversion start automatically
            AD1CON1bits.SAMP = 0;           //Start sampling
            for(w.Val=0;w.Val<1000;w.Val++); //Sample delay, conversion start automatically
            while(!AD1CON1bits.DONE);       //Wait for conversion to complete
        
        #elif defined (DSPIC33E_USB_STARTER_KIT)
        
            w.Val = 0;
        
        #elif defined(PIC24FJ64GB004_PIM) || defined(PIC24FJ64GB502_MICROSTICK)
            AD1CHS = 0x7;           //MUXA uses AN7

            // Get an ADC sample
            AD1CON1bits.SAMP = 1;           //Start sampling
            for(w.Val=0;w.Val<1000;w.Val++); //Sample delay, conversion start automatically
            AD1CON1bits.SAMP = 0;           //Start sampling
            for(w.Val=0;w.Val<1000;w.Val++); //Sample delay, conversion start automatically
            while(!AD1CON1bits.DONE);       //Wait for conversion to complete

        #elif defined(PIC24F_STARTER_KIT)
            AD1CHS = 0x0;           //MUXA uses AN0

            // Get an ADC sample
            AD1CON1bits.SAMP = 1;           //Start sampling
            for(w.Val=0;w.Val<1000;w.Val++); //Sample delay, conversion start automatically
            AD1CON1bits.SAMP = 0;           //Start sampling
            for(w.Val=0;w.Val<1000;w.Val++); //Sample delay, conversion start automatically
            while(!AD1CON1bits.DONE);       //Wait for conversion to complete

        #elif defined(PIC32MX460F512L_PIM) || defined(PIC32_USB_STARTER_KIT) || defined(PIC32MX795F512L_PIM)
            AD1PCFG = 0xFFFB; // PORTB = Digital; RB2 = analog
            AD1CON1 = 0x0000; // SAMP bit = 0 ends sampling ...
            // and starts converting
            AD1CHS = 0x00020000; // Connect RB2/AN2 as CH0 input ..
            // in this example RB2/AN2 is the input
            AD1CSSL = 0;
            AD1CON3 = 0x0002; // Manual Sample, Tad = internal 6 TPB
            AD1CON2 = 0;
            AD1CON1SET = 0x8000; // turn ADC ON

            AD1CON1SET = 0x0002; // start sampling ...
            for(w.Val=0;w.Val<1000;w.Val++); //Sample delay, conversion start automatically
            AD1CON1CLR = 0x0002; // start Converting
            while (!(AD1CON1 & 0x0001));// conversion done?
        #else
            #error
        #endif

        w.Val = ADC1BUF0;

    #endif

    return w;
}//end ReadPOT

/********************************************************************/
/* I2C routines HK USB_IO demo */
// I2C Bus Control Definition
#define I2C_DATA_ACK 0
#define I2C_DATA_NOACK 1
#define I2C_WRITE_CMD 0
#define I2C_READ_CMD 1

#define I2C_START_CMD 0
#define I2C_REP_START_CMD 1
#define I2C_REQ_ACK 0
#define I2C_REQ_NOACK 0
void i2c_init(void) {
    TRISBbits.TRISB0 = 1;  //ports RB0=sda, RB1=scl are input
    TRISBbits.TRISB1 = 1;
  // Initialize the PIC18F45K50 MSSP Peripheral I2C Master Mode (100khz = 0x77)
  // SSPADD + 1 = 48m/(400k*4) = 30 so SPPADD=29 or 0x1D
                                // for 100k @ 48mhz = 0x77  (119)
  //SSP1STAT = 0x80;      // Slew Rate is disable for 100 kHz mode
  //SSP1CON1 = 0x28;      // Enable SDA and SCL, I2C Master mode, clock = FOSC/(4 * (SSPADD + 1))
  //SSP1CON2 = 0x00;      // Reset MSSP Control Register
    SSP1STAT &= 0x3f;     // ==== possibly these are better for our PIC
    SSP1CON1 = 0x00;
    SSP1CON2 = 0x00;
    SSP1CON1 |= 0x08;
    SSP1STAT |= 0x80;
    SSP1CON1 |= 0x20;      // ==== if not can revert to above 3 commented out
  //SSP1ADD = 0x77;       // 100 kHz
    SSP1ADD = 0x1d;       // 400 kHz
  PIR1bits.SSPIF=0;    // Clear MSSP Interrupt Flag
}

void i2c_idle(void)
{
  // Wait I2C Bus and Status Idle (i.e. ACKEN, RCEN, PEN, RSEN, SEN)
  while (( SSP1CON2 & 0x1F ) || ( SSP1STATbits.R_NOT_W));
}

void i2c_start(unsigned char stype)
{
  i2c_idle();                     // Ensure the I2C module is idle
  if (stype == I2C_START_CMD) {
    SSP1CON2bits.SEN = 1;          // Start I2C Transmission
    while(SSP1CON2bits.SEN);
  } else {
    SSP1CON2bits.RSEN = 1;         // ReStart I2C Transmission
    while(SSP1CON2bits.RSEN);
  }
}

void i2c_stop(void)
{
  // Stop I2C Transmission
  SSP1CON2bits.PEN = 1;
  while(SSP1CON2bits.PEN);
}
unsigned char i2c_slave_ack(void)
{
  // Return: 1 = Acknowledge was not received from slave
  //         0 = Acknowledge was received from slave
  return(SSP1CON2bits.ACKSTAT);
}

void i2c_write(unsigned char data)
{
  // Send the Data to I2C Bus
  SSP1BUF = data;
  if (SSP1CON1bits.WCOL)         // Check for write collision
    return;
  while(SSP1STATbits.BF);        // Wait until write cycle is complete
  i2c_idle();                   // Ensure the I2C module is idle
}

void i2c_master_ack(unsigned char ack_type)
{
  SSP1CON2bits.ACKDT = ack_type;   // 1 = Not Acknowledge, 0 = Acknowledge
  SSP1CON2bits.ACKEN = 1;          // Enable Acknowledge
  while (SSP1CON2bits.ACKEN == 1);
}

unsigned char i2c_read(void)
{
  // Ensure the I2C module is idle
  i2c_idle();
  // Enable Receive Mode
  SSP1CON2bits.RCEN = 1;           // Enable master for 1 byte reception
  while(!SSP1STATbits.BF);         // Wait until buffer is full
  return(SSP1BUF);
}

unsigned char i2c_isdatardy(void)
{
    if ( SSP1STATbits.BF) return ( +1);
    else return ( 0 );
}
/************************************************
 SPI routines HK USB_IO */
/* notes:  RB7 = SDO, RB0 = SDI,  RB1 = SCLK, RB5 = CS (on python side) */
void spi_init(unsigned char mode, unsigned char baud, unsigned char sample)
{
    int baud_bits;
    // initialize the SPI peripheral
    // SSP1CON1--->SSPM<3:0>
    // bit 3-0 SSPM<3:0>: Synchronous Serial Port Mode Select bits
    // 0000 = SPI Master mode, clock = FOSC/4  = 12mhz
    // 0001 = SPI Master mode, clock = FOSC/16 = 3mhz
    // 0010 = SPI Master mode, clock = FOSC/64 = 750khz
    if (baud == 0) {
        baud_bits = 0b00000010;
    }
    else if (baud == 1) {
        baud_bits = 0b00000001;
    }
    else if (baud == 2) {
        baud_bits = 0b00000000;
    }
    else {
        baud_bits = 0b00000010;
    }
    ANSELBbits.ANSB1 = 0;
    ANSELBbits.ANSB0 = 0;
    ANSELAbits.ANSA5 = 0;
    SSP1CON1 = 0x00;    // reset serial
    TRISCbits.TRISC7 = 0;  // init code does RC7 for SDO
    TRISAbits.TRISA5 = 0;   // RA5/SS - output (chip select)
    TRISBbits.TRISB0 = 1;   // RB0/SDI - input (serial data in)
    TRISBbits.TRISB1 = 0;   // RB1/SCK - output (clock)
    // TRISBbits.TRISB3 = 0;  // RB3=SDO output (init code does not set this up)
    // clock polarity controlled by: CKP bit of SSP1CON1 and CKE bit of SSP1STAT
    // SSP1CON1 - CKP bit = bit4
    // SSP1STAT - CKE bit = bit6  (clock polarity control with these 2)
    // m0(CKP=0,CKE=0) m1(CKP=1,CKE=0) m2(CKP=0,CKE=1 m3(CKP=1,CKE=1)
    // SSP1CON1 -> bit5 stays 1 to enable MSSP
    // SSP1CON1 [WCOL|SSPOV|SSPEN|CKP| SSPM<3:0>]
    // SSP1STAT [SMP|CKE|<---na-->|BF]
    switch (mode)
    {
        case 0:
            SSP1STAT = 0x40;             // CKE=1, CKP=0                 (mode0)
            SSP1CON1 = 0x00 | baud_bits; // enable SPI master with Fosc/16 = 3mhz
            break;
        case 1:
            SSP1STAT = 0x00;             // CKE=0, CKP=0                 (mode1)
            SSP1CON1 = 0x00 | baud_bits; // enable SPI master with Fosc/16 = 3mhz
            break;
        case 2:
            SSP1STAT = 0x40;             // CKE=1, CKP=1                 (mode2)
            SSP1CON1 = 0x10 | baud_bits; // enable SPI master with Fosc/16 = 3mhz
            break;
        case 3:
            SSP1STAT = 0x00;             // CKE=0, CKP=1                 (mode3)
            SSP1CON1 = 0x10 | baud_bits; // enable SPI master with Fosc/16 = 3mhz
            break;
        default:        //mode 2
            SSP1STAT = 0x40;             // set SMP=0 and CKE=1, CKP=0   (mode2)
            SSP1CON1 = 0x00 | baud_bits; // enable SPI master with Fosc/16 = 3mhz
            break;
    }
    if (sample == 0) {
      SSP1STAT &= 0b01111111;           // input sampled in middle
    }
    else if (sample == 1) {
      SSP1STAT != 0b10000000;           // input sampled at end
    } else { SSP1STAT &= 0b01111111; }  // input sampled in middle}
    SSP1CON1 |= 0b00100000;             // enable serial port: SSPEN=1
    LATAbits.LATA5 = 1;
}

unsigned char spi_transfer(unsigned char addr)
{
  SSP1BUF = addr;           // write byte to SSP1BUF register
  while (!SSP1STATbits.BF); // wait for data transmit/receipt complete
  return ( SSP1BUF );       // return the value (ignored or the one we want)

}

void spi_cs(unsigned char select)
{   // chip select, normally high
    if (select == 0) {
        LATAbits.LATA5 = 0;
    }
    else if (select == 1) {
        LATAbits.LATA5 = 1;
    }
    else {
        LATAbits.LATA5 = 0;
    }
}
/********************************************************************
 * Function:        void BlinkUSBStatus(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        BlinkUSBStatus turns on and off LEDs 
 *                  corresponding to the USB device state.
 *
 * Note:            mLED macros can be found in HardwareProfile.h
 *                  USBDeviceState is declared and updated in
 *                  usb_device.c.
 *******************************************************************/
void BlinkUSBStatus(void)
{
#if defined(PIC24FJ256DA210_DEV_BOARD)
    // No need to clear UIRbits.SOFIF to 0 here.
    // Callback caller is already doing that.
    #define BLINK_INTERVAL 20000
    #define BLANK_INTERVAL 200000

    static WORD blink_count=0;
    static DWORD loop_count = 0;
    
    if(loop_count == 0)
    {
        if(blink_count != 0)
        {
            loop_count = BLINK_INTERVAL;
            if(mGetLED_1())
            {
                mLED_1_Off();
                blink_count--;
            }
            else
            {
                mLED_1_On();
            }
        }
        else
        {
            loop_count = BLANK_INTERVAL;
            switch(USBDeviceState)
            {
                case ATTACHED_STATE:
                    blink_count = 1;
                    break;
                case POWERED_STATE:
                    blink_count = 2;
                    break;
                case DEFAULT_STATE:
                    blink_count = 3;
                    break;
                case ADR_PENDING_STATE:
                    blink_count = 4;
                    break;
                case ADDRESS_STATE:
                    blink_count = 5;
                    break;
                case CONFIGURED_STATE:
                    blink_count = 6;
                    break;
                case DETACHED_STATE:
                    //fall through
                default:
                    blink_count = 0;
                    break;
            }
        }
    }
    else
    {
        loop_count--;
    }

#else
    // No need to clear UIRbits.SOFIF to 0 here.
    // Callback caller is already doing that.
    static WORD led_count=0;
    
    if(led_count == 0)led_count = 10000U;
    led_count--;

    #define mLED_Both_Off()         {mLED_1_Off();mLED_2_Off();}
    #define mLED_Both_On()          {mLED_1_On();mLED_2_On();}
    #define mLED_Only_1_On()        {mLED_1_On();mLED_2_Off();}
    #define mLED_Only_2_On()        {mLED_1_Off();mLED_2_On();}

    if(USBSuspendControl == 1)
    {
        if(led_count==0)
        {
            mLED_1_Toggle();
            if(mGetLED_1())
            {
                mLED_2_On();
            }
            else
            {
                mLED_2_Off();
            }
        }//end if
    }
    else
    {
        if(USBDeviceState == DETACHED_STATE)
        {
            mLED_Both_Off();
        }
        else if(USBDeviceState == ATTACHED_STATE)
        {
            mLED_Both_On();
        }
        else if(USBDeviceState == POWERED_STATE)
        {
            mLED_Only_1_On();
        }
        else if(USBDeviceState == DEFAULT_STATE)
        {
            mLED_Only_2_On();
        }
        else if(USBDeviceState == ADDRESS_STATE)
        {
            if(led_count == 0)
            {
                mLED_1_Toggle();
                mLED_2_Off();
            }//end if
        }
        else if(USBDeviceState == CONFIGURED_STATE)
        {
            if(led_count==0)
            {
                mLED_1_Toggle();
                if(mGetLED_1())
                {
                    mLED_2_Off();
                }
                else
                {
                    mLED_2_On();
                }
            }//end if
        }
    }
#endif
}//end BlinkUSBStatus




// ******************************************************************************************************
// ************** USB Callback Functions ****************************************************************
// ******************************************************************************************************
// The USB firmware stack will call the callback functions USBCBxxx() in response to certain USB related
// events.  For example, if the host PC is powering down, it will stop sending out Start of Frame (SOF)
// packets to your device.  In response to this, all USB devices are supposed to decrease their power
// consumption from the USB Vbus to <2.5mA* each.  The USB module detects this condition (which according
// to the USB specifications is 3+ms of no bus activity/SOF packets) and then calls the USBCBSuspend()
// function.  You should modify these callback functions to take appropriate actions for each of these
// conditions.  For example, in the USBCBSuspend(), you may wish to add code that will decrease power
// consumption from Vbus to <2.5mA (such as by clock switching, turning off LEDs, putting the
// microcontroller to sleep, etc.).  Then, in the USBCBWakeFromSuspend() function, you may then wish to
// add code that undoes the power saving things done in the USBCBSuspend() function.

// The USBCBSendResume() function is special, in that the USB stack will not automatically call this
// function.  This function is meant to be called from the application firmware instead.  See the
// additional comments near the function.

// Note *: The "usb_20.pdf" specs indicate 500uA or 2.5mA, depending upon device classification. However,
// the USB-IF has officially issued an ECN (engineering change notice) changing this to 2.5mA for all 
// devices.  Make sure to re-download the latest specifications to get all of the newest ECNs.

/******************************************************************************
 * Function:        void USBCBSuspend(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Call back that is invoked when a USB suspend is detected
 *
 * Note:            None
 *****************************************************************************/
void USBCBSuspend(void)
{
	//Example power saving code.  Insert appropriate code here for the desired
	//application behavior.  If the microcontroller will be put to sleep, a
	//process similar to that shown below may be used:
	
	//ConfigureIOPinsForLowPower();
	//SaveStateOfAllInterruptEnableBits();
	//DisableAllInterruptEnableBits();
	//EnableOnlyTheInterruptsWhichWillBeUsedToWakeTheMicro();	//should enable at least USBActivityIF as a wake source
	//Sleep();
	//RestoreStateOfAllPreviouslySavedInterruptEnableBits();	//Preferrably, this should be done in the USBCBWakeFromSuspend() function instead.
	//RestoreIOPinsToNormal();									//Preferrably, this should be done in the USBCBWakeFromSuspend() function instead.

	//IMPORTANT NOTE: Do not clear the USBActivityIF (ACTVIF) bit here.  This bit is 
	//cleared inside the usb_device.c file.  Clearing USBActivityIF here will cause 
	//things to not work as intended.	
	

    #if defined(__C30__) || defined __XC16__
        //This function requires that the _IPL level be something other than 0.
        //  We can set it here to something other than 
        #ifndef DSPIC33E_USB_STARTER_KIT
        _IPL = 1;
        USBSleepOnSuspend();
        #endif
    #endif
}



/******************************************************************************
 * Function:        void USBCBWakeFromSuspend(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The host may put USB peripheral devices in low power
 *					suspend mode (by "sending" 3+ms of idle).  Once in suspend
 *					mode, the host may wake the device back up by sending non-
 *					idle state signalling.
 *					
 *					This call back is invoked when a wakeup from USB suspend 
 *					is detected.
 *
 * Note:            None
 *****************************************************************************/
void USBCBWakeFromSuspend(void)
{
	// If clock switching or other power savings measures were taken when
	// executing the USBCBSuspend() function, now would be a good time to
	// switch back to normal full power run mode conditions.  The host allows
	// 10+ milliseconds of wakeup time, after which the device must be 
	// fully back to normal, and capable of receiving and processing USB
	// packets.  In order to do this, the USB module must receive proper
	// clocking (IE: 48MHz clock must be available to SIE for full speed USB
	// operation).  
	// Make sure the selected oscillator settings are consistent with USB 
    // operation before returning from this function.
}

/********************************************************************
 * Function:        void USBCB_SOF_Handler(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The USB host sends out a SOF packet to full-speed
 *                  devices every 1 ms. This interrupt may be useful
 *                  for isochronous pipes. End designers should
 *                  implement callback routine as necessary.
 *
 * Note:            None
 *******************************************************************/
void USBCB_SOF_Handler(void)
{
    // No need to clear UIRbits.SOFIF to 0 here.
    // Callback caller is already doing that.
}

/*******************************************************************
 * Function:        void USBCBErrorHandler(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The purpose of this callback is mainly for
 *                  debugging during development. Check UEIR to see
 *                  which error causes the interrupt.
 *
 * Note:            None
 *******************************************************************/
void USBCBErrorHandler(void)
{
    // No need to clear UEIR to 0 here.
    // Callback caller is already doing that.

	// Typically, user firmware does not need to do anything special
	// if a USB error occurs.  For example, if the host sends an OUT
	// packet to your device, but the packet gets corrupted (ex:
	// because of a bad connection, or the user unplugs the
	// USB cable during the transmission) this will typically set
	// one or more USB error interrupt flags.  Nothing specific
	// needs to be done however, since the SIE will automatically
	// send a "NAK" packet to the host.  In response to this, the
	// host will normally retry to send the packet again, and no
	// data loss occurs.  The system will typically recover
	// automatically, without the need for application firmware
	// intervention.
	
	// Nevertheless, this callback function is provided, such as
	// for debugging purposes.
}


/*******************************************************************
 * Function:        void USBCBCheckOtherReq(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        When SETUP packets arrive from the host, some
 * 					firmware must process the request and respond
 *					appropriately to fulfill the request.  Some of
 *					the SETUP packets will be for standard
 *					USB "chapter 9" (as in, fulfilling chapter 9 of
 *					the official USB specifications) requests, while
 *					others may be specific to the USB device class
 *					that is being implemented.  For example, a HID
 *					class device needs to be able to respond to
 *					"GET REPORT" type of requests.  This
 *					is not a standard USB chapter 9 request, and 
 *					therefore not handled by usb_device.c.  Instead
 *					this request should be handled by class specific 
 *					firmware, such as that contained in usb_function_hid.c.
 *
 * Note:            None
 *******************************************************************/
void USBCBCheckOtherReq(void)
{
    USBCheckHIDRequest();
}//end


/*******************************************************************
 * Function:        void USBCBStdSetDscHandler(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The USBCBStdSetDscHandler() callback function is
 *					called when a SETUP, bRequest: SET_DESCRIPTOR request
 *					arrives.  Typically SET_DESCRIPTOR requests are
 *					not used in most applications, and it is
 *					optional to support this type of request.
 *
 * Note:            None
 *******************************************************************/
void USBCBStdSetDscHandler(void)
{
    // Must claim session ownership if supporting this request
}//end


/*******************************************************************
 * Function:        void USBCBInitEP(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is called when the device becomes
 *                  initialized, which occurs after the host sends a
 * 					SET_CONFIGURATION (wValue not = 0) request.  This 
 *					callback function should initialize the endpoints 
 *					for the device's usage according to the current 
 *					configuration.
 *
 * Note:            None
 *******************************************************************/
void USBCBInitEP(void)
{
    //enable the HID endpoint
    USBEnableEndpoint(HID_EP,USB_IN_ENABLED|USB_OUT_ENABLED|USB_HANDSHAKE_ENABLED|USB_DISALLOW_SETUP);
    //Re-arm the OUT endpoint for the next packet
    USBOutHandle = HIDRxPacket(HID_EP,(BYTE*)&ReceivedDataBuffer,64);
}

/********************************************************************
 * Function:        void USBCBSendResume(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The USB specifications allow some types of USB
 * 					peripheral devices to wake up a host PC (such
 *					as if it is in a low power suspend to RAM state).
 *					This can be a very useful feature in some
 *					USB applications, such as an Infrared remote
 *					control	receiver.  If a user presses the "power"
 *					button on a remote control, it is nice that the
 *					IR receiver can detect this signalling, and then
 *					send a USB "command" to the PC to wake up.
 *					
 *					The USBCBSendResume() "callback" function is used
 *					to send this special USB signalling which wakes 
 *					up the PC.  This function may be called by
 *					application firmware to wake up the PC.  This
 *					function will only be able to wake up the host if
 *                  all of the below are true:
 *					
 *					1.  The USB driver used on the host PC supports
 *						the remote wakeup capability.
 *					2.  The USB configuration descriptor indicates
 *						the device is remote wakeup capable in the
 *						bmAttributes field.
 *					3.  The USB host PC is currently sleeping,
 *						and has previously sent your device a SET 
 *						FEATURE setup packet which "armed" the
 *						remote wakeup capability.   
 *
 *                  If the host has not armed the device to perform remote wakeup,
 *                  then this function will return without actually performing a
 *                  remote wakeup sequence.  This is the required behavior, 
 *                  as a USB device that has not been armed to perform remote 
 *                  wakeup must not drive remote wakeup signalling onto the bus;
 *                  doing so will cause USB compliance testing failure.
 *                  
 *					This callback should send a RESUME signal that
 *                  has the period of 1-15ms.
 *
 * Note:            This function does nothing and returns quickly, if the USB
 *                  bus and host are not in a suspended condition, or are 
 *                  otherwise not in a remote wakeup ready state.  Therefore, it
 *                  is safe to optionally call this function regularly, ex: 
 *                  anytime application stimulus occurs, as the function will
 *                  have no effect, until the bus really is in a state ready
 *                  to accept remote wakeup. 
 *
 *                  When this function executes, it may perform clock switching,
 *                  depending upon the application specific code in 
 *                  USBCBWakeFromSuspend().  This is needed, since the USB
 *                  bus will no longer be suspended by the time this function
 *                  returns.  Therefore, the USB module will need to be ready
 *                  to receive traffic from the host.
 *
 *                  The modifiable section in this routine may be changed
 *                  to meet the application needs. Current implementation
 *                  temporary blocks other functions from executing for a
 *                  period of ~3-15 ms depending on the core frequency.
 *
 *                  According to USB 2.0 specification section 7.1.7.7,
 *                  "The remote wakeup device must hold the resume signaling
 *                  for at least 1 ms but for no more than 15 ms."
 *                  The idea here is to use a delay counter loop, using a
 *                  common value that would work over a wide range of core
 *                  frequencies.
 *                  That value selected is 1800. See table below:
 *                  ==========================================================
 *                  Core Freq(MHz)      MIP         RESUME Signal Period (ms)
 *                  ==========================================================
 *                      48              12          1.05
 *                       4              1           12.6
 *                  ==========================================================
 *                  * These timing could be incorrect when using code
 *                    optimization or extended instruction mode,
 *                    or when having other interrupts enabled.
 *                    Make sure to verify using the MPLAB SIM's Stopwatch
 *                    and verify the actual signal on an oscilloscope.
 *******************************************************************/
void USBCBSendResume(void)
{
    static WORD delay_count;
    
    //First verify that the host has armed us to perform remote wakeup.
    //It does this by sending a SET_FEATURE request to enable remote wakeup,
    //usually just before the host goes to standby mode (note: it will only
    //send this SET_FEATURE request if the configuration descriptor declares
    //the device as remote wakeup capable, AND, if the feature is enabled
    //on the host (ex: on Windows based hosts, in the device manager 
    //properties page for the USB device, power management tab, the 
    //"Allow this device to bring the computer out of standby." checkbox 
    //should be checked).
    if(USBGetRemoteWakeupStatus() == TRUE) 
    {
        //Verify that the USB bus is in fact suspended, before we send
        //remote wakeup signalling.
        if(USBIsBusSuspended() == TRUE)
        {
            USBMaskInterrupts();
            
            //Clock switch to settings consistent with normal USB operation.
            USBCBWakeFromSuspend();
            USBSuspendControl = 0; 
            USBBusIsSuspended = FALSE;  //So we don't execute this code again, 
                                        //until a new suspend condition is detected.

            //Section 7.1.7.7 of the USB 2.0 specifications indicates a USB
            //device must continuously see 5ms+ of idle on the bus, before it sends
            //remote wakeup signalling.  One way to be certain that this parameter
            //gets met, is to add a 2ms+ blocking delay here (2ms plus at 
            //least 3ms from bus idle to USBIsBusSuspended() == TRUE, yeilds
            //5ms+ total delay since start of idle).
            delay_count = 3600U;        
            do
            {
                delay_count--;
            }while(delay_count);
            
            //Now drive the resume K-state signalling onto the USB bus.
            USBResumeControl = 1;       // Start RESUME signaling
            delay_count = 1800U;        // Set RESUME line for 1-13 ms
            do
            {
                delay_count--;
            }while(delay_count);
            USBResumeControl = 0;       //Finished driving resume signalling

            USBUnmaskInterrupts();
        }
    }
}


/*******************************************************************
 * Function:        BOOL USER_USB_CALLBACK_EVENT_HANDLER(
 *                        USB_EVENT event, void *pdata, WORD size)
 *
 * PreCondition:    None
 *
 * Input:           USB_EVENT event - the type of event
 *                  void *pdata - pointer to the event data
 *                  WORD size - size of the event data
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is called from the USB stack to
 *                  notify a user application that a USB event
 *                  occured.  This callback is in interrupt context
 *                  when the USB_INTERRUPT option is selected.
 *
 * Note:            None
 *******************************************************************/
BOOL USER_USB_CALLBACK_EVENT_HANDLER(int event, void *pdata, WORD size)
{
    switch(event)
    {
        case EVENT_TRANSFER:
            //Add application specific callback task or callback function here if desired.
            break;
        case EVENT_SOF:
            USBCB_SOF_Handler();
            break;
        case EVENT_SUSPEND:
            USBCBSuspend();
            break;
        case EVENT_RESUME:
            USBCBWakeFromSuspend();
            break;
        case EVENT_CONFIGURED: 
            USBCBInitEP();
            break;
        case EVENT_SET_DESCRIPTOR:
            USBCBStdSetDscHandler();
            break;
        case EVENT_EP0_REQUEST:
            USBCBCheckOtherReq();
            break;
        case EVENT_BUS_ERROR:
            USBCBErrorHandler();
            break;
        case EVENT_TRANSFER_TERMINATED:
            //Add application specific callback task or callback function here if desired.
            //The EVENT_TRANSFER_TERMINATED event occurs when the host performs a CLEAR
            //FEATURE (endpoint halt) request on an application endpoint which was 
            //previously armed (UOWN was = 1).  Here would be a good place to:
            //1.  Determine which endpoint the transaction that just got terminated was 
            //      on, by checking the handle value in the *pdata.
            //2.  Re-arm the endpoint if desired (typically would be the case for OUT 
            //      endpoints).
            break;
        default:
            break;
    }      
    return TRUE; 
}

/** EOF main.c *************************************************/
#endif
