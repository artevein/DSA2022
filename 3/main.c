#include <stdio.h>
#include <string.h>

void print(int m[]) {
	int i = 0;
	while (m[i] != -1) {
		i++;
	}
	for (int j = i-1; j >= 0; j--) printf("%d", m[j]);
	printf("\n");
}

// swaps array pointer
void swap(int **a, int **b) {
	int *temp = *a;
	*a = *b;
	*b = temp;
}

// a > b
int compare(int a[], int b[]) {
	int a_len = 0, b_len = 0;
	for (int i = 0; i < 257 && !(a_len && b_len); i++) {
		if (a[i] == -1) a_len = i;
		if (b[i] == -1) b_len = i;
	}
	if (a_len > b_len) return 1;
	if (b_len > a_len) return 0;
	for (int i = a_len-1; i >= 0; i--) {
		if (a[i] > b[i]) return 1;
		else if (b[i] > a[i]) return 0;
	}
	return -1;
}

// a - b
void subtract(int a[], int b[]) {
	// assuming a > b.
	int i;
	int carry = 0;
	for (i = 0; i < 257 && a[i] != -1; i++) {
		int sub = b[i] >= 0 ? b[i] : 0;
		a[i] -= carry;
		carry = 0;
		if (a[i] >= sub) a[i] -= sub;
		else {
			a[i] = 10+a[i]-sub;
			carry++;
		}
	}
	while (i > 0 && a[i] <= 0) {
		a[i--] = -1;
	}
}

void divide(int a[]) {
	int i = 0;
	while (a[i] != -1) {
		int left = a[i] % 2;
		if (i != 0 && left) a[i-1] += 5;
		a[i] /= 2;
		i++;
	}
	if (i != 0 && a[i-1] == 0) a[i-1] = -1;
}

void multi(int a[], int k) {
	for (int t = 0; t < k; t++) {
		int i = 0;
		int carry = 0;
		int cur = 0;
		while (a[i] != -1) {
			cur = (a[i] >= 5);
			a[i] = (a[i] * 2) % 10 + carry;
			carry = cur;
			i++;
		}
		if (carry) {
			a[i+1] = -1;
			a[i] = carry;
		}
	}
}

// a *= b
void product(int a[], int b[]) {
	int res[257] = {};
	int i = 0, jj = 0;
	for (; b[i] != -1; i++) {
		int j = 0;
		int carry = 0;
		for (; a[j] != -1; j++) {
			int p = a[j]*b[i]+res[j+i]+carry;
			carry = p / 10;
			res[j+i] = p % 10;
		}
		res[j+i] += carry;
		jj = j;
	}
	for (int i = 255; i >= 0; i--) {
		if (res[i] > 0) {
			res[i+1] = -1;
			break;
		}
	}
	print(res);
}

int main() {
	int mr[257] = {}; // -1 signifies the end
	int nr[257] = {};
	char a[257], b[257];
	scanf("%s%s", &a, &b);
	for (int i = 0; i < strlen(a); i++) {
		mr[i] = a[strlen(a)-1-i] - '0';
	}
	mr[strlen(a)] = -1;
	for (int i = 0; i < strlen(b); i++) {
		nr[i] = b[strlen(b)-1-i] - '0';
	}
	nr[strlen(b)] = -1;

	int *m = mr;
	int *n = nr;

	if (compare(n, m) == 1) swap(&n, &m);
	
	int ans[257] = {1, -1};
	int empty[2] = {0, -1};
	while (compare(m, empty) != -1 && compare(n, empty) != -1) {
		if (!(m[0] % 2) && !(n[0] % 2)) {
			multi(ans, 1);
			divide(n);
			divide(m);
		} else if (!(n[0] % 2)) {
			divide(n);
		} else if (!(m[0] % 2)) {
			divide(m);
		}
		if (compare(n, m) == 1) swap(&n, &m);
		subtract(m, n);
	}
	if (compare(n, ans)) {
		product(n, ans);
	}
	else {
		product(ans, n);
	}
}
