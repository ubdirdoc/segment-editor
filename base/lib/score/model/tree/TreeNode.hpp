#pragma once
#include <score/serialization/DataStreamVisitor.hpp>
#include <score/serialization/JSONVisitor.hpp>
#include <list>
/**
 * @brief Base type for a tree data structure.
 *
 * This class adds a tree structure around a data type.
 * It can then be used in abstract item models easily.
 */

template<typename T>
auto& child_at(const std::list<T>& list, int index)
{
  SCORE_ASSERT(index >= 0 && index < list.size());
  auto it = list.begin();
  std::advance(it, index);
  return *it;
}

template<typename T>
auto& child_at(std::list<T>& list, int index)
{
  SCORE_ASSERT(index >= 0 && index < list.size());
  auto it = list.begin();
  std::advance(it, index);
  return *it;
}

template<typename T>
int index_of_child(const std::list<T>& list, const T* child)
{
  int i = 0;
  const auto end = list.end();
  for(auto it = list.begin(); it != end; ++it)
  {
    if(&(*it) == child)
      return i;
    i++;
  }

  return -1;
}

template <typename DataType>
class TreeNode : public DataType
{
private:
  TreeNode* m_parent{};
  std::list<TreeNode> m_children;
  using impl_type = std::list<TreeNode>;

public:
  using iterator = typename impl_type::iterator;
  using const_iterator = typename impl_type::const_iterator;

  auto begin()
  {
    return m_children.begin();
  }
  auto begin() const
  {
    return cbegin();
  }
  auto cbegin() const
  {
    return m_children.cbegin();
  }

  auto end()
  {
    return m_children.end();
  }
  auto end() const
  {
    return cend();
  }
  auto cend() const
  {
    return m_children.cend();
  }

  TreeNode() = default;

  // The parent has to be set afterwards.
  TreeNode(const TreeNode& other)
      : DataType(static_cast<const DataType&>(other))
      , m_parent{other.m_parent}
      , m_children(other.m_children)
  {
    for (auto& child : m_children)
      child.setParent(this);
  }

  TreeNode(TreeNode&& other)
      : DataType(std::move(static_cast<DataType&&>(other)))
      , m_parent{other.m_parent}
      , m_children(std::move(other.m_children))
  {
    for (auto& child : m_children)
      child.setParent(this);
  }

  TreeNode& operator=(const TreeNode& source)
  {
    static_cast<DataType&>(*this) = static_cast<const DataType&>(source);
    m_parent = source.m_parent;

    m_children = source.m_children;
    for (auto& child : m_children)
    {
      child.setParent(this);
    }

    return *this;
  }

  TreeNode& operator=(TreeNode&& source)
  {
    static_cast<DataType&>(*this) = static_cast<DataType&&>(source);
    m_parent = source.m_parent;

    m_children = std::move(source.m_children);
    for (auto& child : m_children)
    {
      child.setParent(this);
    }

    return *this;
  }

  TreeNode(DataType data, TreeNode* parent)
      : DataType(std::move(data)), m_parent{parent}
  {
  }

  // Clone
  explicit TreeNode(TreeNode source, TreeNode* parent)
      : TreeNode{std::move(source)}
  {
    m_parent = parent;
  }

  void push_back(const TreeNode& child)
  {
    m_children.push_back(child);

    auto& cld = m_children.back();
    cld.setParent(this);
  }

  void push_back(TreeNode&& child)
  {
    m_children.push_back(std::move(child));

    auto& cld = m_children.back();
    cld.setParent(this);
  }

  template <typename... Args>
  auto& emplace_back(Args&&... args)
  {
    m_children.emplace_back(std::forward<Args>(args)...);

    auto& cld = m_children.back();
    cld.setParent(this);
    return cld;
  }

  template <typename... Args>
  auto& emplace(Args&&... args)
  {
    auto& n = *m_children.emplace(std::forward<Args>(args)...);
    n.setParent(this);
    return n;
  }

  TreeNode* parent() const
  {
    return m_parent;
  }

  bool hasChild(std::size_t index) const
  {
    return m_children.size() > index;
  }

  TreeNode& childAt(int index)
  {
    return child_at(m_children, index);
  }

  const TreeNode& childAt(int index) const
  {
    return child_at(m_children, index);
  }

  // returns -1 if not found
  int indexOfChild(const TreeNode* child) const
  {
    return index_of_child(m_children, child);
  }

  auto iterOfChild(const TreeNode* child)
  {
    const auto end = m_children.end();
    for (auto it = m_children.begin(); it != end; ++it)
    {
      if (&*it == child)
        return it;
    }
    return end;
  }

  int childCount() const
  {
    return m_children.size();
  }

  bool hasChildren() const
  {
    return !m_children.empty();
  }

  const auto& children() const
  {
    return m_children;
  }
  void reserve(std::size_t s)
  {
    //m_children.reserve(s);
  }
  void resize(std::size_t s)
  {
    m_children.resize(s);
  }

  auto erase(const_iterator it)
  {
    return m_children.erase(it);
  }

  auto erase(const_iterator it_beg, const_iterator it_end)
  {
    return m_children.erase(it_beg, it_end);
  }

  void setParent(TreeNode* parent)
  {
    m_parent = parent;
  }

  template <typename Fun>
  void visit(Fun f) const
  {
    f(*this);

    for (const auto& child : m_children)
    {
      child.visit(f);
    }
  }
};

template <typename T>
struct TSerializer<DataStream, TreeNode<T>>
{
  static void readFrom(DataStream::Serializer& s, const TreeNode<T>& n)
  {
    s.readFrom(static_cast<const T&>(n));

    s.stream() << n.childCount();
    for (const auto& child : n)
    {
      s.readFrom(child);
    }

    s.insertDelimiter();
  }

  static void writeTo(DataStream::Deserializer& s, TreeNode<T>& n)
  {
    s.writeTo(static_cast<T&>(n));

    int childCount;
    s.stream() >> childCount;
    for (int i = 0; i < childCount; ++i)
    {
      TreeNode<T> child;
      s.writeTo(child);
      n.push_back(std::move(child));
    }

    s.checkDelimiter();
  }
};

template <typename T>
struct TSerializer<JSONObject, TreeNode<T>>
{
  static void readFrom(JSONObject::Serializer& s, const TreeNode<T>& n)
  {
    s.readFrom(static_cast<const T&>(n));
    s.obj[s.strings.Children] = toJsonArray(n);
  }

  static void writeTo(JSONObject::Deserializer& s, TreeNode<T>& n)
  {
    s.writeTo(static_cast<T&>(n));
    auto children = s.obj[s.strings.Children].toArray();
    for (const auto& val : children)
    {
      TreeNode<T> child;
      JSONObject::Deserializer nodeWriter(val.toObject());

      nodeWriter.writeTo(child);
      n.push_back(std::move(child));
    }
  }
};
