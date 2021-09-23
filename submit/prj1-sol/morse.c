#include "morse.h"

#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <stddef.h>
#include <string.h>

typedef struct {
  char c;
  const char *code;
} TextMorse;

//<https://en.wikipedia.org/wiki/Morse_code#/media/File:International_Morse_Code.svg>
static const TextMorse charCodes[] = {
  { 'A', ".-" },
  { 'B', "-..." },
  { 'C', "-.-." },
  { 'D', "-.." },
  { 'E', "." },
  { 'F', "..-." },
  { 'G', "--." },
  { 'H', "...." },
  { 'I', ".." },
  { 'J', ".---" },
  { 'K', "-.-" },
  { 'L', ".-.." },
  { 'M', "--" },
  { 'N', "-." },
  { 'O', "---" },
  { 'P', ".--." },
  { 'Q', "--.-" },
  { 'R', ".-." },
  { 'S', "..." },
  { 'T', "-" },
  { 'U', "..-" },
  { 'V', "...-" },
  { 'W', ".--" },
  { 'X', "-..-" },
  { 'Y', "-.--" },
  { 'Z', "--.." },

  { '1', ".----" },
  { '2', "..---" },
  { '3', "...--" },
  { '4', "....-" },
  { '5', "....." },
  { '6', "-...." },
  { '7', "--..." },
  { '8', "---.." },
  { '9', "----." },
  { '0', "-----" },

  { '\0', ".-.-." }, //AR Prosign indicating End-of-message
                     //<https://en.wikipedia.org/wiki/Prosigns_for_Morse_code>
};


/** Return NUL-terminated code string for char c. Returns NULL if
 *  there is no code for c.
 */
static const char *
charToCode(Byte c) {
  for (int i = 0; i < sizeof(charCodes)/sizeof(charCodes[0]); i++) {
    if (charCodes[i].c == c) return charCodes[i].code;
  }
  return NULL;
}


/** Return char for code. Returns < 0 if code is invalid.
 */
static int
codeToChar(const char *code) {
  for (int i = 0; i < sizeof(charCodes)/sizeof(charCodes[0]); i++) {
    if (strcmp(charCodes[i].code, code) == 0) return charCodes[i].c;
  }
  return -1;
}

/** Given an array of Bytes, a bit index is the offset of a bit
 *  in the array (with MSB having offset 0).
 *
 *  Given a bytes[] array and some bitOffset, and assuming that
 *  BITS_PER_BYTE is 8, then (bitOffset >> 3) represents the index of
 *  the byte within bytes[] and (bitOffset & 0x7) gives the bit-index
 *  within the byte (MSB represented by bit-index 0) and .
 *
 *  For example, given array a[] = {0xB1, 0xC7} which is
 *  { 0b1011_0001, 0b1100_0111 } we have the following:
 *
 *     Bit-Offset   Value
 *        0           1
 *        1           0
 *        2           1
 *        3           1
 *        4           0
 *        5           0
 *        6           0
 *        7           1
 *        8           1
 *        9           1
 *       10           0
 *       11           0
 *       12           0
 *       13           1
 *       14           1
 *       15           1
 *
 */


/** Return mask for a Byte with bit at bitIndex set to 1, all other
 *  bits set to 0.  Note that bitIndex == 0 represents the MSB,
 *  bitIndex == 1 represents the next significant bit and so on.
 */
static inline unsigned
byteBitMask(unsigned bitIndex)
{
  return (1 << (BITS_PER_BYTE - 1 - bitIndex));
}

/** Given a power-of-2 powerOf2, return log2(powerOf2) */
static inline unsigned
getLog2PowerOf2(unsigned powerOf2)
{
  unsigned count = 0;
  unsigned one = 1;
  for (; one != powerOf2; count++){
    one = one << 1;
  }
  return count;
}

/** Given a bitOffset return the bitIndex part of the bitOffset. */
static inline unsigned
getBitIndex(unsigned bitOffset)
{
  return (bitOffset & (BITS_PER_BYTE - 1));
}

/** Given a bitOffset return the byte offset part of the bitOffset */
static inline unsigned
getOffset(unsigned bitOffset)
{
  return (bitOffset >> getLog2PowerOf2(BITS_PER_BYTE));
}

/** Return bit at offset bitOffset in array[]; i.e., return
 *  (bits(array))[bitOffset]
 */
static inline int
getBitAtOffset(const Byte array[], unsigned bitOffset)
{
  int bit = array[getOffset(bitOffset)];
  bit = bit & byteBitMask(getBitIndex(bitOffset));
  bit = bit >> (BITS_PER_BYTE - 1 - getBitIndex(bitOffset));
  return bit;
}

