#ifndef GEO_BTREE_HPP
#define GEO_BTREE_HPP

namespace geo
{

template<class _Tp>
class Status
{
	typedef enum { RED, BLACK } Color;

	struct Node {
		_Tp*				value;
		Color				color;
		Node				*leftTree, *rightTree, *parent;

		Node()
			: value(nullptr)
			, color(RED)
			, leftTree(nullptr)
			, rightTree(nullptr)
			, parent(nullptr)
		{}

		Node* grandParent()
		{
			if (parent == nullptr) {
				return nullptr;
			}
			return parent->parent;
		}

		Node* uncle()
		{
			if (grandParent() == nullptr) {
				return nullptr;
			}

			if (parent == grandParent()->rightTree) {
				return grandParent()->leftTree;
			}
			return grandParent()->rightTree;
		}

		Node* sibling() {
			if (parent->leftTree == this) {
				return parent->rightTree;
			}
			return parent->leftTree;
		}
	};

private:

	Node *root, *NIL;

	void rotateRight(Node* p) {
		Node *gp = p->grandparent();
		Node *fa = p->parent;
		Node *y  = p->rightTree;

		fa->leftTree = y;

		if (y != NIL) {
			y->parent = fa;
		}
		p->rightTree = fa;
		fa->parent = p;

		if (root == fa) {
			root = p;
		}
		p->parent = gp;

		if (gp != nullptr) {
			if (gp->leftTree == fa) {
				gp->leftTree = p;
			}
			else {
				gp->rightTree = p;
			}
		}
	}

	void rotate_left(Node *p) {
		if (p->parent == NULL) {
			root = p;
			return;
		}
		Node *gp = p->grandparent();
		Node *fa = p->parent;
		Node *y = p->leftTree;

		fa->rightTree = y;

		if (y != NIL) {
			y->parent = fa;
		}
		p->leftTree = fa;
		fa->parent = p;

		if (root == fa) {
			root = p;
		}
		p->parent = gp;

		if (gp != nullptr) {
			if (gp->leftTree == fa)
				gp->leftTree = p;
			else
				gp->rightTree = p;
		}
	}

	void inorder(Node *p) {
		if (p == NIL) {
			return;
		}

		if (p->leftTree) {
			inorder(p->leftTree);
		}
		// std::cout << p->value << " ";

		if (p->rightTree) {
			inorder(p->rightTree);
		}
	}

	Node* getSmallestChild(Node *p) {
		if (p->leftTree == NIL) {
			return p;
		}
		return getSmallestChild(p->leftTree);
	}

	bool delete_child(Node *p, int data) {
		if (p->value > data) {
			if (p->leftTree == NIL) {
				return false;
			}
			return delete_child(p->leftTree, data);
		}

		if (p->value < data) {
			if (p->rightTree == NIL) {
				return false;
			}
			return delete_child(p->rightTree, data);
		}

		if (p->rightTree == NIL) {
			delete_one_child(p);
			return true;
		}
		Node *smallest = getSmallestChild(p->rightTree);
		swap(p->value, smallest->value);
		delete_one_child(smallest);

		return true;
	}

	void delete_one_child(Node *p) {
		Node *child = p->leftTree == NIL ? p->rightTree : p->leftTree;
		if (p->parent == NULL && p->leftTree == NIL && p->rightTree == NIL) {
			p = NULL;
			root = p;
			return;
		}

		if (p->parent == NULL) {
			delete  p;
			child->parent = NULL;
			root = child;
			root->color = BLACK;
			return;
		}

		if (p->parent->leftTree == p) {
			p->parent->leftTree = child;
		}
		else {
			p->parent->rightTree = child;
		}
		child->parent = p->parent;

		if (p->color == BLACK) {
			if (child->color == RED) {
				child->color = BLACK;
			}
			else
				delete_case(child);
		}

		delete p;
	}

