#include <iostream>
#include "books.h"

using std::cin;

int main() {
  NodeMemory nm;
  nm.initialise("ma");
  char s[70];
  int n;
  cin >> n;
  std::string op;
  int val;
  while (n--) {
    cin >> op;
    if (op == "print") {
      nm.print();
      continue;
    } else if (op == "quit") {
      break;
    } else {
      cin >> s;
      if (op == "insert") {
        cin >> val;
        nm.insert_pair(s, val);
      } else if (op == "delete") {
        cin >> val;
        nm.remove_pair(s, val);
      } else if (op == "find") {
        nm.get_key(s);
      } else if (op == "get") {
        cin >> val;
        Node node;
        nm.read(node, val);
        node.print();
      } else {
        continue;
      }
    }
  }
  nm.update_list();
  nm.update_aux();
  return 0;
}
