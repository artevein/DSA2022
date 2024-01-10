#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>

int cmp(const void *a, const void *b);
int cmp_global(const void *a, const void *b);
struct global_cat;

typedef struct same_color_cat { // circular arr content
	int color; // INDEX
	int appetite;
	struct global_cat *to_global;
} same_color_cat;

typedef struct global_cat { // LL
	int index;
	int color; // INDEX
	int appetite;
	struct global_cat *prev, *next;
	struct same_color_cat *to_color;
} global_cat;

typedef struct same_color { // circular arr
	int color; // VALUE
	int count;
	int head, tail;
	struct same_color_cat *arr;
} same_color;


global_cat *head = NULL, *tail = NULL;
same_color *map = NULL;
int color_count = 0;

same_color *get_color(int c) {
	int l = 0, r = color_count-1;
	int m = 0;
	while (l <= r) {
		m = l + (r-l)/2;
		if (c == map[m].color) return &map[m];
		else if (c > map[m].color) l = m + 1;
		else r = m - 1;
	}
	return NULL;
}

int in_range(same_color *c, int l, int r) {
	if (c == NULL) return 0;
	int lower_index = INT_MAX;
	int count = c->count;
	int h = c->head, t = c->tail >= h ? c->tail : c->tail+count;
	if (l <= c->arr[h%count].appetite) lower_index = h;
	else if (l > c->arr[t%count].appetite) return 0; 
	else if (l == c->arr[t%count].appetite) return 1;
	else {
		int sl = h, sr = t, m = 0;
		while (sl + 1 < sr) {
			m = sl + (sr-sl)/2;
			if (l == c->arr[m % count].appetite) {
				lower_index = m;
				break;
			} else if (l > c->arr[m % count].appetite) {
				sl = m + 1;
			} else {
				sr = m;
			}
		}
		if (lower_index == INT_MAX) {
			if (l <= c->arr[sl % count].appetite) lower_index = sl;
			else lower_index = sr;
		}
	}
	int upper_index = INT_MIN;
	if (r >= c->arr[t%count].appetite) upper_index = t;
	else if (r < c->arr[h%count].appetite) return 0;
	else if (r == c->arr[h%count].appetite) return 1;
	else {
		int sl = h, sr = t, m = 0;
		while (sl + 1 < sr) {
			m = sl + (sr-sl)/2;
			if (r == c->arr[m % count].appetite) {
				upper_index = m;
				break;
			} else if (r > c->arr[m % count].appetite) {
				sl = m;
			} else {
				sr = m - 1;
			}
		}
		if (upper_index == INT_MIN) {
			if (r >= c->arr[sr % count].appetite) upper_index = sr;
			else upper_index = sl;
		}
	}
	//printf("lower = %d, upper = %d\n", c->arr[upper_index], c->arr[lower_index]);
	return upper_index - lower_index + 1;
}

void swap(global_cat *cur) {
	//if (cur->next) {
	//	printf("next = %d\n", cur->next->to_color->appetite);
	//}
	//if (cur->prev) {
	//	printf("prev = %d\n", cur->prev->to_color->appetite);
	//}
	global_cat *next = cur->next;
	global_cat *prev = cur->prev;
	//assert(next != NULL); // 
	//if (next == NULL) return;
	global_cat *temp = next->next;
	if (prev != NULL) prev->next = next;
	next->prev = prev;
	next->next = cur;
	cur->prev = next;
	cur->next = temp;
	if (temp != NULL) temp->prev = cur;
	//printf(" cur = %d, next = %d\n", cur->appetite, next->appetite);
	if (cur == head) head = next;
	if (next == tail) tail = cur;
	// appetite on global cat was not changed because we dont need it
	if (next->color != cur->color) { // then we dont need to swap in the sorted arr
		int atemp = cur->to_color->appetite;
		//printf("1 Swapping %d with %d\n", next->to_color->appetite, cur->to_color->appetite);
		cur->to_color->appetite = next->to_color->appetite;
		next->to_color->appetite = atemp;
	} else {
		same_color_cat *atemp = cur->to_color;
		//printf("2 Swapping %d with %d\n", next->to_color->appetite, cur->to_color->appetite);
		cur->to_color = next->to_color;
		next->to_color = atemp;
		cur->to_color->to_global = cur;
		next->to_color->to_global = next;
	}
}

void print() {
	printf("---------------------\n");
	printf("HEAD = %d | TAIL = %d\n", head->to_color->appetite, tail->to_color->appetite);
	global_cat *cur = head;
	global_cat *bcur = tail;
	while (cur) {
		printf("%d ", cur->to_color->appetite);
		cur = cur->next;
	}
	putchar('\n');
	while (bcur) {
		printf("%d ", bcur->to_color->appetite);
		bcur = bcur->prev;
	}
	putchar('\n');
	for (int i = 0; i < color_count; i++) {
		printf("%d (h = %d, l = %d):", map[i].color, map[i].head, map[i].tail);
		for (int j = 0; j < map[i].count; j++) {
			printf(" %d", map[i].arr[j].appetite);
		}
		putchar('\n');
	}
	printf("---------------------\n");
}

