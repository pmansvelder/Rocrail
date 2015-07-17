/*
 Copyright (c) 2002 Robert Jan Versluis, Rocrail.net

 

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rocs/impl/mime64_impl.h"

#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/trace.h"

static int instCnt = 0;

/* SHA1

Copyright (C) 1998, 2009
Paul E. Jones <paulej@packetizer.com>

Freeware Public License (FPL)

This software is licensed as "freeware."  Permission to distribute
this software in source and binary forms, including incorporation
into other products, is hereby granted without a fee.  THIS SOFTWARE
IS PROVIDED 'AS IS' AND WITHOUT ANY EXPRESSED OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS FOR A PARTICULAR PURPOSE.  THE AUTHOR SHALL NOT BE HELD
LIABLE FOR ANY DAMAGES RESULTING FROM THE USE OF THIS SOFTWARE, EITHER
DIRECTLY OR INDIRECTLY, INCLUDING, BUT NOT LIMITED TO, LOSS OF DATA
OR DATA BEING RENDERED INACCURATE.
 */
typedef struct SHA1Context
{
    unsigned Message_Digest[5]; /* Message Digest (output)          */

    unsigned Length_Low;        /* Message length in bits           */
    unsigned Length_High;       /* Message length in bits           */

    unsigned char Message_Block[64]; /* 512-bit message blocks      */
    int Message_Block_Index;    /* Index into message block array   */

    int Computed;               /* Is the digest computed?          */
    int Corrupted;              /* Is the message digest corruped?  */
} SHA1Context;

/*
 *  Function Prototypes
 */
static void SHA1Reset(SHA1Context *);
static int SHA1Result(SHA1Context *);
static void SHA1Input( SHA1Context *, const unsigned char *, unsigned);

/*
 *  Define the circular shift macro
 */
#define SHA1CircularShift(bits,word) \
                ((((word) << (bits)) & 0xFFFFFFFF) | \
                ((word) >> (32-(bits))))

/* Function prototypes */
static void SHA1ProcessMessageBlock(SHA1Context *);
static void SHA1PadMessage(SHA1Context *);

/*
 *  SHA1Reset
 *
 *  Description:
 *      This function will initialize the SHA1Context in preparation
 *      for computing a new message digest.
 *
 *  Parameters:
 *      context: [in/out]
 *          The context to reset.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *
 */
static void SHA1Reset(SHA1Context *context)
{
    context->Length_Low             = 0;
    context->Length_High            = 0;
    context->Message_Block_Index    = 0;

    context->Message_Digest[0]      = 0x67452301;
    context->Message_Digest[1]      = 0xEFCDAB89;
    context->Message_Digest[2]      = 0x98BADCFE;
    context->Message_Digest[3]      = 0x10325476;
    context->Message_Digest[4]      = 0xC3D2E1F0;

    context->Computed   = 0;
    context->Corrupted  = 0;
}

/*
 *  SHA1Result
 *
 *  Description:
 *      This function will return the 160-bit message digest into the
 *      Message_Digest array within the SHA1Context provided
 *
 *  Parameters:
 *      context: [in/out]
 *          The context to use to calculate the SHA-1 hash.
 *
 *  Returns:
 *      1 if successful, 0 if it failed.
 *
 *  Comments:
 *
 */
static int SHA1Result(SHA1Context *context)
{

    if (context->Corrupted)
    {
        return 0;
    }

    if (!context->Computed)
    {
        SHA1PadMessage(context);
        context->Computed = 1;
    }

    return 1;
}

/*
 *  SHA1Input
 *
 *  Description:
 *      This function accepts an array of octets as the next portion of
 *      the message.
 *
 *  Parameters:
 *      context: [in/out]
 *          The SHA-1 context to update
 *      message_array: [in]
 *          An array of characters representing the next portion of the
 *          message.
 *      length: [in]
 *          The length of the message in message_array
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *
 */
