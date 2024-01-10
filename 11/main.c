#include <stdio.h>
#include <stdlib.h>

#define RED 0
#define BLACK 1
int ReadInt(int *x) {
    char c;
    while((c = getchar()) < '-')    {if(c == EOF) return 0;}
    *x = c-'0';
    while((c = getchar()) >= '0')
        *x = (*x << 3) + (*x << 1) + c-'0';
    return 1;
}

int int_to_buffer(char *buffer, long long int num) {
	char aaa[15];
	int i = 0;
	long long int t = num;
	while (t > 0) {
		aaa[i++] = t%10 +'0';
		t /= 10;
	}
	for (int j = i-1; j >= 0; j--) {
		buffer[i-1-j] = aaa[j];
	}
	return i;
}


typedef struct rb_node {
    int data;
	long long int sum1;
	long long int sum2;
	int cnt;
    int color;
	int disabled;
    struct rb_node *parent, *left, *right;
} Node;

int N, M;
long long int min_cost = 0;
Node *sweets[100000];
int prev_min[100000];
Node *NIL = NULL;

Node* create_node(int value) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    new_node->data = value;
    new_node->color = RED;
	new_node->sum1 = 0;
	new_node->sum2 = 0;
	new_node->cnt = 1;
	new_node->disabled = 0;
    new_node->parent = new_node->left = new_node->right = NIL;
    return new_node;
}

Node* bst_insert(Node* root, Node* newNode) {
    if (root == NIL)
        return newNode;

    if (newNode->data <= root->data) {
        root->left = bst_insert(root->left, newNode);
        root->left->parent = root;
    } else if (newNode->data > root->data) {
        root->right = bst_insert(root->right, newNode);
        root->right->parent = root;
    }

    return root;
}

void add_sum(Node *y) {
	if (y->disabled) {
		y->cnt = y->left->cnt + y->right->cnt;
		if (y->left->cnt % 2 == 0) {
			y->sum1 = y->left->sum1 + y->right->sum1;
			y->sum2 = y->left->sum2 + y->right->sum2;
		} else {
			y->sum1 = y->left->sum1 + y->right->sum2;
			y->sum2 = y->left->sum2 + y->right->sum1;
		}
		return;
	}
	y->cnt = y->left->cnt + y->right->cnt + 1;

	if (y->left->cnt % 2 == 0) {
		y->sum1 = y->data + y->left->sum1 + y->right->sum2;
		y->sum2 = y->left->sum2 + y->right->sum1;
	} else {
		y->sum1 = y->left->sum1 + y->right->sum1;
		y->sum2 = y->data + y->left->sum2 + y->right->sum2;
	}
}

void update(Node *root, Node *x) {
	if (x == NIL) return;

	add_sum(x);
	while (x != root) {
		x = x->parent;
		add_sum(x);
	}
}

void rotate_left(Node** root, Node* x) {
    Node* y = x->right;
    x->right = y->left;
    if (y->left != NIL) y->left->parent = x;
    y->parent = x->parent;
    if (x->parent == NIL) *root = y;
    else if (x == x->parent->left) x->parent->left = y;
    else x->parent->right = y;
    y->left = x;
    x->parent = y;
	add_sum(x);
	add_sum(y);
}

void rotate_right(Node** root, Node* x) {
    Node* y = x->left;
    x->left = y->right;
    if (y->right != NIL) y->right->parent = x;
    y->parent = x->parent;
    if (x->parent == NIL) *root = y;
    else if (x == x->parent->left) x->parent->left = y;
    else x->parent->right = y;
    y->right = x;
    x->parent = y;
	add_sum(x);
	add_sum(y);
}

