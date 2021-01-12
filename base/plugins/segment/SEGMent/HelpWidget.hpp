#pragma once

#include <QSplitter>
#include <QTextBrowser>
#include <QHelpEngine>
#include <score_git_info.hpp>


/**
 * @brief Widget that displays the Qt help file.
 */
class HelpBrowser final : public QTextBrowser
{
public:
  HelpBrowser(QHelpEngine* helpEngine, QWidget* parent = 0)
      : QTextBrowser{parent}, m_help_engine{helpEngine}
  {
    setMinimumWidth(1024);
    setMinimumHeight(768);
  }

  QVariant loadResource(int type, const QUrl& name) override
  {
    if (name.scheme() == "qthelp")
      return QVariant(m_help_engine->fileData(name));
    else
      return QTextBrowser::loadResource(type, name);
  }

private:
  QHelpEngine* m_help_engine{};
};

/**
 * @brief Load and setup the help browser.
 */
class HelpWidget : public QSplitter
{
public:
  HelpWidget(QString help_path, QWidget* parent = nullptr)
      : QSplitter{Qt::Horizontal, parent}
      , m_help_engine{help_path, this}
      , m_help_browser{&m_help_engine, this}
  {
    auto content = m_help_engine.contentWidget();
    content->setMinimumWidth(300);
    content->setMaximumWidth(300);

    m_help_browser.setSource(QUrl(
        "qthelp://org.sphinx.segmenteditor."
        + QString::number(SCORE_VERSION_MAJOR) + "."
        + QString::number(SCORE_VERSION_MINOR) + "/doc/index.html"));
    connect(
        m_help_engine.contentWidget(),
        &QHelpContentWidget::linkActivated,
        &m_help_browser,
        [&] (const auto& url) { m_help_browser.setSource(url); });
    connect(
        m_help_engine.indexWidget(),
        &QHelpIndexWidget::linkActivated,
        &m_help_browser,
        [&](const auto& url) { m_help_browser.setSource(url); });

    insertWidget(0, content);
    insertWidget(1, &m_help_browser);
  }

private:
  QHelpEngine m_help_engine;
  HelpBrowser m_help_browser;
};
