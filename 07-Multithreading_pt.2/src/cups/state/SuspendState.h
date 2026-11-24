#pragma once

#include "../Printer.h"
#include "PrinterState.h"

namespace cups
{
    class SuspendState : public PrinterState
    {
        public:
            SuspendState(Printer *printer) noexcept;
            
            bool resume() const override;
            bool cancel() const override;
            
            bool isAvailable() const override;
    };
}
