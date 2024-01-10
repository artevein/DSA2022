#include <stdio.h>
#include <stdlib.h>

typedef struct heap {
	int capacity;
	int tail;
	int *A;
} heap;

typedef struct node {
	struct node *root;
	int indx;
	int hp;
	long long int atk;
	int real_atk;
	long long int diff;
	long long int dmg_taken;
	int compression;
} node;

int N, M;
int res[200001];
int dead[200001];
int dead_count[200001];
node knights[200001];
heap *health = NULL;

int hasEOF = 0;
int readchar() {
    static int N = 1<<20;
    static char buf[1<<20];
    static char *p = buf, *end = buf;
    if(p == end) {
        if((end = buf + fread(buf, 1, N, stdin)) == buf) {
            hasEOF = 1;
            return EOF;   
        }
        p = buf;
    }
    return *p++;
}
int ReadInt(int *x) {
    char c;
    while((c = readchar()) < '-')    {if(c == EOF) return 0;}
    *x = c-'0';
    while((c = readchar()) >= '0')
        *x = (*x << 3) + (*x << 1) + c-'0';
    return 1;
}

node *find(node *A) {
	if (!A->root) return A;
	if (!A->root->root) return A->root;
	A->compression += A->root->compression;
	node *rootA = find(A->root);
	A->dmg_taken += A->root->dmg_taken; // check
	A->root = rootA;
	return A->root;
}

int count(node *A) {
	if (dead[A->indx]) return dead_count[A->indx];
	node *root = find(A);
	if (root == A) return A->compression;
	return A->compression + root->compression;
}

long long int get_hp(int indx) {
	node *A = &knights[indx];
	node *root = find(A);
	long long int hp_left = A->hp - A->dmg_taken;
	if (A == root) return hp_left;
	hp_left -= A->root->dmg_taken - A->diff;
	return hp_left;
}

void pop_min(heap *h) {
	h->A[1] = h->A[h->tail--];

	int cur = 1, smallest = 100000;
	while (cur*2 <= h->tail) {
		if (get_hp(h->A[cur]) < get_hp(h->A[cur*2])) smallest = cur;
		else smallest = cur*2;
		if (cur*2+1 <= h->tail && get_hp(h->A[cur*2+1]) < get_hp(h->A[smallest])) smallest = cur*2+1;
		if (cur == smallest) break;
		int temp = h->A[cur];
		h->A[cur] = h->A[smallest];
		h->A[smallest] = temp;
		cur = smallest;
	}
}

void remove_dead(heap *h) {
	while (h->tail >= 1 && get_hp(h->A[1]) <= 0) {
		node *root = find(&knights[h->A[1]]);
		dead_count[h->A[1]] = count(&knights[h->A[1]]);
		dead[h->A[1]] = 1;
		root->atk -= knights[h->A[1]].real_atk;
		pop_min(h);
	}
}

void insert(heap *h, int indx) {
	h->tail++;
	h->A[h->tail] = indx;
	int cur = h->tail;
	int next = cur/2;
	int tindx = indx;
	long long int a = get_hp(h->A[cur]), b = 0;
	while (next != 0) {
		b = get_hp(h->A[next]);
		if (a >= b) return;
		tindx = h->A[cur];
		h->A[cur] = h->A[next];
		h->A[next] = tindx;
		cur = next;
		next = next/2;
		a = b;
	}
}

void merge_heap(heap *small, heap *large, int diff) {
	if (small->tail + large->tail + 1 > large->capacity) {
		large->capacity *= 2;
		large->A = realloc(large->A, large->capacity*sizeof(int));
	}
	for (int i = 1; i <= small->tail; i++) {
		knights[small->A[i]].diff += diff;
		insert(large, small->A[i]);
	}
	free(small->A);
}

void do_union(node *B, node *C) {
	node *rootC = find(C);
	node *rootB = find(B);
	if (rootC == rootB) return;
	if (health[rootB->indx].tail <= health[rootC->indx].tail) {
		rootB->root = rootC;
		rootC->dmg_taken += rootB->atk;
		rootC->atk += rootB->atk;
		rootB->compression += 1 - rootC->compression;
		merge_heap(&health[rootB->indx], &health[rootC->indx], rootC->dmg_taken);
		remove_dead(&health[rootC->indx]);
	} else { // defender is smaller
		rootC->root = rootB;
		rootC->dmg_taken += rootB->atk;
		rootB->atk += rootC->atk;
		rootB->compression += 1;
		rootC->compression -= rootB->compression;
		merge_heap(&health[rootC->indx], &health[rootB->indx], rootB->dmg_taken);
		remove_dead(&health[rootB->indx]);
	}
}

int main() {
	scanf("%d%d", &N, &M);
	health = malloc((N+1)*sizeof(heap));
	for (int i = 1; i <= N; i++) {
		health[i].A = malloc(20*sizeof(int));
		health[i].A[1] = i;
		health[i].tail = 1;
		health[i].capacity = 20; // includes 0
	}
	for (int i = 0; i <= N; i++) {
		knights[i].indx = i;
		knights[i].diff = 0;
		knights[i].dmg_taken = 0;
		knights[i].compression = 0;
		knights[i].root = NULL;
	}
	for (int i = 1; i <= N; i++) ReadInt(&knights[i].hp);
	for (int i = 1; i <= N; i++) {
		ReadInt(&knights[i].real_atk);
		knights[i].atk = knights[i].real_atk;
	}
	int x, y;
	for	(int i = 0; i < M; i++) {
		ReadInt(&x);
		ReadInt(&y);
		if (dead[x] || dead[y]) continue;
		do_union(&knights[x], &knights[y]);
	}
	printf("%d", count(&knights[1]));
	for (int i = 2; i <= N; i++) {
		printf(" %d", count(&knights[i]));
	}
}
