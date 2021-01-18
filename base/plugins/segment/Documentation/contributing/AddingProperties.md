# Adding a new property with an existing appearance in the inspector

e.g. : adding a new small or large text field, a boolean checkbox, a new sound file chooser... 
something that does not necessitate new UI code.

Let's say we want to add a "bonus" checkbox to the Scene inspector.

1. In the SceneModel header (`Scene.hpp`), add the relevant functions / members / ...:

```C++
public:
  bool bonus() const MSVC_NOEXCEPT;
  void setBonus(bool v) MSVC_NOEXCEPT;
  void bonusChanged(bool v) W_SIGNAL(bonusChanged, v);
  
  // note: remove the comments as MSVC does not like comments in macros
  PROPERTY(
      bool, // this is the type
      bonus  // this is the name of the property, important for later
      READ bonus // this is the name of the getter
      WRITE setBonus  // this is the name of the setter
      NOTIFY bonusChanged  // this is the name of the notification signal
  )

private:
  bool m_bonus = false; // don't forget to initialize
```

Note: I just do a search & replace with the "Preserve case when replacing" most of the time

2. In the SceneModel implementation `Scene.cpp`, add the code for the functions:

```C++
bool SceneModel::bonus() const MSVC_NOEXCEPT
{
  return m_bonus;
}

void SceneModel::setBonus(bool v) MSVC_NOEXCEPT
{
  if (m_bonus != v)
  {
    m_bonus = v;
    bonusChanged(v);
  }
}
```

And for the serialization code (at the end) of the file, in these functions:

```C++
void DataStreamReader::read(const SEGMent::SceneModel& v);
void DataStreamWriter::write(SEGMent::SceneModel& v);

void JSONObjectReader::read(const SEGMent::SceneModel& v);
void JSONObjectWriter::write(SEGMent::SceneModel& v);
```


3. Add the implementation for the undo-redo commands: in `SEGMent/Commands/Properties.hpp`: 

It only needs two macros, they generate all the undo-redo code from the property: 

```C++
PROPERTY_COMMAND_T(
    SEGMent,
    SetSceneBonus,
    SceneModel::p_bonus,
    "Set bonus")
SCORE_COMMAND_DECL_T(SEGMent::SetSceneBonus)
```

5. In your IDE run CMake once as it looks for the "SCORE_COMMAND_DECL_T" lines and registers them 
(you can check iscore_addon_SEGMent_commands_files.hpp and iscore_addon_SEGMent_commands.hpp in the build 
dir if you're curious)

6. Add a line like the others in `SceneInspector`, in `Inspectors.hpp`.

For instance: 

```C++
setup_inspector(SceneModel::p_bonus{}, sc, doc, *lay, this);
```


# Adding a new property with a new appearance in the inspector

Let's say we want to add a new string field to the Scene and have it show up into the inspector.
Let's call it `VictoryText`.

1. Create a new small VictoryText struct which will contain the data. 
See `SEGMent/Model/SceneDataModels.hpp` for inspiration - likely it can be put there

2. In the SceneModel header (`Scene.hpp`), add the relevant functions / members / ...:

```C++
public:
  VictoryText victoryText() const MSVC_NOEXCEPT;
  void setVictoryText(const VictoryText& v) MSVC_NOEXCEPT;
  void victoryTextChanged(VictoryText v) W_SIGNAL(victoryTextChanged, v);
  
  // note: remove the comments as MSVC does not like comments in macros
  PROPERTY(
      VictoryText, // this is the type
      victoryText  // this is the name of the property, important for later
      READ victoryText // this is the name of the getter
      WRITE setVictoryText  // this is the name of the setter
      NOTIFY victoryTextChanged  // this is the name of the notification signal
  )

private:
  QString m_victoryText{};
```

Note: I just do a search & replace with the "Preserve case when replacing" most of the time

3. In the SceneModel implementation `Scene.cpp`, add the code for the functions:

```C++
VictoryText SceneModel::victoryText() const MSVC_NOEXCEPT
{
  return {m_victoryText};
}

void SceneModel::setVictoryText(const VictoryText& v) MSVC_NOEXCEPT
{
  if (m_victoryText != v)
  {
    m_victoryText = v;
    victoryTextChanged(v);
  }
}
```

And for the serialization code (at the end) of the file, in these functions:

```C++
void DataStreamReader::read(const SEGMent::SceneModel& v);
void DataStreamWriter::write(SEGMent::SceneModel& v);

void JSONObjectReader::read(const SEGMent::SceneModel& v);
void JSONObjectWriter::write(SEGMent::SceneModel& v);
```


4. Add the implementation for the undo-redo commands: in `SEGMent/Commands/Properties.hpp`: 

It only needs two macros, they generate all the undo-redo code from the property: 

```C++
PROPERTY_COMMAND_T(
    SEGMent,
    SetSceneVictoryText,
    SceneModel::p_victoryText,
    "Set victory text")
SCORE_COMMAND_DECL_T(SEGMent::SetSceneVictoryText)
```

5. In your IDE run CMake once as it looks for the "SCORE_COMMAND_DECL_T" lines and registers them 
(you can check iscore_addon_SEGMent_commands_files.hpp and iscore_addon_SEGMent_commands.hpp in the build 
dir if you're curious)

6. Add the implementation for the UI widget creation in `SEGMent/Inspector/InspectorSetup.hpp`

It just needs a function like this:

```C++
auto make(const VictoryText& cur)
{
  ...
}
```

Check for instance `auto make(const LongText& cur)` which is a good example.

7. Add a line like the others in `SceneInspector`, in `Inspectors.hpp`.

For instance: 

```C++
setup_inspector(SceneModel::p_victoryText{}, sc, doc, *lay, this);
```
