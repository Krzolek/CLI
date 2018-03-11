/*! \file CliCallbacks.c
 *
 * \brief Callbacks to be used to interface with CLI module.
 *
 *  Created on: Mar 10, 2018
 *      Author: Krzysztof Olek
 *
 * COPYRIGHT NOTICE: (c) 2018 Krzysztof Olek
 * All rights reserved.
 */
/*================================ INCULDES =================================*/
#include "CliMod.h"
#include <stdio.h>

/*================================ DEFINES ==================================*/


/*================================ TYPEDEFS =================================*/


/*====================== LOCAL FUNCTION DECLARATIONS ========================*/


/*============================= LOCAL VARIABLES =============================*/


/*====================== LOCAL FUNCTION DEFINITIONS =========================*/
void Cli_PrintInterface(const char * const pcMsg)
{
   printf("%s", pcMsg);
}
