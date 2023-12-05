#include <iostream>
#include "books.h"

using std::cin;

int main() {
  NodeMemory mr;
  mr.initialise("ma");
  char s[70];
  int n;
  cin >> n;
  std::string op;
  int val;
  while (n--) {
    cin >> op;
    if (op == "print") {
      mr.print();
      continue;
    } else if (op == "quit") {
      break;
    } else {
      cin >> s;
      if (op == "insert") {
        cin >> val;
        mr.insert_pair(s, val);
      } else if (op == "delete") {
        cin >> val;
        mr.remove_pair(s, val);
      } else if (op == "find") {
        mr.get_key(s);
      } else if (op == "get") {
        cin >> val;
        Node node;
        mr.read(node, val);
        node.print();
      } else {
        continue;
      }
    }
    //mr.print();
  }
  mr.update_list();
  mr.update_aux();
  return 0;
}
