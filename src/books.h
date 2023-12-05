#ifndef BOOKSTORE_PRE_BOOKS_H
#define BOOKSTORE_PRE_BOOKS_H

#include <iostream>
#include <cstring>
#include <fstream>
#include <climits>
#include <map>

using std::string;
using std::fstream;
using std::ifstream;
using std::ofstream;
using std::pair;
using std::map;

const int sqn = 22;
const int info_num=25;

class Node;

class KV;

class NodeMemory;

class KV {

public:

  char key[70] = "";
  int value = 0;

  KV() = default;

  KV(char _key[], int _value) {
    strcpy(key, _key);
    value = _value;
  }

  bool operator==(const KV &rhs) const {
    return (strcmp(key, rhs.key) == 0) && (value == rhs.value);
  }

  bool operator<(const KV &rhs) const {
    if (strcmp(key, rhs.key) == 0) return value < rhs.value;
    return strcmp(key, rhs.key) < 0;
  }

};

class Node {

public:

  KV first = KV();
  int size = 0;
  int pos = 0;
  KV data[sqn];

  Node() = default;

  Node(char _key[70], int _val, int _pos) {
    first = KV(_key, _val);
    //std::cout<<key<<'\n';
    pos = _pos;
    size = 0;
    data[0]=first;
  }

  void print() {
    std::cout << first.key << " " << first.value <<" "<<pos<< ":\n";
    KV val;
    if (pos < 3) {
      std::cout << "   null\n";
    }
    for (int i = 0; i < size; ++i) {
      val = data[i];
      std::cout << "   " << val.key << "   " << val.value << '\n';
    }
  }

  void insert(KV kv) {
    int i;
    int left=0,right=size;
    int mid;
    while(left+1<right){
      mid=(left+right)>>1;
      if(data[mid]<kv) left=mid;
      else right=mid;
    }
    i=left+1;
    if(kv<data[0]) i=0;
    for (int j = size; j > i; j--) {
      data[j] = data[j - 1];
    }
    data[i] = kv;
    first=data[0];
    size++;
  }

  void remove(KV kv) {
    int i;
    int left=0,right=size;
    int mid;
    while(left+1<right){
      mid=(left+right)>>1;
      if(data[mid]<kv) left=mid;
      else right=mid;
    }
    i=left+1;
    if(data[0]==kv) i=0;
    if(!(data[i]==kv)) return;
    for(;i<size-1;i++) {
      data[i]=data[i+1];
    }
    size--;
    if (size > 0) first = data[0];
    else first = KV();
  }

  void get(char _key[], bool &exist) const {
    int i;
    int left=0,right=size;
    int mid;
    while(left+1<right){
      mid=(left+right)>>1;
      if(strcmp(data[mid].key,_key)<0) left=mid;
      else right=mid;
    }
    i=left+1;
    if(strcmp(data[0].key,_key)==0) i=0;
    for (; i < size; i++) {
      if (strcmp(data[i].key, _key) == 0) {
        exist = true;
        std::cout << data[i].value << ' ';
      }
    }
  }

};// stored in the auxiliary file



class NodeMemory {

  // in the main file,
  // the keys: ordered index
  // the first 2 Nodes: head node, tail node
  // then it stores every Node

private:

  fstream file_main,file_aux;
  string main_name;//file_main
  int lengthofnodes = 0;//total length of Nodes
  int lengthoflist = 0;//total length of linked list (including head and tail)
  std::map<KV,int> list;//in the beginning of main, maps the key-value to the position

public:

  NodeMemory() = default;

  NodeMemory(const string &_main_name) : main_name(_main_name) {}

