// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "Application.hpp"
#include "SEGMent/ImageCache.hpp"
#include <ossia/detail/thread.hpp>

#include <QApplication>
#include <QItemSelectionModel>
#include <QPixmapCache>
#include <QSurfaceFormat>
#include <qnamespace.h>

static void setup_locale()
{
  // QLocale::setDefault(QLocale::C);
  // setlocale(LC_ALL, "C");
}

static void setup_app_flags()
{
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
  QCoreApplication::setAttribute(Qt::AA_CompressHighFrequencyEvents);
}

int main(int argc, char** argv)
{
#if defined(__APPLE__)
  qputenv("QT_MAC_WANTS_LAYER", "1");
#endif

  setup_locale();
  setup_app_flags();

  QPixmapCache::setCacheLimit(819200);
  int res;
  Application app(argc, argv);
  score::setQApplicationMetadata();

  qRegisterMetaType<SEGMent::CacheInstance>();
  qRegisterMetaTypeStreamOperators<SEGMent::CacheInstance>();
  qRegisterMetaType<std::unordered_map<QString,SEGMent::CacheInstance>>();
  qRegisterMetaTypeStreamOperators<std::unordered_map<QString,SEGMent::CacheInstance>>();
  {
    SEGMent::ImageCache cache;
    SEGMent::ImageCache::self = &cache;

    app.init();
    res = app.exec();
  }
  return res;
}

#if defined(Q_CC_MSVC)
#include <qt_windows.h>

#include <ShlObj.h>
#include <shellapi.h>
#include <stdio.h>
#include <windows.h>
static inline char* wideToMulti(int codePage, const wchar_t* aw)
{
  const int required
      = WideCharToMultiByte(codePage, 0, aw, -1, NULL, 0, NULL, NULL);
  char* result = new char[required];
  WideCharToMultiByte(codePage, 0, aw, -1, result, required, NULL, NULL);
  return result;
}

extern "C" int APIENTRY
WinMain(HINSTANCE, HINSTANCE, LPSTR /*cmdParamarg*/, int /* cmdShow */)
{
  int argc;
  wchar_t** argvW = CommandLineToArgvW(GetCommandLineW(), &argc);
  if (!argvW)
    return -1;
  char** argv = new char*[argc + 1];
  for (int i = 0; i < argc; ++i)
    argv[i] = wideToMulti(CP_ACP, argvW[i]);
  argv[argc] = nullptr;
  LocalFree(argvW);
  const int exitCode = main(argc, argv);
  for (int i = 0; i < argc && argv[i]; ++i)
    delete[] argv[i];
  delete[] argv;
  return exitCode;
}

#endif
