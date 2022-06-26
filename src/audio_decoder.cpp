#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "audio_decoder.h"

#define PTR_LOBYTE (UINTPTR_MAX >> ((CHAR_BIT * sizeof(void *)) / 2))
#define reverse_bytes_16(num) (((num & 0xFF00) >> 8) | ((num & 0x00FF) << 8))
#define HIBYTE(x) (x >> 8)
#define LOBYTE(x) (x << 8)
#define HI_NIBBLE(b) (((b) >> 4) & 0x0F)
#define LO_NIBBLE(b) ((b)&0x0F)

static int8_t gLookupTable[] = {
  (int8_t)0xf8, (int8_t)0xf9, (int8_t)0xfa, (int8_t)0xfb, (int8_t)0xfc, (int8_t)0xfd, (int8_t)0xfe, (int8_t)0xff,
  (int8_t)0x01, (int8_t)0x02, (int8_t)0x03, (int8_t)0x04, (int8_t)0x05, (int8_t)0x06, (int8_t)0x07, (int8_t)0x08,
  (int8_t)0xf0, (int8_t)0xf2, (int8_t)0xf4, (int8_t)0xf6, (int8_t)0xf8, (int8_t)0xfa, (int8_t)0xfc, (int8_t)0xfe,
  (int8_t)0x02, (int8_t)0x04, (int8_t)0x06, (int8_t)0x08, (int8_t)0x0a, (int8_t)0x0c, (int8_t)0x0e, (int8_t)0x10,
  (int8_t)0xe8, (int8_t)0xeb, (int8_t)0xee, (int8_t)0xf1, (int8_t)0xf4, (int8_t)0xf7, (int8_t)0xfa, (int8_t)0xfd,
  (int8_t)0x03, (int8_t)0x06, (int8_t)0x09, (int8_t)0x0c, (int8_t)0x0f, (int8_t)0x12, (int8_t)0x15, (int8_t)0x18,
  (int8_t)0xe0, (int8_t)0xe4, (int8_t)0xe8, (int8_t)0xec, (int8_t)0xf0, (int8_t)0xf4, (int8_t)0xf8, (int8_t)0xfc,
  (int8_t)0x04, (int8_t)0x08, (int8_t)0x0c, (int8_t)0x10, (int8_t)0x14, (int8_t)0x18, (int8_t)0x1c, (int8_t)0x20,
  (int8_t)0xd8, (int8_t)0xdd, (int8_t)0xe2, (int8_t)0xe7, (int8_t)0xec, (int8_t)0xf1, (int8_t)0xf6, (int8_t)0xfb,
  (int8_t)0x05, (int8_t)0x0a, (int8_t)0x0f, (int8_t)0x14, (int8_t)0x19, (int8_t)0x1e, (int8_t)0x23, (int8_t)0x28,
  (int8_t)0xd0, (int8_t)0xd6, (int8_t)0xdc, (int8_t)0xe2, (int8_t)0xe8, (int8_t)0xee, (int8_t)0xf4, (int8_t)0xfa,
  (int8_t)0x06, (int8_t)0x0c, (int8_t)0x12, (int8_t)0x18, (int8_t)0x1e, (int8_t)0x24, (int8_t)0x2a, (int8_t)0x30,
  (int8_t)0xc8, (int8_t)0xcf, (int8_t)0xd6, (int8_t)0xdd, (int8_t)0xe4, (int8_t)0xeb, (int8_t)0xf2, (int8_t)0xf9,
  (int8_t)0x07, (int8_t)0x0e, (int8_t)0x15, (int8_t)0x1c, (int8_t)0x23, (int8_t)0x2a, (int8_t)0x31, (int8_t)0x38,
  (int8_t)0xc0, (int8_t)0xc8, (int8_t)0xd0, (int8_t)0xd8, (int8_t)0xe0, (int8_t)0xe8, (int8_t)0xf0, (int8_t)0xf8,
  (int8_t)0x08, (int8_t)0x10, (int8_t)0x18, (int8_t)0x20, (int8_t)0x28, (int8_t)0x30, (int8_t)0x38, (int8_t)0x40,
  (int8_t)0xb8, (int8_t)0xc1, (int8_t)0xca, (int8_t)0xd3, (int8_t)0xdc, (int8_t)0xe5, (int8_t)0xee, (int8_t)0xf7,
  (int8_t)0x09, (int8_t)0x12, (int8_t)0x1b, (int8_t)0x24, (int8_t)0x2d, (int8_t)0x36, (int8_t)0x3f, (int8_t)0x48,
  (int8_t)0xb0, (int8_t)0xba, (int8_t)0xc4, (int8_t)0xce, (int8_t)0xd8, (int8_t)0xe2, (int8_t)0xec, (int8_t)0xf6,
  (int8_t)0x0a, (int8_t)0x14, (int8_t)0x1e, (int8_t)0x28, (int8_t)0x32, (int8_t)0x3c, (int8_t)0x46, (int8_t)0x50,
  (int8_t)0xa8, (int8_t)0xb3, (int8_t)0xbe, (int8_t)0xc9, (int8_t)0xd4, (int8_t)0xdf, (int8_t)0xea, (int8_t)0xf5,
  (int8_t)0x0b, (int8_t)0x16, (int8_t)0x21, (int8_t)0x2c, (int8_t)0x37, (int8_t)0x42, (int8_t)0x4d, (int8_t)0x58,
  (int8_t)0xa0, (int8_t)0xac, (int8_t)0xb8, (int8_t)0xc4, (int8_t)0xd0, (int8_t)0xdc, (int8_t)0xe8, (int8_t)0xf4,
  (int8_t)0x0c, (int8_t)0x18, (int8_t)0x24, (int8_t)0x30, (int8_t)0x3c, (int8_t)0x48, (int8_t)0x54, (int8_t)0x60,
  (int8_t)0x98, (int8_t)0xa5, (int8_t)0xb2, (int8_t)0xbf, (int8_t)0xcc, (int8_t)0xd9, (int8_t)0xe6, (int8_t)0xf3,
  (int8_t)0x0d, (int8_t)0x1a, (int8_t)0x27, (int8_t)0x34, (int8_t)0x41, (int8_t)0x4e, (int8_t)0x5b, (int8_t)0x68,
  (int8_t)0x90, (int8_t)0x9e, (int8_t)0xac, (int8_t)0xba, (int8_t)0xc8, (int8_t)0xd6, (int8_t)0xe4, (int8_t)0xf2,
  (int8_t)0x0e, (int8_t)0x1c, (int8_t)0x2a, (int8_t)0x38, (int8_t)0x46, (int8_t)0x54, (int8_t)0x62, (int8_t)0x70,
  (int8_t)0x88, (int8_t)0x97, (int8_t)0xa6, (int8_t)0xb5, (int8_t)0xc4, (int8_t)0xd3, (int8_t)0xe2, (int8_t)0xf1,
  (int8_t)0x0f, (int8_t)0x1e, (int8_t)0x2d, (int8_t)0x3c, (int8_t)0x4b, (int8_t)0x5a, (int8_t)0x69, (int8_t)0x78,
  (int8_t)0x80, (int8_t)0x90, (int8_t)0xa0, (int8_t)0xb0, (int8_t)0xc0, (int8_t)0xd0, (int8_t)0xe0, (int8_t)0xf0,
  (int8_t)0x10, (int8_t)0x20, (int8_t)0x30, (int8_t)0x40, (int8_t)0x50, (int8_t)0x60, (int8_t)0x70, (int8_t)0x7f};

