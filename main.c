#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

typedef uint8_t FLAG;
typedef uint8_t INSTR;
typedef uint8_t REGISTER;
typedef uint16_t POINTER;

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
#define INSTR_SEVA 0x18
#define INSTR_SPMW 0x19
#define INSTR_JCPF 0x1A

typedef REGISTER (*loader_t)(POINTER);
typedef void (*storer_t)(REGISTER, POINTER);

typedef struct cpu_state {
  FLAG flags;
  POINTER ip;
  REGISTER rA;
  POINTER sp;
  POINTER ds;
  POINTER eva;
  POINTER pmw;
} cpu_state;

typedef struct REGISTER_ATTEMPT {
  uint8_t failed;
  REGISTER value;
} REGISTER_ATTEMPT;

typedef struct POINTER_ATTEMPT {
  uint8_t failed;
  POINTER value;
} POINTER_ATTEMPT;

uint8_t _debug = 0;

POINTER load_word( loader_t load, POINTER addr) {
  REGISTER upper_addr = load( addr);
  REGISTER lower_addr = load( addr + 1);
  return upper_addr << 8 | lower_addr;
}

void store_word( storer_t store, POINTER val, POINTER addr) {
  REGISTER val_lo = val & 0x00FF;
  REGISTER val_hi = (val & 0xFF00) >> 8;
  store(val_hi, addr);
  store(val_lo, addr + 1);
}

cpu_state raise_exception(cpu_state *state) {
  state->flags |= FLAG_EXCEPTION;
  state->flags |= FLAG_PMODE;
  state->ip = state->eva;
  return *state;
}

POINTER_ATTEMPT load_word_attempt(loader_t load, POINTER addr, cpu_state *state) {
  POINTER_ATTEMPT ret;
  if( (addr < state->pmw || (addr + 1) < state->pmw) && !(state->flags & FLAG_PMODE) ) {
    if( _debug) printf("DEBUG[FAILED LOAD WORD] addr:%.4x\n", addr);
    ret.failed = 1;
    return ret;
  }
  ret.failed = 0;
  ret.value = load_word(load, addr);
  return ret;
}

REGISTER_ATTEMPT load_attempt(loader_t load, POINTER addr, cpu_state *state) {
  REGISTER_ATTEMPT ret;
  if( (addr < state->pmw) && !(state->flags & FLAG_PMODE) ) {
    if( _debug) printf("DEBUG[FAILED LOAD BYTE] addr:%.4x\n", addr);
    ret.failed = 1;
    return ret;
  }
  ret.failed = 0;
  ret.value = load(addr);
  return ret;
}

struct cpu_state cpu(
  cpu_state *curr_state,
  loader_t load,
  storer_t store
) {
  cpu_state new_state;
  new_state.flags = curr_state->flags;
  new_state.ip = curr_state->ip;
  new_state.rA = curr_state->rA;
  new_state.sp = curr_state->sp;
  new_state.ds = curr_state->ds;
  new_state.eva = curr_state->eva;
  new_state.pmw = curr_state->pmw;

  REGISTER_ATTEMPT inst = load_attempt(load, curr_state->ip, curr_state);
  if( inst.failed)
    return raise_exception(&new_state);
  switch( inst.value) {
    case INSTR_LOAD: {
      POINTER_ATTEMPT p_att = load_word_attempt(load, curr_state->ip + 1, curr_state);
      if( p_att.failed)
        return raise_exception(&new_state);
      REGISTER_ATTEMPT r_att = load_attempt(load, p_att.value, curr_state);
      if( r_att.failed)
        return raise_exception(&new_state);
      new_state.rA = r_att.value;
      new_state.ip = curr_state->ip + 3;
      break;

      // // old way, w/o exceptions
      // POINTER addr = load_word(load, curr_state->ip + 1);
      // new_state.rA = load( addr);
      // new_state.ip = curr_state->ip + 3;
      // break;
    }
    case INSTR_STORE: {
      POINTER addr = load_word(load, curr_state->ip + 1);
      store( curr_state->rA, addr);
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
      POINTER addr = load_word(load, curr_state->ip + 1);
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
    case INSTR_SEVA: {
      if( curr_state->flags & FLAG_PMODE)
        new_state.eva = load_word(load, curr_state->ip + 1);
      new_state.ip = curr_state->ip + 3;
      break;
    }
    case INSTR_SPMW: {
      if( curr_state->flags & FLAG_PMODE)
        new_state.pmw = load_word(load, curr_state->ip + 1);
      new_state.ip = curr_state->ip + 3;
      break;
    }
    case INSTR_JCPF: {
      if( curr_state->flags & FLAG_PMODE) {
        new_state.ip = load_word(load, curr_state->ip + 1);
        new_state.flags &= ~FLAG_PMODE;
      } else {
        new_state.ip = curr_state->ip + 3;
      }
    }
    default: {
      return raise_exception(&new_state);
      break;
    }
  }
  return new_state;
}

POINTER RAMTOP;
uint8_t *ram;

REGISTER load( POINTER addr) {
  if( addr == 0xFFFF) {
    return getchar();
  }
  if( _debug) printf("DEBUG[LOAD] addr:%.4x\n", addr);
  return ram[addr % (RAMTOP+1)];
}

void store( REGISTER value, POINTER addr) {
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
  printf("DEBUG[CPU] flags:%.2x ip:%.4x sp:%.4x ds:%.4x rA:%.2x eva:%.4x pmw:%.4x\n",
    state.flags, state.ip, state.sp, state.ds, state.rA, state.eva, state.pmw);
}

void debug_dump(uint8_t *block, uint16_t start, uint16_t end) {
  if( _debug == 0) return;
  if( start % 16 != 0) {
    printf("%.4x  ", start); // the odd spacing indicates it's misaligned, which is better than
                             // nothing until I figure out how to properly align it
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
  char *rval = NULL;
  int num_bytes = 512;
  while ((opt = getopt(argc, argv, "dr:")) != -1) {
    switch (opt) {
      case 'd': _debug = 1; break;
      case 'r': {
        rval = optarg;
        break;
      }
      default:
        fprintf(stderr, "usage: %s [-d] [-r BYTES] [file.bin]\n", argv[0]);
        return 1;
    }
  }
  if( optind >= argc) {
    fprintf(stderr, "error: ram image file required\n");
    return 1;
  }
  if( rval != NULL ) {
    num_bytes = atoi(rval);
    if(num_bytes < 8 || num_bytes > 65536) {
      fprintf(stderr, "Specified RAM size \"%s\" is invalid. RAM size must be between 8 and 65536 bytes\n", rval);
      return 1;
    }
  }
  RAMTOP = num_bytes - 1;

  if(_debug) {
    printf("DEBUG[MAIN] Using %d bytes of RAM\n", RAMTOP + 1);
  }

  ram = (REGISTER *) calloc(RAMTOP + 1, (sizeof(REGISTER)));
  if( ram == NULL) {
    fprintf(stderr, "Could not allocate memory for RAM\n");
    return 1;
  }

  POINTER c = 0;
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
  state.eva = 0;
  state.pmw = 0xFF;

  while( (state.flags & FLAG_HALT) == 0) {
    debug_state(state);
    next_state = cpu( &state, &load, &store);
    state = next_state;
  }

  debug_state(state);
  debug_dump(ram, 0, 0x27);
  debug_dump(ram, 0x1E, 0x1F);
  debug_dump(ram, 0x100, 0x11F);
  debug_dump(ram, RAMTOP-16, RAMTOP);
}