static void SHA1Input(     SHA1Context         *context,
                    const unsigned char *message_array,
                    unsigned            length)
{
    if (!length)
    {
        return;
    }

    if (context->Computed || context->Corrupted)
    {
        context->Corrupted = 1;
        return;
    }

    while(length-- && !context->Corrupted)
    {
        context->Message_Block[context->Message_Block_Index++] =
                                                (*message_array & 0xFF);

        context->Length_Low += 8;
        /* Force it to 32 bits */
        context->Length_Low &= 0xFFFFFFFF;
        if (context->Length_Low == 0)
        {
            context->Length_High++;
            /* Force it to 32 bits */
            context->Length_High &= 0xFFFFFFFF;
            if (context->Length_High == 0)
            {
                /* Message is too long */
                context->Corrupted = 1;
            }
        }

        if (context->Message_Block_Index == 64)
        {
            SHA1ProcessMessageBlock(context);
        }

        message_array++;
    }
}

/*
 *  SHA1ProcessMessageBlock
 *
 *  Description:
 *      This function will process the next 512 bits of the message
 *      stored in the Message_Block array.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      Many of the variable names in the SHAContext, especially the
 *      single character names, were used because those were the names
 *      used in the publication.
 *
 *
 */
static void SHA1ProcessMessageBlock(SHA1Context *context)
{
    const unsigned K[] =            /* Constants defined in SHA-1   */
    {
        0x5A827999,
        0x6ED9EBA1,
        0x8F1BBCDC,
        0xCA62C1D6
    };
    int         t;                  /* Loop counter                 */
    unsigned    temp;               /* Temporary word value         */
    unsigned    W[80];              /* Word sequence                */
    unsigned    A, B, C, D, E;      /* Word buffers                 */

    /*
     *  Initialize the first 16 words in the array W
     */
    for(t = 0; t < 16; t++)
    {
        W[t] = ((unsigned) context->Message_Block[t * 4]) << 24;
        W[t] |= ((unsigned) context->Message_Block[t * 4 + 1]) << 16;
        W[t] |= ((unsigned) context->Message_Block[t * 4 + 2]) << 8;
        W[t] |= ((unsigned) context->Message_Block[t * 4 + 3]);
    }

    for(t = 16; t < 80; t++)
    {
       W[t] = SHA1CircularShift(1,W[t-3] ^ W[t-8] ^ W[t-14] ^ W[t-16]);
    }

    A = context->Message_Digest[0];
    B = context->Message_Digest[1];
    C = context->Message_Digest[2];
    D = context->Message_Digest[3];
    E = context->Message_Digest[4];

    for(t = 0; t < 20; t++)
    {
        temp =  SHA1CircularShift(5,A) +
                ((B & C) | ((~B) & D)) + E + W[t] + K[0];
        temp &= 0xFFFFFFFF;
        E = D;
        D = C;
        C = SHA1CircularShift(30,B);
        B = A;
        A = temp;
    }

    for(t = 20; t < 40; t++)
    {
        temp = SHA1CircularShift(5,A) + (B ^ C ^ D) + E + W[t] + K[1];
        temp &= 0xFFFFFFFF;
        E = D;
        D = C;
        C = SHA1CircularShift(30,B);
        B = A;
        A = temp;
    }

    for(t = 40; t < 60; t++)
    {
        temp = SHA1CircularShift(5,A) +
               ((B & C) | (B & D) | (C & D)) + E + W[t] + K[2];
        temp &= 0xFFFFFFFF;
        E = D;
        D = C;
        C = SHA1CircularShift(30,B);
        B = A;
        A = temp;
    }

    for(t = 60; t < 80; t++)
    {
        temp = SHA1CircularShift(5,A) + (B ^ C ^ D) + E + W[t] + K[3];
        temp &= 0xFFFFFFFF;
        E = D;
        D = C;
        C = SHA1CircularShift(30,B);
        B = A;
        A = temp;
    }

    context->Message_Digest[0] =
                        (context->Message_Digest[0] + A) & 0xFFFFFFFF;
    context->Message_Digest[1] =
                        (context->Message_Digest[1] + B) & 0xFFFFFFFF;
    context->Message_Digest[2] =
                        (context->Message_Digest[2] + C) & 0xFFFFFFFF;
    context->Message_Digest[3] =
                        (context->Message_Digest[3] + D) & 0xFFFFFFFF;
    context->Message_Digest[4] =
                        (context->Message_Digest[4] + E) & 0xFFFFFFFF;

    context->Message_Block_Index = 0;
}

