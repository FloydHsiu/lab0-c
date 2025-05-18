#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *q = malloc(sizeof(struct list_head));
    if (!q)
        return NULL;

    INIT_LIST_HEAD(q);
    return q;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    struct list_head *pos, *n;

    if (!head)
        return;

    list_for_each_safe(pos, n, head) {
        element_t *e;
        e = list_entry(pos, element_t, list);
        free(e->value);
        free(e);
    }

    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *e = malloc(sizeof(element_t));
    if (!e)
        return false;

    e->value = strdup(s);
    if (!e->value) {
        free(e);
        return false;
    }

    INIT_LIST_HEAD(&e->list);
    list_add(&e->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *e = malloc(sizeof(element_t));
    if (!e)
        return false;

    e->value = strdup(s);
    if (!e->value) {
        free(e);
        return false;
    }

    INIT_LIST_HEAD(&e->list);
    list_add_tail(&e->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *e = list_entry(head->next, element_t, list);

    if (sp) {
        strncpy(sp, e->value, bufsize);
        sp[bufsize - 1] = '\0';
    }

    list_del(&e->list);
    INIT_LIST_HEAD(&e->list);

    return e;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *e = list_entry(head->prev, element_t, list);

    if (sp) {
        strncpy(sp, e->value, bufsize);
        sp[bufsize - 1] = '\0';
    }

    list_del(&e->list);
    INIT_LIST_HEAD(&e->list);

    return e;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return 0;
    }

    int size = 0;
    struct list_head *node = head->next;
    while (node != head) {
        size++;
        node = node->next;
    }

    return size;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;

    struct list_head *forward = head->next;
    struct list_head *backward = head->prev;

    while (!(forward == backward || forward->prev == backward)) {
        forward = forward->next;
        backward = backward->prev;
    }

    if (forward != head) {
        element_t *e = list_entry(forward, element_t, list);
        list_del(forward);
        free(e->value);
        free(e);
    } else
        return false;

    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head))
        return false;

    struct list_head *node = head->next;
    struct list_head *next;
    bool is_duplicated = false;

    while (node != head) {
        element_t *e_curr, *e_next;
        next = node->next;
        e_curr = list_entry(node, element_t, list);

        if (next == head) {
            if (is_duplicated) {
                list_del(node);
                free(e_curr->value);
                free(e_curr);
            }
            break;
        }

        e_next = list_entry(next, element_t, list);

        if (0 == strcmp(e_curr->value, e_next->value)) {
            list_del(next);
            free(e_next->value);
            free(e_next);
            is_duplicated = true;
        } else {
            if (is_duplicated) {
                list_del(node);
                free(e_curr->value);
                free(e_curr);
            }
            node = next;
            is_duplicated = false;
        }
    }

    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    struct list_head *n0, *n1, *n2;

    if (!head)
        return;

    if (list_is_singular(head))
        return;

    n0 = head;
    n1 = n0->next;
    n2 = n1->next;

    while (n1 != head && n2 != head) {
        n2->next->prev = n1;
        n0->next = n2;

        n1->next = n2->next;
        n1->prev = n2;
        n2->prev = n0;
        n2->next = n1;

        n0 = n1;
        n1 = n0->next;
        n2 = n1->next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head)
        return;

    struct list_head *curr = head;
    struct list_head *prev = head->prev;

    while (prev != head) {
        prev = curr->prev;
        curr->prev = curr->next;
        curr->next = prev;
        curr = prev;
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head))
        return;
    if (list_is_singular(head))
        return;
    if (k == 1)
        return;

    struct list_head *prev = head;
    struct list_head *curr = head->next;
    struct list_head *front, *back;
    int size = q_size(head);
    int count = 0;

    size = size - (size % k);

    while (count < size) {
        if ((count % k) == (k - 1)) {
            front = curr;
            back = prev->next;
            curr = curr->next;

            front->next = front->prev;
            front->prev = prev;
            prev->next = front;
            back->next = curr;
            curr->prev = back;
            prev = back;
        } else {
            struct list_head *t_node = curr->next;
            curr->next = curr->prev;
            curr->prev = t_node;
            curr = curr->prev;
        }
        count++;
    }
}

