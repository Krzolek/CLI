/*! \file CliMod.c
 *
 * \brief \TODO
 *
 *  Created on: Mar 10, 2018
 *      Author: Krzysztof Olek
 *
 * COPYRIGHT NOTICE: (c) 2018 Krzysztof Olek
 * All rights reserved.
 */
/*================================ INCLUDES =================================*/
#include "CliMod.h"
#include <stdio.h>

/*================================ DEFINES ==================================*/

/*================================ TYPEDEFS =================================*/
/*! \brief Structure with buffers and indexes for internal computing */
typedef struct CliInternalBuffer_tag
{
   char pcBuf[CLI_BUFFER_SIZE]; /*!< Accumulating received characters */
   char * pu16TokenAddress[CLI_TOKEN_COUNT_MAX]; /*!< Starting addresses of recognized tokens in pcBuf will be stored here */
   uint16_t u16Index; /*!< Received bytes counter */
   uint16_t u16TokenCount; /*!< Recognized tokens counter */
} CliInternalBuffer_t;

/*====================== LOCAL FUNCTION DECLARATIONS ========================*/
/*! \brief Try to find called command and execute it.
 *
 *  \param  psCliBuf - structure with internal CLI buffers
 *  \return void
 *
 */
static void ExecuteCommand(CliInternalBuffer_t * psCliBuf);

/*! \brief Divide received buffer into separate strings.
 *
 *  \param  psCliBuf - structure with internal CLI buffers
 *  \return bool - false if no tokens were found, otherwise true
 *
 *  \note False will be returned only if only white characters are present in buffer
 *
 *  Each string of non-white characters surrounded by white characters is recognized as
 *  separate token. Address of first non-white char is saved in psCliBuf and zero-termination
 *  is added after each token.
 */
static bool GetTokensFromString(CliInternalBuffer_t * psCliBuf);

/*! \brief Check if byte is one of white characters
 *
 *  \param  pcCharacter - byte to be checked
 *  \return bool - true if white char, otherwise false
 *
 *  \note checked against ' ' and '\t'
 */
static bool IsWhiteChar(char pcCharacter);

/*! \brief Check if byte is one of defined line endings
 *
 *  \param  pcCharacter - byte to be checked
 *  \return bool - true if newline char, otherwise false
 *
 *  \note checked agains '\n' and '\r'
 */
static bool IsNewLineChar(char pcCharacter);

/*! \brief Try to find command with name matching first token
 *
 *  \param  psCliBuf - structure with internal CLI buffers
 *  \return bool - true if command was found, false otherwise
 *
 *  Detailed description starts here.
 */
static bool FindAndExecuteCommand(CliInternalBuffer_t * psCliBuf);

static void ErrorCommandHandler(void);

/*! \brief Help command function
 *
 *  \param  argc - count of passed parameters
 *  \param  argv - array with passed parameters
 *  \return void
 *
 */
static void CliHelpCmd(uint16_t argc, char* argv[]);
static void CliVersionCmd(uint16_t argc, char* argv[]);
/*============================= LOCAL VARIABLES =============================*/
static CliInternalBuffer_t CliBuf = { .u16Index = 0u, .u16TokenCount = 0u };
static CliCommand_t sCliHelpCmd = { .pcName = "Help", .CmdFun = &CliHelpCmd };
static CliCommand_t sCliVersionCmd = { .pcName = "Version", .CmdFun =
      &CliVersionCmd };

CliCommand_t * CliCommandTable[] = { &sCliHelpCmd, &sCliVersionCmd };

static CliCommandList_t sCliCmdList = { .ppsCliCmds = CliCommandTable,
      .u16NoCmds = sizeof(CliCommandTable) / sizeof(CliCommandTable[0]) };

/*====================== LOCAL FUNCTION DEFINITIONS =========================*/
void Cli_PutChar(char cByte)
{
   if (CliBuf.u16Index < CLI_BUFFER_SIZE)
   {
      CliBuf.pcBuf[CliBuf.u16Index] = cByte;
      CliBuf.u16Index++;
   }

   if (('\n' == cByte) || ('\r' == cByte)
         || (CLI_BUFFER_SIZE == CliBuf.u16Index))
   {
      if (CliBuf.u16Index < CLI_BUFFER_SIZE)
      {
         CliBuf.pcBuf[CliBuf.u16Index] = '\0';
      }
      else
      {
         CliBuf.pcBuf[CLI_BUFFER_SIZE - 1u] = '\0';
      }
      ExecuteCommand(&CliBuf);
      CliBuf.u16Index = 0u;
   }
}

