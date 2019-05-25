#include <SEGMent/Panel/Minimap.hpp>
#include <SEGMent/Model/ProcessModel.hpp>
#include <SEGMent/Document.hpp>
#include <core/document/Document.hpp>
#include <core/document/DocumentModel.hpp>
#include <QVBoxLayout>
namespace SEGMent
{
Minimap::Minimap(const score::GUIApplicationContext &ctx)
  : score::PanelDelegate{ctx}
  , m_widget{new QWidget}
  , m_scene{new QGraphicsScene{m_widget}}
  , m_view{new QGraphicsView{m_widget}}
{
  m_widget->setLayout(new QVBoxLayout);
}

QWidget* Minimap::widget()
{
  return m_widget;
}

const score::PanelStatus& Minimap::defaultPanelStatus() const
{
  static const score::PanelStatus status{
    true, Qt::RightDockWidgetArea, 100,
        QObject::tr("Navigation"), QObject::tr("Ctrl+N")};

  return status;
}

void Minimap::on_modelChanged(
    score::MaybeDocument oldm, score::MaybeDocument newm)
{
  delete m_scene;
  m_scene = new QGraphicsScene{this};
  m_view->setScene(m_scene);

  if(newm)
  {
    if(auto d = dynamic_cast<DocumentModel*>(&newm->document.model().modelDelegate()))
    {
      for(auto& scene : d->process().scenes)
      {
        auto pix = m_scene->addPixmap(scene.image().path);
        pix->setPos(scene.rect().topLeft());
      }
    }
  }
}

}
