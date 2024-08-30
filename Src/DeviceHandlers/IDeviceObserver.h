#pragma once

class IDeviceObserver {
    public:
        virtual void OnDeviceLost() = 0;
        virtual void OnDeviceRestored() = 0;
};
