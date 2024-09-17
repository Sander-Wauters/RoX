#pragma once

// Abstraction over window events.
// Used for when classes need to execute code when a certain window event is fired.
class IWindowObserver {
    public:   
        virtual void OnActivated() = 0;
        virtual void OnDeactivated() = 0;
        virtual void OnSuspending() = 0;
        virtual void OnResuming() = 0;
        virtual void OnWindowMoved() = 0;
        virtual void OnDisplayChanged() = 0;
        virtual void OnWindowSizeChanged(int width, int height) = 0;
};