/*
 *  SHA1PadMessage
 *
 *  Description:
 *      According to the standard, the message must be padded to an even
 *      512 bits.  The first padding bit must be a '1'.  The last 64
 *      bits represent the length of the original message.  All bits in
 *      between should be 0.  This function will pad the message
 *      according to those rules by filling the Message_Block array
 *      accordingly.  It will also call SHA1ProcessMessageBlock()
 *      appropriately.  When it returns, it can be assumed that the
 *      message digest has been computed.
 *
 *  Parameters:
 *      context: [in/out]
 *          The context to pad
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *
 */
static void SHA1PadMessage(SHA1Context *context)
{
    /*
     *  Check to see if the current message block is too small to hold
     *  the initial padding bits and length.  If so, we will pad the
     *  block, process it, and then continue padding into a second
     *  block.
     */
    if (context->Message_Block_Index > 55)
    {
        context->Message_Block[context->Message_Block_Index++] = 0x80;
        while(context->Message_Block_Index < 64)
        {
            context->Message_Block[context->Message_Block_Index++] = 0;
        }

        SHA1ProcessMessageBlock(context);

        while(context->Message_Block_Index < 56)
        {
            context->Message_Block[context->Message_Block_Index++] = 0;
        }
    }
    else
    {
        context->Message_Block[context->Message_Block_Index++] = 0x80;
        while(context->Message_Block_Index < 56)
        {
            context->Message_Block[context->Message_Block_Index++] = 0;
        }
    }

    /*
     *  Store the message length as the last 8 octets
     */
    context->Message_Block[56] = (context->Length_High >> 24) & 0xFF;
    context->Message_Block[57] = (context->Length_High >> 16) & 0xFF;
    context->Message_Block[58] = (context->Length_High >> 8) & 0xFF;
    context->Message_Block[59] = (context->Length_High) & 0xFF;
    context->Message_Block[60] = (context->Length_Low >> 24) & 0xFF;
    context->Message_Block[61] = (context->Length_Low >> 16) & 0xFF;
    context->Message_Block[62] = (context->Length_Low >> 8) & 0xFF;
    context->Message_Block[63] = (context->Length_Low) & 0xFF;

    SHA1ProcessMessageBlock(context);
}


/** ----- OBase ----- */
static const char* __id( void* inst ) {
  return NULL;
}

static void* __event( void* inst, const void* evt ) {
  return NULL;
}

static void __del( void* inst ) {
  return;
}

static const char* __name( void ) {
  return name;
}

static unsigned char* __serialize( void* inst, long* size ) {
  return NULL;
}

static void __deserialize( void* inst,unsigned char* bytestream ) {
  return;
}

static char* __toString( void* inst ) {
  return NULL;
}

static int __count( void ) {
  return instCnt;
}

static struct OBase* __clone( void* inst ) {
  return NULL;
}

static Boolean __equals( void* inst1, void* inst2 ) {
  return False;
}

static void* __properties( void* inst ) {
  return NULL;
}

