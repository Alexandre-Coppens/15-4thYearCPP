#pragma once

#include "PrinterState.h"

namespace cups
{
    class OffState : public PrinterState
    {
        public:
            OffState(Printer *printer) noexcept;
            
            bool start() const override;
            
            bool isAvailable() const override;
        protected:
            Job *job;
    };
}