void magic(same_color *c, int dir, int success) {
	// edge case: smallest to smallest, highest etc.
	int count = c->count;
	int local_tail_indx = c->tail % count;
	int local_head_indx = c->head % count;
	same_color_cat *local_tail = &c->arr[local_tail_indx];
	same_color_cat *local_head = &c->arr[local_head_indx];
	global_cat *g_tail = local_tail->to_global;
	global_cat *g_head = local_head->to_global;
	//printf("HEAD = %d, TAIL = %d\n", head->to_color->appetite, tail->to_color->appetite);
	//printf("SUBJECT high = %d, low = %d\n", local_head->appetite, local_tail->appetite);
	if (dir) {
		if (success) {
			//printf("highest S\n");
			local_tail->appetite = tail->to_color->appetite + 1;
			if (g_tail == tail) return;
			if (g_tail == head && g_tail->next) head = g_tail->next;
			tail->next = g_tail;
			if (g_tail->prev) g_tail->prev->next = g_tail->next;
			if (g_tail->next) g_tail->next->prev = g_tail->prev;
			g_tail->next = NULL;
			g_tail->prev = tail;
			tail = g_tail;
		} else {
			//printf("highest F\n");
			local_tail->appetite = head->to_color->appetite - 1;
			if (g_tail == tail && g_tail->prev) tail = g_tail->prev;
			if (g_tail == head) return;
			head->prev = g_tail;
			if (g_tail->prev) g_tail->prev->next = g_tail->next;
			if (g_tail->next) g_tail->next->prev = g_tail->prev;
			g_tail->next = head;
			g_tail->prev = NULL;
			head = g_tail;
			c->head = (c->head+count-1) % count;
			c->tail = (c->tail+count-1) % count;
		}
	} else {
		if (!success) {
			//printf("lowest F\n");
			local_head->appetite = head->to_color->appetite - 1;
			if (g_head == head) return;
			if (g_head == tail && g_head->prev) tail = g_head->prev;
			head->prev = g_head;
			if (g_head->prev) g_head->prev->next = g_head->next;
			if (g_head->next) g_head->next->prev = g_head->prev;
			g_head->next = head;
			g_head->prev = NULL;
			head = g_head;
		} else {
			//printf("lowest S\n");
			local_head->appetite = tail->to_color->appetite + 1;
			//printf("local updated to %d\n", local_head->appetite);
			if (g_head == tail) return;
			if (g_head == head && g_head->next) head = g_head->next;
			tail->next = g_head;
			if (g_head->prev) g_head->prev->next = g_head->next;
			if (g_head->next) g_head->next->prev = g_head->prev;
			g_head->next = NULL;
			g_head->prev = tail;
			tail = g_head;
			c->head = (c->head+1);
			c->tail = (c->tail+1);
		}
	}
}

int main() {
	int N, M;
	scanf("%d%d", &N, &M);
	global_cat *global_arr = malloc(N*sizeof(global_cat));
	for (int i = 0; i < N; i++) {
		global_arr[i].index = i;
		scanf("%d", &global_arr[i].appetite);
	}
	for (int i = 0; i < N; i++) {
		scanf("%d", &global_arr[i].color);
	}
	qsort(global_arr, N, sizeof(global_cat), cmp);
	global_cat **index_to_global = malloc(N*sizeof(global_cat *)); // i -> obj cat
	map = malloc(N*sizeof(same_color)); // arr of circular arrs
	// make map
	int count = 1;
	for (int i = 0; i < N; i++) {
		if (i == N-1 || global_arr[i].color != global_arr[i+1].color) {
			map[color_count].arr = malloc(count*sizeof(same_color_cat));
			map[color_count].head = 0;
			map[color_count].tail = count-1;
			map[color_count].count = count;
			map[color_count].color = global_arr[i].color;
			color_count++;
			count = 1;
		} else count++;
	}
	// insert into map
	int x = 0;
	for (int i = 0; i < color_count; i++) {
		for (int j = 0; j < map[i].count; j++) {
			global_arr[x].color = i; // from color value to color index
			global_arr[x].to_color = &map[i].arr[j];
			map[i].arr[j].color = i;
			map[i].arr[j].appetite = global_arr[x].appetite;
			x++;
		}
	}
	qsort(global_arr, N, sizeof(global_cat), cmp_global);
	// build LL
	global_cat *prev = NULL;
	for (int i = 0; i < N; i++) {
		if (prev != NULL) prev->next = &global_arr[i];
		global_arr[i].prev = prev;
		prev = &global_arr[i];
		index_to_global[global_arr[i].index] = &global_arr[i];
		global_arr[i].to_color->to_global = &global_arr[i];
	}
	if (prev != NULL) prev->next = NULL;
	head = &global_arr[0];
	tail = prev;

	int step, c, lower, upper;
	int indx;
	int s, t;
	//print();
	for (int i = 0; i < M; i++) {
		scanf("%d", &step);
		if (step == 1) {
			scanf("%d%d%d", &c, &lower, &upper);
			printf("%d\n", in_range(get_color(c), lower, upper));
		} else if (step == 2) {
			scanf("%d", &indx);	
			swap(index_to_global[indx]);
			//print();
		} else {
			scanf("%d%d%d", &c, &s, &t);
			magic(get_color(c), s, t);
			//print();
		}
	}
}

int cmp(const void *a, const void *b) {
	global_cat *ia = (global_cat *) a;
	global_cat *ib = (global_cat *) b;
	if (ia->color == ib->color) return (ia->appetite - ib->appetite);
	return (ia->color - ib->color);
}

int cmp_global(const void *a, const void *b) {
	global_cat *ia = (global_cat *) a;
	global_cat *ib = (global_cat *) b;
	return (ia->appetite - ib->appetite);
}
