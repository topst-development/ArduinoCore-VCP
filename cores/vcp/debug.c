// SPDX-License-Identifier: Apache-2.0

/*
***************************************************************************************************
*
*   FileName : debug.c
*
*   Copyright (c) Telechips Inc.
*
*   Description :
*
*
***************************************************************************************************
*/

#include <stdint.h>
#include "uart.h"
#include <debug.h>

uint32_t                                  gDebugOption = 0xC0000001UL;

DbgTag_t                                dbgTags[DBG_MAX_TAGS] =
{
    {"SAL\0\0\0"},
    {"MPU\0\0\0"},
    {"GIC\0\0\0"},
    {"GMAC\0\0"},
    {"MIDF\0\0"},  //5
    {"CAN\0\0\0"},
    {"GPSB\0\0"},
    {"UART\0\0"},
    {"I2C\0\0\0"},
    {"PDM\0\0\0"},
    {"ICTC\0\0"}, //10
    {"GDMA\0\0"},
    {"ADC\0\0\0"},
    {"MBOX\0\0"},
    {"TIMER\0"},
    {"WDT\0\0\0"}, //15
    {"CKC\0\0\0"},
    {"GPIO\0\0"},
    {"PMIO\0\0"},
    {"DSE\0\0\0"},
    {"SDM\0\0\0"}, //20
    {"FMU\0\0\0"},
    {"FWUG\0\0"},
    {"LIN\0\0\0"},
    {"I2S\0\0\0"},
    {"EFLASH"}, //25
    {"SSM\0\0\0"},
};

#if DEBUG_ENABLE
#define is_digit(c)                     (((c) >= (int8_t)'0') && ((c) <= (int8_t)'9'))
#define to_upper(c)                     ((c) & ~0x20)

typedef void                            (fPUTC)(int8_t c);

static int32_t DBG_Print8
(
    uint32_t                              num,
    int8_t *                             str
);

static int32_t DBG_Print10
(
    uint32_t                              num,
    int8_t *                             str
);

static int32_t DBG_Print16L
(
    uint32_t                              num,
    int8_t *                             str
); //Lowercase

static int32_t DBG_Print16U
(
    uint32_t                              num,
    int8_t *                             str
); //Uppercase

int32_t DBG_Putc
(
    int8_t                               putChar
);

int32_t DBG_Printfi
(
    fPUTC *                             fPutC,
    int8_t *                             fmt_in,
    int32_t *                            args_in
);

/*------- external -----------*/
int32_t DBG_Putc
(
    int8_t                               putChar
) /* QAC - MISRA-C:2004 Rule 8.8 - This function is used in ASM */
{
    if (putChar == (int8_t)'\n')
    {
        (void)UART_PutChar((uint32_t)UART_DEBUG_CH, (uint8_t)'\r');
    }

    (void)UART_PutChar((uint32_t)UART_DEBUG_CH, (uint8_t)putChar);

    return 0;
}

/*
 * print character
 */
static int32_t DBG_Print8
(
    uint32_t                              num,
    int8_t *                             str
)
{
    int32_t  len;
    int8_t   temp[20];
    int8_t   *p  = temp;
    int8_t   *destStr;
    uint32_t  numChar;
    uint32_t  calcTemp;

    numChar = num;
    *p      = 0;
    len     = 0;

    if(str != NULL_PTR)
    {
        destStr = str;

        do
        {
            ++p;
            calcTemp    = numChar%8UL; //orig numChar & 0x7
            *p          = ((int8_t)((int8_t)calcTemp) + (int8_t)'0');
            numChar     = numChar/8UL;//orig numChar     = numChar >> 3U;
            len++;
        } while (numChar!=0UL);

        while(1)
        {
            *destStr    = *p;
            destStr++;

            if((*p) == 0)
            {
                break;
            }

            p--;
         }
    }

    return  len;
}

/*
 * print decimal
 */
static int32_t DBG_Print10
(
    uint32_t                              num,
    int8_t *                             str
)
{
    int32_t  len;
    int8_t   temp[20];
    int8_t   *p  = temp;
    int8_t   *destStr;
    uint32_t numChar;
    uint32_t calcTemp;

    numChar = num;
    *p      = 0;
    len     = 0;

    if(str != NULL_PTR)
    {
        destStr = str;

        do
        {
            ++p;
            calcTemp    = numChar % 10UL;
            *p          = (int8_t)((int8_t)calcTemp + (int8_t)'0');
            numChar     = numChar / 10UL;
            len++;
        } while (numChar != 0UL);

        while(1)
        {
            *destStr    = *p;
            destStr++;

            if((*p) == 0)
            {
                break;
            }

            p--;
        }
    }

    return  len;
}

/*
 * print hexa-decimal, lower case
 */
