include(../test-include.pri)
QT += quick-private
SOURCES += tst_mousefilterstest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

OTHER_FILES += \
    FilterTextInput.qml \
    PressAndHoldAcceptedEatsClick.qml \
    PressAndHoldAndClicked.qml \
    FilterInverseTextInput.qml \
    FilterInverseTextInputOSKExcluded.qml \
    AttachedToItem.qml \
    InverseAttachedToItem.qml \
    MouseFilterAttachedToNonItem.qml \
    InverseMouseFilterAttachedToNonItem.qml \
    ForwardedMouseEvents.qml \
    ForwardToMouseArea.qml \
    FilterTextInputAfter.qml \
    FilterInverseTextInputAfter.qml \
    ForwardedMouseEventsStopped.qml \
    ForwardedEventsToTargetBlocked.qml \
    ForwardToInverseMouseArea.qml \
    FilterFlickable.qml \
    FilterFlickableAfter.qml \
    Hover.qml
