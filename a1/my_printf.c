#include <stdio.h>
#include <string.h>

/* Convert the integer D to a string and save the string in BUF. If
   BASE is equal to 'd', interpret that D is decimal, and if BASE is
   equal to 'x', interpret that D is hexadecimal. */
void itoa (char *buf, int base, int d)
{
  char *p = buf;
  char *p1, *p2;
  unsigned long ud = d;
  int divisor = 10;
     
  /* If %d is specified and D is minus, put `-' in the head. */
  if (base == 'd' && d < 0)
    {
      *p++ = '-';
      buf++;
      ud = -d;
    }
  else if (base == 'x')
    divisor = 16;
 
  /* Divide UD by DIVISOR until UD == 0. */
  do
    {
      int remainder = ud % divisor;
     
      *p++ = (remainder < 10) ? remainder + '0' : remainder + 'a' - 10;
    }
  while (ud /= divisor);
     
  /* Terminate BUF. */
  *p = 0;
     
  /* Reverse BUF. */
  p1 = buf;
  p2 = p - 1;
  while (p1 < p2)
    {
      char tmp = *p1;
      *p1 = *p2;
      *p2 = tmp;
      p1++;
      p2--;
    }
}


void my_printf(const char *str, ...){

  //argument parsing
  unsigned long long getarg[10];
  //copy registers into buffer using inline assembly
  //write first 6 registers to diff indexes of the buffer
  //move rbp by 16 (address) to caller saved rdi
  //move 8bytes up
  asm("mov %%rsi, 0(%0)\n"
      "mov %%rdx, 8(%0)\n"
      "mov %%rcx, 16(%0)\n"
      "mov %%r8, 24(%0)\n"
      "mov %%r9, 32(%0)\n"
      "mov 16(%%rbp),%%rdi\n"
      "mov %%rdi, 40(%0)\n"
      "mov 24(%%rbp),%%rdi\n"
      "mov %%rdi, 48(%0)\n"
      "mov 32(%%rbp),%%rdi\n"
      "mov %%rdi, 56(%0)\n"
      "mov 40(%%rbp),%%rdi\n"
      "mov %%rdi, 64(%0)\n"
      "mov 48(%%rbp),%%rdi\n"
      "mov %%rdi, 72(%0)\n"
      :
      :"r" (getarg)
      :"memory");

  //process getargs
  size_t len = strlen(str);
  //printf("%d \n", len);                                                                                                                                                                           
  int i = 0;
  int seenargs = 0;

  //check for arguments
  while(i<len) {
    if(str[i] == '%') {
      //if there is a specified type process it and print iy
      switch(str[i+1]) {

        case 's': {

	  char * argument = (char *)getarg[seenargs++];
	  //print the argument
	  puts(argument);
	  break;
        }

      case 'c': { 

	char argument = (char)getarg[seenargs++];
	putchar(argument);
	break;
      }

      case 'd': {

       int argument = (int)getarg[seenargs++];
       //declare a buffer
       char buffer[21];
       memset(buffer, 0, 21);
       //print decimal
       itoa(buffer, 'd', argument);
       puts(buffer);
       break;
      }

      case 'u': {
       int argument = (int)getarg[seenargs++];
       //declare a buffer                                                                                                                            
       char buffer[21];
       memset(buffer, 0, 21);
       //print unsigned
       itoa(buffer, 'u', argument);
       puts(buffer);
       break;
      }

      case 'x': {
	//hex
       int argument = (int)getarg[seenargs++];
       //declare a buffer                                                                                                                            
       char buffer[21];
       memset(buffer, 0, 21);
       //print hex
       itoa(buffer, 'x', argument);
       puts(buffer);
       break;
      }
      }
      
      i = i+2;
    } else {
    
    putchar(str[i]);
    i++;
    }
  }

}

int main(){
  my_printf("%u", 39);
  my_printf("%x", 39);
  my_printf("%d",10);
  my_printf("%s", "hi");
  my_printf("%d %d %d %d %d %d %d %d %d %d", 1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
  my_printf("%s, %s", "hello", "How are you");
}