static int32_t DBG_Print16L
(
    uint32_t                              num,
    int8_t *                             str
)
{
    int32_t  len;
    int8_t   temp[20];
    int8_t   *p  = temp;
    int8_t   *destStr;
    uint32_t  numChar;
    uint32_t  calcTemp;

    numChar = num;
    *p      = 0;
    len     = 0;

    if(str != NULL_PTR)
    {
        destStr = str;

        do
        {
            ++p;
            calcTemp    = numChar % 16UL; //numChar & 0x0f
            *p          = (int8_t)"0123456789abcdef"[calcTemp];
            numChar     = numChar / 16UL;
            len++;
        } while (numChar != 0UL);

        while(1)
        {
            *destStr    = *p;
            destStr++;

            if((*p) == 0)
            {
                break;
            }

            p--;
        }
    }

    return  len;
}

/*
 * print hexa-decimal, upper case
 */
static int32_t DBG_Print16U
(
    uint32_t                              num,
    int8_t *                             str
)
{
    int32_t  len;
    int8_t   temp[20];
    int8_t   *p  = temp;
    int8_t   *destStr;
    uint32_t  numChar;
    uint32_t  calcTemp;

    numChar = num;
    *p      = 0;
    len     = 0;

    if(str != NULL_PTR)
    {
        destStr = str;

        do
        {
            ++p;
            calcTemp    = numChar % 16UL; //numChar & 0x0f
            *p          = (int8_t)"0123456789ABCDEF"[calcTemp];
            numChar     = numChar / 16UL;
            len++;
        } while (numChar != 0UL);

        while(1)
        {
            *destStr    = *p;
            destStr++;

            if((*p) == 0)
            {
                break;
            }

            p--;
        }
    }

    return  len;
}

/*
 *
 */
