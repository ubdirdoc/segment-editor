#include "ProcessModel.hpp"

#include <SEGMent/Model/BackClickArea.hpp>
#include <SEGMent/Model/ClickArea.hpp>
#include <SEGMent/Model/GifObject.hpp>
#include <SEGMent/Model/Scene.hpp>
#include <SEGMent/Model/SimpleObject.hpp>
#include <SEGMent/Model/Sound.hpp>
#include <SEGMent/Model/TextArea.hpp>
#include <SEGMent/Model/Transition.hpp>
#include <QApplication>
namespace SEGMent
{

ProcessModel::ProcessModel(
    Id<SEGMent::ProcessModel> id,
    QObject* parent)
    : score::Entity<ProcessModel>{id, "SEGMentProcess", parent}
{
  metadata().setInstanceName(*this);

#if !defined(NDEBUG)
  {
    auto scene1 = new SceneModel{Id<SceneModel>{0}, this};
    scene1->setImage(Image{"Scenes/bobineZoom.png"});
    scene1->setRect({2300, 2300, 640, 480});
    scenes.add(scene1);

    auto obj1 = new ImageModel{Id<ImageModel>{0}, scene1};
    obj1->setImage(Image{"Objects/enveloppe.png"});
    obj1->setPos({0.5, 0.7});
    obj1->setSize({0.3, 0.3});
    scene1->objects.add(obj1);

    auto scene2 = new SceneModel{Id<SceneModel>{1}, this};
    scene2->setImage(Image{"Scenes/TableauDia1.png"});
    scene2->setRect({2800, 2800, 640, 480});
    scenes.add(scene2);

    auto obj2 = new ImageModel{Id<ImageModel>{0}, scene2};
    obj2->setImage(Image{"Objects/clef.png"});
    obj2->setPos({0.5, 0.7});
    obj2->setSize({0.3, 0.3});
    scene2->objects.add(obj2);

    auto obj3 = new ClickAreaModel{Id<ClickAreaModel>{0}, scene2};
    obj3->setPos({0.2, 0.2});
    obj3->setSize({0.3, 0.3 * 640 / 480});
    scene2->clickAreas.add(obj3);

    ObjectToScene t{*obj1, *scene2, 6, 4};
    auto trans = new TransitionModel{t, Id<TransitionModel>{0}, this};
    transitions.add(trans);
  }
#endif
}

ProcessModel::~ProcessModel()
{
  transitions.clear();
  scenes.clear();
  identified_object_destroying(this);
}

}

template <>
void DataStreamReader::read(const SEGMent::ProcessModel& proc)
{
  ossia::for_each_in_tuple(
      std::tie(proc.scenes, proc.transitions), [&](const auto& map) {
        m_stream << (int32_t)map.size();
        for (const auto& obj : map)
        {
          readFrom(obj);
        }
      });
  insertDelimiter();
}

template <>
void DataStreamWriter::write(SEGMent::ProcessModel& proc)
{
  ossia::for_each_in_tuple(
      std::tie(proc.scenes, proc.transitions), [&](auto& map) {
        using entity_type =
            typename std::remove_reference_t<decltype(map)>::value_type;
        int32_t sz;
        m_stream >> sz;
        for (; sz-- > 0;)
        {
          auto obj = new entity_type{*this, &proc};
          map.add(obj);
        }
      });
  checkDelimiter();
}

template <>
void JSONObjectReader::read(const SEGMent::ProcessModel& proc)
{
  obj["Scenes"] = toJsonArray(proc.scenes);
  obj["Transitions"] = toJsonArray(proc.transitions);
}

template <>
void JSONObjectWriter::write(SEGMent::ProcessModel& proc)
{
  {
    const auto& objs = obj["Scenes"].toArray();
    for (const auto& json_vref : objs)
    {
      auto obj = new SEGMent::SceneModel{
          JSONObject::Deserializer{json_vref.toObject()}, &proc};
      proc.scenes.add(obj);
    }
  }
  {
    const auto& objs = obj["Transitions"].toArray();
    for (const auto& json_vref : objs)
    {
      auto obj = new SEGMent::TransitionModel{
          JSONObject::Deserializer{json_vref.toObject()}, &proc};
      proc.transitions.add(obj);
    }
  }
}