void
decode(int8_t *buffer, int8_t *destBuffer)
{
  //    int16_t ax = *((uint16_t *) buffer + 3);
  uint16_t outSize = *(int16_t *)buffer; // 27FF == 10239
  uint16_t v56 = (uint8_t)buffer[4];

  //    int8_t v4 = (int8_t *) &ax;
  //    int8_t *hgh = (int8_t *) &ax + 1;
  int8_t v4 = buffer[8];
  int8_t *data = &buffer[9];
  *destBuffer = v4;
  int8_t *dest = &destBuffer[1];

  uint16_t v7 = (uint8_t)buffer[8];
  uint16_t remainingSize = outSize - 1;
  int16_t counter = 0;

  int8_t *ebx = nullptr;

  if (remainingSize)
  {
    //        *hgh = v4;
    do
    {
      while (true)
      {
        while (true)
        {
          v4 = *data++;
          if (!(v4 & 0x80))
          { // Checking if negative bit is set (0x80 == 0b10000000)
            break;
          }

          // We left shift byte by 1 (* 2 is same as << 1)
          // Note that we cast to 1 byte size.
          // Seems like we try to get rid of 1st byte
          // in order to keep result it 1 byte long
          v7 = (uint8_t)(v4 * 2);
          *dest++ = (uint8_t)(v4 * 2);
          if (!--remainingSize)
          {
            return;
          }
        }

        // Checking if second bit is set (0x40 == 0b01000000)
        if (!(v4 & 0x40))
        {
          break;
        }

        // We keep the last 6 bits. (0x3F == 0b00111111)
        // And store it as length
        int length = v4 & 0x3F;

        // We write {length} times the current value
        memset(dest, v7, length);
        dest += length; // Our dest pointer is moved

        // Have we decoded all bytes?
        if (remainingSize == length)
        {
          return;
        }

        // We subtract the number of bytes decoded.
        remainingSize -= length;
      }

      // ebx = &gLookupTable[(v4 & 0xF) * 16];
      /**
       * We keep the low nibble (lower-half of byte)
       * of the current value and shift it to the left by 4.
       * e.g:
       *     0x3d (0b00111101)
       *   & 0x0f (0b00001111)
       *   = 0x0d (0b00001101)
       * <<4 0xd0 (0b11010000)
       */
      int test = (v4 & 0xF) << 4;
      ebx = &gLookupTable[test];

      /**
       * We check the 2 last bits of the high nibble
       * of the current value.
       * see if conditions a bit later...
       */
      v4 &= 0x30u; // is highest byte 16, 32 or 48?
      counter = remainingSize;

      /**
       * Is the current flag == 16
       * means that 2 last bits
       * of high nibble were == 01
       * ex: 0x18 (0b00[01]1000)
       *
       * For each byte, we check from
       * left to right if which bits are set.
       *
       * If a bit is set, we add the lookup
       * table data to the result.
       * If not, we subtract it from the result.
       */
      if (v4 == 0x10)
      {
        // printf("0x10\n");
        remainingSize = v56 >> 3; // v56 comes from file data directly.
        uint16_t v12 = (uint16_t)gLookupTable[8];
        do
        {
          v4 = *data++;
          for (size_t i = 0; i < 8; i++)
          {
            /**
             * For each iteration, we shift left using i
             * before checking if the MSB is set.
             * This is how we test if each bit is set.
             * Example with v4 == 0x18 (00011000)
             *  i == 0: 00011000 (24)
             *  i == 1: 00110000 (48)
             *  i == 2: 01100000 (96)
             *  i == 3: 11000000 (-64)
             *  i == 4: 10000000 (-128)
             *  i == 5: 00000000 (0)
             *  i == 6: 00000000 (0)
             */
            if ((v4 << i) < 0)
            {
              v7 += v12;
              if (HIBYTE(v7))
              {
                v7 = 0xFF;
              }
            }
            else
            {
              v7 -= v12;
              if (v7 < 0)
              {
                v7 = 0;
              }
            }

            *dest++ = (int8_t)v7;
          }
          --remainingSize;
        } while (remainingSize);
      }
      /**
       * Is the current flag == 32
       * means that 2 last bits
       * of high nibble were == 10
       * ex: 0x28 (0b00[10]1000)
       *
       * For each byte, we get each half-nibble (2 bits)
       * in order to calculate its result
       */
      else if (v4 == 32)
      {
        // printf("32\n");
        int8_t *ptr = &ebx[6];
        // int8_t *ptr = ((uintptr_t) ebx & ~PTR_LOBYTE) | (uintptr_t) (ebx + 6);
        // ebx = ((uintptr_t)ebx & ~PTR_LOBYTE) | (uintptr_t)(ebx + 6);
        // int8_t *ptr = ebx;
        remainingSize = v56 >> 2;

        do
        {
          uint8_t b = *data++;

          /**
           * we look for an association of
           * the MSB of current
           * value's high nibble in lookup table.
           * Right shifting by 6 allows to
           * get those bits.
           *
           * e.g with b = 0x90:
           *      0x90 (0b[10]010000)
           *  >>6 0x02 (0b000000[10])
           */
          v4 = ptr[b >> 6];
          v7 += v4;
          if (v7 < 0)
          {
            v7 = 0;
          }
          if (HIBYTE(v7))
          {
            v7 = 0xFF;
          }

          /**
           * We add the byte-clamped result
           * in the result buffer
           */
          *dest++ = (int8_t)v7;

          /**
           * we look for an association of
           * the LSB of current
           * value's high nibble in lookup table.
           *
           * e.g with b = 0x90:
           *      0x90 (0b10[01]0000)
           *  >>4 0x09 (0b0000[1001])
           *  & 3 0x01 (0b000000[01])
           */
          v4 = ptr[b >> 4 & 3];
          v7 += v4;
          if (v7 < 0)
          {
            v7 = 0;
          }
          if (HIBYTE(v7))
          {
            v7 = 0xFF;
          }

          /**
           * We add the byte-clamped result
           * in the result buffer
           */
          *dest++ = (int8_t)v7;

          /**
           * we look for an association of
           * the MSB of current
           * value's low nibble in lookup table.
           *
           * e.g with b = 0x90:
           *      0x84 (0b1000[01]00)
           *  >>2 0x21 (0b001000[01])
           *  & 3 0x01 (0b000000[01])
           */
          v4 = ptr[b >> 2 & 3];
          v7 += v4;
          if (v7 < 0)
          {
            v7 = 0;
          }
          if (HIBYTE(v7))
          {
            v7 = 0xFF;
          }

          /**
           * We add the byte-clamped result
           * in the result buffer
           */
          *dest++ = (int8_t)v7;

          /**
           * we look for an association of
           * the LSB of current
           * value's low nibble in lookup table.
           *
           * e.g with b = 0x42:
           *      0x42 (0b010000[10])
           *  & 3 0x02 (0b000000[10])
           */
          v4 = ptr[b & 3];
          v7 += v4;
          if (v7 < 0)
          {
            v7 = 0;
          }
          if (HIBYTE(v7))
          {
            v7 = 0xFF;
          }

          /**
           * We add the byte-clamped result
           * in the result buffer
           */
          *dest++ = (int8_t)v7;
          --remainingSize;
        } while (remainingSize);
      }
      /**
       * Is the current flag not 16 or 32
       * means that 2 LSB
       * of high nibble were == 00
       * ex: 0x08 (0b00[00]1000)
       *
       * For each byte, we get each nibble (4 bits)
       * in order to calculate its result
       */
      else
      {
        // printf("other\n");
        // v56 is divided by 2 (>> 1)
        remainingSize = (v56 >> 1);
        do
        {
          uint8_t b = *data++;

          /**
           * we look for an association of
           * the high nibble of current
           * value in lookup table.
           */
          v4 = ebx[b >> 4];
          //                    if (v4 & 0x80) {
          //                        *hgh = 0xff;
          //                    } else {
          //                        *hgh = 0;
          //                    }
          v7 += v4;
          if (v7 < 0)
          {
            v7 = 0;
          }
          if (HIBYTE(v7))
          {
            v7 = 0xFF;
          }

          /**
           * We add the byte-clamped result
           * in the result buffer
           */
          *dest++ = (int8_t)v7;

          /**
           * we look for an association of
           * the low nibble of current
           * value in lookup table.
           */
          v4 = ebx[b & 0xf];
          //                    if (v4 & 0x80) {
          //                        *hgh = 0xff;
          //                    } else {
          //                        *hgh = 0;
          //                    }
          v7 += v4;
          if (v7 < 0)
          {
            v7 = 0;
          }
          if (HIBYTE(v7))
          {
            v7 = 0xFF;
          }

          /**
           * We add the byte-clamped result
           * in the result buffer
           */
          *dest++ = (int8_t)v7;
          --remainingSize;
        } while (remainingSize);
      }

      remainingSize = counter - v56;
    } while (counter != v56);
  }
}

