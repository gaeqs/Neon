//
// Created by gaeqs on 27/08/24.
//

#ifndef ABSTRACTCOROUTINE_H
#define ABSTRACTCOROUTINE_H

/**
* Base declaration of a coroutine.
* <p>
* If you want an implemented Coroutine struct instead,
* use neon::Coroutine.
*/
struct AbstractCoroutine {
    /**
     * Destroys the coroutine.
     */
    virtual ~AbstractCoroutine() = default;

    /**
     * Returns whether this coroutine is ready to be launched.
     * <p>
     * A ready coroutine must not have finished, must not be waiting
     * and must be valid.
     *
     * @return whether this coroutine is ready to be launched.
     */
    [[nodiscard]] virtual bool isReady() const = 0;

    /**
     *
     * @return whether this coroutine has finished all its job.
     */
    [[nodiscard]] virtual bool isDone() const = 0;

    /**
     * @return whether this coroutine is valid.
     */
    [[nodiscard]] virtual bool isValid() const = 0;

    /**
     * Executes this coroutine until the next breaking point.
     */
    virtual void launch() const = 0;
};

#endif //ABSTRACTCOROUTINE_H
