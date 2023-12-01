#ifndef BOOKSTORE_PRE_BOOKS_H
#define BOOKSTORE_PRE_BOOKS_H

#include <iostream>
#include <cstring>
#include <fstream>

using std::string;
using std::fstream;
using std::ifstream;
using std::ofstream;

const int sqn=400;

class Node;
class KV;
class NodeMemory;

class KV{
public:
  char key[70]="";
  int value=0;
  KV()= default;
  KV(char _key[],int _value) {
    strcpy(key,_key);
    value=_value;
  }
};

class KVblock {
public:
  KV data[sqn];
  KVblock()= default;
  void insert(KV &kv) {}
};

class Node {

public:

    char key[70]="";
    int size=0;
    int pos=0;
    int pre=0;//position in the main file, 1-based
    int next=0;

    Node()= default;

    Node(const char _key[70], int _pos) {
      strcpy(key,_key);
      //std::cout<<key<<'\n';
      pos=_pos;
      size=0;
      pre=0;
      next=0;
    }

    void print(fstream &file, const std::string& file_name) {
      file.open(file_name, ifstream::in);
      if (!file.is_open()) {
        std::cout << "d\n";
        file.close();
        return;
      }
      std::cout << key << ":\n";
      KV val;
      if (pos >= 3) {
        file.seekg((pos - 3) * 400 * sizeof(KV));
      } else {
        std::cout << "   null\n";
        file.close();
        return;
      }
      for (int i = 0; i < size; i++) {
        file.read(reinterpret_cast<char *>(&val), sizeof(KV));
        std::cout << "   " << val.key<<"   "<<val.value<<'\n';
      }
      file.close();
    }

};//stored in the auxiliary file



class NodeMemory {

    //in the auxiliary file,
    //the first int: current number of books
    //the first 2 Nodes: head node, tail node
    //then it stores every Node

private:

    fstream file_aux,file_main;
    string aux_name,main_name;//file_aux
    int lengthofT = 0;
    int cursize=0;

public:

    NodeMemory() = default;

    NodeMemory(const string &_aux_name,const string &_main_name) : aux_name(_aux_name),main_name(_main_name) {}

    void initialise(const string &FN = "",const string &MN = "") {
      if (FN != "") aux_name = FN;
      if (MN != "") main_name = MN;
      file_aux.open(aux_name, std::ios::in);
      file_main.open(main_name, std::ios::in);
      if (!file_aux.is_open()||!file_main.is_open())
      {
        std::cout << "create new\n";
        file_aux.close();
        file_main.close();
        file_aux.open(aux_name, fstream::out|fstream::binary);
        file_main.open(main_name, fstream::out|fstream::binary);
        if (!file_main.is_open()||!file_aux.is_open()) {
          std::cout << "d234d\n";
        }
        int tmp=2;
        char st[70]="",ed[70]="zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz";
        Node head(st,1),tail(ed,2);
        head.pre=0,head.next=2;
        tail.pre=1,tail.next=0;
        for (int i = 0; i < 2; ++i){
          file_aux.write(reinterpret_cast<char *>(&tmp), sizeof(int));
          tmp-=2;
        }
        lengthofT=2;cursize=0;
        file_aux.write(reinterpret_cast<char*>(&head),sizeof(Node));
        file_aux.write(reinterpret_cast<char*>(&tail),sizeof(Node));
      } else {
        int l,c;
        file_aux.read(reinterpret_cast<char*>(&l),sizeof(int));
        file_aux.read(reinterpret_cast<char*>(&c),sizeof(int));
        lengthofT=l,cursize=c;
      }
      file_aux.close();
      file_main.close();
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
      std::cout<<"[lengthofT="<<lengthofT<<", cursize="<<cursize<<"]\n";
      int tmp=1;
      Node n;
      while(tmp!=2) {
        read(n,tmp);
        std::cout<<"pos="<<tmp<<", ";
        std::cout<<"n.pos="<<n.pos<<", ";
        std::cout<<"pre="<<n.pre<<", ";
        std::cout<<"next="<<n.next<<"| ";
        tmp=n.next;
        n.print(file_main,main_name);
      }
      read(n,tmp);
      std::cout<<"pos="<<tmp<<", ";
      std::cout<<"n.pos="<<n.pos<<", ";
      std::cout<<"pre="<<n.pre<<", ";
      std::cout<<"next="<<n.next<<"| ";
      tmp=n.next;
      n.print(file_main,main_name);
    }

