#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <sdsl/bp_support.hpp> 
#include <sdsl/sd_vector.hpp> 
#include <sdsl/bit_vectors.hpp> 
#include <sdsl/util.hpp> 
#include <string>
#include <sstream>

using namespace std;
using namespace sdsl;

const int ALPHABET_SIZE = 28; //$ 36, space 32

int get_index(char c){

	if(c == ' ')return 26;
	else if(c == '$')return 27;
	else if(c < 'a' || c > 'z'){
		cout<<"ERRRRRRRRRRRRROOOOOOORERRRRRRRRRRRRROOOOOOORERRRRRRRRRRRRROOOOOOOR"<<endl;
		return 0;
	}
	else{
		return (c - 'a');
	}
}

char get_char(int i){
	if(i==26){
		return ' ';
	}
	else if(i==27){
		return '$';
	}
	else{
		char c = char(97+i);
		return c;
	}
}
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

class RegularCompresion{
public:

	vector< pair<int, char> > pairs;

	RegularCompresion(){
		pairs.push_back(make_pair(0,'$'));
	}

	void add_pair(int number, int letter){
		pairs.push_back(make_pair(number, letter));
	}

	void print_pairs(){
		for (int i = 0; i < pairs.size(); ++i)
		{
			cout<<pairs[i].first<<" "<<pairs[i].second<<endl;
		}
	}

	string decompress_phrase(int k){
		int p = pairs[k].first;
		char c = pairs[k].second;

		if(p==0){
			string s;
			return s+c;
		}
		else{
			return decompress_phrase(p) + c;
		}
	}

	string decompress(){
		stringstream ss;
		for (int i = 1; i < pairs.size(); ++i)
		{
			string s = decompress_phrase(i);
			ss << decompress_phrase(i);
		}
		return ss.str();
	}

	string decompress_index(int i, int j){
		stringstream ss;
		ss<<"$";
		int length = 1;
		for(int k=1; k<pairs.size();k++){
			string s = decompress_phrase(k);
			length+=s.size();
			ss << s;
			if(length > j){
				string result = ss.str();
				return result.substr(i, j-i+1);
			}
		}
		return ss.str();
	}
};

class Trie{
public:
	TrieNode *root;
	RegularCompresion *rcomp;
	int k = 1;
	vector<bool> bp;
	vector<char> S;
	vector<int> N;

	Trie(RegularCompresion *regular_compression){
		root = new TrieNode(0);
		rcomp = regular_compression;
	}

	~Trie(){
		delete root;
	}

	int get_index(char c){

		if(c == ' ')return 26;
		else if(c == '$')return 27;
		else if(c < 'a' || c > 'z'){
			cout<<"ERRRRRRRRRRRRROOOOOOORERRRRRRRRRRRRROOOOOOORERRRRRRRRRRRRROOOOOOOR"<<endl;
			return 0;
		}
		else{
			return (c - 'a');
		}
	}

	char get_char(int i){
		if(i==26){
			return ' ';
		}
		else if(i==27){
			return '$';
		}
		else{
			char c = char(97+i);
			return c;
		}
	}

	int insert_letter(TrieNode *node, string s, int i){
		int index = get_index(s[i]);
		bool nulo = node->get_child(index) == nullptr;

		if((node->get_child(index)) == nullptr){
			TrieNode *new_node = new TrieNode(k);
			node->set_child(new_node, index);
			rcomp->add_pair(node->get_node_number(), s[i]);
			k++;
			return i;
		}
		else{
			return insert_letter(node->get_child(index), s, i+1);
		}
	}


