#include<stdio.h>
#include<malloc.h>
#include<stdlib.h>
#define BLACK 0
#define RED 1
typedef struct Node* NodePtr;
struct Node
{
	int val, col;
	NodePtr left, right, parent;
};
typedef struct RBT* RBTPtr;
struct RBT
{
	int nodecount, d_count, m_count, B_count;
	NodePtr root, nil;
};

void sim(const char*);
NodePtr grandparent(NodePtr);
NodePtr node_alloc(int);
RBTPtr RBT_alloc();
void RB_insert(RBTPtr, NodePtr);
void RB_insertfixup(RBTPtr, NodePtr);
void left_rotate(RBTPtr, NodePtr);
void right_rotate(RBTPtr, NodePtr);
void transplant(RBTPtr, NodePtr, NodePtr);
void RB_delete(RBTPtr, NodePtr);
void RB_deletefixup(RBTPtr, NodePtr);
NodePtr Tree_search(RBTPtr, NodePtr, int);
void RB_inorder(RBTPtr, NodePtr);
void Black_count(RBTPtr, NodePtr);

void main() {
	sim("test01.txt");
	sim("test02.txt");
//	sim("test03.txt");
//	sim("test04.txt");
//	sim("test05.txt");
	return;
}
void sim(const char* f) {
	FILE*fp;
	int data;
	NodePtr x;
	NodePtr y;
	int bh = 0;
	RBTPtr tree = RBT_alloc();
	fp = fopen(f, "r");
	while (fscanf(fp, "%d", &data) != EOF) {
		if (data > 0)
			RB_insert(tree, node_alloc(data));
		else if (data < 0) {
			x = Tree_search(tree, tree->root, -data);
			if(x != NULL)
				RB_delete(tree, x);
		}
		else
			break;
	}
	fclose(fp);
	printf("filename = %s\n", f);
	printf("total = %d\n",tree->nodecount - tree->d_count);
	printf("insert = %d\n", tree->nodecount);
	printf("delete = %d\n", tree->d_count);
	printf("miss = %d\n",tree->m_count);
	Black_count(tree, tree->root);
	printf("nb = %d\n", tree->B_count);
	y = tree->root;
	while (y != tree->nil) {
		if (y->col == BLACK)
			bh++;
		y = y->left;
	}
	printf("bh = %d\n", bh);
	RB_inorder(tree, tree->root);
	free(tree);
	return;
}
NodePtr grandparent(NodePtr x) {
	if ((x != NULL) && (x->parent != NULL))
		return x->parent->parent;
	else
		return NULL;
}

NodePtr node_alloc(int newval) {
	NodePtr self = (NodePtr)malloc(sizeof(struct Node));
	self->val = newval;
	self->left = NULL;
	self->right = NULL;
	self->parent = NULL;
	self->col = RED;
	return self;
}

RBTPtr RBT_alloc() {
	RBTPtr self = (RBTPtr)malloc(sizeof(struct RBT));
	self->nil = node_alloc(0);
	self->nil->val = 0;
	self->nil->col = BLACK;
	self->root = self->nil;
	self->root->parent = self->nil;
	self->nil->left = self->root;
	self->nil->right = self->root;
	self->nil->parent = self->nil;
	self->nodecount = 0;
	self->d_count = 0;
	self->m_count = 0;
	self->B_count = 0;
	return self;
}

void RB_insert(RBTPtr T, NodePtr n) {

	NodePtr x = T->root;
	NodePtr y = T->nil;
	while (x != T->nil) {
		y = x;
		if (n->val < x->val)
			x = x->left;
		else x = x->right;
	}
	n->parent = y;
	if (y == T->nil) T->root = n;
	else if (n->val < y->val) y->left = n;
	else y->right = n;
	n->left = T->nil;
	n->right = T->nil;
	n->col = RED;
	T->nodecount++;
	RB_insertfixup(T, n);
}
/*void RB_insert(RBTPtr self, NodePtr a, NodePtr n)
{
	if (self->root == NULL)
		self->root = n;
	else if (n->val < a->val) {
		if (a->left == NULL) {
			a->left = n;
			n->parent = a;
			n->col = RED;
			RB_insertfixup(self, n);
		}
		else
			RB_insert(self, a->left, n);
	}
	else {
		if (a->right == NULL) {
			a->right = n;
			n->parent = a;
			n->col = RED;
			RB_insertfixup(self, n);
		}
		else
			RB_insert(self, a->right, n);
	}
}*/
void RB_insertfixup(RBTPtr T, NodePtr x) {
	NodePtr y;
	while (x->parent->col == RED) {
		if (x->parent == grandparent(x)->left) { //parent가 왼쪽 자식일때
			y = grandparent(x)->right;
			if (y->col == RED) {
				x->parent->col = BLACK;
				y->col = BLACK;
				grandparent(x)->col = RED;
				x = grandparent(x);
			}
			else {
				if (x == x->parent->right) {
					x = x->parent;
					left_rotate(T, x);
				}
				x->parent->col = BLACK;
				grandparent(x)->col = RED;
				right_rotate(T, grandparent(x));
			}
		}
		else {
			y = grandparent(x)->left; //parent가 오른쪽 자식일 때
			if (y->col == RED) {
				x->parent->col = BLACK;
				y->col = BLACK;
				grandparent(x)->col = RED;
				x = grandparent(x);
			}
			else {
				if (x == x->parent->left) {
					x = x->parent;
					right_rotate(T, x);
				}
				x->parent->col = BLACK;
				grandparent(x)->col = RED;
				left_rotate(T, grandparent(x));
			}
		}


	}
	T->root->col = BLACK;
}