int32_t DBG_Printfi
(
    fPUTC *                             fPutC,
    int8_t *                             fmt_in,
    int32_t *                            args_in
) /* QAC - MISRA-C:2004 Rule 8.8 - This function is used in ASM */
{
    int32_t      i;
    int32_t      f;
    int8_t       c;
    int32_t      length;
    int32_t      leftjust;
    int32_t      leading;
    int32_t      fmaxString;
    int32_t      fminString;
    int8_t       sign;
    int8_t       fill;
    int8_t *     str;
    int8_t       string[64];
    int8_t *     fmt;
    int32_t *    args;

    length      = 0;
    leftjust    = 0;

    if ((fPutC != NULL_PTR) && (fmt_in != NULL_PTR) && (args_in != NULL_PTR))
    {
        fmt     = fmt_in;
        args    = args_in;

        for(;;)
        {
            /* Echo character until '%' or end of fmt string */
            while ((*fmt) != (int8_t)'%')
            {
                c = *fmt;
                fmt++;

                if (c == (int8_t)0)//if (!c)
                {
                    return (-1);
                }

                (*fPutC)(c);
            }

            fmt++;

            /* Echo " ... %%..." as '%'  */
            if (*fmt == (int8_t)'%')
            {
                (*fPutC)(*fmt);
                fmt++;

                continue;
            }

            /* Check for "%-...",== Left-justified output */
            if (*fmt == (int8_t)'-')//if ((*fmt == (int8_t)'-') != 0)
            {
                leftjust = 1;
                fmt ++;
            }
            else
            {
                leftjust = 0;
            }

            /* Allow for zero-filled numeric outputs ("%0...") */
            if (*fmt ==(int8_t)'0')
            {
                fill = *fmt;
                fmt++;
            }
            else
            {
                fill = (int8_t)' ';
            }

            /* Allow for minimum field width specifier for %d %u x,c,o,s */
            /* Also  allow %* for variable width (%0* as well)   */
            fminString = 0;

            if (*fmt == (int8_t)'*')
            {
                fminString = *args;
                args++;
                ++fmt;
            }
            else
            {
                while ((int8_t)is_digit(*fmt) != 0)
                {
                    fminString = (((fminString * 10) + (*fmt)) - (int8_t)'0') ;
                    fmt++;
                }
            }

            /* Allow for maximum string width for %s */
            fmaxString = 0;

            if (*fmt == (int8_t)'.')
            {
                ++fmt;

                if (*fmt == (int8_t)'*' )
                {
                    fmaxString = *args;
                    args++;
                    ++fmt;
                }
                else
                {
                    while ((int8_t)is_digit(*fmt) != 0)
                    {
                        fmaxString = (((fmaxString * 10) + (*fmt)) - (int8_t)'0');
                        fmt++;
                    }
                }
            }

            if ((*fmt) == 0)
            {
                f = *fmt;
                fmt++;
                (*fPutC)((int8_t)'%');

                return (-1);
            }
            else
            {
                f = *fmt;
                fmt++;
            }

            sign = 0;

            switch (f)
            {
                case 0x63/*'c'*/ :
                {
                    string[0]   = (int8_t)*args;
                    string[1]   = (int8_t)'\0';
                    str         = string;
                    length      = 1;
                    fmaxString  = 0;
                    fill        = (int8_t)' ';
                    break;
                }

                case 0x73/*'s'*/ :
                {
                    str = (int8_t *)*args;

                    for (length = 0; length < 2147483647; length ++)//change of for(;;)
                    {
                        if(*str == 0)
                        {
                            break;
                        }

                        str++;
                    }
                    str++;
                    str     = (int8_t *)*args;
                    fill    = (int8_t)' ';
                    break;
                }

                case 0x64 /*'d'*/ :
                {
                    str = string;
                    i   = *(const int32_t *)args;

                    if (i < 0)
                    {
                        sign    = (int8_t)'-';
                        i       = -i;
                    }

                    length      = DBG_Print10((uint32_t)i, str);
                    fmaxString  = 0;
                    break;
                }

                case 0x6f /*'o'*/ :
                {
                    str         = string;
                    length      = DBG_Print8((uint32_t)(*(const int32_t *)args), str);
                    fmaxString  = 0;
                    break;
                }

                case 0x78 /*'x'*/ :
                {
                    str         = string;
                    length      = DBG_Print16L((uint32_t)(*(const int32_t *)args), str);
                    fmaxString  = 0;
                    break;
                }

                case 0x58 /*'X'*/ :
                {
                    str         = string;
                    length      = DBG_Print16U((uint32_t)(*(const int32_t *)args), str);
                    fmaxString        = 0;
                    break;
                }

                default:
                {
                    /* format is not supported */
                    str     = (int8_t *) *args;
                    length  = 1;
                    break;
                }
            }

        //  if (fminString > uDBG_txbuf_len)
        //      fminString = 0;
        //  if (fmaxString > uDBG_txbuf_len)
        //      fmaxString = 0;

            leading = 0;

            if ((fmaxString != 0) || (fminString!=0))
            {
                if (fmaxString != 0)
                {
                    if (length > fmaxString)
                    {
                        length  = fmaxString;
                    }
                }

                if (fminString!=0)
                {
                    leading     = fminString - length;
                }

                if (sign == (int8_t)'-')
                {
                    leading --;
                }
            }

            if ((sign == (int8_t)'-') && (fill == (int8_t)'0'))
            {
                (*fPutC)(sign);
            }

            if (leftjust==0)//if (!leftjust)
            {
                for (i = 0; i < leading; i ++)
                {
                    (*fPutC)(fill);
                }
            }

            if ((sign == (int8_t)'-') && (fill == (int8_t)' '))
            {
                (*fPutC)(sign);
            }

            for (i = 0; i < length; i ++)
            {
                (*fPutC)(*str);
                str++;
            }

            if (leftjust != 0)
            {
                for (i = 0; i < leading; i ++)
                {
                    (*fPutC)(fill);
                }
            }

            args++;
        }
    }

    return (-1);
}

__attribute__ ((naked)) void DBG_SerialPrintf
(
    const int8_t *                        format,
    ...
) /* QAC - MISRA-C:2004 Rule 16.1 - Printf() needs variable parameters */
{
    /*The compiler only supports basic __asm statements in __attribute__((naked)) functions.
      Using extended assembly, parameter references or mixing C code with __asm statements might not work reliably.*/
    //(void)format; 
#if defined(__GNUC__)
    __asm volatile (
            "stmdb  sp!, {r1-r3}    \n"
            "mov    r1, r0          \n"     /* r1 = format string */
            "mov    r2, sp          \n"     /* r2 = arguments */
            "ldr    r0, =DBG_Putc     \n"     /* r0 = function pointer which will put character */
            "str    lr, [sp, #-4]!  \n"     /* save r14 */
            "bl     DBG_Printfi       \n"
            "ldr    lr, [sp], #4    \n"     /* restore r14 */
            "add    sp, sp, #12     \n"     /* restore stack */
            "mov    pc, lr          \n"
            :::"r0", "r1", "r2", "r3" \
    );
#else //Green Hills case
    #pragma asm
    mov r1, r0
    mov r2, sp
    add r2, r2, #8
    ldr r0, =DBG_Putc
    str lr, [sp, #-4]!
    bl  DBG_Printfi
    ldr lr, [sp], #4
    add sp, sp, #20
    mov pc, lr
    #pragma endasm
#endif
}
#else
__attribute__ ((naked)) void DBG_SerialPrintf
(
    const int8_t *                        format,
    ...
) /* QAC - MISRA-C:2004 Rule 16.1 - Printf() needs variable parameters */
{

}
#endif

