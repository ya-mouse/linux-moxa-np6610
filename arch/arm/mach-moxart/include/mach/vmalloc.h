#ifndef _VMALLOC_H_
#define _VMALLOC_H_

#define VMALLOC_OFFSET	  (8*1024*1024)
#define VMALLOC_START	  (((unsigned long)high_memory + VMALLOC_OFFSET) & ~(VMALLOC_OFFSET-1))
#define VMALLOC_VMADDR(x) ((unsigned long)(x))
#define VMALLOC_END       (PAGE_OFFSET + 0x10000000)

#endif