  void initialise(const string &FN = "") {
    if (FN != "") main_name = FN;
    file_main.open(main_name, std::ios::in);
    file_aux.open(main_name+"_aux",std::ios::in);
    if (!file_main.is_open()) {
      //std::cout << "create new\n";
      file_main.close();
      file_aux.close();
      char st[70] = "", ed[70] = "zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz";
      list.insert(pair<KV,int>(KV(st,0),1));
      list.insert(pair<KV,int>(KV(ed,0),2));
      init_list();
      Node head(st, 0, 1), tail(ed, 0, 2);
      write(head);
      write(tail);
      update_aux();
    } else {
      file_main.close();
      file_aux.close();
      read_aux();
      read_list();
    }
  }

  //push
  void update_aux() {
    file_aux.open(main_name+"_aux",fstream::out | fstream::binary);
    file_aux.write(reinterpret_cast<char *>(&lengthofnodes), sizeof(int));
    file_aux.write(reinterpret_cast<char *>(&lengthoflist), sizeof(int));
    file_aux.close();
  }

  void init_list() {
    file_main.open(main_name, fstream::out | fstream::binary);
    int cnt=0;
    for(auto i :list) {
      file_main.write(reinterpret_cast<char *>(&i), sizeof(pair<KV,int>));
      cnt++;
    }
    pair<KV,int> tmp(KV(),0);
    while(cnt<info_num) {
      file_main.write(reinterpret_cast<char *>(&tmp), sizeof(pair<KV,int>));
      cnt++;
    }
    file_main.close();
  }

  void update_list() {
    file_main.open(main_name, fstream::out | fstream::in);
    int cnt=0;
    for(auto i :list) {
      file_main.write(reinterpret_cast<char *>(&i), sizeof(pair<KV,int>));
      cnt++;
    }
    pair<KV,int> tmp(KV(),0);
    while(cnt<info_num) {
      file_main.write(reinterpret_cast<char *>(&tmp), sizeof(pair<KV,int>));
      cnt++;
    }
    file_main.close();
  }

  //pull
  void read_aux() {
    file_aux.open(main_name+"_aux",std::ios::in);
    file_aux.read(reinterpret_cast<char *>(&lengthofnodes), sizeof(int));
    file_aux.read(reinterpret_cast<char *>(&lengthoflist), sizeof(int));
    file_aux.close();
  };

  void read_list() {
    list.clear();
    file_main.open(main_name, ifstream::in);
    pair<KV,int> tmp;
    for(int i=0;i<lengthoflist;i++) {
      file_main.read(reinterpret_cast<char *>(&tmp), sizeof(pair<KV,int>));
      list.insert(tmp);
    }
    file_main.close();
  }

  //pair<KV,int> tmp = main[n], 1-based
  void get_info(pair<KV,int> &tmp, int n) {
    if (n > info_num) return;
    file_main.open(main_name, std::ifstream::in);
    file_main.seekp((n - 1) * sizeof(pair<KV,int>));
    file_main.read(reinterpret_cast<char *>(&tmp), sizeof(pair<KV,int>));
    file_main.close();
  }

  //main[n] = tmp, 1-based
  void write_info(pair<KV,int> tmp, int n) {
    if (n > info_num) return;
    file_main.open(main_name, std::ofstream::out | std::ifstream::in);
    file_main.seekg((n - 1) * sizeof(pair<KV,int>));
    file_main.write(reinterpret_cast<char *>(&tmp), sizeof(pair<KV,int>));
    file_main.close();
  }

  //append t to the end
  void write(Node &t) {
    file_main.open(main_name, std::ofstream::app);
    file_main.write(reinterpret_cast<char *>(&t), sizeof(Node));
    file_main.close();
    ++lengthofnodes;
    ++lengthoflist;
    update_aux();
  }

  //main[index] = t, 1-based
  void update(Node &t, const int index) {
    if (index > lengthofnodes) return;
    file_main.open(main_name, std::ofstream::out | std::ifstream::in);
    file_main.seekp(info_num * sizeof(pair<KV,int>) + (index - 1) * sizeof(Node));
    file_main.write(reinterpret_cast<char *> (&t), sizeof(Node));
    file_main.close();
  }

