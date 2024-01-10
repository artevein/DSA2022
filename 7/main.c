#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>

// https://openhome.cc/Gossip/CGossip/MallocFree.html
typedef struct node {
	int val;
	int expiry;
} node;

typedef struct heap {
	int company_indx; // for debug
	int tail; // index of last element
	node *A;
} heap;

typedef struct treenode {
	int indx;
	int count;
	int parent;
	struct treenode *child;
	struct treenode *next;
} treenode;

//source: https://judgegirl.csie.org/source/highlight/503983
//https://judgegirl.csie.org/problem/0/222
int readInt(int *num) {
    char c;
    while ((c = getchar()) < '-')
    {
        if(c == EOF)
        {
            return 0;
        }
    }
    *num = c - '0';
    while ((c = getchar()) >= '0')
    {
        *num = (*num << 3) + (*num << 1) + c - '0';
    }
    return 1;
}

int N, M, C;
void insert(heap *h, int val, int expiry) {
	h->tail++;
	h->A[h->tail].val = val;	
	h->A[h->tail].expiry = expiry;	

	int cur = h->tail;
	int tval = val, texp = expiry;
	int next = cur/2;
	while (next != 0 && h->A[cur].val < h->A[next].val) {
		// swap parent with child
		tval = h->A[cur].val;
		texp = h->A[cur].expiry;
		h->A[cur].val = h->A[next].val;
		h->A[cur].expiry = h->A[next].expiry;
		h->A[next].val = tval;
		h->A[next].expiry = texp;
		
		cur = next;
		next = next/2;
	}
}

void extract_min(heap *h) {
	int tail = h->tail;
	h->A[1].val = h->A[tail].val;
	h->A[1].expiry = h->A[tail].expiry;
	h->tail--;
	
	int cur = 1, smallest = 1;
	int tval = 0, texp = 0;
	while (cur*2 <= h->tail) {
		if (h->A[cur].val < h->A[cur*2].val) smallest = cur;
		else smallest = cur*2;
		if (cur*2+1 <= h->tail && h->A[cur*2+1].val < h->A[smallest].val) smallest = cur*2+1;

		if (cur == smallest) break;
		// write a swap fnction
		tval = h->A[cur].val;
		texp = h->A[cur].expiry;
		h->A[cur].val = h->A[smallest].val;
		h->A[cur].expiry = h->A[smallest].expiry;
		h->A[smallest].val = tval;
		h->A[smallest].expiry = texp;
		cur = smallest;
	}
}

void filter(heap *h, int date) { // deletes expired mins
	while (h->tail > 0 && date >= h->A[1].expiry) extract_min(h);
}

int get_min(heap *h) { 
	return h->A[1].val;
}

int max = 0;
long int best_offer(heap *prices, treenode *cur) {
	long int cost = get_min(&prices[cur->indx]);
	treenode *traverse = cur->child; 
	while (traverse != NULL) {
		cost += best_offer(prices, traverse);
		traverse = traverse->next;
	}
	if (cost <= C && cur->count > max) max = cur->count;
	return cost;
}

int find_subcompany(treenode *cur) {
	treenode *traverse = cur->child; 
	while (traverse != NULL) {
		cur->count += find_subcompany(traverse);
		traverse = traverse->next;
	}
	return cur->count;
}

int main() {
	scanf("%d%d%d", &N, &M, &C);

	heap *prices = (heap *) malloc((N+1)*sizeof(heap));
	for (int i = 1; i <= N; i++) {
		prices[i].A = (node *) malloc((M+1)*sizeof(node));
		prices[i].company_indx = i;
		prices[i].tail = 0;
		prices[i].A[0].val = INT_MIN;
		prices[i].A[0].expiry = 0;
	}

	treenode *tree = (treenode *) malloc((N+1)*sizeof(treenode));
	tree[1].parent = -1;
	for (int i = 1; i <= N; i++) {
		tree[i].indx = i;
		tree[i].count = 1;
		tree[i].next = NULL;
		tree[i].child = NULL;
	}

	// Company is 1-indexed, Date is 0-indexed
	int in; // parent company
	treenode *cur = NULL;
	for (int i = 2; i <= N; i++) {
		readInt(&in);
		//scanf("%d", &in);
		tree[i].parent = in;
		if (tree[in].child == NULL) {
			tree[in].child = &tree[i];
		} else {
			cur = tree[in].child;
			while (cur->next != NULL) cur = cur->next;
			cur->next = &tree[i];
		}
	}

	// find subcompanies count
	find_subcompany(&tree[1]);

	int c, d; // price and expiry date
	for (int j = 0; j < M; j++) { // j == date
		for (int i = 1; i <= N; i++) {
			readInt(&c);
			readInt(&d);
			//scanf("%d%d", &c, &d);
			insert(&prices[i], c, j+d);
		}
		max = 0;
		best_offer(prices, &tree[1]);
		printf("%d", max);
		putchar('\n');
		for (int i = 1; i <= N; i++) filter(&prices[i], j);
	}
}
