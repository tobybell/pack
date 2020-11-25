#include "put.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

char buf[4096];

int help() {
  printf(
    "Usage: pack [OPTIONS]\n\n"
    "Compile a pack source into binary.\n\n"
    "Options:\n"
    "  --help / -h  Show this message and exit\n");
  return 0;
}

int usage() {
  printf(
    "Usage: pack [OPTIONS]\n"
    "See 'pack --help'.\n");
  return 1;
}

/** Callback function for receiving a token, (start, length). */
typedef int (*token_cb)(void *aux, const char *tok, size_t len);

struct tokenizer {

  /** Whether inside a comment. */
  bool comment;

  /** Current token. Max size 100. */
  char tok[100];

  /** Current token length. */
  size_t tok_len;

  /** Token callback. */
  token_cb cb;

  /** Aux/instance passed to callback. */
  void *cb_aux;
};

typedef struct tokenizer tokenizer;

void tok_init(tokenizer *t, token_cb cb, void *cb_aux) {
  t->comment = false;
  t->tok_len = 0;
  t->cb = cb;
  t->cb_aux = cb_aux;
}

void tok_add(tokenizer *t, const char* data, size_t len) {
  for (int i = 0; i < len; i += 1) {
    const char c = buf[i];
    const bool c_newline = c == '\n';
    const bool c_ws = c == ' ' || c == '\t' || c_newline;
    const bool c_cs = c == '#';

    // If this is non-token and we have a current token, handle it.
    if ((c_ws || c_cs) && t->tok_len) {
      t->tok[t->tok_len] = '\0';
      t->cb(t->cb_aux, t->tok, t->tok_len);
      t->tok_len = 0;
    }

    // Skip comments and whitespace.
    if (c_cs) {
      t->comment = true;
    }
    if (c_newline) {
      t->comment = false;
    }
    if (t->comment || c_ws) {
      continue;
    }

    // Default case. Add character to current token.
    t->tok[t->tok_len] = c;
    t->tok_len += 1;
  }
}

void tok_finish(tokenizer *t) {
  if (t->tok_len) {
    t->tok[t->tok_len] = '\0';
    t->cb(t->cb_aux, t->tok, t->tok_len);
    t->tok_len = 0;
  }
}

/** Callback function for receiving a pack piece, (start, length). */
typedef void (*pack_cb)(void *aux, const unsigned char *data, size_t len);

enum atom_type {
  LU8 = 0,
  BU8 = 1,
  LU16 = 2,
  BU16 = 3,
  LU32 = 4,
  BU32 = 5,
  LU64 = 6,
  BU64 = 7,
  LI8 = 8,
  BI8 = 9,
  LI16 = 10,
  BI16 = 11,
  LI32 = 12,
  BI32 = 13,
  LI64 = 14,
  BI64 = 15,
  LF32 = 16,
  BF32 = 17,
  LF64 = 18,
  BF64 = 19,
};

typedef enum atom_type atom_type;

bool type_big_endian(atom_type t) {
  return t % 2;
}

int type_size(atom_type t) {
  switch (t) {
  case LU8:
  case BU8:
  case LI8:
  case BI8:
    return 1;
  case LU16:
  case BU16:
  case LI16:
  case BI16:
    return 2;
  case LU32:
  case BU32:
  case LI32:
  case BI32:
  case LF32:
  case BF32:
    return 4;
  case LU64:
  case BU64:
  case LI64:
  case BI64:
  case LF64:
  case BF64:
    return 8;
  default:
    return 0;
  }
}

struct packer {
  /** Current atom type. */
  atom_type type;

  /** Callback for handling pack pieces. */
  pack_cb cb;

  /** Aux/instance passed to callback. */
  void *cb_aux;
};

typedef struct packer packer;

void pack_init(packer *p, pack_cb cb, void *cb_aux) {
  p->type = LI32;
  p->cb = cb;
  p->cb_aux = cb_aux;
}

