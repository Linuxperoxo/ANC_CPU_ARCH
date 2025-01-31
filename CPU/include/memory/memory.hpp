/*
 *
 *
 *    /--------------------------------------------O
 *    |                                            |
 *    |  COPYRIGHT : (c) 2024 per Linuxperoxo.     |
 *    |  AUTHOR    : Linuxperoxo                   |
 *    |  FILE      : memory.hpp                    |
 *    |  SRC MOD   : 03/11/2024                    |
 *    |                                            |
 *    O--------------------------------------------/
 *
 *
 */

/*
 *
 * Aqui é a memória principal, e vai ser aqui também que nós vamos
 * manipular dispositivos mapeados na memória
 *
 * A memória MEMORY é dividida em célular de 1 byte de tamanho
 *
 * +--------------------------------------------------------------+
 * | 0x00 | 0x01 | 0x02 | 0x03 | 0x04 | 0x05 | 0x06 | 0x07 | 0x08 | -> MEMORY 
 * +--------------------------------------------------------------+
 *
 * Normalmente dispositivos são mapeados nos endereços mais altos(depende do OS)
 *
 * Digamos que um dispositivos qualquer esteja mapeado no endereço 0x05 - 0x07, com o tamanho
 * de 3 bytes
 *
 * +--------------------------------------------------------------+
 * | 0x00 | 0x01 | 0x02 | 0x03 | 0x04 | 0x05 | 0x06 | 0x07 | 0x08 | -> MEMORY 
 * +--------------------------------------------------------------+
 *                                       ^             ^
 *                                       |             |
 *                                     Início        Final
 *
 * Então com o dispositivo mapeado na memória, qualquer dado que é escrito nesse endereço
 * será uma manipulação com o dispositivo mapeado
 *
 * Exemplo, digamos que é um teclado mapeado nesse endereço, digamos também que todo dato de input
 * do teclado seja armazenado nesse endereço em expecífico, podemos pedir para ler esse bloco para 
 * saber qual tecla foi pressionada
 *
 * Mas não é como se o dispositivo também tivesse acesso a esse lugar na memória, por baixo dos panos
 * o que realmente acontece é que após traduzir o endereço fornecido pelo processador, o barramento
 * percebe que aquele endereço não é para um lugar na memória, e sim para um dispositivo, então ele 
 * redireciona para o dispositivo
 *
 */

#ifndef __MEMORY_HPP__
#define __MEMORY_HPP__

#include <cstdint>

/*
 *
 * Faixa de endereço que cada chip de memória responde
 *
 */

/*
 *
 * Firmware
 *
 */

#define FIRMWARE_INIT 0x00
#define FIRMWARE_END  0xFF

/*
 *
 * Rom do programa
 *
 */

#define PROGRAM_ROM_INIT 0x8000
#define PROGRAM_ROM_END  0xFFFF

/*
 *
 * Para dispositivos I/O
 *
 */

#define IO_MEMORY_INIT 0x7000
#define IO_MEMORY_END  0x7FFF

/*
 *
 * Para RAM
 *
 */

#define RAM_MEMORY_INIT 0x100
#define RAM_MEMORY_END  0x6FFF

class BUS;

class MEMORY
{
private:

  MEMORY(const MEMORY&)            = delete;
  MEMORY(MEMORY&&)                 = delete;
  MEMORY& operator=(const MEMORY&) = delete;
  MEMORY& operator=(MEMORY&&)      = delete;

private:

  uint8_t* _FIRMWARE_ROM;
  uint8_t* _PROGRAM_ROM;
  uint8_t* _IO_DEVICE;
  uint8_t* _RAM;
  
  uint8_t _CE;
public:

  explicit MEMORY() noexcept;

  ~MEMORY() noexcept;

private:

  uint8_t addrs_decoder(uint16_t, uint8_t, uint8_t) noexcept;

  /*
   *
   * @info   : Essa função lê um dado em um bloco de memória
   *
   * @return : uint8_t 
   *
   * @param  : Recebe um endereço que será escrito, e o dado a ser escrito
   *
   */

  inline uint8_t read(uint16_t _addrs_to_read) noexcept { return addrs_decoder(_addrs_to_read, 0, 0); }

  /*
   *
   * @info   : Essa função escreve um dado em um bloco de memória
   *
   * @return : void
   *
   * @param  : Recebe um endereço que será escrito, e o dado a ser escrito
   *
   */
  
  inline void write(uint16_t _addrs_to_write, uint8_t _data_to_write) noexcept { addrs_decoder(_addrs_to_write, _data_to_write, 1); }

public:

  /*
   *
   * @info : Essa função lê binário string e carrega na memória para a rom
   *
   * @return : void
   *
   * @param : Recebe o arquivo da rom
   *
   */

  void load_rom(const char*) noexcept;

  /*
   *
   * Decidi começar a fazer uma implementação do nosso firmware que por enquanto está bem simples.
   *
   * Com esse firmware nós vamos configurar nosso hardware para começar a executar nosso programa.
   *
   * Essa função vai servir para carregar nosso firmware
   *
   */
  
  void load_firmware() noexcept;

  friend class BUS;
};

#endif
