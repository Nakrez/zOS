#ifndef ARCH_X86_PM_H
# define ARCH_X86_PM_H

# include <arch/gdt.h>

# define KERNEL_CS KERNEL_SELECTOR(GDT_KERNEL_CS)
# define KERNEL_DS KERNEL_SELECTOR(GDT_KERNEL_DS)
# define USER_CS USER_SELECTOR(GDT_USER_CS)
# define USER_DS USER_SELECTOR(GDT_USER_DS)

void pm_setup(void);

#endif /* !ARCH_X86_PM_H */
