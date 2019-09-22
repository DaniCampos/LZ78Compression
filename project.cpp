#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <sdsl/bp_support.hpp> 
#include <sdsl/sd_vector.hpp> 
#include <sdsl/bit_vectors.hpp> 
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

class BPCompression{
public:
	bit_vector b;
	sd_vector<> M;
	bit_vector bp;
	vector<char> S;
	vector<int> N;
	bp_support_sada<> bps;//(&b);
	int t_size;

	// sbd = sd_vector<>(b)?
	BPCompression(){
		
	}

	void set_size(int text_size){
		t_size = text_size;
		b.resize(t_size);
	}

	void set_size_bp(int k){
		bp.resize(2*k + 2);
		N.resize(k);
	}


	void set_M(int index){
		// cout<<"en M"<<endl;
		// cout<<size_t(index)<<" "<<index<<endl;
		b[size_t(index)] = 1; 
	}

	void close_M(){
		M = sd_vector<>(b);
	}

	void set_bp(int index){
		bp[index] = 1;
	}

	void set_S(char c){
		S.push_back(c);
	}

	void fun(vector<int> v){
		bit_vector a = bit_vector(v.size(), 0);
		// bit_vector<> a(v.size(), 0);
		for(size_t i=0; i<a.size(); i++){
			if(v[i]==1){
				a[i]=1;
			}
		}
		M = sd_vector<>(a);
	}

	void set_N(int i){
		// cout<<S.size()<<endl;
		N[i-1]=S.size();
	}

	void close_bp(){
		bp_support_sada<> temp(&bp);
		// sdsl::util::assign(temp, bps);
		bps=temp;
	}

	string part_decompress(int v, int kr, int cant){
		string s = "";
		while(cant){
			// cout<<"el ran "<<bps.rank(v)-1<<endl;
			char c = S[bps.rank(v)-1];
			// cout<<"c:"<<c<<endl;
			s = c + s; 
			cant--;
			// cout<<"enclo "<<bps.enclose(v)<<endl;
			if(bps.enclose(v)==0){
				// cout<<kr-1<<endl;
				// cout<<N[kr-1]<<endl;
				v = bps.select(N[kr-2]);
				// cout<<"nuev v "<<v<<endl;
				kr--;
			}
			else{
				v=bps.enclose(v);
			}
		}

		return s;
	} 

	string decompress_index(int i, int j){
		cout<<"valores a des "<<i<<" "<<j<<endl;
		int ans =0;
		for(int i=0; i<j; i++){
			if(b[i]==1){
				ans++;
			}
		}
		// cout<<ans<<endl;
		sd_vector<>::rank_1_type sdb_rank(&M);
		rank_support_v<1> b_rank(&b); // rank exclusivo
		int kr = sdb_rank(j);
		// cout<<"se hizo rank"<<endl;
		

		sd_vector<>::select_1_type sdb_sel(&M);
		// cout<<"kr "<<kr<<endl;
		// cout<<b_rank(j)<<endl;
		// cout<<N[kr-1]<<endl;
		// cout<<bps.select(N[kr-1])<<endl;

		int v = bps.select(N[kr-1]); /// bps indexado desde 0
		// cout<<"se selecciono nodo"<<endl;
		// cout<<"v "<<v<<endl;
		// cout<<j+1<<endl;
		// cout<<"rank "<<sdb_rank(18)<<endl;
		// cout<<sdb_rank(j+2)<<endl;
		// cout<<sdb_sel(2)<<endl;
		// cout<<sdb_sel(sdb_rank(j)+1)<<endl;
		int d = sdb_sel(sdb_rank(j)+1)+1 - (j+1);
		// cout<<"d "<<d<<endl;
		if(d<0){
			d=0;
		}
		int va = bps.level_anc(v, d);
		// cout<<va<<endl;
		// cout<<"se llego al nodo"<<endl;
		return part_decompress(va, kr, j-i+1);
		// cout<<bps.enclose(2)<<endl;
	}

	void print_bps(){
		for (int i = 0; i < bp.size(); ++i)
		{
			cout<<bps.rank(i)<<" ";
		}
		cout<<endl;
		
	}
};

class Trie{
public:
	TrieNode *root;
	RegularCompresion *rcomp;
	BPCompression *bpcomp;
	int k = 1;