	vector<int> insert_word(string s){
		string new_s = "$"+s+"$";
		vector<int> v(new_s.size()-1, 0);

		for (int i = 1; i < new_s.size(); ++i)
		{
			v[i-1]=1;
			i = insert_letter(root, new_s, i);
		}
		
		return v;
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

	vector<bool> compact_representation(){
		bp.resize(2*(k-1)+2, 0);
		S.push_back('-');
		N.resize(k-1);
		cout<<"comprimido hasta aca"<<endl;
		create_BP(root, 0);
		return bp;
	}

	int create_BP(struct TrieNode *node, int j){
		bp[j] = 1;
		j++;

		for (int i = 0; i < ALPHABET_SIZE; ++i)
		{
			if(node->children[i] != nullptr){
				S.push_back(get_char(i));
				N[(node->children[i]->node_number)-1] = S.size();
				j = create_BP(node->children[i], j);
			}
		}
		j++;
		return j;
	}
};

void fun(bp_support_sada<> t){
	cout<<t.select(1)<<endl;
	cout<<t.select(2)<<endl;
	cout<<t.select(3)<<endl;
	cout<<t.select(4)<<endl;
	cout<<t.select(5)<<endl;
	cout<<t.select(6)<<endl;
}

string part_decompress(int v, int kr, int cant, bp_support_sada<> bps, vector<int> N, vector<char> S){
	string s = "";

	while(cant){
		char c = S[bps.rank(v)-1];
		s = c + s; 
		cant--;
		if(bps.enclose(v)==0){
			v = bps.select(N[kr-2]);
			kr--;
		}
		else{
			v=bps.enclose(v);
		}
	}

	return s;
} 

string decompress_index_compact(int i, int j, sd_vector<> M, bp_support_sada<> bps, vector<int> N, vector<char> S){
	sd_vector<>::rank_1_type sdb_rank(&M);
	sd_vector<>::select_1_type sdb_sel(&M);
	int kr = sdb_rank(j);
	int v = bps.select(N[kr-1]); /// bps indexado desde 0
	int d = sdb_sel(sdb_rank(j)+1)+1 - (j+1);

	if(d<0){
		d=0;
	}
	int va = bps.level_anc(v, d);
	return part_decompress(va, kr, j-i+1, bps, N, S);
}

void memory_usage_regular(RegularCompresion *regular_compression){
	int pair_size = sizeof(regular_compression->pairs[0]);
	cout<<sizeof(regular_compression->pairs) + pair_size*regular_compression->pairs.size()<<endl;
}

void memory_usage_compact(sd_vector<> M, bp_support_sada<> bps, vector<int> N, vector<char> S){
	int size_S = sizeof(S[0])*S.size() + sizeof(S);
	int size_bps = size_in_bytes(bps);
	int size_N = size_in_bytes(N);
	int size_M = size_in_bytes(M);
	cout<< size_S + size_bps + size_N + size_M <<endl;
}

int rand_num(int lower, int upper){
	return (rand() % (upper - lower + 1)) + lower;
}

int main(){
	
	RegularCompresion *regular_compression = new RegularCompresion();
	vector<int> substring_length={2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192};
	stringstream ss;
	string s;
	ifstream infile;
	infile.open("bible1.txt");
	while(getline(infile, s)){
		ss<<s;
	}
	infile.close();
	string text = ss.str();
	int text_size = text.size();
	Trie *trie = new Trie(regular_compression);
	vector<int> v = trie->insert_word(text);
	text.clear();
	ss.str(std::string());
	s.clear();
	cout<<"creado el arbol"<<endl;

	//turning v to M
	bit_vector v_bit = bit_vector(v.size(), 0);
	for(size_t i=0; i<v_bit.size(); i++){
		if(v[i]==1){
			v_bit[i]=1;
		}
	}
	sd_vector<> M = sd_vector<>(v_bit);
	v.clear();
	v.shrink_to_fit();
	// util::clear(v_bit);
	cout<<"creado M"<<endl;
	vector<bool> bp = trie->compact_representation();
	vector<int> N = trie->N;
	vector<char> S = trie->S;
	delete trie;
	cout<<"se termina de hacer compacta"<<endl;
	//turning bp into bps
	bit_vector bp_bit = bit_vector(bp.size(), 0);
	cout<<"se creo bit bp"<<endl;
	for(size_t i=0; i<v_bit.size(); i++){
		cout<<i<<" ";
		if(bp[i]){
			bp_bit[i]=1;
		}
	}
	cout<<endl;
	cout<<"se definio bit bp"<<endl;
	bp_support_sada<> bps(&bp_bit);
	cout<<"se hizo bp definitivo"<<endl;
	bp.clear();
	bp.shrink_to_fit();
	cout<<"creado bp"<<endl;

	// vector<int> N = trie->N;
	// vector<char> S = trie->S;
	// delete trie;
	cout<<decompress_index_compact(6, 18, M, bps, N, S)<<endl;
	// cout<<regular_compression->decompress_index(6, 18)<<endl;
	// fun(bps);
	memory_usage_regular(regular_compression);
	memory_usage_compact(M, bps, N, S);

	// for(int k = 0; k < substring_length.size(); k++){
	// 	int sub_l = substring_length[k];
	// 	cout<<"substring de largo "<<sub_l<<endl;
	// 	int i = rand_num(1, text.size()-sub_l);
	// 	int j = i+sub_l-1;
	// 	cout<<"i y j elegidos "<<i<<" "<<j<<endl;
	// 	clock_t start, end; 
	// 	start = clock();
	// 	string reg_str = regular_compression.decompress_index(i, j);
	// 	end = clock();
	// 	double regular_time = double(end - start) / double(CLOCKS_PER_SEC);
	// 	cout<<"fin descompresion regular"<<endl;
	// 	start = clock();
	// 	string comp_str = decompress_index_compact(i, j, M, bps, trie.N, trie.S);
	// 	end = clock();
	// 	double compact_time = double(end - start) / double(CLOCKS_PER_SEC);
	// 	cout<<"fin descompresion compacta"<<endl;
	// 	cout<<"Tiempo representacion regular: "<<fixed<<regular_time<<setprecision(7)<<endl;
	// 	cout<<"Tiempo representacion compacta: "<<fixed<<compact_time<<setprecision(7)<<endl;
	// 	cout<<"Diferencia: "<<fixed<<regular_time-compact_time<<setprecision(7)<<endl;
	// 	if(reg_str == comp_str){
	// 		cout<<"String comparados, son iguales"<<endl;
	// 	}
	// 	else{
	// 		cout<<"String comparados, NO son iguales"<<endl;
	// 	}

	// }
	// cout<<decompress_index_compact(6, 18, M, bps, trie.N, trie.S)<<endl;
	// cout<<regular_compression.decompress_index(6, 18)<<endl;


}