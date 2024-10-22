/*
 *
 *
 *    /--------------------------------------------O
 *    |                                            |
 *    |  COPYRIGHT : (c) 2024 per Linuxperoxo.     |
 *    |  AUTHOR    : Linuxperoxo                   |
 *    |  FILE      : main.cpp                      |
 *    |  SRC MOD   : 20/10/2024                    |
 *    |                                            |
 *    O--------------------------------------------/
 *
 *
 */

#include <cstdlib>
#include <iostream>

#include "../include/bus/bus.hpp"
#include "../include/cpu/cpu.hpp"
#include "../include/display/display.hpp"

#ifndef ROM_INIT
#define ROM_INIT 0x8000
#endif

/*
 *
 * Milissegundo (ms): 1 segundo é igual a 1.000 milissegundos.
 * Microsegundo (µs): 1 milissegundo é igual a 1.000 microsegundos.
 * Nanosegundo (ns): 1 microsegundo é igual a 1.000 nanosegundos.
 * Picosegundo (ps): 1 nanosegundo é igual a 1.000 picosegundos.
 * Femtosegundo (fs): 1 picosegundo é igual a 1.000 femtosegundos.

 * 1 Hz  : 1 evento por segundo
 * 1 KHz : 1000 eventos por segundo
 * 1 MHz : 1000000 eventos por segundo
 * 1 GHz : 1000000000 eventos por segundo
 *
*/

int main(int argc, char** argv)
{
  /*
   *
   * Alocando memória para cada classe
   *
   */

  CPU* _cpu { static_cast<CPU*>(std::malloc(sizeof(CPU))) };
  if(_cpu == nullptr) { std::cerr << "Error to alloc memory for CPU class\n"; exit(EXIT_FAILURE); }
  
  RAM* _ram { static_cast<RAM*>(std::malloc(sizeof(RAM))) };
  if(_ram == nullptr) { std::free(_cpu); std::cout << "Error to alloc memory for RAM class\n"; exit(EXIT_FAILURE); }

  BUS* _bus { static_cast<BUS*>(std::malloc(sizeof(BUS))) };
  if(_bus == nullptr) { std::free(_cpu); std::free(_ram); std::cout << "Error to alloc memory for BUS class\n"; exit(EXIT_FAILURE); }
  
  DISPLAY* _display { static_cast<DISPLAY*>(std::malloc(sizeof(DISPLAY))) };
  if(_display == nullptr) { std::free(_cpu); std::free(_ram); std::free(_bus); std::cout << "Error to alloc memory for DISPLAY class\n"; exit(EXIT_FAILURE); }

  /*
   *
   * Construindo cada objeto no addrs alocado
   *
   */
  
  new(_bus) BUS(_cpu, _ram);
  new(_ram) RAM();
  new(_cpu) CPU(_bus); // Aqui vai ter os 2 primeiros ciclos do processador um RST e um JMP
  new(_display) DISPLAY(_bus);

  
  _ram->load_rom(argv[1]); // Carregando a ROM
  
  _cpu->clock_loop();

  _cpu->~CPU();
  _ram->~RAM();
  _bus->~BUS();
  _display->~DISPLAY();

  std::free(_cpu);
  std::free(_ram);
  std::free(_bus);
  std::free(_display);

  return 0;
}

