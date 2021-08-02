#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
queue_t *q_new()
{
    queue_t *q = malloc(sizeof(queue_t));
    if (q == NULL) {
        return NULL;
    }
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
    return q;
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{
    if (!q)
        return;
    while (q->head != NULL) {
        list_ele_t *tmp = q->head->next;
        free(q->head->value);
        free(q->head);
        q->head = tmp;
    }
    free(q);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(queue_t *q, char *s)
{
    if (q == NULL) {
        return false;
    }
    list_ele_t *newh = malloc(sizeof(list_ele_t));
    if (newh == NULL) {
        return false;
    }
    size_t strSize = strlen(s) + 1;
    newh->value = malloc(sizeof(char) * strSize);
    if (!newh->value) {
        free(newh);
        return false;
    }
    snprintf(newh->value, strSize, "%s", s);
    q->size++;
    if (!q->tail) {
        q->tail = newh;
    }
    newh->next = q->head;
    q->head = newh;
    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(queue_t *q, char *s)
{
    if (!q)
        return false;
    list_ele_t *newt;
    newt = malloc(sizeof(list_ele_t));
    if (!newt)
        return false;
    size_t strSize = strlen(s) + 1;
    newt->value = malloc(sizeof(char) * strSize);
    if (!newt->value) {
        free(newt);
        return false;
    }
    snprintf(newt->value, strSize, "%s", s);
    q->size++;
    if (!q->tail) {
        q->tail = newt;
        newt->next = q->head;
        q->head = newt;
        /* If the tail is NULL, the whole queue is empty. So, inserting tail is
         * same as inserting head. */
        return true;
    }
    q->tail->next = newt;
    q->tail = newt;
    q->tail->next = NULL;
    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return true if successful.
 * Return false if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 * The space used by the list element and the string should be freed.
 */
bool q_remove_head(queue_t *q, char *sp, size_t bufsize)
{
    if (!q || q->size == 0)
        return false;
    list_ele_t *rmvtarget = q->head;
    q->head = q->head->next;
    if (sp) {
        size_t slen = strlen(rmvtarget->value) + 1;
        size_t spSize = (bufsize > slen) ? slen : bufsize - 1;
        memset(sp, '\0', spSize + 1);
        snprintf(sp, spSize + 1, "%s", rmvtarget->value);
    }
    free(rmvtarget->value);
    free(rmvtarget);
    q->size--;
    return true;
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
    if (!q)
        return 0;
    return q->size;
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(queue_t *q)
{
    if (!q)
        return;
    if (q->size <= 1)
        return;
    list_ele_t *prev = q->head;
    list_ele_t *current = q->head->next;
    list_ele_t *next = q->head->next->next;
    do {
        current->next = prev;
        if (!next)
            break;
        prev = current;
        current = next;
        next = next->next;
    } while (current);
    list_ele_t *tmp = q->head;
    q->head = q->tail;
    q->tail = tmp;
    q->tail->next = NULL;
}
list_ele_t *merge(queue_t *q, list_ele_t *headA, list_ele_t *headB)
{
    list_ele_t *tmp;
    list_ele_t *head;
    if (strcmp(headB->value, headA->value) > 0) {
        tmp = headA;
        headA = headA->next;
    } else {
        tmp = headB;
        headB = headB->next;
    }
    head = tmp;
    while (headA && headB) {
        if (strcmp(headB->value, headA->value) > 0) {
            tmp->next = headA;
            tmp = tmp->next;
            headA = headA->next;
        } else {
            tmp->next = headB;
            tmp = tmp->next;
            headB = headB->next;
        }
    }
    if (headA) {
        tmp->next = headA;
        while (headA->next) {
            headA = headA->next;
        }
        q->tail = headA;
    } else if (headB) {
        tmp->next = headB;
        while (headB->next) {
            headB = headB->next;
        }
        q->tail = headB;
    }
    return head;
}
list_ele_t *mergeSort(queue_t *q, list_ele_t *head)
{
    if (!head || !head->next)
        return head;
    list_ele_t *fast = head->next;
    list_ele_t *slow = head;
    while (fast && fast->next) {
        fast = fast->next->next;
        slow = slow->next;
    }
    fast = slow->next;
    slow->next = NULL;
    list_ele_t *headA = mergeSort(q, head);
    list_ele_t *headB = mergeSort(q, fast);
    return merge(q, headA, headB);
}
/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(queue_t *q)
{
    if (!q)
        return;
    if (q->size <= 1)
        return;
    q->head = mergeSort(q, q->head);
}