/** Set bit selected by bitOffset in array to bit. */
static inline void
setBitAtOffset(Byte array[], unsigned bitOffset, unsigned bit)
{
  unsigned mask = byteBitMask(getBitIndex(bitOffset));
  unsigned byteIndex = getOffset(bitOffset);
  if (bit == 1) {
    array[byteIndex] = array[byteIndex] | mask;
  }
  else if (bit == 0) {
    array[byteIndex] = array[byteIndex] & ~mask;
  }
  return;
}

/** Set count bits in array[] starting at bitOffset to bit.  Return
 *  bit-offset one beyond last bit set.
 */
static inline unsigned
setBitsAtOffset(Byte array[], unsigned bitOffset, unsigned bit, unsigned count)
{
  for (; count > 0; count--){
    setBitAtOffset(array, bitOffset, bit);
    bitOffset++;
  }
  return (bitOffset);
}


/** Convert text[nText] into a binary encoding of morse code in
 *  morse[].  It is assumed that array morse[] is initially all zero
 *  and is large enough to represent the morse code for all characters
 *  in text[].  The result in morse[] should be terminated by the
 *  morse prosign AR.  Any sequence of non-alphanumeric characters in
 *  text[] should be treated as a *single* inter-word space.  Leading
 *  non alphanumeric characters in text are ignored.
 *
 *  Returns count of number of bytes used within morse[].
 */
int
textToMorse(const Byte text[], unsigned nText, Byte morse[])
{
  unsigned bitOffset = 0;
  unsigned i = 0;
  while (isalnum(text[i]) == 0){
    i++;
  }
  for (; i <= nText; i++){
    const char* code = charToCode(text[i]);
    if (code == NULL){
      bitOffset = setBitsAtOffset(morse, bitOffset, 0, 4);
      continue;
    }
    unsigned codeCharIndex = 0;
    while (code[codeCharIndex] != '\0'){
      char codeChar = code[codeCharIndex];
      if (codeChar == '.') {
	setBitAtOffset(morse, bitOffset, 1);
	bitOffset++;
	setBitAtOffset(morse, bitOffset, 0);
	bitOffset++;
      }
      else if (codeChar == '-'){
	bitOffset = setBitsAtOffset(morse, bitOffset, 1, 3);
	setBitAtOffset(morse, bitOffset, 0);
	bitOffset++;
      }
      codeCharIndex++;
    }
    bitOffset = setBitsAtOffset(morse, bitOffset, 0, 2); 
  }
  return (getOffset(bitOffset) + 1);
}

/** Return count of run of identical bits starting at bitOffset
 *  in bytes[nBytes].
 */
static inline unsigned
runLength(const Byte bytes[], unsigned nBytes, unsigned bitOffset)
{
  unsigned count = 1;
  int bit = getBitAtOffset(bytes, bitOffset);
  bitOffset++;
  for (; getOffset(bitOffset) != nBytes; bitOffset++){
    if (getBitAtOffset(bytes, bitOffset) == bit){
      count++;
    }
    else {
      break;
    }
  }
  return count;
}


/** Convert AR-prosign terminated binary Morse encoding in
 *  morse[nMorse] into text in text[].  It is assumed that array
 *  text[] is large enough to represent the decoding of the code in
 *  morse[].  Leading zero bits in morse[] are ignored. Encodings
 *  representing word separators are output as a space ' ' character.
 *
 *  Returns count of number of bytes used within text[], < 0 on error.
 */
int
morseToText(const Byte morse[], unsigned nMorse, Byte text[])
{
  unsigned bitOffset = 0;
  unsigned textIndex = 0;
  char code[6];
  code[5] = '\0';
  unsigned codeIndex = 0;
  while (getOffset(bitOffset) < nMorse){
    int bit = getBitAtOffset(morse, bitOffset);
    if (bit == 0){
      unsigned run = runLength(morse, nMorse, bitOffset);
      if (run == 2 || run == 6){
	code[codeIndex] = '\0';
	text[textIndex] = codeToChar(code);
	if (run == 6) {
	  textIndex++;
	  text[textIndex] = ' ';
	}
      }
      else {
	code[codeIndex] = '\0';
	if (codeToChar(code) == '\0'){
	  text[textIndex] = '\0';
	  break;
	}
	else return -1;
      }

      textIndex++;
      codeIndex = 0;
      bitOffset += run;
    }
    else if (bit == 1){
      unsigned run = runLength(morse, nMorse, bitOffset);
      if (run == 1){
	code[codeIndex] = '.';
      }
      else if (run == 3){
	code[codeIndex] = '-';
      }
      else {
	return -1;
      }
      codeIndex++;
      bitOffset += run + 1;
    }
  }
  return (textIndex);
}
