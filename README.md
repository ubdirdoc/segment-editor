segment-editor
=======

An editor for games made with the SEGMent engine.


Building a release on windows (example):

    cmake ../segment-editor -DSEGMENT_RELEASE=1 -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=c:/Qt/5.12.3/msvc2017_64 -DBOOST_ROOT=c:/dev/boost_1_69_0 -GNinja -DCMAKE_MAKE_PROGRAM=c:/dev/ninja.exe
    ninja
    ninja package