void fix_violation(Node** root, Node* newNode) {
    Node* parent = NIL;
    Node* grandparent = NIL;

    while ((newNode != *root) && (newNode->color != BLACK) &&
           (newNode->parent->color == RED)) {

        parent = newNode->parent;
        grandparent = newNode->parent->parent;

        if (parent == grandparent->left) {
            Node* uncle = grandparent->right;

            if (uncle != NIL && uncle->color == RED) {
                grandparent->color = RED;
                parent->color = BLACK;
                uncle->color = BLACK;
                newNode = grandparent;
            } else {
                if (newNode == parent->right) {
                    rotate_left(root, parent);
                    newNode = parent;
                    parent = newNode->parent;
                }

                rotate_right(root, grandparent);
                int temp = parent->color;
                parent->color = grandparent->color;
                grandparent->color = temp;
                newNode = parent;
            }
        } else {
            Node* uncle = grandparent->left;

            if ((uncle != NIL) && (uncle->color == RED)) {
                grandparent->color = RED;
                parent->color = BLACK;
                uncle->color = BLACK;
                newNode = grandparent;
            } else {
                if (newNode == parent->left) {
                    rotate_right(root, parent);
                    newNode = parent;
                    parent = newNode->parent;
                }

                rotate_left(root, grandparent);
                int temp = parent->color;
                parent->color = grandparent->color;
                grandparent->color = temp;
                newNode = parent;
            }
        }
    }

    (*root)->color = BLACK;
}

void insert(Node** root, Node *new_node) {

    *root = bst_insert(*root, new_node);

    fix_violation(root, new_node);

	update(*root, new_node);
}
 
char buffer[3000000];
int main() {
	NIL = (Node *) malloc(sizeof(Node));
    Node* root = NIL;
	NIL->color = BLACK;
	NIL->left = NULL;
	NIL->right = NULL;
	NIL->cnt = 0;
	NIL->sum1 = 0;
	NIL->sum2 = 0;
	NIL->data = 0;
	NIL->disabled = 0;

	ReadInt(&N);
	ReadInt(&M);
	int a, b;
	for (int i = 0; i < N; i++) {
		ReadInt(&a);
		ReadInt(&b);
		if (a > b) {
			sweets[i] = create_node(a-b);
			insert(&root, sweets[i]);
			prev_min[i] = b;
			min_cost += b;
		} else if (b > a) {
			sweets[i] = create_node(b-a);
			insert(&root, sweets[i]);
			prev_min[i] = a;
			min_cost += a;
		} else {
			sweets[i] = NIL;
			prev_min[i] = a;
			min_cost += a;
		}
	}
	long long int prev;
	if (root->cnt%2) prev = min_cost+root->sum2;
	else prev = min_cost+root->sum1;
	int buffer_head = int_to_buffer(&buffer[0], prev);
	buffer[buffer_head++] = '\n';
	//printf("%lld\n", prev);

	int t, c, d, e, f;
	long int mod = 1000000007;
	for (int i = 0; i < M-1; i++) {
		ReadInt(&t);
		ReadInt(&c);
		ReadInt(&d);
		ReadInt(&e);
		ReadInt(&f);
		//scanf("%d%d%d%d%d", &t, &c, &d, &e, &f);
		t--;
		sweets[t]->disabled = 1;
		Node *n = sweets[t];
		update(root, n);
		long long int a2 = (c*(prev%mod) + d) % mod; 
		long long int b2 = (e*(prev%mod) + f) % mod; 
		if (a2 > b2) {
			sweets[t] = create_node(a2-b2);
			insert(&root, sweets[t]);
			min_cost += b2 - prev_min[t];
			prev_min[t] = b2;
		} else if (b2 > a2) {
			sweets[t] = create_node(b2-a2);
			insert(&root, sweets[t]);
			min_cost += a2 - prev_min[t];
			prev_min[t] = a2;
		} else {
			sweets[t] = NIL;
			min_cost += a2 - prev_min[t];
			prev_min[t] = a2;
		}

		if (root->cnt%2) prev = min_cost+root->sum2;
		else prev = min_cost+root->sum1;
		buffer_head += int_to_buffer(&buffer[buffer_head], prev);
		buffer[buffer_head++] = '\n';
		//printf("%lld\n", prev);
	}
	fwrite(buffer, sizeof(char), buffer_head, stdout);
    return 0;
}