void left_rotate(RBTPtr self, NodePtr x) {
	NodePtr y = x->right;
	x->right = y->left;
	if (y->left != self->nil)
		y->left->parent = x;
	y->parent = x->parent;
	if (x->parent == self->nil)
		self->root = y;
	else if (x == x->parent->left)
		x->parent->left = y;
	else x->parent->right = y;
	y->left = x;
	x->parent = y;
}

void right_rotate(RBTPtr self, NodePtr x) {
	NodePtr y = x->left;
	x->left = y->right;
	if (y->right != self->nil)
		y->right->parent = x;
	y->parent = x->parent;
	if (x->parent == self->nil)
		self->root = y;
	else if (x == x->parent->left)
		x->parent->left = y;
	else x->parent->right = y;
	y->right = x;
	x->parent = y;
}

void transplant(RBTPtr T, NodePtr u, NodePtr v) {
	if (u->parent == T->nil)
		T->root = v;
	else if (u == u->parent->left)
		u->parent->left = v;
	else
		u->parent->right = v;
	v->parent = u->parent;
}

void RB_delete(RBTPtr T, NodePtr target) {
	NodePtr x = NULL;
	NodePtr tmp = NULL;
	int color = target->col;
	if (target->left == T->nil) {
		x = target->right;
		transplant(T, target, target->right);
	}
	else if (target->right == T->nil) {
		x = target->left;
		transplant(T, target, target->left);
	}
	else {
		tmp = target->right;
		while (tmp->left != T->nil)
			tmp = tmp->left;
		color = tmp->col;
		x = tmp->right;		
		transplant(T, tmp, tmp->right);
		tmp->right = target->right;
		tmp->right->parent = tmp;
		transplant(T, target, tmp);
		tmp->left = target->left;
		tmp->left->parent = tmp;
		tmp->col = target->col;
	}
	if (color == BLACK)
		RB_deletefixup(T, x);
	free(target);
	T->d_count++;
}
void RB_deletefixup(RBTPtr T, NodePtr x) {
	NodePtr sib = NULL;
	while (x != T->root && x->col == BLACK) {
		if (x == x->parent->left) {
			sib = x->parent->right;
			if (sib->col == RED) { //형제노드가 레드일경우
				sib->col = BLACK;
				x->parent->col = RED;
				left_rotate(T, x->parent);
				sib = x->parent->right;
			}
			else {//형제노드가 블랙일경우
				if (sib->left->col == BLACK && sib->right->col == BLACK) {//형제노드가 블랙이고, 형제노드의 자식노드가 모두 블랙일경우
					sib->col = RED;
					x = x->parent;
				}
				else if (sib->left->col == RED&&sib->right->col == BLACK) {//형제노드가 블랙이고, 형제노드의 왼쪽 자식노드가 레드, 오른쪽 자식노드가 블랙일경우
					sib->col = RED;
					sib->left->col = BLACK;
					right_rotate(T, sib);
					sib = x->parent->right;
				}
				else {//형제노드가 블랙이고 오른쪽 자식노드가 레드일경우
					sib->col = x->parent->col;
					sib->right->col = BLACK;
					x->parent->col = BLACK;
					left_rotate(T, x->parent);
					x = T->root;
				}
			}
		}
		else {
			sib = x->parent->left;
			if (sib->col == RED) { //형제노드가 레드일경우
				sib->col = BLACK;
				x->parent->col = RED;
				right_rotate(T, x->parent);
				sib = x->parent->left;
			}
			else {//형제노드가 블랙일경우
				if (sib->left->col == BLACK && sib->right->col == BLACK) {//형제노드가 블랙이고, 형제노드의 자식노드가 모두 블랙일경우
					sib->col = RED;
					x = x->parent;
				}
				else if (sib->right->col == RED && sib->left->col == BLACK) {//형제노드가 블랙이고, 형제노드의 오른쪽 자식노드가 레드, 왼쪽 자식노드가 블랙일경우
					sib->col = RED;
					sib->right->col = BLACK;
					left_rotate(T, sib);
					sib = x->parent->left;
				}
				else {//형제노드가 블랙이고 왼쪽 자식노드가 레드일경우
					sib->col = x->parent->col;
					sib->left->col = BLACK;
					x->parent->col = BLACK;
					right_rotate(T, x->parent);
					x = T->root;
				}
			}
		}
	}
	x->col = BLACK;
}
NodePtr Tree_search(RBTPtr T, NodePtr root, int key)
{
	if (root == T->nil) {
		T->m_count++;
		return NULL; //해당 값 없음
	}
	if (root->val < key)
		return Tree_search(T, root->right, key);
	else if (root->val > key)
		return Tree_search(T, root->left, key);
	else
		return root;
}
void RB_inorder(RBTPtr T, NodePtr x)
{
	if (x == T->nil)
		return;
	else {
		RB_inorder(T, x->left);
		if (x->col == BLACK)
			printf("%d B\n", x->val);
		else
			printf("%d R\n", x->val);
		RB_inorder(T, x->right);
	}
}
void Black_count(RBTPtr T, NodePtr x)
{
	if (x == T->nil)
		return;
	else {
		Black_count(T, x->left);
		if (x->col == BLACK)
			T->B_count++;
		Black_count(T, x->right);
	}
}