	Trie(RegularCompresion *regular_compression, BPCompression *bp_compression){
		root = new TrieNode(0);
		rcomp = regular_compression;
		bpcomp = bp_compression;
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
		if((node->get_child(index)) == nullptr){
			// cout<<"se retorna altiro"<<endl;
			TrieNode *new_node = new TrieNode(k);
			// cout<<"se crea nodo "<<new_node->get_node_number()<<endl;
			// cout<<s[i]<<" se crea nodo "<<new_node->get_node_number()<<endl;
			node->set_child(new_node, index);
			rcomp->add_pair(node->get_node_number(), s[i]);
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
		std::vector<int> v(new_s.size()-1, 0);
		for (int i = 1; i < new_s.size(); ++i)
		{
			// bpcomp->set_M(i-1);
			v[i-1]=1;
			// cout<<"se inserta "<< new_s[i]<< " "<< i<<endl;
			i = insert_letter(root, new_s, i);
			// if(i % 1000 == 0){
			// 	cout<<i<<" "<<s[i]<<endl;
			// }
		}
		// bpcomp->close_M();
		// cout<<"EL V ES "<<endl;
		// cout<<v<<endl;
		bpcomp->fun(v);
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

	void compact_representation(){
		bpcomp->set_size_bp(k-1);
		bpcomp->set_S('-');
		create_BP(root, 0);
		bpcomp->close_bp();
	}

	int create_BP(struct TrieNode *node, int j){

		bpcomp->set_bp(j);
		j++;
		for (int i = 0; i < ALPHABET_SIZE; ++i)
		{
			if(node->children[i] != nullptr){
				bpcomp->set_S(get_char(i));
				// cout<<get_char(i)<<" "<<node->children[i]->node_number<<endl;
				bpcomp->set_N(node->children[i]->node_number);
				// S.push_back(get_char(i));
				// N[node->children[i]->node_number]=S.size()-1;
				j = create_BP(node->children[i], j);
			}
		}
		j++;
		return j;
	}
};

class Test{
public:
	RegularCompresion regular_compression;
	BPCompression bp_compression;
	string text;
	vector<int> substring_length={2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192};

	Test(string file){
		stringstream ss;
		string s;
		ifstream infile;
		infile.open(file);
		while(getline(infile, s)){
			ss<<s;
		}
		infile.close();
		text = ss.str();
		bp_compression.set_size(text.size() + 1);
		Trie trie(&regular_compression, &bp_compression);
		trie.insert_word(text);
		// cout<<"tamano trie "<<trie.k - 1<<endl;
		// cout<<bp_compression.M.size()<<endl;
		// cout<<bp_compression.M<<endl;
		// trie.print_trie();

		trie.compact_representation();

		// cout<<"s es"<<endl;
		// cout<<bp_compression.S<<endl;
		// cout<<bp_compression.N<<endl;
		// cout<<bp_compression.bp<<endl;
		// cout<<bp_compression.M<<endl;
	}

	void memory_usage_regular(){
		int pair_size = sizeof(regular_compression.pairs[0]);
		cout<<sizeof(regular_compression.pairs) + pair_size*regular_compression.pairs.size()<<endl;
	}

	void memory_usage_compact(){
		int size_S = sizeof(bp_compression.S[0])*bp_compression.S.size() + sizeof(bp_compression.S);
		int size_bps = size_in_bytes(bp_compression.bps);
		int size_N = size_in_bytes(bp_compression.N);
		int size_M = size_in_bytes(bp_compression.M);
		cout<< size_S + size_bps + size_N + size_M <<endl;
	}

	int rand_num(int lower, int upper){
		return (rand() % (upper - lower + 1)) + lower;
	}

	void decompress_timing(int sub_l){
		cout<<"Corriendo test para substring de largo: "<<sub_l<<endl;
		int i = rand_num(1, text.size()-sub_l);
		int j = i+sub_l;
		// cout<<i<<" "<<j<<endl;
		clock_t start, end; 

		start = clock();
		regular_compression.decompress_index(i, j);
		end = clock();

		double regular_time = double(end - start) / double(CLOCKS_PER_SEC);

		start = clock();
		bp_compression.decompress_index(i, j);
		end = clock();

		double compact_time = double(end - start) / double(CLOCKS_PER_SEC);

		cout<<"Tiempo representacion regular: "<<fixed<<regular_time<<setprecision(7)<<endl;
		cout<<"Tiempo representacion compacta: "<<fixed<<compact_time<<setprecision(7)<<endl;
		cout<<"Diferencia: "<<fixed<<regular_time-compact_time<<setprecision(7)<<endl;

	}

	void test(){

		for (int i = 0; i < substring_length.size(); ++i)
		{
			decompress_timing(substring_length[i]);
		}
	}

};
// 11111111010110100100110101011010001000100
// 11111111010110100100110101011010001000100
// 1101010101100101010100
// 1101010101100101010100
int main(){

	Test t("bible.txt");
	t.memory_usage_regular();
	t.memory_usage_compact();
	t.test();
	// cout<<t.bp_compression.decompress_index(340, 342)<<endl;
	// cout<<t.regular_compression.decompress_index(6810, 6812)<<endl;
	// stringstream ss;
	// string s;
	// ifstream infile;
	// infile.open("test1.txt");
	// while(getline(infile, s)){
	// 	ss<<s;
	// }
	// infile.close();
	// string text = ss.str();
	// cout<<text<<endl;
	// cout<<"creando trie"<<endl;
	// RegularCompresion regular_compression;
	// BPCompression bp_compression;
	// bp_compression.set_size(text.size() + 1);
	// Trie trie(&regular_compression, &bp_compression);
	// auto start = std::chrono::system_clock::now();
	// trie.insert_word(text);
	// auto end = std::chrono::system_clock::now();
	// cout<<trie.k<<endl;
	// std::chrono::duration<double> elapsed_seconds = end-start;

	// cout<<elapsed_seconds.count()<<endl;
	// trie.compact_representation();

	// cout<<bp_compression.bp<<endl;
	// cout<<bp_compression.M.size()<<endl;
	// cout<<bp_compression.S<<endl;
	// cout<<bp_compression.N<<endl;
	// bp_compression.decompress_index(2, 8);
	
	// bp_compression.decompress_index(32, 36);
	// bp_compression.print_bps();
	// for (int i = 0; i < 2*trie.k + 2; ++i)
	// {
	// 	cout<(bp_compression.bps).rank(i)<<" ";
	// }
	// cout<<endl;
	// cout<<regular_compression.decompress_index(2047, 2112)<<endl;

}
// int main(){

// 	RegularCompresion regular_compression;
// 	Trie trie(&regular_compression);
// 	string s = "how can a clam cram in a clean cream can";
// 	// string s = "how ca";
// 	// trie.print_trie();
// 	trie.insert_word(s);
// 	cout<<trie.k<<endl;
// 	regular_compression.print_pairs();
// 	cout<<regular_compression.decompress_index(11, 18)<<endl;
// 	// trie.print_trie();
// 	return 0;
// }