  //find the first node that >= the given key, then insert the given key before it
  void insert(char *_key, int value) {
    Node next_node;
    int tmp=1;
    while(true) {
      read(next_node,tmp);
      std::cout<<tmp<<":"<<_key<<"||"<<next_node.key<<"||"<<next_node.pos<<"||"<<next_node.next<<'\n';
      if(strcmp(_key,next_node.key)<=0) break;// if key<=next_node.key then break
      tmp=next_node.next;
    }//next_node: the first node whose key >= the given key
    //tmp: the position of next_node

    std::cout<<tmp<<'\n';
    cursize++;
    write_info(cursize,2);
    if(strcmp(next_node.key,_key)==0) {

      while(strcmp(next_node.key,_key)==0) {
        if((next_node.size+1)*(next_node.size+1)<=cursize) break;
        tmp=next_node.next;
        read(next_node,tmp);
      }
      std::cout<<tmp<<'\n';
      // next_node: the first node that has a different key / enough storage
      // tmp: the position of next_node
      // we should insert the given data int next_node,
      // or insert a new Node before next_node

      if(strcmp(next_node.key,_key)==0) {
        std::cout<<"insert\n";
        next_node.size++;
        update(next_node,next_node.pos);
        KV t=KV(_key,value);
        file_main.open(main_name, std::ofstream::out | std::ifstream::in);
        file_main.seekp((next_node.pos-3)*400*sizeof(KV)+(next_node.size-1)*sizeof(KV));
        file_main.write(reinterpret_cast<char*> (&t),sizeof(KV));
        file_main.close();
      } else {
        std::cout<<"divide\n";
        insert_node(_key,next_node.pos);
        KV t=KV(_key,value);
        file_main.open(main_name, std::ofstream::out | std::ifstream::in);
        file_main.seekp((lengthofT-3)*400*sizeof(KV));
        file_main.write(reinterpret_cast<char*> (&t),sizeof(KV));
        file_main.close();
      }
    } else {
      std::cout<<"new Node\n";
      insert_node(_key,next_node.pos);
      KV t=KV(_key,value);
      file_main.open(main_name, std::ofstream::out | std::ifstream::in);
      file_main.seekp((lengthofT-3)*400*sizeof(KV));
      file_main.write(reinterpret_cast<char*> (&t),sizeof(KV));
      file_main.close();
    }
  }

  //insert the Node before the Node at pos
  void insert_node(char _key[70],int pos ) {
    Node next_node,pre_node;
    read(next_node,pos);
    read(pre_node,next_node.pre);
    std::cout<<pre_node.pos<<" "<<next_node.pos<<'\n';
    Node ins(_key,lengthofT+1);
    ins.next=pos;
    ins.pre=next_node.pre;
    next_node.pre=lengthofT+1;
    pre_node.next=lengthofT+1;
    ins.size=1;
    std::cout<<ins.pre<<" "<<ins.next<<'\n';
    std::cout<<pre_node.next<<" "<<next_node.pre<<'\n';
    update(pre_node,pre_node.pos);
    update(next_node,next_node.pos);
    write(ins);
    write_info(lengthofT,1);
    file_main.open(main_name,ofstream::app);
    char s[70];
    KV a(s,0);
    for(int i=0;i<sqn;i++)
      file_main.write(reinterpret_cast<char *> (&a),sizeof(KV));
    file_main.close();
    //ins.print(file_main,main_name);
  }

  //删除位置索引index对应的对象(不涉及空间回收时，可忽略此函数)，保证调用的index都是由write函数产生
  void Delete(int index) {}
};


#endif //BOOKSTORE_PRE_BOOKS_H
