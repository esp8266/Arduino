/*
cencoder.c - c source to a base64 encoding algorithm implementation

This is part of the libb64 project, and has been placed in the public domain.
For details, see http://sourceforge.net/projects/libb64
*/

#include "cencode.h"

extern "C" {

void base64_init_encodestate(base64_encodestate* state_in){
  state_in->step = step_A;
  state_in->result = 0;
  state_in->stepcount = 0;
  state_in->stepsnewline = BASE64_CHARS_PER_LINE;
}


void base64_init_encodestate_nonewlines(base64_encodestate* state_in){
  base64_init_encodestate(state_in);
  state_in->stepsnewline = -1;
}

char base64_encode_value(const char n) {
  char r;

  if (n < 26)
    r = n + 'A';
  else if (n < 26 + 26)
    r = n - 26 + 'a';
  else if (n < 26 + 26 + 10 )
    r = n - 26 - 26 + '0';
  else if (n == 62 )
    r = '+';
  else
    r = '/';
  return r;
}

int base64_encode_block(const char* plaintext_in, int length_in, char* code_out, base64_encodestate* state_in){
  const char* plainchar = plaintext_in;
  const char* const plaintextend = plaintext_in + length_in;
  char* codechar = code_out;
  char result;
  char fragment;
  
  result = state_in->result;
  
  switch (state_in->step){
    while (1){
  case step_A:
      if (plainchar == plaintextend){
        state_in->result = result;
        state_in->step = step_A;
        return codechar - code_out;
      }
      fragment = *plainchar++;
      result = (fragment & 0x0fc) >> 2;
      *codechar++ = base64_encode_value(result);
      result = (fragment & 0x003) << 4;
      // falls through
  case step_B:
      if (plainchar == plaintextend){
        state_in->result = result;
        state_in->step = step_B;
        return codechar - code_out;
      }
      fragment = *plainchar++;
      result |= (fragment & 0x0f0) >> 4;
      *codechar++ = base64_encode_value(result);
      result = (fragment & 0x00f) << 2;
      // falls through
  case step_C:
      if (plainchar == plaintextend){
        state_in->result = result;
        state_in->step = step_C;
        return codechar - code_out;
      }
      fragment = *plainchar++;
      result |= (fragment & 0x0c0) >> 6;
      *codechar++ = base64_encode_value(result);
      result  = (fragment & 0x03f) >> 0;
      *codechar++ = base64_encode_value(result);
      
      ++(state_in->stepcount);
      if ((state_in->stepcount == BASE64_CHARS_PER_LINE/4) && (state_in->stepsnewline > 0)){
        *codechar++ = '\n';
        state_in->stepcount = 0;
      }
    }
  }
  /* control should not reach here */
  return codechar - code_out;
}

int base64_encode_blockend(char* code_out, base64_encodestate* state_in){
  char* codechar = code_out;
  
  switch (state_in->step){
  case step_B:
    *codechar++ = base64_encode_value(state_in->result);
    *codechar++ = '=';
    *codechar++ = '=';
    break;
  case step_C:
    *codechar++ = base64_encode_value(state_in->result);
    *codechar++ = '=';
    break;
  case step_A:
    break;
  }
  *codechar = 0x00;
  
  return codechar - code_out;
}

int base64_encode_chars(const char* plaintext_in, int length_in, char* code_out){
  base64_encodestate _state;
  base64_init_encodestate(&_state);
  int len = base64_encode_block(plaintext_in, length_in, code_out, &_state);
  return len + base64_encode_blockend((code_out + len), &_state);
}

};
