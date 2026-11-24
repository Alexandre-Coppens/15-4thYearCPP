#pragma once

#include "PrinterState.h"

namespace cups
{
    class IsPrintingState : public PrinterState
    {
        public:
            IsPrintingState(Printer *printer) noexcept;
            bool printingCompleted() const override;
            bool suspend() const override;
            bool isAvailable() const override;
        protected:
    };
}