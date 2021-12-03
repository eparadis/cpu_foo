#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

typedef uint8_t FLAG;
typedef uint8_t INSTR;
typedef uint8_t REGISTER;
typedef uint16_t IP;

const FLAG FLAG_HALT = 1 << 7;
const FLAG FLAG_EQ = 1 << 6;
const FLAG FLAG_GT = 1 << 5;
const FLAG FLAG_EXCEPTION = 1 << 4;
const FLAG FLAG_PMODE = 1 << 3;
#define INSTR_LOAD 0x08
#define INSTR_STORE 0x09
#define INSTR_ADDI 0x0A
#define INSTR_NOP 0x00
#define INSTR_CMP 0x0B
#define INSTR_JE 0x0C
#define INSTR_JLT 0x0D
#define INSTR_JMP 0x0E
#define INSTR_PUSH 0x0F
#define INSTR_POP 0x10
#define INSTR_HALT 0x11
#define INSTR_SHIFTL 0x12
#define INSTR_ADD 0x13
#define INSTR_CALL 0x14
#define INSTR_RET 0x15
#define INSTR_SDS 0x16
#define INSTR_SSP 0x17

typedef REGISTER (*loader_t)(IP);
typedef void (*storer_t)(REGISTER, IP);

typedef struct cpu_state {
  FLAG flags;
  IP ip;
  REGISTER rA;
  IP sp;
  IP ds;
} cpu_state;

IP load_word( loader_t load, IP addr) {
  REGISTER upper_addr = load( addr);
  REGISTER lower_addr = load( addr + 1);
  return upper_addr << 8 | lower_addr;
}

void store_word( storer_t store, IP val, IP addr) {
  REGISTER val_lo = val & 0x00FF;
  REGISTER val_hi = (val & 0xFF00) >> 8;
  store(val_hi, addr);
  store(val_lo, addr + 1);
}

struct cpu_state cpu(
  cpu_state *curr_state,
  loader_t load,
  storer_t store,
  loader_t io_read,
  storer_t io_write 
) {
  cpu_state new_state;
  new_state.flags = curr_state->flags;
  new_state.ip = curr_state->ip;
  new_state.rA = curr_state->rA;
  new_state.sp = curr_state->sp;
  new_state.ds = curr_state->ds;

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
    case INSTR_PUSH: {
      new_state.ds = curr_state->ds - 1;
      store( curr_state->rA, new_state.ds);
      new_state.ip = curr_state->ip + 1;
      break;
    }
    case INSTR_POP: {
      new_state.rA = load( curr_state->ds );
      new_state.ds = curr_state->ds + 1;
      new_state.ip = curr_state->ip + 1;
      break;
    }
    case INSTR_HALT: {
      new_state.ip = curr_state->ip + 1;
      new_state.flags |= FLAG_HALT;
      break;
    }
    case INSTR_SHIFTL: {
      new_state.ip = curr_state->ip + 1;
      new_state.rA = curr_state->rA << 1;
      break;
    }
    case INSTR_ADD: {
      IP addr = load_word(load, curr_state->ip + 1);
      REGISTER data = load( addr);
      new_state.rA = curr_state->rA + data;
      new_state.ip = curr_state->ip + 3;
      break;
    }
    case INSTR_CALL: {
      // push the addr of the _next_ instruction
      new_state.sp = curr_state->sp - 2;
      store_word(store, curr_state->ip + 3, new_state.sp);
      // jump to the given address
      new_state.ip = load_word(load, curr_state->ip + 1);
      break;
    }
    case INSTR_RET: {
      new_state.ip = load_word(load, curr_state->sp);
      new_state.sp = curr_state->sp + 2;
      break;
    }
    case INSTR_SDS: {
      if( curr_state->flags & FLAG_PMODE)
        new_state.ds = load_word(load, curr_state->ip + 1);
      new_state.ip = curr_state->ip + 3;
      break;
    }
    case INSTR_SSP: {
      if( curr_state->flags & FLAG_PMODE)
        new_state.sp = load_word(load, curr_state->ip + 1);
      new_state.ip = curr_state->ip + 3;
      break;
    }
    default: {
      //new_state.flags |= FLAG_HALT;
      new_state.flags |= FLAG_EXCEPTION;
      new_state.rA = instruction;
      new_state.ip = 0x0100;
      break;
    }
  }
  return new_state;
}

uint8_t _debug = 0;

#define RAMTOP 511
uint8_t *ram;

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
  printf("DEBUG[CPU] flags:%.2x ip:%.4x sp:%.4x ds:%.4x rA:%.2x\n", state.flags, state.ip, state.sp, state.ds, state.rA);
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

  ram = (REGISTER *) calloc(RAMTOP + 1, (sizeof(REGISTER)));
  if( ram == NULL) {
    fprintf(stderr, "Could not allocate memory for RAM\n");
    return 1;
  }

  IP c = 0;
  FILE *stream = fopen(argv[optind], "r");
  while( !feof(stream)) {
    store( fgetc(stream), c);
    c += 1;
  }

  struct cpu_state state;
  struct cpu_state next_state;

  state.ip = 0;
  state.flags = 0 | FLAG_PMODE;
  state.rA = 0;
  state.sp = 0; 
  state.ds = 0;

  while( (state.flags & FLAG_HALT) == 0) {
    debug_state(state);
    next_state = cpu( &state, &load, &store, &load, &store);
    state = next_state;
  }

  debug_state(state);
  debug_dump(ram, 0, 0x27);
  debug_dump(ram, 0x1E, 0x1F);
  debug_dump(ram, 0x100, 0x11F);
  debug_dump(ram, RAMTOP-16, RAMTOP);
}
