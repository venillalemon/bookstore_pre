#ifndef BOOKSTORE_PRE_BOOKS_H
#define BOOKSTORE_PRE_BOOKS_H

#include <iostream>
#include <cstring>
#include <fstream>

using std::string;
using std::fstream;
using std::ifstream;
using std::ofstream;

const int sqn = 1000;

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
  int pre = 0;//position in the main file, 1-based
  int next = 0;
  KV data[sqn];

  Node() = default;

  Node(char _key[70], int _val, int _pos) {
    first = KV(_key, _val);
    //std::cout<<key<<'\n';
    pos = _pos;
    size = 0;
    pre = 0;
    next = 0;
  }

  void print() {
    std::cout << first.key << " " << first.value << ":\n";
    KV val;
    if (pos < 3) {
      std::cout << "   null\n";
    }
    for (int i = 0; i < size; i++) {
      val = data[i];
      std::cout << "   " << val.key << "   " << val.value << '\n';
    }
  }

  void insert(KV kv) {
    int i;
    for (i = 0; i < size; i++) {
      if (!(data[i] < kv)) break;
    }
    for (int j = size; j > i; j--) {
      data[j] = data[j - 1];
    }
    data[i] = kv;
    size++;
  }

  void remove(KV kv) {
    int i;
    int left = -1, right = -1;
    int status = 0;
    for (i = 0; i < size; i++) {
      if (status == 0 && data[i] == kv) {
        left = i;
        status = 1;
      }
      if (status == 1 && !(data[i] == kv)) {
        right = i;
        status = 2;
      }
    }
    if (status == 1) {
      size = left;
    } else if (status == 2) {
      int cnt = 0;
      while (right + cnt < size) {
        data[left + cnt] = data[right + cnt];
        cnt++;
      }
      size = left + cnt;
    }
    if (size > 0) first = data[0];
    else first = KV();
  }

  void get(char _key[], bool &exist) const {
    for (int i = 0; i < size; i++) {
      if (strcmp(data[i].key, _key) == 0) {
        exist = true;
        std::cout << data[i].value << ' ';
      }
    }
  }

};// stored in the auxiliary file



class NodeMemory {

  // in the auxiliary file,
  // the first int: current number of books
  // the first 2 Nodes: head node, tail node
  // then it stores every Node

private:

  fstream file_aux;
  string aux_name;//file_aux
  int lengthofT = 0;
  int cursize = 0;

public:

  NodeMemory() = default;

  NodeMemory(const string &_aux_name) : aux_name(_aux_name) {}

  void initialise(const string &FN = "") {
    if (FN != "") aux_name = FN;
    file_aux.open(aux_name, std::ios::in);
    if (!file_aux.is_open()) {
      //std::cout << "create new\n";
      file_aux.close();
      file_aux.open(aux_name, fstream::out | fstream::binary);
      if (!file_aux.is_open()) {
        std::cout << "error\n";
      }
      int tmp = 2;
      char st[70] = "", ed[70] = "zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz";
      Node head(st, 0, 1), tail(ed, 0, 2);
      head.pre = 0, head.next = 2;
      tail.pre = 1, tail.next = 0;
      for (int i = 0; i < 2; ++i) {
        file_aux.write(reinterpret_cast<char *>(&tmp), sizeof(int));
        tmp -= 2;
      }
      lengthofT = 2;
      cursize = 0;
      file_aux.write(reinterpret_cast<char *>(&head), sizeof(Node));
      file_aux.write(reinterpret_cast<char *>(&tail), sizeof(Node));
    } else {
      int l, c;
      file_aux.read(reinterpret_cast<char *>(&l), sizeof(int));
      file_aux.read(reinterpret_cast<char *>(&c), sizeof(int));
      lengthofT = l, cursize = c;
    }
    file_aux.close();
  }

  //读出aux中第n个int的值赋给tmp，1_base
  void get_info(int &tmp, int n) {
    if (n > 2) return;
    file_aux.open(aux_name, std::ifstream::in);
    file_aux.seekp((n - 1) * sizeof(int));
    file_aux.read(reinterpret_cast<char *>(&tmp), sizeof(int));
    file_aux.close();
  }

