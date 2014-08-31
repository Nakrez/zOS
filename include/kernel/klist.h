#ifndef KLIST_H
# define KLIST_H

# include <kernel/zos.h>

struct klist
{
    struct klist *prev;
    struct klist *next;
};

static inline void klist_head_init(struct klist *name)
{
    name->prev = name;
    name->next = name;
}

static inline int klist_empty(struct klist *head)
{
    return head->prev == head;
}

static inline void klist_add(struct klist *insert_pt, struct klist *new)
{
    new->prev = insert_pt;
    new->next = insert_pt->next;

    insert_pt->next->prev = new;
    insert_pt->next = new;
}

static inline void klist_add_back(struct klist *insert_pt, struct klist *new)
{
    new->prev = insert_pt->prev;
    new->next = insert_pt;

    insert_pt->prev->next = new;
    insert_pt->prev = new;
}

static inline void klist_del(struct klist *elem)
{
    if (elem->prev)
        elem->prev->next = elem->next;
    if (elem->next)
        elem->next->prev = elem->prev;

    elem->prev = NULL;
    elem->next = NULL;
}

# define klist_elem(elem, type, field)                                  \
    (type *)((char *)elem - (char *)&((type *)0)->field)

# define klist_for_each(head, name, field)                              \
    for (struct klist *name = (head)->next, *next = name->next;          \
         name != head;                                                  \
         name = next, next = next->next)

# define klist_for_each_elem(head, elem, field)                         \
    for (elem = klist_elem((head)->next, __typeof__(*elem), field);     \
         &elem->field != head;                                          \
         elem = klist_elem(elem->field.next, __typeof__(*elem), field))

#endif /* !KLIST_H */
