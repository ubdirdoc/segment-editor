#pragma once
#include <score/model/IdentifiedObject.hpp>
#include <score/selection/Selection.hpp>

struct VisitorVariant;

namespace score
{
class SCORE_LIB_BASE_EXPORT DocumentDelegateModel
    : public IdentifiedObject<DocumentDelegateModel>
{
  W_OBJECT(DocumentDelegateModel)
public:
  using IdentifiedObject<DocumentDelegateModel>::IdentifiedObject;
  virtual ~DocumentDelegateModel();

  virtual void serialize(const VisitorVariant&) const = 0;
  virtual void savedDocumentAs(const QString& origPath, const QString& newPath) = 0;
};
} // namespace score