/** ----- OMime64 ----- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
                  "0123456789+/";

enum TOKENTYPE { NONE, BLANKS, PUNCT, TAG, NAME, CONTENT };

struct TOKEN {
                char *text;
                int  length;
                int  index;
                enum TOKENTYPE type;
             };

static int compare_token( struct TOKEN *token, char *text )
{
   int index=0;
   int count;
   int result;
   char blivit1, blivit2;

   count = token->length;

   if ( count > 0 )
   {
      result = 1;
   }
   else
   {
      result = 0;
   }

   while ( (count > 0) && ( result != 0 ) )
   {
      blivit1 = token->text[index++];
      if ( (blivit1 >= 'a' ) && (blivit1 <= 'z') )
      {
         blivit1 -= ' ';
      }

      blivit2 = *text++;
      if ( (blivit2 >= 'a' ) && (blivit2 <= 'z') )
      {
         blivit2 -= ' ';
      }

      if ( blivit1 != blivit2 )
      {
         result = 0;
      }

      count--;
   }

   return result;
}

static int __ispunct( char blivit )
{
   if ( ( blivit >= 'a' ) && (blivit <= 'z' ) )
   {
      blivit -= ' ';
   }

   if ( ( ( blivit < '0' ) ||
          ( ( blivit > '9' ) && (blivit < 'A') ) ||
          ( blivit > 'Z' ) ) &&
        ( blivit != '-') && (blivit != '/') && (blivit != '.') )
   {
      return 1;
   }
   else
   {
      return 0;
   }
}

static void fixname( char *name )
{
   while ( *name != '\0' )
   {

     if (__ispunct( *name ) )
     {
        *name = '\0';
     }

     name++;
   }
}

static void acquire_token( char *line, enum TOKENTYPE type, struct TOKEN *token )
{
   int doneflag=0, startflag=1;
   int index;
   enum TOKENTYPE nextstate=NONE;
   char blivit;

   if (token->type == NONE)
   {
      token->index = 0;
      token->length = 0;
   }

   index = token->index + token->length;

   token->text = 0;

   while ( doneflag == 0 )
   {
      blivit = line[index];
      if ( (blivit >= 'a') && (blivit <= 'z') )
      {
         blivit -= ' ';
      }

      switch (token->type)
      {
         case NONE:
         if ( blivit == ' ')
         {
            index++;
            token->index++;
         }
         else
         {
            token->type = TAG;
            nextstate = TAG;
         }
         break;

         case BLANKS:
         if      ( blivit == ' ')
         {
            index++;
         }
         else if ( __ispunct( blivit ) )
         {
            token->type = PUNCT;
            token->index = index;
         }
         else
         {
            token->type = nextstate;
            token->index = index;
         }
         break;

         case PUNCT:
         if      ( blivit <  ' ')
         {
            doneflag = 1;
            token->type = NONE;
            token->index = index;
            token->text = line + index;
            token->length = 0;
         }
         else if ( blivit == ' ' )
         {
            token->type = BLANKS;
            token->index = index;
            if      ( line[ token->index ] == ';' )
            {
               nextstate = NAME;
            }
            else if ( line[ token->index ] == '=' )
            {
               nextstate = CONTENT;
            }

         }
         else if ( __ispunct( blivit ) )
         {
            index++;
         }
         else
         {
            if      ( line[ token->index ] == ';' )
            {
               nextstate = NAME;
            }
            else if ( line[ token->index ] == '=' )
            {
               nextstate = CONTENT;
            }

            token->type = nextstate;
            token->index = index;
         }
         break;

         case TAG:
         if ( __ispunct( blivit ) )
         {
            token->length = index - token->index;
            token->text = line + token->index;
            nextstate = NAME;

            if ( ( ( type == TAG ) || ( type == NONE ) ) && !startflag)
            {
               doneflag = 1;
            }
            else if (blivit == ' ')
            {
               token->type = BLANKS;
               token->index = index;
            }
            else
            {
               token->type = PUNCT;
               token->index = index;
            }
         }
         else
         {
            index++;
         }
         break;

         case NAME:
         if ( __ispunct( blivit ) )
         {
            token->length = index - token->index;
            token->text = line + token->index;

            if ( blivit != ';' )
            {
               nextstate = CONTENT;
            }
            else
            {
               nextstate = NAME;
            }

            if ( ( ( type == NAME ) || ( type == NONE ) ) && !startflag )
            {
               doneflag = 1;
            }
            else if (blivit == ' ')
            {
               token->type = BLANKS;
               token->index = index;
            }
            else
            {
               token->type = PUNCT;
               token->index = index;
            }
         }
         else
         {
            index++;
         }
         break;

         case CONTENT:
         if ( __ispunct( blivit ) )
         {
            token->length = index - token->index;
            token->text = line + token->index;
            nextstate = NAME;

            if ( ( ( type == CONTENT ) || ( type == NONE ) ) && !startflag )
            {
               doneflag = 1;
            }
            else if (blivit == ' ')
            {
               token->type = BLANKS;
               token->index = index;
            }
            else
            {
               token->type = PUNCT;
               token->index = index;
            }
         }
         else
         {
            index++;
         }
         break;
      }
      startflag = 0;
   }
}

static void fputch( char blivit, FILE *f )
{
/*   if (blivit == '\n') fputc( '\r', f );*/
   fputc( blivit, f );
}

