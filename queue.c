#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (head == NULL) {
        return NULL;
    }
    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (head == NULL) {
        return;
    }
    struct list_head *pos, *q;
    list_for_each_safe (pos, q, head) {
        element_t *ele = list_entry(pos, element_t, list);
        list_del(pos);
        free(ele->value);
        free(ele);
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
    char *tmp = malloc(strlen(s) + 1);
    if (!tmp) {
        free(ele);
        return false;
    }
    ele->value = tmp;
    memcpy(ele->value, s, strlen(s) + 1);
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
    char *tmp = malloc(strlen(s) + 1);
    if (!tmp) {
        free(ele);
        return false;
    }
    ele->value = tmp;
    memcpy(ele->value, s, strlen(s) + 1);
    list_add_tail(&ele->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    const struct list_head *origin = head;
    if (!origin || !sp || bufsize == 0)
        return NULL;
    if (list_empty(origin))
        return NULL;
    element_t *ele = list_first_entry(origin, element_t, list);
    memcpy(sp, ele->value,
           strlen(ele->value) > bufsize ? bufsize - 1 : strlen(ele->value) + 1);
    sp[bufsize - 1] = '\0';
    list_del(&ele->list);
    return ele;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    const struct list_head *origin = head;
    if (!origin || !sp || bufsize == 0)
        return NULL;
    if (list_empty(origin))
        return NULL;
    element_t *ele = list_last_entry(origin, element_t, list);
    memcpy(sp, ele->value,
           strlen(ele->value) > bufsize ? bufsize - 1 : strlen(ele->value) + 1);
    sp[bufsize - 1] = '\0';
    list_del(&ele->list);
    return ele;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return -1;
    struct list_head *pos;
    int count = 0;
    list_for_each (pos, head) {
        count++;
    }
    return count;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head)
        return false;
    struct list_head *slow = head;
    struct list_head *fast = head;
    const struct list_head *prev = head->prev;
    do {
        slow = slow->next;
        fast = fast->next->next;
    } while (fast != prev && fast != head);
    list_del(slow);
    element_t *node = list_entry(slow, element_t, list);
    free(node->value);
    free(node);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head)
        return false;
    struct list_head *pos, *tmp;
    bool dup = false;
    list_for_each_safe (pos, tmp, head) {
        element_t *ele = list_entry(pos, element_t, list);
        const element_t *next = list_entry(pos->next, element_t, list);
        if (pos->next != head && strcmp(ele->value, next->value) == 0) {
            list_del(pos);
            free(ele->value);
            free(ele);
            dup = true;
        } else if (dup) {
            list_del(pos);
            free(ele->value);
            free(ele);
            dup = false;
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_empty(head))
        return;
    struct list_head *pos;
    list_for_each (pos, head) {
        if (pos->next == head)
            break;
        struct list_head *node1, *node2;
        struct list_head *prev, *next;
        prev = pos->prev;
        node1 = pos;
        node2 = pos->next;
        next = node2->next;

        prev->next = node2;
        node2->next = node1;
        node1->next = next;
        next->prev = node1;
        node1->prev = node2;
        node2->prev = prev;
        pos = node1;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    struct list_head q;
    INIT_LIST_HEAD(&q);
    struct list_head *pos, *tmp;
    list_for_each_safe (pos, tmp, head) {
        list_del(pos);
        list_add(pos, &q);
    }
    head->next = q.next;
    head->prev = q.prev;
    q.prev->next = head;
    q.next->prev = head;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    int count = 0;
    LIST_HEAD(tmp);
    struct list_head *pos, *safe, *khead = head;
    list_for_each_safe (pos, safe, head) {
        if (++count == k) {
            list_cut_position(&tmp, khead, pos);
            q_reverse(&tmp);
            list_splice_init(&tmp, khead);
            khead = safe->prev;
            count = 0;
        }
    }
}

static struct list_head *__merge(struct list_head *l1,
                                 struct list_head *l2,
                                 bool descend);

static struct list_head *__merge_sort(struct list_head *node, bool descend)
{
    LIST_HEAD(head);
    struct list_head *const prev = node->prev;
    list_splice_init(node->prev, &head);
    struct list_head *slow, *fast, *mid;
    slow = &head;
    fast = head.next;
    do {
        slow = slow->next;
        fast = fast->next->next;
    } while (fast != &head && fast->next != &head);
    mid = slow;
    LIST_HEAD(hleft);
    struct list_head *left, *right;

    list_cut_position(&hleft, &head, mid);
    left = hleft.next;
    right = head.next;

    if (!list_is_singular(&hleft))
        left = __merge_sort(hleft.next, descend);

    if (!list_is_singular(&head))
        right = __merge_sort(head.next, descend);

    __merge(left, right, descend);
    list_splice_init(&hleft, prev);

    return prev->next;
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    __merge_sort(head->next, descend);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;
    struct list_head *pos, *safe;
    for (pos = (head)->prev, safe = pos->prev; pos != (head);
         pos = safe, safe = pos->prev) {
        const element_t *ele = list_entry(pos, element_t, list);
        element_t *prev = list_entry(pos->prev, element_t, list);
        if (pos->prev != head && strcmp(prev->value, ele->value) > 0) {
            list_del(&prev->list);
            free(prev->value);
            free(prev);
            safe = pos;
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
    struct list_head *pos, *safe;
    for (pos = (head)->prev, safe = pos->prev; pos != (head);
         pos = safe, safe = pos->prev) {
        const element_t *ele = list_entry(pos, element_t, list);
        element_t *prev = list_entry(pos->prev, element_t, list);
        if (pos->prev != head && strcmp(prev->value, ele->value) < 0) {
            list_del(&prev->list);
            free(prev->value);
            free(prev);
            safe = pos;
        }
    }
    return q_size(head);
}

static struct list_head *__merge(struct list_head *l1,
                                 struct list_head *l2,
                                 bool descend)
{
    if (!l1 || !l2)
        return l1;
    struct list_head *const prev1 = l1->prev;
    struct list_head *const prev2 = l2->prev;
    LIST_HEAD(tmp);
    while (!list_empty(prev1) && !list_empty(prev2)) {
        element_t *ele1 = list_first_entry(prev1, element_t, list);
        element_t *ele2 = list_first_entry(prev2, element_t, list);
        if (descend) {
            element_t *node =
                strcmp(ele1->value, ele2->value) > 0 ? ele1 : ele2;
            list_move_tail(&node->list, &tmp);
        } else {
            element_t *node =
                strcmp(ele1->value, ele2->value) < 0 ? ele1 : ele2;
            list_move_tail(&node->list, &tmp);
        }
    }
    list_splice_tail_init(prev1, &tmp);
    list_splice_tail_init(prev2, &tmp);
    list_splice(&tmp, prev1);
    return prev1->next;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    const struct list_head *origin = head;
    if (!origin || list_empty(origin))
        return 0;
    else if (list_is_singular(origin))
        return q_size(list_first_entry(origin, queue_contex_t, chain)->q);
    queue_contex_t *pos, *safe;
    queue_contex_t *first_queue =
        list_entry(origin->next, queue_contex_t, chain);
    list_for_each_entry_safe (pos, safe, origin, chain) {
        if (&pos->chain == origin)
            break;
        // merge next queue to the first queue
        if (pos == first_queue)
            continue;
        __merge(first_queue->q->next, pos->q->next, descend);
    }
    return q_size(first_queue->q);
}
