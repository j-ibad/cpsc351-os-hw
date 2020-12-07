/**
 *	Program Name: threads (CPSC351 Assignment 3)
 *	Author: Josh Ibad
 *	Professor: Dr. Gofman
 *	Class: CPSC 351-03, Fall 2020
 */
 
/**
 *	Custom BST. Tree is unbalanced and only implements functions relevant to
 *	the assignment (Add and print functions).
 *
 *	Header includes implementation due to small scope of assignment.
 */
  
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

/**
 *	Custom Tree Node structure storing key, left pointer, right pointer.
 */
template<class T>
struct Node{
	T key;
	Node* left = NULL;
	Node* right = NULL;
};


/**
 *	Custom BST. Tree is unbalanced and only implements functions relevant to
 *	the assignment (Add and print functions).
 */
template<class T>
class BinaryTree{
  public:
	BinaryTree(){ //Constructor. Set root node pointer to null.
		root = NULL;
	}
	~BinaryTree(){ //Destructor. Deallocate whole tree starting at root.
		_dealloc(root);
	}
	
	
	/**
	 *	Adds the key to the current BST subtree
	 *	@param key - Key to insert into the BST subtree
	 *	@param curr_node - Root node of the curent BST subtree
	 */
	void add(const T& key, Node<T>* curr_node){
		if(key < curr_node->key){
			if(curr_node->left == NULL){
				curr_node->left = new Node<T>{key, NULL, NULL};
			}else{
				add(key, curr_node->left);
			}
		}else{
			if(curr_node->right == NULL){
				curr_node->right = new Node<T>{key, NULL, NULL};
			}else{
				add(key, curr_node->right);
			}
		}
	}
	/**
	 *	Adds the key to the BST
	 *	@param key - Key to insert into the BST
	 */
	void add(const T& key){
		if(root == NULL){
			root = new Node<T>{key, NULL, NULL};
		}else{
			add(key, root);
		}
	}
	
	
	/**
	 *	Prints the specified BST Subtree using In-Order traversal
	 *	@param curr_node - Root node of the curent BST subtree
	 */
	void print(Node<T>* curr_node){
		if(curr_node != NULL){
			print(curr_node->left);
			fprintf(stderr, "%d\n", curr_node->key);
			print(curr_node->right);
		}
	}
	/**
	 *	Prints the whole BST using In-Order traversal
	 */
	void print(){ print(root); }
	
	
  private:
	Node<T>* root;
	
	
	/**
	 *	Deallocate the current subtree
	 *	@param curr_node - Pointer to current root node of subtree
	 */
	void _dealloc(Node<T>* curr_node){
		if(curr_node != NULL){
			_dealloc(curr_node->left);
			_dealloc(curr_node->right);
			delete curr_node;
		}
	}
};