static int cvt_ascii( unsigned char alpha )
{
   if      ( (alpha >= 'A') && (alpha <= 'Z') ) return (int)(alpha - 'A');
   else if ( (alpha >= 'a') && (alpha <= 'z') )
        return 26 + (int)(alpha - 'a');
   else if ( (alpha >= '0') && (alpha <= '9' ) )
        return 52 + (int)(alpha - '0');
   else if ( alpha == '+' ) return 62;
   else if ( alpha == '/' ) return 63;
   else if ( alpha == '=' ) return -2;
   else                     return -1;
}

enum { ENCODE, DECODE };

static int __mime64( const char* infile, const char* outfile, int action )
{
   int n_options, n_files, index, jndex, shift, save_shift;
   int whattodo = action;
   int help_flag = 0, replace_flag = 0, perm_replace_flag = 0, quit = 0;
   int cycle_flag = 0;
   FILE *fin, *fout, *dummy;
   unsigned char blivit;
   unsigned long accum, value;
   char buf[80], dumname[80];
   char *cptr, *altptr;
   int decode_state;
   struct TOKEN token;
   int firsttime = 1;
   int skipflag = 0;
   int printmsg = 1;

   n_files = 2;

   if ( whattodo == DECODE )
   {
      fin = fopen( infile, "r" );
   }
   else
   {
      fin = fopen( infile, "rb" );
   }

   if ( fin == 0 )
   {
      printf( "%s file not found\n", infile );
      return -1;
   }

    if ( whattodo == DECODE )
    {
       sprintf( dumname, "%s", outfile );
    }
    else
    {
       fout = fopen( outfile, "w" );
    }

    if ( fout == 0 )
    {
       printf( "Couldn't open %s for output\n", outfile );
       return -1;
    }


  do {
   quit = 0;
   printmsg = 1;

   if ( whattodo == DECODE )
   {
      shift = 0;
      accum = 0;
      decode_state = 0;

      while ( ( !feof( fin ) ) && (quit == 0) )
      {
         fgets( buf, 80, fin );
         if ( feof( fin ) )
         {
            if ( ( dumname[0] != '\0' ) && ( shift != 0 ) )
            {
               printf( "Unexpected end of file encountered in %s\n"
                       "last few bytes may have been lost\n", dumname );
               quit = 1;
               decode_state = 1;
               continue;
            }
            else if ( cycle_flag == 0 )
            {
               quit = 1;
               decode_state = 1;
               continue;
            }
         }
         else
         {
            cycle_flag = 1;

            if ( (decode_state == 1) &&
                 ( (buf[0] == '\n') || (buf[0] < '+') ) )
            {
               quit = 1;

               if ( shift != 0 )
               {
                  printf( "Unexpected end of section in %s\n"
                          "last few bytes may have been lost\n", dumname );
               }

               continue;
            }
         }


         if ( decode_state == 0 )
         {
            for ( index = 0;
                  (buf[index] != '\n') && (buf[index] != '\0') &&
                  (decode_state >= 0);
                  index++ )
            {
               if ( ( (buf[index] >= 'A') && (buf[index] <= 'Z') ) ||
                    ( (buf[index] >= 'a') && (buf[index] <= 'z') ) ||
                    ( (buf[index] >= '0') && (buf[index] <= '9') ) ||
                    (buf[index] == '+') ||
                    (buf[index] == '/') ||
                    (buf[index] == '=') )
               {
                  decode_state = 1;
               }
               else
               {
                  decode_state = -2;
               }
            }

            if ( decode_state <= 0 )
            {

               decode_state = 0;
               token.type = NONE;

               acquire_token( buf, TAG, &token );
               if      ( compare_token( &token, "Content-Type") )
               {
                  do
                  {
                     acquire_token( buf, NAME, &token );
                     if ( compare_token( &token, "name" ) )
                     {
                        acquire_token( buf, CONTENT, &token );

                        if ( ( replace_flag ) ||
                             ( firsttime == 0 ) )
                        {
                           sscanf( token.text, "%s", dumname );
                           fixname( dumname );

                           if ( strcasecmp( dumname, infile ) != 0 )
                           {
                              replace_flag = 0;
                           }
                           else
                           {
                              if ( perm_replace_flag )
                              {
                                 printf( 
                                 "More than one output file named %s\n",
                                 dumname );

                                 return -1;
                              }
                           }
                        }
                     }
                  } while ( token.type != NONE );
               }
               else if ( compare_token( &token, "Content-transfer-encoding" ) )
               {
                  skipflag = 1;

                  do
                  {
                     acquire_token( buf, NAME, &token );
                     if ( compare_token( &token, "base64" ) )
                     {
                        skipflag = 0;
                     }
                  } while ( token.type != NONE );
               }
               continue;
            }
            else if ( skipflag != 0 )
            {
               continue;
            }
         }

         if ( printmsg )
         {
            if ( skipflag )
            {
               printf( "Section %s not MIME base64\n", dumname );
            }
            else
            {
               printf( "Creating %s\n", dumname );
               if ( strcasecmp( dumname, infile ) == 0 )
               {
                  replace_flag = 1;
               }

               if ( replace_flag )
               {
                  fout = fopen( "$$$$$$$$.$$$", "wb" );
               }
               else
               {
                  fout = fopen( dumname, "wb" );
               }

               if ( fout == 0 )
               {
                  printf( "Couldn't open %s for output\n", dumname );
               }
            }

            printmsg = 0;
         }

         if ( fout == 0 )
         {
            printf( "No filename given for subsequent section\n" );
            return -1;
         }

         if ( feof(fin) )
         {
            quit = 1;
         }

         if ( quit != 0 )
         {
            buf[0] = '\0';
         }

         for ( index = 0; (buf[index] != '\n') && (buf[index] != '\0'); index++)
         {
            value = cvt_ascii( buf[index] );

            if ( value < 64 )
            {
               accum <<= 6;
               shift += 6;
               accum |= value;
               if ( shift >= 8 )
               {
                  shift -= 8;
                  value = accum >> shift;
                  blivit = (unsigned char)value & 0xFFl;
                  fputc( blivit, fout );
               }
            }
            else
            {
               quit = 1;
               break;
            }
         }
      }
   }
   else
   {
      /*
      fprintf ( fout,
       "Content-Type: text/plain; charset=US-ASCII; name=%s\n"
       "Content-transfer-encoding: base64\n\n", infile );
      */
      shift = 0;
      accum = 0;
      index = 0;
      while ( ( !feof( fin ) ) || (shift != 0) )
      {
         if ( ( !feof( fin ) ) && ( quit == 0 ) )
         {
            blivit = fgetc( fin );

            if ( feof( fin ) )
            {
               quit = 1;
               save_shift = shift;
               blivit = 0;
            }
         }
         else
         {
            quit = 1;
            save_shift = shift;
            blivit = 0;
         }

         if ( (quit == 0) || (shift != 0) )
         {
            value = (unsigned long)blivit;
            accum <<= 8;
            shift += 8;
            accum |= value;
         } /* ENDIF */

         while ( shift >= 6 )
         {
            shift -= 6;
            value = (accum >> shift) & 0x3Fl;
            blivit = alphabet[value];

            buf[index++] = blivit;
            if ( index >= 60 )
            {
               buf[index] = '\0';
               fprintf( fout, "%s\n", buf );
               index = 0;
            }

            if ( quit != 0 )
            {
               shift = 0;
            }
         }
      }

      if      ( save_shift == 2 )
      {
         buf[index++] = '=';
         if ( index >= 60 )
         {
            buf[index] = '\0';
            fprintf( fout, "%s\n", buf );
            index = 0;
         }

         buf[index++] = '=';
         if ( index >= 60 )
         {
            buf[index] = '\0';
            fprintf( fout, "%s\n", buf );
            index = 0;
         }
      }
      else if ( save_shift == 4 )
      {
         buf[index++] = '=';
         if ( index >= 60 )
         {
            buf[index] = '\0';
            fprintf( fout, "%s\n", buf );
            index = 0;
         }
      }

      if ( index != 0 )
      {
         buf[index] = '\0';
         fprintf( fout, "%s\n", buf );
      }
   }

   fclose( fout );

   if ( replace_flag )
   {
      perm_replace_flag = 1;

      if ( ( whattodo == DECODE ) && ( decode_state <= 0 ) )
      {
         remove( "$$$$$$$$.$$$" );
         printf( "No MIME base64 lines found in %s\n", infile );
      }
   }
   else
   {
      if ( ( whattodo == DECODE ) && ( decode_state <= 0 ) )
      {
         remove( outfile );
         printf( "No MIME base64 lines found in %s\n", infile );
      }
   }

   fout = 0;
   firsttime = 0;
   dumname[0] = '\0';
   cycle_flag = 0;

  } while ( !feof( fin ) );


  fclose( fin );

  return 0;
}



