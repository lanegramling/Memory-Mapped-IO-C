/*
 * Example of using mmap. Taken from Advanced Programming in the Unix
 * Environment by Richard Stevens.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h> /* mmap() is defined in this header */
#include <fcntl.h>
#include <unistd.h>
#include <string.h>  /* memcpy */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

void err_quit (const char * mesg)
{
  printf ("%s\n", mesg);
  exit(1);
}

void err_sys (const char * mesg)
{
  perror(mesg);
  exit(errno);
}

int main (int argc, char *argv[])
{
  int fdin, fdout, i;
  char *src, *dst, buf[256];
  struct stat statbuf;

  src = dst = NULL;

  if (argc != 3)
    err_quit ("usage: memmap <fromfile> <tofile>");

  /*
   * open the input file
   */
  if ((fdin = open (argv[1], O_RDONLY)) < 0) {
    sprintf(buf, "can't open %s for reading", argv[1]);
    perror(buf);
    exit(errno);
  }

  /*
   * open/create the output file
   */
  if ((fdout = open (argv[2], O_RDWR | O_CREAT | O_TRUNC, 0644)) < 0) {
    sprintf (buf, "can't create %s for writing", argv[2]);
    perror(buf);
    exit(errno);
  }

  /*
   * 1. find size of input file
   */
    if (fstat(fdin, &statbuf)) err_sys("\n[fstat] Failed to read file size of input file");
    else printf("\n[fstat] Successfully read file size of input file.");

  /*
   * 2. go to the location corresponding to the last byte
   */
   if (lseek(fdout, statbuf.st_size - 1, SEEK_SET) == -1) err_sys("\n[lseek] Failed to seek the last byte");
   else printf("\n[lseek] Successfuly moved fdout to last byte...");
  /*
   * 3. write a dummy byte at the last location
   */
   if (write(fdout, "1", 1) == -1) err_sys("\n[write] Dummy write failed");
   else printf("\n[write] Dummy write succeeded...");

  /*
   * 4. mmap the input file
   */
   //set src map with found file size, read-only, map_shared, input fd, and 0 offset.
   src = mmap(NULL, statbuf.st_size, PROT_READ, MAP_SHARED, fdin, 0);
   if (src == -1) err_sys("\n[mmap] Memory map of input file failed.");
   else printf("\n[mmap] Memory map of input file succeeded...");

  /*
   * 5. mmap the output file
   */
   //set dst map with found file size, read+write, map_shared, output fd, and 0 offset.
   dst = mmap(NULL, statbuf.st_size, (PROT_READ | PROT_WRITE), MAP_SHARED, fdout, 0);
   if (dst == -1) err_sys("\n[mmap] Memory map of output file failed.");
   else printf("\n[mmap] Memory map of output file succeeded...");

  /*
   * 6. copy the input file to the output file
   */
    /* Memory can be dereferenced using the * operator in C.  This line
     * stores what is in the memory location pointed to by src into
     * the memory location pointed to by dest.
     */

     //Copy the memory from the src to the destination using memcpy
     printf("\n[memcpy] Copying memory from source to destination...\n\n");
     memcpy(dst, src, statbuf.st_size);

    *dst = *src;

}
