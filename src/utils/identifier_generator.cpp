#include <string>
#include <vector>

#include "identifier_generator.h"

namespace utils {
bool TreeNode::contains(char c) { return _children.find(c) != _children.end(); }
TreeNode *TreeNode::get(char c) {
  if (!contains(c)) {
    return nullptr;
  }
  return _children[c].get();
}

TreeNode *TreeNode::add_child(char c) {
  _children[c] = std::make_unique<TreeNode>();
  return _children[c].get();
}
bool TreeNode::is_end() { return contains('\0'); }

PrefixTree::PrefixTree() { _root = std::make_unique<TreeNode>(); }
void PrefixTree::insert(const std::string &s) {
  auto cur = _root.get();
  for (char c : s) {
    if (!cur->contains(c)) {
      cur->add_child(c);
    }
    cur = cur->get(c);
  }
  cur->add_child('\0');
}
bool PrefixTree::starts_with(const std::string &s) {
  auto cur = _root.get();
  for (char c : s) {
    if (!cur->contains(c)) {
      return false;
    }
    cur = cur->get(c);
  }
  return true;
}
bool PrefixTree::search(const std::string &s) {
  auto cur = _root.get();
  for (char c : s) {
    if (!cur->contains(c)) {
      return false;
    }
    cur = cur->get(c);
  }
  return cur->is_end();
}

IdentifierGenerator::IdentifierGenerator(const std::string &name) : _name(name) {}
IdentifierGenerator::IdentifierGenerator(const std::string &name,
                             std::vector<std::string> &existing)
    : _name(name) {
  for (auto &s : existing) {
    _tree.insert(s);
  }
}

void IdentifierGenerator::insert(const std::string &s) {
  _tree.insert(s);
  while (_tree.starts_with(_name)) {
    _name += "_hi_";
  }
}

std::string IdentifierGenerator::get() { return _name + std::to_string(_count++); }

} // namespace utils