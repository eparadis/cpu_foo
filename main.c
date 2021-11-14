#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

typedef uint8_t FLAG;
typedef uint8_t INSTR;
typedef uint8_t REGISTER;
typedef uint16_t IP;

const FLAG FLAG_HALT = 1 << 7;
const FLAG FLAG_EQ = 1 << 6;
const FLAG FLAG_GT = 1 << 5;
#define INSTR_LOAD 0x08
#define INSTR_STORE 0x09
#define INSTR_ADDI 0x0A
#define INSTR_NOP 0x00
#define INSTR_CMP 0x0B
#define INSTR_JE 0x0C
#define INSTR_JLT 0x0D
#define INSTR_JMP 0x0E

typedef REGISTER (*loader_t)(IP);
typedef void (*storer_t)(REGISTER, IP);

typedef struct cpu_state {
  FLAG flags;
  IP ip;
  REGISTER rA;
} cpu_state;

IP load_word( loader_t load, IP addr) {
  REGISTER upper_addr = load( addr);
  REGISTER lower_addr = load( addr + 1);
  return upper_addr << 8 | lower_addr;
}

struct cpu_state cpu(
  cpu_state *curr_state,
  loader_t load,
  storer_t store,
  loader_t io_read,
  storer_t io_write 
) {
  cpu_state new_state;
  INSTR instruction = load( curr_state->ip);
  switch( instruction) {
    case INSTR_LOAD: {
      REGISTER upper_addr = load( curr_state->ip + 1);
      REGISTER lower_addr = load( curr_state->ip + 2);
      new_state.rA = load( upper_addr << 8 | lower_addr);
      new_state.ip = curr_state->ip + 3;
      break;
    }
    case INSTR_STORE: {
      REGISTER upper_addr = load( curr_state->ip + 1);
      REGISTER lower_addr = load( curr_state->ip + 2);
      store( curr_state->rA, upper_addr << 8 | lower_addr);
      new_state.ip = curr_state->ip + 3;
      break;
    }
    case INSTR_ADDI: {
      REGISTER addend = load( curr_state->ip + 1);
      new_state.rA = curr_state->rA + addend;
      new_state.ip = curr_state->ip + 2;
      break;
    }
    case INSTR_NOP: {
      new_state.ip = curr_state->ip + 1;
      break;
    }
    case INSTR_CMP: {
      REGISTER value = load( curr_state->ip + 1);
      if( curr_state->rA == value)
        new_state.flags |= FLAG_EQ;
      else
        new_state.flags &= ~FLAG_EQ;
      if(curr_state->rA > value)
        new_state.flags |= FLAG_GT;
      else
        new_state.flags &= ~FLAG_GT; 
      new_state.ip = curr_state->ip + 2;
      break;
    }
    case INSTR_JE: {
      if( curr_state->flags & FLAG_EQ ) {
        new_state.ip = load_word(load, curr_state->ip + 1);
        break;
      }
      new_state.ip = curr_state->ip + 3;
      break;
    }
    case INSTR_JLT: {
      if( !(curr_state->flags & FLAG_GT) && !(curr_state->flags & FLAG_EQ) ) {
        new_state.ip = load_word(load, curr_state->ip + 1);
        break;
      }
      new_state.ip = curr_state->ip + 3;
      break;
    }
    case INSTR_JMP: {
      new_state.ip = load_word(load, curr_state->ip + 1);
      break;
    }
    default: {
      new_state.flags |= FLAG_HALT;
      break;
    }
  }
  return new_state;
}

uint8_t _debug = 0;

#define RAMTOP 511
uint8_t ram[RAMTOP + 1];

REGISTER load( IP addr) {
  if( addr == 0xFFFF) {
    return getchar();
  }
  if( _debug) printf("DEBUG[LOAD] addr:%.4x\n", addr);
  return ram[addr % (RAMTOP+1)];
}

void store( REGISTER value, IP addr) {
  // memory mapped IO, or at least O
  if(addr == 0xFFFF) {
    if( _debug)
      printf("OUT: %c\n", value);
    else
      printf("%c", value);
    return;
  }
  if( _debug) printf("DEBUG[STORE] addr:%.4x value:%.4x\n", addr, value);
  ram[addr % (RAMTOP+1)] = value;
}

void debug_state( cpu_state state) {
  if( _debug == 0) return;
  printf("DEBUG[CPU] flags:%.2x ip:%.4x rA:%.2x\n", state.flags, state.ip, state.rA);
}

void debug_dump(uint8_t *block, uint16_t start, uint16_t end) {
  if( _debug == 0) return;
  if( start % 16 != 0) {
    printf("%.4x  ", start); // the odd spacing indicates it's misaligned, which is better than nothing until I figure out how to properly align it
  }
  for( uint16_t i = start; i <= end; i += 1) {
    if( i % 16 == 0) {
      printf("%.4x ", i);
    }
    printf("%.2x ", block[i]);
    if( i % 16 == 7 ) {
      printf(" ");
    }
    if( i % 16 == 15) {
      printf("\n");
    }
  }
  if( end % 16 != 15) {
    printf("\n");
  }
}

void zero_addresses(uint8_t *block, uint16_t start, uint16_t end) {
  for( uint16_t i = start; i <= end; i += 1) {
    block[i] = 0;
  }
}

int main( int argc, char *argv[] ) {
  int opt;
  while ((opt = getopt(argc, argv, "d")) != -1) {
    switch (opt) {
      case 'd': _debug = 1; break;
      default:
        fprintf(stderr, "usage: %s [-d] [file.bin]\n", argv[0]);
        return 1;
    }
  }
  if( optind >= argc) {
    fprintf(stderr, "error: ram image file required\n");
    return 1;
  }

  zero_addresses(ram, 0, RAMTOP);

  IP c = 0;
  FILE *stream = fopen(argv[optind], "r");
  while( !feof(stream)) {
    store( fgetc(stream), c);
    c += 1;
  }

  struct cpu_state state;
  struct cpu_state next_state;

  state.ip = 0;
  state.flags = 0;
  state.rA = 0;

  while( (state.flags & FLAG_HALT) == 0) {
    debug_state(state);
    next_state = cpu( &state, &load, &store, &load, &store);
    state = next_state;
  }

  debug_state(state);
  debug_dump(ram, 0, 0x27);
  debug_dump(ram, 0x1E, 0x1F);
  debug_dump(ram, 0x100, 0x11F);
}