int pack_add(packer *p, const char *tok, size_t len) {
  if (!len) {
    return 0;
  }
  if ((len == 2 && !strncmp(tok, "u8", len)) ||
      (len == 3 && !strncmp(tok, "lu8", len))) {
    p->type = LU8;
  } else if ((len == 3 && !strncmp(tok, "u16", len)) ||
             (len == 4 && !strncmp(tok, "lu16", len))) {
    p->type = LU16;
  } else if ((len == 3 && !strncmp(tok, "u32", len)) ||
             (len == 4 && !strncmp(tok, "lu32", len))) {
    p->type = LU32;
  } else if ((len == 3 && !strncmp(tok, "u64", len)) ||
             (len == 4 && !strncmp(tok, "lu64", len))) {
    p->type = LU64;
  } else if ((len == 2 && !strncmp(tok, "i8", len)) ||
             (len == 3 && !strncmp(tok, "li8", len))) {
    p->type = LI8;
  } else if ((len == 3 && !strncmp(tok, "i16", len)) ||
             (len == 4 && !strncmp(tok, "li16", len))) {
    p->type = LI16;
  } else if ((len == 3 && !strncmp(tok, "i32", len)) ||
             (len == 4 && !strncmp(tok, "li32", len))) {
    p->type = LI32;
  } else if ((len == 3 && !strncmp(tok, "i64", len)) ||
             (len == 4 && !strncmp(tok, "li64", len))) {
    p->type = LI64;
  } else if ((len == 3 && !strncmp(tok, "f32", len)) ||
             (len == 4 && !strncmp(tok, "lf32", len))) {
    p->type = LF32;
  } else if ((len == 3 && !strncmp(tok, "f64", len)) ||
             (len == 4 && !strncmp(tok, "lf64", len))) {
    p->type = LF64;
  } else if (len == 3 && !strncmp(tok, "bu8", len)) {
    p->type = BU8;
  } else if (len == 4 && !strncmp(tok, "bu16", len)) {
    p->type = BU16;
  } else if (len == 4 && !strncmp(tok, "bu32", len)) {
    p->type = BU32;
  } else if (len == 4 && !strncmp(tok, "bu64", len)) {
    p->type = BU64;
  } else if (len == 3 && !strncmp(tok, "bi8", len)) {
    p->type = BI8;
  } else if (len == 4 && !strncmp(tok, "bi16", len)) {
    p->type = BI16;
  } else if (len == 4 && !strncmp(tok, "bi32", len)) {
    p->type = BI32;
  } else if (len == 4 && !strncmp(tok, "bi64", len)) {
    p->type = BI64;
  } else if (len == 4 && !strncmp(tok, "bf32", len)) {
    p->type = BF32;
  } else if (len == 4 && !strncmp(tok, "bf64", len)) {
    p->type = BF64;
  } else {
    unsigned char piece[8];
    char *end;
    long long ll;
    float f;
    double d;

    switch (p->type) {
    case LU8: case LU16: case LU32: case LU64:
    case BU8: case BU16: case BU32: case BU64:
    case LI8: case LI16: case LI32: case LI64:
    case BI8: case BI16: case BI32: case BI64:
      ll = strtoll(tok, &end, 0);
      if (end == tok) return 1;
    case LF32: case BF32:
      f = strtof(tok, &end);
      if (end == tok) return 1;
    case LF64: case BF64:
      d = strtod(tok, &end);
      if (end == tok) return 1;
    }

    switch (p->type) {
    case LU8:
      put_u8(ll, piece); break;
    case BU8:
      put_u8(ll, piece); break;
    case LU16:
      put_lu16(ll, piece); break;
    case BU16:
      put_bu16(ll, piece); break;
    case LU32:
      put_lu32(ll, piece); break;
    case BU32:
      put_bu32(ll, piece); break;
    case LU64:
      put_lu64(ll, piece); break;
    case BU64:
      put_bu64(ll, piece); break;
    case LI8:
      put_i8(ll, piece); break;
    case BI8:
      put_i8(ll, piece); break;
    case LI16:
      put_li16(ll, piece); break;
    case BI16:
      put_bi16(ll, piece); break;
    case LI32:
      put_li32(ll, piece); break;
    case BI32:
      put_bi32(ll, piece); break;
    case LI64:
      put_li64(ll, piece); break;
    case BI64:
      put_bi64(ll, piece); break;
    case LF32:
      put_lf32(f, piece); break;
    case BF32:
      put_bf32(f, piece); break;
    case LF64:
      put_lf64(d, piece); break;
    case BF64:
      put_bf64(d, piece); break;
    }

    // Write the piece out.
    p->cb(p->cb_aux, piece, type_size(p->type));
  }
  return 0;
}

void handle_pack(void *unused, const unsigned char* data, size_t len) {
  write(STDOUT_FILENO, data, len);
}

int pack_add_cb(void *p, const char *c, size_t len) {
  return pack_add(p, c, len);
}

int pack() {
  packer p;
  pack_init(&p, handle_pack, NULL);
  tokenizer t;
  tok_init(&t, pack_add_cb, &p);
  while (1) {
    int n = read(STDIN_FILENO, buf, sizeof (buf));
    if (!n) break;
    tok_add(&t, buf, n);
  }
  tok_finish(&t);
  return 0;
}

int main(int argc, const char* argv[]) {
  if (argc > 2) {
    return usage();
  }
  if (argc > 1 && (!strcmp(argv[1], "--help") || !strcmp(argv[1], "-h"))) {
    return help();
  }
  if (argc > 1) {
    return usage();
  }
  return pack();
}
