#ifndef QPROGRESSINDICATOR_H
#define QPROGRESSINDICATOR_H

#include <QWidget>
#include <QColor>
#include <wobjectdefs.h>
/*!
    \class QProgressIndicator
    \brief The QProgressIndicator class lets an application display a progress indicator to show that a lengthy task is under way.

    Progress indicators are indeterminate and do nothing more than spin to show that the application is busy.
    \sa QProgressBar
*/
class Q_DECL_EXPORT QProgressIndicator : public QWidget
{
    W_OBJECT(QProgressIndicator)
public:
    QProgressIndicator(QWidget* parent = nullptr);

    /*! Returns the delay between animation steps.
        \return The number of milliseconds between animation steps. By default, the animation delay is set to 40 milliseconds.
        \sa setAnimationDelay
     */
    int animationDelay() const { return m_delay; }

    /*! Returns a Boolean value indicating whether the component is currently animated.
        \return Animation state.
        \sa startAnimation stopAnimation
     */
    bool isAnimated () const;

    /*! Returns a Boolean value indicating whether the receiver shows itself even when it is not animating.
        \return Return true if the progress indicator shows itself even when it is not animating. By default, it returns false.
        \sa setDisplayedWhenStopped
     */
    bool isDisplayedWhenStopped() const;

    /*! Returns the color of the component.
        \sa setColor
      */
    const QColor & color() const { return m_color; }

    QSize sizeHint() const override;
    int heightForWidth(int w) const override;
public:
    /*! Starts the spin animation.
        \sa stopAnimation isAnimated
     */
    void startAnimation(); W_SLOT(startAnimation)

    /*! Stops the spin animation.
        \sa startAnimation isAnimated
     */
    void stopAnimation(); W_SLOT(stopAnimation)

    /*! Sets the delay between animation steps.
        Setting the \a delay to a value larger than 40 slows the animation, while setting the \a delay to a smaller value speeds it up.
        \param delay The delay, in milliseconds.
        \sa animationDelay
     */
    void setAnimationDelay(int delay); W_SLOT(setAnimationDelay)

    /*! Sets whether the component hides itself when it is not animating.
       \param state The animation state. Set false to hide the progress indicator when it is not animating; otherwise true.
       \sa isDisplayedWhenStopped
     */
    void setDisplayedWhenStopped(bool state); W_SLOT(setDisplayedWhenStopped)

    /*! Sets the color of the components to the given color.
        \sa color
     */
    void setColor(const QColor & color); W_SLOT(setColor);

    W_PROPERTY(int, delay READ animationDelay WRITE setAnimationDelay)
    W_PROPERTY(bool, displayedWhenStopped READ isDisplayedWhenStopped WRITE setDisplayedWhenStopped)
    W_PROPERTY(QColor, color READ color WRITE setColor)
protected:
    void timerEvent(QTimerEvent * event) override;
    void paintEvent(QPaintEvent * event) override;
private:
    int m_angle;
    int m_timerId;
    int m_delay;
    QColor m_color;
    bool m_displayedWhenStopped;
};

#endif // QPROGRESSINDICATOR_H
