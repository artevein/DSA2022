#include <stdio.h>
#include <stdlib.h>

typedef struct listnode {
	int val;
	int indx;
} node;

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

int int_to_buffer(char *buffer, int num) {
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


int main() {
	int N, M;
	int cur;
	int size = 1;
	int start = 0, end = 0;
	ReadInt(&N);
	ReadInt(&M);
	// large -> small
	char *buffer = malloc(10*M*sizeof(char) + 20);
	buffer[0] = 'R';
	buffer[1] = 'o';
	buffer[2] = 'u';
	buffer[3] = 'n';
	buffer[4] = 'd';
	buffer[5] = ' ';
	int buffer_head = 6;
	node* players = malloc(M*sizeof(node));
	ReadInt(&players[0].val);
	players[0].indx = 1;
	printf("Round 1:\n");
	for (int i = 2; i <= N; i++) {
		size++;
		ReadInt(&cur);
		// linear search
		int l = end;
		buffer_head += int_to_buffer(&buffer[buffer_head], i);
		buffer[buffer_head++] = ':';
		for (; l >= start; l--) {
			if (players[l%M].val >= cur) break; 
			int pos = l%M;
			int indx = players[pos].indx;
			size--;
			buffer[buffer_head++] = ' ';
			buffer_head += int_to_buffer(&buffer[buffer_head], players[pos].indx);
		}
		// revolution
		if (size > M) {
			size--;
			int pos = start%M;
			int indx = players[pos].indx;
			buffer[buffer_head++] = ' ';
			buffer_head += int_to_buffer(&buffer[buffer_head], indx);
			start += 1;
		}
		// insert new entry
		end = l+1;
		int pos = end%M;
		players[pos].val = cur;
		players[pos].indx = i;
		buffer[buffer_head++] = '\n';
		fwrite(buffer, sizeof(char), buffer_head, stdout);
		buffer_head = 6;
	}
	buffer[0] = 'F';
	buffer[1] = 'i';
	buffer[2] = 'n';
	buffer[3] = 'a';
	buffer[4] = 'l';
	buffer[5] = ':';
	buffer_head = 6;
	for (int i = end; i >= start; i--) {
		int indx = players[i%M].indx;
		buffer[buffer_head++] = ' ';
		buffer_head += int_to_buffer(&buffer[buffer_head], indx);
	}
	fwrite(buffer, sizeof(char), buffer_head, stdout);
}