static const unsigned char base64_enc_map[64] =
{
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
    'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
    'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
    'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
    'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', '+', '/'
};

static const unsigned char base64_dec_map[128] =
{
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127,  62, 127, 127, 127,  63,  52,  53,
     54,  55,  56,  57,  58,  59,  60,  61, 127, 127,
    127,  64, 127, 127, 127,   0,   1,   2,   3,   4,
      5,   6,   7,   8,   9,  10,  11,  12,  13,  14,
     15,  16,  17,  18,  19,  20,  21,  22,  23,  24,
     25, 127, 127, 127, 127, 127, 127,  26,  27,  28,
     29,  30,  31,  32,  33,  34,  35,  36,  37,  38,
     39,  40,  41,  42,  43,  44,  45,  46,  47,  48,
     49,  50,  51, 127, 127, 127, 127, 127
};

/*
 * Encode a buffer into base64 format
 */
static int base64_encode( unsigned char *dst, int *dlen, const unsigned char *src, int slen )
{
    size_t i, n;
    int C1, C2, C3;
    unsigned char *p;

    if( slen == 0 )
    {
        *dlen = 0;
        return( 0 );
    }

    n = ( slen << 3 ) / 6;

    switch( ( slen << 3 ) - ( n * 6 ) )
    {
        case  2: n += 3; break;
        case  4: n += 2; break;
        default: break;
    }

    if( *dlen < n + 1 )
    {
        *dlen = n + 1;
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "decode: buffer too small %d", *dlen);
        return -1;
    }

    n = ( slen / 3 ) * 3;

    for( i = 0, p = dst; i < n; i += 3 )
    {
        C1 = *src++;
        C2 = *src++;
        C3 = *src++;

        *p++ = base64_enc_map[(C1 >> 2) & 0x3F];
        *p++ = base64_enc_map[(((C1 &  3) << 4) + (C2 >> 4)) & 0x3F];
        *p++ = base64_enc_map[(((C2 & 15) << 2) + (C3 >> 6)) & 0x3F];
        *p++ = base64_enc_map[C3 & 0x3F];
    }

    if( i < slen )
    {
        C1 = *src++;
        C2 = ( ( i + 1 ) < slen ) ? *src++ : 0;

        *p++ = base64_enc_map[(C1 >> 2) & 0x3F];
        *p++ = base64_enc_map[(((C1 & 3) << 4) + (C2 >> 4)) & 0x3F];

        if( ( i + 1 ) < slen )
             *p++ = base64_enc_map[((C2 & 15) << 2) & 0x3F];
        else *p++ = '=';

        *p++ = '=';
    }

    *dlen = p - dst;
    *p = 0;

    return( 0 );
}

