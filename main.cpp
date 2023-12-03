#include <iostream>
#include <fstream>
#include "books.h"

using std::cout;
using std::endl;

int main() {
  NodeMemory mr;
  mr.initialise("aux");
  char s[70];
  int n;
  std::string op;
  int val;
  //std::cout<<s<<'\n';
  while (n--) {
    std::cin >> op;
    if (op == "print") {
      mr.print();
      continue;
    }
    else if (op == "quit") break;
    std::cin >> s;
    if (op == "insert") {
      std::cin >> val;
      mr.insert_pair(s, val);
    } else if (op == "delete") {
      std::cin >> val;
      mr.remove_pair(s, val);
    } else if (op == "find") {
      mr.get_key(s);
    }
    //mr.print();
  }
  return 0;
}
