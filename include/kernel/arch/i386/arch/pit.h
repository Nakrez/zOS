#ifndef I386_PIT_H
# define I386_PIT_H

# define PIT_RATE 1193180

# define PIT_CMD 0x43
# define PIT0_DATA 0x40
# define PIT1_DATA 0x41
# define PIT2_DATA 0x42

# define PIT_VALUE_MAX 65636

void pit_initialize(void);

#endif /* !I386_PIT_H */
