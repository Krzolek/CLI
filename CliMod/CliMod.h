/*! \file CliMod.h
 *
 * \brief \TODO
 *
 *  Created on: Mar 10, 2018
 *      Author: Krzysztof Olek
 *
 * COPYRIGHT NOTICE: (c) 2018 Krzysztof Olek
 * All rights reserved.
 */
#ifndef _CLIMOD_H
#define _CLIMOD_H

/*================================ INCLUDES =================================*/
#include <stdint.h>

/*================================ DEFINES ==================================*/
#define CLI_BUFFER_SIZE (512u)      /*!< Longest command variation must not exceed this value */
#define CLI_TOKEN_COUNT_MAX (10u)   /*!< Maximum separate parts of command (name included) */

#ifndef bool
#define bool uint16_t               /*!< Define simple bool in case not stdbool or similar is used  */
#endif
#ifndef true
#define true (1u)                   /*!< Define true for internal bool */
#endif
#ifndef false
#define false (0u)                  /*!< Define false for internal bool */
#endif

//#define DEBUG_MODE                  /*!< Uncomment for additional debug prints */

/*================================ TYPEDEFS =================================*/
/*! \brief Pointer to command function */
typedef void (*CliFunCmdPointer)(uint16_t,char **);
/*! \brief Structure with command relevant data */
typedef struct CliCommand_tag
{
   const char * const pcName; /*!< Name of the command */
   const CliFunCmdPointer CmdFun;
} CliCommand_t;
/*! \brief Structure with command relevant data */
typedef struct CliCommandList_tag
{
   CliCommand_t** ppsCliCmds;
   uint16_t u16NoCmds;
} CliCommandList_t;

/*============================ GLOBAL VARIABLES =============================*/

/*===================== GLOBAL FUNCTION DECLARATIONS ========================*/
/*! \brief Put print function used in your project inside this function.
 *
 *  \param pcMsg - pointer to data (0 -terminated string) to be set at output
 *  \return void
 *
 *  It can be printf, UART_Send, etc. Depending on project.
 */
void Cli_PrintInterface(const char * const pcMsg);

/*! \brief Add received char for processing by CLI.
 *
 *  \param cByte - receiving new line char (\n or \r) will trigger command exec
 *  \return void
 */
void Cli_PutChar(char cByte);

/*! \brief Adds u16Count of bytes stored under address of pcByte
 *
 *  \param u16Count - how many bytes will be copied
 *  \param pcByte - pointer do array with characters
 *  \return
 *
 *  Detailed description starts here.
 */
void Cli_PutString(uint16_t u16Count, const char * pcByte);

bool IsStringEqual(const char * pcString1, const char * pcString2);

#endif /* _CLIMOD_H */
