#ifndef KLIST_H
# define KLIST_H

struct klist
{
    struct klist *prev;
    struct klist *next;
};

#endif /* !KLIST_H */
