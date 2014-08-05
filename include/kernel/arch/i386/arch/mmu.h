#ifndef MMU_H
# define MMU_H

# define PD_PRESENT (1 << 0)
# define PD_WRITE (1 << 1)
# define PD_USER (1 << 2)
# define PD_4MB (1 << 7)

# define PAGE_SIZE 0x1000

#endif /* !MMU_H */
