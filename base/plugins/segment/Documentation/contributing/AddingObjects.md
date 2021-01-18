# Adding a new kind of object

e.g. imagine that we want to add a new kind of zone inside scenes, 
like back click areas, etc.

Let's call it BlobArea.

## 1. Create a data model for it.

Header: 

```C++
namespace SEGMent
{
// Inheriting form Object<T> will give the default set of properties:
// position, size, sound, name, etc...
class BlobAreaModel : public Object<BlobAreaModel>
{
  // Used for reflection
  W_OBJECT(BlobAreaModel)
  
  // Used for serialization
  SCORE_SERIALIZE_FRIENDS
public:
  using base_type = Object<BlobAreaModel>;
  
  // Used to make the object selectable
  Selectable selection{this};

  // New object constructor
  BlobAreaModel(const Id<BlobAreaModel>& id, QObject* parent);

  // Constructor used when loading
  template <typename DeserializerVisitor>
  BlobAreaModel(DeserializerVisitor&& vis, QObject* parent)
      : base_type{vis, parent}
  {
    vis.writeTo(*this);
  }

  // Add additional properties as you see fit. 
  // See AddingProperties.md 
};
} 

// Name used as an internal identifier
DEFAULT_MODEL_METADATA(SEGMent::BlobAreaModel, "BlobArea")

// Name used in various places shown to the user
TR_TEXT_METADATA(, SEGMent::BlobAreaModel, PrettyName_k, "Blob area")
```

Implementation:

```C++
#include "BlobArea.hpp"

// Reflection
#include <wobjectimpl.h>
W_OBJECT_IMPL(SEGMent::BlobAreaModel)

namespace SEGMent
{
// Constructor
BlobAreaModel::BlobAreaModel(
    const Id<BlobAreaModel>& id,
    QObject* parent)
    : base_type{id, "BlobArea", parent}
{
}
} 

// Serialization code - by default nothing to serialize.
// All the common metadata is saved / reloaded automatically by the parent classes.
template <>
void DataStreamReader::read(const SEGMent::BlobAreaModel& v)
{
  insertDelimiter();
}

template <>
void DataStreamWriter::write(SEGMent::BlobAreaModel& v)
{
  checkDelimiter();
}

template <>
void JSONObjectReader::read(const SEGMent::BlobAreaModel& v)
{
}

template <>
void JSONObjectWriter::write(SEGMent::BlobAreaModel& v)
{
}
```


## 2. Create an UI class

See for instance `ClickWindow.hpp`: BackClickWindow is a good starting point to make your own.
Let's call our new class `BlobWindow`.

## 3. Add it to the scene model

In `SceneModel` (hpp): 
- Add a `score::EntityMap<BlobAreaModel> blobAreas;` member.
  This will contain all the children blob elements.

In `SceneModel` (cpp): 
- Add serialization code in 
```
void JSONObjectReader::read(const SEGMent::SceneModel& v) { ... }
void JSONObjectWriter::write(SEGMent::SceneModel& v) { ... } 
```
following the pattern for objects, gifs, etc.
There is a way to refactor it for that to be done automatically but I haven't gotten around it yet.

## 4. Bind the model and the UI class

Add the class in relevant places in `SEGMent/Accessors.hpp` and `SEGMent/Visitors.hpp`.

```C++
using SceneChildrenTypes = std::tuple<
  ..., BlobAreaModel, ...
>;

template <>
struct SceneAccessor<BlobAreaModel>
{
  using view = BlobWindow;
  static auto& get(SceneModel& scene) { return scene.blobAreas; }
  static auto& get(const SceneModel& scene) { return scene.blobAreas; }
};

etc...
```

Also add a macro with the id in SEGMent/GlobalVariables.hpp, and maybe add a color style in that file too.

To make sure that the binding works, at this step one can add a fake new BlobAreaModel in the SEGMent::ProcessModel 
constructor for instance : it should show up on screen when creating a scene.

## 5. Making it addable by the user

Let's do like the others: by dragging it from the inspector on the left.

1. Create a new button in `SEGMent/Panel/Library.cpp`, in the `Library::Library` constructor.

e.g.
```C++
lay->addWidget(
    new DraggableButton{tr("Blob Area"), SEGMENT_BLOBAREA_ID, m_zones});
```

2. In `Creation.hpp`, add a new undo-redo command that will create the item and set it up.
See `DropBackClickArea` for a simple example.

Technically this could be refactored and be made automatic... not there yet.

3. In `SceneWindow::dropEvent`, handle the case where the MIME data is a SEGMENT_BLOBAREA_ID.
(see the end of the function: )
```C++
if (data == SEGMENT_BACKCLICKAREA_ID)
{
  CommandDispatcher<> disp{context.commandStack};

  disp.submitCommand(new DropBlobArea{m_scene, pos, sz});
}
```

That's it ! 

## 6. Making it removable by the user

2. In `Deletion.hpp`, add a new undo-redo command that will remove the item.
It is very sumple : it just needs a line at the end: 

```C++
SCORE_COMMAND_DECL_T(SEGMent::RemoveObject<SEGMent::BlobAreaModel>)
```

(that is how creation would look also if it was refactored).

## 7. Allowing it to be moved

In `Properties.hpp` add property macros for the relevant properties, see e.g. 

```C++
PROPERTY_COMMAND_T(SEGMent, SetTextAreaPos, TextAreaModel::p_pos, "Set pos")
PROPERTY_COMMAND_T(SEGMent, SetTextAreaSize, TextAreaModel::p_size, "Set size")

SCORE_COMMAND_DECL_T(SEGMent::SetTextAreaPos)
SCORE_COMMAND_DECL_T(SEGMent::SetTextAreaSize)
```

just copy-pasting the macros from the other classes should be enough.

## 8. Further improvements

Some classes will complain that the new BlobArea case is not handled: for instance during export, etc.
The simplest is to do like the other classes and then modify to suit your needs.

In particular check out ObjectCopier.hpp and ObjectPaster.hpp (specifically ObjectPaster::pasteObjects)
for handling of copy-paste of objects of that type if it is necessary.
