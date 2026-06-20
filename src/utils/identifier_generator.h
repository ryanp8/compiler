#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace utils {

class TreeNode {
public:
  TreeNode() {}
  bool contains(char c);
  TreeNode *get(char c);
  TreeNode *add_child(char c);
  bool is_end();

private:
  std::unordered_map<char, std::unique_ptr<TreeNode>> _children;
};

class PrefixTree {
public:
  PrefixTree();
  void insert(const std::string &s);
  bool starts_with(const std::string &s);
  bool search(const std::string &s);

private:
  std::unique_ptr<TreeNode> _root;
};

class IdentifierGenerator {
public:
  IdentifierGenerator(const std::string &);
  IdentifierGenerator(const std::string &, std::vector<std::string> &);
  void insert(const std::string &);
  std::string get();

private:
  std::string _name;
  size_t _count = 0;
  PrefixTree _tree;
};

} // namespace utils