#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

#include "queue.h"

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (!head)
        return NULL;
    head->next = head;
    head->prev = head;
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;
    element_t *ele, *safe = NULL;
    list_for_each_entry_safe (ele, safe, head, list) {
        q_release_element(ele);
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;
    element_t *ele = malloc(sizeof(element_t));
    if (!ele)
        return false;
    ele->value = strdup(s);
    if (!ele->value) {
        free(ele);
        return false;
    }
    list_add(&ele->list, head);

    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;
    element_t *ele = malloc(sizeof(element_t));
    if (!ele)
        return false;
    ele->value = strdup(s);
    if (!ele->value) {
        free(ele);
        return false;
    }
    list_add_tail(&ele->list, head);

    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *ele = list_entry(head->next, element_t, list);
    list_del(&ele->list);
    if (sp) {
        strncpy(sp, ele->value, strnlen(ele->value, bufsize - 1) + 1);
        sp[bufsize - 1] = '\0';
    }

    return ele;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *ele = list_entry(head->prev, element_t, list);
    list_del(&ele->list);
    if (sp) {
        strncpy(sp, ele->value, strnlen(ele->value, bufsize - 1) + 1);
        sp[bufsize - 1] = '\0';
    }

    return ele;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;
    struct list_head *slow = head->next, *fast = head->next;
    for (; fast->next != head && fast->next->next != head;
         slow = slow->next, fast = fast->next->next)
        ;
    list_del(slow);
    q_release_element(list_entry(slow, element_t, list));
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;

    bool is_this_dup = false, is_next_dup;
    element_t *ele, *safe = NULL;

    list_for_each_entry_safe (ele, safe, head, list) {
        if (ele->list.next != head && strcmp(ele->value, safe->value) == 0)
            is_next_dup = true;
        else
            is_next_dup = false;

        if (is_this_dup || is_next_dup) {
            list_del(&ele->list);
            q_release_element(ele);
        }
        is_this_dup = is_next_dup;
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    struct list_head *fast = head->next->next, *slow = head;
    while (slow->next != head && slow->next->next != head) {
        list_move(fast, slow);
        slow = slow->next->next;
        fast = slow->next->next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    struct list_head *pos, *safe;
    list_for_each_safe (pos, safe, head) {
        list_move(pos, head);
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    if (!head || list_empty(head) || list_is_singular(head) || k <= 0)
        return;
    int pos = 0;
    struct list_head *node, *safe, splice_head;
    INIT_LIST_HEAD(&splice_head);
    list_for_each_safe (node, safe, head) {
        if (++pos % k == 0) {
            if (k > pos)
                return;
            struct list_head tmp;
            list_cut_position(&tmp, head, node);
            q_reverse(&tmp);
            list_splice_tail(&tmp, &splice_head);
        }
    }
    list_splice(&splice_head, head);
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    struct list_head *node, *tmp;
    for (node = head->next; node != head;) {
        struct list_head *target = node;
        for (tmp = node; tmp != head; tmp = tmp->next) {
            int result = strcmp(list_entry(target, element_t, list)->value,
                                list_entry(tmp, element_t, list)->value);
            if ((descend && result > 0) || (!descend && result < 0)) {
                target = tmp;
            }
        }
        struct list_head *safe = node->next;
        list_move(target, head);
        if (target == node)
            node = safe;
    }
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;
    element_t *ele, *safe;
    int count = 0, total = 0;
    char min = 127;

    for (ele = list_entry((head)->prev, element_t, list),
        safe = list_entry(ele->list.prev, element_t, list);
         &ele->list != (head);
         ele = safe, safe = list_entry(safe->list.prev, element_t, list)) {
        total++;
        if (*ele->value > min) {
            list_del(&ele->list);
            q_release_element(ele);
            count++;
            continue;
        }
        if (*ele->value < min)
            min = *ele->value;
    }
    return total - count;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;
    element_t *ele, *safe;
    int count = 0, total = 0;
    char max = 0;

    for (ele = list_entry((head)->prev, element_t, list),
        safe = list_entry(ele->list.prev, element_t, list);
         &ele->list != (head);
         ele = safe, safe = list_entry(safe->list.prev, element_t, list)) {
        total++;
        if (*ele->value < max) {
            list_del(&ele->list);
            q_release_element(ele);
            count++;
            continue;
        }
        if (*ele->value > max)
            max = *ele->value;
    }
    return total - count;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    if (!head || list_empty(head))
        return 0;
    int count = 0;

    queue_contex_t *chain = list_entry(head->next, queue_contex_t, chain);
    struct list_head *chain_head, *node, *safe, *merge_queue;
    merge_queue = chain->q;

    if (!merge_queue)
        return 0;
    struct list_head *pos;
    list_for_each (pos, merge_queue) {
        count++;
    }
    if (list_is_singular(head->next))
        return count;

    list_for_each (chain_head, &chain->chain) {
        if (chain_head == head)
            break;
        struct list_head *q_head =
            list_entry(chain_head, queue_contex_t, chain)->q;
        struct list_head *tmp = merge_queue->next;

        if (!q_head || list_empty(q_head))
            continue;

        list_for_each_safe (node, safe, q_head) {
            if (!descend) {
                while (tmp != merge_queue &&
                       strcmp(list_entry(tmp, element_t, list)->value,
                              list_entry(node, element_t, list)->value) < 0) {
                    tmp = tmp->next;
                }
            } else {
                while (tmp != merge_queue &&
                       strcmp(list_entry(tmp, element_t, list)->value,
                              list_entry(node, element_t, list)->value) > 0) {
                    tmp = tmp->next;
                }
            }
            list_move_tail(node, tmp);
            count++;
        }
    }

    return count;
}
