#include <iostream>
#include "books.h"

using std::cin;

int main() {
  NodeMemory mr;
  mr.initialise("aux");
  char s[70];
  int n;
  std::string op;
  int val;
  while (n--) {
    cin >> op;
    if (op == "print") {
      mr.print();
      continue;
    }
    else if (op == "quit") break;
    cin >> s;
    if (op == "insert") {
      cin >> val;
      mr.insert_pair(s, val);
    } else if (op == "delete") {
      cin >> val;
      mr.remove_pair(s, val);
    } else if (op == "find") {
      mr.get_key(s);
    }
    //mr.print();
  }
  return 0;
}
