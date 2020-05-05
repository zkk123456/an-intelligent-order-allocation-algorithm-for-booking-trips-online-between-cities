#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <windows.h>
#include <vector>

#define new(type) (type *)malloc(sizeof(type))
#define FREE(p) \
        if (p != NULL) {\
                free(p);\
                p = NULL;\
            }
typedef struct Node {
	int ind;
	double data;
	struct Node *next;
}ListNode, *pListNode;

typedef struct _Queue {
	int size;
	pListNode headLink;
	pListNode tailLink;
}Queue, *pQueue;

pQueue CreatedQueue(void);
pListNode CreateNode(int ind, double value);
pListNode popQueue(pQueue);
void pushQueue(pQueue queue, pListNode node);
void DestroyQueue(pQueue *queue);
void DestroyListNode(pListNode *node);
int LengthOfQueue(pQueue queue);
void ShowQueue(pQueue queue);
void insertNode(pQueue queue, int ind, double c);
std::vector<int> FindValuesQueue(pQueue *queue, double low, double high, bool *U);
void DeleteKeyQueue(pQueue *queue, double ind);
std::vector<int> DeleteValuesQueue(pQueue *queue, double low, double high);
