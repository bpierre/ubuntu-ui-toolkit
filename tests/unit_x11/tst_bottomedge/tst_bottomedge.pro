include(../test-include.pri)
QT += core-private qml-private quick-private gui-private UbuntuGestures

SOURCES += \
    tst_bottomedge.cpp

DISTFILES += \
    Defaults.qml \
    DifferentSizes.qml \
    LastItem.qml \
    BottomEdgeInItem.qml \
    ShorterBottomEdge.qml \
    AlternateRegionContent.qml \
    AddCustomRegionUsingRegionsProperty.qml \
    AddCustomRegionUsingDataProperty.qml \
    AddCustomRegionOnCompleted.qml \
    AddCustomRegionOwnedByOtherBottomEdge.qml \
    ClearCustomRegions.qml \
    AlternateDefaultRegionContent.qml \
    OverlappingRegions.qml \
    AutoCollapseInPageHeader.qml \
    AutoCollapseInPageWithPageHeader.qml
