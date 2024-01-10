#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define UP 0
#define LEFT 1
#define DOWN 2
#define RIGHT 3

int int_to_buffer(char *buffer, int num) {
	if (num == 0) {
		buffer[0] = '0';
		return 1;
	}
	char aaa[10];
	int i = 0;
	int t = num;
	while (t > 0) {
		aaa[i++] = t%10 +'0';
		t /= 10;
	}
	for (int j = i-1; j >= 0; j--) {
		buffer[i-1-j] = aaa[j];
	}
	return i;
}

int ReadInt(int *num) {
    char c, negative;
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

typedef struct Bakery {
	int rank;
	int toast;
	struct Bakery *next[4]; // [UP, LEFT, DOWN, RIGHT]
	struct Bakery *anext[4]; // [UP, LEFT, DOWN, RIGHT]
	struct Bakery *bnext[4]; // [UP, LEFT, DOWN, RIGHT]
	int d[4]; // [UP, LEFT, DOWN, RIGHT]
	int bd[4]; // [UP, LEFT, DOWN, RIGHT]
	struct Bakery *other;
} bakery;

int n, m;
bakery *ranker[1000001]; // can be optimized via malloc
bakery *head = NULL;

void bike(bakery *cur, int len, int count) {
	for (int i = 0; i < 4; i++) {
		if (cur->anext[i]) {
			cur->anext[i]->anext[(i+2)%4] = cur->anext[(i+2)%4];
			cur->anext[i]->d[(i+2)%4] += cur->d[(i+2)%4];
		}
	}
	if (len <= 0) {
		for (int i = 0; i < 4; i++) {
			cur->anext[i] = cur->bnext[i];
			cur->d[i] = cur->bd[i];
			if (cur->anext[i]) {
				cur->anext[i]->anext[(i+2)%4] = cur->anext[i]->bnext[(i+2)%4];
				cur->anext[i]->d[(i+2)%4] = cur->anext[i]->bd[(i+2)%4];
			}
		}
		return;
	}
	
	int min_rank = INT_MAX, min_distance = INT_MAX;
	bakery *found = NULL;
	bakery *select[4] = {cur->anext[UP], cur->anext[LEFT], cur->anext[DOWN], cur->anext[RIGHT]};
	for (int i = 0; i < 4; i++) {
		if (select[i] != NULL && (min_distance > cur->d[i] || min_distance == cur->d[i] && min_rank > select[i]->rank)) {
			found = select[i];
			min_distance = cur->d[i];
			min_rank = select[i]->rank;
		}
	}
	if (found) {
		found->toast -= count >= found->toast ? found->toast : count;
		found->other->toast -= count >= found->other->toast ? found->other->toast : count;
		if (found->toast <= 0) {
			for (int i = 0; i < 4; i++) {
				if (found->bnext[i]) {
					found->bnext[i]->bnext[(i+2)%4] = found->bnext[(i+2)%4];
					found->bnext[i]->bd[(i+2)%4] += found->bd[(i+2)%4];
				}
			}
		}
		bike(found, len-1, count);
	}
	for (int i = 0; i < 4; i++) {
		cur->anext[i] = cur->bnext[i];
		cur->d[i] = cur->bd[i];
		if (cur->anext[i]) {
			cur->anext[i]->anext[(i+2)%4] = cur->anext[i]->bnext[(i+2)%4];
			cur->anext[i]->d[(i+2)%4] = cur->anext[i]->bd[(i+2)%4];
		}
	}
}

void rotate(bakery *mid, int len) {
	if (len <= 1) return;
	bakery *ltemp = NULL, *rtemp = NULL;
	bakery *lcorner = mid, *rcorner = mid;
	bakery *lnext = NULL, *rnext = NULL;
	int LLEFT = 0, RRIGHT = 0;
	for (int i = 0; i < len/2; i++) {
		lcorner = lcorner->next[LEFT];
		rcorner = rcorner->next[RIGHT];
		lcorner = lcorner->next[UP];
		rcorner = rcorner->next[DOWN];
	}
	if (lcorner == head) head = rcorner->other;
	else if (lcorner->other == head) head = rcorner;
	else if (rcorner == head) head = lcorner->other;
	else if (rcorner->other == head) head = lcorner;
	for (int d = 0; d < 2; d++) { // UP LEFT DOWN RIGHT
			LLEFT = (LEFT+d)%4;
			RRIGHT = (RIGHT+d)%4;
		for (int i = 0; i < len; i++) {
			lnext = lcorner->next[(DOWN+d)%4];
			rnext = rcorner->next[(UP+d)%4];
			if (lcorner->other->next[RRIGHT])
				lcorner->other->next[RRIGHT]->next[LLEFT] = rcorner;
			if (rcorner->other->next[LLEFT])
				rcorner->other->next[LLEFT]->next[RRIGHT] = lcorner;
			if (lcorner->next[LLEFT])
				lcorner->next[LLEFT]->next[RRIGHT] = rcorner->other;
			if (rcorner->next[RRIGHT])
				rcorner->next[RRIGHT]->next[LLEFT] = lcorner->other;
			ltemp = lcorner->next[LLEFT];
			rtemp = rcorner->next[RRIGHT];
			lcorner->next[LLEFT] = rcorner->other->next[LLEFT];
			rcorner->next[RRIGHT] = lcorner->other->next[RRIGHT];
			lcorner->other->next[RRIGHT] = rtemp;
			rcorner->other->next[LLEFT] = ltemp;
			
			if (i == len-1) continue;
			lcorner = lnext;
			rcorner = rnext;
		}
	}
}

int main() {
	scanf("%d%d", &n, &m);
	char *buffer = malloc(26*n*m*sizeof(char));
	bakery *prev = NULL, *cur = NULL;
	bakery *prev_ura = NULL, *ura = NULL;
	bakery *prev_row[1000];
	bakery *prev_ura_row[1000];
	for (int i = 0; i < n; i++) { 
		prev = NULL;
		for (int j = 0; j < m; j++) {
			cur = (bakery *) malloc(sizeof(bakery));
			ura = (bakery *) malloc(sizeof(bakery));
			if (i + j == 0) head = cur;
			ura->next[RIGHT] = prev_ura;
			if (prev != NULL) {
				prev->next[RIGHT] = cur; 
				prev->anext[RIGHT] = cur;
				prev->bnext[RIGHT] = cur;
			}
			for (int dir = 0; dir < 4; dir++) {
				cur->d[dir] = 1;
				cur->bd[dir] = 1;
				ura->d[dir] = 1;
				ura->bd[dir] = 1;
			}
			if (prev_ura) prev_ura->next[LEFT] = ura;
			cur->next[LEFT] = prev;
			cur->anext[LEFT] = prev;
			cur->bnext[LEFT] = prev;
			ReadInt(&(cur->rank));
			//scanf("%d", &(cur->rank));
			ura->rank = cur->rank;
			ranker[cur->rank] = cur;
			if (i != 0) {
				cur->next[UP] = prev_row[j];
				cur->anext[UP] = prev_row[j];
				cur->bnext[UP] = prev_row[j];
				ura->next[DOWN] = prev_ura_row[j];
				prev_ura_row[j]->next[UP] = ura;
				prev_row[j]->next[DOWN] = cur;
				prev_row[j]->anext[DOWN] = cur;
				prev_row[j]->bnext[DOWN] = cur;
			} else {
				ura->next[DOWN] = NULL;
				cur->next[UP] = NULL;
				cur->anext[UP] = NULL;
				cur->bnext[UP] = NULL;
			}
			cur->other = ura;
			ura->other = cur;
			prev_row[j] = cur;
			prev_ura_row[j] = ura;
			prev = cur;
			prev_ura = ura;
		}
		ura->next[LEFT] = NULL;
		cur->next[RIGHT] = NULL;
		cur->anext[RIGHT] = NULL;
		cur->bnext[RIGHT] = NULL;
	}
	for (int j = 0; j < m; j++) {
		prev_row[j]->next[DOWN] = NULL;
		prev_ura_row[j]->next[UP] = NULL;
		prev_row[j]->anext[DOWN] = NULL;
		prev_row[j]->bnext[DOWN] = NULL;
	}


	bakery *tail = cur;
	bakery *traverse = head;
	bakery *head_row = head;
	for (int i = 0; i < n; i++) {
		traverse = head_row;
		for (int j = 0; j < m; j++) {
			ReadInt(&(traverse->toast));
			//scanf("%d", &(traverse->toast));
			traverse->other->toast = traverse->toast;
			traverse = traverse->next[RIGHT];
		}
		head_row = head_row->next[DOWN];
	}
	int T1, R, T2;
	scanf("%d%d%d", &T1, &R, &T2);
	int r, l, s;
	for (int i = 0; i < T1; i++) {
		ReadInt(&r);
		ReadInt(&l);
		ReadInt(&s);
		//scanf("%d%d%d", &r, &l, &s);
		bike(ranker[r], l, s);
	}
	for (int i = 0; i < R; i++) {
		ReadInt(&r);
		ReadInt(&l);
		//scanf("%d%d", &r, &l);
		rotate(ranker[r], l); 
	}
	head_row = head;
	for (int i = 0; i < n; i++) {
		traverse = head_row;
		for (int j = 0; j < m; j++) {
			for (int d = 0; d < 4; d++) {
				ranker[traverse->rank] = traverse;
				traverse->anext[d] = traverse->next[d];
				traverse->d[d] = 1;
				traverse->bnext[d] = traverse->next[d];
				traverse->bd[d] = 1;
			}
			traverse = traverse->next[RIGHT];
		}
		head_row = head_row->next[DOWN];
	}
	head_row = head;
	for (int i = 0; i < n; i++) {
		traverse = head_row;
		for (int j = 0; j < m; j++) {
			if (traverse->toast <= 0) {
				for (int d = 0; d < 4; d++) {
					if (traverse->anext[d]) {
						traverse->anext[d]->anext[(d+2)%4] = traverse->anext[(d+2)%4];
						traverse->anext[d]->d[(d+2)%4] += traverse->d[(d+2)%4];
						traverse->bnext[d]->bnext[(d+2)%4] = traverse->bnext[(d+2)%4];
						traverse->bnext[d]->bd[(d+2)%4] += traverse->bd[(d+2)%4];
					}
				}
			}
			traverse = traverse->next[RIGHT];
		}
		head_row = head_row->next[DOWN];
	}
	for (int i = 0; i < T2; i++) {
		ReadInt(&r);
		ReadInt(&l);
		ReadInt(&s);
		//scanf("%d%d%d", &r, &l, &s);
		bike(ranker[r], l, s);
	}
	int buffer_head = 0;
	head_row = head;
	for (int i = 0; i < n; i++) {
		traverse = head_row;
		for (int j = 0; j < m; j++) {
			buffer_head += int_to_buffer(&buffer[buffer_head], traverse->toast);
			if (j == m-1) buffer[buffer_head++] = '\n';
			else buffer[buffer_head++] = ' ';
			//printf("%d%c", traverse->toast, " \n"[j==m-1]);
			traverse = traverse->next[RIGHT];
		}
		head_row = head_row->next[DOWN];
	}
	fwrite(buffer, sizeof(char), buffer_head, stdout);
}