/*
 * Decode a base64-formatted buffer
 */
static int base64_decode( unsigned char *dst, int *dlen, const unsigned char *src, int slen )
{
    int i, n;
    unsigned int j, x;
    unsigned char *p;

    /* First pass: check for validity and get output length */
    for( i = n = j = 0; i < slen; i++ )
    {
        /* Skip spaces before checking for EOL */
        x = 0;
        while( i < slen && src[i] == ' ' )
        {
            ++i;
            ++x;
        }

        /* Spaces at end of buffer are OK */
        if( i == slen )
            break;

        if( ( slen - i ) >= 2 &&
            src[i] == '\r' && src[i + 1] == '\n' )
            continue;

        if( src[i] == '\n' )
            continue;

        /* Space inside a line is an error */
        if( x != 0 ) {
          TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "decode: invalid character");
          return -1;
        }

        if( src[i] == '=' && ++j > 2 ) {
          TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "decode: invalid character %X", src[i]);
          return -1;
        }

        if( src[i] > 127 || base64_dec_map[src[i]] == 127 ) {
          TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "decode: invalid character %X", src[i]);
          return -1;
        }

        if( base64_dec_map[src[i]] < 64 && j != 0 ) {
          TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "decode: invalid character %X", src[i]);
          return -1;
        }

        n++;
    }

    if( n == 0 )
        return( 0 );

    n = ( ( n * 6 ) + 7 ) >> 3;
    n -= j;

    if( dst == NULL || *dlen < n )
    {
        *dlen = n;
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "decode: buffer too small dlen=%d n=%d", *dlen, n);
        return -1;
    }

   for( j = 3, n = x = 0, p = dst; i > 0; i--, src++ )
   {
        if( *src == '\r' || *src == '\n' || *src == ' ' )
            continue;

        j -= ( base64_dec_map[*src] == 64 );
        x  = ( x << 6 ) | ( base64_dec_map[*src] & 0x3F );

        if( ++n == 4 )
        {
            n = 0;
            if( j > 0 ) *p++ = (unsigned char)( x >> 16 );
            if( j > 1 ) *p++ = (unsigned char)( x >>  8 );
            if( j > 2 ) *p++ = (unsigned char)( x       );
        }
    }

    *dlen = p - dst;

    return( 0 );
}



/** decode the input file into it's  */
static Boolean _decodeFile( const char* infile ,const char* outfile ) {
  return __mime64( infile, outfile, DECODE );
}


/** encode the input file into mime64 */
static Boolean _encodeFile( const char* infile ,const char* outfile ) {
  return __mime64( infile, outfile, ENCODE );
}

static int _decode( unsigned char *dst, int *dlen, const unsigned char *src, int slen ) {
  return base64_decode( dst, dlen, src, slen );
}

static int _encode( unsigned char *dst, int *dlen, const unsigned char *src, int slen ) {
  return base64_encode( dst, dlen, src, slen );
}

static char* _sha1(const char* str) {
  SHA1Context sha;
  int i;

  SHA1Reset(&sha);
  SHA1Input(&sha, (const unsigned char *) str, strlen(str));

  if( SHA1Result(&sha) ) {
     char* result = StrOp.fmt("%08X%08X%08X%08X%08X",
         sha.Message_Digest[0], sha.Message_Digest[1], sha.Message_Digest[2], sha.Message_Digest[3], sha.Message_Digest[4]);
     TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SHA1 = %s", result);
     return result;
  }
  return NULL;
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocs/impl/mime64.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