  //t = main[index], 1-based
  void read(Node &t, const int index) {
    if (index > lengthofnodes) return;
    file_main.open(main_name, std::ifstream::in);
    file_main.seekg(info_num * sizeof(pair<KV,int>) + (index - 1) * sizeof(Node));
    file_main.read(reinterpret_cast<char *> (&t), sizeof(Node));
    file_main.close();
  }

  void print() {
    std::cout << "[lengthofnodes=" << lengthofnodes << ", lengthoflist=" << lengthoflist << "]\n";
    Node n;
    file_main.open(main_name, std::ifstream::in);
    for(auto i : list) {
      std::cout<<i.first.key<<" "<<i.first.value<<" "<<i.second<<'\n';
    }
    for(auto i : list) {
      file_main.seekg(info_num * sizeof(pair<KV,int>) + (i.second - 1) * sizeof(Node));
      file_main.read(reinterpret_cast<char *> (&n), sizeof(Node));
      n.print();
    }
    file_main.close();
  }

  //find the first node that > the given key, then insert the given key before it
  void insert_pair(char *_key, int _val) {
    KV in_pair(_key, _val);

    auto it=list.lower_bound(in_pair);
    auto last=it;
    it--;

    if(it==list.begin()) {
      if(lengthoflist==2) {
        first_node(_key,_val);
      }
      else {
        Node next_node;
        read(next_node,(*last).second);
        list.erase(next_node.first);
        next_node.insert(in_pair);
        update(next_node,(*last).second);
        list.insert(pair<KV,int>(next_node.first,next_node.pos));
        if (next_node.size >= sqn - 20) {
          divide_node((*last).second);
        }
      }
    } else {
      Node next_node;
      read(next_node,(*it).second);
      next_node.insert(in_pair);
      update(next_node,(*it).second);
      if (next_node.size >= sqn - 20) {
        divide_node((*it).second);
      }
    }
    //print();
  }

  //divide the node at pos into 2 parts, the other part is at the tail
  void divide_node(int pos) {
    Node node;
    read(node, pos);

    Node new_node(node.data[node.size / 2].key, node.data[node.size / 2].value, lengthofnodes + 1);

    for (int i = node.size / 2; i < node.size; i++) {
      new_node.data[i - node.size / 2] = node.data[i];
      new_node.size++;
    }
    node.size /= 2;
    list.insert(pair<KV,int>(new_node.data[0],lengthofnodes + 1));
    update(node, pos);
    write(new_node);
  }

  //insert the Node before the Node at pos
  void first_node(char _key[70], int _val) {
    KV in_pair(_key,_val);
    Node new_node(_key, _val, lengthofnodes + 1);
    new_node.size = 1;
    list.insert(pair<KV,int>(in_pair,lengthofnodes + 1));
    write(new_node);
  }

  void remove_pair(char _key[], int _val) {
    Node first_node;
    KV in_pair(_key, _val);
    auto del=list.upper_bound(in_pair);
    auto it=del;
    del--;
    Node node;
    if((*del).second!=1&&(*del).second!=2){
      read(node, (*del).second);
      list.erase(node.first);
      node.remove(in_pair);
      update(node, (*del).second);
      if(node.size!=0) list.insert(pair<KV,int>(node.first,node.pos));
      else {
        lengthoflist--;
      }
    }

  }


  void get_key(char _key[]) {
    KV lowest(_key,INT_MIN),highest(_key,INT_MAX);
    auto beg=list.lower_bound(lowest);
    beg--;
    auto end=list.upper_bound(highest);
    Node node;
    bool exist=false;
    for(auto it=beg;it!=end;it++) {
      read(node,(*it).second);
      node.get(_key,exist);
    }
    if(!exist) std::cout<<"null";
    std::cout<<'\n';
  }

};


#endif //BOOKSTORE_PRE_BOOKS_H
