#include <iostream>
#include <fstream>
#include "books.h"

using std::cout;
using std::endl;

int main() {
  //ofstream out("aux",ios::out);
  //ifstream in;
  NodeMemory mr;
  mr.initialise("aux","maindata");
  ifstream in("aux",ifstream::in);
  if (!in.is_open()||!in.is_open()) {
    std::cout << "dddddd\n";
  }
  /*int i;
  Node n;
  in.read(reinterpret_cast<char *>(&i),sizeof(int));
  //cout<<i<<endl;
  in.read(reinterpret_cast<char *>(&i),sizeof(int));
  //cout<<i<<endl;
  in.read(reinterpret_cast<char *>(&n),sizeof(Node));
  cout<<n.key<<" "<<n.size<<" "<<n.pos<<" "<<n.pre<<" "<<n.next<<endl;
  in.read(reinterpret_cast<char *>(&n),sizeof(Node));
  cout<<n.key<<" "<<n.size<<" "<<n.pos<<" "<<n.pre<<" "<<n.next<<endl;*/
  char s[70]= {'b','s'};
  in.close();
  //std::cout<<s<<'\n';
  mr.insert(s,2543);

  mr.print();
  return 0;
  /*ofstream out("aux",ios::out);
  ofstream ou("main",ios::out);
  out.close();
  ou.close();*/
}