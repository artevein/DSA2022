#include <stdio.h>
#include <stdlib.h>

//https://cp-algorithms.com/string/string-hashing.html#calculation-of-the-hash-of-a-string
typedef struct node {
	struct node *next;
	int offset; // the first character index (offset)
} node;

typedef struct ll {
	int len;
	node *head;
	node *tail;
} ll;

int N, M;
const int p = 31, K = 1000007;
char spell[1000001], pattern[1000001];
ll pattern_hash[1000007];

int hash(char *str, int len) {
	long long int cur = 0;
	for (int i = 0; i < len; i++) {
		cur = (cur*p + (str[i] - 'A' + 1)) % K;
		//printf("cur = %d\n", cur);
	}
	//printf("returning %lld\n", cur);
	return cur;
}

void insert(ll *list, int offset) {
	node *tail = list->tail;
	node *new = malloc(sizeof(node));
	new->offset = offset;
	new->next = NULL;
	if (tail == NULL) {
		list->head = new;
	} else {
		tail->next = new;
	}
	list->tail = new;
	list->len += 1;
}

void print(ll *list) {
	for (int i = 0; i < K; i++) {
		if (list[i].len <= 0) continue;
		printf("%d:", i);
		node *cur = list[i].head;
		for (int j = 0; j < list[i].len; j++) {
			int start = cur->offset;
			//for (int z = 0; z < M; z++) {
			//	
			//}
			printf(" %d", cur->offset);
			cur = cur->next;
		}
		putchar('\n');
	}
}

int match(ll *list, int hash, int spell_offset) {
	if (list[hash].len <= 0) return 0;
	//printf("Current hash = %d, Spell Offset = %d\n", hash, spell_offset);
	short found = 0;
	node *cur = list[hash].head;
	while (!found && cur) {
		// match cur
		short matched = 1;
		//printf("Spell: ");
		for (int i = 0; i < M; i++) {
			//printf("%c", spell[spell_offset+i]);
			if (spell[spell_offset+i] != pattern[(cur->offset+i)%M]) {
				matched = 0;
				break;
			}
		}
		//printf("\nPattern: ");
		//for (int i = 0; i < M; i++) {
		//	printf("%c", pattern[(cur->offset+i)%M]);
		//}
		//putchar('\n');
		if (matched) found = 1;
		cur = cur->next;
	}
	return found;
}

int main() {
	scanf("%d%d", &N, &M);
	scanf("%s", &spell);
	scanf("%s", &pattern);

	for (int i = 0; i < 1000007; i++) {
		pattern_hash[i].len = 0;
		pattern_hash[i].head = NULL;
	}

	long long int cur = hash(pattern, M);
	insert(&pattern_hash[cur], 0);

	int p_pow = 1;
	for (int i = 1; i < M; i++) p_pow = (p_pow * p) % K;
	//printf("p_pow = %d\n", p_pow);

	int num = 0;
	for (int i = 1; i < M; i++) {
		num = pattern[i-1]-'A'+1;
		cur = (cur * p + K - ((num*p_pow*p) % K) + num) % K;
		//printf("cur = %d\n", cur);
		insert(&pattern_hash[cur], i);
	}

	//print(pattern_hash);

	cur = hash(spell, M);
	int count = 0;
	count += match(pattern_hash, cur, 0);

	int num2 = 0;
	for (int i = 1; i < N-M+1; i++) {
		num = spell[i-1]-'A'+1;
		num2 = spell[i-1+M]-'A'+1;
		cur = (cur * p + K - ((num*p_pow*p) % K) + num2) % K;
		count += match(pattern_hash, cur, i);
	}
	printf("%d", count);
}
