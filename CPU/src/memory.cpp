/*
 *
 *
 *    /--------------------------------------------O
 *    |                                            |
 *    |  COPYRIGHT : (c) 2024 per Linuxperoxo.     |
 *    |  AUTHOR    : Linuxperoxo                   |
 *    |  FILE      : memory.cpp                    |
 *    |  SRC MOD   : 29/10/2024                    |
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

#include "../include/memory/memory.hpp"

constexpr uint16_t ROM_MAX_SIZE { ROM_END - ROM_INIT };
constexpr uint32_t MEMORY_SIZE  { 1024 * 64 }; 

MEMORY::MEMORY() noexcept
  : _RAM(nullptr),
    _ROM(nullptr)
{
  _RAM = static_cast<uint8_t*>(mmap(nullptr, MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0));
  _ROM = static_cast<uint8_t*>(_RAM + ROM_INIT);
  _WR  = 1;

  if(_RAM == MAP_FAILED)
  {
    std::cout << "Error to alloc memory for class MEMORY\n";
    exit(EXIT_FAILURE);
  }
  std::memset(_RAM, 0, MEMORY_SIZE); // Escrevendo 0 na memória alocada 
}

MEMORY::~MEMORY() noexcept
{
  munmap(_RAM, MEMORY_SIZE);
}

void MEMORY::load_rom(const char* _rom_file) noexcept
{
  int _file
  {
    open(_rom_file, O_RDONLY) // Abrindo arquivo para apenas escrita
  };

  if(_file == -1)
  {
    std::cout << "Error to load ROM file -> " << _rom_file << '\n';
    exit(EXIT_FAILURE);
  }

  /*
   *
   * Usamos a função fstat para pegar o tamanho da ROM 
   *
   */

  struct stat* _rom_file_info { new struct stat()}; // Struct necessária para guardar informações do arquivo

  if(fstat(_file, _rom_file_info) == -1) // Pegando as informações e verificando se ocorreu algum problema
  {
    std::cout << "Error to get file infos -> " << _rom_file << '\n';
    exit(EXIT_FAILURE);
  }

  /*
   *
   * Aqui fiz uma pequena verificação para ver se o tamanho do arquivo não é maior que o tamanho
   * máximo que no caso é de 0x8000 até 0xFFFF 
   *
   */

  if(_rom_file_info->st_size > ROM_MAX_SIZE)
  {
    std::cout << "ROM file -> " << _rom_file << " is very long he has " << _rom_file_info->st_size << "Bytes of size, he most have at most " << ROM_MAX_SIZE << '\n';
    exit(EXIT_FAILURE);
  }

  /*
   *
   * Como eu so quero saber o tamanho do arquivo, apenas armazeno o tamanho em uma variável para poder fechar
   * arquivo já que ele vai ser mapeado na memória
   *
   */

  uint32_t _rom_file_size { static_cast<uint32_t>(_rom_file_info->st_size) };

  uint8_t* _ROM_BUFFER
  {
    /*
     * 
     * Mapeado arquivo na memória com permissão de apenas escrita e privado, ou seja, qualquer alteração no seu 
     * conteúdo não vai ser escrito no arquivo
     *
     */
    
    static_cast<uint8_t*>(mmap(nullptr, _rom_file_size, PROT_READ, MAP_PRIVATE, _file, 0))
  };

  if(_ROM_BUFFER == MAP_FAILED)
  {
    std::cout << "Error to alloc memory for ROM BUFFER\n";
    exit(EXIT_FAILURE);
  }

  /*
   *
   * Já mapeamos o arquivo na memória então podemos fecha-ló
   *
   */

  delete _rom_file_info;
  
  close(_file);

  uint16_t _bits { 0 }; // Vendo quantos bits nos copiamos
  uint8_t _byte  { 0 }; // Aqui onde vamos mover cada "bit" do arquivo  
  uint8_t _flip  { 0 }; // Quantas vezes vamos flipar o bit para a esquerda, isso serve para montar nosso byte

  for(; _bits < _rom_file_size; _bits++, _flip++) // Copiando 32 bytes do arquivo, mais pra frente vamos aumentar essa quantidade
  {
    if(_bits % 8 == 0 && _bits > 0)
    {

      /*
       *
       * Quando copiamos 1 byte do arquivo jogamos esse byte para a memória da rom
       *
       */
      
      /*
      std::cout << "---------------------\n";
      std::cout << "ADDRS : 0x800" << (_bits / 8) - 1 << '\n';
      std::cout << "DATA  : " << static_cast<int>(_byte) << '\n';
      std::cout << "---------------------\n";
      */

      _ROM[(_bits / 8) - 1] = _byte;
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

  munmap(_ROM_BUFFER, _rom_file_size);

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

void MEMORY::load_firmware() noexcept
{
  /*
   *
   * Não queria fazer um arquivo separado para ser usado como firmware
   * então fiz assim mesmo :)
   *
   */

  /*
   *
   * Configurando o registrador STKPTR para o início da stack que é 0xFF 
   *
   * MOV STKPTR, 0xFF
   *
   */
  
  write(0x0000, 0x04); // MOV
  write(0x0001, 0x04); // STKPTR
  write(0x0002, 0xFF); // Valor a mover

  /*
   *
   * Configurando o registrador PC para o início da ROM do programa
   *
   * JMP 0x8000
   *
   */ 

  write(0x0003, 0x01); // JMP
  write(0x0004, 0x80); // HIGH BYTE 
  write(0x0005, 0x00); // LOWER BYTE

  /*
   *
   * Definindo esse bit como 0 já que não podemos mais acessar qualquer local da memória, 
   * apenas usamos ele para escrever um firmware de maneira mais fácil
   *
   */

  _WR = 0;
}

void MEMORY::load_firmware(const char* _firmware_file) noexcept
{
  /*
   *
   * Vou fazer ainda :^)
   *
   */ 
}