void Cli_PutString(uint16_t u16Count, const char * pcByte)
{
   for (; u16Count > 0u; u16Count--)
   {
      Cli_PutChar(*pcByte);
      pcByte++;
   }
}

bool IsStringEqual(const char * pcString1, const char * pcString2)
{
   bool bRetVal = true;

   if ((*pcString1) && (*pcString2)) /* Not 0 length */
   {
      while ((*pcString1) && (*pcString2)) /* Until end of (shorter) string */
      {
         if ((*pcString1) != (*pcString2))
         {
            bRetVal = false;
         }
         pcString1++;
         pcString2++;
      }
   }

   return bRetVal;
}

static void ExecuteCommand(CliInternalBuffer_t * psCliBuf)
{
   uint16_t u16Index = 0u;
   bool bTokensFound;

   bTokensFound = GetTokensFromString(psCliBuf);
#ifdef DEBUG_MODE
   for (; u16Index < psCliBuf->u16TokenCount; ++u16Index)
   {
      printf("Token %d:\n\t", u16Index);
      Cli_PrintInterface((char *)psCliBuf->pu16TokenAddress[u16Index]);
      Cli_PrintInterface("\n");
   }
#endif
   if (true == bTokensFound)
   {
      FindAndExecuteCommand(psCliBuf);
   }
   else
   {
      ErrorCommandHandler();
   }

   Cli_PrintInterface("> ");

}

static bool FindAndExecuteCommand(CliInternalBuffer_t * psCliBuf)
{
   bool bRetVal = false;

   for (uint16_t u16I = 0u;
         (u16I < sCliCmdList.u16NoCmds) && (false == bRetVal); ++u16I)
   {
      if (true
            == IsStringEqual(sCliCmdList.ppsCliCmds[u16I]->pcName,
                  psCliBuf->pu16TokenAddress[0]))
      {
         sCliCmdList.ppsCliCmds[u16I]->CmdFun(psCliBuf->u16TokenCount,
               psCliBuf->pu16TokenAddress);
         bRetVal = true;
      }
   }
   if (false == bRetVal)
   {
      ErrorCommandHandler();
   }

   return bRetVal;
}

static bool GetTokensFromString(CliInternalBuffer_t * psCliBuf)
{
   char * pu16SourcePointer = psCliBuf->pcBuf;
   bool bRetVal = true;
   psCliBuf->u16TokenCount = 0u;

   /* Go through all white chars before first token */
   while (IsWhiteChar(*pu16SourcePointer))
   {
      pu16SourcePointer++;
   }

   /* Get all tokens separated by white characters */
   while (!IsNewLineChar(*pu16SourcePointer)) /* Go through message */
   {
      /* Start of new token */
      psCliBuf->pu16TokenAddress[psCliBuf->u16TokenCount] = pu16SourcePointer;
      psCliBuf->u16TokenCount++;
      /* Go through length of token */
      while (!IsWhiteChar(*pu16SourcePointer)
            && !IsNewLineChar(*pu16SourcePointer))
      {
         pu16SourcePointer++;
      }
      /* White char after token, add terminator */
      if (IsNewLineChar(*pu16SourcePointer))
      {
         *pu16SourcePointer = '\0';
         break;
      }
      *pu16SourcePointer = '\0';
      pu16SourcePointer++;
      /* Go through all white chars before next token */
      while (IsWhiteChar(*pu16SourcePointer))
      {
         pu16SourcePointer++;
      }
   }

   if (0u == psCliBuf->u16TokenCount)
   {
      bRetVal = false;
   }

   return bRetVal;
}

static bool IsWhiteChar(char pcCharacter)
{
   bool bRetVal = false;

   if ((' ' == pcCharacter) || ('\t' == pcCharacter))
   {
      bRetVal = true;
   }

   return bRetVal;
}

static bool IsNewLineChar(char pcCharacter)
{
   bool bRetVal = false;

   if (('\n' == pcCharacter) || ('\r' == pcCharacter))
   {
      bRetVal = true;
   }

   return bRetVal;
}

static void CliHelpCmd(uint16_t argc, char* argv[])
{
   Cli_PrintInterface("This is Help!\n");

   for (uint16_t u16I = 0; u16I < sCliCmdList.u16NoCmds; ++u16I)
   {
      Cli_PrintInterface(sCliCmdList.ppsCliCmds[u16I]->pcName);
      Cli_PrintInterface("\n");
   }

}

static void CliVersionCmd(uint16_t argc, char* argv[])
{
   Cli_PrintInterface("This is 0.0 version!\n");
}

static void ErrorCommandHandler(void)
{
   Cli_PrintInterface(
         "No valid command was found! Type Help for command list\n");
}
