#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <sdsl/bp_support.hpp> 
#include <sdsl/sd_vector.hpp> 
#include <string>
#include <sstream>

using namespace std;
using namespace sdsl;

const int ALPHABET_SIZE = 28; //$ 36, space 32

class TrieNode{
public:
	vector<TrieNode *> children;
	int node_number;

	TrieNode(int number):children(28, nullptr){
		node_number = number;
	}

	~TrieNode(){
		for (int i = 0; i < ALPHABET_SIZE; ++i)
		{
			if(get_child(i) != nullptr){
				delete children[i];
			}
		}
	}

	int get_node_number(){
		return node_number;
	}
	void set_child(TrieNode *node, int i){
		children[i] = node;
	}

	TrieNode *get_child(int i){
		return children[i];
	}
};

class Trie{
public:
	TrieNode *root;
	int k = 1;

	Trie(){
		root = new TrieNode(0);
	}

	~Trie(){
		delete root;
	}

	int get_index(char c){
		if(c == ' ')return 26;
		else if(c == '$')return 27;
		else return c - 'a';
	}

	char get_char(int i){
		// cout<<i<<endl;
		if(i==26){
			// cout<<"entre en espacio"<<endl;
			return ' ';
		}
		else if(i==27){
			// cout<<"entre en signo peso"<<endl;
			return '$';
		}
		else{
			// cout<<"entre otro caso "<<endl;
			char c = char(97+i);
			return c;
		}
	}

	int insert_letter(TrieNode *node, string s, int i){
		int index = get_index(s[i]);
		// cout<<"insertando letra "<<s[i]<<" "<<i<<endl;
		// cout<<node->get_node_number()<<endl;
		// // cout<<"aqui"<<endl;
		// cout<<node->get_child(index)<<endl;
		// // cout<<"aqui2"<<endl;
		// cout<<(node->get_child(index) == nullptr)<<endl;
		// cout<<"aqui3"<<endl;
		bool nulo = node->get_child(index) == nullptr;
		if(nulo){
			// cout<<"se retorna altiro"<<endl;
			TrieNode *new_node = new TrieNode(k);
			// cout<<"se crea nodo "<<new_node->get_node_number()<<endl;
			node->set_child(new_node, index);
			k++;
			return i;
		}
		else{
			// cout<<"se entro por hijo "<<node->get_child(index)->get_node_number()<<" "<<get_char(index)<<endl;
			return insert_letter(node->get_child(index), s, i+1);
		}
	}


	void insert_word(string s){
		string new_s = "$"+s+"$";
		for (int i = 1; i < new_s.size(); ++i)
		{
			// cout<<"se inserto "<< new_s[i]<< " "<< i<<endl;
			i = insert_letter(root, new_s, i);
			if(i % 1000 == 0){
				cout<<i<<" "<<s[i]<<endl;
			}
		}
	}

	void print_node(TrieNode *node){
		cout<<"printeando hijos de "<<node->get_node_number()<<endl;
		for (int i = 0; i < ALPHABET_SIZE; ++i)
		{
			if(node->get_child(i) != nullptr){
				char c = get_char(i);
				cout<<"("<<c<<","<<node->get_child(i)->get_node_number()<<")   ";
			}
		}
		cout<<endl;

		for (int i = 0; i < ALPHABET_SIZE; ++i)
		{
			if(node->get_child(i) != nullptr){
				print_node(node->get_child(i));
			}
		}

	}
	void print_trie(){
		print_node(root);
	}
};

class RegularCompresion{
public:

	vector< pair<int, char> > pairs;

	RegularCompresion(){
		pairs.push_back(make_pair(0,'$'));
	}

};

int main(){

	stringstream ss;
	string s;
	ifstream infile;
	infile.open("test4.txt");
	while(infile >> s){
		ss<<s;
	}
	infile.close();
	string text = ss.str();
	cout<<"creando trie"<<endl;
	Trie trie;
	auto start = std::chrono::system_clock::now();
	trie.insert_word(text);
	auto end = std::chrono::system_clock::now();
	cout<<trie.k<<endl;
	std::chrono::duration<double> elapsed_seconds = end-start;

	cout<<elapsed_seconds.count()<<endl;
	// ofstream myfile;
	// myfile.open ("example.txt");
	// myfile << text;
	// myfile.close();
}
// int main(){

// 	stringstream ss;
// 	string s, text;
// 	ifstream infile; 
// 	infile.open("test4.txt");
// 	while(infile>>s){
// 		ss<<s;
// 	}
// 	text = ss.str();

// 	Trie trie;
// 	// string s = "how can a clam cram in a clean cream can";
// 	// string s = "how ca";
// 	// trie.print_trie();
// 	trie.insert_word(text);
// 	cout<<trie.k<<endl;
// 	// trie.print_trie();
// 	return 0;
// }