// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "SelectionStack.hpp"

#include <score/model/IdentifiedObjectAbstract.hpp>
#include <score/selection/FocusManager.hpp>
#include <score/selection/Selectable.hpp>
#include <score/selection/Selection.hpp>
#include <score/tools/Todo.hpp>

#include <QList>
#include <QPointer>
#include <QVector>
#include <qnamespace.h>

#include <wobjectimpl.h>

#include <algorithm>
W_OBJECT_IMPL(score::SelectionStack)
W_OBJECT_IMPL(Selectable)
W_OBJECT_IMPL(score::FocusManager)
namespace score
{
SelectionStack::SelectionStack()
{
  connect(
      this, &SelectionStack::pushNewSelection, this, &SelectionStack::push);
  m_unselectable.push(Selection{});
}

bool SelectionStack::canUnselect() const
{
  return m_unselectable.size() > 1;
}

bool SelectionStack::canReselect() const
{
  return !m_reselectable.empty();
}

void SelectionStack::clear()
{
  m_unselectable.clear();
  m_reselectable.clear();
  m_unselectable.push(Selection{});
  currentSelectionChanged(m_unselectable.top());
}

void SelectionStack::clearAllButLast()
{
  Selection last;
  if (canUnselect())
    last = m_unselectable.top();

  m_unselectable.clear();
  m_reselectable.clear();
  m_unselectable.push(Selection{});
  m_unselectable.push(std::move(last));
}

void SelectionStack::push(const Selection& selection)
{
  m_unselectable.clear();
  m_reselectable.clear();
  m_unselectable.push(Selection{});
  // TODO don't push "empty" selections, just add a "deselected" mode.
  if (selection != m_unselectable.top())
  {
    auto s = selection;
    auto it = s.begin();
    while (it != s.end())
    {
      if (*it)
        ++it;
      else
        it = s.erase(it);
    }

    Foreach(s, [&](auto obj) {
      // TODO we should erase connections once the selected objects aren't in
      // the stack anymore.
      connect(
          obj,
          &IdentifiedObjectAbstract::identified_object_destroyed,
          this,
          &SelectionStack::prune,
          Qt::UniqueConnection);
    });

    m_unselectable.push(s);

    currentSelectionChanged(s);
  }
}

void SelectionStack::unselect()
{
  clear();
}

void SelectionStack::reselect()
{
  clear();
}

void SelectionStack::deselect()
{
  clear();
}

Selection SelectionStack::currentSelection() const
{
  return canUnselect() ? m_unselectable.top() : Selection{};
}

void SelectionStack::prune(IdentifiedObjectAbstract* p)
{
  clear();
}
} // namespace score