  //将tmp写入aux中第n个int的位置，1_base
  void write_info(int tmp, int n) {
    if (n > 2) return;
    file_aux.open(aux_name, std::ofstream::out | std::ifstream::in);
    file_aux.seekg((n - 1) * sizeof(int), std::ios::beg);
    file_aux.write(reinterpret_cast<char *>(&tmp), sizeof(int));
    file_aux.close();
  }

  //在文件aux合适位置写入类对象t，并返回写入的位置索引index (1-based)
  //位置索引意味着当输入正确的位置索引index，在以下三个函数中都能顺利的找到目标对象进行操作
  //位置索引index可以取为对象写入的起始位置
  int write(Node &t) {
    file_aux.open(aux_name, std::ofstream::app);
    file_aux.write(reinterpret_cast<char *>(&t), sizeof(Node));
    file_aux.close();
    ++lengthofT;
    write_info(lengthofT, 1);
    return lengthofT;
  }

  //用t的值更新位置索引index对应的对象，保证调用的index都是由write函数产生, 1-based
  void update(Node &t, const int index) {
    if (index > lengthofT) return;
    file_aux.open(aux_name, std::ofstream::out | std::ifstream::in);
    file_aux.seekp(2 * sizeof(int) + (index - 1) * sizeof(Node));
    file_aux.write(reinterpret_cast<char *> (&t), sizeof(Node));
    file_aux.close();
  }

  //读出位置索引index对应的T对象的值并赋值给t，保证调用的index都是由write函数产生
  void read(Node &t, const int index) {
    if (index > lengthofT) return;
    file_aux.open(aux_name, std::ifstream::in);
    file_aux.seekg(2 * sizeof(int) + (index - 1) * sizeof(Node));
    file_aux.read(reinterpret_cast<char *> (&t), sizeof(Node));
    file_aux.close();
  }

  void print() {
    std::cout << "[lengthofT=" << lengthofT << ", cursize=" << cursize << "]\n";
    int tmp = 1;
    Node n;
    while (tmp != 2) {
      read(n, tmp);
      std::cout << "pos=" << tmp << ", ";
      std::cout << "n.pos=" << n.pos << ", ";
      std::cout << "pre=" << n.pre << ", ";
      std::cout << "next=" << n.next << "| ";
      tmp = n.next;
      n.print();
    }
    read(n, tmp);
    std::cout << "pos=" << tmp << ", ";
    std::cout << "n.pos=" << n.pos << ", ";
    std::cout << "pre=" << n.pre << ", ";
    std::cout << "next=" << n.next << "| ";
    tmp = n.next;
    n.print();
  }

  //find the first node that > the given key, then insert the given key before it
  void insert_pair(char *_key, int _val) {
    KV in_pair(_key, _val);
    Node next_node;
    int tmp = 1;
    while (true) {
      read(next_node, tmp);
      //std::cout<<tmp<<":"<<_key<<"||"<<next_node.first.key<<"||"<<next_node.pos<<"||"<<next_node.next<<'\n';
      if (in_pair < next_node.first) break;// if key<=next_node.key then break
      tmp = next_node.next;
    }//next_node: the first node whose key > the given key
    tmp = next_node.pre;
    read(next_node, tmp);
    //next_node: the last node whose key <= the given key
    //tmp: the position of next_node
    //std::cout<<tmp<<'\n';
    cursize++;
    write_info(cursize, 2);

    if (lengthofT == 2) {
      insert_node(_key, _val, next_node.next);
      read(next_node, lengthofT);
      next_node.data[0] = KV(_key, _val);
      update(next_node, lengthofT);
    } else if (tmp == 1) {
      tmp = next_node.next;
      read(next_node, tmp);
      next_node.insert(KV(_key, _val));
      next_node.first = KV(_key, _val);
      update(next_node, next_node.pos);
    } else {
      next_node.insert(KV(_key, _val));
      update(next_node, tmp);
    }

    //print();
    //std::cout<<next_node.size*next_node.size<<" "<<1.8*cursize<<'\n';
    if (next_node.size * next_node.size >= 1.8 * cursize) {
      divide_node(tmp);
    }
  }

