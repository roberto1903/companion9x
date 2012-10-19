// TODO should be rle

/*
 * Author - Bertrand Songis <bsongis@gmail.com>
 * 
 * Based on th9x -> http://code.google.com/p/th9x/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#ifndef file_h
#define file_h

#if __GNUC__
#define PACK( __Declaration__ ) __Declaration__ __attribute__((__packed__))
#else
#include "../winbuild/winbuild.h"
#endif

#define ERR_NONE 0
#define ERR_FULL 1
#define ERR_TMO  2

#define BS                 16

PACK(struct DirEnt{
  uint8_t  startBlk;
  uint16_t size:12;
  uint16_t typ:4;
});

PACK(struct EeFs{
  uint8_t  version;
  uint8_t  mySize;
  uint8_t  freeList;
  uint8_t  bs;
  DirEnt   files[36];
});

struct t_eeprom_header
{
  uint32_t sequence_no ;          // sequence # to decide which block is most recent
  uint16_t data_size ;                    // # bytes in data area
  uint8_t flags ;
  uint8_t hcsum ;
};

class EFile
{
  uint8_t  m_fileId;    //index of file in directory = filename
  unsigned int m_pos;       //over all filepos
  uint8_t  m_currBlk;   //current block.id
  uint8_t  m_ofs;       //offset inside of the current block
  uint8_t  m_zeroes;    //control byte for run length decoder
  uint8_t  m_bRlc;      //control byte for run length decoder
  uint8_t  m_err;       //error reasons
  uint16_t m_size;
  //uint16_t m_stopTime10ms; //maximum point of time for writing


  uint8_t *eeprom;
  int     eeprom_size;
  EeFs    *eeFs;
  uint8_t eeFsVersion;
  uint8_t eeFsSize;
  uint8_t eeFsFirstBlock;
  uint8_t eeFsBlocksOffset;
  uint8_t eeFsBlocksMax;

  void eeprom_read_block (void *pointer_ram, unsigned int pointer_eeprom, size_t size);
  void eeprom_write_block(const void *pointer_ram, unsigned int pointer_eeprom, size_t size);

  uint8_t EeFsRead(uint8_t blk,uint8_t ofs);
  void EeFsWrite(uint8_t blk,uint8_t ofs,uint8_t val);
  uint8_t EeFsGetLink(uint8_t blk);
  void EeFsSetLink(uint8_t blk,uint8_t val);
  uint8_t EeFsGetDat(uint8_t blk,uint8_t ofs);
  void EeFsSetDat(uint8_t blk,uint8_t ofs, const uint8_t*buf,uint8_t len);
  uint16_t EeFsGetFree();
  void EeFsFree(uint8_t blk);///free one or more blocks
  uint8_t EeFsAlloc(); ///alloc one block from freelist

public:

  EFile();

  void EeFsCreate(uint8_t *eeprom, int size, uint8_t version);

  bool EeFsOpen(uint8_t *eeprom, int size);

  ///remove contents of given file
  void rm(uint8_t i_fileId);

  ///swap contents of file1 with them of file2
  void swap(uint8_t i_fileId1,uint8_t i_fileId2);

  ///return true if the file with given fileid exists
  bool exists(uint8_t i_fileId);

  ///open file for reading, no close necessary
  ///for writing use writeRlc() or create()
  uint8_t openRd(uint8_t i_fileId); 
  /// create a new file with given fileId, 
  /// !!! if this file already exists, then all blocks are reused
  /// and all contents will be overwritten.
  /// after writing closeTrunc has to be called
  void    create(uint8_t i_fileId, uint8_t typ);
  /// close file and truncate the blockchain if to long.
  void    closeTrunc();

  ///open file, write to file and close it. 
  ///If file existed before, then contents is overwritten. 
  ///If file was larger before, then unused blocks are freed
  uint16_t writeRlc1(uint8_t i_fileId, uint8_t typ,const uint8_t*buf,uint16_t i_len);
  uint16_t writeRlc2(uint8_t i_fileId, uint8_t typ,const uint8_t*buf,uint16_t i_len);

  uint8_t read(uint8_t*buf, uint16_t i_len);
  uint8_t write1(uint8_t b);
  uint8_t write(const uint8_t*buf, uint8_t i_len);

  ///return size of compressed file without block overhead
  uint16_t size(uint8_t id);
  ///read from opened file and decode rlc-coded data
  uint16_t readRlc12(uint8_t*buf,uint16_t i_len,bool rlc2);
  inline uint16_t readRlc1(uint8_t*buf,uint16_t i_len)
  {
    return readRlc12(buf,i_len,false);
  }
  inline uint16_t readRlc2(uint8_t*buf, uint16_t i_len)
  {
    return readRlc12(buf, i_len, true);
  }

  uint8_t byte_checksum( uint8_t *p, uint32_t size );
  uint32_t ee32_check_header( struct t_eeprom_header *hptr );
  uint32_t get_current_block_number( uint32_t block_no, uint16_t *p_size);


};

#endif
/*eof*/
