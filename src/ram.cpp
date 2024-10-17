/*
 *
 *
 *    /--------------------------------------------O
 *    |                                            |
 *    |  COPYRIGHT : (c) 2024 per Linuxperoxo.     |
 *    |  AUTHOR    : Linuxperoxo                   |
 *    |  FILE      : ram.cpp                       |
 *    |  SRC MOD   : 17/10/2024                    |
 *    |                                            |
 *    O--------------------------------------------/
 *
 *
 */

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <sys/mman.h> // mmap()
#include <iostream>
#include <fcntl.h> // open()
#include <unistd.h> // close()
#include <sys/stat.h> // fstat()

#include "../include/ram/ram.hpp"

#ifndef ROM_INIT
#define ROM_INIT 0x8000
#endif

#define ROM_END 0xFFFF
#define BYTE 8

constexpr uint16_t _ROM_MAX_SIZE { ROM_END - ROM_INIT };

RAM::RAM() noexcept
  : _MEMORY(nullptr),
    _ROM(nullptr)
{
  _MEMORY     = static_cast<uint8_t*>(mmap(nullptr, MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0));
  _ROM        = static_cast<uint8_t*>(_MEMORY + ROM_INIT);
  
  if(_MEMORY == MAP_FAILED || _ROM == MAP_FAILED)
  {
    std::cout << "Error to alloc memory for class RAM\n";
    exit(EXIT_FAILURE);
  }
  std::memset(_MEMORY, 0x00, MEMORY_SIZE); 
}

RAM::~RAM() noexcept
{
  munmap(_MEMORY, MEMORY_SIZE);
}

NONE RAM::load_rom(const char* _rom_file) noexcept
{
  int _file
  {
    open(_rom_file, O_RDONLY)
  };

  if(_file == -1)
  {
    std::cout << "Error to load ROM file -> " << _rom_file << '\n';
    exit(EXIT_FAILURE);
  }

  struct stat* _rom_file_info { new struct stat()};

  if(fstat(_file, _rom_file_info) == -1)
  {
    std::cout << "Error to get file infos -> " << _rom_file << '\n';
    exit(EXIT_FAILURE);
  }

  if(_rom_file_info->st_size > _ROM_MAX_SIZE)
  {
    std::cout << "ROM file -> " << _rom_file << " is very long he has " << _rom_file_info->st_size << "Bytes of size, he most have at most " << _ROM_MAX_SIZE << '\n';
    exit(EXIT_FAILURE);
  }

  uint32_t _rom_file_size { static_cast<uint32_t>(_rom_file_info->st_size) };

  uint8_t* _ROM_BUFFER
  {
    static_cast<uint8_t*>(mmap(nullptr, _rom_file_size, PROT_READ, MAP_PRIVATE, _file, 0))
  };

  if(_ROM_BUFFER == MAP_FAILED)
  {
    std::cout << "Error to alloc memory for ROM BUFFER\n";
    exit(EXIT_FAILURE);
  }

  delete _rom_file_info;
  close(_file);

  uint8_t _byte { 0 }; // Aqui onde vamos mover cada "bit" do arquivo  
  uint8_t _bits { 0 }; // Vendo quantos bits nos copiamos
  uint8_t _flip { 0 }; // Quantas vezes vamos flipar o bit para a esquerda, isso serve para montar nosso byte

  for(; _bits < _rom_file_size; _bits++, _flip++) // Copiando 32 bytes do arquivo, mais pra frente vamos aumentar essa quantidade
  {
    if(_bits % 8 == 0 && _bits > 0)
    {

      /*
       *
       * Quando copiamos 1 byte do arquivo jogamos esse byte para a memória da rom
       *
       */

      _ROM[(_bits / 7) - 1] = _byte;
      _byte                 = 0;
      _flip                 = 0;
    }
    
    /*
     *
     * Aqui convertemos as strings '0', '1' do arquivo rom em numeral 0, 1
     *
     */

    _byte = _byte | (_ROM_BUFFER[_bits] - '0') << (7 - _flip);
  }

  munmap(_ROM_BUFFER, _ROM_MAX_SIZE);

  /*
   *
   * ÁREA DEBUG:
   *
   * Pequenos testes xD
   *
   * std::cout << reinterpret_cast<void*>(&_ROM[0]) << '\n';
   * std::cout << static_cast<int>(*reinterpret_cast<uint8_t*>(_MEMORY + ROM_INIT)) << '\n';
   * std::cout << *reinterpret_cast<int*>(_MEMORY + ROM_INIT + 1) << '\n';
   *
   *
   *
   * std::cout << std::hex;

   * std::cout << static_cast<int>(_ROM[0]) << '\n';
   * std::cout << static_cast<int>(_ROM[1]) << '\n';
   * std::cout << static_cast<int>(_ROM[2]) << '\n';
   * exit(EXIT_FAILURE);
   *
   */
}