struct list_head *q_mid(struct list_head *snode, struct list_head *enode)
{
    struct list_head *first = snode->next->next;
    struct list_head *second = snode->next;

    while (first != enode && first->next != enode) {
        first = first->next->next;
        second = second->next;
    }

    return second;
}

void q_sort_helper(struct list_head *snode,
                   struct list_head *enode,
                   bool descend)
{
    if (snode->next == enode || snode == enode->prev ||
        snode->next == enode->prev)
        return;

    struct list_head *p = q_mid(snode, enode);

    q_sort_helper(snode, p->next, descend);
    q_sort_helper(p, enode, descend);

    struct list_head *tail = snode;
    struct list_head *node1 = snode->next;
    struct list_head *node2 = p->next;

    while (node1 != p->next && node2 != enode) {
        const element_t *e1 = list_entry(node1, element_t, list);
        const element_t *e2 = list_entry(node2, element_t, list);

        int comp = strcmp(e1->value, e2->value);
        if (descend ? (0 < comp) : (0 > comp) || comp == 0) {
            tail->next = node1;
            node1->prev = tail;
            tail = node1;
            node1 = node1->next;
        } else {
            tail->next = node2;
            node2->prev = tail;
            tail = node2;
            node2 = node2->next;
        }
    }

    if (node1 != p->next) {
        tail->next = node1;
        node1->prev = tail;
        enode->prev = p;
        p->next = enode;
    }

    if (node2 != enode) {
        tail->next = node2;
        node2->prev = tail;
    }
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    q_sort_helper(head, head, descend);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;
    if (list_is_singular(head))
        return 1;

    struct list_head *node = head->prev;

    while (node->prev != head) {
        const element_t *curr = list_entry(node, element_t, list);
        element_t *prev = list_entry(node->prev, element_t, list);

        if (strcmp(curr->value, prev->value) < 0) {
            list_del(&prev->list);
            free(prev->value);
            free(prev);
        } else {
            node = node->prev;
        }
    }

    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;
    if (list_is_singular(head))
        return 1;

    struct list_head *node = head->prev;

    while (node->prev != head) {
        const element_t *curr = list_entry(node, element_t, list);
        element_t *prev = list_entry(node->prev, element_t, list);

        if (strcmp(curr->value, prev->value) > 0) {
            list_del(&prev->list);
            free(prev->value);
            free(prev);
        } else {
            node = node->prev;
        }
    }

    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head))
        return 0;

    if (list_is_singular(head))
        return 1;

    struct list_head *first = head->next;
    struct list_head *curr = first->next;

    while (curr != head) {
        struct list_head *q1 = first;
        struct list_head *q2 = curr;

        queue_contex_t *qc1 = list_entry(q1, queue_contex_t, chain);
        queue_contex_t *qc2 = list_entry(q2, queue_contex_t, chain);

        qc1->size += qc2->size;

        struct list_head *tail = qc1->q;
        struct list_head *node1 = qc1->q->next;
        struct list_head *node2 = qc2->q->next;

        while (node1 != qc1->q && node2 != qc2->q) {
            const element_t *e1 = list_entry(node1, element_t, list);
            const element_t *e2 = list_entry(node2, element_t, list);

            int comp = strcmp(e1->value, e2->value);
            if (descend ? (0 < comp) : (0 > comp)) {
                tail->next = node1;
                node1->prev = tail;
                tail = node1;
                node1 = node1->next;
            } else {
                tail->next = node2;
                node2->prev = tail;
                tail = node2;
                node2 = node2->next;
            }
        }

        if (node1 != qc1->q) {
            tail->next = node1;
            node1->prev = tail;
        }

        if (node2 != qc2->q) {
            tail->next = node2;
            node2->prev = tail;
            qc2->q->prev->next = qc1->q;
            qc1->q->prev = qc2->q->prev;
        }

        INIT_LIST_HEAD(qc2->q);

        curr = curr->next;
    }

    const queue_contex_t *qc = list_entry(head->next, queue_contex_t, chain);

    return qc->size;
}