uint
decode_audio(const uint8_t *encodedData, char *&decoded)
{
  FileHeader header = {0};
  int offset = 0;
  memcpy(&header, encodedData, 10);
  offset += 10;

  printf("Audio asset is composed of %d chunks\n", header.numberOfChunks);

  header.chunks = new FileChunk[header.numberOfChunks]; // malloc(sizeof(FileChunk) * header.numberOfChunks);
  if (header.chunks == NULL)
  {
    fprintf(stderr, "Could not allocate memory\n");
    return 0;
  }

  uint16_t finalSize = 0;
  for (size_t i = 0; i < header.numberOfChunks; i++)
  {
    memcpy(&header.chunks[i], encodedData + offset, sizeof(FileChunk));
    finalSize += (header.chunks[i].decodedSize);
    offset += sizeof(FileChunk);

    printf(
      "\nChunk #%d/%d\n"
      "\tOffset: %d\n"
      "\tEncoded size: %d\n"
      "\tDecoded size: %d\n",
      i + 1, header.numberOfChunks,
      header.chunks[i].offset,
      header.chunks[i].encodedSize,
      header.chunks[i].decodedSize);
  }

  decoded = new char[finalSize];

  offset = 0;
  for (size_t i = 0; i < header.numberOfChunks; i++)
  {
    uint32_t baseSize = header.chunks[i].encodedSize;
    int8_t buffer[baseSize];
    int8_t destBuffer[header.chunks[i].decodedSize];

    memcpy(buffer, encodedData + header.chunks[i].offset, baseSize);
    decode(buffer, destBuffer);

    memcpy(decoded + offset, destBuffer, header.chunks[i].decodedSize);
    offset += header.chunks[i].decodedSize;
  }

  return finalSize;
}