	void delete_case(Node *p) {
		if (p->parent == NULL) {
			p->color = BLACK;
			return;
		}
		if (p->sibling()->color == RED) {
			p->parent->color = RED;
			p->sibling()->color = BLACK;
			if (p == p->parent->leftTree)
				rotate_left(p->sibling());
			else
				rotate_right(p->sibling());
		}
		if (p->parent->color == BLACK && p->sibling()->color == BLACK
			&& p->sibling()->leftTree->color == BLACK && p->sibling()->rightTree->color == BLACK) {
			p->sibling()->color = RED;
			delete_case(p->parent);
		}
		else if (p->parent->color == RED && p->sibling()->color == BLACK
			&& p->sibling()->leftTree->color == BLACK && p->sibling()->rightTree->color == BLACK) {
			p->sibling()->color = RED;
			p->parent->color = BLACK;
		}
		else {
			if (p->sibling()->color == BLACK) {
				if (p == p->parent->leftTree && p->sibling()->leftTree->color == RED
					&& p->sibling()->rightTree->color == BLACK) {
					p->sibling()->color = RED;
					p->sibling()->leftTree->color = BLACK;
					rotate_right(p->sibling()->leftTree);
				}
				else if (p == p->parent->rightTree && p->sibling()->leftTree->color == BLACK
					&& p->sibling()->rightTree->color == RED) {
					p->sibling()->color = RED;
					p->sibling()->rightTree->color = BLACK;
					rotate_left(p->sibling()->rightTree);
				}
			}
			p->sibling()->color = p->parent->color;
			p->parent->color = BLACK;
			if (p == p->parent->leftTree) {
				p->sibling()->rightTree->color = BLACK;
				rotate_left(p->sibling());
			}
			else {
				p->sibling()->leftTree->color = BLACK;
				rotate_right(p->sibling());
			}
		}
	}

	void insert(Node *p, int data) {
		if (p->value >= data) {
			if (p->leftTree != NIL)
				insert(p->leftTree, data);
			else {
				Node *tmp = new Node();
				tmp->value = data;
				tmp->leftTree = tmp->rightTree = NIL;
				tmp->parent = p;
				p->leftTree = tmp;
				insert_case(tmp);
			}
		}
		else {
			if (p->rightTree != NIL)
				insert(p->rightTree, data);
			else {
				Node *tmp = new Node();
				tmp->value = data;
				tmp->leftTree = tmp->rightTree = NIL;
				tmp->parent = p;
				p->rightTree = tmp;
				insert_case(tmp);
			}
		}
	}

	void insert_case(Node *p) {
		if (p->parent == nullptr) {
			root = p;
			p->color = BLACK;
			return;
		}
		if (p->parent->color == RED) {
			if (p->uncle()->color == RED) {
				p->parent->color = p->uncle()->color = BLACK;
				p->grandparent()->color = RED;
				insert_case(p->grandparent());
			}
			else {
				if (p->parent->rightTree == p && p->grandparent()->leftTree == p->parent) {
					rotate_left(p);
					rotate_right(p);
					p->color = BLACK;
					p->leftTree->color = p->rightTree->color = RED;
				}
				else if (p->parent->leftTree == p && p->grandparent()->rightTree == p->parent) {
					rotate_right(p);
					rotate_left(p);
					p->color = BLACK;
					p->leftTree->color = p->rightTree->color = RED;
				}
				else if (p->parent->leftTree == p && p->grandparent()->leftTree == p->parent) {
					p->parent->color = BLACK;
					p->grandparent()->color = RED;
					rotate_right(p->parent);
				}
				else if (p->parent->rightTree == p && p->grandparent()->rightTree == p->parent) {
					p->parent->color = BLACK;
					p->grandparent()->color = RED;
					rotate_left(p->parent);
				}
			}
		}
	}

	void DeleteTree(Node *p) {
		if (!p || p == NIL) {
			return;
		}
		DeleteTree(p->leftTree);
		DeleteTree(p->rightTree);
		delete p;
	}

public:

	Status() {
		NIL = new Node();
		NIL->color = BLACK;
		root = NULL;
	}

	~Status() {
		if (root) {
			DeleteTree(root);
		}
		delete NIL;
	}

	void inorder() {
		if (root == nullptr) {
			return;
		}
		inorder(root);
	}

	void insert(int x) {
		if (root == nullptr) {
			root = new Node();
			root->color = BLACK;
			root->leftTree = root->rightTree = NIL;
			root->value = x;
		}
		else {
			insert(root, x);
		}
	}

	bool delete_value(int data) {
		return delete_child(root, data);
	}
};

} // namespace geo


#endif /* GEO_BTREE_HPP */