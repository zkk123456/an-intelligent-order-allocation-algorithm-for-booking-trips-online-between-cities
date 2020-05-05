#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <windows.h>
#include <string.h>
#include <assert.h>
#include "linkList.h"

//创建队列时，头尾指针均指向data域为0的节点.
pQueue CreatedQueue(void) {
	pQueue pq = new(Queue);
	assert(pq != NULL);
	//pListNode pn = CreateNode(0);
	//assert(pn != NULL);
	pq->size = 0;
	pq->headLink = NULL; //pn;
	pq->tailLink = NULL; //pn;
	return pq;
}

pListNode CreateNode(int ind, double value) {
	pListNode pn = new(ListNode);
	assert(pn != NULL);
	pn->ind = ind;
	pn->data = value;
	pn->next = NULL;
	return pn;
}

//删除节点是删除headLink指向的节点，改变headLink指向
pListNode popQueue(pQueue queue) {
	assert(queue != NULL);
	if (queue->size == 0)
		return NULL;
	pListNode pn = queue->headLink;
	queue->headLink = pn->next;
	pn->next = NULL;
	queue->size--;
	if (queue->size == 0)
		queue->tailLink = NULL;
	return pn;
}

//增加节点放在队尾，改变tailLink指向,添加第一个元素headLink和tailLink均指向这个节点
void pushQueue(pQueue queue, pListNode node) {
	assert(queue != NULL);
	assert(node != NULL);
	if (queue->size == 0) {
		queue->headLink = node;
		queue->tailLink = node;
	}
	else {
		queue->tailLink->next = node;
		queue->tailLink = node;
	}
	queue->size++;
}

void DestroyQueue(pQueue *queue) {
	assert(*queue != NULL);
	while ((*queue)->size-- != 0) { //清空所有节点
		pListNode pn = popQueue(*queue);
		DestroyListNode(&pn);
	}
	//FREE(queue->headLink);
	//FREE(queue->tailLink);
	FREE(*queue);
}

std::vector<int> FindValuesQueue(pQueue *queue, double low, double high, bool *U) {
	assert(*queue != NULL);
	std::vector<int> tmps;
	pListNode p = (*queue)->headLink;
	int count = -999999;
	while (p != NULL) { //清空所有节点
		if (U[p->ind] && p->data >= low && p->data < high) {
			tmps.push_back(p->ind);
			count = 0;
		}
		else
			count++;
		if (count > 5)
			break;

		p = p->next;

	}
	return tmps;
}

std::vector<int> DeleteValuesQueue(pQueue *queue, double low, double high) {
	assert(*queue != NULL);
	std::vector<int> tmps;
	pListNode p = (*queue)->headLink;
	while (p != NULL && p->data >= low && p->data < high) { //清空所有节点
		p = p->next;
		pListNode pn = popQueue(*queue);
		tmps.push_back(pn->ind);
		DestroyListNode(&pn);
	}
	return tmps;
}


void DeleteKeyQueue(pQueue *queue, double ind) {
	assert(*queue != NULL);
	//第一个不是m的节点，连续几个m，非m后移指针
	pListNode p, q;
	while ((*queue)->headLink && (*queue)->headLink->ind == ind) { //头结点不是m
		p = (*queue)->headLink;
		(*queue)->headLink = (*queue)->headLink->next;
		(*queue)->size--;
		free(p);
		return;
	}

	//有一个非m的头结点L，如果p下一个节点是m则删除下一节点
	p = (*queue)->headLink;
	while (p && p->next) {
		while (p->next && p->next->ind == ind) {
			q = p->next;
			p->next = q->next;
			(*queue)->size--;
			free(q);
			return;
		}
		//此时p的下一个节点为空或者不是m，将p后移一个位置为空或者不是m
		p = p->next;
	}
}

void DestroyListNode(pListNode *node) {
	assert(*node != NULL);
	(*node)->next = NULL;
	FREE(*node);
}

int LengthOfQueue(pQueue queue) {
	assert(queue != NULL);
	assert(queue->size == 0 || queue->size > 0);
	return queue->size;
}

void ShowQueue(pQueue queue) {
	pListNode pn = queue->headLink;
	if (pn == NULL)
		return;
	printf("ShowQueue Order ");
	int length = queue->size;
	while (length-- != 0) {
		printf(" [%f]", pn->data);
		pn = pn->next;
	}
	printf("\n");
}


void insertNode(pQueue queue, int ind, double value) {
	pListNode p_new = CreateNode(ind, value);
	pListNode pb = NULL, pf = NULL;

	pf = queue->headLink;
	queue->size++;

	if (queue->headLink == NULL) { // 1.头结点为空，直接插入
		queue->headLink = p_new;
		return;
	}

	while ((p_new->data >= pf->data) && (pf->next != NULL)) { // 2.遍历，找到插入位置的前后两个结点，pb,pf
		pb = pf;
		pf = pf->next;
	}

	if (p_new->data < pf->data) {// 3.找到一个pf指向的节点的num比新来的节点的num大，则插到pf节点的前面。
		if (pf == queue->headLink) {// 3.1.找的节点是头结点，插到最前面
			queue->headLink = p_new;
			p_new->next = pf;
		}else {// 3.2.找到的是中间节点，插在中间节点pf的前面
			pb->next = p_new;
			p_new->next = pf;
		}
	}else // 4.没有找到一个节点，即：比新来的节点都大，插在末尾
		pf->next = p_new;
}