  //divide the node at pos into 2 parts, the other part is at the tail
  void divide_node(int pos) {
    Node node, next_node;
    read(node, pos);
    read(next_node, node.next);

    Node new_node(node.data[node.size / 2].key, node.data[node.size / 2].value, lengthofT + 1);

    for (int i = node.size / 2; i < node.size; i++) {
      new_node.data[i - node.size / 2] = node.data[i];
      new_node.size++;
    }
    node.size /= 2;
    new_node.pre = pos;
    new_node.next = node.next;
    node.next = lengthofT + 1;
    update(node, pos);
    next_node.pre = lengthofT + 1;
    update(next_node, next_node.pos);
    write(new_node);
  }

  //insert the Node before the Node at pos
  void insert_node(char _key[70], int _val, int pos) {
    Node next_node, pre_node;
    read(next_node, pos);
    read(pre_node, next_node.pre);

    //std::cout<<pre_node.pos<<" "<<next_node.pos<<'\n';
    Node new_node(_key, _val, lengthofT + 1);
    new_node.next = pos;
    new_node.pre = next_node.pre;
    next_node.pre = lengthofT + 1;
    pre_node.next = lengthofT + 1;
    new_node.size = 1;
    //std::cout<<new_node.pre<<" "<<new_node.next<<'\n';
    //std::cout<<pre_node.next<<" "<<next_node.pre<<'\n';
    update(pre_node, pre_node.pos);
    update(next_node, next_node.pos);
    write(new_node);
    write_info(lengthofT, 1);
  }

  void remove_pair(char _key[], int _val) {
    Node first_node;
    KV in_pair(_key, _val);
    int first = 1;
    while (true) {
      read(first_node, first);
      //std::cout<<first<<":"<<_key<<"||"<<first_node.first.key<<"||"<<first_node.pos<<"||"<<first_node.next<<'\n';
      if (!(first_node.first < in_pair)) break;// if key<=first_node.key then break
      first = first_node.next;
    }//first_node: the first node whose key >= the given key
    first = first_node.pre;
    read(first_node, first);
    //first_node: the last node whose key < the given key
    //first: the position of first_node
    //if(first==1) return;
    Node last_node;
    int last = first;
    while (true) {
      read(last_node, last);
      //std::cout<<last<<":"<<_key<<"||"<<last_node.first.key<<"||"<<last_node.pos<<"||"<<last_node.next<<'\n';
      if (in_pair < last_node.first) break;// if key<last_node.key then break
      cursize -= last_node.size;
      last_node.remove(KV(_key, _val));
      update(last_node, last);
      cursize += last_node.size;
      write_info(cursize, 2);
      if (last != 1 && last_node.size == 0) {
        Node pre_node, next_node;
        read(pre_node, last_node.pre);
        read(next_node, last_node.next);
        pre_node.next = last_node.next;
        next_node.pre = last_node.pre;
        update(pre_node, pre_node.pos);
        update(next_node, next_node.pos);
        update(last_node, last_node.pos);
      }
      last = last_node.next;
    }// normally,there exists one and only one pair to be removed
  }


  void get_key(char _key[]) {
    Node first_node;
    int first = 1;
    while (true) {
      read(first_node, first);
      //std::cout<<first<<":"<<_key<<"||"<<first_node.first.key<<"||"<<first_node.pos<<"||"<<first_node.next<<'\n';
      if (strcmp(_key, first_node.first.key) <= 0) break;// if key<=first_node.key then break
      first = first_node.next;
    }//first_node: the first node whose key >= the given key
    first = first_node.pre;
    read(first_node, first);
    //first_node: the last node whose key < the given key
    //first: the position of first_node
    Node last_node;
    int last = first;
    bool exist = false;
    while (true) {
      read(last_node, last);
      //std::cout<<last<<":"<<_key<<"||"<<last_node.first.key<<"||"<<last_node.pos<<"||"<<last_node.next<<'\n';
      if (strcmp(_key, last_node.first.key) < 0) break;// if key<last_node.key then break
      last_node.get(_key, exist);
      last = last_node.next;
    }
    if (!exist) std::cout << "null";
    std::cout << '\n';
  }

};


#endif //BOOKSTORE_PRE_BOOKS_H
