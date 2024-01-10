#include <stdio.h>
#include <stdlib.h>

#define RED 0
#define BLACK 1

typedef struct rb_node {
    int data;
	int sum1;
	int sum2;
	int cnt;
    int color;
    struct rb_node *parent, *left, *right;
} Node;

int N, M;
long long int min_cost = 0;
int sweets[100000];
Node *NIL = NULL;

Node* create_node(int value) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    new_node->data = value;
    new_node->color = RED;
	new_node->sum1 = 0;
	new_node->sum2 = 0;
	new_node->cnt = 1;
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
	y->cnt = y->left->cnt + y->right->cnt + 1;

	if (y->left->cnt % 2 == 0) {
		y->sum1 = y->data + y->left->sum1 + y->right->sum2;
		y->sum2 = y->left->sum2 + y->right->sum1;
	} else {
		y->sum1 = y->left->sum1 + y->right->sum1;
		y->sum2 = y->data + y->left->sum2 + y->right->sum2;
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

void insert(Node** root, int value) {
    Node* new_node = create_node(value);

    *root = bst_insert(*root, new_node);

    fix_violation(root, new_node);

	add_sum(new_node);
	while (new_node != *root) {
		new_node = new_node->parent;
		add_sum(new_node);
	}
}

void inorder(Node* root) {
    if (root == NIL)
        return;

    inorder(root->left);
	printf("%d: sum1 = %d, sum2= %d, cnt = %d\n", root->data, root->sum1, root->sum2, root->cnt);
    inorder(root->right);
}
void print2DUtil(Node* root, int space) {
    if (root == NIL)
        return;
    space += 10;
    print2DUtil(root->right, space);
    //printf("\n");
    for (int i = 10; i < space; i++)
        printf(" ");
    printf("%d\n", root->data);
    print2DUtil(root->left, space);
}
 
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

	scanf("%d%d", &N, &M);
	int a, b;
	for (int i = 0; i < N; i++) {
		scanf("%d%d", &a, &b);
		if (a > b) {
			insert(&root, a-b);
			min_cost += b;
		} else if (b > a) {
			insert(&root, b-a);
			min_cost += a;
		} else {
			// 0
			min_cost += a;
		}
	}
	printf("%lld\n", min_cost+root->sum1);
    return 0;
}
